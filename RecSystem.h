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
