#pragma once

#define MAX_PICKUP_DISPLAYABLE_LIFETIME 8

void T4PlusSetValidLaraGunType();

int T4PlusGetInventoryCount(short object_number);
void T4PlusSetInventoryCount(short object_number, int count, bool update_weapon_state);
void T4ShowObjectPickup(int object_number);