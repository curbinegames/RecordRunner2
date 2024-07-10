#pragma once

#include "playbox.h"

extern void RecJudgeAllNotes(note_box_2_t note[], short int noteNo[], int Ntime,
	distance_score_t *Dscore, int Sitem[], judge_action_box *judgeA, rec_play_key_hold_t *keyhold,
	rec_play_chara_hit_attack_t *hitatk, play_sound_t *p_sound, int LaneTrack[], int *charahit,
	int MelodySnd[], short int charaput);
