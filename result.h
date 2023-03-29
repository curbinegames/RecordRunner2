#include "define.h"
#include "fontcur.h"
#include "playbox.h"
#include "system.h"
#include "recr_cutin.h"

#define CAL_ACC(judge, notes)												\
	(((judge).just * 10000 + (judge).good * 9500 +							\
	(judge).safe * 5500) / ((notes) * 100.0))
#define CAL_DIF_RATE(mdif, Lv)												\
	((Lv) == 0 ? 0 : maxs((mdif), (Lv) * 100 + 90))

char CalScoreRank(int score);
int CalPlayRate(judge_box judge, double DifRate);
int GetFullRate();
char JudgeClearRank(char drop, judge_box judge);
void SaveCharPlayCount(char charaNo);
void SavePlayCount(char drop, struct judge_box const* const judge);
void SaveRate(wchar_t songN[], double rate);
void SaveScore(wchar_t songN[], char dif,
	int score, double acc, int Dscore, short rank, char Clear);

now_scene_t ViewResult(int dif, wchar_t DifFN[255], wchar_t songN[255],
	struct judge_box const* const judge, int Mcombo, short int notes,
	int NewRate, int RateSub, char rank, int score, double acc,
	int gapa[], short int Clear, char charNo) {
	/* num */
	char closeFg = 0;
	short int fontNo;
	switch (rank) {
	case 0:
		fontNo = 6;
		break;
	case 1:
		fontNo = 4;
		break;
	case 2:
		fontNo = 2;
		break;
	case 3:
		fontNo = 3;
		break;
	case 4:
		fontNo = 5;
		break;
	case 5:
		fontNo = 1;
		break;
	default:
		fontNo = 4;
		break;
	}
	int G[10] = { 0,0,0,0,0,0,0,0,0,0 };
	int CutTime = 0;
	/* image */
	int coleimg;
	switch (Clear) {
	case 0:
		coleimg = LoadGraph(L"picture/DROPED.png");
		break;
	case 1:
		coleimg = LoadGraph(L"picture/CLEARED.png");
		break;
	case 2:
		coleimg = LoadGraph(L"picture/NOMISS.png");
		break;
	case 3:
		coleimg = LoadGraph(L"picture/FULLCOMBO.png");
		break;
	case 4:
		coleimg = LoadGraph(L"picture/PERFECT.png");
		break;
	default:
		coleimg = LoadGraph(L"picture/DROPED.png");
		break;
	}
	int	difberimg;
	switch (dif) {
	case 0:
		difberimg = LoadGraph(L"picture/difauto.png");
		break;
	case 1:
		difberimg = LoadGraph(L"picture/difeasy.png");
		break;
	case 2:
		difberimg = LoadGraph(L"picture/difnormal.png");
		break;
	case 3:
		difberimg = LoadGraph(L"picture/difhard.png");
		break;
	case 4:
	case 5:
		difberimg = LoadGraph(DifFN);
		break;
	default:
		difberimg = LoadGraph(L"picture/difeasy.png");
		break;
	}
	int rankimg;
	switch (rank) {
	case 0:
		rankimg = LoadGraph(L"picture/rankEX.png");
		break;
	case 1:
		rankimg = LoadGraph(L"picture/rankS.png");
		break;
	case 2:
		rankimg = LoadGraph(L"picture/rankA.png");
		break;
	case 3:
		rankimg = LoadGraph(L"picture/rankB.png");
		break;
	case 4:
		rankimg = LoadGraph(L"picture/rankC.png");
		break;
	case 5:
	default:
		rankimg = LoadGraph(L"picture/rankD.png");
		break;
	}
	int Rchaimg;
	switch (charNo) {
	case 0:
		Rchaimg = LoadGraph(L"picture/RePicker.png");
		break;
	case 1:
		Rchaimg = LoadGraph(L"picture/ReGator.png");
		break;
	case 2:
		Rchaimg = LoadGraph(L"picture/ReTaylor.png");
		break;
	default:
		Rchaimg = LoadGraph(L"picture/RePicker.png");
		break;
	}
	int resultimg = LoadGraph(L"picture/result.png");
	/* audio */
	int musicmp3 = LoadSoundMem(L"song/Balloon Art.mp3");
	CutinReady();
	PlaySoundMem(musicmp3, DX_PLAYTYPE_LOOP);
	WaitTimer(10);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	CutTime = GetNowCount();
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, resultimg, TRUE);
		DrawGraph(460, 20, difberimg, TRUE);
		DrawString(100, 13, songN, COLOR_WHITE);
		DrawCurFont(judge->just, 140, 52, 30, 4);
		DrawCurFont(judge->good, 140, 93, 30, 2);
		DrawCurFont(judge->safe, 140, 134, 30, 3);
		DrawCurFont(judge->miss, 140, 175, 30, 1);
		DrawCurFont(Mcombo, 155, 215, 30, 4);
		DrawCurFont(notes, 265, 215, 30, 5);
		DrawFormatString(10, 320, COLOR_WHITE, L"%d.%2d",
			NewRate / 100, NewRate % 100);
		if (0 < RateSub) {
			DrawFormatString(10, 340, COLOR_WHITE, L"+%d.%2d",
				RateSub / 100, RateSub % 100);
		}
		else { DrawString(10, 340, L"not rise", COLOR_WHITE); }
		DrawCurFont(score, 310, 75, 55, fontNo);
		DrawCurFont(acc, 430, 150, 30, fontNo, 2);
		if (gapa[1] == 0) { gapa[1] = 1; }
		DrawCurFont(gapa[0] / gapa[1], 510, 205, 20, 0);
		DrawCurFont((gapa[2] / gapa[1]) - (gapa[0] * gapa[0]) / (gapa[1] * gapa[1]),
			500, 230, 20, 0);
		DrawGraph(140, 260, rankimg, TRUE);
		DrawGraph(5, 420, coleimg, TRUE);
		DrawGraph(336, 252, Rchaimg, TRUE);
		if (closeFg == 0) {
			ViewCutOut(CutTime);
		}
		if (closeFg == 1) {
			ViewCutIn(CutTime);
		}
		ScreenFlip();
		//エンターが押された
		if (CheckHitKey(KEY_INPUT_RETURN) == 1) {
			closeFg = 1;
			CutTime = GetNowCount();
		}
		if (closeFg == 1 && CutTime + 2000 <= GetNowCount()) {
			StopSoundMem(musicmp3);
			DeleteSoundMem(musicmp3);
			break;
		}
		if (GetWindowUserCloseFlag(TRUE)) return SCENE_EXIT;
	}
	InitGraph();
	return SCENE_SERECT;
}

