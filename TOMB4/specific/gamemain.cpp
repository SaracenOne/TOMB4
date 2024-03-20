#include "../tomb4/pch.h"
#include "gamemain.h"
#include "audio.h"
#include "file.h"
#include "function_stubs.h"
#include "function_table.h"
#include "3dmath.h"
#include "../game/text.h"
#include "time.h"
#include "winmain.h"
#include "../game/sound.h"
#include "../game/gameflow.h"
#include "dxshell.h"
#include "../game/savegame.h"
#include "../tomb4/tomb4.h"
#include "platform.h"

#include "specificfx.h"

#include "../game/trep/trepsave.h"

#include "../tomb4/tomb4plus/t4plus_weather.h"
#include "../tomb4/mod_config.h"

#ifdef USE_BGFX
#include "bgfx.h"
#else
LPDIRECT3DVERTEXBUFFER DestVB;
#endif
WATERTAB WaterTable[22][64];
THREAD MainThread;
short* clipflags;
float vert_wibble_table[32];
long SaveCounter;

static float unused_vert_wibble_table[256];
static uchar water_abs[4] = { 4, 8, 12, 16 };
static short water_shimmer[4] = { 31, 63, 95, 127 };
static short water_choppy[4] = { 16, 53, 90, 127 };

void GameClose()
{
	Log(2, "GameClose");
	ACMClose();
	FreeLevel();

	T4PlusCleanup();

#ifndef USE_BGFX
	if (DestVB)
	{
		Log(4, "Released %s @ %x - RefCnt = %d", "Dest VB", DestVB, DestVB->Release());
		DestVB = 0;
	}
	else
		Log(1, "%s Attempt To Release NULL Ptr", "Dest VB");
#endif

	SYSTEM_FREE(clipflags);

	if (wav_file_buffer)
		SYSTEM_FREE(wav_file_buffer);

	if (ADPCMBuffer)
		SYSTEM_FREE(ADPCMBuffer);

	if (logF)
		fclose(logF);

	SYSTEM_FREE(malloc_buffer);
	SYSTEM_FREE(gfScriptFile);
	SYSTEM_FREE(gfLanguageFile);

	SYSTEM_FREE(mesh_mapping_table);

#ifdef USE_BGFX
	ShutdownBGFX();
#endif
}

#ifdef USE_SDL
int GameMain(void* ptr)
#else
unsigned int __stdcall GameMain(void* ptr)
#endif
{
	Log(2, "GameMain");

	if (GameInitialise())
	{
		InitialiseFunctionTable();
		HWInitialise();
		InitWindow(0, 0, App.dx.dwRenderWidth, App.dx.dwRenderHeight, 20, 20480, 80, App.dx.dwRenderWidth, App.dx.dwRenderHeight);
		// T4Plus - Moved init fonts to a per-level basis
		TIME_Init();
		App.SetupComplete = 1;
		S_CDStop();
		ClearSurfaces();

		if (!App.SoundDisabled)
			SOUND_Init();

		RPC_Init();
		init_tomb4_stuff();
		DoGameflow();
		GameClose();
		S_CDStop();

		RPC_close();
		PostMessage(App.hWnd, WM_CLOSE, 0, 0);
		MainThread.active = 0;
		_endthreadex(1);
	}

	return 1;
}

ushort GetRandom(WATERTAB* wt, long lp)
{
	long loop;
	ushort ret;

	do
	{
		ret = rand() & 0xFC;

		for (loop = 0; loop < lp; loop++)
			if (wt[loop].random == ret)
				break;

	} while (loop != lp);

	return ret;
}

