#pragma once

#include <dxcur.h>

typedef enum cutin_tips_e {
	CUTIN_TIPS_NONE = 0,
	CUTIN_TIPS_ON,
	CUTIN_TIPS_SONG,
} cutin_tips_e;

extern void CutinReadyPic();
extern void CutinReadySnd();
extern void SetCutSong(wchar_t* songName, wchar_t* picName);
extern void SetCutTipFg(cutin_tips_e Fg);
extern void SetTipNo();
extern void ViewCutIn(int Stime);
extern void ViewCutOut(int Stime);
extern void RecViewCut2();
extern void RecCutSetIo(int val);
extern  int RecCutIsClosing();
extern  int RecCutInEndAnim();

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
