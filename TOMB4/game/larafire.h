#pragma once
#include "../global/vars.h"

void inject_larafire(bool replace);

void InitialiseNewWeapon();
void LaraTargetInfo(WEAPON_INFO* winfo);
short* get_current_ammo_pointer(long weapon_type);
long FireWeapon(long weapon_type, ITEM_INFO* target, ITEM_INFO* src, short* angles);
void AimWeapon(WEAPON_INFO* winfo, LARA_ARM* arm);
void LaraGetNewTarget(WEAPON_INFO* winfo);

#define WeaponObject	( (long(__cdecl*)(long)) 0x0042EA70 )
#define WeaponObjectMesh	( (long(__cdecl*)(long)) 0x0042EAC0 )
#define LaraGun	( (void(__cdecl*)()) 0x0042D840 )
#define HitTarget	( (void(__cdecl*)(ITEM_INFO*, GAME_VECTOR*, long, long)) 0x0042E920 )
#define DoProperDetection	( (void(__cdecl*)(short, long, long, long, long, long, long)) 0x0042EB30 )
