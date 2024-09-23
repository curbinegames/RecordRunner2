#pragma once

#include "DxLib.h"
#include "recr_cutin.h"
#include "helpBar.h"

#define WINDOW_SIZE_X 1280                     // �E�B���h�E�̉��̃T�C�Y
#define WINDOW_SIZE_Y (WINDOW_SIZE_X * 9 / 16) // �E�B���h�E�̏c�̃T�C�Y 720

#define OLD_WINDOW_SIZE_X 640                         // �O�o�[�W�����̃E�B���h�E�̉��̃T�C�Y
#define OLD_WINDOW_SIZE_Y (OLD_WINDOW_SIZE_X * 3 / 4) // �O�o�[�W�����̃E�B���h�E�̏c�̃T�C�Y 480

#define COLOR_WHITE (0xffffffff)

#define OLD_RATE_NUM 10
#define OLD_RATE_FILE_NAME_S L"save/rateS.dat" /* ���O */
#define OLD_RATE_FILE_NAME_N L"save/rateN.dat" /* ���l */

#define RATE_NUM 20
#define RATE_FILE_NAME L"save/Prate.dat" /* ���O+���l */

#define INIT_PIC() InitGraph(); CutinReadyPic(); InitHelpBar()
#define INIT_SND() InitSoundMem(); CutinReadySnd()
#define INIT_MAT() INIT_PIC(); INIT_SND()

#define SWITCH_NOTE_BOX_2 1

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

extern int SmallFontData;
extern int LargeFontData;
