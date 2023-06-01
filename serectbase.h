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
	wchar_t packName[256];
	wchar_t SongName[6][256];
	wchar_t artist[6][256];
	wchar_t SongFileName[6][256];
	wchar_t jacketP[6][256];
}MUSIC_BOX;

void ChangeSortMode(int *mode);
void DrawBackPicture(int img);
void DrawDifMaker(MUSIC_BOX songdata, int comdif, int comnum,
	const int *difC);
void DrawSongBar(int cmd1, int cmd2, int SongNumCount, int UD, int moveC,
	int *bar, MUSIC_BOX *songdata, int *Mapping, int *CRatepic, int *CRankpic);
void DrawRate(int rate, int bar, int chap, int charaimg);
int GetRate();
int GetRateBarPic(int rate);
void ShowHelpBar(unsigned int Cr, int bar, int lan);
void SortSong(MUSIC_BOX *songdata, int *mapping, const int mode,
	const int dif, const int SongNumCount);
void ViewSortMode(int mode, int lan);

/* +++++ */

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

void DrawSongBar(int cmd1, int cmd2, int SongNumCount, int UD, int moveC,
	int *bar, MUSIC_BOX *songdata, int *Mapping, int *CRatepic, int *CRankpic) {
	int BasePosX = 0;
	int BasePosY = 0;
	int BarColor = 0;
	int MarkPosX = 0;
	int MarkPosY = 0;
	unsigned int Cr[2] = {
		GetColor(255, 255, 255),
		GetColor(0, 0, 0)
	};
	int picsong = (cmd1 - 3) % SongNumCount;
	if (picsong < 0) { picsong += SongNumCount; }
	for (int i = 0; i < 7; i++) {
		if (i < 3) {
			BasePosY = pals(0, i * 80 + 120, 250, i * 80 + UD * 80 + 120,
				moveC);
			BarColor = 0;
			MarkPosX = 152;
			MarkPosY = 163;
		}
		else if (i == 3) {
			BasePosY = pals(0, i * 120, 250, i * 120 + UD * 80, moveC);
			BarColor = 1;
			MarkPosX = 156;
			MarkPosY = 132;
		}
		else {
			BasePosY = pals(0, i * 80 + 160, 250, i * 80 + UD * 80 + 160,
				moveC);
			BarColor = 0;
			MarkPosX = 152;
			MarkPosY = 163;
		}
		BasePosX = lins(480, 80, 240, 40, BasePosY);
		DrawGraph(BasePosX - 120, BasePosY - 170, bar[BarColor], TRUE);
		DrawString(BasePosX - 30, BasePosY - 157,
			songdata[Mapping[picsong]].SongName[cmd2], Cr[BarColor]);
		DrawString(BasePosX - 30, BasePosY - 129,
			songdata[Mapping[picsong]].artist[cmd2], Cr[BarColor]);
		if (1 <= songdata[Mapping[picsong]].ClearRate[cmd2]) {
			DrawGraph(BasePosX + MarkPosX, BasePosY - MarkPosY,
				CRatepic[songdata[Mapping[picsong]].ClearRate[cmd2] - 1],
				TRUE);
		}
		if (0 <= songdata[Mapping[picsong]].ClearRank[cmd2]
			&& songdata[Mapping[picsong]].ClearRank[cmd2] <= 5) {
			DrawGraph(BasePosX + MarkPosX, BasePosY - MarkPosY,
				CRankpic[songdata[Mapping[picsong]].ClearRank[cmd2]],
				TRUE);
		}
		if (i == 3) {
			for (int j = 0; j < 3; j++) {
				DrawFormatString(BasePosX - 25 + j * 70, BasePosY - 97, Cr[1],
					L"%2d", songdata[Mapping[cmd1]].level[1 + j]);
			}
		}
		picsong = (picsong + 1) % SongNumCount;
	}
	return;
}

/* レートだけじゃなくなってるw */
void DrawRate(int rate, int bar, int chap, int charaimg) {
	DrawGraph(250, 0, bar, TRUE);
	DrawGraph(370, 0, charaimg, TRUE);
	DrawFormatString(265, 12, GetColor(0, 0, 0), L"Lv:%2d", chap + 1);
	DrawFormatString(270, 37, GetColor(0, 0, 0), L"RATE:%d.%02d", rate / 100, rate % 100);
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

int GetRateBarPic(int rate) {
	int pic = NULL;
	if (rate < 2500) {
		pic = LoadGraph(L"picture/MSrate1.png");
	}
	else if (2500 <= rate && rate < 5500) {
		pic = LoadGraph(L"picture/MSrate2.png");
	}
	else if (5500 <= rate && rate < 9000) {
		pic = LoadGraph(L"picture/MSrate3.png");
	}
	else if (9000 <= rate && rate < 12000) {
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

void ViewSortMode(int mode, int lan) {
#define POS_X 550
#define POS_Y 110
	if (lan == 1) {
		switch (mode) {
		case SORT_DEFAULT:
			DrawString(POS_X, POS_Y, L"default", GetColor(255, 255, 255));
			break;
		case SORT_LEVEL:
			DrawString(POS_X, POS_Y, L"level", GetColor(255, 255, 255));
			break;
		case SORT_SCORE:
			DrawString(POS_X, POS_Y, L"score", GetColor(255, 255, 255));
			break;
		}
	}
	else {
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
}

#endif