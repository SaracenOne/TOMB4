#pragma once
#include "../../../global/types.h"

void TriggerCrocgodMissile(PHD_3DPOS* pos, short room_number, short num);
void TriggerCrocgodMissileFlame(short fx_number, long xv, long yv, long zv);
void InitialiseCrocgod(short item_number);
void CrocgodControl(short item_number);
