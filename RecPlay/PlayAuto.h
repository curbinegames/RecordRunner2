#pragma once

#include "RecPlay/playbox.h"

extern void InitAdif();
extern int GetAdif();
extern void AutoAution(rec_play_key_hold_t *key, note_box_2_t note[],
	short int objectNG[], int Ntime);
