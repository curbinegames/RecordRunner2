#pragma once

#include "playbox.h"

/* DOTO: 効果音系を別ファイルに移動する */
extern void RecPlayInitMelodySnd();
extern void RecJudgeAllNotes(note_box_2_t note[], short noteNo[], int Ntime, int Sitem[],
	rec_play_key_hold_t *keyhold, rec_play_chara_hit_attack_t *hitatk, int LaneTrack[],
	int *charahit, short charaput, rec_play_userpal_t *userpal, rec_play_sound_c *p_sound);
