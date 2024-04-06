#include "../tomb4/pch.h"
#include "registry.h"
#include "LoadSave.h"
#include "cmdline.h"
#include "input.h"
#include "winmain.h"
#include <string>

#ifdef USE_INI
#ifndef USE_SDL
#error "INI config support requires SDL!"
#endif

#include <SDL_filesystem.h>

#include <filesystem>
#include <algorithm>

#include "simpleIni.h"
#include "platform.h"

CSimpleIniA ini;

std::string config_file_path = "";

const char *current_section = "";
bool section_just_created = false;
#else
static HKEY phkResult;
static DWORD dwDisposition;
#endif

static bool REG_Setup;

#ifndef USE_INI

#ifdef LEVEL_EDITOR
#define REGISTRY_PATH "Software\\Core Design\\Tomb Raider Level Editor"
#else
#define REGISTRY_PATH "Software\\Core Design\\Tomb Raider IV"
#endif


bool REG_OpenKey(const char *lpSubKey)
{
	return RegCreateKeyEx(HKEY_CURRENT_USER, lpSubKey, 0, (CHAR*)"", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &phkResult, &dwDisposition) == ERROR_SUCCESS;
}
#endif

bool OpenRegistry(const char *section_key)
{
#ifdef USE_INI
	config_file_path = platform_get_userdata_path() + "config.ini";

	ini.SetUnicode();

	SI_Error rc = ini.LoadFile(config_file_path.c_str());
	if (rc >= 0) {
		section_just_created = false;
		if (!ini.SectionExists(section_key)) {
			section_just_created = true;
		}
	}

	current_section = section_key;

	return true;
#else
	char buffer[256];

	if (!section_key)
		return REG_OpenKey(REGISTRY_PATH);

	sprintf(buffer, "%s\\%s", REGISTRY_PATH, section_key);
	return REG_OpenKey(buffer);
#endif
}

#ifndef USE_INI
void REG_CloseKey()
{
	RegCloseKey(phkResult);
}
#endif

void CloseRegistry()
{
#ifdef USE_INI
	std::string config_path = platform_get_userdata_path();

	ini.SetUnicode();

	ini.SaveFile((config_path + std::string("config.ini")).c_str());
#else
	REG_CloseKey();
#endif
}

void REG_WriteLong(char* SubKeyName, ulong value)
{
#ifdef USE_INI
	ini.SetLongValue(current_section, SubKeyName, value);
#else
	RegSetValueEx(phkResult, SubKeyName, 0, REG_DWORD, (CONST BYTE*) & value, sizeof(ulong));
#endif
}

void REG_WriteBool(char* SubKeyName, bool value)
{
#ifdef USE_INI
	ini.SetBoolValue(current_section, SubKeyName, value);
#else
	ulong Lvalue;

	Lvalue = (ulong)value;
	RegSetValueEx(phkResult, SubKeyName, 0, REG_DWORD, (CONST BYTE*) & Lvalue, sizeof(ulong));
#endif
}

void REG_WriteString(char* SubKeyName, char* string, long length)
{
#ifdef USE_INI
	if (string)
	{
		ini.SetValue(current_section, SubKeyName, string);
	}
	else
	{
		ini.Delete(current_section, SubKeyName);
	}
#else
	long checkLength;

	if (string)
	{
		if (length < 0)
			checkLength = strlen(string);
		else
			checkLength = length;

		RegSetValueEx(phkResult, SubKeyName, 0, REG_SZ, (CONST BYTE*)string, checkLength + 1);
	}
	else
		RegDeleteValue(phkResult, SubKeyName);
#endif
}

void REG_WriteFloat(char* SubKeyName, float value)
{
#ifdef USE_INI
	ini.SetDoubleValue(current_section, SubKeyName, value);
#else
	long length;
	char buf[64];

	length = sprintf(buf, "%.5f", value);
	REG_WriteString(SubKeyName, buf, length);
#endif
}

bool REG_ReadLong(char* SubKeyName, ulong& value, ulong defaultValue)
{
#ifdef USE_INI
	value = ini.GetLongValue(current_section, SubKeyName, defaultValue);
	return true;
#else
	ulong type;
	ulong cbData;

	cbData = 4;

	if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)&value, &cbData) == ERROR_SUCCESS && type == REG_DWORD && cbData == 4)
		return 1;

	REG_WriteLong(SubKeyName, defaultValue);
	value = defaultValue;
	return 0;
