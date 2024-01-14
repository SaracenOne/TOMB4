#pragma once
#include "../global/types.h"

long ZClipper(long n, GFXBUMPVERTEX* in, GFXBUMPVERTEX* out);
long visible_zclip(GFXVERTEX* v0, GFXVERTEX* v1, GFXVERTEX* v2);
long XYUVGClipper(long n, GFXBUMPVERTEX* in);
