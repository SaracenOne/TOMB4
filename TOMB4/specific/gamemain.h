#pragma once
#include "../global/types.h"

void GameClose();
#ifdef USE_SDL
int GameMain(void* ptr);
#else
unsigned int __stdcall GameMain(void* ptr);
#endif
ushort GetRandom(WATERTAB* wt, long lp);
void init_water_table();
bool GameInitialise();
long S_SaveGame(long slot_num);
long S_LoadGame(long slot_num);

#define WATER_TABLE_COUNT 22
#define WATER_TABLE_SIZE 64
#define WIBBLE_TABLE_SIZE 32
#define UNUSED_WIBBLE_TABLE_SIZE 256

#ifndef USE_BGFX
extern LPDIRECT3DVERTEXBUFFER DestVB;
#endif
extern WATERTAB WaterTable[WATER_TABLE_COUNT][WATER_TABLE_SIZE];
extern THREAD MainThread;
extern short* clipflags;
extern float vert_wibble_table[WIBBLE_TABLE_SIZE];
extern long SaveCounter;