#endif
}

bool REG_ReadBool(char* SubKeyName, bool& value, bool defaultValue)
{
#ifdef USE_INI
	value = ini.GetBoolValue(current_section, SubKeyName, defaultValue);
	return true;
#else
	ulong type;
	ulong cbData;
	ulong data;

	cbData = 4;

	if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)&data, &cbData) == ERROR_SUCCESS && type == REG_DWORD && cbData == 4)
	{
		value = (bool)data;
		return 1;
	}

	REG_WriteBool(SubKeyName, defaultValue);
	value = defaultValue;
	return 0;
#endif
}

bool REG_ReadString(char* SubKeyName, char* value, long length, char* defaultValue)
{
#ifdef USE_INI
	const char* loaded_string = ini.GetValue(current_section, SubKeyName, defaultValue);
	if (loaded_string) {
		size_t loaded_str_length = strlen(loaded_string);
		if (loaded_str_length <= size_t(length)) {
			memcpy(value, loaded_string, loaded_str_length);
		}
	}

	return false;
#else
	ulong type;
	ulong cbData;
	long len;

	cbData = length;

	if (RegQueryValueEx(phkResult, SubKeyName, 0, &type, (LPBYTE)value, (LPDWORD)&cbData) == ERROR_SUCCESS && type == REG_SZ)
		return 1;

	if (defaultValue)
	{
		REG_WriteString(SubKeyName, defaultValue, -1);
		len = strlen(defaultValue) + 1;

		if (len > length)
		{
			len = length - 1;
			value[len] = 0;
		}

		memcpy(value, defaultValue, len);
	}
	else
		RegDeleteValue(phkResult, SubKeyName);

	return 0;
#endif
}

bool REG_ReadFloat(char* SubKeyName, float& value, float defaultValue)
{
#ifdef USE_INI
	value = (float)ini.GetDoubleValue(current_section, SubKeyName, defaultValue);
	return true;
#else
	char buf[64];

	if (REG_ReadString(SubKeyName, buf, sizeof(buf), 0))
	{
		value = (float)atof(buf);
		return 1;
	}

	REG_WriteFloat(SubKeyName, defaultValue);
	value = defaultValue;
	return 0;
#endif
}

