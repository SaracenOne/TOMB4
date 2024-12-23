#pragma once
#include "../global/types.h"

#define MAX_SEQUENCES 3
#define MAX_USED_SEQUENCES 6

void FullBlockSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
long SwitchTrigger(short item_number, short timer);
long GetSwitchTrigger(ITEM_INFO* item, short* ItemNos, long AttatchedToSwitch);
void TestTriggersAtXYZ(long x, long y, long z, short room_number, bool heavy, short flags);
void SwitchControl(short item_number);
void SwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void SwitchCollision2(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void SwitchType78Collision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void UnderwaterSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void PulleyCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void TurnSwitchControl(short item_number);
void TurnSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void RailSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void JumpSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void CrowbarSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);
void FullBlockSwitchControl(short item_number);
void CogSwitchControl(short item_number);
void CogSwitchCollision(short item_number, ITEM_INFO* l, COLL_INFO* coll);

extern PHD_VECTOR OldPickupPos;
extern uchar CurrentSequence;
extern uchar Sequences[MAX_SEQUENCES];
extern uchar SequenceUsed[MAX_USED_SEQUENCES];
extern uchar SequenceResults[MAX_SEQUENCES][MAX_SEQUENCES][MAX_SEQUENCES];
