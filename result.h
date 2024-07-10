#include "fontcur.h"
#include "system.h"
#include "recr_cutin.h"
#include "RecPlay/playbox.h"

#define VER_1_6 0

#define CAL_ACC(judge, notes)												\
	(((judge).just * 10000 + (judge).good * 9500 +							\
	(judge).safe * 5500) / ((notes) * 100.0))
#define CAL_DIF_RATE(mdif, Lv)												\
	((Lv) == 0 ? 0 : maxs((mdif), (Lv) * 100 + 90))
#define CAL_GAP(all_gap, count) ((all_gap) / (count))
#define CAL_GAP_WIDTH(all_gap, count, all_d_gap)							\
	(((all_d_gap) / (count)) - ((all_gap) * (all_gap)) / ((count) * (count)))

typedef enum rec_play_rank_e {
	REC_PLAY_RANK_EX = 0,
	REC_PLAY_RANK_S,
	REC_PLAY_RANK_A,
	REC_PLAY_RANK_B,
	REC_PLAY_RANK_C,
	REC_PLAY_RANK_D,
	REC_PLAY_RANK_F,
} rec_play_rank_t;

char GetCharNo();
rec_play_rank_t CalScoreRank(int score);
int CalPlayRate(judge_box judge, double DifRate);
int GetFullRate();
char JudgeClearRank(char drop, judge_box judge);
void SaveCharPlayCount(char charaNo);
void SavePlayCount(char drop, struct judge_box const* const judge);
void SaveRate(wchar_t songN[], double rate);
void SaveScore(wchar_t const songN[], char dif,
	int score, double acc, int Dscore, short rank, char Clear);

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

