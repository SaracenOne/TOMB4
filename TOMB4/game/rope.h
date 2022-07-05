#pragma once
#include "../global/vars.h"

void inject_rope(bool replace);

void DrawRopeList();
void ProjectRopePoints(ROPE_STRUCT* Rope);
PHD_VECTOR* Normalise(PHD_VECTOR* v);
void vMul(PHD_VECTOR* v, long scale, PHD_VECTOR* d);
long mDotProduct(PHD_VECTOR* a, PHD_VECTOR* b);
