
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

#include <cmath>
#include <sancur.h>
#include <strcur.h>
#include <dxcur.h>
#include <RecSystem.h>
#include <result.h>

/* TODO: クリア状況はcleartype、スコア評価はscorerateに改名しよう */

#define CAL_ACC(judge, notes)												\
	DIV_AVOID_ZERO((judge).just * 10000 + (judge).good * 9500 + (judge).safe * 5500, (notes) * 100.0, 0)

#define CAL_DIF_RATE(mdif, Lv)												\
	((Lv) == 0 ? 0 : mins_2((mdif), (Lv) * 100 + 90))

#define CAL_GAP(all_gap, count) DIV_AVOID_ZERO((all_gap), (count), 0)

#define CAL_GAP_WIDTH(all_gap, count, all_d_gap)							\
	(std::sqrt(DIV_AVOID_ZERO((all_d_gap) * (count) - (all_gap) * (all_gap), (count) * (count), 0)))

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

#if 1 /* class */

class rec_result_numfont_c {
private:
	dxcur_number_pic_c pic[7] = {
		dxcur_number_pic_c(_T("fontcur/NumberMono.png"   )),
		dxcur_number_pic_c(_T("fontcur/NumberRed.png"    )),
		dxcur_number_pic_c(_T("fontcur/NumberYellow.png" )),
		dxcur_number_pic_c(_T("fontcur/NumberGreen.png"  )),
		dxcur_number_pic_c(_T("fontcur/NumberBlue.png"   )),
		dxcur_number_pic_c(_T("fontcur/NumberPurple.png" )),
		dxcur_number_pic_c(_T("fontcur/NumberRainbow.png"))
	};

public:
	rec_result_numfont_c(void) {
		for (int i = 0; i < 7; i++) {
			this->pic[i].SetPicGapAll(46);
			this->pic[i].SetPointSizeX(17);
		}
	}

	void DrawNum(int x, int y, double size, int num, cur_font_cr_t cr) const {
		this->pic[cr].DrawNum(x, y, size, num);
	}

	void DrawFloat(int x, int y, double size, double num, cur_font_cr_t cr, uint under, bool zero = TRUE) const {
		this->pic[cr].DrawFloat(x, y, size, num, under);
	}

	void RescaleDrawNum(int x, int y, double size, int num, cur_font_cr_t cr) const {
		int drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
		int drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
		this->DrawNum(drawX, drawY, size * WINDOW_SIZE_Y / (double)OLD_WINDOW_SIZE_Y, num, cr);
	}

	void RescaleDrawFloat(int x, int y, double size, double num, cur_font_cr_t cr, uint under, bool zero = TRUE) const {
		int drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
		int drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
		this->DrawFloat(drawX, drawY, size * WINDOW_SIZE_Y / (double)OLD_WINDOW_SIZE_Y, num, cr, under, zero);
	}

	void RescaleDrawUnderFloat(int x, int y, double size, double num, cur_font_cr_t cr, uint under) const {
		int drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
		int drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
		this->pic[cr].DrawUnderFloat(drawX, drawY, size * WINDOW_SIZE_Y / (double)OLD_WINDOW_SIZE_Y, num, under);
	}
};

class rec_result_charfont_c {
private:
	dxcur_alphabet_pic_c pic[1] = {
		dxcur_alphabet_pic_c(_T("picture/result/font.png"))
	};

public:
	rec_result_charfont_c(void) {
		for (int i = 0; i < 1; i++) {
			this->pic[i].SetPicSizeAll(24);
		}
	}

	void DrawString(int x, int y, double size, const tstring &str, int cr) const {
		this->pic[cr].DrawString(x, y, size, str);
	}

	void RescaleDrawString(int x, int y, double size, const tstring &str, int cr) const {
		int drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
		int drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
		this->DrawString(drawX, drawY, size * WINDOW_SIZE_Y / (double)OLD_WINDOW_SIZE_Y, str, cr);
	}
};

class rec_result_cleartype_pic_c {
private:
	dxcur_pic_c pic;

public:
	rec_result_cleartype_pic_c(void) {}

	rec_result_cleartype_pic_c(rec_clear_rank_t Clear) {
		this->set_pic(Clear);
	}

	void set_pic(rec_clear_rank_t Clear) {
		switch (Clear) {
		case REC_CLEAR_RANK_DROPED:
			this->pic.reload(_T("picture/DROPED.png"));
			break;
		case REC_CLEAR_RANK_CLEARED:
			this->pic.reload(_T("picture/CLEARED.png"));
			break;
		case REC_CLEAR_RANK_NOMISS:
			this->pic.reload(_T("picture/NOMISS.png"));
			break;
		case REC_CLEAR_RANK_FULLCOMBO:
			this->pic.reload(_T("picture/FULLCOMBO.png"));
			break;
		case REC_CLEAR_RANK_PERFECT:
			this->pic.reload(_T("picture/PERFECT.png"));
			break;
		default:
			this->pic.reload(_T(""));
			break;
		}
	}

