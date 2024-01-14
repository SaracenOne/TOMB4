#pragma once
#include "../global/types.h"

void SetFogColor(long r, long g, long b);
void HWInitialise();
bool _NVisible(GFXVERTEX* v0, GFXVERTEX* v1, GFXVERTEX* v2);
bool _Visible(GFXVERTEX* v0, GFXVERTEX* v1, GFXVERTEX* v2);
void SetCullCW();
void SetCullCCW();
HRESULT HWBeginScene();
HRESULT HWEndScene();
void InitialiseFunctionTable();

extern void (*AddQuadSorted)(GFXVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
extern void (*AddTriSorted)(GFXVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
extern void (*AddQuadZBuffer)(GFXVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
extern void (*AddTriZBuffer)(GFXVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
extern void (*AddLineSorted)(GFXVERTEX* v0, GFXVERTEX* v1, short drawtype);
extern bool (*IsVisible)(GFXVERTEX* v0, GFXVERTEX* v1, GFXVERTEX* v2);
extern HRESULT (*_BeginScene)();
extern HRESULT (*_EndScene)();

extern GFXVERTEX MyVertexBuffer[0x2000];
extern long CurrentFog;
