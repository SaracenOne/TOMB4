#include "../tomb4/pch.h"
#include "winmain.h"
#include "function_stubs.h"
#include "cmdline.h"
#include "registry.h"
#include "dxshell.h"
#include "../game/text.h"
#include "lighting.h"
#include "function_table.h"
#include "d3dmatrix.h"
#include "3dmath.h"
#include "audio.h"
#include "output.h"
#include "file.h"
#include "../game/gameflow.h"
#include "dxsound.h"
#include "gamemain.h"
#include "fmv.h"
#include "audio.h"
#include "platform.h"

#include "../tomb4/mod_config.h"
#include "../tomb4/tomb4.h"

static COMMANDLINES commandlines[] =
{
	{ "SETUP", 0, &CLSetup },
	{ "NOFMV", 0, &CLNoFMV },
	{ "PATH", 1, &CLPath }
};

WINAPP App;
char* cutseqpakPtr;
long resChangeCounter;
bool appIsUnfocused = false;

#ifdef USE_SDL

#include <SDL.h>
#ifndef USE_BGFX
#include <SDL_syswm.h>
#endif

SDL_Window* sdl_window = NULL;
#ifdef _WIN32
WNDPROC originalWndProc;
#endif

#ifdef USE_BGFX
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#	if ENTRY_CONFIG_USE_WAYLAND
#		include <wayland-egl.h>
#	endif
#elif BX_PLATFORM_WINDOWS
#	define SDL_MAIN_HANDLED
#endif

#include <bx/os.h>

BX_PRAGMA_DIAGNOSTIC_PUSH()
BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG("-Wextern-c-compat")
#include <SDL_syswm.h>
BX_PRAGMA_DIAGNOSTIC_POP()

#endif
#include "input.h"

void SDLProcessCommandLine(int argc, char* argv[])
{
	COMMANDLINES* command;
	char* pCommand;
	char* p;
	char* last;
	ulong l;
	long num;
	char parameter[PARAMETER_MAX_LENGTH];

	Log(2, "SDLProcessCommandLine");

	num = sizeof(commandlines) / sizeof(commandlines[0]);

	for (int i = 0; i < num; i++)
	{
		command = &commandlines[i];
		command->code((char*)"_INIT");
	}

	for (int cur_arg = 1; cur_arg < argc; cur_arg++) {
		for (int i = 0; (ulong)i < strlen(argv[cur_arg]); i++)
		{
			if (argv[cur_arg][i] == '=')
			{
				break;
			}

			if (toupper(argv[cur_arg][i]))
				argv[cur_arg][i] = toupper(argv[cur_arg][i]);
		}
	}

	for (int i = 0; i < num; i++)
	{
		command = &commandlines[i];
		memset(parameter, 0, sizeof(parameter));

		pCommand = NULL;
		for (int cur_arg = 1; cur_arg < argc; cur_arg++) {
			pCommand = strstr(argv[cur_arg], command->command);
			if (pCommand != NULL) {
				break;
			}
		}

		if (pCommand)
		{
			if (command->needs_parameter)
			{
				p = 0;
				l = strlen(pCommand);

				for (int j = 0; (ulong)j < l; j++, pCommand++)
				{
					if (*pCommand != '=')
						continue;

					p = pCommand + 1;
					l = strlen(p);

					for (j = 0; (ulong)j < l; j++, p++)
					{
						if (*p != ' ')
							break;
					}

					last = p;
					l = strlen(last);

					if (l > (PARAMETER_MAX_LENGTH - 8))
						l = (PARAMETER_MAX_LENGTH - 8);

					strncpy(parameter, p, l);
					break;
				}

				command->code(parameter);
			}
			else
				command->code(0);
		}
	}
}

float SDLFrameRate()
{
	double t;
	static float fps;
	static Uint64 time, time_now, counter;
	static Uint8 first_time;

	if (!(first_time & 1))
	{
		first_time |= 1;
		time = SDL_GetTicks64();
	}

	counter++;

	if (counter == 10)
	{
		time_now = SDL_GetTicks64();
		t = (double)(time_now - time) / (double)1000;
		time = (long)time_now;
		fps = float(counter / t);
		counter = 0;
	}

	App.fps = fps;
	return fps;
}

