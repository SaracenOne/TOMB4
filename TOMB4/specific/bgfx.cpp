#include "../tomb4/pch.h"
#include <bx/readerwriter.h>
#include "platform.h"
#include "winmain.h"
#include "drawroom.h"

#ifdef USE_BGFX

bgfx::ProgramHandle m_outputProgram = BGFX_INVALID_HANDLE;

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

    bgfx::ShaderHandle handle = bgfx::createShader(loadMem(filePath));
    bgfx::setName(handle, _name);

    return handle;
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
    init.resolution.width = WINDOW_DEFAULT_WIDTH;
    init.resolution.height = WINDOW_DEFAULT_HEIGHT;
    init.resolution.reset = BGFX_RESET_VSYNC;
    if (!bgfx::init(init))
    {
        platform_fatal_error("Could not create BGFX API context!");
    }

    bgfx::setDebug(BGFX_DEBUG_NONE);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

    SetupBuckets();

    m_outputProgram = loadProgram("vs_generic", "fs_generic");
}

void SetupBGFXOutputPolyList() {
    float view[16];
    float ortho[16];

    const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
    const bx::Vec3 eye = { 0.0f, 0.0f, -30.0f};

    bx::mtxLookAt(view, eye, at);

    bx::mtxOrtho(ortho, 0.0f, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, 0.0f, 0.00001f, 10000.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(0, view, ortho);

    bgfx::setViewRect(0, 0, 0, uint16_t(WINDOW_DEFAULT_WIDTH), uint16_t(WINDOW_DEFAULT_HEIGHT));

}

void StartBGFXFrame() {
}

void EndBGFXFrame() {
    bgfx::frame();
}

#endif