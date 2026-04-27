#pragma once

#include <queue>
#include <RecScoreFile.h>
#include "playbox.h"

typedef struct rec_judge_event_s {
	note_material mat = NOTE_NONE;
	int gaptime = 0;
	note_box_2_t noteinfo;
	note_judge judge;
	int lineNo;
} rec_judge_event_st;

/* DOTO: Œø‰Ê‰¹Œn‚ð•Êƒtƒ@ƒCƒ‹‚ÉˆÚ“®‚·‚é */
extern void RecPlayInitMelodySnd();
extern void RecJudgeAllNotes(
	std::queue<rec_judge_event_st> &event_queue, cvec<note_box_2_t> note[], int Ntime,
	const std::vector<DxSnd_t> &Sitem, rec_play_key_hold_t *keyhold, rec_hitatk_event_ec &hitatk,
	int LaneTrack[], int *charahit, short charaput, rec_play_userpal_t *userpal,
	rec_play_sound_c *p_sound
);
