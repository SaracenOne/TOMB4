#include "../tomb4/pch.h"
#include "platform.h"
#include "winmain.h"

#ifdef USE_BGFX

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
}

void StartBGFXFrame() {
}

void EndBGFXFrame() {
}

#endif