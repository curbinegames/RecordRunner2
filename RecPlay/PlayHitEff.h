#pragma once

#include "playbox.h"

extern void ReadyEffPicture();
extern void PlayCheckHitEffect();
extern void PlaySetHitEffect(note_judge judge, note_material notemat, int LineNo);
extern void PlayShowHitEffect(const dxcur_camera_c &camera_pos, rec_play_lanepos_t *lanePos);