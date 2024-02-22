#pragma once
#include "../global/types.h"

#define MAX_WRAITH_DATA 8

void InitialiseWraith(short item_number);
void TriggerWraithFlame(long x, long y, long z, short xv, short yv, short zv, long objnum);
void TriggerWraithEffect(long x, long y, long z, short vel, long objnum);
void WraithControl(short item_number);
