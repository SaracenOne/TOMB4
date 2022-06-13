#pragma once
#include "types.h"
#include "math_tbls.h"

#define VAR_U_(address, type)			(*(type*)(address))
#define ARRAY_(address, type, length)	(*(type(*)length)(address))

#define lara	VAR_U_(0x0080DFE0, LARA_INFO)
#define lara_item	VAR_U_(0x0080E13C, ITEM_INFO*)
#define mycoll	VAR_U_(0x0080DEC0, COLL_INFO)
#define lara_coll	VAR_U_(0x004AD9C4, COLL_INFO*)
#define input	VAR_U_(0x00535704, long)
#define linput	VAR_U_(0x005358A8, long)
#define inputBusy	VAR_U_(0x00535600, long)
#define bTrackCamInit	VAR_U_(0x004BF6E4, long)
#define bUseSpotCam	VAR_U_(0x004BF6EC, long)
#define SetDebounce	VAR_U_(0x007FE160, long)
#define framecount	VAR_U_(0x004BF420, long)
#define GlobalCounter	VAR_U_(0x004BF3FA, short)
#define cdtrack	VAR_U_(0x004AEC00, short)
#define bDisableLaraControl	VAR_U_(0x004BF6F0, long)
#define gfCurrentLevel	VAR_U_(0x007FD290, char)
#define gfLevelComplete	VAR_U_(0x007FD2B0, char)
#define dbinput	VAR_U_(0x005358AC, long)
#define cutseq_trig	VAR_U_(0x004BFC7C, long)
#define cutseq_num	VAR_U_(0x004BFC78, long)
#define GLOBAL_enterinventory	VAR_U_(0x004AE068, long)
#define GLOBAL_inventoryitemchosen	VAR_U_(0x004AE06C, long)
#define GLOBAL_lastinvitem	VAR_U_(0x004AE064, long)
#define reset_flag	VAR_U_(0x004BF414, long)
#define Gameflow	VAR_U_(0x007FD278, GAMEFLOW*)
#define FadeScreenHeight	VAR_U_(0x004BF380, short)
#define gfGameMode	VAR_U_(0x004AF451, char)
#define BinocularRange	VAR_U_(0x004BF3E4, long)
#define ExittingBinos	VAR_U_(0x004BF3D4, long)
#define BinocularOldCamera	VAR_U_(0x004BF3EC, camera_type)
#define LaserSight	VAR_U_(0x004BF3F0, long)
#define camera	VAR_U_(0x007FE820, CAMERA_INFO)
#define BinocularOn	VAR_U_(0x004BF3E8, long)
#define items	VAR_U_(0x007FE28C, ITEM_INFO*)
#define GotLaraSpheres	VAR_U_(0x007F6EDE, char)
#define InItemControlLoop	VAR_U_(0x004BF3F5, char)
#define next_item_active	VAR_U_(0x007FD20A, short)
#define KillEverythingFlag	VAR_U_(0x004BF364, long)
#define wibble	VAR_U_(0x004BF360, long)
#define SplashCount	VAR_U_(0x004BF36C, long)
#define WeaponDelay	VAR_U_(0x004BF3F4, long)
#define SmokeCountL	VAR_U_(0x00804E68, long)
#define SmokeCountR	VAR_U_(0x00804E6C, long)
#define SmokeWeapon	VAR_U_(0x00804F20, long)
#define XSoff1	VAR_U_(0x00753C6C, short)
#define XSoff2	VAR_U_(0x00753C68, short)
#define YSoff1	VAR_U_(0x00753C7C, short)
#define YSoff2	VAR_U_(0x00753C80, short)
#define	ZSoff1	VAR_U_(0x00753CAC, short)
#define ZSoff2	VAR_U_(0x00753CB0, short)
#define GLOBAL_playing_cutseq	VAR_U_(0x004BFC80, long)
#define PoisonFlag	VAR_U_(0x007FD214, char)
#define gfLevelFlags	VAR_U_(0x007FD260, short)
#define CamRot	VAR_U_(0x007E71C0, PHD_VECTOR)
#define health_bar_timer	VAR_U_(0x004BF680, long)
#define GameTimer	VAR_U_(0x007FD258, ulong)
#define next_fx_active	VAR_U_(0x008012F0, short)
#define effects	VAR_U_(0x008012E0, FX_INFO*)
#define trigger_index	VAR_U_(0x007FE248, short*)
#define SmashedMeshCount	VAR_U_(0x004BF3FE, short)
#define meshes	VAR_U_(0x00533A80, short**)
#define bones	VAR_U_(0x00533A88, long*)
#define anims	VAR_U_(0x00533A68, ANIM_STRUCT*)
#define cheat_hit_points	VAR_U_(0x0080DEB8, short)
#define savegame	VAR_U_(0x007F76C0, SAVEGAME_INFO)
#define ForcedFixedCamera	VAR_U_(0x007FE890, GAME_VECTOR)
#define UseForcedFixedCamera	VAR_U_(0x007FE8C0, char)
#define room	VAR_U_(0x00533A64, ROOM_INFO*)
#define GlobalBikeItem	VAR_U_(0x007F504C, ITEM_INFO*)
#define phd_winwidth	VAR_U_(0x00753CF8, long)
#define phd_winheight	VAR_U_(0x00753C48, long)
#define bLaraTorch	VAR_U_(0x00536F10, long)
#define phd_mxptr	VAR_U_(0x007E71D8, long*)
#define GlobalAmbient	VAR_U_(0x005C047C, long)
#define height_type	VAR_U_(0x007FE290, long)
#define bikefspeed	VAR_U_(0x004BFC08, long)
#define dont_exit_bike	VAR_U_(0x004BFC24, char)
#define DashTimer	VAR_U_(0x0080E13A, short)
#define font_height	VAR_U_(0x007F5050, long)
#define small_font	VAR_U_(0x007F5054, long)
#define App	VAR_U_(0x00753980, WINAPP)
#define spriteinfo	VAR_U_(0x00533AC4, SPRITESTRUCT*)
#define current_item    VAR_U_(0x005B74B0, ITEM_INFO*)
#define DestVB  VAR_U_(0x00753910, LPDIRECT3DVERTEXBUFFER)
#define f_left   VAR_U_(0x00753CB8, float)
#define f_top   VAR_U_(0x00753CFC, float)
#define f_right   VAR_U_(0x00753C44, float)
#define f_bottom   VAR_U_(0x007540C8, float)
#define clip_left   VAR_U_(0x0052AA38, float)
#define clip_top   VAR_U_(0x0052AA34, float)
#define clip_right   VAR_U_(0x0052AA30, float)
#define clip_bottom   VAR_U_(0x0052A34C, float)
#define bWaterEffect    VAR_U_(0x0052AA3C, long)
#define clipflags   VAR_U_(0x0075397C, short*)
#define textinfo	VAR_U_(0x00533AC0, TEXTURESTRUCT*)
#define D3DMView	VAR_U_(0x00753B90, D3DMATRIX)
#define D3DMWorld	VAR_U_(0x00753BE0, D3DMATRIX)
#define D3DMProjection	VAR_U_(0x00753918, D3DMATRIX)
#define GlobalAlpha	VAR_U_(0x004AD7D4, long)
#define level_items	VAR_U_(0x004BF68C, long)
#define FlashFadeR	VAR_U_(0x004BF386, short)
#define FlashFadeG	VAR_U_(0x004BF388, short)
#define FlashFadeB	VAR_U_(0x004BF38A, short)
#define FlashFader	VAR_U_(0x004BF38C, short)
#define aranges	VAR_U_(0x00753BDC, short*)
#define f_mznear	VAR_U_(0x00753C3C, float)
#define f_mzfar	VAR_U_(0x007540CC, float)
#define f_mpersp	VAR_U_(0x00753C40, float)
#define f_zfar	VAR_U_(0x00753C90, float)
#define f_centerx	VAR_U_(0x00753C94, float)
#define f_centery	VAR_U_(0x00753C98, float)
#define f_moneopersp	VAR_U_(0x00753CBC, float)
#define f_persp	VAR_U_(0x00753CC4, float)
#define f_oneopersp	VAR_U_(0x00753C64, float)
#define one	VAR_U_(0x004B4678, float)
#define mone	VAR_U_(0x004B467C, float)
#define f_znear	VAR_U_(0x00753CB4, float)
#define f_perspoznear	VAR_U_(0x00753CA0, float)
#define LfAspectCorrection	VAR_U_(0x007540C4, float)
#define f_mperspoznear	VAR_U_(0x00753D00, float)
#define FogStart	VAR_U_(0x004B4670, float)
#define FogEnd	VAR_U_(0x004B4674, float)
#define StaticMeshShade	VAR_U_(0x00536EF4, long)
#define water_color_R	VAR_U_(0x004B25D0, long)
#define water_color_G	VAR_U_(0x004B25D4, long)
#define water_color_B	VAR_U_(0x004B25D8, long)
#define current_room	VAR_U_(0x007FDF6C, long)
#define StaticMeshLightItem	VAR_U_(0x005358F0, ITEM_INFO)
#define D3DLights	VAR_U_(0x00536F08, D3DLIGHT_STRUCT*)
#define D3DDynamics	VAR_U_(0x00536F0C, D3DLIGHT_STRUCT*)
#define MaxRoomLights	VAR_U_(0x00536F04, long)
#define LaraTorchStart	VAR_U_(0x00536EF8, PHD_VECTOR)
#define LaraTorchEnd	VAR_U_(0x005358D0, PHD_VECTOR)
#define LaraTorchIntensity	VAR_U_(0x00536EE8, long)
#define LaraTorchYRot	VAR_U_(0x00536EEC, long)
#define SetupLight_thing	VAR_U_(0x00536EF0, long)
#define gfUVRotate	VAR_U_(0x007FD2E9, char)
#define trainmappos	VAR_U_(0x007FD2F0, long)
#define gfStringOffset	VAR_U_(0x007FD274, ushort*)
#define gfStringWad	VAR_U_(0x007FD2B8, char*)
#define gfMirrorRoom	VAR_U_(0x007FD250, uchar)
#define gfMirrorZPlane	VAR_U_(0x007FDF64, long)
#define ScreenFading	VAR_U_(0x004BF37C, short)
#define bDoCredits	VAR_U_(0x004BF669, char)
#define last_target	VAR_U_(0x007FE8A0, GAME_VECTOR)
#define bLaraInWater	VAR_U_(0x0080ECD0, char)
#define bLaraUnderWater	VAR_U_(0x00536F14, long)
#define MusicVolume	VAR_U_(0x004B38AC, long)
#define SFXVolume	VAR_U_(0x004B38B0, long)
#define SoundQuality	VAR_U_(0x004B38B4, long)
#define phd_centerx	VAR_U_(0x00753C4C, long)
#define phd_centery	VAR_U_(0x00753C50, long)
#define ControlMethod	VAR_U_(0x005B7494, long)
#define num_fmvs	VAR_U_(0x004BF65D, char)
#define gfScriptOffset	VAR_U_(0x004BF64C, ushort*)
#define gfScriptWad	VAR_U_(0x004BF658, uchar*)
#define gfStatus	VAR_U_(0x004BF650, long)
#define gfLegendTime	VAR_U_(0x004BF668, uchar)
#define gfNumMips	VAR_U_(0x004BF66B, uchar)
#define gfFog	VAR_U_(0x004BF664, CVECTOR)
#define gfInitialiseGame	VAR_U_(0x004AF452, uchar)
#define gfResetHubDest	VAR_U_(0x007FD288, uchar)
#define skipped_level	VAR_U_(0x004BF3CE, char)
#define gfCutNumber	VAR_U_(0x004BF66A, uchar)
#define	gfLayer1Vel	VAR_U_(0x007FD252, char)
#define	gfLayer2Vel	VAR_U_(0x007FD251, char)
#define gfLayer1Col	VAR_U_(0x007FD264, CVECTOR)
#define gfLayer2Col	VAR_U_(0x007FD25C, CVECTOR)
#define gfLegend	VAR_U_(0x007FD2E8, uchar)
#define gfLensFlare	VAR_U_(0x007FD2A0, PHD_VECTOR)
#define gfLensFlareColour	VAR_U_(0x007FD2B4, CVECTOR)
#define load_cam	VAR_U_(0x00533AA8, PHD_VECTOR)
#define load_target	VAR_U_(0x00533AB4, PHD_VECTOR)
#define load_roomnum	VAR_U_(0x004B27A5, uchar)
#define phd_top	VAR_U_(0x00753C30, long)
#define phd_left	VAR_U_(0x00753C78, long)
#define phd_bottom	VAR_U_(0x00753C70, long)
#define phd_right	VAR_U_(0x007540D0, long)
#define phd_winymax	VAR_U_(0x00753CA8, short)
#define phd_winxmax	VAR_U_(0x00753C84, short)
#define phd_winymin	VAR_U_(0x00753C54, short)
#define phd_winxmin	VAR_U_(0x00753C9C, short)
#define phd_znear	VAR_U_(0x00753C88, long)
#define phd_zfar	VAR_U_(0x00753C60, long)
#define GLaraShadowframe	VAR_U_(0x0080E710, short*)
#define CamPos	VAR_U_(0x007E71B0, PHD_VECTOR)
#define in_joints	VAR_U_(0x0080E500, long)
#define cutseq_busy_timeout	VAR_U_(0x004BFC84, char)
#define	ScreenFadedOut	VAR_U_(0x004BF37A, short)
#define numnailed	VAR_U_(0x007E71E8, long)
#define GLOBAL_oldcamtype	VAR_U_(0x007E722C, camera_type)
#define GLOBAL_cutme	VAR_U_(0x007E7230, NEW_CUTSCENE*)
#define CurrentAtmosphere	VAR_U_(0x007FE21C, uchar)
#define IsAtmospherePlaying	VAR_U_(0x007FE2E4, uchar)
#define gfRequiredStartPos	VAR_U_(0x007FD270, uchar)
#define ScreenFade	VAR_U_(0x004BF374, short)
#define dScreenFade	VAR_U_(0x004BF376, short)
#define ScreenFadeBack	VAR_U_(0x004BF378, short)
#define ScreenFadeSpeed	VAR_U_(0x004ADF58, short)
#define camera_pnodes	VAR_U_(0x007F4F20, PACKNODE*)
#define GLOBAL_cutseq_frame	VAR_U_(0x007F4FD4, long)
#define IsRoomOutsideNo	VAR_U_(0x007FE7E2, short)
#define GLOBAL_numcutseq_frames	VAR_U_(0x007F4FD0, long)
#define LightningCount	VAR_U_(0x007FDF68, short)
#define dLightningRand	VAR_U_(0x007FE14C, short)
#define LightningRand	VAR_U_(0x007FDF5C, short)
#define LightningSFXDelay	VAR_U_(0x004BF624, short)
#define old_lara_holster	VAR_U_(0x007F4FB0, short)
#define lara_chat_cnt	VAR_U_(0x004BFC85, char)
#define actor_chat_cnt	VAR_U_(0x004BFC86, char)
#define SmokeWindZ	VAR_U_(0x00804E64, long)
#define SmokeWindX	VAR_U_(0x00804E60, long)
#define hair_wind	VAR_U_(0x004BF33C, long)
#define hair_dwind_angle	VAR_U_(0x004AD7F8, long)
#define hair_wind_angle	VAR_U_(0x004AD7F4, long)
#define loadbar_pos	VAR_U_(0x00577488, float)
#define loadbar_maxpos	VAR_U_(0x00577458, long)
#define SortCount	VAR_U_(0x00660C88, long)
#define FPCW	VAR_U_(0x0053551C, short)
#define nPolyType	VAR_U_(0x007E71E0, long)
#define f_moneoznear	VAR_U_(0x00753CC0, float)
#define IM_rate	VAR_U_(0x007FE120, long)
#define IM_frac	VAR_U_(0x007FDF54, long)
#define IMptr	VAR_U_(0x007FE124, long*)
#define horus_item_thing	VAR_U_(0x007F4FA8, ITEM_INFO*)
#define compass_settle_thang	VAR_U_(0x007FEA50, long)
#define examine_mode	VAR_U_(0x004BF3CC, short)
#define stats_mode	VAR_U_(0x007FEA18, short)
#define left_debounce	VAR_U_(0x007FE9C9, char)
#define right_debounce	VAR_U_(0x007FEA38, char)
#define up_debounce	VAR_U_(0x007FEA4C, char)
#define down_debounce	VAR_U_(0x007FE9F8, char)
#define go_left	VAR_U_(0x007FE9C1, uchar)
#define go_right	VAR_U_(0x007FE9CA, uchar)
#define go_up	VAR_U_(0x007FEA45, uchar)
#define go_down	VAR_U_(0x007FEA4B, uchar)
#define select_debounce	VAR_U_(0x007FEA43, uchar)
#define deselect_debounce	VAR_U_(0x007FEA12, uchar)
#define go_select	VAR_U_(0x007FF022, uchar)
#define go_deselect	VAR_U_(0x007FEA54, uchar)
#define left_repeat	VAR_U_(0x007FF021, uchar)
#define right_repeat	VAR_U_(0x007FEA39, uchar)
#define loading_or_saving	VAR_U_(0x007FE9CB, char)
#define use_the_bitch	VAR_U_(0x007FEA44, char)
#define AmountPistolsAmmo	VAR_U_(0x004BF3C0, short)
#define AmountRevolverAmmo	VAR_U_(0x004BF3BE, short)
#define AmountCrossBowAmmo1	VAR_U_(0x004BF3B6, short)
#define AmountCrossBowAmmo2	VAR_U_(0x004BF3B4, short)
#define AmountCrossBowAmmo3	VAR_U_(0x004BF3B8, short)
#define AmountGrenadeAmmo1	VAR_U_(0x004BF3AA, short)
#define AmountGrenadeAmmo2	VAR_U_(0x004BF3A6, short)
#define AmountGrenadeAmmo3	VAR_U_(0x004BF3A8, short)
#define AmountShotGunAmmo1	VAR_U_(0x004BF3AE, short)
#define AmountShotGunAmmo2	VAR_U_(0x004BF3AC, short)
#define AmountUziAmmo	VAR_U_(0x004BF3C4, short)
#define ammo_selector_flag	VAR_U_(0x007FEA4A, char)
#define ammo_selector_fade_val	VAR_U_(0x007FEA2A, short)
#define ammo_selector_fade_dir	VAR_U_(0x007FEA48, short)
#define combine_ring_fade_val	VAR_U_(0x007FEA16, short)
#define combine_ring_fade_dir	VAR_U_(0x007FEA40, short)
#define normal_ring_fade_val	VAR_U_(0x007FEA28, short)
#define normal_ring_fade_dir	VAR_U_(0x007FEA46, short)
#define combine_type_flag	VAR_U_(0x007FF020, char)
#define seperate_type_flag	VAR_U_(0x007FEA1A, char)
#define combine_obj1	VAR_U_(0x007FEA34, long)
#define combine_obj2	VAR_U_(0x007FEA30, long)
#define friggrimmer	VAR_U_(0x007FEA14, char)
#define friggrimmer2	VAR_U_(0x007FEA13, char)
#define pcbright	VAR_U_(0x004AE060, long)
#define xoffset	VAR_U_(0x007FE9CC, long)
#define yoffset	VAR_U_(0x007FEA2C, long)
#define current_ammo_type	VAR_U_(0x007FEA3C, char*)
#define CurrentPistolsAmmoType	VAR_U_(0x004BF3B0, char)
#define CurrentUziAmmoType	VAR_U_(0x004BF3BB, char)
#define CurrentRevolverAmmoType	VAR_U_(0x004BF3B1, char)
#define CurrentShotGunAmmoType	VAR_U_(0x004BF3B2, char)
#define CurrentGrenadeGunAmmoType	VAR_U_(0x004BF3A4, char)
#define CurrentCrossBowAmmoType	VAR_U_(0x004BF3C6, char)
#define current_selected_option	VAR_U_(0x007FEA1B, uchar)
#define StashedCurrentPistolsAmmoType	VAR_U_(0x004BF3BC, char)
#define StashedCurrentUziAmmoType	VAR_U_(0x004BF3A5, char)
#define StashedCurrentRevolverAmmoType	VAR_U_(0x004BF3C7, char)
#define StashedCurrentShotGunAmmoType	VAR_U_(0x004BF3C2, char)
#define StashedCurrentGrenadeGunAmmoType	VAR_U_(0x004BF3BA, char)
#define StashedCurrentCrossBowAmmoType	VAR_U_(0x004BF3BD, char)
#define Stashedcurrent_selected_option	VAR_U_(0x007FEA4D, char)
#define menu_active	VAR_U_(0x007FE9C8, char)
#define ammo_active	VAR_U_(0x007FEA42, char)
#define num_ammo_slots	VAR_U_(0x007FE9D0, char)
#define oldLaraBusy	VAR_U_(0x007FE9C0, char)
#define OBJLIST_SPACING	VAR_U_(0x007FE9BC, long)
#define inventry_xpos	VAR_U_(0x004BF3D0, short)
#define inventry_ypos	VAR_U_(0x004BF3D2, short)
#define InventoryActive	VAR_U_(0x004BF3C8, long)
#define GnFrameCounter	VAR_U_(0x007E71DC, long)
#define FXType	VAR_U_(0x007FE7E0, short)
#define OnObject	VAR_U_(0x007FE200, long)
#define FootPrintNum	VAR_U_(0x008010C0, long)
#define PickupY	VAR_U_(0x007FD20C, short)
#define PickupX	VAR_U_(0x007FD20E, short)
#define PickupVel	VAR_U_(0x007FD210, short)
#define CurrentPickup	VAR_U_(0x007FD212, short)
#define nRope	VAR_U_(0x004BF6A8, long)
#define phkResult	VAR_U_(0x00670E3C, HKEY)
#define dwDisposition	VAR_U_(0x00670E40, DWORD)
#define outside	VAR_U_(0x007FD300, long)
#define number_draw_rooms	VAR_U_(0x007FD304, long)
#define camera_underwater	VAR_U_(0x007FE12C, long)
#define room_list_start	VAR_U_(0x004BF628, long)
#define room_list_end	VAR_U_(0x004BF62C, long)
#define outside_top	VAR_U_(0x007FE150, long)
#define outside_left	VAR_U_(0x007FE118, long)
#define outside_right	VAR_U_(0x007FE138, long)
#define outside_bottom	VAR_U_(0x007FDF58, long)
#define SkyPos	VAR_U_(0x007FE128, short)
#define SkyPos2	VAR_U_(0x007FDF5E, short)
#define LaserSightActive	VAR_U_(0x004BF37E, char)
#define DoFade	VAR_U_(0x00672E60, long)
#define FadeVal	VAR_U_(0x00672E58, long)
#define FadeStep	VAR_U_(0x00670E48, long)
#define FadeCnt	VAR_U_(0x00670E4C, long)
#define FadeEnd	VAR_U_(0x00670E50, long)
#define f_a	VAR_U_(0x00753C34, float)
#define f_b	VAR_U_(0x00753C38, float)
#define f_boo	VAR_U_(0x00753C8C, float)
#define phd_persp	VAR_U_(0x00753C74, long)
#define Textures	VAR_U_(0x007538F4, TEXTURE*)
#define nTextures	VAR_U_(0x007538F0, long)
#define DrawPrimitiveCnt	VAR_U_(0x00753BD4, long)
#define CutSceneTriggered	VAR_U_(0x007FE7E4, long)
#define FmvSceneTriggered	VAR_U_(0x007FE210, long)
#define nFrames	VAR_U_(0x004AF454, long)
#define DestFadeScreenHeight	VAR_U_(0x004BF382, short)
#define DEL_playingamefmv	VAR_U_(0x004BF65C, char)
#define Chris_Menu	VAR_U_(0x004BF66C, char)
#define title_controls_locked_out	VAR_U_(0x007FD268, char)
#define MainThread	VAR_U_(0x00533B08, THREAD)
#define LevelLoadingThread	VAR_U_(0x005333A8, THREAD)
#define gfFilenameWad	VAR_U_(0x007FD28C, char*)
#define gfFilenameOffset	VAR_U_(0x007FD2EC, ushort*)
#define mesh_base	VAR_U_(0x00533A7C, short*)
#define malloc_buffer	VAR_U_(0x007E71E4, char*)
#define SGcount	VAR_U_(0x004BF6D8, long)
#define SGpoint	VAR_U_(0x004BF6D4, char*)
#define AIObjects	VAR_U_(0x007FD204, AIOBJECT*)
#define TriggerTimer	VAR_U_(0x004BF3FC, char)
#define flipeffect	VAR_U_(0x004AEBFC, long)
#define rand_1	VAR_U_(0x004B96A4, long)
#define rand_2	VAR_U_(0x004B96A8, long)
#define malloc_size	VAR_U_(0x007E71A8, long)
#define malloc_ptr	VAR_U_(0x007E71BC, char*)
#define malloc_free	VAR_U_(0x007E71D4, long)
#define malloc_used	VAR_U_(0x007E71D0, long)
#define baddie_slots	VAR_U_(0x007FD1FC, CREATURE_INFO*)
#define old_cam	VAR_U_(0x007FE8E0, OLD_CAMERA)
#define	last_ideal	VAR_U_(0x007FE8B0, GAME_VECTOR)
#define camerasnaps	VAR_U_(0x004BF3E0, char)
#define static_lookcamp	VAR_U_(0x007FE930, GAME_VECTOR)
#define static_lookcamt	VAR_U_(0x007FE920, GAME_VECTOR)
#define cutseqpakPtr	VAR_U_(0x007F4F1C, char*)
#define lastcamnum	VAR_U_(0x007E7228, long)
#define GLOBAL_resident_depack_buffers	VAR_U_(0x007F4FAC, char*)
#define SenetTargetX	VAR_U_(0x007FE204, long)
#define SenetTargetZ	VAR_U_(0x007FE208, long)
#define ActivePiece	VAR_U_(0x004AEBF8, char)
#define SenetDisplacement	VAR_U_(0x004BF40C, char)
#define LaraFixedPosition	VAR_U_(0x007F5760, PHD_VECTOR)
#define InitialCameraTarget	VAR_U_(0x007F5750, PHD_VECTOR)
#define InitialCameraPosition	VAR_U_(0x007F57D0, PHD_VECTOR)
#define InitialCameraRoom	VAR_U_(0x007F5948, short)
#define LastSequence	VAR_U_(0x007F6EDC, short)
#define CurrentFov	VAR_U_(0x007F58F4, short)
#define LastFov	VAR_U_(0x007F58F6, short)
#define CameraFade	VAR_U_(0x007F576C, long)
#define spotcam_timer	VAR_U_(0x004BF6E0, short)
#define spotcam_loopcnt	VAR_U_(0x007F5888, short)
#define LaraHealth	VAR_U_(0x007F6ED0, long)
#define LaraAir	VAR_U_(0x007F5A68, long)
#define current_sequence	VAR_U_(0x007F58F2, short)
#define current_spline_camera	VAR_U_(0x007F58F0, short)
#define current_spline_position	VAR_U_(0x007F6ED8, long)
#define current_camera_cnt	VAR_U_(0x007F5740, short)
#define first_camera	VAR_U_(0x007F6ED4, short)
#define last_camera	VAR_U_(0x007F5A6C, short)
#define spline_to_camera	VAR_U_(0x007F588C, long)
#define spline_from_camera	VAR_U_(0x007F57DC, long)
#define bCheckTrigger	VAR_U_(0x004BF6E8, long)
#define number_spotcams	VAR_U_(0x007F5828, short)
#define WraithSpeed	VAR_U_(0x004AD6D0, short)
#define next_locust	VAR_U_(0x004BF2E0, long)
#define	tiltyoff	VAR_U_(0x007FE20C, long)
#define	tiltxoff	VAR_U_(0x007FE214, long)
#define boxes	VAR_U_(0x007FE9B0, BOX_INFO*)
#define number_rooms	VAR_U_(0x00533A5C, short)
#define flip_status	VAR_U_(0x007FE218, long)
#define wav_file_buffer	VAR_U_(0x004BFE5C, uchar*)
#define ADPCMBuffer	VAR_U_(0x004BFE60, uchar*)
#define gfScriptFile	VAR_U_(0x007FD2AC, uchar*)
#define gfLanguageFile	VAR_U_(0x007FD26C, uchar*)
#define MonoScreenOn	VAR_U_(0x00536F1C, char)
#define nAIObjects	VAR_U_(0x007FD200, short)
#define XATrack	VAR_U_(0x004B22F4, long)
#define XAFlag	VAR_U_(0x004B22F8, long)
#define pSortBuffer	VAR_U_(0x00660C8C, char*)
#define pSortList	VAR_U_(0x005C07A8, SORTLIST**)
#define nPolys	VAR_U_(0x00753964, long)
#define DebrisFlags	VAR_U_(0x004BF354, short)
#define DebrisMesh	VAR_U_(0x00808F84, MESH_DATA*)
#define DebrisMeshC1	VAR_U_(0x0080D7A4, long)
#define DebrisMeshC2	VAR_U_(0x0080D7A0, long)
#define DebrisMeshC3	VAR_U_(0x0080D7A8, long)
#define DebrisMeshAmbient	VAR_U_(0x00808F80, long)
#define DebrisMeshFlags	VAR_U_(0x00808F88, long)
#define next_debris	VAR_U_(0x004BF358, long)
#define NumRPickups	VAR_U_(0x007FD1DC, uchar)
#define LaserSightX	VAR_U_(0x00800168, long)
#define LaserSightY	VAR_U_(0x00800160, long)
#define LaserSightZ	VAR_U_(0x00800164, long)
#define num_level_meshes VAR_U_(0x0052A344, long)
#define mesh_vtxbuf VAR_U_(0x0052A340, MESH_DATA**)
#define dont_exit_jeep	VAR_U_(0x004BFC68, char)
#define targetMeshP	VAR_U_(0x007FE11C, MESH_DATA*)
#define binocsMeshP	VAR_U_(0x007FDF60, MESH_DATA*)
#define CurrentSequence	VAR_U_(0x007F5700, uchar)
#define SequenceUsed	ARRAY_(0x007F5708, uchar, [6])
#define SequenceResults	ARRAY_(0x007F5720, uchar, [3][3][3])
#define Sequences	ARRAY_(0x007F573C, uchar, [3])
#define REG_Setup	VAR_U_(0x00670E44, bool)
#define start_setup	VAR_U_(0x004BFE84, bool)
#define fmvs_disabled	VAR_U_(0x004BFE85, bool)
#define KeyTriggerActive	VAR_U_(0x004BF3F8, char)
#define G_dxptr	VAR_U_(0x007544F4, DXPTR*)
#define G_dxinfo	VAR_U_(0x00754120, DXINFO*)
#define G_ddraw	VAR_U_(0x0075411C, LPDIRECTDRAW4)
#define G_d3d	VAR_U_(0x007541A4, LPDIRECT3D3)
#define G_hwnd	VAR_U_(0x00754118, HWND)
#define joy_fire	VAR_U_(0x0053589C, long)
#define joy_x	VAR_U_(0x005358A0, long)
#define joy_y	VAR_U_(0x005358A4, long)
#define ClipRoomNum	VAR_U_(0x007FDF50, short)
#define OutsideRoomTable	VAR_U_(0x007FE384, char*)
#define OutsideRoomOffsets	VAR_U_(0x007FE2E8, short*)
#define cd_drive	VAR_U_(0x005333A0, char)
#define OldPickupPos	VAR_U_(0x007FD1D0, PHD_VECTOR)
#define number_dynamics	VAR_U_(0x007FE110, long)
#define SaveCounter	VAR_U_(0x005355B4, long)
#define D3DMW2VMatrix	VAR_U_(0x007540D8, D3DMATRIX)
#define nClippedPolys	VAR_U_(0x00753958, long)
#define nDDDevice	VAR_U_(0x004BFE90, long)
#define nD3DDevice	VAR_U_(0x004BFE7C, long)
#define Filter	VAR_U_(0x004B253A, bool)
#define volumetric_fx	VAR_U_(0x004BFE96, bool)
#define BumpMap	VAR_U_(0x004BFE95, bool)
#define TextLow	VAR_U_(0x004BFE94, bool)
#define resChangeCounter	VAR_U_(0x00753BD8, long)
#define level_fp	VAR_U_(0x00533A74, FILE*)
#define CompressedData	VAR_U_(0x00533A58, char*)
#define FileData	VAR_U_(0x005333A4, char*)
#define floor_data	VAR_U_(0x00533A78, short*)



