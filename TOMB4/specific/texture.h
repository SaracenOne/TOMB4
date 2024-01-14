#pragma once
#include "../global/types.h"

typedef void(__cdecl* rgbfunc)(uchar*, uchar*, uchar*);
#ifndef USE_BGFX
LPDIRECTDRAWSURFACEX CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format);
#endif
void FreeTextures();

extern TEXTURE* Textures;
extern long nTextures;