#ifdef _WIN32
LRESULT CALLBACK WinMainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static long mouseX, mouseY, mouseB;
	static bool closing;

	switch (uMsg)
	{
	case WM_MOVE:
		Log(6, "WM_MOVE");
#ifndef USE_BGFX
		DXMove((short)lParam, short((lParam >> 16) & 0xFFFF));
#endif
		break;
	}

	LRESULT result = CallWindowProc(originalWndProc, hwnd, uMsg, wParam, lParam);

	return result;
}
#endif

#ifdef USE_BGFX
void* SDLGetNativeWindowHandle(SDL_Window *window)
{
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (!SDL_GetWindowWMInfo(window, &wmi))
	{
		return NULL;
	}

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#		if ENTRY_CONFIG_USE_WAYLAND
	wl_egl_window* win_impl = (wl_egl_window*)SDL_GetWindowData(_window, "wl_egl_window");
	if (!win_impl)
	{
		int width, height;
		SDL_GetWindowSize(_window, &width, &height);
		struct wl_surface* surface = wmi.info.wl.surface;
		if (!surface)
			return nullptr;
		win_impl = wl_egl_window_create(surface, width, height);
		SDL_SetWindowData(_window, "wl_egl_window", win_impl);
	}
	return (void*)(uintptr_t)win_impl;
#		else
	return (void*)wmi.info.x11.window;
#		endif
#elif BX_PLATFORM_OSX || BX_PLATFORM_IOS
	return wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
	return wmi.info.win.window;
#elif BX_PLATFORM_ANDROID
	return wmi.info.android.window;
#else
	#error "Unsupported platform!"
#endif // BX_PLATFORM_
}


void* SDLGetNativeDisplayHandle(SDL_Window *window)
{
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (!SDL_GetWindowWMInfo(window, &wmi))
	{
		return NULL;
	}

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#		if ENTRY_CONFIG_USE_WAYLAND
	return wmi.info.wl.display;
#		else
	return wmi.info.x11.display;
#		endif // ENTRY_CONFIG_USE_WAYLAND
#else
	return NULL;
#endif // BX_PLATFORM_*
}
#endif

void SDLDisplayString(long x, long y, char* string, ...)
{
	va_list list;
	char buf[4096];

	va_start(list, string);
	vsprintf(buf, string, list);
	PrintString(x, y, 6, buf, 0);
}

void ClearSurfaces()
{
#ifndef USE_BGFX
	D3DRECT r;

	r.x1 = App.dx.rViewport.left;
	r.y1 = App.dx.rViewport.top;
	r.y2 = App.dx.rViewport.top + App.dx.rViewport.bottom;
	r.x2 = App.dx.rViewport.left + App.dx.rViewport.right;

	if (App.dx.Flags & DXF_HWR)
		DXAttempt(App.dx.lpViewport->Clear2(1, &r, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0F, 0));

	S_DumpScreen();

	if (App.dx.Flags & DXF_HWR)
		DXAttempt(App.dx.lpViewport->Clear2(1, &r, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0F, 0));

	S_DumpScreen();
#endif
}

bool SDLCreateWindow()
{
	sdl_window = SDL_CreateWindow("Tomb4Plus", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, SDL_WINDOW_HIDDEN);
	if (!sdl_window)
	{
		return false;
	}
#ifdef _WIN32
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(sdl_window, &wmInfo);
	App.hWnd = wmInfo.info.win.window;
	App.hInstance = wmInfo.info.win.hinstance;

	// Subclass the window procedure to intercept WM_MOVE messages
	originalWndProc = (WNDPROC)GetWindowLongPtr(App.hWnd, GWLP_WNDPROC);
	SetWindowLongPtr(App.hWnd, GWLP_WNDPROC, (LONG_PTR)WinMainWndProc);
#endif

	if (!App.hWnd)
		return false;

	return true;
}

void SDLSetStyle(bool fullscreen, ulong& set)
{
	ulong style;

	style = GetWindowLong(App.hWnd, GWL_STYLE);

	if (fullscreen)
		style = (style & ~WS_OVERLAPPEDWINDOW) | WS_POPUP;
	else
		style = (style & ~WS_POPUP) | WS_OVERLAPPEDWINDOW;

	style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX | WS_SYSMENU);

	if (fullscreen)
		SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(sdl_window, 0);

	if (set)
		set = style;
}

