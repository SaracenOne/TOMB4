#include "../tomb4/pch.h"
#include "texture.h"
#include "dxshell.h"
#include "function_stubs.h"
#include "winmain.h"

TEXTURE* Textures;
long nTextures;

#ifndef USE_BGFX
LPDIRECTDRAWSURFACEX CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format)
#else
bgfx::TextureHandle CreateTexturePage(long w, long h, long MipMapCount, long* pSrc, rgbfunc RGBM, long format)
#endif
{
#ifdef USE_BGFX
	bgfx::TextureHandle tSurf;
#else
	DXTEXTUREINFO* tex;
	LPDIRECTDRAWSURFACEX tSurf;
	DDSURFACEDESCX desc;
#endif
	long* lS;
	long* lD;
	short* sS;
	short* sD;
	char* cD;
	ulong c, o, ro, go, bo, ao;
	uchar r, g, b, a;

#ifndef USE_BGFX
	memset(&desc, 0, sizeof(DDSURFACEDESCX));
	desc.dwSize = sizeof(DDSURFACEDESCX);
	desc.dwWidth = w;
	desc.dwHeight = h;
#endif
	uint16_t buffer_width = w;
	uint16_t buffer_height = h;

	if (w < 32 || h < 32)
		MipMapCount = 0;

#ifdef USE_BGFX
	const bgfx::Memory* texture_buffer = bgfx::alloc(((buffer_width * buffer_height) * sizeof(int)));
#else
	desc.ddpfPixelFormat = G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].TextureInfos[G_dxinfo->nTexture].ddpf;
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;

	if (App.dx.Flags & DXF_HWR)
		desc.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	else
		desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	if (MipMapCount)
	{
		desc.dwFlags |= DDSD_MIPMAPCOUNT;
		desc.dwMipMapCount = MipMapCount + 1;
		desc.ddsCaps.dwCaps |= DDSCAPS_COMPLEX | DDSCAPS_MIPMAP;
	}

	DXCreateSurface(App.dx.lpDD, &desc, &tSurf);
	DXAttempt(tSurf->Lock(0, &desc, DDLOCK_NOSYSLOCK, 0));
#endif

	if (!format)
	{
		lS = pSrc;
#ifdef USE_BGFX
		cD = (char*)texture_buffer->data;
#else
		cD = (char*)desc.lpSurface;
#endif

		for (ulong y = 0; y < buffer_height; y++)
		{
			for (ulong x = 0; x < buffer_width; x++)
			{
				c = *(lS + x * 256 / w + y * 0x10000 / h);
				r = CLRR(c);
				g = CLRG(c);
				b = CLRB(c);
				a = CLRA(c);

				if (RGBM)
					RGBM(&r, &g, &b);

#ifndef USE_BGFX
				tex = &G_dxinfo->DDInfo[G_dxinfo->nDD].D3DDevices[G_dxinfo->nD3D].TextureInfos[G_dxinfo->nTexture];
				ro = r >> (8 - tex->rbpp) << (tex->rshift);
				go = g >> (8 - tex->gbpp) << (tex->gshift);
				bo = b >> (8 - tex->bbpp) << (tex->bshift);
				ao = a >> (8 - tex->abpp) << (tex->ashift);
				o = ro | go | bo | ao;
#else
				ro = r;
				go = g << 8;
				bo = b << 16;
				ao = a << 24;
				o = ro | go | bo | ao;
#endif

#ifdef USE_BGFX
				for (int i = 32; i > 0; i -= 8)
#else
				for (int i = tex->bpp; i > 0; i -= 8)
#endif
				{
					*cD++ = (char)o;
					o >>= 8;
				}
			}
		}
	}
	else if (format == 2)
	{
		sS = (short*)pSrc;
#ifdef USE_BGFX
		sD = (short*)texture_buffer->data;
#else
		sD = (short*)desc.lpSurface;
#endif

		for (ulong y = 0; y < buffer_height; y++)
		{
			for (ulong x = 0; x < buffer_width; x++)
				*sD++ = *(sS + x * 256 / w + y * 0x10000 / h);
		}
	}
	else if (format == 1)
	{
		lS = pSrc;

#ifdef USE_BGFX
		lD = (long*)texture_buffer->data;
#else
		lD = (long*)desc.lpSurface;
#endif

		for (ulong y = 0; y < buffer_height; y++)
		{
			for (ulong x = 0; x < buffer_width; x++)
				*lD++ = *(lS + x * 256 / w + y * 0x10000 / h);
		}
	}

#ifdef USE_BGFX
	tSurf = bgfx::createTexture2D(buffer_width, buffer_height, false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, texture_buffer);
#else
	DXAttempt(tSurf->Unlock(0));
#endif

	return tSurf;
}

void FreeTextures()
{
	TEXTURE* tex;

#ifndef USE_BGFX
	DXAttempt(App.dx.lpD3DDevice->SetTexture(0, 0));
	DXAttempt(App.dx.lpD3D->EvictManagedTextures());
#endif

	for (int i = 0; i < nTextures; i++)
	{
		tex = &Textures[i];

#ifdef USE_BGFX
		if (bgfx::isValid(tex->tex))
		{
			bgfx::destroy(tex->tex);
			Log(4, "Released %s @ %i", "Texture", i);
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Texture");
#else
		if (tex->tex)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Texture", tex->tex, tex->tex->Release());
			tex->tex = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Texture");

		if (tex->surface)
		{
			Log(4, "Released %s @ %x - RefCnt = %d", "Surface", tex->surface, tex->surface->Release());
			tex->surface = 0;
		}
		else
			Log(1, "%s Attempt To Release NULL Ptr", "Surface");
#endif

	}

	if (Textures) {
		SYSTEM_FREE(Textures);
		Textures = nullptr;
	}
}
