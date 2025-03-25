#pragma once

#include <RecSystem.h>

typedef struct rec_to_play_set_s {
	int packNo = 0;
	int musicNo = 0;
	int dif = 0;
	int shift = 0;
	int autoFg = 0;
} rec_to_play_set_t;

extern now_scene_t musicserect(rec_to_play_set_t *toPlay);
