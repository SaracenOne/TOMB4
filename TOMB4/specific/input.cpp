#include "../tomb4/pch.h"
#include "input.h"
#include "dxshell.h"
#include "../game/sound.h"
#include "../game/newinv.h"
#include "../game/lara_states.h"
#include "../game/control.h"
#include "../game/camera.h"
#include "LoadSave.h"
#include "winmain.h"
#include "../game/lara.h"
#include "../game/savegame.h"
#include "../game/gameflow.h"
#include "../game/spotcam.h"
#include "../tomb4/tomb4.h"

#ifdef USE_SDL
int convert_sdl_scancode_to_tomb_keycode(int scancode) {
	switch (scancode) {
	case SDL_SCANCODE_UNKNOWN:
		return 0x00; // 
	case SDL_SCANCODE_A:
		return 0x1E; // DIK_A
	case SDL_SCANCODE_B:
		return 0x30; // DIK_B
	case SDL_SCANCODE_C:
		return 0x2E; // DIK_C
	case SDL_SCANCODE_D:
		return 0x20; // DIK_D
	case SDL_SCANCODE_E:
		return 0x12; // DIK_E
	case SDL_SCANCODE_F:
		return 0x21; // DIK_F
	case SDL_SCANCODE_G:
		return 0x22; // DIK_G
	case SDL_SCANCODE_H:
		return 0x23; // DIK_H
	case SDL_SCANCODE_I:
		return 0x17; // DIK_I
	case SDL_SCANCODE_J:
		return 0x24; // DIK_J
	case SDL_SCANCODE_K:
		return 0x25; // DIK_K
	case SDL_SCANCODE_L:
		return 0x26; // DIK_L
	case SDL_SCANCODE_M:
		return 0x32; // DIK_M
	case SDL_SCANCODE_N:
		return 0x31; // DIK_N
	case SDL_SCANCODE_O:
		return 0x18; // DIK_O
	case SDL_SCANCODE_P:
		return 0x19; // DIK_P
	case SDL_SCANCODE_Q:
		return 0x10; // DIK_Q
	case SDL_SCANCODE_R:
		return 0x13; // DIK_R
	case SDL_SCANCODE_S:
		return 0x1F; // DIK_S
	case SDL_SCANCODE_T:
		return 0x14; // DIK_T
	case SDL_SCANCODE_U:
		return 0x16; // DIK_U
	case SDL_SCANCODE_V:
		return 0x2F; // DIK_V
	case SDL_SCANCODE_W:
		return 0x11; // DIK_W
	case SDL_SCANCODE_X:
		return 0x2D; // DIK_X
	case SDL_SCANCODE_Y:
		return 0x15; // DIK_Y
	case SDL_SCANCODE_Z:
		return 0x2C; // DIK_Z

	case SDL_SCANCODE_1:
		return 0x02; // DIK_1
	case SDL_SCANCODE_2:
		return 0x03; // DIK_2
	case SDL_SCANCODE_3:
		return 0x04; // DIK_3
	case SDL_SCANCODE_4:
		return 0x05; // DIK_4
	case SDL_SCANCODE_5:
		return 0x06; // DIK_5
	case SDL_SCANCODE_6:
		return 0x07; // DIK_6
	case SDL_SCANCODE_7:
		return 0x08; // DIK_7
	case SDL_SCANCODE_8:
		return 0x09; // DIK_8
	case SDL_SCANCODE_9:
		return 0x0A; // DIK_9
	case SDL_SCANCODE_0:
		return 0x0B; // DIK_0

	case SDL_SCANCODE_RETURN:
		return 0x1C; // DIK_RETURN
	case SDL_SCANCODE_ESCAPE:
		return 0x01; // DIK_ESCAPE
	case SDL_SCANCODE_BACKSPACE:
		return 0x0E; // DIK_BACK
	case SDL_SCANCODE_TAB:
		return 0x0F; // DIK_TAB
	case SDL_SCANCODE_SPACE:
		return 0x39; // DIK_SPACE

	case SDL_SCANCODE_RIGHT:
		return 0xCD; // DIK_RIGHT
	case SDL_SCANCODE_LEFT:
		return 0xCB; // DIK_LEFT
	case SDL_SCANCODE_DOWN:
		return 0xD0; // DIK_DOWN
	case SDL_SCANCODE_UP:
		return 0xC8; // DIK_UP

	case SDL_SCANCODE_MINUS:
		return DIK_MINUS; // DIK_MINUS
	case SDL_SCANCODE_EQUALS:
		return DIK_EQUALS; // DIK_EQUALS
	case SDL_SCANCODE_LEFTBRACKET:
		return DIK_LBRACKET; // DIK_LBRACKET
	case SDL_SCANCODE_RIGHTBRACKET:
		return DIK_RBRACKET; // DIK_RBRACKET
	case SDL_SCANCODE_BACKSLASH:
		return DIK_BACKSLASH; // DIK_BACKSLASH

	case SDL_SCANCODE_LCTRL:
		return 0x1D; // DIK_LCONTROL
	case SDL_SCANCODE_LSHIFT:
		return 0x2A; // DIK_LSHIFT
	case SDL_SCANCODE_LALT:
		return 0x38; // DIK_LALT
	case SDL_SCANCODE_LGUI:
		return 0xDB; // DIK_LWIN
	case SDL_SCANCODE_RCTRL:
		return 0x9D; // DIK_RCONTROL
	case SDL_SCANCODE_RSHIFT:
		return 0x36; // DIK_RSHIFT
	case SDL_SCANCODE_RALT:
		return 0xB8; // DIK_RALT
	case SDL_SCANCODE_RGUI:
		return 0xDC; // DIK_RWIN

	default:
		return 0x00;
	}
};