bool LoadSettings()
{
	ulong key;
	bool val;

	if (!OpenRegistry("System"))
		return 0;

	REG_ReadBool((char*)"Setup", REG_Setup, 0);

	if (REG_Setup)
	{
		App.TextureSize = 256;
		App.BumpMapSize = 256;
		App.StartFlags = DXF_FPUSETUP;

#ifdef USE_BGFX
		REG_ReadLong((char*)"VideoWidth", (ulong&)App.VideoWidth, WINDOW_DEFAULT_WIDTH);
		REG_ReadLong((char*)"VideoHeight", (ulong&)App.VideoHeight, WINDOW_DEFAULT_HEIGHT);
#else
		REG_ReadLong((char*)"DD", (ulong&)App.DXInfo.nDD, 0);
		REG_ReadLong((char*)"D3D", (ulong&)App.DXInfo.nD3D, 0);
		REG_ReadLong((char*)"VMode", (ulong&)App.DXInfo.nDisplayMode, 0);
		REG_ReadLong((char*)"TFormat", (ulong&)App.DXInfo.nTexture, 0);
#endif
		REG_ReadLong((char*)"DS", (ulong&)App.DXInfo.nDS, 0);
		REG_ReadBool((char*)"BumpMap", App.BumpMapping, 1);
		REG_ReadBool((char*)"Filter", App.Filtering, 1);
		REG_ReadBool((char*)"DisableSound", App.SoundDisabled, 0);
		REG_ReadBool((char*)"Volumetric", App.Volumetric, 1);
		REG_ReadBool((char*)"NoFMV", fmvs_disabled, 0);

		REG_ReadBool((char*)"TextLow", val, 0);

		if (val)
			App.TextureSize = 128;

		REG_ReadBool((char*)"BumpLow", val, 0);

		if (val)
			App.BumpMapSize = 128;

		REG_ReadBool((char*)"HardWare", val, 0);

		if (val)
			App.StartFlags |= DXF_ZBUFFER | DXF_HWR;

		REG_ReadBool((char*)"Window", val, 0);

		if (val)
			App.StartFlags |= DXF_WINDOWED;
		else
			App.StartFlags |= DXF_FULLSCREEN;
	}

	CloseRegistry();

	OpenRegistry("Game");

	REG_ReadLong((char*)"Key0", key, keyboard_layout[0][0]);
	keyboard_layout[1][0] = (short)key;

	REG_ReadLong((char*)"Key1", key, keyboard_layout[0][1]);
	keyboard_layout[1][1] = (short)key;

	REG_ReadLong((char*)"Key2", key, keyboard_layout[0][2]);
	keyboard_layout[1][2] = (short)key;

	REG_ReadLong((char*)"Key3", key, keyboard_layout[0][3]);
	keyboard_layout[1][3] = (short)key;

	REG_ReadLong((char*)"Key4", key, keyboard_layout[0][4]);
	keyboard_layout[1][4] = (short)key;

	REG_ReadLong((char*)"Key5", key, keyboard_layout[0][5]);
	keyboard_layout[1][5] = (short)key;

	REG_ReadLong((char*)"Key6", key, keyboard_layout[0][6]);
	keyboard_layout[1][6] = (short)key;

	REG_ReadLong((char*)"Key7", key, keyboard_layout[0][7]);
	keyboard_layout[1][7] = (short)key;

	REG_ReadLong((char*)"Key8", key, keyboard_layout[0][8]);
	keyboard_layout[1][8] = (short)key;

	REG_ReadLong((char*)"Key9", key, keyboard_layout[0][9]);
	keyboard_layout[1][9] = (short)key;

	REG_ReadLong((char*)"Key10", key, keyboard_layout[0][10]);
	keyboard_layout[1][10] = (short)key;

	REG_ReadLong((char*)"Key11", key, keyboard_layout[0][11]);
	keyboard_layout[1][11] = (short)key;

	REG_ReadLong((char*)"Key12", key, keyboard_layout[0][12]);
	keyboard_layout[1][12] = (short)key;

	REG_ReadLong((char*)"Key13", key, keyboard_layout[0][13]);
	keyboard_layout[1][13] = (short)key;

	REG_ReadLong((char*)"Key14", key, keyboard_layout[0][14]);
	keyboard_layout[1][14] = (short)key;

	REG_ReadLong((char*)"Key15", key, keyboard_layout[0][15]);
	keyboard_layout[1][15] = (short)key;

	REG_ReadLong((char*)"Key16", key, keyboard_layout[0][16]);
	keyboard_layout[1][16] = (short)key;

	REG_ReadLong((char*)"Key17", key, keyboard_layout[0][17]);
	keyboard_layout[1][17] = (short)key;

	REG_ReadBool((char*)"UseGamepad", use_gamepad, true);

	REG_ReadLong((char*)"MusicVolume", (ulong&)MusicVolume, 80);
	REG_ReadLong((char*)"SFXVolume", (ulong&)SFXVolume, 90);
	REG_ReadLong((char*)"ControlMethod", (ulong&)ControlMethod, 0);
	REG_ReadLong((char*)"SoundQuality", (ulong&)SoundQuality, 1);
	REG_ReadLong((char*)"AutoTarget", (ulong&)App.AutoTarget, 1);
	REG_ReadLong((char*)"WindowX", (ulong&)App.dx.rScreen, 0);
	REG_ReadLong((char*)"WindowY", (ulong&)App.dx.rScreen.top, 0);

	CloseRegistry();
	CheckKeyConflicts();
	return REG_Setup;
}

