#pragma once

#define MAX_PICKUP_DISPLAYABLE_COUNT 8
#define MAX_PICKUP_DISPLAYABLE_LIFETIME 8

int T4PlusGetInventoryCount(short object_number);
void T4PlusSetInventoryCount(short object_number, int count);
void T4ShowObjectPickup(int object_number);