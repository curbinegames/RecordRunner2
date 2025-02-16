
#include <dxcur.h>
#include <strcur.h>
#include <fontcur.h>

#include <RecSave.h>
#include <option.h>
#include <system.h>
#include <playbox.h>
#include <RecWindowRescale.h>
#include <RecScoreFile.h>

/* TODO: セーブファイル関係をまとめたソースを作っても良いかも。名前はRecSave.cppとか? */
/* TODO: クリア状況はclearrank、スコア評価はscorerateに改名しよう */

#define VER_1_6 0

#define CAL_ACC(judge, notes)												\
	DIV_AVOID_ZERO((judge).just * 10000 + (judge).good * 9500 + (judge).safe * 5500, (notes) * 100.0, 0)

#define CAL_DIF_RATE(mdif, Lv)												\
	((Lv) == 0 ? 0 : mins_2((mdif), (Lv) * 100 + 90))

#define CAL_GAP(all_gap, count) DIV_AVOID_ZERO((all_gap), (count), 0)

#define CAL_GAP_WIDTH(all_gap, count, all_d_gap)							\
	(sanrute(DIV_AVOID_ZERO((all_d_gap) * (count) - (all_gap) * (all_gap), (count) * (count), 0)))

typedef struct rec_result_mat_s {
	DxPic_t clearRate = DXLIB_PIC_NULL;
	DxPic_t difBer    = DXLIB_PIC_NULL;
	DxPic_t rank      = DXLIB_PIC_NULL;
	DxPic_t chara     = DXLIB_PIC_NULL;
	DxSnd_t BGM       = DXLIB_SND_NULL;
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

		/* 背景 */
		RecRescaleDrawGraph(0, 0, resultimg, TRUE);
		
		/* 曲情報 */
		RecRescaleDrawGraph( 460, 20, val->mat.difBer, TRUE);
		RecRescaleDrawString(100, 13, val->songN, COLOR_WHITE);

		/* 判定周り */
		RecRescaleDrawCurFont(val->judge.just, 140,  52, 30, CUR_FONT_COLOR_BLUE);
		RecRescaleDrawCurFont(val->judge.good, 140,  93, 30, CUR_FONT_COLOR_YELLOW);
		RecRescaleDrawCurFont(val->judge.safe, 140, 134, 30, CUR_FONT_COLOR_GREEN);
		RecRescaleDrawCurFont(val->judge.miss, 140, 175, 30, CUR_FONT_COLOR_RED);
		RecRescaleDrawCurFont(val->Mcombo,     155, 215, 30, CUR_FONT_COLOR_BLUE);
		RecRescaleDrawCurFont(val->noteCount,  265, 215, 30, CUR_FONT_COLOR_PURPLE);

		/* レート周り */
		RecRescaleDrawFormatString(10, 320, COLOR_WHITE, L"%d.%02d", val->newRate / 100, val->newRate % 100);
		if (0 < val->subRate) { RecRescaleDrawFormatString(10, 340, COLOR_WHITE, L"+%d.%02d", val->subRate / 100, val->subRate % 100); }
		else { RecRescaleDrawString(10, 340, L"not rise", COLOR_WHITE); }

		/* スコア周り */
		RecRescaleDrawCurFont(val->score, 310,  75, 55, val->mat.fontNo);
		RecRescaleDrawCurFont(val->acc,   430, 150, 30, val->mat.fontNo, 2);
		RecRescaleDrawCurFont(val->gap,   510, 205, 20, CUR_FONT_COLOR_MONO);
		RecRescaleDrawCurFont(val->width, 500, 230, 20, CUR_FONT_COLOR_MONO);

		/* ランク周り */
		RecRescaleDrawGraph(140, 260, val->mat.rank,      TRUE);
		RecRescaleDrawGraph(5,   420, val->mat.clearRate, TRUE);
		RecRescaleDrawGraph(336, 252, val->mat.chara,     TRUE);
#if VER_1_6 == 1
		RecRescaleDrawCurFont(val->floatRank, 280, 390, 30, val->mat.floatfontNo, 3, 0);
#endif

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

static rec_score_rate_t CalScoreRank(int score) {
	if (score >= 98000) { return REC_SCORE_RATE_EX; }
	if (score >= 95000) { return REC_SCORE_RATE_S; }
	if (score >= 90000) { return REC_SCORE_RATE_A; }
	if (score >= 85000) { return REC_SCORE_RATE_B; }
	if (score >= 80000) { return REC_SCORE_RATE_C; }
	return REC_SCORE_RATE_D;
}

static int CalPlayRate(const judge_box *judge, const rec_map_detail_t *map_detail) {
	const double DifRate = CAL_DIF_RATE(map_detail->mpal.mdif, map_detail->Lv) / 100.0;

	double rate = 0;
	// level0なら0固定
	if (DifRate == 0) { rate = 0; }
	// "譜面定数" - "miss数" x "譜面定数" x 0.03 (下限=0)
	else if (judge->miss > 0) {
		rate = DifRate - judge->miss * DifRate * 0.03;
		rate = maxs_2(rate, 0);
	}
	// NO MISS,"譜面定数" + 1 - "safe数" x 0.05 (下限="譜面定数")
	else if (judge->miss == 0 && judge->safe > 0) {
		rate = DifRate + 1 - judge->safe * 0.05;
		rate = maxs_2(rate, DifRate);
	}
	// FULL COMBO,"譜面定数" + 2 - "good数" x 0.01 (下限="譜面定数" + 1)
	else if (judge->miss == 0 && judge->safe == 0 && judge->good > 0) {
		rate = DifRate + 2 - judge->good * 0.01;
		rate = maxs_2(rate, DifRate + 1);
	}
	// PERFECT, "譜面定数" + 2
	else if (judge->miss == 0 && judge->safe == 0 && judge->good == 0) {
		rate = DifRate + 2;
	}
	return (int)(rate * 100);
}

rec_clear_rank_t JudgeClearRank(const rec_play_userpal_t *userpal) {
	const judge_box *judge = &userpal->judgeCount;
	if (userpal->status == REC_PLAY_STATUS_DROPED) { return REC_CLEAR_RANK_DROPED; }
	if (0 < userpal->judgeCount.miss) { return REC_CLEAR_RANK_CLEARED; }
	if (0 < userpal->judgeCount.safe) { return REC_CLEAR_RANK_NOMISS; }
	if (0 < userpal->judgeCount.good) { return REC_CLEAR_RANK_FULLCOMBO; }
	return REC_CLEAR_RANK_PERFECT;
}

#if VER_1_6 == 1
double GetFloatRank(int score, int miss, int notes, char rank) {
	switch (rank) {
	case REC_SCORE_RATE_EX:
		return score / 2 - 49000;
	case REC_SCORE_RATE_S:
		return (score - 95000) / 3;
	case REC_SCORE_RATE_A:
		return score / 5 - 18000;
	case REC_SCORE_RATE_B:
		return score / 10 - 8000;
	case REC_SCORE_RATE_C:
		return score / 20 - 3000;
	case REC_SCORE_RATE_D:
		if (50000 <= score) {
			return score * 3 / 100 + 800;
		}
		else if (40000 <= score) {
			return score / 50 - 300;
		}
		else {
			return score / 80;
		}
	case REC_SCORE_RATE_F:
		return pals(0, 1000, 100, 0, 100 * miss / notes);
	}
	return 0;
}
#endif

static DxPic_t RecResultLoadClearRateGraph(rec_clear_rank_t Clear) {
	switch (Clear) {
	case REC_CLEAR_RANK_DROPED:
		return LoadGraph(L"picture/DROPED.png");
	case REC_CLEAR_RANK_CLEARED:
		return LoadGraph(L"picture/CLEARED.png");
	case REC_CLEAR_RANK_NOMISS:
		return LoadGraph(L"picture/NOMISS.png");
	case REC_CLEAR_RANK_FULLCOMBO:
		return LoadGraph(L"picture/FULLCOMBO.png");
	case REC_CLEAR_RANK_PERFECT:
		return LoadGraph(L"picture/PERFECT.png");
	default:
		return DXLIB_PIC_NULL;
	}
}

static DxPic_t RecResultLoadDifBarGraph(rec_dif_t dif, const TCHAR *difFN) {
	switch (dif) {
	case REC_DIF_AUTO:
		return LoadGraph(L"picture/difauto.png");
	case REC_DIF_EASY:
		return LoadGraph(L"picture/difeasy.png");
	case REC_DIF_NORMAL:
		return LoadGraph(L"picture/difnormal.png");
	case REC_DIF_HARD:
		return LoadGraph(L"picture/difhard.png");
	case REC_DIF_ANOTHER:
	case REC_DIF_SECRET:
		return LoadGraph(difFN);
	default:
		return DXLIB_PIC_HAND_DEFAULT;
	}
}

static DxPic_t RecResultLoadClearRankGraph(rec_score_rate_t rank) {
	switch (rank) {
	case REC_SCORE_RATE_EX:
		return LoadGraph(L"picture/rankEX.png");
	case REC_SCORE_RATE_S:
		return LoadGraph(L"picture/rankS.png");
	case REC_SCORE_RATE_A:
		return LoadGraph(L"picture/rankA.png");
	case REC_SCORE_RATE_B:
		return LoadGraph(L"picture/rankB.png");
	case REC_SCORE_RATE_C:
		return LoadGraph(L"picture/rankC.png");
	case REC_SCORE_RATE_D:
		return LoadGraph(L"picture/rankD.png");
	default:
		return DXLIB_PIC_NULL;
	}
}

static DxPic_t RecResultLoadCharaGraph(void) {
	switch (optiondata.chara) {
	case REC_CHARA_PICKER:
		return LoadGraph(L"picture/RePicker.png");
	case REC_CHARA_MAPGATOR:
		return LoadGraph(L"picture/ReGator.png");
	case REC_CHARA_TAYLOR:
		return LoadGraph(L"picture/ReTaylor.png");
	default:
		return DXLIB_PIC_NULL;
	}
}

static cur_font_cr_t RecResultGetCurFontColor(rec_score_rate_t rank) {
	switch (rank) {
	case REC_SCORE_RATE_EX:
		return CUR_FONT_COLOR_RAINBOW;
	case REC_SCORE_RATE_S:
		return CUR_FONT_COLOR_BLUE;
	case REC_SCORE_RATE_A:
		return CUR_FONT_COLOR_YELLOW;
	case REC_SCORE_RATE_B:
		return CUR_FONT_COLOR_GREEN;
	case REC_SCORE_RATE_C:
		return CUR_FONT_COLOR_PURPLE;
	case REC_SCORE_RATE_D:
	default:
		return CUR_FONT_COLOR_RED;
	}
}

#if VER_1_6 == 1
static cur_font_cr_t RecResultGetFlortCurFontColor(rec_score_rate_t rank) {
	switch (rank) {
	case REC_SCORE_RATE_EX:
		return CUR_FONT_COLOR_BLUE;
	case REC_SCORE_RATE_S:
		return CUR_FONT_COLOR_YELLOW;
	case REC_SCORE_RATE_A:
		return CUR_FONT_COLOR_MONO; /* TODO: result floatfontNo(2) must be GRAY */
	case REC_SCORE_RATE_B:
		return CUR_FONT_COLOR_GREEN;
	case REC_SCORE_RATE_C:
		return CUR_FONT_COLOR_PURPLE;
	case REC_SCORE_RATE_D: /* TODO: Fランクの定義、ver1.6またはそれ以降でやる */
	default:
		return CUR_FONT_COLOR_RED;
	}
}
#endif

/**
 * リザルト表示に必要な情報を取得します。
 */
static void RecResultCalParameter(rec_result_pal_t *result_pal, const rec_play_userpal_t *userpal,
	const rec_play_nameset_t *nameset, rec_dif_t dif, short noteCount)
{
	const rec_clear_rank_t Clear = JudgeClearRank(userpal);
	const rec_score_rate_t rank  = CalScoreRank(userpal->score.sum);

	result_pal->acc             = CAL_ACC(userpal->judgeCount, noteCount);
	result_pal->gap             = CAL_GAP(userpal->gap.sum, userpal->gap.count);
	result_pal->width           = CAL_GAP_WIDTH(userpal->gap.sum, userpal->gap.count, userpal->gap.ssum);
	result_pal->score           = userpal->score.sum;
	result_pal->judge           = userpal->judgeCount;
	result_pal->Mcombo          = userpal->Mcombo;
	result_pal->noteCount       = noteCount;
#if VER_1_6 == 1
	result_pal->floatRank       = GetFloatRank(userpal->score.sum, userpal->judgeCount.miss, noteCount, rank) / 1000;
#endif
	result_pal->newRate         = RecSaveGetFullRunnerRate() * 100;
	result_pal->subRate         = result_pal->newRate - result_pal->oldRate; /* oldRateはレート保存前に代入している */
	result_pal->mat.clearRate   = RecResultLoadClearRateGraph(Clear);
	result_pal->mat.difBer      = RecResultLoadDifBarGraph(dif, nameset->DifFN);
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

static void SaveScore(const rec_play_userpal_t *userpal, const TCHAR *songN, rec_dif_t dif,
	short noteCount)
{
	rec_save_score_t buf;

	buf.clearRank = JudgeClearRank(userpal);
	buf.scoreRate = CalScoreRank(userpal->score.sum);
	buf.score     = userpal->score.sum;
	buf.dist      = userpal->Dscore.point;
	buf.acc       = CAL_ACC(userpal->judgeCount, noteCount);

	RecSaveUpdateScoreOneDif(&buf, songN, dif);
	return;
}

now_scene_t result(const rec_map_detail_t *map_detail, const rec_play_userpal_t *userpal,
	const rec_play_nameset_t *nameset, rec_dif_t dif, const TCHAR *songN)
{
	rec_result_pal_t result_pal;

	/* セーブ前作業 */
	result_pal.oldRate = RecSaveGetFullRunnerRate() * 100;

	/* セーブ作業 */
	RecSaveUpdateUserPlay(userpal);
	SaveScore(userpal, songN, dif, map_detail->notes);
	RecSaveUpdateCharaPlay((rec_nowchara_t)optiondata.chara);
	RecSaveUpdateRunnerRate(songN, (double)CalPlayRate(&userpal->judgeCount, map_detail) / 100.0);

	/* リザルト表示 */
	RecResultCalParameter(&result_pal, userpal, nameset, dif, map_detail->notes);
	return ViewResult(&result_pal);
}
