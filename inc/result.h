#include <sancur.h>
#include <strcur.h>
#include <dxcur.h>
#include <fontcur.h>
#include <system.h>
#include <recr_cutin.h>
#include <RecScoreFile.h>
#include <playbox.h>

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

typedef struct rec_result_mat_s {
	DxPic_t clearRate;
	DxPic_t difBer;
	DxPic_t rank;
	DxPic_t chara;
	cur_font_cr_t fontNo = CUR_FONT_COLOR_MONO;
#if VER_1_6 == 1
	cur_font_cr_t floatfontNo = CUR_FONT_COLOR_MONO;
#endif
} rec_result_mat_t;

typedef struct rec_result_pal_s {
	TCHAR songN[64];
	struct judge_box judge;
	int Mcombo = 0;
	int noteCount = 0;
	int score = 0;
	double acc = 0;
#if VER_1_6 == 1
	double floatRank = 0;
#endif
	intx100_t newRate = 0;
	intx100_t subRate = 0;
	int gap = 0;
	int width = 0;
	rec_result_mat_t mat;
} rec_result_pal_t;

char GetCharNo();
rec_play_rank_t CalScoreRank(int score);
int CalPlayRate(judge_box judge, double DifRate);
int GetFullRate();
char JudgeClearRank(char drop, judge_box judge);
void SaveCharPlayCount(char charaNo);
void SavePlayCount(char drop, const struct judge_box *judge, int dist);
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