#define lpClass	ARRAY_(0x004BFE78, CHAR, [4])
#define SmashedMeshRoom	ARRAY_(0x007FE360, short, [16])
#define SmashedMesh	ARRAY_(0x007FE7A0, MESH_INFO*, [16])
#define keymap	ARRAY_(0x007542EC, char, [256])
#define keymap2	ARRAY_(0x007543EC, char, [256])
#define static_objects	ARRAY_(0x005333C0, STATIC_INFO, [60])
#define flip_stats	ARRAY_(0x007FE300, long, [10])
#define Lightning	ARRAY_(0x007FFB20, LIGHTNING_STRUCT, [16])
#define dynamics	ARRAY_(0x00804860, DYNAMIC, [64])
#define w2v_matrix	ARRAY_(0x00753CC8, long, [12])
#define dels_handy_train_map	ARRAY_(0x004AFF9C, short, [512])
#define LaraNodeAmbient	ARRAY_(0x0080E9F0, long, [2])
#define sfx_frequencies	ARRAY_(0x004B38A0, long, [3])
#define GermanKeyboard	ARRAY_(0x004B2BF4, char*, [272])
#define KeyboardButtons	ARRAY_(0x004B3034, char*, [272])
#define layout	ARRAY_(0x004B2BAC, short, [2][18])
#define fmv_to_play	ARRAY_(0x004BF660, char, [2])
#define gfResidentCut	ARRAY_(0x007FD254, char, [4])
#define	LightningRGBs	ARRAY_(0x007FE130, ushort, [3])
#define LightningRGB	ARRAY_(0x007FD2F8, ushort, [3])
#define gfMips	ARRAY_(0x007FD280, char, [8])
#define inventry_objects_list	ARRAY_(0x004AE070, INVOBJ, [119])
#define lara_matrices	ARRAY_(0x0080E720, long, [180])
#define lara_joint_matrices	ARRAY_(0x0080EA00, long, [180])
#define LaraNodeUnderwater	ARRAY_(0x0080E700, uchar, [15])
#define cutseq_meshswapbits	ARRAY_(0x007F4F80, ulong, [10])
#define cutseq_meshbits	ARRAY_(0x007F4F40, ulong, [10])
#define cutseq_control_routines	ARRAY_(0x004B01B0, CUTSEQ_ROUTINES, [31])
#define actor_pnodes	ARRAY_(0x007E7200, PACKNODE*, [10])
#define temp_rotation_buffer	ARRAY_(0x007E7240, short, [160])
#define duff_item	ARRAY_(0x007E7380, ITEM_INFO, [10])
#define old_status_flags	ARRAY_(0x007F4FC0, char, [16])
#define hairs	ARRAY_(0x0080E160, HAIR_STRUCT, [2][7])
#define first_hair	ARRAY_(0x0080E140, long, [2])
#define SortList	ARRAY_(0x00660C94, SORTLIST*, [16384])
#define SortBuffer	ARRAY_(0x005C07AC, char, [65536])
#define ScratchVertNums	ARRAY_(0x0080E320, char, [40][12])
#define HairRotScratchVertNums	ARRAY_(0x004AD6D4, char, [5][12])
#define IMstack	ARRAY_(0x007FD320, long, [768])
#define vert_wibble_table	ARRAY_(0x00535520, float, [32])
#define WaterTable	ARRAY_(0x00533B1C, WATERTAB, [22][64])
#define no_rotation	ARRAY_(0x004BF630, short, [12])
#define rings	ARRAY_(0x007FEA20, RINGME*, [2])
#define options_table	ARRAY_(0x004AE9BC, short, [120])
#define ammo_object_list	ARRAY_(0x007FEA00, AMMOLIST, [3])
#define current_options	ARRAY_(0x007FE9E0, MENUTHANG, [3])
#define FootPrint	ARRAY_(0x008010E0, FOOTPRINT, [32])
#define tsv_buffer	ARRAY_(0x00804F80, char, [16384])
#define scratchpad	ARRAY_(0x0080D7C0, char, [1024])
#define RopeList	ARRAY_(0x007FB4C0, ROPE_STRUCT, [5])
#define pickups	ARRAY_(0x007FD220, DISPLAYPU, [8])
#define Gunshells	ARRAY_(0x007FF680, GUNSHELL_STRUCT, [24])
#define EnemyBites	ARRAY_(0x004AED00, BITE_INFO, [])
#define draw_room_list	ARRAY_(0x004BF424, long, [128])
#define draw_rooms	ARRAY_(0x007FDF80, short, [100])
#define Bucket	ARRAY_(0x004BFEA0, TEXTUREBUCKET, [20])
#define title_string	ARRAY_(0x004AFAA4, char, [8])
#define	gfLevelNames	ARRAY_(0x007FD2C0, uchar, [40])
#define Drips	ARRAY_(0x00800DC0, DRIP_STRUCT, [32])
#define GlobalCollisionBounds	ARRAY_(0x007FE7F0, short, [6])
#define Slist	ARRAY_(0x007F6EE0, SPHERE, [34])
#define SenetPieceNumbers	ARRAY_(0x004BF400, short, [6])
#define SenetBoard	ARRAY_(0x007FE340, char, [17])
#define ActiveSenetPieces	ARRAY_(0x007FE158, char, [6])
#define SpotCam	ARRAY_(0x007F5A80, SPOTCAM, [128])
#define camera_xposition	ARRAY_(0x007F5840, long, [18])
#define camera_yposition	ARRAY_(0x007F59C0, long, [18])
#define camera_zposition	ARRAY_(0x007F5900, long, [18])
#define camera_xtarget	ARRAY_(0x007F57E0, long, [18])
#define camera_ytarget	ARRAY_(0x007F6E80, long, [18])
#define camera_ztarget	ARRAY_(0x007F5960, long, [18])
#define camera_roll	ARRAY_(0x007F58A0, long, [18])
#define camera_fov	ARRAY_(0x007F5780, long, [18])
#define camera_speed	ARRAY_(0x007F5A20, long, [18])
#define SpotRemap	ARRAY_(0x007F58E8, uchar, [8])
#define CameraCnt	ARRAY_(0x007F6EC8, uchar, [8])
#define LibraryTab	ARRAY_(0x004BF2FC, char, [8])
#define spark	ARRAY_(0x00801400, SPARKS, [256])
#define smoke_spark	ARRAY_(0x007FF100, SMOKE_SPARKS, [32])
#define objects	ARRAY_(0x0052B850, OBJECT_INFO, [465])
#define Locusts	ARRAY_(0x0080FA00, LOCUST_STRUCT, [64])
#define ground_zone	ARRAY_(0x007FE980, short*, [5][2])
#define broomies	ARRAY_(0x007F5020, short, [22])
#define MonoScreen	ARRAY_(0x00577460, MONOSCREEN_STRUCT, [5])
#define XYUVClipperBuffer	ARRAY_(0x006607B0, D3DTLBUMPVERTEX, [20])
#define zClipperBuffer	ARRAY_(0x005C0488, D3DTLBUMPVERTEX, [20])
#define debris	ARRAY_(0x00808FA0, DEBRIS_STRUCT, [256])
#define RPickups	ARRAY_(0x007FD1C0, uchar, [16])
#define jroomies	ARRAY_(0x007F4FE0, short, [22])
#define conflict	ARRAY_(0x005355B8, long, [18])
#define SaveGames	ARRAY_(0x00536F28, SAVEFILE_INFO, [15])
#define matrix_stack	ARRAY_(0x00753D04, long, [12])

