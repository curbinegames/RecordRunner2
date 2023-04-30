#pragma once

/* include */
#include "playbox.h"

/* enum */
typedef enum difkey_act_e {
	DIFKEY_ACT_NONE = 0,
	DIFKEY_ACT_1HIT,
	DIFKEY_ACT_1ARW,
	DIFKEY_ACT_2HIT,
	DIFKEY_ACT_1HIT_1ARW,
	DIFKEY_ACT_2ARW,
	DIFKEY_ACT_3HIT,
	DIFKEY_ACT_2HIT_1ARW,
	DIFKEY_ACT_1HIT_2ARW,
} difkey_act_t;

typedef enum difkey_pos_e {
	DIFKEY_POS_NONE = 0,
	DIFKEY_POS_UP,
	DIFKEY_POS_MID,
	DIFKEY_POS_DOWN,
} difkey_pos_t;

/* function */
extern void DifkeyCalInsertNote(note_box const* const note, char lane);
extern difkey_act_t DifkeyCalAct(note_box const* const noteUp,
	note_box const* const noteMid, note_box const* const noteLow);
extern void DifkeyCalInit(int notes, int songlength);