void SDLProcessEvents()
{
	SDL_bool quit = SDL_FALSE;
	SDL_bool is_dragging = SDL_FALSE;

	while (!quit) {
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type) {
				case SDL_QUIT: {
					quit = SDL_TRUE;
					break;
				}
				case SDL_WINDOWEVENT:
				{
					switch (event.window.event)
					{
						case SDL_WINDOWEVENT_FOCUS_LOST:
						{
							if (App.SetupComplete)
							{
								appIsUnfocused = true;
								if (tomb4.hang_game_thread)
								{
									Log(5, "Change Video Mode");
									Log(5, "HangGameThread");
									S_PauseAudio();
									S_SoundPauseSamples();

									while (App.dx.InScene) {};
									App.dx.WaitAtBeginScene = 1;
									while (!App.dx.InScene) {};

									Log(5, "Game Thread Suspended");
								}
							}
							break;
						}
						case SDL_WINDOWEVENT_FOCUS_GAINED:
						{
							if (App.SetupComplete)
							{
								appIsUnfocused = false;
								if (tomb4.hang_game_thread)
								{
									App.dx.WaitAtBeginScene = 0;
									Log(5, "Game Thread Resumed");
									S_SoundUnpauseSamples();
									S_UnpauseAudio();
								}
							}

							break;
						}
						case SDL_WINDOWEVENT_MOVED:
						{
#ifndef USE_BGFX
							DXMove(event.window.data1, event.window.data2);
#endif
							break;
						}
					}
					break;
				}
				case SDL_CONTROLLERDEVICEADDED:
				{
					UpdateGamepad();
					break;
				}
				case SDL_CONTROLLERDEVICEREMOVED:
				{
					UpdateGamepad();
					break;
				}
				default:
				{
					break;
				}
			}
		}
	}
}

void SDLClose()
{
	Log(2, "SDLClose");
	InputShutdown();
	SaveSettings();
	CloseHandle(App.mutex);
	DXFreeInfo(&App.DXInfo);
#ifndef USE_BGFX
	DXClose();
#endif
	FreeBinkStuff();

	if (!G_dxptr)
		return;

	DXDSClose();

	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	DXDISPLAYMODE* dm;
	char* buf;
	long size;

	start_setup = 0;
	App.mmx = 0;
	App.SetupComplete = 0;
	App.AutoTarget = 0;

	SDLProcessCommandLine(argc, argv);

	// Tomb4Plus
	T4PlusInit();
	//

	// Tomb4Plus
	LoadGameModConfigFirstPass();
	//

	LoadGameflow();

	// Tomb4Plus
	LoadGameModConfigSecondPass();
	//

	SDL_Init(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);

	InputInit();

	if (!SDLCreateWindow())
	{
		Log(1, "Unable To Create Window");
		return 0;
	}

	DXGetInfo(&App.DXInfo, App.hWnd);

	LoadSettings();

	if (start_setup || !LoadSettings())
	{
		if (!DXSetupDialog())
		{
			free(gfScriptFile);
			free(gfLanguageFile);

			SDLClose();

			return 0;
		}

		LoadSettings();
	}

	fmvs_disabled = 1; // Disable all FMVs for now.

	SetWindowPos(App.hWnd, 0, App.dx.rScreen.left, App.dx.rScreen.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	App.dx.WaitAtBeginScene = 0;
	App.dx.InScene = 0;
	App.fmv = 0;

#ifdef USE_BGFX
	DXDISPLAYMODE display_mode;
	dm = &display_mode;

	dm->w = WINDOW_DEFAULT_WIDTH;
	dm->h = WINDOW_DEFAULT_HEIGHT;
	dm->bpp = 32;
#else
	dm = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].DisplayModes[G_dxinfo->nDisplayMode];
#endif
	SDL_SetWindowSize(sdl_window, dm->w, dm->h);

	if (!DXCreate(dm->w, dm->h, dm->bpp, App.StartFlags, &App.dx, App.hWnd, WS_OVERLAPPEDWINDOW))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"Tomb4Plus",
			GetStringForTextID(TXT_Failed_To_Setup_DirectX),
			sdl_window);
	}

	// TODO: add fullscreen support here.

	SDL_ShowWindow(sdl_window);

	if (!App.SoundDisabled)
	{
		DXDSCreate();
		ACMInit();
	}

	cutseqpakPtr = 0;
	buf = 0;
	size = LoadFile("data\\cutseq.pak", &buf);

	if (size)
	{
		cutseqpakPtr = (char*)malloc(*(long*)buf);
		Decompress(cutseqpakPtr, buf + 4, size - 4, *(long*)buf);
		free(buf);
	}

	MainThread.active = 1;
	MainThread.ended = 0;

	MainThread.handle = SDL_CreateThread(GameMain, "GameMain", (void*)NULL);
	if (MainThread.handle == NULL) {
		printf("SDL_CreateThread failed: %s\n", SDL_GetError());
	}

	SDLProcessEvents();

	MainThread.ended = 1;
	while (MainThread.active) {};

	if (cutseqpakPtr)
		free(cutseqpakPtr);

