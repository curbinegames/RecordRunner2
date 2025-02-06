#include <sancur.h>
#include <strcur.h>
#include <dxcur.h>
#include <fontcur.h>
#include <system.h>
#include <recr_cutin.h>
#include <RecScoreFile.h>
#include <playbox.h>
#include <RecWindowRescale.h>

// TODO: レートが更新されない!!

#define VER_1_6 0

#define CAL_ACC(judge, notes)												\
	(((judge).just * 10000 + (judge).good * 9500 +							\
	(judge).safe * 5500) / ((notes) * 100.0))
#define CAL_DIF_RATE(mdif, Lv)												\
	((Lv) == 0 ? 0 : maxs((mdif), (Lv) * 100 + 90))
#define CAL_GAP(all_gap, count) ((all_gap) / (count))
#define CAL_GAP_WIDTH(all_gap, count, all_d_gap)							\
	(((all_d_gap) / (count)) - ((all_gap) * (all_gap)) / ((count) * (count))) /* TODO:この値にルート入れたい */

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
	DxPic_t clearRate = -1;
	DxPic_t difBer = -1;
	DxPic_t rank = -1;
	DxPic_t chara = -1;
	DxSnd_t BGM = -1;
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
	intx100_t oldRate = 0;
	intx100_t subRate = 0;
	int gap = 0;
	int width = 0;
	rec_result_mat_t mat;
} rec_result_pal_t;

static now_scene_t ViewResult(const rec_result_pal_t *val) {
	/* typedef */
	DxPic_t resultimg = LoadGraph(L"picture/result.png");
	/* class */
	rec_cutin_c cutin;

	InitCurFont();
	PlaySoundMem(val->mat.BGM, DX_PLAYTYPE_LOOP);
	WaitTimer(10);
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
		RecRescaleDrawCurFont(val->Mcombo,     155, 215, 30, CUR_FONT_COLOR_BLUE); /* max combo */
		RecRescaleDrawCurFont(val->noteCount,  265, 215, 30, CUR_FONT_COLOR_PURPLE); /* note count */
		RecRescaleDrawFormatString(10, 320, COLOR_WHITE, L"%d.%02d",
			val->newRate / 100, val->newRate % 100); /* now runner rate */
		/* runner rate rase */
		if (0 < val->subRate) {
			RecRescaleDrawFormatString(10, 340, COLOR_WHITE, L"+%d.%02d",
				val->subRate / 100, val->subRate % 100);
		}
		else { RecRescaleDrawString(10, 340, L"not rise", COLOR_WHITE); }
		RecRescaleDrawCurFont(val->score, 310,  75, 55, val->mat.fontNo); /* score */
		RecRescaleDrawCurFont(val->acc,   430, 150, 30, val->mat.fontNo, 2); /* acc */
		RecRescaleDrawCurFont(val->gap,   510, 205, 20, CUR_FONT_COLOR_MONO); /* gap */
		RecRescaleDrawCurFont(val->width, 500, 230, 20, CUR_FONT_COLOR_MONO); /* width */
		RecRescaleDrawGraph(140, 260, val->mat.rank,      TRUE); /* rank */
#if VER_1_6 == 1
		RecRescaleDrawCurFont(val->floatRank, 280, 390, 30, val->mat.floatfontNo, 3, 0); /* floatrank */
#endif
		RecRescaleDrawGraph(5,   420, val->mat.clearRate, TRUE); /* clear rate */
		RecRescaleDrawGraph(336, 252, val->mat.chara,     TRUE); /* chara */

		cutin.DrawCut();

		ScreenFlip(); /* 描画エリアここまで */

		//エンターが押された
		if (CheckHitKey(KEY_INPUT_RETURN) == 1 && cutin.IsClosing() == 0) {
			cutin.SetCutTipFg(CUTIN_TIPS_ON);
			cutin.SetTipNo();
			cutin.SetIo(1);
		}
		if (cutin.IsEndAnim()) {
			StopSoundMem(val->mat.BGM);
			break;
		}
		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }
	}

	INIT_PIC();

	return SCENE_SERECT;
}

#if 1 /* RecResultCalParameter */

static rec_play_rank_t CalScoreRank(int score) {
	if (score >= 98000) { return REC_PLAY_RANK_EX; }
	if (score >= 95000) { return REC_PLAY_RANK_S; }
	if (score >= 90000) { return REC_PLAY_RANK_A; }
	if (score >= 85000) { return REC_PLAY_RANK_B; }
	if (score >= 80000) { return REC_PLAY_RANK_C; }
	return REC_PLAY_RANK_D;
}

