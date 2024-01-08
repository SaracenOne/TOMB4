#pragma once
#include "../global/types.h"

#ifndef USE_INI
bool REG_OpenKey(const char *lpSubKey);
#endif
bool OpenRegistry(const char *SubKeyName);
void REG_CloseKey();
void CloseRegistry();
void REG_WriteLong(char* SubKeyName, ulong value);
void REG_WriteBool(char* SubKeyName, bool value);
void REG_WriteString(char* SubKeyName, char* string, long length);
void REG_WriteFloat(char* SubKeyName, float value);
bool REG_ReadLong(char* SubKeyName, ulong& value, ulong defaultValue);
bool REG_ReadBool(char* SubKeyName, bool& value, bool defaultValue);
bool REG_ReadString(char* SubKeyName, char* value, long length, char* defaultValue);
bool REG_ReadFloat(char* SubKeyName, float& value, float defaultValue);
bool LoadSettings();
void SaveSettings();
bool SaveSetup(HWND hDlg);
bool REG_KeyWasCreated();
