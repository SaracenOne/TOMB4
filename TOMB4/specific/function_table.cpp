#include "../tomb4/pch.h"
#include "function_table.h"
#include "function_stubs.h"
#include "dxshell.h"
#include "polyinsert.h"
#include "3dmath.h"
#include "winmain.h"
#include "bgfx.h"
#include "../game/gameflow.h"

void (*AddQuadSorted)(GFXTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
void (*AddTriSorted)(GFXTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
void (*AddQuadZBuffer)(GFXTLVERTEX* v, short v0, short v1, short v2, short v3, TEXTURESTRUCT* tex, long double_sided);
void (*AddTriZBuffer)(GFXTLVERTEX* v, short v0, short v1, short v2, TEXTURESTRUCT* tex, long double_sided);
void (*AddLineSorted)(GFXTLVERTEX* v0, GFXTLVERTEX* v1, short drawtype);
bool (*IsVisible)(GFXTLVERTEX* v0, GFXTLVERTEX* v1, GFXTLVERTEX* v2);
HRESULT(*_BeginScene)();
HRESULT(*_EndScene)();

GFXTLVERTEX MyVertexBuffer[0x2000];

void SetDistanceFogColor(long r, long g, long b)
{
	r &= 0xFF;
	g &= 0xFF;
	b &= 0xFF;
	gfDistanceFog.r = r;
	gfDistanceFog.g = g;
	gfDistanceFog.b = b;
	gfDistanceFog.a = 0;
#ifndef USE_BGFX
	long CurrentFog = RGBA(r, g, b, 0xFF);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, CurrentFog);
#else
	bgfx_fog_color[0] = (float)r / 255.0f;
	bgfx_fog_color[1] = (float)g / 255.0f;
	bgfx_fog_color[2] = (float)b / 255.0f;
	bgfx_fog_color[3] = 1.0f;
#endif
}

void SetVolumetricFogColor(long r, long g, long b)
{
	r &= 0xFF;
	g &= 0xFF;
	b &= 0xFF;

	gfVolumetricFog.r = r;
	gfVolumetricFog.g = g;
	gfVolumetricFog.b = b;
	gfVolumetricFog.a = 0;
#ifndef USE_BGFX
	if (IsVolumetric()) {
		SetDistanceFogColor(r, g, b);
	}
#else
	bgfx_volumetric_fog_color[0] = (float)r / 255.0f;
	bgfx_volumetric_fog_color[1] = (float)g / 255.0f;
	bgfx_volumetric_fog_color[2] = (float)b / 255.0f;
	bgfx_volumetric_fog_color[3] = 1.0f;
#endif
}

void HWInitialise()
{
	Log(2, "HWIntialise");	//nice typo
#ifdef USE_BGFX
	InitializeBGFX();
#else
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(3, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(4, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(5, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(6, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(7, D3DTSS_COLOROP, D3DTOP_DISABLE);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);

	if (App.Filtering)
	{
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
	}
	else
	{
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
		App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_POINT);
	}

	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	App.dx.lpD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);

	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, 1);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

	DXAttempt(App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE));
	DXAttempt(App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, 1));
	DXAttempt(App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, 1));

	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, 1);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF, 0);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_NOTEQUAL);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, 0);

	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_AMBIENT, 0));
	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_COLORVERTEX, 0));
	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_COLORMODEL, D3DCOLOR_RGB));

	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_FOGMODE, D3DFOG_LINEAR));
	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_FOGSTART, *(DWORD*)(&LevelFogStart)));
	DXAttempt(App.dx.lpD3DDevice->SetLightState(D3DLIGHTSTATE_FOGEND, *(DWORD*)(&LevelFogEnd)));
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR, 0xFF000000);
	App.dx.lpD3DDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE, 1);
#endif
}

bool _NVisible(GFXTLVERTEX* v0, GFXTLVERTEX* v1, GFXTLVERTEX* v2)
{
	return (v0->sy - v1->sy) * (v2->sx - v1->sx) - (v2->sy - v1->sy) * (v0->sx - v1->sx) < 0;
}

bool _Visible(GFXTLVERTEX* v0, GFXTLVERTEX* v1, GFXTLVERTEX* v2)
{
	return (v0->sy - v1->sy) * (v2->sx - v1->sx) - (v2->sy - v1->sy) * (v0->sx - v1->sx) > 0;
}

void SetCullCW()
{
	IsVisible = _Visible;
}

void SetCullCCW()
{
	IsVisible = _NVisible;
}

HRESULT HWBeginScene()
{
	if (App.dx.InScene)
		Log(1, "Already In Scene");

	App.dx.InScene = 1;
	App.dx.DoneBlit = 0;
	while (App.dx.WaitAtBeginScene) {};
#ifdef USE_BGFX
	StartBGFXFrame();
	return 1;
#else
	return App.dx.lpD3DDevice->BeginScene();
#endif
}

HRESULT HWEndScene()
{
	App.dx.InScene = 0;
#ifdef USE_BGFX
	EndBGFXFrame();
	return 1;
#else
	return App.dx.lpD3DDevice->EndScene();
#endif
}

void InitialiseFunctionTable()
{
	_BeginScene = HWBeginScene;
	_EndScene = HWEndScene;
	IsVisible = _NVisible;

#ifndef USE_BGFX
	if (App.dx.lpZBuffer)
#endif
	{
		AddQuadZBuffer = AddQuadClippedZBuffer;
		AddTriZBuffer = AddTriClippedZBuffer;
		AddQuadSorted = AddQuadClippedSorted;
		AddTriSorted = AddTriClippedSorted;
	}
#ifndef USE_BGFX
	else
	{
		AddQuadZBuffer = AddQuadSubdivide;
		AddTriZBuffer = AddTriSubdivide;
		AddQuadSorted = AddQuadSubdivide;
		AddTriSorted = AddTriSubdivide;
	}
#endif

	AddLineSorted = AddLineClippedSorted;
}