	DxPic_t handle(void) const {
		return this->pic.handle();
	}
};

class rec_result_difber_pic_c {
private:
	dxcur_pic_c pic;

public:
	rec_result_difber_pic_c(void) {}

	rec_result_difber_pic_c(rec_dif_t dif, const tstring &difFN) {
		this->set_pic(dif, difFN);
	}

	void set_pic(rec_dif_t dif, const tstring &difFN) {
		switch (dif) {
		case REC_DIF_AUTO:
			this->pic.reload(L"picture/difauto.png");
			break;
		case REC_DIF_EASY:
			this->pic.reload(L"picture/difeasy.png");
			break;
		case REC_DIF_NORMAL:
			this->pic.reload(L"picture/difnormal.png");
			break;
		case REC_DIF_HARD:
			this->pic.reload(L"picture/difhard.png");
			break;
		case REC_DIF_ANOTHER:
		case REC_DIF_SECRET:
			this->pic.reload(difFN);
			break;
		default:
			this->pic.reload(_T(""));
			break;
		}
	}

	DxPic_t handle(void) const {
		return this->pic.handle();
	}
};

class rec_result_scorerank_pic_c {
private:
	dxcur_pic_c pic;

public:
	rec_result_scorerank_pic_c(void) {}

	rec_result_scorerank_pic_c(rec_score_rate_t rank) {
		this->set_pic(rank);
	}

	void set_pic(rec_score_rate_t rank) {
		switch (rank) {
		case REC_SCORE_RATE_EX:
			this->pic.reload(L"picture/rankEX.png");
			break;
		case REC_SCORE_RATE_S:
			this->pic.reload(L"picture/rankS.png");
			break;
		case REC_SCORE_RATE_A:
			this->pic.reload(L"picture/rankA.png");
			break;
		case REC_SCORE_RATE_B:
			this->pic.reload(L"picture/rankB.png");
			break;
		case REC_SCORE_RATE_C:
			this->pic.reload(L"picture/rankC.png");
			break;
		case REC_SCORE_RATE_D:
			this->pic.reload(L"picture/rankD.png");
			break;
		case REC_SCORE_RATE_F:
			this->pic.reload(L"picture/rankF.png"); /* TODO: 画像作る */
			break;
		default:
			this->pic.reload(_T(""));
			break;
		}
	}

	DxPic_t handle(void) const {
		return this->pic.handle();
	}
};

class rec_result_chara_pic_c {
private:
	dxcur_pic_c pic;

public:
	rec_result_chara_pic_c(void) {
	switch (optiondata.chara) {
		case REC_CHARA_PICKER:
			this->pic.reload(L"picture/result/Picker.png");
			break;
		case REC_CHARA_GATOR:
			this->pic.reload(L"picture/result/Gator.png");
			break;
		case REC_CHARA_TAYLOR:
			this->pic.reload(L"picture/result/Taylor.png");
			break;
		default:
			this->pic.reload(_T(""));
			break;
		}
	}

	DxPic_t handle(void) const {
		return this->pic.handle();
	}
};

#endif /* class */

