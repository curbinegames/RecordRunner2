#pragma once

#include "DxLib.h"
#include "recr_cutin.h"
#include "helpBar.h"

#define WINDOW_SIZE_X 1280                     // ウィンドウの横のサイズ
#define WINDOW_SIZE_Y (WINDOW_SIZE_X * 9 / 16) // ウィンドウの縦のサイズ 720

#define OLD_WINDOW_SIZE_X 640                         // 前バージョンのウィンドウの横のサイズ
#define OLD_WINDOW_SIZE_Y (OLD_WINDOW_SIZE_X * 3 / 4) // 前バージョンのウィンドウの縦のサイズ 480

#define OLD_RATE_NUM 10
#define OLD_RATE_FILE_NAME_S L"save/rateS.dat" /* 名前 */
#define OLD_RATE_FILE_NAME_N L"save/rateN.dat" /* 数値 */

#define RATE_NUM 20
#define RATE_FILE_NAME L"save/Prate.dat" /* 名前+数値 */

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

typedef struct play_rate_s {
	wchar_t name[255] = L"\0";
	double num = 0;
} play_rate_t;

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

typedef struct rec_user_data_s {
	int playCount = 0;
	int dropCount = 0;
	int a; /* ??? */
	int clearCount = 0;
	int NMCount = 0;
	int FCCount = 0;
	int PFcount = 0;
	int mileage = 0;
} rec_user_data_t;

extern int RecPlayDebug[3];

extern int SmallFontData;
extern int LargeFontData;

extern int RecGetMusicPath(TCHAR *ret, size_t size, uint packNo, uint songNo);
