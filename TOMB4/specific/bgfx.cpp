#include "../tomb4/pch.h"
#include <bx/readerwriter.h>
#include "platform.h"
#include "winmain.h"
#include "drawroom.h"
#include "bgfx.h"
#include "function_stubs.h"

#ifdef USE_BGFX

bgfx::ProgramHandle m_outputVTLTexProgram = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle m_outputVTLTexAlphaClippedProgram = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle m_outputVTLTexAlphaBlendedProgram = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle m_outputVTLAlphaProgram = BGFX_INVALID_HANDLE;

bgfx::UniformHandle s_texColor;
bgfx::VertexLayout ms_outputBucketVertexLayout;

size_t current_sort_vertex_buffer_idx = 0;
size_t current_sort_vertex_buffer_offset = 0;

extern GFXTLBUMPVERTEX *sort_buffer_vertex_buffer = nullptr;
const bgfx::Memory *sort_buffer_vertex_buffers_ref = nullptr;

bgfx::DynamicVertexBufferHandle sort_buffer_vertex_handle;

BGFXSortDrawCommand sort_buffer_commands[MAX_SORT_DRAW_COMMANDS];

void SetupOutputBucketVertexLayout()
{
    ms_outputBucketVertexLayout
        .begin()
        .add(bgfx::Attrib::Position, 4, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .add(bgfx::Attrib::Color1, 4, bgfx::AttribType::Uint8, true)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord1, 2, bgfx::AttribType::Float)
        .end();
}

static const bgfx::Memory* loadMem(const char* _filePath)
{
    FILE* fin = fopen(_filePath, "rb");

    if (fin == nullptr)
    {
        printf("%s not found\n", _filePath);
        fflush(stdout);
        return NULL;
    }

    long file_size;

    if (fseek(fin, 0, SEEK_END) != 0) {
    }

    file_size = ftell(fin);
    if (file_size == -1) {
    }

    uint32_t size = file_size;
    const bgfx::Memory* mem = bgfx::alloc(size + 1);

    rewind(fin);

    fread(mem->data, 1, size, fin);

    fclose(fin);

    mem->data[mem->size - 1] = '\0';

    return mem;
}

bgfx::ShaderHandle loadShader(const char* _name) {
    char filePath[512];

    const char* shaderPath = "???";

    switch (bgfx::getRendererType()) {
    case bgfx::RendererType::Noop:
    case bgfx::RendererType::Direct3D11:
    case bgfx::RendererType::Direct3D12: shaderPath = "shaders/dx11/";  break;
    case bgfx::RendererType::Agc:
    case bgfx::RendererType::Gnm:        shaderPath = "shaders/pssl/";  break;
    case bgfx::RendererType::Metal:      shaderPath = "shaders/metal/"; break;
    case bgfx::RendererType::Nvn:        shaderPath = "shaders/nvn/";   break;
    case bgfx::RendererType::OpenGL:     shaderPath = "shaders/glsl/";  break;
    case bgfx::RendererType::OpenGLES:   shaderPath = "shaders/essl/";  break;
    case bgfx::RendererType::Vulkan:     shaderPath = "shaders/spirv/"; break;

    case bgfx::RendererType::Count:
        BX_ASSERT(false, "You should not be here!");
        break;
    }

    bx::strCopy(filePath, BX_COUNTOF(filePath), shaderPath);
    bx::strCat(filePath, BX_COUNTOF(filePath), _name);
    bx::strCat(filePath, BX_COUNTOF(filePath), ".bin");

    const bgfx::Memory *shader_memory = loadMem(filePath);
    if (shader_memory) {
        bgfx::ShaderHandle handle = bgfx::createShader(shader_memory);
        bgfx::setName(handle, _name);

        return handle;
    }
    else
    {
        platform_fatal_error("Failed to load shader at path: '%s'", filePath);
    }

    return BGFX_INVALID_HANDLE;
}

bgfx::ProgramHandle loadProgram(const char* _vsName, const char* _fsName)
{
    bgfx::ShaderHandle vsh = loadShader(_vsName);
    bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
    if (NULL != _fsName)
    {
        fsh = loadShader(_fsName);
    }

    return bgfx::createProgram(vsh, fsh, true);
}

