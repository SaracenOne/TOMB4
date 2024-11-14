#pragma once
#include "../../../global/types.h"

#define MAX_LOCUSTS 64

long GetFreeLocust();
void TriggerLocust(ITEM_INFO* item);
void InitialiseLocustEmitter(short item_number);
void ControlLocustEmitter(short item_number);
void DrawLocusts();
void UpdateLocusts();

extern LOCUST_STRUCT Locusts[MAX_LOCUSTS];
