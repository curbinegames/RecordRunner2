#pragma once

#include "general/dxcur.h"

typedef enum help_bar_mat_e {
	HELP_MAT_MUSIC_NONE = 0,
	HELP_MAT_MUSIC_SELECT,
	HELP_MAT_OPTION,
} help_bar_mat_t;

class rec_helpbar_c {
private:
	DxPic_t pic;
public:
	rec_helpbar_c();
	~rec_helpbar_c();
	void DrawHelp(help_bar_mat_t mat);
};

extern void InitHelpBar();
extern void DrawHelpBar(help_bar_mat_t mat, char lan);
