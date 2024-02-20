#pragma once
#include "../global/types.h"

typedef void(__cdecl* rgbfunc)(uchar*, uchar*, uchar*);
#ifdef USE_BGFX
bgfx::TextureHandle CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format);
#else
LPDIRECTDRAWSURFACEX CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format);
#endif
void FreeTextures();

extern TEXTURE* Textures;
extern long nTextures;
