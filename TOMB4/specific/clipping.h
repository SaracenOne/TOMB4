#pragma once
#include "../global/types.h"

long ZClipper(long n, GFXTLBUMPVERTEX* in, GFXTLBUMPVERTEX* out);
long visible_zclip(GFXTLVERTEX* v0, GFXTLVERTEX* v1, GFXTLVERTEX* v2);
long XYUVGClipper(long n, GFXTLBUMPVERTEX* in);