int convert_tomb_keycode_to_sdl_scancode(int tomb_keycode) {
	switch (tomb_keycode) {
	case 0x1E: // DIK_A
		return SDL_SCANCODE_A;
	case 0x30: // DIK_B
		return SDL_SCANCODE_B;
	case 0x2E: // DIK_C
		return SDL_SCANCODE_C;
	case 0x20: // DIK_D
		return SDL_SCANCODE_D;
	case 0x12: // DIK_E
		return SDL_SCANCODE_E;
	case 0x21: // DIK_F
		return SDL_SCANCODE_F;
	case 0x22: // DIK_G
		return SDL_SCANCODE_G;
	case 0x23: // DIK_H
		return SDL_SCANCODE_H;
	case 0x17: // DIK_I
		return SDL_SCANCODE_I;
	case 0x24: // DIK_J
		return SDL_SCANCODE_J;
	case 0x25: // DIK_K
		return SDL_SCANCODE_K;
	case 0x26: // DIK_L
		return SDL_SCANCODE_L;
	case 0x32: // DIK_M
		return SDL_SCANCODE_M;
	case 0x31: // DIK_N
		return SDL_SCANCODE_N;
	case 0x18: // DIK_O
		return SDL_SCANCODE_O;
	case 0x19: // DIK_P
		return SDL_SCANCODE_P;
	case 0x10: // DIK_Q
		return SDL_SCANCODE_Q;
	case 0x13: // DIK_R
		return SDL_SCANCODE_R;
	case 0x1F: // DIK_S
		return SDL_SCANCODE_S;
	case 0x14: // DIK_T
		return SDL_SCANCODE_T;
	case 0x16: // DIK_U
		return SDL_SCANCODE_U;
	case 0x2F: // DIK_V
		return SDL_SCANCODE_V;
	case 0x11: // DIK_W
		return SDL_SCANCODE_W;
	case 0x2D: // DIK_X
		return SDL_SCANCODE_X;
	case 0x15: // DIK_Y
		return SDL_SCANCODE_Y;
	case 0x2C: // DIK_Z
		return SDL_SCANCODE_Z;

	case 0x02: // DIK_1
		return SDL_SCANCODE_1;
	case 0x03: // DIK_2
		return SDL_SCANCODE_2;
	case 0x04: // DIK_3
		return SDL_SCANCODE_3;
	case 0x05: // DIK_4
		return SDL_SCANCODE_4;
	case 0x06: // DIK_5
		return SDL_SCANCODE_5;
	case 0x07: // DIK_6
		return SDL_SCANCODE_6;
	case 0x08: // DIK_7
		return SDL_SCANCODE_7;
	case 0x09: // DIK_8
		return SDL_SCANCODE_8;
	case 0x0A: // DIK_9
		return SDL_SCANCODE_9;
	case 0x0B: // DIK_0
		return SDL_SCANCODE_0;

	case 0x1C: // DIK_RETURN
		return SDL_SCANCODE_RETURN;
	case 0x01: // DIK_ESCAPE
		return SDL_SCANCODE_ESCAPE;
	case 0x0E: // DIK_BACK
		return SDL_SCANCODE_BACKSPACE;
	case 0x0F: // DIK_TAB
		return SDL_SCANCODE_TAB;
	case 0x39: // DIK_SPACE
		return SDL_SCANCODE_SPACE;

	case 0xCD: // DIK_RIGHT
		return SDL_SCANCODE_RIGHT;
	case 0xCB: // DIK_LEFT
		return SDL_SCANCODE_LEFT;
	case 0xD0: // DIK_DOWN
		return SDL_SCANCODE_DOWN;
	case 0xC8: // DIK_UP
		return SDL_SCANCODE_UP;

	case DIK_MINUS: // DIK_MINUS
		return SDL_SCANCODE_MINUS;
	case DIK_EQUALS: // DIK_EQUALS
		return SDL_SCANCODE_EQUALS;
	case DIK_LBRACKET: // DIK_LBRACKET
		return SDL_SCANCODE_LEFTBRACKET;
	case DIK_RBRACKET: // DIK_RBRACKET
		return SDL_SCANCODE_RIGHTBRACKET;
	case DIK_BACKSLASH: // DIK_BACKSLASH
		return SDL_SCANCODE_BACKSLASH;

	case 0x1D: // DIK_LCONTROL
		return SDL_SCANCODE_LCTRL;
	case 0x2A: // DIK_LSHIFT
		return SDL_SCANCODE_LSHIFT;
	case 0x38: // DIK_LALT
		return SDL_SCANCODE_LALT;
	case 0xDB: // DIK_LWIN
		return SDL_SCANCODE_LGUI;
	case 0x9D: // DIK_RCONTROL
		return SDL_SCANCODE_RCTRL;
	case 0x36: // DIK_RSHIFT
		return SDL_SCANCODE_RSHIFT;
	case 0xB8: // DIK_RALT
		return SDL_SCANCODE_RALT;
	case 0xDC: // DIK_RWIN
		return SDL_SCANCODE_RGUI;

	default:
		return SDL_SCANCODE_UNKNOWN;
	}
}
#else 
int convert_sdl_scancode_to_tomb_keycode(int scancode) {
	return scancode;
};