static int GetFullRate(void) {
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
	return ret * 10 / RATE_NUM;
}

static int CalPlayRate(const judge_box judge, const rec_map_detail_t *map_detail) {
	const double DifRate = CAL_DIF_RATE(map_detail->mpal.mdif, map_detail->Lv) / 100.0;

	double rate = 0;
	// level0なら0固定
	if (DifRate == 0) { rate = 0; }
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

static char JudgeClearRank(const rec_play_userpal_t *userpal) {
	const judge_box *judge = &userpal->judgeCount;
	if (userpal->status == REC_PLAY_STATUS_DROPED) { return 1; } /* droped */
	if (0 < userpal->judgeCount.miss) { return 2; } /* cleared */
	if (0 < userpal->judgeCount.safe) { return 3; } /* no miss */
	if (0 < userpal->judgeCount.good) { return 4; } /* full combo */
	return 5; /* perfect */
}

#if VER_1_6 == 1
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
#endif

static DxPic_t RecResultLoadClearRateGraph(char Clear) {
	switch (Clear) {
	case 1: /* TODO:enum対象 */
		return LoadGraph(L"picture/DROPED.png");
	case 2:
		return LoadGraph(L"picture/CLEARED.png");
	case 3:
		return LoadGraph(L"picture/NOMISS.png");
	case 4:
		return LoadGraph(L"picture/FULLCOMBO.png");
	case 5:
		return LoadGraph(L"picture/PERFECT.png");
	default:
		return LoadGraph(L"picture/DROPED.png");
	}
}

static DxPic_t RecResultLoadDifBarGraph(int dif, const rec_play_nameset_t *nameset) {
	switch (dif) {
	case 0: /* TODO:enum対象 */
		return LoadGraph(L"picture/difauto.png");
	case 1:
		return LoadGraph(L"picture/difeasy.png");
	case 2:
		return LoadGraph(L"picture/difnormal.png");
	case 3:
		return LoadGraph(L"picture/difhard.png");
	case 4:
	case 5:
		return LoadGraph(nameset->DifFN); /* TODO: DifFNを直接引数にする */
	default:
		return LoadGraph(L"picture/difeasy.png");
	}
}

static DxPic_t RecResultLoadClearRankGraph(rec_play_rank_t rank) {
	switch (rank) {
	case REC_PLAY_RANK_EX:
		return LoadGraph(L"picture/rankEX.png");
	case REC_PLAY_RANK_S:
		return LoadGraph(L"picture/rankS.png");
	case REC_PLAY_RANK_A:
		return LoadGraph(L"picture/rankA.png");
	case REC_PLAY_RANK_B:
		return LoadGraph(L"picture/rankB.png");
	case REC_PLAY_RANK_C:
		return LoadGraph(L"picture/rankC.png");
	case REC_PLAY_RANK_D:
	default:
		return LoadGraph(L"picture/rankD.png");
	}
}

static DxPic_t RecResultLoadCharaGraph(void) {
	switch (optiondata.chara) {
	case 0: /* TODO:enum対象 */
		return LoadGraph(L"picture/RePicker.png");
	case 1:
		return LoadGraph(L"picture/ReGator.png");
	case 2:
		return LoadGraph(L"picture/ReTaylor.png");
	default:
		return -1;
	}
}

static cur_font_cr_t RecResultGetCurFontColor(rec_play_rank_t rank) {
	switch (rank) {
	case REC_PLAY_RANK_EX:
		return CUR_FONT_COLOR_RAINBOW;
	case REC_PLAY_RANK_S:
		return CUR_FONT_COLOR_BLUE;
	case REC_PLAY_RANK_A:
		return CUR_FONT_COLOR_YELLOW;
	case REC_PLAY_RANK_B:
		return CUR_FONT_COLOR_GREEN;
	case REC_PLAY_RANK_C:
		return CUR_FONT_COLOR_PURPLE;
	case REC_PLAY_RANK_D:
	default:
		return CUR_FONT_COLOR_RED;
	}
}

#if VER_1_6 == 1
static cur_font_cr_t RecResultGetFlortCurFontColor(rec_play_rank_t rank) {
	switch (rank) {
	case REC_PLAY_RANK_EX:
		return CUR_FONT_COLOR_BLUE;
	case REC_PLAY_RANK_S:
		return CUR_FONT_COLOR_YELLOW;
	case REC_PLAY_RANK_A:
		return CUR_FONT_COLOR_MONO; /* TODO: result floatfontNo(2) must be GRAY */
	case REC_PLAY_RANK_B:
		return CUR_FONT_COLOR_GREEN;
	case REC_PLAY_RANK_C:
		return CUR_FONT_COLOR_PURPLE;
	case REC_PLAY_RANK_D: /* TODO: Fランクの定義、ver1.6またはそれ以降でやる */
	default:
		return CUR_FONT_COLOR_RED;
	}
}
#endif

static void RecResultCalParameter(rec_result_pal_t *result_pal, const rec_map_detail_t *map_detail,
	const rec_play_userpal_t *userpal, const rec_play_nameset_t *nameset, int dif)
{
	const            char Clear    = JudgeClearRank(userpal);
	const            uint gapCount = maxs_2(1, userpal->gap.count);
	const rec_play_rank_t rank     = CalScoreRank(userpal->score.sum);

	result_pal->acc             = CAL_ACC(userpal->judgeCount, map_detail->notes);
	result_pal->gap             = CAL_GAP(userpal->gap.sum, gapCount);
	result_pal->width           = CAL_GAP_WIDTH(userpal->gap.sum, gapCount, userpal->gap.ssum);
	result_pal->score           = userpal->score.sum;
	result_pal->judge           = userpal->judgeCount;
	result_pal->Mcombo          = userpal->Mcombo;
	result_pal->noteCount       = map_detail->notes;
#if VER_1_6 == 1
	result_pal->floatRank       = GetFloatRank(userpal->score.sum, userpal->judgeCount.miss, map_detail->notes, rank) / 1000;
#endif
	result_pal->newRate         = GetFullRate();
	result_pal->subRate         = result_pal->newRate - result_pal->oldRate; /* oldRateはレート保存前に代入している */
	result_pal->mat.clearRate   = RecResultLoadClearRateGraph(Clear);
	result_pal->mat.difBer      = RecResultLoadDifBarGraph(dif, nameset);
	result_pal->mat.rank        = RecResultLoadClearRankGraph(rank);
	result_pal->mat.chara       = RecResultLoadCharaGraph();
	result_pal->mat.fontNo      = RecResultGetCurFontColor(rank);
#if VER_1_6 == 1
	result_pal->mat.floatfontNo = RecResultGetFlortCurFontColor(rank);
#endif
	result_pal->mat.BGM         = (userpal->status == REC_PLAY_STATUS_DROPED) ? LoadSoundMem(L"song/Regret.mp3") : LoadSoundMem(L"song/Balloon Art.mp3");

	strcopy_2(nameset->songN, result_pal->songN, 64);

	return;
}

#endif /* RecResultCalParameter */

#if 1 /* save */

/* 同じ曲名があったら上書きしてしまう */
static void SaveScore(const rec_play_userpal_t *userpal, const rec_map_detail_t *map_detail,
	const wchar_t *songN, int dif)
{
	const   char Clear  = JudgeClearRank(userpal);
	const  short rank   = CalScoreRank(userpal->score.sum);
	const    int score  = userpal->score.sum;
	const    int Dscore = userpal->Dscore.point;
	const double acc    = CAL_ACC(userpal->judgeCount, map_detail->notes);

	int	read[7] = { 0,0,0,0,0,0,0 };
	int	Readdis[7] = { 0,0,0,0,0,0,0 };
	int	ReadRank[7] = { 6,6,6,6,6,6,6 };
	int	ReadClear[7] = { 0,0,0,0,0,0,0 };
	double ReadAcc[7] = { 0,0,0,0,0,0,0 };
	wchar_t save[255] = L"score/";
	strcats(save, songN); // save = score/<曲名>
	strcats(save, L".dat"); // save = score/<曲名>.dat
	FILE* fp;

	_wfopen_s(&fp, save, L"rb");
	if (fp == NULL) { return; }
	fread(&read, sizeof(int), 6, fp);
	fread(&ReadAcc, sizeof(double), 6, fp);
	fread(&Readdis, sizeof(int), 6, fp);
	fread(&ReadRank, sizeof(int), 6, fp);
	fread(&ReadClear, sizeof(int), 6, fp);
	fclose(fp);

	if (read[dif] < score) { read[dif] = score; } //ハイスコア保存
	if (ReadAcc[dif] < acc) { ReadAcc[dif] = acc; } //ACC保存
	if (Readdis[dif] < Dscore) { Readdis[dif] = Dscore; } //最長走行距離保存
	if (ReadRank[dif] > rank || ReadRank[dif] < 0) { ReadRank[dif] = rank; } //ランク保存
	if (ReadClear[dif] < Clear) { ReadClear[dif] = Clear; } //クリアレート保存

	_wfopen_s(&fp, save, L"wb");
	if (fp == NULL) { return; }
	fwrite(&read, sizeof(int), 6, fp);
	fwrite(&ReadAcc, sizeof(double), 6, fp);
	fwrite(&Readdis, sizeof(int), 6, fp);
	fwrite(&ReadRank, sizeof(int), 6, fp);
	fwrite(&ReadClear, sizeof(int), 6, fp);
	fclose(fp);
	return;
}

static void SavePlayCount(const rec_play_userpal_t *userpal) {
	rec_user_data_t data;
	FILE *fp;

	_wfopen_s(&fp, L"save/data.dat", L"rb");
	if (fp == NULL) { return; }
	fread(&data, sizeof(rec_user_data_t), 1, fp);
	fclose(fp);

	data.playCount++;
	if (userpal->status == REC_PLAY_STATUS_DROPED) { data.dropCount++; }
	else { data.clearCount++; }
	if (userpal->judgeCount.miss == 0) {
		data.NMCount++;
		if (userpal->judgeCount.safe == 0) {
			data.FCCount++;
			if (userpal->judgeCount.good == 0) { data.PFcount++; }
		}
	}
	data.mileage += userpal->Dscore.point;

	_wfopen_s(&fp, L"save/data.dat", L"wb");
	if (fp == NULL) { return; }
	fwrite(&data, sizeof(rec_user_data_t), 1, fp);
	fclose(fp);

	return;
}

static void SaveCharPlayCount(void) {
	int chap[3] = { 0,0,0 };
	FILE *fp;

	_wfopen_s(&fp, L"save/chap.dat", L"rb");
	if (fp == NULL) { return; }
	fread(&chap, sizeof(int), 3, fp);
	fclose(fp);

	chap[optiondata.chara]++;

	_wfopen_s(&fp, L"save/chap.dat", L"wb");
	if (fp == NULL) { return; }
	fwrite(&chap, sizeof(int), 3, fp);
	fclose(fp);
	return;
}

static void RecUpdateRunnerRate(const rec_play_userpal_t *userpal,
	const rec_map_detail_t *map_detail, const wchar_t *songN)
{
	const double rate = (double)CalPlayRate(userpal->judgeCount, map_detail) / 100.0; /* TODO: judgeCountを直接引数にする */

	char num = -1;
	play_rate_t data[RATE_NUM];
	FILE *fp;

	_wfopen_s(&fp, RATE_FILE_NAME, L"rb");
	if (fp == NULL) { return; }
	fread(&data, sizeof(play_rate_t), RATE_NUM, fp);
	fclose(fp);

	// 同じ曲、または未収録を探す
	for (uint i = 0; i < RATE_NUM; i++) {
		if (strands(songN, data[i].name) ||
			(data[i].name[0] == L'\0' && data[i].num == 0))
		{
			num = i;
			break;
		}
	}

	// なかったら、一番低いレートを探す
	if (num == -1) {
		num = 0;
		for (uint i = 1; i < RATE_NUM; i++) {
			if (data[num].num > data[i].num) { num = i; }
		}
	}

	if (rate <= data[num].num) { return; }

	// レートを更新する
	data[num].num = rate;
	strcopy(songN, data[num].name, 1);
	_wfopen_s(&fp, RATE_FILE_NAME, L"wb");
	if (fp == NULL) { return; }
	fwrite(&data, sizeof(play_rate_t), RATE_NUM, fp);
	fclose(fp);
}

#endif /* save */

now_scene_t result(const rec_map_detail_t *map_detail, const rec_play_userpal_t *userpal,
	const rec_play_nameset_t *nameset, int dif, const wchar_t *songN)
{
	rec_result_pal_t result_pal;

	/* セーブ前作業 */
	result_pal.oldRate = GetFullRate();

	/* セーブ作業 */
	SavePlayCount(userpal);
	SaveScore(userpal, map_detail, songN, dif);
	SaveCharPlayCount();
	RecUpdateRunnerRate(userpal, map_detail, songN);

	/* リザルト表示 */
	RecResultCalParameter(&result_pal, map_detail, userpal, nameset, dif);
	return ViewResult(&result_pal);
}
