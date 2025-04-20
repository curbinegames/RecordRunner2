#pragma once

#include <dxcur.h>

/* TODO: enum‰» */
#define CUT_FRAG_OUT 0
#define CUT_FRAG_IN  1

typedef enum cutin_tips_e {
	CUTIN_TIPS_NONE = 0,
	CUTIN_TIPS_ON,
	CUTIN_TIPS_SONG,
} cutin_tips_e;

class rec_cutin_c {
private:
public:
	rec_cutin_c();
	~rec_cutin_c();
	void SetCutSong(TCHAR *songName, TCHAR *picName);
	void SetCutTipFg(cutin_tips_e Fg);
	void SetTipNo();
	void DrawCut();
	void SetIo(int val);
	int IsClosing();
	int IsEndAnim();
};