int convert_tomb_keycode_to_sdl_scancode(int tomb_keycode) {
	return tomb_keycode;
}
#endif

const char* KeyboardButtons[272] =
{
	0,
	"Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "+", "Bksp",
	"Tab", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "<", ">", "Return",
	"Ctrl", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "`",
	"Shift", "#", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "Shift",
	"Padx", "Alt", "Space", "Caps", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Nmlk", 0,
	"Pad7", "Pad8", "Pad9", "Pad-",
	"Pad4", "Pad5", "Pad6", "Pad+",
	"Pad1", "Pad2", "Pad3",
	"Pad0", "Pad.", 0, 0, "\\", 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Enter", "Ctrl", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Shift", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Pad/", 0, 0, "Alt", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Home", "Up", "PgUp", 0, "Left", 0, "Right", 0, "End", "Down", "PgDn", "Ins", "Del",
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Joy 1", "Joy 2", "Joy 3", "Joy 4", "Joy 5", "Joy 6", "Joy 7", "Joy 8",
	"Joy 9", "Joy 10", "Joy 11", "Joy 12", "Joy 13", "Joy 14", "Joy 15", "Joy 16"
};

const char* GermanKeyboard[272] =
{
	0,
	"Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "=", "`", "Entf",
	"Tab", "Q", "W", "E", "R", "T", "Z", "U", "I", "O", "P", "~u", "x", "Enter",
	"Strg", "A", "S", "D", "F", "G", "H", "J", "K", "L", "~o", "~a", "( ",
	"Shift", "#", "Y", "X", "C", "V", "B", "N", "M", ",", ".", "-", "Shift",
	"Num x", "Alt", "Leert.", "Caps", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Nmlk", 0,
	"Num7", "Num8", "Num9", "Num-",
	"Num4", "Num5", "Num6", "Num+",
	"Num1", "Num2", "Num3",
	"Num0", "Num.", 0, 0, ">", 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Enter", "Strg", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Shift", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Num/", 0, 0, "Alt", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Pos1", "Hoch", "BdAuf", 0, "Links", 0, "Rechts", 0, "Ende", "Runter", "BdAb", "Einfg", "Entf",
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"Joy 1", "Joy 2", "Joy 3", "Joy 4", "Joy 5", "Joy 6", "Joy 7", "Joy 8",
	"Joy 9", "Joy 10", "Joy 11", "Joy 12", "Joy 13", "Joy 14", "Joy 15", "Joy 16"
};

short layout[2][18] =
{
#ifdef USE_SDL
	{ SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_PERIOD, SDL_SCANCODE_SLASH, SDL_SCANCODE_RSHIFT, SDL_SCANCODE_RALT, SDL_SCANCODE_RCTRL,
	SDL_SCANCODE_SPACE, SDL_SCANCODE_COMMA, SDL_SCANCODE_KP_0, SDL_SCANCODE_END, SDL_SCANCODE_ESCAPE, SDL_SCANCODE_DELETE, SDL_SCANCODE_PAGEDOWN, SDL_SCANCODE_P, SDL_SCANCODE_RETURN },

	{ SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_PERIOD, SDL_SCANCODE_SLASH, SDL_SCANCODE_RSHIFT, SDL_SCANCODE_RALT, SDL_SCANCODE_RCTRL,
	SDL_SCANCODE_SPACE, SDL_SCANCODE_COMMA, SDL_SCANCODE_KP_0, SDL_SCANCODE_END, SDL_SCANCODE_ESCAPE, SDL_SCANCODE_DELETE, SDL_SCANCODE_PAGEDOWN, SDL_SCANCODE_P, SDL_SCANCODE_RETURN }
#else
	{ DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT, DIK_PERIOD, DIK_SLASH, DIK_RSHIFT, DIK_RALT, DIK_RCONTROL,
	DIK_SPACE, DIK_COMMA, DIK_NUMPAD0, DIK_END, DIK_ESCAPE, DIK_DELETE, DIK_PGDN, DIK_P, DIK_RETURN },

	{ DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT, DIK_PERIOD, DIK_SLASH, DIK_RSHIFT, DIK_RALT, DIK_RCONTROL,
	DIK_SPACE, DIK_COMMA, DIK_NUMPAD0, DIK_END, DIK_ESCAPE, DIK_DELETE, DIK_PGDN, DIK_P, DIK_RETURN }
#endif
};

long conflict[18];
long input;
long linput;
long dbinput;
long inputBusy;
short ammo_change_timer = 0;
char ammo_change_buf[12];

static long joy_x;
static long joy_y;
static long joy_fire;

static void DoWeaponHotkey()	//adds extra checks and does ammo type swaps..
{
	short state;
	bool goin;

	if (!lara_item)
		goin = 0;
	else
	{
		state = lara_item->current_anim_state;
		goin = !(gfLevelFlags & GF_YOUNGLARA) && (lara.water_status == LW_ABOVE_WATER || lara.water_status == LW_WADE) && !bDisableLaraControl &&
			(state != AS_ALL4S && state != AS_CRAWL && state != AS_ALL4TURNL && state != AS_ALL4TURNR && state != AS_CRAWLBACK &&
				state != AS_CRAWL2HANG && state != AS_DUCK && state != AS_DUCKROTL && state != AS_DUCKROTR);
	}

	if (!goin)
		return;

#ifdef USE_SDL
	if (keymap[SDL_SCANCODE_1])
#else
	if (keymap[DIK_1])
#endif
	{
		if (!(lara.pistols_type_carried & W_PRESENT))
			return;

		lara.request_gun_type = WEAPON_PISTOLS;

		if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_PISTOLS)
			lara.gun_status = LG_DRAW_GUNS;
	}
#ifdef USE_SDL
	else if (keymap[SDL_SCANCODE_2])
#else
	else if (keymap[DIK_2])
#endif
	{
		if (!(lara.shotgun_type_carried & W_PRESENT))
			return;

		lara.request_gun_type = WEAPON_SHOTGUN;

		if (lara.gun_type == WEAPON_SHOTGUN)
		{
			if (lara.gun_status == LG_NO_ARMS)
				lara.gun_status = LG_DRAW_GUNS;
			else if (lara.gun_status == LG_READY && !ammo_change_timer)
			{
				if (!tomb4.ammotype_hotkeys)
					return;

				memset(ammo_change_buf, 0, sizeof(ammo_change_buf));

				if (lara.shotgun_type_carried & W_AMMO2)
				{
					lara.shotgun_type_carried &= ~W_AMMO2;
					lara.shotgun_type_carried |= W_AMMO1;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Normal");
				}
				else if (lara.shotgun_type_carried & W_AMMO1)
				{
					lara.shotgun_type_carried &= ~W_AMMO1;
					lara.shotgun_type_carried |= W_AMMO2;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Wideshot");
				}
			}
		}
	}
#ifdef USE_SDL
	else if (keymap[SDL_SCANCODE_3])
#else
	else if (keymap[DIK_3])
#endif
	{
		if (!(lara.uzis_type_carried & W_PRESENT))
			return;

		lara.request_gun_type = WEAPON_UZI;

		if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_UZI)
			lara.gun_status = LG_DRAW_GUNS;
	}
#ifdef USE_SDL
	else if (keymap[SDL_SCANCODE_4])
#else
	else if (keymap[DIK_4])
#endif
	{
		if (!(lara.sixshooter_type_carried & W_PRESENT))
			return;

		lara.request_gun_type = WEAPON_REVOLVER;

		if (lara.gun_status == LG_NO_ARMS && lara.gun_type == WEAPON_REVOLVER)
			lara.gun_status = LG_DRAW_GUNS;
	}
#ifdef USE_SDL
	else if (keymap[SDL_SCANCODE_5])
#else
	else if (keymap[DIK_5])
#endif
	{
		if (!(lara.grenade_type_carried & W_PRESENT))
			return;

		lara.request_gun_type = WEAPON_GRENADE;

		if (lara.gun_type == WEAPON_GRENADE)
		{
			if (lara.gun_status == LG_NO_ARMS)
				lara.gun_status = LG_DRAW_GUNS;
			else if (lara.gun_status == LG_READY && !ammo_change_timer)
			{
				if (!tomb4.ammotype_hotkeys)
					return;

				memset(ammo_change_buf, 0, sizeof(ammo_change_buf));

				if (lara.grenade_type_carried & W_AMMO3)
				{
					lara.grenade_type_carried &= ~W_AMMO3;
					lara.grenade_type_carried |= W_AMMO2;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Super");
				}
				else if (lara.grenade_type_carried & W_AMMO2)
				{
					lara.grenade_type_carried &= ~W_AMMO2;
					lara.grenade_type_carried |= W_AMMO1;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Normal");
				}
				else if (lara.grenade_type_carried & W_AMMO1)
				{
					lara.grenade_type_carried &= ~W_AMMO1;
					lara.grenade_type_carried |= W_AMMO3;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Flash");
				}
			}
		}
	}
#ifdef USE_SDL
	else if (keymap[SDL_SCANCODE_6])
#else
	else if (keymap[DIK_6])
#endif
	{
		if (!(lara.crossbow_type_carried & W_PRESENT))
			return;

		lara.request_gun_type = WEAPON_CROSSBOW;

		if (lara.gun_type == WEAPON_CROSSBOW)
		{
			if (lara.gun_status == LG_NO_ARMS)
				lara.gun_status = LG_DRAW_GUNS;
			else if (lara.gun_status == LG_READY && !ammo_change_timer)
			{
				if (!tomb4.ammotype_hotkeys)
					return;

				memset(ammo_change_buf, 0, sizeof(ammo_change_buf));

				if (lara.crossbow_type_carried & W_AMMO3)
				{
					lara.crossbow_type_carried &= ~W_AMMO3;
					lara.crossbow_type_carried |= W_AMMO2;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Poison");
				}
				else if (lara.crossbow_type_carried & W_AMMO2)
				{
					lara.crossbow_type_carried &= ~W_AMMO2;
					lara.crossbow_type_carried |= W_AMMO1;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Normal");
				}
				else if (lara.crossbow_type_carried & W_AMMO1)
				{
					lara.crossbow_type_carried &= ~W_AMMO1;
					lara.crossbow_type_carried |= W_AMMO3;
					ammo_change_timer = 30;
					sprintf(ammo_change_buf, "Explosive");
				}
			}
		}
	}
}

long Key(long number)
{
	short key;

	key = layout[1][number];

	if (key < 256)
	{
		if (keymap[key])
			return 1;

		switch (key)
		{
#ifdef USE_SDL
		case SDL_SCANCODE_RCTRL:
			return keymap[SDL_SCANCODE_LCTRL];

		case SDL_SCANCODE_LCTRL:
			return keymap[SDL_SCANCODE_RCTRL];

		case SDL_SCANCODE_RSHIFT:
			return keymap[SDL_SCANCODE_LSHIFT];

		case SDL_SCANCODE_LSHIFT:
			return keymap[SDL_SCANCODE_RSHIFT];

		case SDL_SCANCODE_RALT:
			return keymap[SDL_SCANCODE_LALT];

		case SDL_SCANCODE_LALT:
			return keymap[SDL_SCANCODE_RALT];
#else
		case DIK_RCONTROL:
			return keymap[DIK_LCONTROL];

		case DIK_LCONTROL:
			return keymap[DIK_RCONTROL];

		case DIK_RSHIFT:
			return keymap[DIK_LSHIFT];

		case DIK_LSHIFT:
			return keymap[DIK_RSHIFT];

		case DIK_RMENU:
			return keymap[DIK_LMENU];

		case DIK_LMENU:
			return keymap[DIK_RMENU];
#endif
		}
	}
	else if (joy_fire & (1 << key))
		return 1;

	if (conflict[number])
		return 0;

	key = layout[0][number];

	if (keymap[key])
		return 1;

	switch (key)
	{
#ifdef USE_SDL
	case SDL_SCANCODE_RCTRL:
		return keymap[SDL_SCANCODE_LCTRL];

	case SDL_SCANCODE_LCTRL:
		return keymap[SDL_SCANCODE_RCTRL];

	case SDL_SCANCODE_RSHIFT:
		return keymap[SDL_SCANCODE_LSHIFT];

	case SDL_SCANCODE_LSHIFT:
		return keymap[SDL_SCANCODE_RSHIFT];

	case SDL_SCANCODE_RALT:
		return keymap[SDL_SCANCODE_LALT];

	case SDL_SCANCODE_LALT:
		return keymap[SDL_SCANCODE_RALT];
	}
#else
	case DIK_RCONTROL:
		return keymap[DIK_LCONTROL];

	case DIK_LCONTROL:
		return keymap[DIK_RCONTROL];

	case DIK_RSHIFT:
		return keymap[DIK_LSHIFT];

	case DIK_LSHIFT:
		return keymap[DIK_RSHIFT];

	case DIK_RMENU:
		return keymap[DIK_LMENU];

	case DIK_LMENU:
		return keymap[DIK_RMENU];
	}
#endif
	return 0;
}

long S_UpdateInput()
{
	static long LookCnt;
	static long med_hotkey_timer;
	short state;
	static bool flare_no_db = 0;
	bool debounce;

	debounce = SetDebounce;
#ifdef USE_SDL
	keymap = DXReadKeyboard(keymap);
#else
	DXReadKeyboard(keymap);
#endif

	if (ControlMethod == 1)
		joy_fire = ReadJoystick(joy_x, joy_y);

	linput = 0;

	if (ControlMethod == 1)
	{
		if (joy_x < -8)
			linput = IN_LEFT;
		else if (joy_x > 8)
			linput = IN_RIGHT;

		if (joy_y > 8)
			linput |= IN_BACK;
		else if (joy_y < -8)
			linput |= IN_FORWARD;
	}

	if (Key(0))
		linput |= IN_FORWARD;

	if (Key(1))
		linput |= IN_BACK;

	if (Key(2))
		linput |= IN_LEFT;

	if (Key(3))
		linput |= IN_RIGHT;

	if (Key(4))
		linput |= IN_DUCK;

	if (Key(5))
		linput |= IN_SPRINT;

	if (Key(6))
		linput |= IN_WALK;

	if (Key(7))
		linput |= IN_JUMP;

	if (Key(8))
		linput |= IN_SELECT | IN_ACTION;

	if (Key(9))
		linput |= IN_DRAW;

	if (Key(10))
	{
		if (!flare_no_db)
		{
			state = lara_item->current_anim_state;

			if (state == AS_ALL4S || state == AS_CRAWL || state == AS_ALL4TURNL ||
				state == AS_ALL4TURNR || state == AS_CRAWLBACK || state == AS_CRAWL2HANG)
			{
				SoundEffect(SFX_LARA_NO, 0, SFX_ALWAYS);
				flare_no_db = 1;
			}
			else
			{
				flare_no_db = 0;
				linput |= IN_FLARE;
			}
		}
	}
	else
		flare_no_db = 0;

	if (Key(11))
		linput |= IN_LOOK;

	if (Key(12))
		linput |= IN_ROLL;

	if (Key(13))
		linput |= IN_OPTION;

	if (Key(14))
		linput |= IN_WALK | IN_LEFT;

	if (Key(15))
		linput |= IN_WALK | IN_RIGHT;

	if (Key(16))
		linput |= IN_PAUSE;

	if (Key(17))
		linput |= IN_SELECT;

#ifdef USE_SDL
	if (keymap[SDL_SCANCODE_ESCAPE])
		linput |= IN_DESELECT | IN_OPTION;
#else
	if (keymap[DIK_ESCAPE])
		linput |= IN_DESELECT | IN_OPTION;
#endif

	if (lara.gun_status == LG_READY)
	{
		savegame.AutoTarget = (uchar)App.AutoTarget;

		if (linput & IN_LOOK)
		{
			if (LookCnt >= 6)
				LookCnt = 100;
			else
			{
				linput &= ~IN_LOOK;
				LookCnt++;
			}
		}
		else
		{
			if (LookCnt && LookCnt != 100)
				linput |= IN_TARGET;

			LookCnt = 0;
		}
	}

	DoWeaponHotkey();

#ifdef USE_SDL
	if (keymap[SDL_SCANCODE_0])
#else
	if (keymap[DIK_0])
#endif
	{
		if (!med_hotkey_timer)
		{
			if (lara_item->hit_points > 0 && lara_item->hit_points < 1000 || lara.poisoned)
			{
				if (lara.num_small_medipack)
				{
					if (lara.num_small_medipack != -1)
						lara.num_small_medipack--;

					lara.dpoisoned = 0;
					lara_item->hit_points += 500;
					SoundEffect(SFX_MENU_MEDI, 0, SFX_ALWAYS);
					savegame.Game.HealthUsed++;

					if (lara_item->hit_points > 1000)
						lara_item->hit_points = 1000;

					if (InventoryActive && !lara.num_small_medipack)
					{
						construct_object_list();

						if (lara.num_large_medipack)
							setup_objectlist_startposition(INV_BIGMEDI_ITEM);
						else
							setup_objectlist_startposition(INV_MEMCARD_LOAD_ITEM);
					}

					med_hotkey_timer = 15;
				}
			}
		}
	}
#ifdef USE_SDL
	else if (keymap[SDL_SCANCODE_9])
#else
	else if (keymap[DIK_9])
#endif
	{
		if (!med_hotkey_timer)
		{
			if (lara_item->hit_points > 0 && lara_item->hit_points < 1000 || lara.poisoned)
			{
				if (lara.num_large_medipack)
				{
					if (lara.num_large_medipack != -1)
						lara.num_large_medipack--;

					lara.dpoisoned = 0;
					lara_item->hit_points = 1000;
					SoundEffect(SFX_MENU_MEDI, 0, SFX_ALWAYS);
					savegame.Game.HealthUsed++;
					med_hotkey_timer = 15;

					if (InventoryActive && !lara.num_large_medipack)
					{
						construct_object_list();

						if (lara.num_small_medipack)
							setup_objectlist_startposition(INV_SMALLMEDI_ITEM);
						else
							setup_objectlist_startposition(INV_MEMCARD_LOAD_ITEM);
					}
				}
			}
		}
	}
	else if (med_hotkey_timer)
		med_hotkey_timer--;

	if (linput & IN_WALK && !(linput & (IN_FORWARD | IN_BACK)))
	{
		if (linput & IN_LEFT)
			linput = (linput & ~IN_LEFT) | IN_LSTEP;
		else if (linput & IN_RIGHT)
			linput = (linput & ~IN_RIGHT) | IN_RSTEP;
	}

	if (linput & IN_FORWARD && linput & IN_BACK)
		linput |= IN_ROLL;

	if (linput & IN_ROLL && BinocularRange)
		linput &= ~IN_ROLL;

	if ((linput & (IN_RIGHT | IN_LEFT)) == (IN_RIGHT | IN_LEFT))
		linput -= IN_RIGHT | IN_LEFT;

	if (debounce)
		dbinput = inputBusy;

	if (!gfGameMode && Gameflow->LoadSaveEnabled)
	{
#ifdef USE_SDL
		if (keymap[SDL_SCANCODE_F5])
#else
		if (keymap[DIK_F5])
#endif
			linput |= IN_SAVE;

#ifdef USE_SDL
		if (keymap[SDL_SCANCODE_F6])
#else
		if (keymap[DIK_F6])
#endif
			linput |= IN_LOAD;
	}

#ifdef USE_SDL
	if (keymap[SDL_SCANCODE_APOSTROPHE])
#else
	if (keymap[DIK_APOSTROPHE])
#endif
		DXSaveScreen(App.dx.lpBackBuffer, "Tomb");

	inputBusy = linput;

	if (lara.Busy)
	{
		linput &= IN_PAUSE | IN_LOOK | IN_OPTION | IN_RIGHT | IN_LEFT | IN_BACK | IN_FORWARD;

		if (linput & IN_FORWARD && linput & IN_BACK)
			linput ^= IN_BACK;
	}

	if (debounce)
		dbinput = inputBusy & (dbinput ^ inputBusy);

	input = linput;
	return 1;
}

long ReadJoystick(long& x, long& y)
{
	JOYINFOEX joystick;
	static JOYCAPS caps;
	static long unavailable = 1;

	joystick.dwSize = sizeof(JOYINFOEX);
	joystick.dwFlags = JOY_RETURNX | JOY_RETURNY | JOY_RETURNBUTTONS;

	if (joyGetPosEx(0, &joystick) != JOYERR_NOERROR)
	{
		unavailable = 1;
		x = 0;
		y = 0;
		return 0;
	}

	if (unavailable)
	{
		if (joyGetDevCaps(JOYSTICKID1, &caps, sizeof(caps)) != JOYERR_NOERROR)
		{
			x = 0;
			y = 0;
			return 0;
		}
		else
			unavailable = 0;
	}

	x = (joystick.dwXpos << 5) / (caps.wXmax - caps.wXmin) - 16;
	y = (joystick.dwYpos << 5) / (caps.wYmax - caps.wYmin) - 16;
	return joystick.dwButtons;
}
