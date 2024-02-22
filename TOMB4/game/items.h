#pragma once
#include "../global/types.h"

void InitialiseItemArray(size_t num);
void KillItem(size_t item_num);
short CreateItem();
void InitialiseItem(size_t item_num);
void RemoveActiveItem(size_t item_num);
void RemoveDrawnItem(size_t item_num);
void AddActiveItem(size_t item_num);
void ItemNewRoom(size_t item_num, short room_num);
void InitialiseFXArray(size_t allocmem);
short CreateEffect(short room_num);
void KillEffect(short fx_num);
void EffectNewRoom(short fx_num, short room_num);

extern short next_fx_active;
extern short next_item_active;
