#pragma once
#include "../global/types.h"

extern int convert_sdl_scancode_to_tomb_keycode(int scancode);
extern int convert_tomb_keycode_to_sdl_scancode(int tomb_keycode);

long Key(long number);
long S_UpdateInput();
long ReadJoystick(long& x, long& y);

extern const char* KeyboardButtons[272];
extern const char* GermanKeyboard[272];
extern short layout[2][18];

extern long conflict[18];
extern long input;
extern long linput;
extern long dbinput;
extern long inputBusy;
extern short ammo_change_timer;
extern char ammo_change_buf[12];
