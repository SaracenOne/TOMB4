#pragma once
#include "../global/types.h"

GFXMATRIX* D3DIdentityMatrix(GFXMATRIX* mx);
void SetD3DMatrix(GFXMATRIX* mx, float* imx);
void SetD3DViewMatrix();
void D3DTransform(GFXVECTOR* vec, GFXMATRIX* mx);
GFXVECTOR* D3DNormalise(GFXVECTOR* vec);
void S_InitD3DMatrix();
GFXMATRIX*D3DMultMatrix(GFXMATRIX *d, GFXMATRIX *a, GFXMATRIX *b);

extern GFXMATRIX D3DMView;
extern GFXMATRIX D3DLightMatrix;
extern GFXMATRIX D3DInvCameraMatrix;
