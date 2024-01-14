#pragma once
#include "../global/types.h"

void HWR_DrawSortList(GFXBUMPVERTEX* info, short num_verts, short texture, short type);
void DrawSortList();
void CreateFogPos(FOGBULB_STRUCT* FogBulb);
void ControlFXBulb(FOGBULB_STRUCT* FogBulb);
void CreateFXBulbs();
void ClearFXFogBulbs();
void TriggerFXFogBulb(long x, long y, long z, long FXRad, long density, long r, long g, long b, long room_number);
long IsVolumetric();
int DistCompare(const void* a, const void* b);
void InitialiseFogBulbs();
void OmniEffect(GFXVERTEX* v);
void OmniFog(GFXVERTEX* v);
void AddTriClippedSorted(GFXVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
void AddQuadClippedSorted(GFXVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
void AddLineClippedSorted(GFXVERTEX* v0, GFXVERTEX* v1, short drawtype);
void InitialiseSortList();
void DoSort(long left, long right, SORTLIST** list);
void SortPolyList(long count, SORTLIST** list);
void mD3DTransform(FVECTOR* vec, GFXMATRIX* mx);
void AddClippedPoly(GFXBUMPVERTEX* dest, long nPoints, GFXBUMPVERTEX* v, TEXTURESTRUCT* pTex);
void AddTriClippedZBuffer(GFXVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
void AddQuadClippedZBuffer(GFXVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
void SubdivideEdge(GFXVERTEX* v0, GFXVERTEX* v1, GFXVERTEX* v, short* c, float tu1, float tv1, float tu2, float tv2, float* tu, float* tv);
void SubdivideQuad(GFXVERTEX* v0, GFXVERTEX* v1, GFXVERTEX* v2, GFXVERTEX* v3, TEXTURESTRUCT* tex, long double_sided, long steps, short* c);
void SubdivideTri(GFXVERTEX* v0, GFXVERTEX* v1, GFXVERTEX* v2, TEXTURESTRUCT* tex, long double_sided, long steps, short* c);
void AddTriSubdivide(GFXVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
void AddQuadSubdivide(GFXVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
void CalcColorSplit(GFXCOLOR s, GFXCOLOR* r);

extern GFXBUMPVERTEX XYUVClipperBuffer[20];
extern GFXBUMPVERTEX zClipperBuffer[20];

extern FOGBULB_STRUCT FogBulbs[20];
extern long NumLevelFogBulbs;

extern long nPolys;
extern long nClippedPolys;
extern long DrawPrimitiveCnt;

extern SORTLIST* SortList[262144]; // TRLE: increased size (x16)
extern long SortCount;
