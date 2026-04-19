
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <dxcur.h>
#include <strcur.h>

/* rec system include */
#include <RecSystem.h>
#include <option.h>
#include <playbox.h>
#include <RecSave.h>
#include <RecScoreFile.h>
#include <RecWindowRescale.h>

/* TODO: クリア状況はclearrank、スコア評価はscorerateに改名しよう */

#define CAL_ACC(judge, notes)												\
	DIV_AVOID_ZERO((judge).just * 10000 + (judge).good * 9500 + (judge).safe * 5500, (notes) * 100.0, 0)

#define CAL_DIF_RATE(mdif, Lv)												\
	((Lv) == 0 ? 0 : mins_2((mdif), (Lv) * 100 + 90))

#define CAL_GAP(all_gap, count) DIV_AVOID_ZERO((all_gap), (count), 0)

#define CAL_GAP_WIDTH(all_gap, count, all_d_gap)							\
	(sanrute(DIV_AVOID_ZERO((all_d_gap) * (count) - (all_gap) * (all_gap), (count) * (count), 0)))

typedef enum cur_font_id_e {
	CUR_FONT_ID_0 = 0,
	CUR_FONT_ID_1,
	CUR_FONT_ID_2,
	CUR_FONT_ID_3,
	CUR_FONT_ID_4,
	CUR_FONT_ID_5,
	CUR_FONT_ID_6,
	CUR_FONT_ID_7,
	CUR_FONT_ID_8,
	CUR_FONT_ID_9,
	CUR_FONT_ID_MINUS, /* 10 */
	CUR_FONT_ID_DOT,   /* 11 */
} cur_font_id_t;

typedef enum cur_font_cr_e {
	CUR_FONT_COLOR_MONO = 0,
	CUR_FONT_COLOR_RED,
	CUR_FONT_COLOR_YELLOW,
	CUR_FONT_COLOR_GREEN,
	CUR_FONT_COLOR_BLUE,
	CUR_FONT_COLOR_PURPLE,
	CUR_FONT_COLOR_RAINBOW,
} cur_font_cr_t;

typedef struct rec_result_mat_s {
	dxcur_pic_c clearRate;
	dxcur_pic_c difBer;
	dxcur_pic_c rank;
	dxcur_pic_c chara;
	cur_font_cr_t fontNo = CUR_FONT_COLOR_MONO;
	cur_font_cr_t floatfontNo = CUR_FONT_COLOR_MONO;
} rec_result_mat_t;

typedef struct rec_result_pal_s {
	tstring songN;
	rec_play_judge_t judge;
	int Mcombo = 0;
	int noteCount = 0;
	int score = 0;
	double acc = 0;
	double floatRank = 0;
	intx100_t newRate = 0;
	intx100_t oldRate = 0;
	intx100_t subRate = 0;
	int gap = 0;
	int width = 0;
	rec_result_mat_t mat;
} rec_result_pal_t;

class rec_result_numfont_c {
private:
	dxcur_divpic_c pic[7] = {
		dxcur_divpic_c(_T("fontcur/NumberMono.png"   ), 12, 5, 3),
		dxcur_divpic_c(_T("fontcur/NumberRed.png"    ), 12, 5, 3),
		dxcur_divpic_c(_T("fontcur/NumberYellow.png" ), 12, 5, 3),
		dxcur_divpic_c(_T("fontcur/NumberGreen.png"  ), 12, 5, 3),
		dxcur_divpic_c(_T("fontcur/NumberBlue.png"   ), 12, 5, 3),
		dxcur_divpic_c(_T("fontcur/NumberPurple.png" ), 12, 5, 3),
		dxcur_divpic_c(_T("fontcur/NumberRainbow.png"), 12, 5, 3)
	};

public:
	/**
	* 数字を書きます
	* @param[in] dn draw number
	* @param[in] x draw X pos
	* @param[in] y draw Y pos
	* @param[in] sizeY font Y size
	* @param[in] Cr font color
	* @param[in] a under point count (if dn = 157.423 and a = 2, draw "157.42")
	* @param[in] zero draw "0." (if dn = 0.524 and zero = 0, draw ".524")
	* @return なし
	*/
	void Draw(double dn, short x, short y, int sizeY, cur_font_cr_t Cr, short a = 0, int zero = 1) const {
		/* int */
		int k = 0;
		int n;
		int m;
		int ZeroP = 0; /* minus flag */
		int sizeX = sizeY * 58 / 64;
		int width = sizeY * 46 / 64;
		int pointX = sizeY * 17 / 64; /* dot X size */

		/* cal */
		if (dn < 0) {
			ZeroP = 1;
			dn *= -1;
		}
		for (int i = a; i > 0; i--) { dn *= 10; }
		n = (int)dn;
		if (n != 0) { for (int i = 10; i <= n; i *= 10) { k++; } }

		/* draw */
		for (int i = k; i >= 0; i--) {
			m = n % 10;
			if (a != 1 || zero == 1 || m != 0) {
				DrawExtendGraph(x + i * width + (a > 0) * pointX + ZeroP * width, y,
					x + sizeX + i * width + (a > 0) * pointX + ZeroP * width,
					y + sizeY, this->pic[Cr].handle(m), TRUE);
			}
			n /= 10;
			if (a == 1) {
				DrawExtendGraph(x + i * width + ZeroP * width, y,
					x + sizeX + i * width + ZeroP * width, y + sizeY, this->pic[Cr].handle(CUR_FONT_ID_DOT),
					TRUE);
			}
			a--;
		}
		if (ZeroP) { DrawExtendGraph(x, y, x + sizeX, y + sizeY, this->pic[Cr].handle(CUR_FONT_ID_MINUS), TRUE); }
		return;
	}

