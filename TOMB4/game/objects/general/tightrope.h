#pragma once
#include "../../../global/types.h"

extern void lara_as_trpose(ITEM_INFO* item, COLL_INFO* coll);
extern void lara_as_trwalk(ITEM_INFO* item, COLL_INFO* coll);
extern void lara_as_trfall(ITEM_INFO* item, COLL_INFO* coll);

extern void InitialiseTightRope(short item_number);
extern void TightRopeCollision(short item_num, ITEM_INFO* l, COLL_INFO* coll);

struct LARA_TIGHTROPE_EXTRASTATE {
	uchar TightRopeOnCount;
	uchar TightRopeOff;
	uchar TightRopeFall;
};

extern LARA_TIGHTROPE_EXTRASTATE lara_tightrope_extrastate;

void SetupLaraTightropeExtraState();