now_scene_t result(int dif, short Lv, char drop, int difkey,
	wchar_t songN[255], wchar_t DifFN[255], struct judge_box judge,
	int score, int Mcombo, short notes, int gapa[3], int Dscore) {
	char Clear = JudgeClearRank(drop, judge);
	char CharNo = GetCharNo();
	char rank = CalScoreRank(score);
	int OldRate = GetFullRate();
	int NewRate = 0;
	int RateSub = 0;
	double acc = CAL_ACC(judge, notes);
	double DifRate = CAL_DIF_RATE(difkey, Lv) / 100; //譜面定数
	double rate = (double)CalPlayRate(judge, DifRate) / 100.0;
	SaveScore(songN, (char)dif, score, acc, Dscore, (short)rank, Clear); // スコア保存
	SavePlayCount(drop, &judge); // プレイ回数保存
	SaveCharPlayCount(CharNo); // キャラプレイ数保存
	SaveRate(songN, rate); // レート保存
	NewRate = GetFullRate(); // 保存後のレート
	RateSub = NewRate - OldRate; // レートの差
	return ViewResult(dif, DifFN, songN, &judge, Mcombo, notes, NewRate,
		RateSub, (short)rank, score, acc, gapa, Clear - 1, CharNo); // リザルト画面
}

int CalPlayRate(judge_box judge, double DifRate) {
	double rate = 0;
	// level0なら0固定
	if (DifRate == 0) rate = 0;
	// "譜面定数" - "miss数" x "譜面定数" x 0.03 (下限=0)
	else if (judge.miss > 0) {
		rate = DifRate - judge.miss * DifRate * 0.03;
		rate = mins_D(rate, 0);
	}
	// NO MISS,"譜面定数" + 1 - "safe数" x 0.05 (下限="譜面定数")
	else if (judge.miss == 0 && judge.safe > 0) {
		rate = DifRate + 1 - judge.safe * 0.05;
		rate = mins_D(rate, DifRate);
	}
	// FULL COMBO,"譜面定数" + 2 - "good数" x 0.01 (下限="譜面定数" + 1)
	else if (judge.miss == 0 && judge.safe == 0 && judge.good > 0) {
		rate = DifRate + 2 - judge.good * 0.01;
		rate = mins_D(rate, DifRate + 1);
	}
	// PERFECT, "譜面定数" + 2
	else if (judge.miss == 0 && judge.safe == 0 && judge.good == 0) {
		rate = DifRate + 2;
	}
	return (int)(rate * 100);
}

char CalScoreRank(int score) {
	if (score >= 98000) { return 0; }
	if (score >= 95000) { return 1; }
	if (score >= 90000) { return 2; }
	if (score >= 85000) { return 3; }
	if (score >= 80000) { return 4; }
	return 5;
}

int GetFullRate() {
	int ret = 0;
	play_rate_t data[RATE_NUM];
	FILE* fp;
	(void)_wfopen_s(&fp, RATE_FILE_NAME, L"rb");
	if (fp != NULL) {
		fread(&data, sizeof(play_rate_t), RATE_NUM, fp);
		fclose(fp);
	}
	for (int i = 0; i < RATE_NUM; i++) {
		ret += mins((int)(data[i].num * 100), 0);
	}
	return ret / 2;
}