#ifdef _WIN32
	SetWindowLongPtr(App.hWnd, GWLP_WNDPROC, (LONG_PTR)originalWndProc);
#endif

	SDLClose();

	return 0;
}

#else
bool WinRunCheck(LPSTR WindowName, LPSTR ClassName, HANDLE* mutex)
{
	HWND window;

	Log(2, "WinRunCheck");
	*mutex = CreateMutex(0, 1, WindowName);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		window = FindWindow(ClassName, WindowName);

		if (window)
		{
			SendMessage(window, WM_ACTIVATE, WA_ACTIVE, 0);
			SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		return 1;
	}

	return 0;
}

void WinProcessCommandLine(LPSTR cmd)
{
	COMMANDLINES* command;
	char* pCommand;
	char* p;
	char* last;
	ulong l;
	long num;
	char parameter[PARAMETER_MAX_LENGTH];

	Log(2, "WinProcessCommandLine");

	num = sizeof(commandlines) / sizeof(commandlines[0]);
	
	for (int i = 0; i < num; i++)
	{
		command = &commandlines[i];
		command->code((char*)"_INIT");
	}

	for (int i = 0; (ulong)i < strlen(cmd); i++)
	{
		if (toupper(cmd[i]))
			cmd[i] = toupper(cmd[i]);
	}

	for (int i = 0; i < num; i++)
	{
		command = &commandlines[i];
		memset(parameter, 0, sizeof(parameter));
		pCommand = strstr(cmd, command->command);

		if (pCommand)
		{
			if (command->needs_parameter)
			{
				p = 0;
				l = strlen(pCommand);

				for (int j = 0; (ulong)j < l; j++, pCommand++)
				{
					if (*pCommand != '=')
						continue;

					p = pCommand + 1;
					l = strlen(p);

					for (j = 0; (ulong)j < l; j++, p++)
					{
						if (*p != ' ')
							break;
					}

					last = p;
					l = strlen(last);

					for (j = 0; (ulong)j < l; j++, last++)
					{
						if (*last == ' ')
							break;
					}

					strncpy(parameter, p, j);
					break;
				}

				command->code(parameter);
			}
			else
				command->code(0);
		}
	}
}

void WinClose()
{
	Log(2, "WinClose");
	SaveSettings();
	CloseHandle(App.mutex);
	DXFreeInfo(&App.DXInfo);
	DestroyAcceleratorTable(App.hAccel);
	DXClose();
	FreeBinkStuff();

	if (!G_dxptr)
		return;

	DXAttempt(G_dxptr->Keyboard->Unacquire());

	if (G_dxptr->Keyboard)
	{
		Log(4, "Released %s @ %x - RefCnt = %d", "Keyboard", G_dxptr->Keyboard, G_dxptr->Keyboard->Release());
		G_dxptr->Keyboard = 0;
	}
	else
		Log(1, "%s Attempt To Release NULL Ptr", "Keyboard");

	if (G_dxptr->lpDirectInput)
	{
		Log(4, "Released %s @ %x - RefCnt = %d", "DirectInput", G_dxptr->lpDirectInput, G_dxptr->lpDirectInput->Release());
		G_dxptr->lpDirectInput = 0;
	}
	else
		Log(1, "%s Attempt To Release NULL Ptr", "DirectInput");

	DXDSClose();
}

float WinFrameRate()
{
	double t, time_now;
	static float fps;
	static long time, counter;
	static char first_time;

	if (!(first_time & 1))
	{
		first_time |= 1;
		time = clock();
	}

	counter++;

	if (counter == 10)
	{
		time_now = clock();
		t = (time_now - time) / (double)CLOCKS_PER_SEC;
		time = (long)time_now;
		fps = float(counter / t);
		counter = 0;
	}

	App.fps = fps;
	return fps;
}