// The thread this gets called from becomes the API thread.
void InitializeBGFX() {
    bgfx::renderFrame();

    bgfx::Init init;
    init.type = bgfx::RendererType::Vulkan;
    init.vendorId = BGFX_PCI_ID_NONE;
    init.platformData.nwh = SDLGetNativeWindowHandle(sdl_window);
    init.platformData.ndt = SDLGetNativeDisplayHandle(sdl_window);
    init.resolution.width = App.dx.dwRenderWidth;
    init.resolution.height = App.dx.dwRenderHeight;
    init.resolution.reset = BGFX_RESET_VSYNC;
    if (!bgfx::init(init))
    {
        platform_fatal_error("Could not create BGFX API context!");
    }

    bgfx::setDebug(0);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
    bgfx::setViewMode(0, bgfx::ViewMode::Sequential);

    SetupOutputBucketVertexLayout();
    for (int i = 0; i < MAX_BUCKETS; i++)
    {
        TEXTUREBUCKET* bucket = &Bucket[i];
        bucket->handle = bgfx::createDynamicVertexBuffer(BUCKET_VERT_COUNT, ms_outputBucketVertexLayout);
    }

    sort_buffer_vertex_buffer = (GFXTLBUMPVERTEX*) SYSTEM_MALLOC(SORT_BUFFER_VERT_COUNT * sizeof(GFXTLBUMPVERTEX));
    sort_buffer_vertex_handle = bgfx::createDynamicVertexBuffer(SORT_BUFFER_VERT_COUNT, ms_outputBucketVertexLayout);

    m_outputVTLTexProgram = loadProgram("vs_vtl_tex", "fs_vtl_tex");
    if (App.Filtering) {
        m_outputVTLTexAlphaClippedProgram = loadProgram("vs_vtl_tex_alpha_clipped_filter", "fs_vtl_tex_alpha_clipped_filter");
    } else {
        m_outputVTLTexAlphaClippedProgram = loadProgram("vs_vtl_tex_alpha_clipped_point", "fs_vtl_tex_alpha_clipped_point");
    }
    m_outputVTLTexAlphaBlendedProgram = loadProgram("vs_vtl_tex_alpha_blended", "fs_vtl_tex_alpha_blended");
    m_outputVTLAlphaProgram = loadProgram("vs_vtl_alpha", "fs_vtl_alpha");
}

void ShutdownBGFX() {
    SYSTEM_FREE(sort_buffer_vertex_buffer);
}