	void RescaleDraw(double dn, short x, short y, int sizeY, cur_font_cr_t Cr, short a = 0, int zero = 1) const {
		int drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
		int drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
		this->Draw(dn, drawX, drawY, sizeY * RESCALE_SIZE_Y, Cr, a, zero);
		return;
	}
};

static now_scene_t ViewResult(const rec_result_pal_t *val) {
	/* typedef */
	dxcur_pic_c resultimg = dxcur_pic_c(_T("picture/result.png"));
	/* class */
	rec_result_numfont_c num_font;
	rec_cutin_c cutin;

	rec_bgm_system_g.Play(true, true, true);
	WaitTimer(WAIT_TIME_AFTER_MUSICPLAY);
	cutin.SetIo(CUT_FRAG_OUT);
	while (1) {
		ClearDrawScreen(); /* 描画エリアここから */

		/* 背景 */
		RecRescaleDrawGraph(0, 0, resultimg.handle(), TRUE);
		
		/* 曲情報 */
		RecRescaleDrawGraph( 460, 20, val->mat.difBer.handle(), TRUE);
		RecRescaleDrawString(100, 13, val->songN.c_str(), COLOR_WHITE);

		/* 判定周り */
		num_font.RescaleDraw(val->judge.just, 140,  52, 30, CUR_FONT_COLOR_BLUE);
		num_font.RescaleDraw(val->judge.good, 140,  93, 30, CUR_FONT_COLOR_YELLOW);
		num_font.RescaleDraw(val->judge.safe, 140, 134, 30, CUR_FONT_COLOR_GREEN);
		num_font.RescaleDraw(val->judge.miss, 140, 175, 30, CUR_FONT_COLOR_RED);
		num_font.RescaleDraw(val->Mcombo,     155, 215, 30, CUR_FONT_COLOR_BLUE);
		num_font.RescaleDraw(val->noteCount,  265, 215, 30, CUR_FONT_COLOR_PURPLE);

		/* レート周り */
		RecRescaleDrawFormatString(10, 320, COLOR_WHITE, L"%d.%02d", val->newRate / 100, val->newRate % 100);
		if (0 < val->subRate) { RecRescaleDrawFormatString(10, 340, COLOR_WHITE, L"+%d.%02d", val->subRate / 100, val->subRate % 100); }
		else { RecRescaleDrawString(10, 340, L"not rise", COLOR_WHITE); }

		/* スコア周り */
		num_font.RescaleDraw(val->score, 310,  75, 55, val->mat.fontNo);
		num_font.RescaleDraw(val->acc,   430, 150, 30, val->mat.fontNo, 2);
		num_font.RescaleDraw(val->gap,   510, 205, 20, CUR_FONT_COLOR_MONO);
		num_font.RescaleDraw(val->width, 500, 230, 20, CUR_FONT_COLOR_MONO);

		/* ランク周り */
		RecRescaleDrawGraph(140, 260, val->mat.rank.handle(),      TRUE);
		RecRescaleDrawGraph(5,   420, val->mat.clearRate.handle(), TRUE);
		RecRescaleDrawGraph(336, 252, val->mat.chara.handle(),     TRUE);
		num_font.RescaleDraw(val->floatRank, 280, 390, 30, val->mat.floatfontNo, 3, 0);

		cutin.DrawCut();

		ScreenFlip(); /* 描画エリアここまで */

		//エンターが押された
		if (CheckHitKey(KEY_INPUT_RETURN) == 1 && cutin.IsClosing() == 0) {
			cutin.SetCutTipFg(CUTIN_TIPS_ON);
			cutin.SetTipNo();
			cutin.SetIo(CUT_FRAG_IN);
		}
		if (cutin.IsEndAnim()) {
			rec_bgm_system_g.Stop();
			break;
		}
		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }
	}

	return SCENE_SERECT;
}