void WinDisplayString(long x, long y, char* string, ...)
{
	va_list list;
	char buf[4096];

	va_start(list, string);
	vsprintf(buf, string, list);
	PrintString(x, y, 6, buf, 0);
}

void WinProcMsg()
{
	MSG msg;

	Log(2, "WinProcMsg");

	do
	{
		GetMessage(&msg, 0, 0, 0);

		if (!TranslateAccelerator(App.hWnd, App.hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} while (!MainThread.ended && msg.message != WM_QUIT);
}

void WinProcessCommands(long cmd)
{
	DXDISPLAYMODE* dm;
	long odm;

	if (cmd == KA_ALTENTER)
	{
		if (App.fmv || !(G_dxinfo->DDInfo[G_dxinfo->nDD].DDCaps.dwCaps2 & DDCAPS2_CANRENDERWINDOWED) || LevelLoadingThread.active)
			return;

		Log(6, "KA_ALTENTER");
		Log(5, "HangGameThread");
		while (App.dx.InScene) {};
		App.dx.WaitAtBeginScene = 1;
		while (!App.dx.InScene) {};
		SuspendThread((HANDLE)MainThread.handle);
		Log(5, "Game Thread Suspended");

		DXToggleFullScreen();
		HWInitialise();
		S_InitD3DMatrix();
		SetD3DViewMatrix();
		S_UpdateMVP();
		ResumeThread((HANDLE)MainThread.handle);
		App.dx.WaitAtBeginScene = 0;
		Log(5, "Game Thread Resumed");

		if (App.dx.Flags & DXF_FULLSCREEN)
		{
			SetCursor(0);
			ShowCursor(0);
		}
		else
		{
			SetCursor(LoadCursor(0, IDC_ARROW));
			ShowCursor(1);
		}
	}
	else if (cmd == KA_ALTP || cmd == KA_ALTM)
	{
		if (LevelLoadingThread.active || App.fmv)
			return;

		Log(5, "Change Video Mode");
		Log(5, "HangGameThread");
		while (App.dx.InScene) {};
		App.dx.WaitAtBeginScene = 1;
		while (!App.dx.InScene) {};
		SuspendThread((HANDLE)MainThread.handle);
		Log(5, "Game Thread Suspended");

		odm = App.DXInfo.nDisplayMode;

		if (cmd == KA_ALTP)
		{
			App.DXInfo.nDisplayMode++;

			if (App.DXInfo.nDisplayMode >= G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].nDisplayModes)
				App.DXInfo.nDisplayMode = G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].nDisplayModes - 1;

			dm = G_dxinfo->DDInfo[App.DXInfo.nDD].D3DDevices[App.DXInfo.nD3D].DisplayModes;

			while (dm[odm].bpp != dm[App.DXInfo.nDisplayMode].bpp)
			{
				App.DXInfo.nDisplayMode++;

				if (App.DXInfo.nDisplayMode >= G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].nDisplayModes)
				{
					App.DXInfo.nDisplayMode = odm;
					break;
				}
			}
		}
		else
		{
			App.DXInfo.nDisplayMode--;

			if (App.DXInfo.nDisplayMode < 0)
				App.DXInfo.nDisplayMode = 0;

			dm = G_dxinfo->DDInfo[App.DXInfo.nDD].D3DDevices[App.DXInfo.nD3D].DisplayModes;

			while (dm[odm].bpp != dm[App.DXInfo.nDisplayMode].bpp)
			{
				App.DXInfo.nDisplayMode--;

				if (App.DXInfo.nDisplayMode < 0)
				{
					App.DXInfo.nDisplayMode = odm;
					break;
				}
			}
		}

		if (odm != App.DXInfo.nDisplayMode)
		{
			if (!DXChangeVideoMode())
			{
				App.DXInfo.nDisplayMode = odm;
				DXChangeVideoMode();
			}

			HWInitialise();
			InitWindow(0, 0, App.dx.dwRenderWidth, App.dx.dwRenderHeight, 20, 20480, 80, App.dx.dwRenderWidth, App.dx.dwRenderHeight);
			InitFont();
			S_InitD3DMatrix();
			SetD3DViewMatrix();
			S_UpdateMVP();
		}

		ResumeThread((HANDLE)MainThread.handle);
		App.dx.WaitAtBeginScene = 0;
		Log(5, "Game Thread Resumed");
		resChangeCounter = 120;
	}
}

