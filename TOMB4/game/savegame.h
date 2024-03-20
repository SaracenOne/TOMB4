#pragma once
#include "../global/types.h"

const size_t X32_SAVEGAME_MESH_SIZE = 38;
const size_t INTERNAL_SAVEGAME_MESH_SIZE = sizeof(MESH_DATA);

#define MAX_HUB_LEVELS 10

long CheckSumValid(char* buffer);
void sgInitialiseHub(long dont_save_lara);
void SaveLaraData();
void WriteSG(void* pointer, long size);
void ReadSG(void* pointer, long size);
void SaveHubData(long index);
void RestoreLaraData(bool full_save);
void sgRestoreLevel();
void CreateCheckSum();
void sgSaveLevel();
void sgSaveGame();
void sgRestoreGame();
long OpenSaveGame(uchar current_level, long saving);
void SaveLevelData(bool full_save, bool use_full_flipmask);
void RestoreLevelData(bool full_save, bool use_full_flipmask);

extern LEGACY_SAVEGAME_INFO savegame;
