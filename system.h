#pragma once

#include "DxLib.h"
#include "recr_cutin.h"

#define COLOR_WHITE (0xffffffff)

#define OLD_RATE_NUM 10
#define OLD_RATE_FILE_NAME_S L"save/rateS.dat" /* 名前 */
#define OLD_RATE_FILE_NAME_N L"save/rateN.dat" /* 数値 */

#define RATE_NUM 20
#define RATE_FILE_NAME L"save/Prate.dat" /* 名前+数値 */

#define INIT_PIC() InitGraph(); CutinReadyPic()
#define INIT_SND() InitSoundMem(); CutinReadySnd()
#define INIT_MAT() INIT_PIC(); INIT_SND()

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
} now_scene_t; /* = next */

char GetCharNo() {
	int data[6] = { 0,0,0,2,0,0 };
	FILE* fp;
	(void)_wfopen_s(&fp, L"save/system.dat", L"rb");
	if (fp != NULL) {
		fread(&data, sizeof(int), 6, fp);
		fclose(fp);
	}
	return (char)data[0];
}