static now_scene_t ViewResult(rec_result_pal_t *val) {
	/* typedef */
	DxPic_t resultimg = LoadGraph(L"picture/result.png");
	DxSnd_t musicmp3 = LoadSoundMem(L"song/Balloon Art.mp3");
	/* class */
	rec_cutin_c cutin;

	InitCurFont();
	PlaySoundMem(musicmp3, DX_PLAYTYPE_LOOP);
	WaitTimer(10);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	cutin.SetIo(0);
	while (1) {
		ClearDrawScreen(); /* 描画エリアここから */

		RecRescaleDrawGraph(0, 0, resultimg, TRUE); /* back */
		RecRescaleDrawGraph(460, 20, val->mat.difBer, TRUE); /* dif ber */
		RecRescaleDrawString(100, 13, val->songN, COLOR_WHITE); /* song name */
		RecRescaleDrawCurFont(val->judge.just, 140, 52, 30, CUR_FONT_COLOR_BLUE); /* just count */
		RecRescaleDrawCurFont(val->judge.good, 140, 93, 30, CUR_FONT_COLOR_YELLOW); /* good count */
		RecRescaleDrawCurFont(val->judge.safe, 140, 134, 30, CUR_FONT_COLOR_GREEN); /* safe count */
		RecRescaleDrawCurFont(val->judge.miss, 140, 175, 30, CUR_FONT_COLOR_RED); /* miss count */
		RecRescaleDrawCurFont(val->Mcombo, 155, 215, 30, CUR_FONT_COLOR_BLUE); /* max combo */
		RecRescaleDrawCurFont(val->noteCount, 265, 215, 30, CUR_FONT_COLOR_PURPLE); /* note count */
		RecRescaleDrawFormatString(10, 320, COLOR_WHITE, L"%d.%02d",
			val->newRate / 100, val->newRate % 100); /* now runner rate */
		/* runner rate rase */
		if (0 < val->subRate) {
			RecRescaleDrawFormatString(10, 340, COLOR_WHITE, L"+%d.%02d",
				val->subRate / 100, val->subRate % 100);
		}
		else { RecRescaleDrawString(10, 340, L"not rise", COLOR_WHITE); }
		RecRescaleDrawCurFont(val->score, 310, 75, 55, val->mat.fontNo); /* score */
		RecRescaleDrawCurFont(val->acc, 430, 150, 30, val->mat.fontNo, 2); /* acc */
		RecRescaleDrawCurFont(val->gap, 510, 205, 20, CUR_FONT_COLOR_MONO); /* gap */
		RecRescaleDrawCurFont(val->width, 500, 230, 20, CUR_FONT_COLOR_MONO); /* width */
		RecRescaleDrawGraph(140, 260, val->mat.rank, TRUE); /* rank */
#if VER_1_6 == 1
		RecRescaleDrawCurFont(val->floatRank, 280, 390, 30, val->mat.floatfontNo, 3, 0); /* floatrank */
#endif
		RecRescaleDrawGraph(5, 420, val->mat.clearRate, TRUE); /* clear rate */
		RecRescaleDrawGraph(336, 252, val->mat.chara, TRUE); /* chara */

		cutin.DrawCut();

		ScreenFlip(); /* 描画エリアここまで */

		//エンターが押された
		if (CheckHitKey(KEY_INPUT_RETURN) == 1 && cutin.IsClosing() == 0) {
			cutin.SetCutTipFg(CUTIN_TIPS_ON);
			cutin.SetTipNo();
			cutin.SetIo(1);
		}
		if (cutin.IsEndAnim()) {
			StopSoundMem(musicmp3);
			DeleteSoundMem(musicmp3);
			break;
		}
		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }
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

now_scene_t result(rec_map_detail_t *map_detail, rec_play_userpal_t *userpal,
	rec_play_nameset_t *nameset, int dif, wchar_t fileN[])
{
	/* int */
	char drop = 0;
	char Clear = 0;
	char rank = CalScoreRank(userpal->score.sum);
	char CharNo = GetCharNo();
	int OldRate = GetFullRate();
	double DifRate = CAL_DIF_RATE(map_detail->mdif, map_detail->Lv) / 100; //譜面定数
	double rate = (double)CalPlayRate(userpal->judgeCount, DifRate) / 100.0;
	rec_result_pal_t result_pal;

	/* calculate */
	if (userpal->status == REC_PLAY_STATUS_DROPED) { drop = 1; }
	else { drop = 0; }

	Clear = JudgeClearRank(drop, userpal->judgeCount);

	result_pal.acc = CAL_ACC(userpal->judgeCount, map_detail->notes);
#if VER_1_6 == 1
	result_pal.floatRank = GetFloatRank(userpal->score.sum, userpal->judgeCount.miss, map_detail->notes, rank) / 1000;
#endif

	if (userpal->gap.count == 0) { userpal->gap.count = 1; }
	result_pal.gap = CAL_GAP(userpal->gap.sum, userpal->gap.count);
	result_pal.width = CAL_GAP_WIDTH(userpal->gap.sum, userpal->gap.count, userpal->gap.ssum);

	SaveScore(fileN, (char)dif, userpal->score.sum, result_pal.acc, userpal->Dscore.point, (short)rank, Clear); // スコア保存
	SavePlayCount(drop, &userpal->judgeCount, userpal->Dscore.point); // プレイ回数保存
	SaveCharPlayCount(CharNo); // キャラプレイ数保存
	SaveRate(fileN, rate); // レート保存

	result_pal.newRate = GetFullRate(); // 保存後のレート
	result_pal.subRate = result_pal.newRate - OldRate; // レートの差

	strcopy_2(nameset->songN, result_pal.songN, 64);
	result_pal.judge = userpal->judgeCount;
	result_pal.Mcombo = userpal->Mcombo;
	result_pal.noteCount = map_detail->notes;
	result_pal.score = userpal->score.sum;

	/* picture */
	switch (Clear - 1) {
	case 0:
		result_pal.mat.clearRate = LoadGraph(L"picture/DROPED.png");
		break;
	case 1:
		result_pal.mat.clearRate = LoadGraph(L"picture/CLEARED.png");
		break;
	case 2:
		result_pal.mat.clearRate = LoadGraph(L"picture/NOMISS.png");
		break;
	case 3:
		result_pal.mat.clearRate = LoadGraph(L"picture/FULLCOMBO.png");
		break;
	case 4:
		result_pal.mat.clearRate = LoadGraph(L"picture/PERFECT.png");
		break;
	default:
		result_pal.mat.clearRate = LoadGraph(L"picture/DROPED.png");
		break;
	}

	switch (dif) {
	case 0:
		result_pal.mat.difBer = LoadGraph(L"picture/difauto.png");
		break;
	case 1:
		result_pal.mat.difBer = LoadGraph(L"picture/difeasy.png");
		break;
	case 2:
		result_pal.mat.difBer = LoadGraph(L"picture/difnormal.png");
		break;
	case 3:
		result_pal.mat.difBer = LoadGraph(L"picture/difhard.png");
		break;
	case 4:
	case 5:
		result_pal.mat.difBer = LoadGraph(nameset->DifFN);
		break;
	default:
		result_pal.mat.difBer = LoadGraph(L"picture/difeasy.png");
		break;
	}

	switch (rank) {
	case 0:
		result_pal.mat.rank = LoadGraph(L"picture/rankEX.png");
		result_pal.mat.fontNo = CUR_FONT_COLOR_RAINBOW;
#if VER_1_6 == 1
		result_pal.mat.floatfontNo = CUR_FONT_COLOR_BLUE;
#endif
		break;
	case 1:
		result_pal.mat.rank = LoadGraph(L"picture/rankS.png");
		result_pal.mat.fontNo = CUR_FONT_COLOR_BLUE;
#if VER_1_6 == 1
		result_pal.mat.floatfontNo = CUR_FONT_COLOR_YELLOW;
#endif
		break;
	case 2:
		result_pal.mat.rank = LoadGraph(L"picture/rankA.png");
		result_pal.mat.fontNo = CUR_FONT_COLOR_YELLOW;
#if VER_1_6 == 1
		result_pal.mat.floatfontNo = CUR_FONT_COLOR_MONO; /* TODO: result floatfontNo(2) must be GRAY */
#endif
		break;
	case 3:
		result_pal.mat.rank = LoadGraph(L"picture/rankB.png");
		result_pal.mat.fontNo = CUR_FONT_COLOR_GREEN;
#if VER_1_6 == 1
		result_pal.mat.floatfontNo = CUR_FONT_COLOR_GREEN;
#endif
		break;
	case 4:
		result_pal.mat.rank = LoadGraph(L"picture/rankC.png");
		result_pal.mat.fontNo = CUR_FONT_COLOR_PURPLE;
#if VER_1_6 == 1
		result_pal.mat.floatfontNo = CUR_FONT_COLOR_PURPLE;
#endif
		break;
	case 5:
	default:
		result_pal.mat.rank = LoadGraph(L"picture/rankD.png");
		result_pal.mat.fontNo = CUR_FONT_COLOR_RED;
#if VER_1_6 == 1
		result_pal.mat.floatfontNo = CUR_FONT_COLOR_RED;
#endif
		break;
	}

	switch (CharNo) {
	case 0:
		result_pal.mat.chara = LoadGraph(L"picture/RePicker.png");
		break;
	case 1:
		result_pal.mat.chara = LoadGraph(L"picture/ReGator.png");
		break;
	case 2:
		result_pal.mat.chara = LoadGraph(L"picture/ReTaylor.png");
		break;
	default:
		result_pal.mat.chara = -1;
		break;
	}

	return ViewResult(&result_pal);
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
	if (fp != NULL) {
		fwrite(&chap, sizeof(int), 3, fp);
		fclose(fp);
	}
	return;
}

void SavePlayCount(char drop, const struct judge_box *judge, int dist) {
	rec_user_data_t data;
	FILE* fp;
	(void)_wfopen_s(&fp, L"save/data.dat", L"rb");
	if (fp != NULL) {
		(void)fread(&data, sizeof(rec_user_data_t), 1, fp);
		(void)fclose(fp);
	}
	data.playCount++;
	if (drop == 1) { data.dropCount++; }
	else { data.clearCount++; }
	if (judge->miss == 0) {
		data.NMCount++;
		if (judge->safe == 0) {
			data.FCCount++;
			if (judge->good == 0) { data.PFcount++; }
		}
	}
	data.mileage += dist;

	(void)_wfopen_s(&fp, L"save/data.dat", L"wb");
	if (fp == NULL) {
		(void)fwrite(&data, sizeof(rec_user_data_t), 1, fp);
		(void)fclose(fp);
	}
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