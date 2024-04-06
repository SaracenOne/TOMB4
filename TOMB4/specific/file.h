#pragma once
#include "../global/types.h"

#ifdef USE_SDL
int LoadLevel(void* name);
#else
unsigned int __stdcall LoadLevel(void* name);
#endif
long S_LoadLevelFile(long num);
void FreeLevel();
FILE* FileOpen(const char* name);
void FileClose(FILE* file);
long FileSize(FILE* file);
size_t LoadFile(const char* name, char** dest);
bool LoadTextures(long RTPages, long OTPages, long BTPages);
bool LoadRooms();
bool LoadObjects();
bool LoadSprites();
bool LoadCameras();
bool LoadSoundEffects();
bool LoadBoxes();
bool LoadAnimatedTextures();
bool LoadTextureInfos();
bool LoadItems();
bool LoadCinematic();
bool LoadAIInfo();
bool LoadSamples();
void S_GetUVRotateTextures();
void AdjustUV(long num);
bool Decompress(char* pDest, char* pCompressed, long compressedSize, long size);

extern TEXTURESTRUCT* textinfo;
extern SPRITESTRUCT* spriteinfo;
extern THREAD LevelLoadingThread;

extern TEXTURESTRUCT* AnimatingWaterfalls[3];
extern long AnimatingWaterfallsV[3];

extern int32_t num_meshes;
extern int32_t num_anims;

extern CHANGE_STRUCT* changes;
extern RANGE_STRUCT* ranges;
extern AIOBJECT* AIObjects;
extern short* aranges;
extern short* frames;
extern short* commands;
extern short* floor_data;
extern short* mesh_base;
extern long nAnimUVRanges;
extern long number_cameras;
extern short nAIObjects;

// T4Plus: Helper table for mapping mesh_ptrs between 64-bit and 32-bit offsets
extern size_t mesh_mapping_table_count;
extern MESH_MAP_TABLE_ENTRY* mesh_mapping_table;