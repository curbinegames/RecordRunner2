#pragma once

#include <stdio.h>
#include <strcur.h>
#include <system.h>
#include <playbox.h>

typedef struct rec_score_savefile_s {
	int score  = 0;
	int dist   = 0;
	double acc = 0;
	rec_clear_rank_t clearRank = REC_CLEAR_RANK_NO_PLAY;
	rec_score_rate_t scoreRate = REC_SCORE_RATE_NO_PLAY;
} rec_score_savefile_t;

extern int RecSaveReadScoreAllDif(rec_score_savefile_t dest[], const TCHAR *songname);
extern int RecSaveWriteScoreAllDif(const rec_score_savefile_t src[], const TCHAR *songname);