typedef struct rec_result_mat_s {
	rec_result_cleartype_pic_c clearRate;
	rec_result_difber_pic_c difBer;
	rec_result_scorerank_pic_c rank;
	rec_result_chara_pic_c chara;
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

static now_scene_t ViewResult(const rec_result_pal_t &val) {
	dxcur_pic_c resultimg = dxcur_pic_c(_T("picture/result/back.png"));
	rec_result_charfont_c char_font;
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
		RecRescaleDrawGraph( 460, 20, val.mat.difBer.handle(), TRUE);
		RecRescaleDrawString(100, 13, val.songN.c_str(), COLOR_WHITE);

		/* 判定周り */
		char_font.RescaleDrawString( 20,  50, 60 / 64.0, _T("JUST:"), 0);
		num_font.RescaleDrawNum(    140,  52, 30 / 64.0, val.judge.just, CUR_FONT_COLOR_BLUE);
		char_font.RescaleDrawString( 20,  93, 60 / 64.0, _T("GOOD:"), 0);
		num_font.RescaleDrawNum(    140,  93, 30 / 64.0, val.judge.good, CUR_FONT_COLOR_YELLOW);
		char_font.RescaleDrawString( 20, 134, 60 / 64.0, _T("SAFE:"), 0);
		num_font.RescaleDrawNum(    140, 134, 30 / 64.0, val.judge.safe, CUR_FONT_COLOR_GREEN);
		char_font.RescaleDrawString( 20, 175, 60 / 64.0, _T("MISS:"), 0);
		num_font.RescaleDrawNum(    140, 175, 30 / 64.0, val.judge.miss, CUR_FONT_COLOR_RED);
		char_font.RescaleDrawString( 20, 215, 60 / 64.0, _T("COMBO:"), 0);
		num_font.RescaleDrawNum(    155, 215, 30 / 64.0, val.Mcombo,     CUR_FONT_COLOR_BLUE);
		num_font.RescaleDrawNum(    265, 215, 30 / 64.0, val.noteCount,  CUR_FONT_COLOR_PURPLE);

		/* レート周り */
		RecRescaleDrawFormatString(10, 320, COLOR_WHITE, L"%d.%02d", val.newRate / 100, val.newRate % 100);
		if (0 < val.subRate) { RecRescaleDrawFormatString(10, 340, COLOR_WHITE, L"+%d.%02d", val.subRate / 100, val.subRate % 100); }
		else { RecRescaleDrawString(10, 340, L"not rise", COLOR_WHITE); }

		/* スコア周り */
		char_font.RescaleDrawString(280,  40, 60 / 64.0, _T("SCORE:"), 0);
		num_font.RescaleDrawNum(    310,  75, 55 / 64.0, val.score, val.mat.fontNo);
		char_font.RescaleDrawString(300, 150, 60 / 64.0, _T("ACC:"), 0);
		num_font.RescaleDrawFloat(  430, 150, 30 / 64.0, val.acc,   val.mat.fontNo, 2);
		char_font.RescaleDrawString(400, 205, 40 / 64.0, _T("GAP:"), 0);
		num_font.RescaleDrawFloat(  490, 205, 20 / 64.0, val.gap,   CUR_FONT_COLOR_MONO, 2);
		char_font.RescaleDrawString(400, 230, 40 / 64.0, _T("WIDTH:"), 0);
		num_font.RescaleDrawFloat(  500, 230, 20 / 64.0, val.width, CUR_FONT_COLOR_MONO, 2);

		/* ランク周り */
		char_font.RescaleDrawString( 20, 260, 90 / 64.0, _T("RANK:"), 0);
		RecRescaleDrawGraph(        140, 260, val.mat.rank.handle(),      TRUE);
		RecRescaleDrawGraph(          5, 420, val.mat.clearRate.handle(), TRUE);
		RecRescaleDrawGraph(        336, 252, val.mat.chara.handle(),     TRUE);
		num_font.RescaleDrawUnderFloat(280, 390, 30 / 64.0, val.floatRank, val.mat.floatfontNo, 3);

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

	return SCENE_SELECT;
}

#if 1 /* RecResultCalParameter */

static rec_score_rate_t CalScoreRank(int score) {
	if (score >= RED_SCORE_RC_BORDER) { return REC_SCORE_RATE_EX; }
	if (score >= RED_SCORE_S_BORDER ) { return REC_SCORE_RATE_S; }
	if (score >= RED_SCORE_A_BORDER ) { return REC_SCORE_RATE_A; }
	if (score >= RED_SCORE_B_BORDER ) { return REC_SCORE_RATE_B; }
	if (score >= RED_SCORE_C_BORDER ) { return REC_SCORE_RATE_C; }
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
static int CalPlayRate(const rec_play_judge_t &judge, const rec_map_detail_t &map_detail) {
	const double DifRate = CAL_DIF_RATE(map_detail.mpal.mdif, map_detail.Lv) / 100.0;

	double rate = 0;
	// level0なら0固定
	if (DifRate == 0) { rate = 0; }
	// "譜面定数" - "miss数" x "譜面定数" x 0.03 (下限=0)
	else if (judge.miss > 0) {
		rate = DifRate - judge.miss * DifRate * 0.03;
		rate = maxs_2(rate, 0);
	}
	// NO MISS,"譜面定数" + 1 - "safe数" x 0.05 (下限="譜面定数")
	else if (judge.miss == 0 && judge.safe > 0) {
		rate = DifRate + 1 - judge.safe * 0.05;
		rate = maxs_2(rate, DifRate);
	}
	// FULL COMBO,"譜面定数" + 2 - "good数" x 0.01 (下限="譜面定数" + 1)
	else if (judge.miss == 0 && judge.safe == 0 && judge.good > 0) {
		rate = DifRate + 2 - judge.good * 0.01;
		rate = maxs_2(rate, DifRate + 1);
	}
	// PERFECT, "譜面定数" + 2
	else if (judge.miss == 0 && judge.safe == 0 && judge.good == 0) {
		rate = DifRate + 2;
	}
	return (int)(rate * 100);
}

rec_clear_rank_t JudgeClearRank(const rec_play_userpal_t &userpal) {
	/* TODO: LOSTED, LIGHTCLEAR, MISSLESS, FULLPERFECTを作る  */
	/* if (userpal.status == REC_PLAY_STATUS_LOSTED) { return REC_CLEAR_RANK_LOSTED; } */
	if (userpal.status == REC_PLAY_STATUS_DROPED) { return REC_CLEAR_RANK_DROPED; }
	/* if (userpal.Exlife < 0) { return REC_CLEAR_RANK_CLEARED; } */
	/* if (5 < userpal.judgeCount.miss) { return REC_CLEAR_RANK_LIGHTCLEAR; } */
	/* if (0 < userpal.judgeCount.miss) { return REC_CLEAR_RANK_MISSLESS; } */
	if (0 < userpal.judgeCount.miss) { return REC_CLEAR_RANK_CLEARED; }
	if (0 < userpal.judgeCount.safe) { return REC_CLEAR_RANK_NOMISS; }
	if (0 < userpal.judgeCount.good) { return REC_CLEAR_RANK_FULLCOMBO; }
	/* if (userpal.score.sum < 100100) { return REC_CLEAR_RANK_PERFECT; } */
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
		return pals(0, 1000, 100, 0, DIV_AVOID_ZERO(100 * miss, notes, 100));
	}
	return 0;
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
		rec_bgm_system_g.SetMem(_T("song/Regret.culk"));
	}
	else {
		rec_bgm_system_g.SetMem(_T("song/Balloon Art.culk"));
	}
	return;
}

