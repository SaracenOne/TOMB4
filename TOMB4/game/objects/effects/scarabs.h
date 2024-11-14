#pragma once
#include "../../../global/types.h"

long GetFreeScarab();
void ClearScarabs();
void TriggerScarab(short item_number);
void UpdateScarabs();
void DrawScarabs();
void InitialiseScarabGenerator(short item_number);

#define MAX_SCARABS 128

extern SCARAB_STRUCT Scarabs[MAX_SCARABS];
