#pragma once
#include "../global/types.h"

void SetFogColor(long r, long g, long b);
void HWInitialise();
bool _NVisible(GFXTLVERTEX* v0, GFXTLVERTEX* v1, GFXTLVERTEX* v2);
bool _Visible(GFXTLVERTEX* v0, GFXTLVERTEX* v1, GFXTLVERTEX* v2);
void SetCullCW();
void SetCullCCW();
HRESULT HWBeginScene();
HRESULT HWEndScene();
void InitialiseFunctionTable();

extern void (*AddQuadSorted)(GFXTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
extern void (*AddTriSorted)(GFXTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
extern void (*AddQuadZBuffer)(GFXTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
extern void (*AddTriZBuffer)(GFXTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
extern void (*AddLineSorted)(GFXTLVERTEX* v0, GFXTLVERTEX* v1, short drawtype);
extern bool (*IsVisible)(GFXTLVERTEX* v0, GFXTLVERTEX* v1, GFXTLVERTEX* v2);
extern HRESULT (*_BeginScene)();
extern HRESULT (*_EndScene)();

extern GFXTLVERTEX MyVertexBuffer[0x2000];