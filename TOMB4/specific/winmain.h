#pragma once
#include "../global/types.h"

#ifdef USE_SDL
extern SDL_Window* sdl_window;

float SDLFrameRate();
void SDLDisplayString(long x, long y, char* string, ...);
void ClearSurfaces();
void SDLSetStyle(bool fullscreen, ulong& set);
#else
bool WinRunCheck(LPSTR WindowName, LPSTR ClassName, HANDLE* mutex);
void WinProcessCommandLine(LPSTR cmd);
void WinClose();
float WinFrameRate();
void WinDisplayString(long x, long y, char* string, ...);
void WinProcMsg();
void WinProcessCommands(long cmd);
LRESULT CALLBACK WinMainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ClearSurfaces();
bool WinRegisterWindow(HINSTANCE hinstance);
bool WinCreateWindow();
void WinSetStyle(bool fullscreen, ulong& set);
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd);

#endif

extern WINAPP App;
extern char* cutseqpakPtr;
extern long resChangeCounter;
extern bool appIsUnfocused;
