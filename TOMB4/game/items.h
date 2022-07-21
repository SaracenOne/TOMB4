#pragma once
#include "../global/vars.h"

void inject_items(bool replace);

void InitialiseItemArray(short num);
void KillItem(short item_num);
short CreateItem();
void InitialiseItem(short item_num);
void RemoveActiveItem(short item_num);
void RemoveDrawnItem(short item_num);
void AddActiveItem(short item_num);

#define KillEffect	( (void(__cdecl*)(short)) 0x00454970 )
#define ItemNewRoom	( (void(__cdecl*)(short, short)) 0x004547B0 )
#define EffectNewRoom	( (void(__cdecl*)(short, short)) 0x00454A90 )
#define CreateEffect	( (short(__cdecl*)(short)) 0x00454900 )
#define InitialiseFXArray	( (void(__cdecl*)(long)) 0x004548B0 )
