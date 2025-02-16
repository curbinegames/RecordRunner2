#pragma once

#include <system.h>
#include <RecScoreFile.h>

extern rec_clear_rank_t JudgeClearRank(const rec_play_userpal_t *userpal);

extern now_scene_t result(const rec_map_detail_t *map_detail, const rec_play_userpal_t *userpal,
	const rec_play_nameset_t *nameset, rec_dif_t dif, const TCHAR *songN);