void SetupBGFXOutputPolyList() {
    float view[16];
    float ortho[16];

    const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
    const bx::Vec3 eye = { 0.0f, 0.0f, -1.0f };

    bx::mtxLookAt(view, eye, at);

    bx::mtxOrtho(ortho, 0.0f, App.dx.dwRenderWidth, App.dx.dwRenderHeight, 0.0f, 0.0f, 2.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(0, view, ortho);

    bgfx::setViewRect(0, 0, 0, uint16_t(App.dx.dwRenderWidth), uint16_t(App.dx.dwRenderHeight));

}

void RenderBGFXDrawLists() {
    // Compatibility for the old D3DTL XYZRWH polygon buffer
    for (int i = 0; i < current_sort_vertex_buffer_offset; i++) {
        if (sort_buffer_vertex_buffer[i].rhw != 1.0f && sort_buffer_vertex_buffer[i].rhw != 0.0f)
        {
            float w = 1.0f / sort_buffer_vertex_buffer[i].rhw;
            sort_buffer_vertex_buffer[i].sx *= w;
            sort_buffer_vertex_buffer[i].sy *= w;
            sort_buffer_vertex_buffer[i].sz *= w;
            sort_buffer_vertex_buffer[i].rhw = w;
        }
    }

    bgfx::update(sort_buffer_vertex_handle, 0, sort_buffer_vertex_buffers_ref);

    for (int i = 0; i < current_sort_vertex_buffer_idx; i++) {
    //for (int i = current_sort_vertex_buffer_idx-1; i >= 0; i--) {
        uint64_t state = UINT64_C(0);
        bool is_blended = false;

        switch (sort_buffer_commands[i].draw_type) {
            case 0: {
                is_blended = false;
                state = 0
                    | BGFX_STATE_WRITE_RGB
                    | BGFX_STATE_WRITE_A
                    | BGFX_STATE_WRITE_Z
                    | BGFX_STATE_DEPTH_TEST_LESS
                    | UINT64_C(0);
                break;
            }
            case 1: {
                is_blended = false;
                state = 0
                    | BGFX_STATE_WRITE_RGB
                    | BGFX_STATE_WRITE_A
                    | BGFX_STATE_WRITE_Z
                    | BGFX_STATE_DEPTH_TEST_LESS
                    | BGFX_STATE_BLEND_ALPHA
                    | UINT64_C(0);
                break;
            }
            case 2: {
                is_blended = true;
                state = 0
                    | BGFX_STATE_WRITE_RGB
                    | BGFX_STATE_DEPTH_TEST_LEQUAL
                    | BGFX_STATE_BLEND_ADD
                    | UINT64_C(0);
                break;
            }
            case 3: {
                is_blended = true;
                state = 0
                    | BGFX_STATE_WRITE_RGB
                    | BGFX_STATE_DEPTH_TEST_LESS
                    | BGFX_STATE_BLEND_ALPHA
                    | UINT64_C(0);
                break;
            }
            case 4: {
                is_blended = false;
                state = 0
                    | BGFX_STATE_WRITE_RGB
                    | BGFX_STATE_BLEND_ALPHA
                    | UINT64_C(0);
                break;
            }
            case 5: {
                is_blended = true;
                state = 0
                    | BGFX_STATE_WRITE_RGB
                    | BGFX_STATE_DEPTH_TEST_LESS
                    | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ZERO, BGFX_STATE_BLEND_INV_SRC_COLOR)
                    | UINT64_C(0);
                break;
            }
            case 6: {
                is_blended = true;
                state = 0
                    | BGFX_STATE_WRITE_RGB
                    | BGFX_STATE_WRITE_A
                    | BGFX_STATE_DEPTH_TEST_LESS
                    | BGFX_STATE_BLEND_ADD
                    | BGFX_STATE_PT_LINES
                    | UINT64_C(0);
                break;
            }
            case 7: {
                is_blended = true;
                state = 0
                    | BGFX_STATE_WRITE_RGB
                    | BGFX_STATE_WRITE_A
                    | BGFX_STATE_DEPTH_TEST_LESS
                    | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
                    | UINT64_C(0);
                break;
            }
            default: {
                state = 0
                    | BGFX_STATE_WRITE_RGB
                    | BGFX_STATE_DEPTH_TEST_LESS
                    | BGFX_STATE_BLEND_ALPHA
                    | UINT64_C(0);
                break;
            }
        }

        bgfx::setState(state);
        bgfx::setVertexBuffer(0, sort_buffer_vertex_handle, sort_buffer_commands[i].offset, sort_buffer_commands[i].count);

        if (sort_buffer_commands[i].texture.idx != 0xffff) {
            bgfx::setTexture(0, s_texColor, sort_buffer_commands[i].texture);
            if (is_blended) {
                bgfx::submit(0, m_outputVTLTexAlphaBlendedProgram);
            } else {
                bgfx::submit(0, m_outputVTLTexAlphaClippedProgram);
            }
        } else {
            bgfx::submit(0, m_outputVTLAlphaProgram);
        }
    }
}

void StartBGFXFrame() {
    current_sort_vertex_buffer_idx = 0;
    current_sort_vertex_buffer_offset = 0;

    sort_buffer_vertex_buffers_ref = bgfx::makeRef(sort_buffer_vertex_buffer, SORT_BUFFER_VERT_COUNT * sizeof(GFXTLBUMPVERTEX));
    sort_buffer_vertex_buffer = (GFXTLBUMPVERTEX*)sort_buffer_vertex_buffers_ref->data;
}

void EndBGFXFrame() {
}

#endif