void init_water_table()
{
	float fSin;
	long lSin;
	short sSin, angle;

	srand(121197);

	for (int i = 0; i < 64; i++)
	{
		sSin = rcossin_tbl[i << 7];
		WaterTable[0][i].shimmer = (63 * sSin) >> 15;
		WaterTable[0][i].choppy = (16 * sSin) >> 12;
		WaterTable[0][i].random = (uchar)GetRandom(&WaterTable[0][0], i);
		WaterTable[0][i].abs = 0;

		WaterTable[1][i].shimmer = (32 * sSin) >> 15;
		WaterTable[1][i].choppy = 0;
		WaterTable[1][i].random = (uchar)GetRandom(&WaterTable[1][0], i);
		WaterTable[1][i].abs = -3;

		WaterTable[2][i].shimmer = (64 * sSin) >> 15;
		WaterTable[2][i].choppy = 0;
		WaterTable[2][i].random = (uchar)GetRandom(&WaterTable[2][0], i);
		WaterTable[2][i].abs = 0;

		WaterTable[3][i].shimmer = (96 * sSin) >> 15;
		WaterTable[3][i].choppy = 0;
		WaterTable[3][i].random = (uchar)GetRandom(&WaterTable[3][0], i);
		WaterTable[3][i].abs = 4;

		WaterTable[4][i].shimmer = (127 * sSin) >> 15;
		WaterTable[4][i].choppy = 0;
		WaterTable[4][i].random = (uchar)GetRandom(&WaterTable[4][0], i);
		WaterTable[4][i].abs = 8;

		for (int j = 0, k = 5; j < 4; j++, k += 4)
		{
			for (int m = 0; m < 4; m++)
			{
				WaterTable[k + m][i].shimmer = -((sSin * water_shimmer[m]) >> 15);
				WaterTable[k + m][i].choppy = sSin * water_choppy[j] >> 12;
				WaterTable[k + m][i].random = (uchar)GetRandom(&WaterTable[k + m][0], i);
				WaterTable[k + m][i].abs = water_abs[m];
			}
		}
	}

	for (int i = 0; i < 32; i++)
	{
		fSin = sinf(float(i * (M_PI / 16.0F)));
		vert_wibble_table[i] = fSin + fSin;
	}

	for (int i = 0; i < 256; i++)
	{
		angle = 0x10000 * i / 256;
		lSin = phd_sin(angle);
		unused_vert_wibble_table[i] = float(lSin >> (W2V_SHIFT - 5));
	}
}

bool GameInitialise()
{
#ifndef USE_BGFX
	D3DVERTEXBUFFERDESC desc = {};

	desc.dwCaps = 0;
	desc.dwSize = sizeof(desc);
	desc.dwFVF = D3DFVF_TLVERTEX;
	desc.dwNumVertices = 0x2000;
	DXAttempt(App.dx.lpD3D->CreateVertexBuffer(&desc, &DestVB, D3DDP_DONOTCLIP, 0));
#endif
	init_game_malloc();
	reset_virtual_game_malloc_offset();
	clipflags = (short*)SYSTEM_MALLOC(0x4000);
	init_water_table();
	InitWeatherFX(); // TRLE
	return 1;
}

long S_SaveGame(long slot_num)
{
	size_t bytes;
	long days, hours, minutes, seconds;
	char buffer[80], counter[16];

	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "savegame.%d", slot_num);

	std::string full_path = savegame_dir_path + buffer;

	FILE* file = fopen(full_path.c_str(), "wb");

	if (file)
	{
		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, "%s", GetCustomStringForTextID(gfLevelNames[gfCurrentLevel]));

		bytes = fwrite(buffer, sizeof(char), 75, file);
		bytes = fwrite(&SaveCounter, sizeof(long), 1, file);
		days = savegame.Game.Timer / 30 / 86400;
		hours = savegame.Game.Timer / 30 % 86400 / 3600;
		minutes = savegame.Game.Timer / 30 / 60 % 60;
		seconds = savegame.Game.Timer / 30 % 60;

		bytes = fwrite(&days, sizeof(short), 1, file);
		bytes = fwrite(&hours, sizeof(short), 1, file);
		bytes = fwrite(&minutes, sizeof(short), 1, file);
		bytes = fwrite(&seconds, sizeof(short), 1, file);
		bytes = fwrite(&savegame, sizeof(LEGACY_SAVEGAME_INFO), 1, file);

		fclose(file);
		sprintf(counter, "%d", SaveCounter);
		SaveCounter++;

		MOD_GLOBAL_INFO* mod_global_info = get_game_mod_global_info();
		if (mod_global_info->trep_using_extended_saves)
		{
			S_TREPSavegame(slot_num);
		}

		return 1;
	}

	return 0;
}

long S_LoadGame(long slot_num)
{
	long value;
	char buffer[80];

	sprintf(buffer, "savegame.%d", slot_num);

	std::string full_path = savegame_dir_path + buffer;

	FILE* file = platform_fopen(full_path.c_str(), "rb");

	T4PlusLevelReset();

	if (file)
	{
		if (fread(buffer, sizeof(char), 75, file) == 0) {
			fclose(file);
			return 0;
		}
		if (fread(&value, sizeof(long), 1, file) == 0) {
			fclose(file);
			return 0;
		}
		if (fread(&value, sizeof(long), 1, file) == 0) {
			fclose(file);
			return 0;
		}
		if (fread(&value, sizeof(long), 1, file) == 0) {
			fclose(file);
			return 0;
		}
		if (fread(&savegame, sizeof(LEGACY_SAVEGAME_INFO), 1, file) == 0) {
			fclose(file);
			return 0;
		}
		fclose(file);

		MOD_GLOBAL_INFO *mod_global_info = get_game_mod_global_info();
		if (mod_global_info->trep_using_extended_saves)
		{
			S_TREPLoadgame(slot_num);
		}

		return 1;
	}

	return 0;
}
