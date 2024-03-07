#pragma once
#include "playbox.h"

typedef enum note_judge {
	NOTE_JUDGE_NONE = -1,
	NOTE_JUDGE_JUST = 0,
	NOTE_JUDGE_GOOD,
	NOTE_JUDGE_SAFE,
	NOTE_JUDGE_MISS
} note_judge;

extern void ReadyEffPicture();
extern void PlayCheckHitEffect();
extern void PlaySetHitEffect(int time, note_judge judge, note_material notemat, int LineNo);
extern void PlayShowHitEffect(int *xline, int *yline, int xcam, int ycam);