char JudgeClearRank(char drop, judge_box judge) {
	if (drop == 1) { return 1; }
	if (0 < judge.miss) { return 2; }
	if (0 < judge.safe) { return 3; }
	if (0 < judge.good) { return 4; }
	return 5;
}

void SaveCharPlayCount(char charaNo) {
	int chap[3] = { 0,0,0 };
	FILE* fp;
	(void)_wfopen_s(&fp, L"save/chap.dat", L"rb");
	if (fp != NULL) {
		fread(&chap, sizeof(chap), 3, fp);
		fclose(fp);
	}
	chap[charaNo]++;
	_wfopen_s(&fp, L"save/chap.dat", L"wb");
	fwrite(&chap, sizeof(int), 3, fp);
	fclose(fp);
	return;
}

void SavePlayCount(char drop, struct judge_box const* const judge) {
	int	data[7] = { 0,0,0,0,0,0,0 };
	FILE* fp;
	(void)_wfopen_s(&fp, L"save/data.dat", L"rb");
	if (fp != NULL) {
		(void)fread(&data, sizeof(data), 7, fp);
		(void)fclose(fp);
	}
	data[0]++;
	if (drop == 1) { data[1]++; }
	else { data[3]++; }
	if (judge->miss == 0) {
		data[4]++;
		if (judge->safe == 0) {
			data[5]++;
			if (judge->good == 0) { data[6]++; }
		}
	}
	(void)_wfopen_s(&fp, L"save/data.dat", L"wb");
	(void)fwrite(&data, sizeof(int), 7, fp);
	(void)fclose(fp);
	return;
}

void SaveRate(wchar_t songN[], double rate) {
	char num = -1;
	play_rate_t data[RATE_NUM];
	FILE* fp;
	// 同じ曲、または未収録を探す
	for (char i = 0; i < RATE_NUM; i++) {
		if ((strands(songN, data[i].name) || (data[i].name[0] == L'\0')) &&
			data[i].num == 0) {
			num = i;
			break;
		}
	}
	// なかったら、一番低いレートを探す
	if (num == -1) {
		num = 0;
		for (char i = 1; i < RATE_NUM; i++) {
			if (data[num].num > data[i].num) { num = i; }
		}
	}
	// レートが高かったら更新する
	if (data[num].num < rate) {
		data[num].num = rate;
		strcopy(songN, data[num].name, 1);
		_wfopen_s(&fp, RATE_FILE_NAME, L"wb");
		fwrite(&data, sizeof(play_rate_t), RATE_NUM, fp);
		fclose(fp);
	}
}

void SaveScore(wchar_t songN[], char dif,
	int score, double acc, int Dscore, short rank, char Clear) {
	int	read[7] = { 0,0,0,0,0,0,0 };
	int	Readdis[7] = { 0,0,0,0,0,0,0 };
	int	ReadRank[7] = { 6,6,6,6,6,6,6 };
	int	ReadClear[7] = { 0,0,0,0,0,0,0 };
	double ReadAcc[7] = { 0,0,0,0,0,0,0 };
	wchar_t save[255] = L"score/";
	strcats(save, songN); // save = score/<曲名>
	strcats(save, L".dat"); // save = score/<曲名>.dat
	FILE* fp;
	(void)_wfopen_s(&fp, save, L"rb");
	if (fp != NULL) {
		fread(&read, sizeof(int), 6, fp);
		fread(&ReadAcc, sizeof(double), 6, fp);
		fread(&Readdis, sizeof(int), 6, fp);
		fread(&ReadRank, sizeof(int), 6, fp);
		fread(&ReadClear, sizeof(int), 6, fp);
		fclose(fp);
	}
	if (read[dif] < score) { read[dif] = score; } //ハイスコア保存
	if (ReadAcc[dif] < acc) { ReadAcc[dif] = acc; } //ACC保存
	if (Readdis[dif] < Dscore) { Readdis[dif] = Dscore; } //最長走行距離保存
	if (ReadRank[dif] > rank || ReadRank[dif] < 0) {
		ReadRank[dif] = rank; //ランク保存
	}
	if (ReadClear[dif] < Clear) { ReadClear[dif] = Clear; } //クリアレート保存
	_wfopen_s(&fp, save, L"wb");
	fwrite(&read, sizeof(int), 6, fp);
	fwrite(&ReadAcc, sizeof(double), 6, fp);
	fwrite(&Readdis, sizeof(int), 6, fp);
	fwrite(&ReadRank, sizeof(int), 6, fp);
	fwrite(&ReadClear, sizeof(int), 6, fp);
	fclose(fp);
	return;
}
