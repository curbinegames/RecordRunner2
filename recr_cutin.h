#pragma once

typedef enum cutin_tips_e {
	CUTIN_TIPS_NONE = 0,
	CUTIN_TIPS_ON,
	CUTIN_TIPS_SONG,
} cutin_tips_e;

extern void CutinReady();
extern void SetCutSong(wchar_t* songName, wchar_t* picName);
extern void SetCutTipFg(cutin_tips_e Fg);
extern void SetTipNo();
extern void ViewCutIn(int Stime);
extern void ViewCutOut(int Stime);
