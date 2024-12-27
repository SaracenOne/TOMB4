#pragma once
#include "../global/types.h"

void OpenStreamFile(char* name);
void GetADPCMData();
void ACMSetVolume();
void ACMEmulateCDPlay(long track, long mode);
#if !defined(MA_AUDIO_SAMPLES) || !defined(MA_AUDIO_ENGINE)
BOOL __stdcall ACMEnumCallBack(HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport);
#endif
long ACMSetupNotifications();
void FillADPCMBuffer(char* p, long track);
long ACMHandleNotifications();
bool ACMInit();
void ACMClose();
void S_CDPlay(long track, long mode);
void S_CDPlayExt(unsigned char track_id, unsigned char channel_id, bool looping, bool restore_old_track);
unsigned char S_CDGetTrackID(unsigned char channel_id);
bool S_CDGetChannelIsActive(unsigned char channel_id);
bool S_CDGetChannelIsLooping(unsigned char channel_id);
int32_t S_CDGetChannelPosition(unsigned char channel_id);
void S_CDStop();
void S_CDStopExt(unsigned char channel_id);
void S_StartSyncedAudio(long track);

void S_CDSetChannelVolume(unsigned char volume, unsigned char channel_id);

void S_AudioUpdate();
void S_PauseAudio();
void S_UnpauseAudio();

void S_Reset();

void S_CDSeek(int channel_id, int frame);

void SetUsingNewAudioSystem(bool enabled);
void SetUsingOldTriggerMode(bool enabled);

bool IsUsingNewAudioSystem();
bool IsUsingOldCDTriggerMode();

#if !defined(MA_AUDIO_SAMPLES) || !defined(MA_AUDIO_ENGINE)
extern HACMDRIVER hACMDriver;
#endif

extern uchar* wav_file_buffer;
extern uchar* ADPCMBuffer;
extern bool acm_ready;

// Used for Von Croy cutscenes
extern long LegacyTrack;
extern long LegacyTrackFlag;
