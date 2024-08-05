#pragma once

#include "../../../global/types.h"

void InitialiseGuardian(short item_number);
void TriggerGuardianSparks(GAME_VECTOR* pos, long size, long rgb, long power);
void TriggerBaseLightning(ITEM_INFO* item);
void GuardianControl(short item_number);


struct GUARDIAN_TARGET
{
	long x;
	long y;
	long z;
	LIGHTNING_STRUCT* elptr[2];
	LIGHTNING_STRUCT* blptr[4];
	char ricochet[2];
	char TrackSpeed;
	char TrackLara;
	short Xdiff;
	short Ydiff;
};