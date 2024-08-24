#ifndef SECERT_BASE
#define SECERT_BASE

#include <DxLib.h>
#include "general/dxcur.h"
#include "general/sancur.h"
#include "general/strcur.h"
#include "RecWindowRescale.h"

#define PackNumLim 8
#define SongNumLim 64
#define SORT_DEFAULT 0
#define SORT_LEVEL 1
#define SORT_SCORE 2

typedef struct music_box {
	int limit;
	int level[6];
	int preview[6][2];
	int Hscore[6];
	int Hdis[6];
	int ClearRank[6];//0=EX, 1=S, 2=A, 3=B, 4=C, 5=D, 6=not play
	int ClearRate[6];//0=not play, 1=droped, 2=cleared, 3=no miss!, 4=full combo!!, 5=perfect!!!
	double Hacc[6];
	wchar_t difP[256];
	wchar_t packName[256];
	wchar_t SongName[6][256];
	wchar_t artist[6][256];
	wchar_t SongFileName[6][256];
	wchar_t jacketP[6][256];
}MUSIC_BOX;

typedef MUSIC_BOX rec_music_detail_t;

void ChangeSortMode(int *mode);
void DrawBackPicture(int img);
int GetRate();

/* +++++ */

void ChangeSortMode(int *mode) {
	*mode = *mode + 1;
	if (*mode >= 3) {
		*mode = 0;
	}
}

void DrawBackPicture(int img) {
	const int posx = (-GetNowCount() / 10) % 640;
	RecRescaleDrawGraph(posx, 0, img, TRUE);
	RecRescaleDrawGraph(posx + 640, 0, img, TRUE);
	return;
}

int GetRate() {
	int e;
	int ans = 0;
	play_rate_t prate[RATE_NUM];
	FILE *fp;
	e = _wfopen_s(&fp, RATE_FILE_NAME, L"rb");
	if (fp == NULL) {
		return 0;
	}
	fread(&prate, sizeof(play_rate_t), RATE_NUM, fp);
	fclose(fp);
	for (int i = 0; i < RATE_NUM; i++) {
		if (0 < prate[i].num) {
			ans += prate[i].num * 100;
		}
	}
	return ans / 2;
}

#endif