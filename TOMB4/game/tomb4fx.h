#pragma once
#include "../global/types.h"

LIGHTNING_STRUCT* TriggerLightning(PHD_VECTOR* s, PHD_VECTOR* d, char variation, long rgb, uchar flags, uchar size, uchar segments);
long ExplodingDeath2(short item_number, long mesh_bits, short Flags);
void DrawGunshells();
void TriggerGunSmoke(long x, long y, long z, long xVel, long yVel, long zVel, long notLara, long weaponType, long shade);
void LaraBubbles(ITEM_INFO* item);
void UpdateDrips();
long GetFreeFireSpark();
void TriggerGlobalStaticFlame();
void TriggerGlobalFireFlame();
void keep_those_fires_burning();
void UpdateFireSparks();
void ClearFires();
void AddFire(long x, long y, long z, long size, short room_number, short fade);
void S_DrawFires();
long GetFreeSmokeSpark();
void UpdateSmokeSparks();
void TriggerShatterSmoke(long x, long y, long z);
void DrawLensFlares(ITEM_INFO* item);
void DrawWeaponMissile(ITEM_INFO* item);
long GetFreeGunshell();
void TriggerGunShell(short leftright, short objnum, long weapon);
void UpdateGunShells();
void TriggerSmallSplash(long x, long y, long z, long num);
void TriggerGunflash(SVECTOR* pos);
void SetGunFlash(short weapon);
void DrawGunflashes();
long GetFreeBlood();
void UpdateBlood();
void TriggerBlood(long x, long y, long z, long angle, long num);
long GetFreeBubble();
void CreateBubble(PHD_3DPOS* pos, short room_number, long size, long biggest);
void UpdateBubbles();
long GetFreeDrip();
void TriggerLaraDrips();
long GetFreeShockwave();
void TriggerShockwave(PHD_VECTOR* pos, long InnerOuterRads, long speed, long rgb, long XRotFlags);
void TriggerShockwaveHitEffect(long x, long y, long z, long rgb, short dir, long speed);
void UpdateShockwaves();
void UpdateLightning();
long LSpline(long x, long* knots, long nk);
void CalcLightningSpline(PHD_VECTOR* pos, SVECTOR* dest, LIGHTNING_STRUCT* lptr);
void TriggerLightningGlow(long x, long y, long z, long rgb);
void TriggerFlashSmoke(long x, long y, long z, short room_number);
void S_DrawSparks();
void SetFadeClip(short height, short speed);
void SetFadeClipImmediate(short height); // TRNG
void UpdateFadeClip();
void SetScreenFadeOut(short speed, short back);
void SetScreenFadeIn(short speed);
void Fade();

#define MAX_SNOW_SIZES 32
#define MAX_NODE_OFFSETS 16
#define MAX_LIGHTNING 16
#define MAX_GUNSHELLS 24
#define MAX_DRIPS 32
#define MAX_SMOKE_SPARKS 32
#define MAX_BUBBLES 40
#define MAX_SHOCKWAVES 16
#define MAX_FIRE_SPARKS 20
#define MAX_BLOOD 32
#define MAX_GUN_FLASHES 4
#define MAX_FIRES 32

#define TSV_BUFFER_SIZE 16384

extern float SnowSizes[MAX_SNOW_SIZES]; // TRLE
extern NODEOFFSET_INFO NodeOffsets[MAX_NODE_OFFSETS];
extern LIGHTNING_STRUCT Lightning[MAX_LIGHTNING];
extern GUNSHELL_STRUCT Gunshells[MAX_GUNSHELLS];
extern DRIP_STRUCT Drips[MAX_DRIPS];
extern SMOKE_SPARKS smoke_spark[MAX_SMOKE_SPARKS];
extern BUBBLE_STRUCT Bubbles[MAX_BUBBLES];
extern SHOCKWAVE_STRUCT ShockWaves[MAX_SHOCKWAVES];
extern FIRE_SPARKS fire_spark[MAX_FIRE_SPARKS];
extern BLOOD_STRUCT blood[MAX_BLOOD];
extern GUNFLASH_STRUCT Gunflashes[MAX_GUN_FLASHES];
extern FIRE_LIST fires[MAX_FIRES];
extern long next_fire_spark;
extern long next_smoke_spark;
extern long next_gunshell;
extern long next_bubble;
extern long next_drip;
extern long next_blood;
extern short FlashFadeR;
extern short FlashFadeG;
extern short FlashFadeB;
extern short FlashFader;
extern short ScreenFade;
extern short dScreenFade;
extern short ScreenFadeBack;
extern short ScreenFadedOut;
extern short ScreenFading;
extern short FadeScreenHeight;
extern short DestFadeScreenHeight;
extern short FadeClipSpeed;
extern short ScreenFadeSpeed;
extern char tsv_buffer[16384];
