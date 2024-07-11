#pragma once
/**
 *	@file
 *	@brief	サウンド関連
 */

#define USE_SOUND	(1)
#define ENABLE_OPUS	(0 && USE_SOUND)
#define SE_MAX	1024


bool InitializeSound();
void FinalizeSound();

int CreateSEOgg(const char* filename);
#if ENABLE_OPUS
int CreateSEOpus(const char* filename);
#endif

void PlaySE(int seid, bool isLoop = false, float volume = 1.0f);
void StopSE(int seid);
void SetVolumeSE(int seid, float volume);
void SetFrequencySE(int seid, float ratio);
bool IsPlaySE(int seid);
void ReleaseSE(int seid);
int GetSEUseCount(void);
