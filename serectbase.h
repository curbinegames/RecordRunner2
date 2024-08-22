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
void ShowHelpBar(unsigned int Cr, int bar, int lan);
void SortSong(MUSIC_BOX *songdata, int *mapping, const int mode,
	const int dif, const int SongNumCount);

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

void ShowHelpBar(unsigned int Cr, int bar, int lan) {
	RecRescaleDrawGraph(0, 0, bar, TRUE);
	switch (int(GetNowCount() / 10000 % 3) * 10 + lan) {
	case 0:
		RecRescaleDrawString(5, 460, L"Enterキー: 決定, BackSpaceキー: 戻る", Cr);
		break;
	case 1:
		RecRescaleDrawString(5, 460, L"Enter key: start, BackSpace key: back to menu", Cr);
		break;
	case 10:
		RecRescaleDrawString(5, 460, L"上下キー: 曲選択, 左右キー: 難易度選択", Cr);
		break;
	case 11:
		RecRescaleDrawString(5, 460, L"↑↓key: music select, ←→key: dif select", Cr);
		break;
	case 20:
		RecRescaleDrawString(5, 460, L"Zキー: 楽曲を並び替える, Pキー+Enterキー: オートプレイ", Cr);
		break;
	case 21:
		RecRescaleDrawString(5, 460, L"Z key: Sort Songs, P and Enter key: Auto Play", Cr);
		break;
	}
	return;
}

void SortSong(MUSIC_BOX *songdata, int *mapping, const int mode,
	const int dif, const int SongNumCount) {
	int n = 0;
	int m = SongNumCount;
	int o = 0;
	int p = 1;
	switch (mode) {
	case SORT_DEFAULT:
		for (int i = 0; i < SongNumCount; i++) {
			*mapping = i;
			mapping++;
		}
		break;
	case SORT_LEVEL:
		while (p) {
			p = 0;
			for (int i = 0; i < SongNumCount - 1; i += 2) {
				if (songdata[mapping[i]].level[dif] > songdata[mapping[i + 1]].level[dif]) {
					o = mapping[i];
					mapping[i] = mapping[i + 1];
					mapping[i + 1] = o;
					p = 1;
				}
			}
			for (int i = 1; i < SongNumCount - 1; i += 2) {
				if (songdata[mapping[i]].level[dif] > songdata[mapping[i + 1]].level[dif]) {
					o = mapping[i];
					mapping[i] = mapping[i + 1];
					mapping[i + 1] = o;
					p = 1;
				}
			}
		}
		break;
	case SORT_SCORE:
		while (p) {
			p = 0;
			for (int i = 0; i < SongNumCount - 1; i += 2) {
				if (songdata[mapping[i]].Hscore[dif] > songdata[mapping[i + 1]].Hscore[dif]) {
					o = mapping[i];
					mapping[i] = mapping[i + 1];
					mapping[i + 1] = o;
					p = 1;
				}
			}
			for (int i = 1; i < SongNumCount - 1; i += 2) {
				if (songdata[mapping[i]].Hscore[dif] > songdata[mapping[i + 1]].Hscore[dif]) {
					o = mapping[i];
					mapping[i] = mapping[i + 1];
					mapping[i + 1] = o;
					p = 1;
				}
			}
		}
		break;
	}
	return;
}

#endif