now_scene_t ViewResult(int dif, wchar_t DifFN[255], wchar_t songN[255],
	struct judge_box const* const judge, int Mcombo, short int notes,
	int NewRate, int RateSub, char rank, double floatrank, int score,
	double acc, int gap, int gap_width, short int Clear, char charNo)
{
	/* num */
	char closeFg = 0;
	int G[10] = { 0,0,0,0,0,0,0,0,0,0 };
	int CutTime = 0;
	/* typedef */
	cur_font_cr_t fontNo;
	cur_font_cr_t floatfontNo;
	switch (rank) {
	case 0:
		fontNo = CUR_FONT_COLOR_RAINBOW;
		floatfontNo = CUR_FONT_COLOR_BLUE;
		break;
	case 1:
		fontNo = CUR_FONT_COLOR_BLUE;
		floatfontNo = CUR_FONT_COLOR_YELLOW;
		break;
	case 2:
		fontNo = CUR_FONT_COLOR_YELLOW;
		floatfontNo = CUR_FONT_COLOR_MONO; /* TODO: result floatfontNo(2) must be GRAY */
		break;
	case 3:
		fontNo = CUR_FONT_COLOR_GREEN;
		floatfontNo = CUR_FONT_COLOR_GREEN;
		break;
	case 4:
		fontNo = CUR_FONT_COLOR_PURPLE;
		floatfontNo = CUR_FONT_COLOR_PURPLE;
		break;
	case 5:
		fontNo = CUR_FONT_COLOR_RED;
		floatfontNo = CUR_FONT_COLOR_RED;
		break;
	default:
		fontNo = CUR_FONT_COLOR_MONO;
		floatfontNo = CUR_FONT_COLOR_MONO;
		break;
	}
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
	PlaySoundMem(musicmp3, DX_PLAYTYPE_LOOP);
	WaitTimer(10);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	CutTime = GetNowCount();
	while (1) {
		ClearDrawScreen();
		RecRescaleDrawGraph(0, 0, resultimg, TRUE); /* back */
		RecRescaleDrawGraph(460, 20, difberimg, TRUE); /* dif ber */
		RecRescaleDrawString(100, 13, songN, COLOR_WHITE); /* song name */
		DrawCurFont(judge->just, 140, 52, 30, CUR_FONT_COLOR_BLUE); /* just count */
		DrawCurFont(judge->good, 140, 93, 30, CUR_FONT_COLOR_YELLOW); /* good count */
		DrawCurFont(judge->safe, 140, 134, 30, CUR_FONT_COLOR_GREEN); /* safe count */
		DrawCurFont(judge->miss, 140, 175, 30, CUR_FONT_COLOR_RED); /* miss count */
		DrawCurFont(Mcombo, 155, 215, 30, CUR_FONT_COLOR_BLUE); /* max combo */
		DrawCurFont(notes, 265, 215, 30, CUR_FONT_COLOR_PURPLE); /* note count */
		RecRescaleDrawFormatString(10, 320, COLOR_WHITE, L"%d.%02d",
			NewRate / 100, NewRate % 100); /* now runner rate */
		/* runner rate rase */
		if (0 < RateSub) {
			RecRescaleDrawFormatString(10, 340, COLOR_WHITE, L"+%d.%02d",
				RateSub / 100, RateSub % 100);
		}
		else { RecRescaleDrawString(10, 340, L"not rise", COLOR_WHITE); }
		DrawCurFont(score, 310, 75, 55, fontNo); /* score */
		DrawCurFont(acc, 430, 150, 30, fontNo, 2); /* acc */
		DrawCurFont(gap, 510, 205, 20, CUR_FONT_COLOR_MONO); /* gap */
		DrawCurFont(gap_width, 500, 230, 20, CUR_FONT_COLOR_MONO); /* width */
		RecRescaleDrawGraph(140, 260, rankimg, TRUE); /* rank */
#if VER_1_6 == 1
		DrawCurFont(floatrank, 280, 390, 30, floatfontNo, 3, 0); /* floatrank */
#endif
		RecRescaleDrawGraph(5, 420, coleimg, TRUE); /* clear rate */
		RecRescaleDrawGraph(336, 252, Rchaimg, TRUE); /* chara */
		if (closeFg == 0) { ViewCutOut(CutTime); }
		if (closeFg == 1) { ViewCutIn(CutTime); }
		ScreenFlip();
		//エンターが押された
		if (CheckHitKey(KEY_INPUT_RETURN) == 1) {
			SetCutTipFg(CUTIN_TIPS_ON);
			SetTipNo();
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
	INIT_PIC();
	return SCENE_SERECT;
}

double GetFloatRank(int score, int miss, int notes, char rank) {
	switch (rank) {
	case REC_PLAY_RANK_EX:
		return score / 2 - 49000;
	case REC_PLAY_RANK_S:
		return (score - 95000) / 3;
	case REC_PLAY_RANK_A:
		return score / 5 - 18000;
	case REC_PLAY_RANK_B:
		return score / 10 - 8000;
	case REC_PLAY_RANK_C:
		return score / 20 - 3000;
	case REC_PLAY_RANK_D:
		if (50000 <= score) {
			return score * 3 / 100 + 800;
		}
		else if (40000 <= score) {
			return score / 50 - 300;
		}
		else {
			return score / 80;
		}
	case REC_PLAY_RANK_F:
		return pals(0, 1000, 100, 0, 100 * miss / notes);
	}
	return 0;
}

now_scene_t result(int dif, short Lv, char drop, int difkey,
	wchar_t songN[], wchar_t DifFN[], wchar_t fileN[],
	struct judge_box judge, int score, int Mcombo, short notes,
	int gapa[], int Dscore) {
	/* arg fix */
	if (gapa[1] == 0) { gapa[1] = 1; }
	/* int */
	char Clear = JudgeClearRank(drop, judge);
	char CharNo = GetCharNo();
	char rank = CalScoreRank(score);
	int OldRate = GetFullRate();
	int NewRate = 0;
	int RateSub = 0;
	int gap = CAL_GAP(gapa[0], gapa[1]);
	int gap_width = CAL_GAP_WIDTH(gapa[0], gapa[1], gapa[2]);
	double floatrank = GetFloatRank(score, judge.miss, notes, rank) / 1000;
	double acc = CAL_ACC(judge, notes);
	double DifRate = CAL_DIF_RATE(difkey, Lv) / 100; //譜面定数
	double rate = (double)CalPlayRate(judge, DifRate) / 100.0;
	/* action */
	SaveScore(fileN, (char)dif, score, acc, Dscore, (short)rank, Clear); // スコア保存
	SavePlayCount(drop, &judge); // プレイ回数保存
	SaveCharPlayCount(CharNo); // キャラプレイ数保存
	SaveRate(fileN, rate); // レート保存
	NewRate = GetFullRate(); // 保存後のレート
	RateSub = NewRate - OldRate; // レートの差
	return ViewResult(dif, DifFN, songN, &judge, Mcombo, notes, NewRate,
		RateSub, (short)rank, floatrank, score, acc, gap, gap_width, Clear - 1,
		CharNo); // リザルト画面
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

rec_play_rank_t CalScoreRank(int score) {
	if (score >= 98000) { return REC_PLAY_RANK_EX; }
	if (score >= 95000) { return REC_PLAY_RANK_S; }
	if (score >= 90000) { return REC_PLAY_RANK_A; }
	if (score >= 85000) { return REC_PLAY_RANK_B; }
	if (score >= 80000) { return REC_PLAY_RANK_C; }
	return REC_PLAY_RANK_D;
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
	int G = 0;
	play_rate_t data[RATE_NUM];
	FILE* fp;
	(void)_wfopen_s(&fp, RATE_FILE_NAME, L"rb");
	if (fp != NULL) {
		fread(&data, sizeof(play_rate_t), RATE_NUM, fp);
		fclose(fp);
	}
	// 同じ曲、または未収録を探す
	for (char i = 0; i < RATE_NUM; i++) {
		if (strands(songN, data[i].name) ||
			(data[i].name[0] == L'\0' && data[i].num == 0)) {
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
		G = _wfopen_s(&fp, RATE_FILE_NAME, L"wb");
		if (fp == NULL) {
			while (1) {}
		}
		fwrite(&data, sizeof(play_rate_t), RATE_NUM, fp);
		fclose(fp);
	}
}

void SaveScore(wchar_t const songN[], char dif,
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
