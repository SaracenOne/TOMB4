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

extern LPDIRECT3DVERTEXBUFFER DestVB;
extern WATERTAB WaterTable[22][64];
extern THREAD MainThread;
extern short* clipflags;
extern float vert_wibble_table[32];
extern long SaveCounter;
