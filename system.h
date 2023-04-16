#pragma once

#include "DxLib.h"
#include "recr_cutin.h"

#define COLOR_WHITE (0xffffffff)

#define INIT_PIC() InitGraph(); CutinReadyPic()
#define INIT_SND() InitSoundMem(); CutinReadySnd()
#define INIT_MAT() INIT_PIC(); INIT_SND()

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
