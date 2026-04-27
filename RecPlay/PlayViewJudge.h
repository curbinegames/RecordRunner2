#pragma once

#include <dxcur.h>
#include "playbox.h"

extern void ReadyJudgePicture();
extern void PlaySetJudge(note_judge judge);
extern void PlayShowJudge(const dxcur_camera_c &camera, rec_play_lanepos_t *lanePos, int charaput);