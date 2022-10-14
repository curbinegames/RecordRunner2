#ifndef SECERT_BASE
#define SECERT_BASE

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
	wchar_t SongName[6][256];
	wchar_t artist[6][256];
	wchar_t SongFileName[6][256];
	wchar_t jacketP[6][256];
}MUSIC_BOX;

void ChangeSortMode(int *mode) {
	*mode = *mode + 1;
	if (*mode >= 3) {
		*mode = 0;
	}
}

void DrawBackPicture(int img) {
	const int posx = (-GetNowCount() / 10) % 640;
	DrawGraph(posx, 0, img, TRUE);
	DrawGraph(posx + 640, 0, img, TRUE);
	return;
}

void DrawDifMaker(MUSIC_BOX songdata, int comdif, int comnum,
	const int *difC) {
	int posX = 0;
	int posY = 0;
	for (int i = 0; i < 5; i++) {
		posY = 0;
		if (comdif == i) posY = 1;
		if (comdif < i) posX = 1;
		if (strands(songdata.SongFileName[i], L"NULL") == 0
			&& i <= songdata.limit) {
			DrawGraph(550 + 11 * posX + 16 * i, 290 - 11 * posY,
				difC[i * 2 + posY], TRUE);
		}
	}
}

void DrawRate(double rate, int bar) {
	DrawGraph(320, 0, bar, TRUE);
	DrawFormatString(360, 20, GetColor(0, 0, 0), L"RATE:%.2f", rate);
	return;
}

double GetRate() {
	double ans[10] = { 0,0,0,0,0,0,0,0,0,0 };
	int e;
	FILE *fp;
	e = _wfopen_s(&fp, L"save/rateN.dat", L"rb");
	if (e == 0) {
		fread(&ans, sizeof(double), 10, fp);
		fclose(fp);
	}
	for (int i = 0; i < 10; i++) {
		if (ans[i] < 0) {
			ans[i] = 0;
		}
	}
	return ans[0] + ans[1] + ans[2] + ans[3] + ans[4] + ans[5] + ans[6] + ans[7] + ans[8] + ans[9];
}

int GetRateBarPic(const double rate) {
	int pic = NULL;
	if (rate < 25) {
		pic = LoadGraph(L"picture/MSrate1.png");
	}
	else if (25 <= rate && rate < 55) {
		pic = LoadGraph(L"picture/MSrate2.png");
	}
	else if (55 <= rate && rate < 90) {
		pic = LoadGraph(L"picture/MSrate3.png");
	}
	else if (90 <= rate && rate < 120) {
		pic = LoadGraph(L"picture/MSrate4.png");
	}
	else {
		pic = LoadGraph(L"picture/MSrate5.png");
	}
	return pic;
}

void ShowHelpBar(unsigned int Cr, int bar, int lan) {
	DrawGraph(0, 0, bar, TRUE);
	switch (int(GetNowCount() / 10000 % 3) * 10 + lan) {
	case 0:
		DrawString(5, 460, L"Enterキー: 決定, BackSpaceキー: 戻る", Cr);
		break;
	case 1:
		DrawString(5, 460, L"Enter key: start, BackSpace key: back to menu", Cr);
		break;
	case 10:
		DrawString(5, 460, L"上下キー: 曲選択, 左右キー: 難易度選択", Cr);
		break;
	case 11:
		DrawString(5, 460, L"↑↓key: music select, ←→key: dif select", Cr);
		break;
	case 20:
		DrawString(5, 460, L"Zキー: 楽曲を並び替える, Pキー+Enterキー: オートプレイ", Cr);
		break;
	case 21:
		DrawString(5, 460, L"Z key: Sort Songs, P and Enter key: Auto Play", Cr);
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

void ViewSortMode(const int mode) {
#define POS_X 550
#define POS_Y 110
	switch (mode) {
	case SORT_DEFAULT:
		DrawString(POS_X, POS_Y, L"デフォルト", GetColor(255, 255, 255));
		break;
	case SORT_LEVEL:
		DrawString(POS_X, POS_Y, L"レベル順", GetColor(255, 255, 255));
		break;
	case SORT_SCORE:
		DrawString(POS_X, POS_Y, L"スコア順", GetColor(255, 255, 255));
		break;
	}
}

#endif