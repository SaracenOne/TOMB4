#pragma once
#include "../global/types.h"

enum LaraHairUpdateType {
	LARA_HAIR_UPDATE_TYPE_BRAID,
	LARA_HAIR_UPDATE_TYPE_PIGTAILS_LEFT,
	LARA_HAIR_UPDATE_TYPE_PIGTAILS_RIGHT,
};

void InitialiseHair();
void HairControl(bool in_cutscene, LaraHairUpdateType lara_hair_update_type, short* cutscenething);
void GetCorrectStashPoints(long pigtail, long hair_node, long skin_node);
void DrawHair();

extern HAIR_STRUCT hairs[2][7];
