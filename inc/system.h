#pragma once

#include "DxLib.h"
#include "recr_cutin.h"
#include "helpBar.h"

#define WINDOW_SIZE_X 1280                     // ウィンドウの横のサイズ
#define WINDOW_SIZE_Y (WINDOW_SIZE_X * 9 / 16) // ウィンドウの縦のサイズ 720

#define OLD_WINDOW_SIZE_X 640                         // 前バージョンのウィンドウの横のサイズ
#define OLD_WINDOW_SIZE_Y (OLD_WINDOW_SIZE_X * 3 / 4) // 前バージョンのウィンドウの縦のサイズ 480

#define INIT_PIC() InitGraph(); CutinReadyPic(); InitHelpBar()
#define INIT_SND() InitSoundMem(); CutinReadySnd()
#define INIT_MAT() INIT_PIC(); INIT_SND()

#define SWITCH_NOTE_BOX_2 1

/* debug */
#if 0
#define RECR_DEBUG(ofs, data)											\
		RecRescaleDrawFormatString(20, 120 + ofs * 20, Cr, L#data": %d", data)
#define RECR_DEBUG_LOOP(ofs, n, data_a, data_b)							\
	for (int _rep = 0; _rep < n; _rep++) {								\
		RecRescaleDrawFormatString(20, 120 + _rep * 20 + ofs * 20, Cr,			\
		L#data_a"[%d]"#data_b": %d", _rep, data_a[_rep]data_b);			\
	}
#else
#define RECR_DEBUG(n, data_a, data_b)
#define RECR_DEBUG_LOOP(n, data_a, data_b)
#endif

typedef unsigned int uint;

typedef enum rec_play_rank_e {
	REC_SCORE_RATE_EX = 0,
	REC_SCORE_RATE_S,
	REC_SCORE_RATE_A,
	REC_SCORE_RATE_B,
	REC_SCORE_RATE_C,
	REC_SCORE_RATE_D,
	REC_SCORE_RATE_NO_PLAY,
	REC_SCORE_RATE_F,
} rec_score_rate_t;

typedef enum now_scene_e {
	SCENE_TITLE = 0,
	SCENE_MENU,
	SCENE_SERECT,
	SCENE_COLLECTION,
	SCENE_OPTION,
	SCENE_EXIT = 5,
	SCENE_MUSIC,
	SCENE_RESULT,
} now_scene_t; /* = next */

/* TODO: rec_clear_rank_eと似た定義が他にないかチェック */
typedef enum rec_clear_rank_e {
	REC_CLEAR_RANK_NO_PLAY = 0,
	REC_CLEAR_RANK_DROPED = 1, /* 実装の都合上1スタート */
	REC_CLEAR_RANK_CLEARED,
	REC_CLEAR_RANK_NOMISS,
	REC_CLEAR_RANK_FULLCOMBO,
	REC_CLEAR_RANK_PERFECT,
} rec_clear_rank_t;

/* TODO: rec_dif_eと似た定義が他にないかチェック */
typedef enum rec_dif_e {
	REC_DIF_AUTO = 0,
	REC_DIF_EASY,
	REC_DIF_NORMAL,
	REC_DIF_HARD,
	REC_DIF_ANOTHER,
	REC_DIF_SECRET,
} rec_dif_t;

extern int RecPlayDebug[3];

extern int SmallFontData;
extern int LargeFontData;

extern int RecGetPackFolderPath(TCHAR *ret, size_t size, uint packNo);
extern int RecGetMusicFolderPath(TCHAR *ret, size_t size, uint packNo, uint songNo);
extern int RecGetMusicFolderName(TCHAR *ret, size_t size, uint packNo, uint songNo);
extern int RecGetMusicMapRrsPath(TCHAR *ret, size_t size, uint packNo, uint songNo,
	rec_dif_t difNo);
extern int RecGetMusicMapTxtPath(TCHAR *ret, size_t size, uint packNo, uint songNo,
	rec_dif_t difNo);