#if 1 /* RecResultCalParameter */

static rec_score_rate_t CalScoreRank(int score) {
	if (score >= 98000) { return REC_SCORE_RATE_EX; }
	if (score >= 95000) { return REC_SCORE_RATE_S; }
	if (score >= 90000) { return REC_SCORE_RATE_A; }
	if (score >= 85000) { return REC_SCORE_RATE_B; }
	if (score >= 80000) { return REC_SCORE_RATE_C; }
	if (score > 0) { return REC_SCORE_RATE_D; }
	return REC_SCORE_RATE_F;
}

/**
 * レート計算の考え
 * FULL PERFECTで [譜面定数] + 3くらい
 * PERFECTで      [譜面定数] + 2くらい
 * FULL COMBOで   [譜面定数] + 1くらい
 * NO MISSで      [譜面定数] + 0くらい
 * それ以外       [譜面定数]未満
 * 
 * レート = ([譜面定数] + [good以下ボーナス] + [safe以下ボーナス]) * [miss数ボーナス] + [PERFECTボーナス]
 * PERFECTボーナス  = [PJUSTの加点分(0to100)] / 100
 * good以下ボーナス = min(1 - [good以下率(0to1)] * 10  0)
 * safe以下ボーナス = min(1 - [safe以下率(0to1)] * 10  0)
 * miss数ボーナス   = min((50 - [miss数]) / 50, 0)
 */
static int CalPlayRate(const rec_play_judge_t *judge, const rec_map_detail_t *map_detail) {
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
	const rec_play_judge_t *judge = &userpal->judgeCount;
	/* TODO: LOSTED, LIGHTCLEAR, MISSLESS, FULLPERFECTを作る  */
	/* if (userpal->status == REC_PLAY_STATUS_LOSTED) { return REC_CLEAR_RANK_LOSTED; } */
	if (userpal->status == REC_PLAY_STATUS_DROPED) { return REC_CLEAR_RANK_DROPED; }
	/* if (userpal->Exlife < 0) { return REC_CLEAR_RANK_CLEARED; } */
	/* if (5 < userpal->judgeCount.miss) { return REC_CLEAR_RANK_LIGHTCLEAR; } */
	/* if (0 < userpal->judgeCount.miss) { return REC_CLEAR_RANK_MISSLESS; } */
	if (0 < userpal->judgeCount.miss) { return REC_CLEAR_RANK_CLEARED; }
	if (0 < userpal->judgeCount.safe) { return REC_CLEAR_RANK_NOMISS; }
	if (0 < userpal->judgeCount.good) { return REC_CLEAR_RANK_FULLCOMBO; }
	/* if (userpal->score.sum < 100100) { return REC_CLEAR_RANK_PERFECT; } */
	/* return REC_CLEAR_RANK_FULLPERFECT; */
	return REC_CLEAR_RANK_PERFECT;
}

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

static const TCHAR *RecResultLoadClearRateGraph(rec_clear_rank_t Clear) {
	switch (Clear) {
	case REC_CLEAR_RANK_DROPED:
		return (_T("picture/DROPED.png"));
	case REC_CLEAR_RANK_CLEARED:
		return (_T("picture/CLEARED.png"));
	case REC_CLEAR_RANK_NOMISS:
		return (_T("picture/NOMISS.png"));
	case REC_CLEAR_RANK_FULLCOMBO:
		return (_T("picture/FULLCOMBO.png"));
	case REC_CLEAR_RANK_PERFECT:
		return (_T("picture/PERFECT.png"));
	default:
		return (_T(""));
	}
}