LRESULT CALLBACK WinMainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static long mouseX, mouseY, mouseB;
	static bool closing;

	switch (uMsg)
	{
	case WM_CREATE:
		resChangeCounter = 0;
		Log(6, "WM_CREATE");
		break;

	case WM_MOVE:
		Log(6, "WM_MOVE");
		DXMove((short)lParam, short((lParam >> 16) & 0xFFFF));
		break;

	case WM_ACTIVATE:

		if (!closing)
		{
			if (App.fmv)
				return 0;

			switch (wParam & 0xFFFF)
			{
			case WA_INACTIVE:
				Log(6, "WM_INACTIVE");

				if (App.SetupComplete)
				{
					appIsUnfocused = true;
					if (tomb4.hang_game_thread)
					{
						Log(5, "Change Video Mode");
						Log(5, "HangGameThread");
						S_PauseAudio();
						S_SoundPauseSamples();
						while (App.dx.InScene) {};
						App.dx.WaitAtBeginScene = 1;
						while (!App.dx.InScene) {};
						SuspendThread((HANDLE)MainThread.handle);
						Log(5, "Game Thread Suspended");
					}
				}

				return 0;

			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				Log(6, "WM_ACTIVE");

				if (App.SetupComplete)
				{
					appIsUnfocused = false;
					if (tomb4.hang_game_thread)
					{
						ResumeThread((HANDLE)MainThread.handle);
						App.dx.WaitAtBeginScene = 0;
						Log(5, "Game Thread Resumed");
						S_SoundUnpauseSamples();
						S_UnpauseAudio();
					}
				}

				return 0;
			}
		}

		break;

	case WM_CLOSE:
		closing = 1;
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		Log(6, "WM_COMMAND");
		WinProcessCommands(wParam & 0xFFFF);
		break;

	case WM_MOUSEMOVE:
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		mouseB = wParam;
		break;

	case WM_APP:
		FillADPCMBuffer((char*)lParam, wParam);
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void ClearSurfaces()
{
	D3DRECT r;

	r.x1 = App.dx.rViewport.left;
	r.y1 = App.dx.rViewport.top;
	r.y2 = App.dx.rViewport.top + App.dx.rViewport.bottom;
	r.x2 = App.dx.rViewport.left + App.dx.rViewport.right;

	if (App.dx.Flags & DXF_HWR)
		DXAttempt(App.dx.lpViewport->Clear2(1, &r, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0F, 0));

	S_DumpScreen();

	if (App.dx.Flags & DXF_HWR)
		DXAttempt(App.dx.lpViewport->Clear2(1, &r, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0F, 0));

	S_DumpScreen();
}

bool WinRegisterWindow(HINSTANCE hinstance)
{
	App.hInstance = hinstance;
	App.WindowClass.hIcon = 0;
	App.WindowClass.lpszMenuName = 0;
	App.WindowClass.lpszClassName = "MainGameWindow";
	App.WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	App.WindowClass.hInstance = hinstance;
	App.WindowClass.style = CS_VREDRAW | CS_HREDRAW;
	App.WindowClass.lpfnWndProc = WinMainWndProc;
	App.WindowClass.cbClsExtra = 0;
	App.WindowClass.cbWndExtra = 0;
	App.WindowClass.hCursor = LoadCursor(0, IDC_ARROW);

	if (!RegisterClass(&App.WindowClass))
		return 0;

	return 1;
}

bool WinCreateWindow()
{
	App.hWnd = CreateWindowEx(WS_EX_APPWINDOW, "MainGameWindow", "Tomb4Plus", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0, App.hInstance, 0);

	if (!App.hWnd)
		return 0;

	return 1;
}

void WinSetStyle(bool fullscreen, ulong& set)
{
	ulong style;

	style = GetWindowLong(App.hWnd, GWL_STYLE);

	if (fullscreen)
		style = (style & ~WS_OVERLAPPEDWINDOW) | WS_POPUP;
	else
		style = (style & ~WS_POPUP) | WS_OVERLAPPEDWINDOW;

	style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX | WS_SYSMENU);
	SetWindowLong(App.hWnd, GWL_STYLE, style);

	if (set)
		set = style;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
	DXDISPLAYMODE* dm;
	HWND desktop;
	HDC hdc;
	DEVMODE devmode;
	char* buf;
	long size;

	start_setup = 0;
	App.mmx = 0;
	App.SetupComplete = 0;
	App.AutoTarget = 0;

	// Tomb4Plus
	T4PlusInit();
	//

	LoadGameModConfigFirstPass();

	if (WinRunCheck((char*)"Tomb4Plus", (char*)"MainGameWindow", &App.mutex))
		return 0;

	LoadGameflow();

	LoadGameModConfigSecondPass();

	WinProcessCommandLine(lpCmdLine);

	if (!WinRegisterWindow(hInstance))
	{
		Log(1, "Unable To Register Window Class");
		return 0;
	}

	if (!WinCreateWindow())
	{
		Log(1, "Unable To Create Window");
		return 0;
	}

	ShowWindow(App.hWnd, SW_HIDE);
	UpdateWindow(App.hWnd);
	DXGetInfo(&App.DXInfo, App.hWnd);

	if (start_setup || !LoadSettings())
	{
		if (!DXSetupDialog())
		{
			free(gfScriptFile);
			free(gfLanguageFile);
			WinClose();
			return 0;
		}

		LoadSettings();
	}

	if (!get_game_mod_global_info()->tr_level_editor && !get_game_mod_global_info()->tr_times_exclusive) {
		if (!fmvs_disabled)
		{
			if (!LoadBinkStuff())
			{
				MessageBox(0, "Failed to load Bink, disabling FMVs.", "Tomb Raider IV", 0);
				fmvs_disabled = 1;
			}
		}
	}

	SetWindowPos(App.hWnd, 0, App.dx.rScreen.left, App.dx.rScreen.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	desktop = GetDesktopWindow();
	hdc = GetDC(desktop);
	App.Desktopbpp = GetDeviceCaps(hdc, BITSPIXEL);
	ReleaseDC(desktop, hdc);
	App.dx.WaitAtBeginScene = 0;
	App.dx.InScene = 0;
	App.fmv = 0;
	dm = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].DisplayModes[G_dxinfo->nDisplayMode];

	if (!DXCreate(dm->w, dm->h, dm->bpp, App.StartFlags, &App.dx, App.hWnd, WS_OVERLAPPEDWINDOW))
	{
		MessageBox(0, GetStringForTextID(TXT_Failed_To_Setup_DirectX), "Tomb Raider IV", 0);
		return 0;
	}

	CoInitializeEx(0, COINIT_MULTITHREADED);

	WinSetStyle(G_dxptr->Flags & DXF_FULLSCREEN, G_dxptr->WindowStyle);
	UpdateWindow(App.hWnd);
	ShowWindow(App.hWnd, nShowCmd);

	if (App.dx.Flags & DXF_FULLSCREEN)
	{
		SetCursor(0);
		ShowCursor(0);
	}

	DXInitKeyboard(App.hWnd, App.hInstance);
	App.hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

	if (!App.SoundDisabled)
	{
		DXDSCreate();
		ACMInit();
	}

	cutseqpakPtr = 0;
	buf = 0;
	size = LoadFile("data\\cutseq.pak", &buf);

	if (size)
	{
		cutseqpakPtr = (char*)malloc(*(long*)buf);
		Decompress(cutseqpakPtr, buf + 4, size - 4, *(long*)buf);
		free(buf);
	}

	MainThread.active = 1;
	MainThread.ended = 0;
	MainThread.handle = _beginthreadex(0, 0, GameMain, 0, 0, (unsigned int*)&MainThread.address);
	WinProcMsg();
	MainThread.ended = 1;
	while (MainThread.active) {};

	if (cutseqpakPtr)
		free(cutseqpakPtr);

	WinClose();
	desktop = GetDesktopWindow();
	hdc = GetDC(desktop);
	devmode.dmSize = sizeof(DEVMODE);
	devmode.dmBitsPerPel = App.Desktopbpp;
	ReleaseDC(desktop, hdc);
	devmode.dmFields = DM_BITSPERPEL;
	ChangeDisplaySettings(&devmode, 0);
	CoUninitialize();
	return 0;
}

#endif