/*Fog stuff*/
#define NumFXFogBulbs	VAR_U_(0x00670CB8, long)
#define NumActiveFogBulbs	VAR_U_(0x00670CAC, long)
#define NumFogBulbsInRange	VAR_U_(0x00670CB4, long)
#define NumLevelFogBulbs	VAR_U_(0x0052A348, long)
#define GlobalFogOff	VAR_U_(0x004BF370, long)
#define CurrentFog	VAR_U_(0x00533AEC, long)

#define FogBulbs	ARRAY_(0x0052A350, FOGBULB_STRUCT, [20])
#define FXFogBulbs	ARRAY_(0x00660AD0, FOGBULB_STRUCT, [5])
#define ActiveFogBulbs	ARRAY_(0x00670C94, FOGBULB_STRUCT*, [5])
#define FogTableColor	ARRAY_(0x004ADE1C, long, [28])


/*VonCroy stuff*/
#define bVoncroyCutScene	VAR_U_(0x007FE288, char)
#define actualCameraPos	VAR_U_(0x0080ECF0, PHD_VECTOR)
#define actualCameraTarget	VAR_U_(0x0080ECE0, PHD_VECTOR)
#define actualFOV	VAR_U_(0x0080ECD8, long)
#define actualRoomNumber	VAR_U_(0x0080ECD4, long)

#define VonCroyCutIndices	ARRAY_(0x004AD4D4, uchar, [68])				//indices for VonCroyCutscenes depending on lara.locationPad
#define VonCroyCutscenes	ARRAY_(0x004AD518, VonCroyCutData, [15])
#define VonCroyCutFlags	ARRAY_(0x007FE2A0, uchar, [64])					//flags cuts played
#define VonCroyCutTracks	ARRAY_(0x004AD458, short, [62])


/*Bink bs*/
#define BinkSurfaceType	VAR_U_(0x007E6E00, long)
#define BinkSurface	VAR_U_(0x007E6E04, LPDIRECTDRAWSURFACE4)
#define Bink	VAR_U_(0x007E6E08, BINK_STRUCT*)


#define NO_HEIGHT	-32512
#define NO_ITEM	-1
#define FVF (D3DFVF_TEX2 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW)
#define W2V_SHIFT	14
#ifdef GENERAL_FIXES
#define MAX_DYNAMICS	64
#else
#define MAX_DYNAMICS	32
#endif