/**
 * リザルト表示に必要な情報を取得します。
 */
static void RecResultCalParameter(rec_result_pal_t &result_pal, const rec_play_userpal_t &userpal,
	const rec_play_nameset_t &nameset, rec_dif_t dif, short noteCount)
{
	const rec_clear_rank_t Clear = JudgeClearRank(userpal);
	const rec_score_rate_t rank  = CalScoreRank(userpal.score.GetAllScore());

	result_pal.acc             = CAL_ACC(userpal.judgeCount, noteCount);
	result_pal.gap             = userpal.gap.GetAverage();
	result_pal.width           = userpal.gap.GetDeviation();
	result_pal.score           = userpal.score.GetAllScore();
	result_pal.judge           = userpal.judgeCount;
	result_pal.Mcombo          = userpal.Mcombo;
	result_pal.noteCount       = noteCount;
	result_pal.floatRank       = GetFloatRank(userpal.score.GetAllScore(), userpal.judgeCount.miss, noteCount, rank) / 1000;
	result_pal.newRate         = RecSaveGetFullRunnerRate() * 100;
	result_pal.subRate         = result_pal.newRate - result_pal.oldRate; /* oldRateはレート保存前に代入している */

	result_pal.mat.clearRate.set_pic(Clear);
	result_pal.mat.difBer.set_pic(dif, nameset.DifFN);
	result_pal.mat.rank.set_pic(rank);
	// result_pal.mat.chara.set_pic(); TODO クリア状況でイラスト変える。
	result_pal.mat.fontNo      = RecResultGetCurFontColor(rank);
	result_pal.mat.floatfontNo = RecResultGetFlortCurFontColor(rank);

	RecResultSetBgm(userpal.status);

	result_pal.songN = nameset.songN;
	return;
}

#endif /* RecResultCalParameter */

static void SaveScore(const rec_play_userpal_t &userpal, const tstring &songN, rec_dif_t dif,
	short noteCount)
{
	rec_save_score_t buf;

	buf.clearRank = JudgeClearRank(userpal);
	buf.scoreRate = CalScoreRank(userpal.score.GetAllScore());
	buf.score     = userpal.score.GetAllScore();
	buf.dist      = userpal.Dscore.GetAllPoint();
	buf.acc       = CAL_ACC(userpal.judgeCount, noteCount);

	RecSaveUpdateScoreOneDif(buf, songN, dif);
	return;
}

now_scene_t result(const rec_to_result_st &toResult, const rec_to_play_set_t &ps) {
	rec_result_pal_t result_pal;
	tstring fileName;

	RecGetMusicFolderName(fileName, ps.packNo, ps.musicNo);

	/* セーブ前作業 */
	result_pal.oldRate = RecSaveGetFullRunnerRate() * 100;

	/* セーブ作業 */
	RecSaveUpdateUserPlay(&toResult.userpal);
	SaveScore(toResult.userpal, fileName, (rec_dif_t)ps.dif, toResult.map_detail.notes);
	RecSaveUpdateCharaPlay((rec_chara_et)optiondata.chara);
	RecSaveUpdateRunnerRate(fileName, (double)CalPlayRate(toResult.userpal.judgeCount, toResult.map_detail) / 100.0);

	/* リザルト表示 */
	RecResultCalParameter(result_pal, toResult.userpal, toResult.nameset, (rec_dif_t)ps.dif, toResult.map_detail.notes);
	return ViewResult(result_pal);
}