void SaveSettings()
{
	OpenRegistry("Game");
	REG_WriteLong((char*)"Key0", keyboard_layout[1][0]);
	REG_WriteLong((char*)"Key1", keyboard_layout[1][1]);
	REG_WriteLong((char*)"Key2", keyboard_layout[1][2]);
	REG_WriteLong((char*)"Key3", keyboard_layout[1][3]);
	REG_WriteLong((char*)"Key4", keyboard_layout[1][4]);
	REG_WriteLong((char*)"Key5", keyboard_layout[1][5]);
	REG_WriteLong((char*)"Key6", keyboard_layout[1][6]);
	REG_WriteLong((char*)"Key7", keyboard_layout[1][7]);
	REG_WriteLong((char*)"Key8", keyboard_layout[1][8]);
	REG_WriteLong((char*)"Key9", keyboard_layout[1][9]);
	REG_WriteLong((char*)"Key10", keyboard_layout[1][10]);
	REG_WriteLong((char*)"Key11", keyboard_layout[1][11]);
	REG_WriteLong((char*)"Key12", keyboard_layout[1][12]);
	REG_WriteLong((char*)"Key13", keyboard_layout[1][13]);
	REG_WriteLong((char*)"Key14", keyboard_layout[1][14]);
	REG_WriteLong((char*)"Key15", keyboard_layout[1][15]);
	REG_WriteLong((char*)"Key16", keyboard_layout[1][16]);
	REG_WriteLong((char*)"Key17", keyboard_layout[1][17]);
	REG_WriteBool((char*)"UseGamepad", use_gamepad);
	REG_WriteLong((char*)"ControlMethod", ControlMethod);
	REG_WriteLong((char*)"MusicVolume", MusicVolume);
	REG_WriteLong((char*)"SFXVolume", SFXVolume);
	REG_WriteLong((char*)"SoundQuality", SoundQuality);
	REG_WriteLong((char*)"AutoTarget", App.AutoTarget);
	REG_WriteLong((char*)"WindowX", App.dx.rScreen.left);
	REG_WriteLong((char*)"WindowY", App.dx.rScreen.top);
	CloseRegistry();

	OpenRegistry("System");
#ifdef USE_BGFX
	REG_WriteLong((char*)"VideoWidth", App.VideoWidth);
	REG_WriteLong((char*)"VideoHeight", App.VideoHeight);
#else
	REG_WriteLong((char*)"VMode", App.DXInfo.nDisplayMode);
#endif
	REG_WriteBool((char*)"Window", (App.dx.Flags & DXF_WINDOWED) != 0);
	CloseRegistry();
}

bool SaveSetup(HWND hDlg)
{
	OpenRegistry("System");

	LRESULT video_mode_id = SendMessage(GetDlgItem(hDlg, 1004), CB_GETITEMDATA, SendMessage(GetDlgItem(hDlg, 1004), CB_GETCURSEL, 0, 0), 0);
#ifdef USE_BGFX
	SDL_DisplayMode mode;
	int display_mode_count = SDL_GetNumDisplayModes(0);
	if (display_mode_count < 1)
	{
		mode.w = WINDOW_DEFAULT_WIDTH;
		mode.h = WINDOW_DEFAULT_HEIGHT;
	}
	else
	{
		if (SDL_GetDisplayMode(0, int(video_mode_id), &mode) != 0)
		{
			mode.w = WINDOW_DEFAULT_WIDTH;
			mode.h = WINDOW_DEFAULT_HEIGHT;
		}
	}

	REG_WriteLong((char*)"VideoWidth", mode.w);
	REG_WriteLong((char*)"VideoHeight", mode.h);
#else
	REG_WriteLong((char*)"DD", SendMessage(GetDlgItem(hDlg, 1000), CB_GETCURSEL, 0, 0));
	REG_WriteLong((char*)"D3D", SendMessage(GetDlgItem(hDlg, 1003), CB_GETCURSEL, 0, 0) + 1); // Tomb4Plus: +1 due to us skipping the software emulation device.
	REG_WriteLong((char*)"VMode", vmode_id);
#endif
	REG_WriteLong((char*)"DS", ulong(SendMessage(GetDlgItem(hDlg, 1005), CB_GETCURSEL, 0, 0)));
	REG_WriteLong((char*)"TFormat", ulong(SendMessage(GetDlgItem(hDlg, 1006), CB_GETCURSEL, 0, 0)));

	REG_WriteBool((char*)"Filter", SendMessage(GetDlgItem(hDlg, 1012), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"BumpMap", SendMessage(GetDlgItem(hDlg, 1016), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"HardWare", SendMessage(GetDlgItem(hDlg, 1010), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"DisableSound", SendMessage(GetDlgItem(hDlg, 1018), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"TextLow", SendMessage(GetDlgItem(hDlg, 1014), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"BumpLow", SendMessage(GetDlgItem(hDlg, 1015), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"Window", SendMessage(GetDlgItem(hDlg, 1025), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"Volumetric", SendMessage(GetDlgItem(hDlg, 1029), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"NoFMV", SendMessage(GetDlgItem(hDlg, 1030), BM_GETCHECK, 0, 0));
	REG_WriteBool((char*)"Setup", 1);

	CloseRegistry();
	return 1;
}

bool REG_KeyWasCreated()
{
#ifdef USE_INI
	return section_just_created;
#else
	return dwDisposition == REG_CREATED_NEW_KEY;
#endif
}

