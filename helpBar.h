#pragma once

typedef enum help_bar_mat_e {
	HELP_MAT_MUSIC_NONE = 0,
	HELP_MAT_MUSIC_SELECT,
} help_bar_mat_t;

extern void InitHelpBar();
extern void DrawHelpBar(help_bar_mat_t mat, char lan);
