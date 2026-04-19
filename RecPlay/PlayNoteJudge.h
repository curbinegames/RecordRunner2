#pragma once

#include <RecScoreFile.h>
#include "playbox.h"

/* DOTO: Œø‰Ê‰¹Œn‚ð•Êƒtƒ@ƒCƒ‹‚ÉˆÚ“®‚·‚é */
extern void RecPlayInitMelodySnd();
extern void RecJudgeAllNotes(cvec<note_box_2_t> note[], int Ntime, const std::vector<DxSnd_t> &Sitem,
	rec_play_key_hold_t *keyhold, rec_play_chara_hit_attack_t *hitatk, int LaneTrack[],
	int *charahit, short charaput, rec_play_userpal_t *userpal, rec_play_sound_c *p_sound);
