#pragma once

typedef struct rec_system_s {
	int chara = 0;
	int offset = 0;
	int soundEn = 0;
	int backLight = 2;
	int language = 0;
	int keyViewEn = 0;
	int judgePos = 0;
} rec_system_t;

extern int recSystenLoad(rec_system_t *sys);

extern void RecSysBgmChangeVolume(int val);
extern void RecSysBgmSetMem(const TCHAR *sndPath, size_t size);
extern bool RecSysBgmCheckSoundMem(void);
extern void RecSysBgmPlay(bool force, bool loop, bool top_position);
extern void RecSysBgmDelete(void);
extern void RecSysBgmStop(void);
extern void RecSysBgmSetCurrentPosition(int val);
