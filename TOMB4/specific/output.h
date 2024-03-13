#pragma once
#include "../global/types.h"

__forceinline void CalculateVertexSpecular(
	FVECTOR vPos,
	float DistanceFogStart,
	float DistanceFogEnd,
	long* cR,
	long* cG,
	long* cB,
	long* sR,
	long* sG,
	long* sB,
	long* sA);

void ProcessObjectMeshVertices(MESH_DATA* mesh);
void ProcessStaticMeshVertices(MESH_DATA* mesh);
void ProcessTrainMeshVertices(MESH_DATA* mesh);
void ProcessPickupMeshVertices(MESH_DATA* mesh);
void phd_PutPolygons(short* objptr, long clip);
void phd_PutPolygons_train(short* objptr, long x);
void _InsertRoom(ROOM_INFO* r);
void RenderLoadPic(long unused);
void S_InitialisePolyList();
void phd_PutPolygonsPickup(short* objptr, float x, float y, long color);
void phd_PutPolygonSkyMesh(short* objptr, long clipstatus);
void S_DrawPickup(short object_number);
long S_GetObjectBounds(short* bounds);
#ifndef USE_BGFX
HRESULT DDCopyBitmap(LPDIRECTDRAWSURFACEX surf, HBITMAP hbm, long x, long y, long dx, long dy);
HRESULT _LoadBitmap(LPDIRECTDRAWSURFACEX surf, LPCSTR name);
#endif
void do_boot_screen(long language);
void S_AnimateTextures(long n);
long S_DumpScreen();
void S_OutputPolyList();
void StashSkinVertices(long node);
void SkinVerticesToScratch(long node);
long GetRenderScale(long unit);
long GetFixedScale(long unit);

extern long GlobalAlpha;
extern long GlobalAmbient;

extern float AnimatingTexturesV[16][8][3];