static const TCHAR *RecResultLoadDifBarGraph(rec_dif_t dif, const TCHAR *difFN) {
	switch (dif) {
	case REC_DIF_AUTO:
		return (L"picture/difauto.png");
	case REC_DIF_EASY:
		return (L"picture/difeasy.png");
	case REC_DIF_NORMAL:
		return (L"picture/difnormal.png");
	case REC_DIF_HARD:
		return (L"picture/difhard.png");
	case REC_DIF_ANOTHER:
	case REC_DIF_SECRET:
		return (difFN);
	default:
		return (_T(""));
	}
}

static const TCHAR *RecResultLoadClearRankGraph(rec_score_rate_t rank) {
	switch (rank) {
	case REC_SCORE_RATE_EX:
		return (L"picture/rankEX.png");
	case REC_SCORE_RATE_S:
		return (L"picture/rankS.png");
	case REC_SCORE_RATE_A:
		return (L"picture/rankA.png");
	case REC_SCORE_RATE_B:
		return (L"picture/rankB.png");
	case REC_SCORE_RATE_C:
		return (L"picture/rankC.png");
	case REC_SCORE_RATE_D:
		return (L"picture/rankD.png");
	case REC_SCORE_RATE_F:
		return (L"picture/rankF.png"); /* TODO: 画像作る */
	default:
		return (_T(""));
	}
}

static const TCHAR *RecResultLoadCharaGraph(void) {
	switch (optiondata.chara) {
	case REC_CHARA_PICKER:
		return (L"picture/RePicker.png");
	case REC_CHARA_MAPGATOR:
		return (L"picture/ReGator.png");
	case REC_CHARA_TAYLOR:
		return (L"picture/ReTaylor.png");
	default:
		return (_T(""));
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
		return CUR_FONT_COLOR_RED;
	case REC_SCORE_RATE_F:
	default:
		return CUR_FONT_COLOR_MONO;
	}
}

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
	case REC_SCORE_RATE_D:
		return CUR_FONT_COLOR_RED;
	case REC_SCORE_RATE_F:
	default:
		return CUR_FONT_COLOR_MONO;
	}
}

static void RecResultSetBgm(rec_play_status_t status) {
	if (status == REC_PLAY_STATUS_DROPED) {
		rec_bgm_system_g.SetMem(_T("song/Regret.mp3"));
	}
	else {
		rec_bgm_system_g.SetMem(_T("song/Balloon Art.mp3"));
	}
	return;
}

/**
 * リザルト表示に必要な情報を取得します。
 */
static void RecResultCalParameter(rec_result_pal_t *result_pal, const rec_play_userpal_t *userpal,
	const rec_play_nameset_t *nameset, rec_dif_t dif, short noteCount)
{
	const rec_clear_rank_t Clear = JudgeClearRank(userpal);
	const rec_score_rate_t rank  = CalScoreRank(userpal->score.sum);

	result_pal->acc             = CAL_ACC(userpal->judgeCount, noteCount);
	result_pal->gap             = userpal->gap.GetAverage();
	result_pal->width           = userpal->gap.GetDeviation();
	result_pal->score           = userpal->score.sum;
	result_pal->judge           = userpal->judgeCount;
	result_pal->Mcombo          = userpal->Mcombo;
	result_pal->noteCount       = noteCount;
	result_pal->floatRank       = GetFloatRank(userpal->score.sum, userpal->judgeCount.miss, noteCount, rank) / 1000;
	result_pal->newRate         = RecSaveGetFullRunnerRate() * 100;
	result_pal->subRate         = result_pal->newRate - result_pal->oldRate; /* oldRateはレート保存前に代入している */

	result_pal->mat.clearRate.reload(RecResultLoadClearRateGraph(Clear));
	result_pal->mat.difBer.reload(RecResultLoadDifBarGraph(dif, nameset->DifFN.c_str()));
	result_pal->mat.rank.reload(RecResultLoadClearRankGraph(rank));
	result_pal->mat.chara.reload(RecResultLoadCharaGraph());
	result_pal->mat.fontNo      = RecResultGetCurFontColor(rank);
	result_pal->mat.floatfontNo = RecResultGetFlortCurFontColor(rank);

	RecResultSetBgm(userpal->status);

	result_pal->songN = nameset->songN;
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
