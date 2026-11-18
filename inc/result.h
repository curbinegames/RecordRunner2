#pragma once

#include <RecSystem.h>
#include <RecScoreFile.h>

typedef struct rec_to_result_s {
	rec_map_detail_t map_detail;
	rec_play_userpal_t userpal;
	rec_play_nameset_t nameset;
} rec_to_result_st;

extern rec_clear_rank_t JudgeClearRank(const rec_play_userpal_t &userpal);

extern now_scene_t result(const rec_to_result_st &toResult, const rec_to_play_set_t &ps);
