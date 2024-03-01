#include "../tomb4/pch.h"
#include <bx/readerwriter.h>
#include "platform.h"
#include "winmain.h"
#include "drawroom.h"
#include "bgfx.h"
#include "function_stubs.h"

#ifdef USE_BGFX

bgfx::ProgramHandle m_outputVTLProgram = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle m_outputVTLAlphaProgram = BGFX_INVALID_HANDLE;
bgfx::UniformHandle s_texColor;
bgfx::VertexLayout ms_outputBucketVertexLayout;

size_t current_sort_vertex_buffer_idx = 0;
size_t current_sort_vertex_buffer_offset = 0;

GFXTLBUMPVERTEX *sort_buffer_vertex_buffers_first = nullptr;
GFXTLBUMPVERTEX *sort_buffer_vertex_buffers_second = nullptr;

GFXTLBUMPVERTEX *sort_buffer_vertex_buffers_frontbuffer = nullptr;
GFXTLBUMPVERTEX *sort_buffer_vertex_buffers_backbuffer = nullptr;

bgfx::DynamicVertexBufferHandle sort_buffer_vertex_handle[MAX_SORT_VERTEX_BUFFERS];

BGFXSortDrawCommand sort_buffer_commands[MAX_SORT_VERTEX_BUFFERS];

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

    SetupOutputBucketVertexLayout();
    for (int i = 0; i < MAX_BUCKETS; i++)
    {
        TEXTUREBUCKET* bucket = &Bucket[i];
        bucket->handle = bgfx::createDynamicVertexBuffer(BUCKET_VERT_COUNT, ms_outputBucketVertexLayout);
    }

    for (int i = 0; i < MAX_SORT_VERTEX_BUFFERS; i++)
    {
        sort_buffer_vertex_handle[i] = bgfx::createDynamicVertexBuffer(MAX_VERTS_PER_SORT_BUFFER, ms_outputBucketVertexLayout);
    }

    m_outputVTLProgram = loadProgram("vs_vtl", "fs_vtl");
    m_outputVTLAlphaProgram = loadProgram("vs_vtl_alpha", "fs_vtl_alpha");

    sort_buffer_vertex_buffers_first = (GFXTLBUMPVERTEX * )SYSTEM_MALLOC(SORT_BUFFER_VERT_COUNT * sizeof(GFXTLBUMPVERTEX));
    sort_buffer_vertex_buffers_second = (GFXTLBUMPVERTEX*)SYSTEM_MALLOC(SORT_BUFFER_VERT_COUNT * sizeof(GFXTLBUMPVERTEX));

    sort_buffer_vertex_buffers_frontbuffer = sort_buffer_vertex_buffers_first;
    sort_buffer_vertex_buffers_backbuffer = sort_buffer_vertex_buffers_second;
}

void ShutdownBGFX() {
    SYSTEM_FREE(sort_buffer_vertex_buffers_first);
    SYSTEM_FREE(sort_buffer_vertex_buffers_second);
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
        if (sort_buffer_vertex_buffers_frontbuffer[i].rhw != 1.0f && sort_buffer_vertex_buffers_frontbuffer[i].rhw != 0.0f)
        {
            float w = 1.0f / sort_buffer_vertex_buffers_frontbuffer[i].rhw;
            sort_buffer_vertex_buffers_frontbuffer[i].sx *= w;
            sort_buffer_vertex_buffers_frontbuffer[i].sy *= w;
            sort_buffer_vertex_buffers_frontbuffer[i].sz *= w;
            sort_buffer_vertex_buffers_frontbuffer[i].rhw = w;
        }
    }

    for (int i = current_sort_vertex_buffer_idx-1; i >= 0; i--) {
        uint64_t state = 0
            | BGFX_STATE_WRITE_RGB
            | BGFX_STATE_DEPTH_TEST_LESS
            | BGFX_STATE_MSAA
            | BGFX_STATE_BLEND_ALPHA
            | UINT64_C(0);


        if (sort_buffer_commands[i].texture.idx < 0xffff) {
            bgfx::setState(state);
            bgfx::setVertexBuffer(0, sort_buffer_vertex_handle[i], 0, sort_buffer_commands[i].count);
            bgfx::setTexture(0, s_texColor, sort_buffer_commands[i].texture);
            bgfx::submit(0, m_outputVTLAlphaProgram);
        }
    }
}

void StartBGFXFrame() {
    current_sort_vertex_buffer_idx = 0;
    current_sort_vertex_buffer_offset = 0;
}

void EndBGFXFrame() {
    GFXTLBUMPVERTEX* sort_buffer_vertex_buffers_backup = sort_buffer_vertex_buffers_frontbuffer;
    sort_buffer_vertex_buffers_frontbuffer = sort_buffer_vertex_buffers_backbuffer;
    sort_buffer_vertex_buffers_backbuffer = sort_buffer_vertex_buffers_backup;
}

#endif