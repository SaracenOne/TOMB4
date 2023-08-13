#pragma once
#include "../global/types.h"

void OpenStreamFile(char* name);
void GetADPCMData();
void ACMSetVolume();
void ACMEmulateCDPlay(long track, long mode);
BOOL __stdcall ACMEnumCallBack(HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport);
long ACMSetupNotifications();
void FillADPCMBuffer(char* p, long track);
long ACMHandleNotifications();
bool ACMInit();
void ACMClose();
void S_CDPlay(long track, long mode);
void S_CDPlayExt(unsigned char track_id, unsigned char channel_id, bool looping, bool restore_old_track);
void S_CDStop();
void S_CDStopExt(unsigned char channel_id);
void S_StartSyncedAudio(long track);

void S_CDSetChannelVolume(unsigned char volume, unsigned char channel_id);

void S_AudioUpdate();
void S_PauseAudio();
void S_UnpauseAudio();

void S_Reset();

void SetUsingNewAudioSystem(bool enabled);
void SetUsingOldTriggerMode(bool enabled);

bool IsUsingNewAudioSystem();
bool IsUsingOldTriggerMode();

extern HACMDRIVER hACMDriver;
extern uchar* wav_file_buffer;
extern uchar* ADPCMBuffer;
extern bool acm_ready;

// Used for Von Croy cutscenes
extern long LegacyTrack;
extern long LegacyTrackFlag;
