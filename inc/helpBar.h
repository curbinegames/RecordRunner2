#pragma once

#include <dxcur.h>

typedef enum help_bar_mat_e {
	HELP_MAT_MUSIC_NONE = 0,
	HELP_MAT_MENU,
	HELP_MAT_MUSIC_SELECT,
	HELP_MAT_COLLECTION,
	HELP_MAT_COLLECTION_ITEM,
	HELP_MAT_COLLECTION_STORY,
	HELP_MAT_COLLECTION_STORY_READ,
	HELP_MAT_OPTION,
} help_bar_mat_t;

class rec_helpbar_c {
private:
	dxcur_pic_c pic = dxcur_pic_c(_T("picture/help.png"));
public:
	void DrawHelp(help_bar_mat_t mat);
	void ReloadMat(void);
};
