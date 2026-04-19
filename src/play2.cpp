
#if 1 /* define group */

/* include */

/* base include */
#include <DxLib.h>

/* curbine code include */
#include <dxcur.h>
#include <dxdraw.h>
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <RecSystem.h>
#include <option.h>
#include <recr_cutin.h>
#include <RecSave.h>
#include <RecScoreFile.h>
#include <RecWindowRescale.h>

/* rec play include */
#include <playbox.h>
#include <PlayAuto.h>
#include <PlayCamera.h>
#include <PlayHitEff.h>
#include <PlayNoteJudge.h>
#include <PlayViewJudge.h>

/* rec sub include */
#include <RecordLoad2.h>
#include <recp_cal_ddif_2.h>
#include <result.h>

/* define */

#endif /* define group */

#if 1 /* typedef group */

typedef rec_map_eff_data_t mapeff_t;

/* struct */
typedef struct rec_play_back_pic_s {
	dxcur_pic_c sky;
	dxcur_pic_c ground;
	dxcur_pic_c water;
} rec_play_back_pic_t;

typedef struct note_img {
	dxcur_pic_c notebase = dxcur_pic_c(L"picture/hit.png");
	dxcur_pic_c hitcircle[6] = {
		dxcur_pic_c(L"picture/hitc-G.png"),
		dxcur_pic_c(L"picture/hitc-R.png"),
		dxcur_pic_c(L"picture/hitc-B.png"),
		dxcur_pic_c(L"picture/hitc-Y.png"),
		dxcur_pic_c(L"picture/hitc-X.png"),
		dxcur_pic_c(L"picture/hitc-W.png"),
	};
	dxcur_pic_c catchi = dxcur_pic_c(L"picture/catch.png");
	dxcur_pic_c up     = dxcur_pic_c(L"picture/up.png");
	dxcur_pic_c down   = dxcur_pic_c(L"picture/down.png");
	dxcur_pic_c left   = dxcur_pic_c(L"picture/left.png");
	dxcur_pic_c right  = dxcur_pic_c(L"picture/right.png");
	dxcur_pic_c bomb   = dxcur_pic_c(L"picture/bomb.png");
	dxcur_pic_c goust  = dxcur_pic_c(L"picture/goust.png");
} rec_play_notepic_t;

#endif /* typedef group */

#if 1 /* ボーナス系 */

class rec_play_bonus_cover_c {
private:
	DxTime_t Stime = 0;
	int alpha = 0;
	dxcur_pic_c pic{_T("picture/Black.png")};

public:
	void draw(void) const {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->alpha);
		RecRescaleDrawGraph(0, 0, this->pic.handle(), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}

	void update(DxTime_t Ntime) {
		this->alpha = lins_scale(1800, 127, 2000, 0, Ntime);
	}

	void setBonus(void) {
		this->Stime = GetNowCount();
	}
};

class rec_play_bonus_slight_c {
private:
	DxTime_t Stime = 0;

	int posX[24];
	int posY[24];
	size_t count = 0;

	dxcur_pic_c pic_list[3] = {
		dxcur_pic_c(_T("picture/Bonus-Smalllight1.png")),
		dxcur_pic_c(_T("picture/Bonus-Smalllight2.png")),
		dxcur_pic_c(_T("picture/Bonus-Smalllight3.png"))
	};
	DxPic_t using_pic = DXLIB_PIC_NULL;

public:
	void draw(void) const {
		DxTime_t Ntime = GetNowCount() - this->Stime;

		for (int i = 0; i < this->count; i++) {
			int posX = this->posX[i];
			int posY = this->posY[i];
			int alpha = lins(100, 720, 1000, -240, Ntime);
			alpha = maxs_2(pals(posY, 255, posY + 240, 0, alpha), 0);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			RecRescaleDrawGraph(posX, posY, using_pic, TRUE); /* TODO: 回転したい */
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}

	void setBonus(int mat) {
		int Bonus = -1;
		this->using_pic = this->pic_list[mat - 1].handle();
		this->Stime = GetNowCount();
		this->count = 6 * (4 - Bonus);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 4 - Bonus; k++) {
					posX[12 * i + 4 * j + k] = j * 180 + this->Stime % (57137 + 29 * i + 67 * j + 127 * k) % 180;
					posY[12 * i + 4 * j + k] = i * 190 + this->Stime % (62843 + 37 * i + 67 * j + 157 * k) % 190;
				}
			}
		}
	}
};

class rec_play_bonus_blight_c {
private:
	DxTime_t Stime = 0;
	int alpha = 0;
	size_t count = 0;
	dxcur_pic_c pic{_T("picture/Bonus-Biglight.png")};

public:
	void draw(void) const {
		DxTime_t Ntime = GetNowCount() - this->Stime;
		if (Ntime < 100) { return; }
		if (1000 < Ntime) { return; }
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->alpha);
		for (int i = 0; i < this->count; i++) {
			double angle = Ntime / 200.0;
			angle += 3.14 * (180.0 / this->count) * i / 180.0;
			RecRescaleDrawRotaGraph(320, 240, 1, angle, this->pic.handle(), TRUE);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}

	void update(DxTime_t Ntime) {
		if (IS_BETWEEN(100, Ntime, 1000)) {
			for (int i = 0; i < this->count; i++) {
				this->alpha = lins(500, 255, 1000, 0, maxs_2(Ntime, 500));
			}
		}
	}

	void setBonus(int mat) {
		this->count = mat;
		this->Stime = GetNowCount();
	}
};

class rec_play_bonus_ring_c {
private:
	DxTime_t Stime = 0;

	int LeftPos  = 0;
	int UpPos    = 0;
	int RightPos = 0;
	int DownPos  = 0;
	int alpha    = 0;

	dxcur_pic_c pic{_T("picture/Bonus-Ring.png")};

public:
	void draw(void) const {
		DxTime_t Ntime = GetNowCount() - this->Stime;
		if (Ntime < 100) { return; }
		if (1000 < Ntime) { return; }
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->alpha);
		RecRescaleDrawExtendGraph(this->LeftPos, this->UpPos, this->RightPos, this->DownPos, this->pic.handle(), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}

	void update(DxTime_t Ntime) {
		if (IS_BETWEEN(100, Ntime, 1000)) {
			this->LeftPos  = lins(100, 320 - 160, 1000, 320 - 240, Ntime);
			this->UpPos    = lins(100, 240 - 160, 1000, 240 - 240, Ntime);
			this->RightPos = lins(100, 320 + 160, 1000, 320 + 240, Ntime);
			this->DownPos  = lins(100, 240 + 160, 1000, 240 + 240, Ntime);
			this->alpha    = lins_scale(700, 255, 1000, 0, Ntime);
		}
	}

	void setBonus(void) {
		this->Stime = GetNowCount();
	}
};

class rec_play_bonus_flash_c {
private:
	DxTime_t Stime = 0;
	int alpha = 0;
	dxcur_pic_c pic{_T("picture/White.png")};

public:
	void draw(void) const {
		DxTime_t Ntime = GetNowCount() - this->Stime;
		if (Ntime < 100) { return; }
		if (300 < Ntime) { return; }
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->alpha);
		RecRescaleDrawGraph(0, 0, this->pic.handle(), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}

	void update(DxTime_t Ntime) {
		if (IS_BETWEEN(100, Ntime, 300)) {
			this->alpha = lins(100, 191, 300, 0, Ntime);
		}
	}

	void setBonus(void) {
		this->Stime = GetNowCount();
	}
};

class rec_play_bonus_text_c {
private:
	const int sizeX = 319;
	const int sizeY = 54;
	DxTime_t Stime = 0;

	int LeftPos  = 0;
	int UpPos    = 0;
	int RightPos = 0;
	int DownPos  = 0;
	int alpha    = 0;

	dxcur_pic_c pic_list[3] = {
		dxcur_pic_c(_T("picture/NOMISS.png"   )),
		dxcur_pic_c(_T("picture/FULLCOMBO.png")),
		dxcur_pic_c(_T("picture/PERFECT.png"  ))
	};
	DxPic_t using_pic = DXLIB_PIC_NULL;

public:
	void draw(void) const {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->alpha);
		RecRescaleDrawExtendGraph(this->LeftPos, this->UpPos, this->RightPos, this->DownPos, this->using_pic, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}

	void update(DxTime_t Ntime) {
		if (Ntime < 1800) {
			this->LeftPos  = lins_scale(0, 320 - sizeX * 8 / 2, 100, 320 - sizeX / 2, Ntime);
			this->UpPos    = lins_scale(0, 240 - sizeY * 8 / 2, 100, 240 - sizeY / 2, Ntime);
			this->RightPos = lins_scale(0, 320 + sizeX * 8 / 2, 100, 320 + sizeX / 2, Ntime);
			this->DownPos  = lins_scale(0, 240 + sizeY * 8 / 2, 100, 240 + sizeY / 2, Ntime);
			this->alpha    = 255;
		}
		else { /* (1800 <= NTime) */
			int LeftPos  = lins_scale(2000, 320 - sizeX, 1800, 320 - sizeX / 2, Ntime);
			int UpPos    = lins_scale(2000, 240 - sizeY, 1800, 240 - sizeY / 2, Ntime);
			int RightPos = lins_scale(2000, 320 + sizeX, 1800, 320 + sizeX / 2, Ntime);
			int DownPos  = lins_scale(2000, 240 + sizeY, 1800, 240 + sizeY / 2, Ntime);
			int alpha    = lins_scale(1800, 255, 2000, 0, Ntime);
		}
	}

	void setBonus(int mat) {
		this->using_pic = this->pic_list[mat - 1].handle();
		this->Stime = GetNowCount();
	}
};

class rec_play_bonus_snd_c {
private:
	dxcur_snd_c snd[3] = {
		dxcur_snd_c(_T("sound/a-nomiss.mp3"   )),
		dxcur_snd_c(_T("sound/a-fullcombo.mp3")),
		dxcur_snd_c(_T("sound/a-perfect.mp3"  ))
	};

public:
	rec_play_bonus_snd_c(void) {
		ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, this->snd[0].handle());
		ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, this->snd[1].handle());
		ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, this->snd[2].handle());
	}

	void setBonus(int mat) {
		this->snd[mat - 1].PlaySound();
	}
};

class rec_play_bonus_c {
private:
	bool enable = false;
	DxTime_t Stime = 0;

	rec_play_bonus_cover_c  g_bonus_cover;
	rec_play_bonus_slight_c g_bonus_slight;
	rec_play_bonus_blight_c g_bonus_blight;
	rec_play_bonus_ring_c   g_bonus_ring;
	rec_play_bonus_flash_c  g_bonus_flash;
	rec_play_bonus_text_c   g_bonus_text;
	rec_play_bonus_snd_c    g_bonus_snd;

public:
	void draw(void) const {
		if (!this->enable) { return; }
		g_bonus_cover.draw();
		g_bonus_slight.draw();
		g_bonus_blight.draw();
		g_bonus_ring.draw();
		g_bonus_flash.draw();
		g_bonus_text.draw();
	}

	void update(void) {
		if (!this->enable) { return; }
		DxTime_t Ntime = GetNowCount() - this->Stime;
		if (2000 < Ntime) {
			this->enable = false;
			return;
		}
		g_bonus_cover.update(Ntime);
		g_bonus_blight.update(Ntime);
		g_bonus_ring.update(Ntime);
		g_bonus_flash.update(Ntime);
		g_bonus_text.update(Ntime);
	}

	void setBonus(const rec_play_judge_t &judge) {
		int mat = 0;
		if (judge.miss > 0) { return; }
		if (judge.safe > 0) {
			mat = 1;
		}
		else if (judge.good > 0) {
			mat = 2;
		}
		else {
			mat = 3;
		}
		this->Stime = GetNowCount();
		this->enable = true;
		g_bonus_snd.setBonus(mat);
		g_bonus_cover.setBonus();
		g_bonus_slight.setBonus(mat);
		g_bonus_blight.setBonus(mat);
		g_bonus_ring.setBonus();
		g_bonus_flash.setBonus();
		g_bonus_text.setBonus(mat);
	}
};

#endif /* ボーナス系 */

#if 1 /* sub action */

static void RecResetPlayPartNum(short LineMoveN[]) {
	LineMoveN[0] = 0;
	LineMoveN[1] = 0;
	LineMoveN[2] = 0;
	return;
}

static void RecResetPlayRecfpMapeffNum(rec_map_eff_data_t *mapeff) {
	mapeff->camera.resetNo();
	mapeff->scrool.resetNo();
	mapeff->carrow.resetNo();
	mapeff->move.y[0].resetNo();
	mapeff->move.y[1].resetNo();
	mapeff->move.y[2].resetNo();
	mapeff->move.y[3].resetNo();
	mapeff->move.y[4].resetNo();
	mapeff->move.x[0].resetNo();
	mapeff->move.x[1].resetNo();
	mapeff->move.x[2].resetNo();
	mapeff->chamo[0].resetNo();
	mapeff->chamo[1].resetNo();
	mapeff->chamo[2].resetNo();
	mapeff->fall.resetNo();
	mapeff->lock.x.resetNo();
	mapeff->lock.y.resetNo();
	mapeff->viewT.resetNo();
	mapeff->speedt[0].resetNo();
	mapeff->speedt[1].resetNo();
	mapeff->speedt[2].resetNo();
	mapeff->speedt[3].resetNo();
	mapeff->speedt[4].resetNo();
	mapeff->Movie.resetNo();
	mapeff->viewLine.resetNo();
	return;
}

static void RecResetPlayObjectNum(rec_score_file_t *recfp) {
	recfp->mapdata.note[0].resetNo();
	recfp->mapdata.note[1].resetNo();
	recfp->mapdata.note[2].resetNo();
	return;
}

static void Getxxxpng(tstring &str, int num) {
	str += num / 100 + '0';
	str += num / 10 % 10 + '0';
	str += num % 10 + '0';
	str += _T(".png");
}

static void Getxxxwav(tstring &str, int num) {
	str += num / 100 + '0';
	str += num / 10 % 10 + '0';
	str += num % 10 + '0';
	str += _T(".wav");
}

static void recSetLine(int line[], cvec<rec_mapeff_move_st> move[], int Ntime, int loop) {
	for (int iLine = 0; iLine < loop; iLine++) {
		if (IS_BETWEEN(0, move[iLine].nowData().Stime, Ntime)) {
			line[iLine] =
				(int)movecal_scale(
					move[iLine].nowData().mode,
					move[iLine].nowData().Stime,
					move[iLine].offsetData(-1).pos,
					move[iLine].nowData().Etime,
					move[iLine].nowData().pos, Ntime);
		}
		/* TODO: このwhile文の中はこの関数内でやることではないような... */
		while (
			!move[iLine].isEndNo() &&
			move[iLine].nowData().Etime <= Ntime &&
			move[iLine].nowData().Stime >= 0 ||
			move[iLine].nowData().mode == 4)
		{
			line[iLine] = move[iLine].nowData().pos;
			move[iLine].stepNo();
		}
	}
}

static void PlayDrawItem(rec_map_eff_data_t *mapeff, int Ntime, int Xmidline, const std::vector<DxPic_t> &item) {
	int drawA;
	int drawX;
	int drawY;
	int drawS;
	int drawR;
	rec_play_xy_set_t camera;
	RecPlayGetCameraPos(&camera.x, &camera.y);
	for (size_t i = mapeff->Movie.nowNo(); i < mapeff->Movie.size(); i++) {
		const item_box *pMovie = &mapeff->Movie.at(i);
		if ((Ntime < pMovie->starttime) || (pMovie->endtime < Ntime)) { continue; }
		//base setting
		drawA = (int)movecal(pMovie->movemode,
			pMovie->starttime, pMovie->startalpha,
			pMovie->endtime, pMovie->endalpha, Ntime);
		drawX = (int)movecal(pMovie->movemode,
			pMovie->starttime, pMovie->startXpos,
			pMovie->endtime, pMovie->endXpos, Ntime) + camera.x;
		drawY = (int)movecal(pMovie->movemode,
			pMovie->starttime, pMovie->startYpos,
			pMovie->endtime, pMovie->endYpos, Ntime) + camera.y;
		drawS = (int)movecal(pMovie->movemode,
			pMovie->starttime, pMovie->startsize,
			pMovie->endtime, pMovie->endsize, Ntime);
		drawR = (int)movecal(pMovie->movemode,
			pMovie->starttime, pMovie->startrot,
			pMovie->endtime, pMovie->endrot, Ntime);
		//material setting
		if (pMovie->eff.lock == 1) {
			drawX -= camera.x;
		}
		if (pMovie->eff.lock == 1) {
			drawY -= 25 + camera.y;
		}
		if (pMovie->eff.bpm_alphr == 1) {
			drawA = lins(0, drawA, 60000 / mapeff->v_BPM.nowData(), 0,
				(Ntime - mapeff->v_BPM.nowDataTime()) % (int)(60000 / mapeff->v_BPM.nowData()));
		}
		if (pMovie->eff.chara_alphr == 1) {
			drawA = lins(320, drawA, 60, 0, betweens(60, abss(Xmidline, drawX), 320));
		}
		if (pMovie->eff.bpm_size == 1) {
			drawS = pals(60000 / mapeff->v_BPM.nowData(), drawS / 2.0, 0, drawS,
				(Ntime - mapeff->v_BPM.nowDataTime()) % (int)(60000 / mapeff->v_BPM.nowData()));
		}
		if (pMovie->eff.edge_size == 1) {
			drawS = betweens(0, lins(540, drawS, 640, 0, drawX), drawS);
			drawS = betweens(0, lins(100, drawS, 0, 0, drawX), drawS);
		}
		//rescale
		drawX = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, drawX);
		drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, drawY);
		drawS = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, drawS);
		//drawing
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, drawA);
		DrawDeformationPic(drawX, drawY, drawS / 100.0, drawS / 100.0, drawR, item[pMovie->ID]);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	}
}

static int GetRemainNotes(rec_play_judge_t judge, int Notes) {
	return Notes - judge.just - judge.good - judge.safe - judge.miss;
}

static int GetHighScore(const TCHAR *songName, rec_dif_t dif) {
	rec_save_score_t scoreBuf;
	RecSaveReadScoreOneDif(&scoreBuf, songName, dif);
	return scoreBuf.score;
}

static void RecPlayStepCameraNum(cvec<rec_camera_data_t> &camera, DxTime_t Ntime) {
	while (!camera.isEndNo() && camera.offsetData(1).starttime <= Ntime) {
		camera.stepNo();
	}
}

static void RecPlayStepMovieNum(cvec<item_box> &movie, DxTime_t Ntime) {
	if (optiondata.backbright == 0) { return; }
	if (movie.size() == 0) { return; }
	while (!movie.isEndNo() && IS_BETWEEN_LESS(-500, movie.nowData().endtime, Ntime)) {
		movie.stepNo();
	}
}

static void RecPlayStepGuideLineNum(
	short guideN[], const cvec<rec_mapeff_move_st> ymove[], DxTime_t Ntime
) {
	for (int iLane = 0; iLane < 3; iLane++) {
		while (
			guideN[iLane] + 1 < ymove[iLane].size() &&
			((ymove[iLane].at(guideN[iLane]).Stime >= 0 &&
			ymove[iLane].at(guideN[iLane]).Etime <= Ntime) ||
			ymove[iLane].at(guideN[iLane]).mode == 4))
		{
			guideN[iLane]++;
		}
	}
	return;
}

static void RecPlayStepAllNum(rec_score_file_t *recfp, short *guideN) {
	recfp->mapeff.gnote.stepNoTime(recfp->time.now);

	/* chamo */
	recfp->mapeff.chamo[0].stepNoTime(recfp->time.now);
	recfp->mapeff.chamo[1].stepNoTime(recfp->time.now);
	recfp->mapeff.chamo[2].stepNoTime(recfp->time.now);

	/* speedt */
	recfp->mapeff.speedt[0].stepNoTime(recfp->time.now);
	recfp->mapeff.speedt[1].stepNoTime(recfp->time.now);
	recfp->mapeff.speedt[2].stepNoTime(recfp->time.now);
	if (optiondata.backbright != 0) {
		recfp->mapeff.speedt[3].stepNoTime(recfp->time.now);
	}

	recfp->mapeff.v_BPM.stepNoTime(recfp->time.now);
	RecPlayStepCameraNum(recfp->mapeff.camera, recfp->time.now);
	recfp->mapeff.scrool.stepNoTime(recfp->time.now);
	RecPlayStepMovieNum(recfp->mapeff.Movie, recfp->time.now);
	if (optiondata.backbright != 0) {
		recfp->mapeff.fall.stepNoTime(recfp->time.now);
	}
	RecPlayStepGuideLineNum(guideN, recfp->mapeff.move.y, recfp->time.now);
	recfp->mapeff.carrow.stepNoTime(recfp->time.now);
	recfp->mapeff.lock.x.stepNoTime(recfp->time.now);
	recfp->mapeff.lock.y.stepNoTime(recfp->time.now);
	recfp->mapeff.viewT.stepNoTime(recfp->time.now);
	recfp->mapeff.viewLine.stepNoTime(recfp->time.now);
	return;
}

/* キー入力 */
static void RecPlayActSubKey(
	rec_score_file_t *recfp, int AutoFlag, int *StopFrag, short LineMoveN[]
) {
	InputAllKeyHold();
	if (AutoFlag == 1) {
		switch (GetKeyPushOnce()) {
		case KEY_INPUT_SPACE:
			(*StopFrag) *= -1;
			if (*StopFrag == 1) { rec_bgm_system_g.Stop(); }
			else {
				rec_bgm_system_g.SetCurrentPosition((int)((double)recfp->time.now / 1000.0 * 44100.0));
				rec_bgm_system_g.Play(true, false, false);
			}
			break;
		default:
			break;
		}
	}

	if (*StopFrag != 1) { return; }

	switch (GetKeyPushOnce()) {
	case KEY_INPUT_LEFT:
		recfp->time.now = maxs_2(recfp->time.now - 10000, 0);
		RecResetPlayObjectNum(recfp);
		RecResetPlayRecfpMapeffNum(&recfp->mapeff);
		RecResetPlayPartNum(LineMoveN);
		for (int iLane = 0; iLane < 3; iLane++) {
			while (recfp->mapdata.note[iLane].nowData().hittime < recfp->time.now &&
				(recfp->mapdata.note[iLane].nowNo() + 1) < recfp->mapdata.note[iLane].size())
			{
				recfp->mapdata.note[iLane].stepNo();
			}
		}
		break;
	case KEY_INPUT_RIGHT:
		recfp->time.now += 10000;
		for (int iLane = 0; iLane < 3; iLane++) {
			while (recfp->mapdata.note[iLane].nowData().hittime < recfp->time.now &&
				(recfp->mapdata.note[iLane].nowNo() + 1) < recfp->mapdata.note[iLane].size())
			{
				recfp->mapdata.note[iLane].stepNo();
			}
		}
		break;
	default:
		break;
	}
	return;
}

static void RacPlayDrawFieldGrid(void) {
	int cameraX = 0;
	int cameraY = 0;
	RecPlayGetCameraPos(&cameraX, &cameraY);
	for (int inum = -5; inum < 10; inum++) {
		TCHAR str[5];
		_stprintf_s(str, _T("%d"), inum);
		DrawStringRecField(30, 50 * inum + 8 + 100, str, COLOR_WHITE);
	}
	return;
}

/* キャッチ判定に使う数値を計算 */
static void RecPlayCalLaneTrack(int LaneTrack[], const rec_play_key_hold_t *keyhold, int charaput, int Ntime) {
	LaneTrack[charaput] = Ntime;
	if (keyhold->up == 0 && keyhold->down == 0 || keyhold->up > 0 && keyhold->down > 0) { LaneTrack[1] = Ntime; }
	else if (keyhold->up > 0 && keyhold->down == 0) { LaneTrack[0] = Ntime; }
	else if (keyhold->up == 0 && keyhold->down > 0) { LaneTrack[2] = Ntime; }
	if (LaneTrack[0] <= LaneTrack[2]) { LaneTrack[1] = maxs_2(LaneTrack[1], LaneTrack[0]); }
	else { LaneTrack[1] = maxs_2(LaneTrack[1], LaneTrack[2]); } 
	return;
}

#endif /* sub action */

#if 1 /* Guide Line */

static int RecMapGetPosFromMove(const cvec<rec_mapeff_move_st> &move, int time) {
	int ret = 0;
	int offset = 1;

	while (
		offset + 1 < move.size() &&
		IS_BETWEEN(0, move.at(offset).Etime, time)
	) {
		offset++;
	}

	if (move.at(offset).Stime < 0) {
		return move.at(offset - 1).pos;
	}

	if (move.at(offset).Etime <= move.at(offset).Stime) {
		if (time < move.at(offset).Stime) {
			return move.at(offset - 1).pos;
		}
		else {
			return move.at(offset).pos;
		}
	}

	if (IS_BETWEEN(0, move.at(offset).Stime, time) && move.at(offset).Etime > time) {
		ret = movecal(move.at(offset).mode,
			move.at(offset).Stime,
			move.at(offset - 1).pos,
			move.at(offset).Etime, move.at(offset).pos,
			time);
	}
	else {
		ret = move.at(offset - 1).pos;
	}

	return ret;
}

static void RecPlayGetTimeLanePosBase(int *DrawX, int *DrawY, rec_move_all_set_t *move,
	const rec_play_lanepos_t *lanePos, double speedt, bool Ylock, const rec_scrool_data_t &scrool,
	int Ntime, int Ptime, int lane
) {
	/* TODO: 今後使うから消すな */
	const double scroolSpeed = scrool.speed;
	const double scroolBaseTime = scrool.basetime;

	*DrawY = (Ylock ? RecMapGetPosFromMove(move->y[lane], Ptime) : lanePos->y[lane]) + 15;

	/* TODO: スクロールの考慮ができていない、てかVtime作りたい */
	*DrawX = (int)((speedt * 20 * (Ptime - Ntime) + 5000) / 50) + 50;
	*DrawX += lanePos->x[lane] - 150 + 15;
	return;
}

static void RecPlayGetTimeLanePos(int *retX, int *retY, rec_map_eff_data_t *mapeff,
	const rec_play_lanepos_t *lanePos, int iLine, int Ntime, int Ptime)
{
	/* TODO: 関数まとめ */
	RecPlayGetTimeLanePosBase(retX, retY, &mapeff->move, lanePos,
		mapeff->speedt[iLine].searchDataFront(Ptime),
		mapeff->lock.y.searchDataFront(Ptime),
		mapeff->scrool.nowData(), Ntime, Ptime, iLine);
}

void RecPlayDrawGuideBorder(rec_score_file_t *recfp, const rec_play_lanepos_t *lanePos) {
	int Ptime = 0;
	int pnum  = 0;
	DxTime_t Ntime = recfp->time.now;
	static const uint divColor = 20;

	/* Ntime以上のPスポットを計算したい */
	/* Pスポットとなりえる配列は[offset, offset + 60000 / (bpm / 4), offset + 2 * 60000 / (bpm / 4), ...] */
	/* これは等差数列、初項 = offset, 公差 = 240000 / bpm, Pスポット = 初項 + n * 公差, (ここでは、nは0スタート) */
	/* Ntime <= Pスポット となるような最小のPスポットがほしい */
	/* Ntime <= 初項 + n * 公差 */
	/* Ntime - 初項 <= n * 公差 */
	/* (Ntime - 初項) / 公差 <= n */
	/* roundup((Ntime - 初項) / 公差) = n */
	/* roundup((Ntime - offset) / (240000 / bpm)) = n */
	/* roundup((Ntime - offset) * bpm / 240000) = n */
	pnum = (Ntime - recfp->time.offset) * recfp->mapdata.bpm / 24000;
	pnum = maxs_2(pnum, 0);
	if ((pnum % 10) != 0) { pnum = pnum / 10 + 1; }
	else { pnum = pnum / 10; }

	for (uint iNum = 0; iNum < 7; iNum++) {
		int posX1 = 0;
		int posY1 = 0;
		int posX2 = 0;
		int posY2 = 0;
		int posX3 = 0;
		int posY3 = 0;

		/* 位置計算 */
		Ptime = recfp->time.offset + 240000 * (pnum + iNum) / recfp->mapdata.bpm;

		if (!recfp->mapeff.viewLine.searchDataFront(Ptime)) { continue; }
		if (recfp->time.end < Ptime) { return; }

		RecPlayGetTimeLanePos(&posX1, &posY1, &recfp->mapeff, lanePos, 0, Ntime, Ptime);
		RecPlayGetTimeLanePos(&posX2, &posY2, &recfp->mapeff, lanePos, 1, Ntime, Ptime);
		RecPlayGetTimeLanePos(&posX3, &posY3, &recfp->mapeff, lanePos, 2, Ntime, Ptime);

		if (abss(posY1, posY2) < 25) {
			posY1 = posY2 - 20;
			posX1 = posX2;
		}
		if (abss(posY3, posY2) < 25) {
			posY3 = posY2 + 25;
			posX3 = posX2;
		}

		for (uint idraw = 0; idraw < divColor; idraw++) {
			int drawX  = lins( 0, posX1, divColor    , posX2, idraw);
			int drawY  = lins( 0, posY1, divColor    , posY2, idraw);
			int drawX2 = lins(-1, posX1, divColor - 1, posX2, idraw);
			int drawY2 = lins(-1, posY1, divColor - 1, posY2, idraw);
			int hueP   = lins( 0,     0, divColor - 1,    96, idraw);
			DrawLineRecField(drawX, drawY, drawX2, drawY2, GetColorCurRainbow(hueP, 100, 100), optiondata.barThick);
		}
		for (uint idraw = 0; idraw < divColor; idraw++) {
			int drawX  = lins( 0, posX2, divColor    , posX3, idraw);
			int drawY  = lins( 0, posY2, divColor    , posY3, idraw);
			int drawX2 = lins(-1, posX2, divColor - 1, posX3, idraw);
			int drawY2 = lins(-1, posY2, divColor - 1, posY3, idraw);
			int hueP   = lins( 0,    96, divColor - 1,   192, idraw);
			DrawLineRecField(drawX, drawY, drawX2, drawY2, GetColorCurRainbow(hueP, 100, 100), optiondata.barThick);
		}
	}
	return;
}

#define RET_DRAW       0
#define RET_DISABLE    1
#define RET_OVERSCREEN 2
#define RET_FINISH     3

/* ラスライン */
static int PlayShowLastGuideLine(rec_map_eff_data_t *mapeff, const cvec<rec_mapeff_move_st> &Ymove,
	const rec_play_lanepos_t *lanePos, int Line, int iDraw, int drawC, int Ntime, bool viewEn)
{
	int drawLeft = 0;
	int drawRight = 0;
	int drawY1 = 0;
	int drawY2 = 0;
	rec_play_xy_set_t camera;
	RecPlayGetCameraPos(&camera.x, &camera.y); /* TODO: スクリーン上での位置を返す関数が欲しい */
	RecPlayGetTimeLanePos(&drawLeft, &drawY1, mapeff, lanePos, Line, Ntime, Ymove[iDraw - 1].Etime);
	drawRight = 1280 - camera.x;
	drawY2    = drawY1;
	if (960 < drawLeft) { return RET_FINISH; }
	if (!viewEn) { return RET_FINISH; }
	DrawLineRecField(drawLeft, drawY1, drawRight, drawY2, drawC, optiondata.lineThick);
	return RET_FINISH;
}

/* スタートライン */
static int PlayShowStartGuideLine(rec_map_eff_data_t *mapeff, const cvec<rec_mapeff_move_st> &Ymove,
	const rec_play_lanepos_t *lanePos, int Line, int drawC, int Ntime, bool viewEn)
{
	int drawLeft = 0;
	int drawRight = 0;
	int drawY1 = 0;
	int drawY2 = 0;
	rec_play_xy_set_t camera;
	RecPlayGetCameraPos(&camera.x, &camera.y);
	RecPlayGetTimeLanePos(&drawRight, &drawY2, mapeff, lanePos, Line, Ntime, Ymove[0].Stime);
	drawLeft = 0 - camera.x;
	drawY1   = drawY2;
	if (960 < drawLeft) { return RET_OVERSCREEN; }
	if (!viewEn) { return RET_DISABLE; }
	DrawLineRecField(drawLeft, drawY1, drawRight, drawY2, drawC, optiondata.lineThick);
	return RET_DRAW;
}

/* move繋ぎライン */
static int PlayShowChainGuideLine(rec_map_eff_data_t *mapeff, const cvec<rec_mapeff_move_st> &Ymove,
	const rec_play_lanepos_t *lanePos, int Line, int iDraw, int drawC, int Ntime, bool viewEn)
{
	int drawLeft = 0;
	int drawRight = 0;
	int drawY1 = 0;
	int drawY2 = 0;
	RecPlayGetTimeLanePos(&drawLeft,  &drawY1, mapeff, lanePos, Line, Ntime, Ymove[iDraw - 1].Etime);
	RecPlayGetTimeLanePos(&drawRight, &drawY2, mapeff, lanePos, Line, Ntime, Ymove[iDraw].Stime);
	drawY2 = drawY1;
	if (960 < drawLeft) { return RET_OVERSCREEN; }
	if (!viewEn) { return RET_DISABLE; }
	DrawLineRecField(drawLeft, drawY1, drawRight, drawY2, drawC, optiondata.lineThick);
	return RET_DRAW;
}

/* moveライン */
static int PlayShowMovingGuideLine(rec_map_eff_data_t *mapeff, const cvec<rec_mapeff_move_st> &Ymove,
	const rec_play_lanepos_t *lanePos, int Line, int iDraw, int drawC, int Ntime, bool viewEn)
{
	int drawLeft = 0;
	int drawRight = 0;
	int drawY1 = 0;
	int drawY2 = 0;
	RecPlayGetTimeLanePos(&drawLeft,  &drawY1, mapeff, lanePos, Line, Ntime, Ymove[iDraw].Stime);
	RecPlayGetTimeLanePos(&drawRight, &drawY2, mapeff, lanePos, Line, Ntime, Ymove[iDraw].Etime);
	if (960 < drawLeft) { return RET_OVERSCREEN; }
	if (!viewEn) { return RET_DISABLE; }
	DrawLineCurveRecField(drawLeft, drawY1, drawRight, drawY2, Ymove[iDraw].mode, drawC, optiondata.lineThick);
	return RET_DRAW;
}

/* move後直角ライン */
static int PlayShowOuterGuideLine(rec_map_eff_data_t *mapeff, const cvec<rec_mapeff_move_st> &Ymove,
	const rec_play_lanepos_t *lanePos, int Line, int iDraw, int drawC, int Ntime, bool viewEn)
{
	int drawLeft = 0;
	int drawRight = 0;
	int drawY1 = 0;
	int drawY2 = 0;
	RecPlayGetTimeLanePos(&drawLeft,  &drawY1, mapeff, lanePos, Line, Ntime, Ymove[iDraw].Stime);
	RecPlayGetTimeLanePos(&drawRight, &drawY2, mapeff, lanePos, Line, Ntime, Ymove[iDraw].Etime);
	if (960 < drawLeft) { return RET_DISABLE; } // TODO: 止血コード、本来RET_OVERSCREEN
	if (!viewEn) { return RET_DISABLE; }
	DrawLineCurveRecField(drawLeft, drawY1, drawRight, drawY2, Ymove[iDraw].mode, drawC, optiondata.lineThick);
	return RET_DRAW;
}

/* move前直角ライン */
static int PlayShowInnerGuideLine(rec_map_eff_data_t *mapeff, const cvec<rec_mapeff_move_st> &Ymove,
	const rec_play_lanepos_t *lanePos, int Line, int iDraw, int drawC, int Ntime, bool viewEn)
{
	int drawLeft = 0;
	int drawRight = 0;
	int drawY1 = 0;
	int drawY2 = 0;
	RecPlayGetTimeLanePos(&drawLeft,  &drawY1, mapeff, lanePos, Line, Ntime, Ymove[iDraw].Stime);
	RecPlayGetTimeLanePos(&drawRight, &drawY2, mapeff, lanePos, Line, Ntime, Ymove[iDraw].Stime + 1);
	if (960 < drawLeft) { return RET_DISABLE; } // TODO: 止血コード、本来RET_OVERSCREEN
	if (!viewEn) { return RET_DISABLE; }
	DrawLineRecField(drawLeft, drawY1, drawRight, drawY2, drawC, optiondata.lineThick);
	return RET_DRAW;
}

/* 通常ライングループ */
static int PlayShowGuideLineStandard(rec_map_eff_data_t *mapeff, const cvec<rec_mapeff_move_st> &Ymove,
	const rec_play_lanepos_t *lanePos, int Line, int iDraw, int drawC, int Ntime, bool viewEn)
{
	int ret = 0;
	ret = PlayShowOuterGuideLine(mapeff, Ymove, lanePos, Line, iDraw, drawC, Ntime, viewEn);
	if (ret == RET_OVERSCREEN) { return RET_OVERSCREEN; }
	ret = PlayShowChainGuideLine(mapeff, Ymove, lanePos, Line, iDraw, drawC, Ntime, viewEn);
	if (ret == RET_OVERSCREEN) { return RET_OVERSCREEN; }
	ret = PlayShowInnerGuideLine(mapeff, Ymove, lanePos, Line, iDraw, drawC, Ntime, viewEn);
	if (ret == RET_OVERSCREEN) { return RET_OVERSCREEN; }
	return PlayShowMovingGuideLine(mapeff, Ymove, lanePos, Line, iDraw, drawC, Ntime, viewEn);
}

/* ラスライングループ */
static int PlayShowLastGuideLineGroup(rec_map_eff_data_t *mapeff, const cvec<rec_mapeff_move_st> &Ymove,
	const rec_play_lanepos_t *lanePos, int Line, int iDraw, int drawC, int Ntime, bool viewEn)
{
	int ret = 0;
	ret = PlayShowOuterGuideLine(mapeff, Ymove, lanePos, Line, iDraw, drawC, Ntime, viewEn);
	if (ret == RET_OVERSCREEN) { return RET_OVERSCREEN; }
	return PlayShowLastGuideLine(mapeff, Ymove, lanePos, Line, iDraw, drawC, Ntime, viewEn);
}

static int PlayShowGuideLine(rec_score_file_t *recfp, const rec_play_lanepos_t *lanePos, int Line, int iDraw) {
	int Ntime = recfp->time.now;
	cvec<rec_mapeff_move_st> &Ymove = recfp->mapeff.move.y[Line];
	rec_map_eff_data_t *mapeff = &recfp->mapeff;

	int drawLeft = 0;
	int drawRight = 0;
	int drawY1 = 0;
	int drawY2 = 0;
	int drawC = 0;
	rec_play_xy_set_t camera;
	RecPlayGetCameraPos(&camera.x, &camera.y);

	const bool viewEn = recfp->mapeff.viewLine.searchDataFront(Ymove[iDraw].Stime);

	// color code
	switch (Line) {
	case 0:
		drawC = COLOR_RED;
		break;
	case 1:
		drawC = COLOR_GREEN;
		break;
	case 2:
		drawC = COLOR_BLUE;
		break;
	}

	/* TODO: ラストだけ表示バグってる! */
	if (iDraw + 1 == Ymove.size()) {
		return PlayShowLastGuideLineGroup(mapeff, Ymove, lanePos, Line, iDraw, drawC, Ntime, viewEn);
	}
	else if (iDraw < 1) {
		return PlayShowStartGuideLine(mapeff, Ymove, lanePos, Line, drawC, Ntime, viewEn);
	}
	else {
		return PlayShowGuideLineStandard(mapeff, Ymove, lanePos, Line, iDraw, drawC, Ntime, viewEn);
	}
}

static void PlayShowAllGuideLine(rec_score_file_t *recfp, const rec_play_lanepos_t *lanePos,
	short LineMoveN[])
{
	int ret = 0;

	for (int iLine = 0; iLine < 3; iLine++) {
		for (int iDraw = LineMoveN[iLine]; 1; iDraw++) {
			ret = PlayShowGuideLine(recfp, lanePos, iLine, iDraw);
			if (ret == RET_OVERSCREEN) { break; }
			if (ret == RET_FINISH) { break; }
		}
	}
	return;
}

#undef RET_DRAW
#undef RET_DISABLE
#undef RET_OVERSCREEN

#endif /* Guide Line */

#if 1 /* control */

static void RecPlayGetKeyhold(rec_score_file_t *recfp, rec_play_key_hold_t *keyhold, int *holdG,
	bool AutoFlag)
{
	char key[256];

	GetHitKeyStateAll(key);
	if (AutoFlag) {
		if (key[KEY_INPUT_G] == 0) { *holdG = 0; }
		else if (key[KEY_INPUT_G] == 1) { (*holdG)++; }
		AutoAution(keyhold, recfp->mapdata.note, recfp->time.now);
	}
	else {
		if (key[KEY_INPUT_Z] == 0) keyhold->z = 0;
		else if (key[KEY_INPUT_Z] == 1) keyhold->z++;
		if (key[KEY_INPUT_X] == 0) keyhold->x = 0;
		else if (key[KEY_INPUT_X] == 1) keyhold->x++;
		if (key[KEY_INPUT_C] == 0) keyhold->c = 0;
		else if (key[KEY_INPUT_C] == 1) keyhold->c++;
		if (key[KEY_INPUT_UP] == 0) keyhold->up = 0;
		else if (key[KEY_INPUT_UP] == 1) keyhold->up++;
		if (key[KEY_INPUT_LEFT] == 0) keyhold->left = 0;
		else if (key[KEY_INPUT_LEFT] == 1) keyhold->left++;
		if (key[KEY_INPUT_RIGHT] == 0) keyhold->right = 0;
		else if (key[KEY_INPUT_RIGHT] == 1) keyhold->right++;
		if (key[KEY_INPUT_DOWN] == 0) keyhold->down = 0;
		else if (key[KEY_INPUT_DOWN] == 1) keyhold->down++;
	}
}

#endif

#if 1 /* class */

/* TODO: miss判定でもスコアスリップが発生する */
/* TODO: そもそもスコアフリップいる? */
static class rec_play_score_calculator_c {
private:
	void CalScore(rec_play_userpal_t *userpal, int notes)
	{
		rec_play_score_t *score = &userpal->score;
		const rec_play_judge_t *judge = &userpal->judgeCount;
		int MaxCombo = userpal->Mcombo;
		score->normal = (judge->just * 90000 + judge->good * 86667 + judge->safe * 45000) / notes;
		score->combo = MaxCombo * 10000 / notes;
		if (score->normal + score->combo - score->loss == 100000) {
			score->pjust = maxs_2(maxs_2(100 - (notes - judge->pjust), (judge->pjust * 100 / (double)notes) - 65), 0);
		}
		else {
			score->pjust = 0;
		}
		score->sum = score->normal + score->combo - score->loss + score->pjust;
		return;
	}

public:
	void RecPlayCalUserPal(rec_play_userpal_t *userpal, short notes, rec_play_time_set_t *time, int AutoFg) {
		userpal->Mcombo = maxs_2(userpal->Mcombo, userpal->Ncombo);
		//ライフが0未満の時、1毎に減点スコアを20増やす。
		if (userpal->life < 0) {
			userpal->score.loss = mins_2(userpal->score.loss - userpal->life * 20, userpal->score.normal + userpal->score.combo);
			userpal->life = 0;
		}
		//ライフ上限
		userpal->life = mins_2(userpal->life, 500);
		userpal->Exlife = mins_2(userpal->Exlife, 500);
		//スコア計算
		this->CalScore(userpal, notes);
		//ステータス
		if (userpal->life <= 0 && userpal->status == REC_PLAY_STATUS_PLAYING && AutoFg == 0) {
			userpal->status = REC_PLAY_STATUS_DROPED;
			userpal->Dscore.add_save = userpal->Dscore.add;
			userpal->Dscore.dis_save = maxs_2(time->now - time->offset, 0);
		}
		else if (userpal->status != REC_PLAY_STATUS_DROPED &&
			maxs_2(time->now - time->offset, 0) > time->end - time->offset)
		{ //CLEARED
			userpal->status = REC_PLAY_STATUS_CLEARED;
		}
		//距離計算
		switch (userpal->status) {
		case REC_PLAY_STATUS_PLAYING:
			userpal->Dscore.now_dis = maxs_2(time->now - time->offset, 0);
			userpal->Dscore.add_save = userpal->Dscore.add;
			break;
		case REC_PLAY_STATUS_CLEARED:
			userpal->Dscore.now_dis = time->end - time->offset;
			userpal->Dscore.add_save = userpal->Dscore.add;
			break;
		case REC_PLAY_STATUS_DROPED:
			userpal->Dscore.now_dis = userpal->Dscore.dis_save;
			break;
		}
		userpal->Dscore.point = (int)(userpal->Dscore.now_dis / 100.0 + userpal->Dscore.add_save);
		return;
	}
};

static class rec_play_drawnotes_c {
private:
	void CalPalCrawNote(int *DrawX, int *DrawY, int *DrawC, rec_play_lanepos_t *lanePos,
		bool XLockp, bool YLockp, const note_box_2_t *note, double speedt,
		const rec_scrool_data_t &scrool, int Ntime, int iLine)
	{
		//縦位置
		*DrawY = (YLockp ? note->ypos : lanePos->y[iLine]);
		//横位置
		*DrawX = (2 * speedt * (note->viewtime - scrool.speed * Ntime - scrool.basetime) / 5);
		*DrawX += (XLockp ? note->xpos : lanePos->x[iLine]);
		//色
		*DrawC = note->color;
	}

	void DrawNoteOne(const note_box_2_t *note, rec_map_eff_data_t *mapeff,
		rec_play_lanepos_t *lanePos, int iLine, int Ntime, rec_play_notepic_t *noteimg)
	{
		int DrawX = 0;
		int DrawY = 0;
		int DrawC = 0;
		int DrawID = 0;
		bool XLockp = mapeff->lock.x.searchDataFront(note->hittime);
		bool YLockp = mapeff->lock.y.searchDataFront(note->hittime);
		double Speedp = mapeff->speedt[iLine].searchDataFront(note->hittime);
		this->CalPalCrawNote(&DrawX, &DrawY, &DrawC, lanePos, XLockp, YLockp,
			note, Speedp, mapeff->scrool.nowData(), Ntime, iLine);
		switch (note->object) {
		case 1: /* TODO: enumにする */
		case 3:
		case 4:
		case 5:
		case 6:
			DrawGraphRecField(DrawX, DrawY, noteimg->notebase.handle());
			break;
		}
		switch (note->object) {
		case 1:
			DrawID = noteimg->hitcircle[DrawC].handle();
			break;
		case 2:
			DrawID = noteimg->catchi.handle();
			break;
		case 3:
			DrawID = noteimg->up.handle();
			break;
		case 4:
			DrawID = noteimg->down.handle();
			break;
		case 5:
			DrawID = noteimg->left.handle();
			break;
		case 6:
			DrawID = noteimg->right.handle();
			break;
		case 7:
			DrawID = noteimg->bomb.handle();
			break;
		case 8:
			DrawID = noteimg->goust.handle();
			break;
		}
		DrawGraphRecField(DrawX, DrawY, DrawID);
		return;
	}

public:
	void RecPlayDrawNoteAll(rec_play_lanepos_t *lanePos, cvec<note_box_2_t> note[],
		rec_map_eff_data_t *mapeff, int Ntime, rec_play_notepic_t *noteimg)
	{
		/* TODO: ゴーストノーツは全表示で良いかも */
		/* TODO: てかレーンアイテム作りたい */
		for (size_t i = mapeff->gnote.nowNo(); i < mapeff->gnote.size(); i++) {
			/* 3秒ブレーク */
			if (IS_BETWEEN(
				mapeff->viewT.searchDataFront(mapeff->gnote[i].hittime),
				3000, mapeff->gnote[i].hittime - Ntime
			)) {
				break;
			}
			/* 非表示スキップ */
			if (
				(mapeff->viewT.searchDataFront(mapeff->gnote[i].hittime) + Ntime) <=
				mapeff->gnote[i].hittime
			) {
				continue;
			}
			this->DrawNoteOne(&mapeff->gnote.at(i), mapeff, lanePos, mapeff->gnote.at(i).lane, Ntime, noteimg);
		}
		for (int iLine = 0; iLine < 3; iLine++) {
			for (int iNote = 0; note[iLine].offsetData(iNote).hittime > 0; iNote++) {
				/* 3秒ブレーク */
				if (IS_BETWEEN(
					mapeff->viewT.searchDataFront(note[iLine].offsetData(iNote).hittime),
					3000, note[iLine].offsetData(iNote).hittime - Ntime
				)) {
					break;
				}
				/* 非表示スキップ */
				if (
					(mapeff->viewT.searchDataFront(note[iLine].offsetData(iNote).hittime) + Ntime) <=
					note[iLine].offsetData(iNote).hittime
				) {
					continue;
				}
				this->DrawNoteOne(
					&note[iLine].offsetData(iNote), mapeff, lanePos, iLine, Ntime, noteimg
				);
				if (note[iLine].size() <= note[iLine].nowNo() + iNote + 1) { break; }
			}
		}
		return;
	}
};

static class rec_play_draw_back_c {
private:
	void DrawFallBack(int Yline, const std::vector<DxPic_t> &item, int itemNo) {
		static int baseX = 0;
		static int baseY = 0;
		for (int ix = 0; ix < 2; ix++) {
			for (int iy = 0; iy < 3; iy++) {
				RecRescaleDrawGraph(baseX + ix * 640, baseY + Yline - iy * 480,
					item[itemNo], TRUE);
			}
		}
		baseX -= 5;
		baseY += 2;
		if (baseX <= -640) { baseX += 640; }
		if (baseY >= 640) { baseY -= 480; }
	}

	/* (ret / 100) */
	void cal_back_x(int *xpos, rec_map_eff_data_t *mapeff, int cam) {
		const double scrool = mapeff->scrool.nowData().speed;
		double speed3 = mapeff->speedt[3].nowData();
		double speed4 = mapeff->speedt[4].nowData();

		xpos[0] -= (int)(100 * speed3 * scrool);
		while (xpos[0] + 100 * cam / 5 > 0)      { xpos[0] -= 64000; }
		while (xpos[0] + 100 * cam / 5 < -64000) { xpos[0] += 64000; }

		xpos[1] -= (int)(500 * speed4 * scrool);
		while (xpos[1] + 100 * cam > 0)      { xpos[1] -= 64000; }
		while (xpos[1] + 100 * cam < -64000) { xpos[1] += 64000; }

		xpos[2] = xpos[1];
		return;
	}

public:
	void PlayDrawBackGround(rec_map_eff_data_t *mapeff, int Yline[],
		rec_play_back_pic_t *backpic, const std::vector<DxPic_t> &item)
	{
		static int bgp[3] = { 0,0,0 };
		int camX = 0;
		RecPlayGetCameraPos(&camX, NULL);
		cal_back_x(bgp, mapeff, camX);
		//draw background picture
		for (int loop = bgp[0] / 100; loop + camX / 5 < 70000; loop += 640) {
			DrawGraphRecBackField(loop, Yline[3] / 5 - 160, backpic->sky.handle());
		}
		for (int loop = bgp[1] / 100; loop + camX < 70000; loop += 640) {
			DrawGraphRecField(loop, Yline[3] - 400, backpic->ground.handle());
			DrawGraphRecField(loop, Yline[4] - 400, backpic->water.handle());
		}
		//落ち物背景表示
		if (mapeff->fall.nowData() >= 0) {
			DrawFallBack(Yline[3], item, mapeff->fall.nowData());
		}
		return;
	}
};

static class rec_play_combo_c {
private:
	DxPic_t ComboFontimg[10];

public:
	rec_play_combo_c() {
		LoadDivGraph(L"picture/NumberComboBlue.png", 10, 5, 2, 80, 100, this->ComboFontimg);
	}

	~rec_play_combo_c() {
		for (int i = 0; i < 10; i++) {
			DeleteGraph(this->ComboFontimg[i]);
		}
	}

	void ViewCombo(int combo) {
		const int CHARA_WIDTH = 50;
		int keta = 0;
		int buf = 0;
		int DrawX = 0;

		if (combo < 10) { return; }

		for (int i = combo; i > 0; i /= 10) { keta++; }

		for (int i = keta - 1; i >= 0; i--) {
			buf = combo;
			for (int j = 0; j < i; j++) { buf /= 10; }
			buf %= 10;

			DrawX = ((keta - 1) * CHARA_WIDTH / 2 - i * CHARA_WIDTH) + 280;
			RecRescaleDrawGraph(DrawX, 150, this->ComboFontimg[buf], TRUE);
		}
	}
};

static class rec_play_runner_c {
#define DIV_X 6
#define DIV_Y 6
#define PIC_NUM (DIV_X * DIV_Y)
#define PIC_SIZE_X 160
#define PIC_SIZE_Y 160

#define CHARA_POS_UP 0
#define CHARA_POS_MID 1
#define CHARA_POS_DOWN 2

private:
	DxPic_t	charaimg[PIC_NUM];
	dxcur_pic_c charaguideimg = dxcur_pic_c(_T("picture/Cguide.png"));
	dxcur_pic_c judghimg      = dxcur_pic_c(_T("picture/Marker.png"));

public:
	int pos = 1; //キャラの今の位置[0で上,1で中,2で下]

public:
	rec_play_runner_c(void) {
		switch (optiondata.chara) {
		case 0:
			LoadDivGraph(L"picture/Picker.png",
				PIC_NUM, DIV_X, DIV_Y, PIC_SIZE_X, PIC_SIZE_Y, this->charaimg);
			break;
		case 1:
			LoadDivGraph(L"picture/Gator.png",
				PIC_NUM, DIV_X, DIV_Y, PIC_SIZE_X, PIC_SIZE_Y, this->charaimg);
			break;
		case 2:
			LoadDivGraph(L"picture/Taylor.png",
				PIC_NUM, DIV_X, DIV_Y, PIC_SIZE_X, PIC_SIZE_Y, this->charaimg);
			break;
		}
	}

	~rec_play_runner_c() {
		for (int i = 0; i < PIC_NUM; i++) {
			DeleteGraph(this->charaimg[i]);
		}
	}

private:
	void PlayDrawChara(rec_play_key_hold_t *key, rec_play_lanepos_t *lanePos, int charahit,
		int Ntime, rec_map_eff_data_t *mapeff)
	{
		static int hitpose = 0;
		int drawX = 0;
		int drawY = 0;
		int hitoffset = 0;
		int picID = 0;

		if (key->z == 1) { hitpose = (hitpose + 1) % 2; }
		if (key->x == 1) { hitpose = (hitpose + 1) % 2; }
		if (key->c == 1) { hitpose = (hitpose + 1) % 2; }
		if (GetNowCount() - charahit > 250) { hitoffset = 0; }
		else { hitoffset = pals(250, 0, 0, 50, GetNowCount() - charahit); }
		drawY = lanePos->y[this->pos] - 75;
		if (charahit > 0) {
			drawX = lanePos->x[this->pos] + hitoffset;
			picID = betweens(24 + hitpose * 6,
				(GetNowCount() - charahit) / 125 + 24 + hitpose * 6, 29 + hitpose * 6);
		}
		else {
			drawX = lanePos->x[this->pos];

			picID = (int)(Ntime * mapeff->v_BPM.nowData() / 20000) %
				6 + mapeff->chamo[this->pos].nowData() * 6;
		}
		if (mapeff->carrow.nowData()) {
			DrawGraphRecField(drawX - 160, drawY, this->charaimg[picID]);
		}
		else {
			DrawTurnGraphRecField(drawX + 30, drawY, this->charaimg[picID]);
		}
	}

public:
	void UpdateCharapos(int time, cvec<note_box_2_t> note[],
		rec_play_key_hold_t *keyhold, rec_play_chara_hit_attack_t *hitatk)
	{
		int ans = CHARA_POS_MID;
		// push up
		if (1 <= keyhold->up && 0 == keyhold->down) {
			this->pos = CHARA_POS_UP;
			return;
		}
		// push down
		if (0 == keyhold->up && 1 <= keyhold->down) {
			this->pos = CHARA_POS_DOWN;
			return;
		}
		// push up and down
		if (1 <= keyhold->up && 1 <= keyhold->down) {
			this->pos = CHARA_POS_MID;
			return;
		}
		// near catch/bomb
		for (int i = 0; i < 3; i++) {
			if (note[i].nowData().hittime <= time + 40 &&
				(note[i].nowData().object == NOTE_CATCH ||
					note[i].nowData().object == NOTE_BOMB))
			{
				this->pos = CHARA_POS_MID;
				return;
			}
		}
		// hit note
		if (keyhold->up != 1 && keyhold->down != 1 &&
			keyhold->left != 1 && keyhold->right != 1 && hitatk->time != -1000)
		{
			this->pos = hitatk->pos;
			return;
		}
		this->pos = CHARA_POS_MID;
		return;
	}

	void ViewRunner(rec_map_eff_data_t *mapeff, rec_play_key_hold_t *keyhold,
		rec_play_lanepos_t *lanePos, int charahit, int Ntime)
	{
		// view chara pos guide
		if (mapeff->carrow.nowData()) {
			DrawGraphRecField(lanePos->x[this->pos] - 4, lanePos->y[this->pos] - 4, this->charaguideimg.handle());
		}
		else {
			DrawTurnGraphRecField(lanePos->x[this->pos] - 56, lanePos->y[this->pos] - 4, this->charaguideimg.handle());
		}
		//判定マーカーの表示
		for (int i = 0; i < 3; i++) {
			DrawGraphRecField(lanePos->x[i], lanePos->y[i], this->judghimg.handle());
		}
		/* キャラ表示 */
		this->PlayDrawChara(keyhold, lanePos, charahit, Ntime, mapeff);
	}

#undef DIV_X
#undef DIV_Y
#undef PIC_NUM
#undef PIC_SIZE_X
#undef PIC_SIZE_Y
};

static class rec_play_sbar_c {
private:
	dxcur_pic_c baseImg = dxcur_pic_c(_T("picture/scoreber.png"));
	struct {
		dxcur_pic_c Green = dxcur_pic_c(_T("picture/LIFEbar.png"));
		dxcur_pic_c Yellow = dxcur_pic_c(_T("picture/LIFEbar2.png"));
		dxcur_pic_c Red = dxcur_pic_c(_T("picture/LIFEbar3.png"));
	} Lbarimg;
	dxcur_pic_c Tbarimg[2] = {
		dxcur_pic_c(_T("picture/TIMEbar.png")), dxcur_pic_c(_T("picture/TIMEbar2.png"))
	};
	dxcur_pic_c sbbarimg = dxcur_pic_c(_T("picture/scoreber2.png"));

private:
	int CalPosScore(rec_play_score_t score, int RemainNotes, int Notes, int combo, int MaxCombo) {
		int PosCombo = maxs_2(combo + RemainNotes, MaxCombo);
		return score.normal + 90000 * RemainNotes / Notes + 10000 * PosCombo / Notes;
	}

	void RunningStats(rec_play_judge_t judge, int PosScore, int HighScore) {
		int x1 = 6;
		int y1 = 6;
		int x2 = 188;
		int y2 = 6;
		int x3 = 150;
		int y3 = 69;
		int x4 = 6;
		int y4 = 69;
		DxColor_t CrG = GetColor(63, 63, 63);
		DxColor_t CrD = GetColor(255, 63, 127);
		DxColor_t CrY = GetColor(255, 255, 0);
		DxColor_t CrC = GetColor(0, 255, 255);

		if (judge.miss > 0) {
			RecRescaleDrawTriangle(x1, y1, x2, y2, x3, y3, CrG, TRUE);
		}
		else if (judge.safe > 0) {
			RecRescaleDrawTriangle(x1, y1, x2, y2, x3, y3, CrD, TRUE);
		}
		else if (judge.good > 0) {
			RecRescaleDrawTriangle(x1, y1, x2, y2, x3, y3, CrY, TRUE);
		}
		else {
			RecRescaleDrawTriangle(x1, y1, x2, y2, x3, y3, CrC, TRUE);
		}
		if (PosScore < HighScore) {
			RecRescaleDrawTriangle(x1, y1, x3, y3, x4, y4, CrG, TRUE);
		}
		else {
			DxColor_t Cr = GetColor(lins(HighScore, 255, 100000, 0, PosScore), 255, lins(HighScore, 0, 100000, 255, PosScore));
			RecRescaleDrawTriangle(x1, y1, x3, y3, x4, y4, Cr, TRUE);
		}
	}

	void ViewScore(rec_play_score_t score, int Hscore, int time) {
		DxColor_t Cr = GetColor(255, 255, 255);
		int s_score = score.sum;

		if (time - score.time < 500) {
			s_score = pals(500, score.sum, 0, score.before, time - score.time);
		}
		if (Hscore <= s_score) {
			Cr = GetColor(255, 255, 0);
		}
		RecRescaleDrawFormatString(490, 20, Cr, L"SCORE:%d", s_score);
	}

	void ViewLife(int life, int Exlife) const {
		int DrawX = lins(0, -114, 500, 177, life);
		if (100 < life) {
			RecRescaleDrawGraph(DrawX, 3, this->Lbarimg.Green.handle(), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(100, 255, 500, 0, life));
			RecRescaleDrawGraph(DrawX, 3, this->Lbarimg.Yellow.handle(), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
		else {
			RecRescaleDrawGraph(DrawX, 3, this->Lbarimg.Red.handle(), TRUE);
		}
		DrawX = lins(0, -114, 500, 177, Exlife);
		RecRescaleDrawGraph(DrawX, 3, this->Lbarimg.Red.handle(), TRUE);
		/* TODO: DrawBoxで表現する */
		// DrawBoxAnchor(24, 4, 572, 52, COLOR_GREEN, DXDRAW_ANCHOR_TOP_CENTRE, TRUE);
		RecRescaleDrawFormatString(440, 10, COLOR_WHITE, L"%3d", life);
	}

	void ViewDist(rec_play_status_t status, const distance_score_t *Dscore,
		const rec_play_time_set_t *time, const rec_ddif_time_t *mdiftime, bool holdG)
	{
		int DrawX = 0;
		int DrawNo = 0;
		DxColor_t cr = COLOR_WHITE;

		if (status == REC_PLAY_STATUS_CLEARED) {
			DrawNo = 1;
			cr = COLOR_BLACK;
		}
		DrawX = (291 * Dscore->now_dis - 136 * time->end + 136 * time->offset) / (time->end - time->offset);
		RecRescaleDrawGraph(DrawX, 38, this->Tbarimg[DrawNo].handle(), TRUE);
		RecRescaleDrawFormatString(180, 45, cr, L"%.3fkm", Dscore->point / 1000.0);
		if (holdG) {
			DrawX = lins(time->offset, 175, time->end, 466, mdiftime->notes);
			RecRescaleDrawLine(DrawX, 38, DrawX - 19, 72, GetColor(  0,   0, 255));
			DrawX = lins(time->offset, 175, time->end, 466, mdiftime->arrow);
			RecRescaleDrawLine(DrawX, 38, DrawX - 19, 72, GetColor(255,   0,   0));
			DrawX = lins(time->offset, 175, time->end, 466, mdiftime->chord);
			RecRescaleDrawLine(DrawX, 38, DrawX - 19, 72, GetColor(  0, 255,   0));
			DrawX = lins(time->offset, 175, time->end, 466, mdiftime->chain);
			RecRescaleDrawLine(DrawX, 38, DrawX - 19, 72, GetColor(127,   0, 255));
			DrawX = lins(time->offset, 175, time->end, 466, mdiftime->trill);
			RecRescaleDrawLine(DrawX, 38, DrawX - 19, 72, GetColor(255, 255,   0));
			DrawX = lins(time->offset, 175, time->end, 466, mdiftime->meldy);
			RecRescaleDrawLine(DrawX, 38, DrawX - 19, 72, GetColor(255, 127, 255));
			DrawX = lins(time->offset, 175, time->end, 466, mdiftime->actor);
			RecRescaleDrawLine(DrawX, 38, DrawX - 19, 72, GetColor(255, 127,   0));
			DrawX = lins(time->offset, 175, time->end, 466, mdiftime->trick);
			RecRescaleDrawLine(DrawX, 38, DrawX - 19, 72, GetColor(127,   0,   0));
		}
	}

	void ViewRunStatus(const rec_play_userpal_t *userpal, int notes, int HighScore) {
		int RemainNotes = GetRemainNotes(userpal->judgeCount, notes);
		int PosScore = this->CalPosScore(userpal->score, RemainNotes, notes, userpal->Ncombo, userpal->Mcombo);
		this->RunningStats(userpal->judgeCount, PosScore, HighScore);
	}

	void ViewDdif(const int ddif[], int ddifG1) {
		int DrawX1 = 0;
		int DrawY1 = 0;
		int DrawX2 = 0;
		int DrawY2 = 0;
		int leftLimit = 0;
		int rightLimit = 0;
		DxColor_t Cr = COLOR_WHITE;

		leftLimit = lins(0, 155, ddifG1, 175, ddif[0]);
		rightLimit = lins(0, 447, ddifG1, 467, ddif[24]);

		for (int iBlock = 0; iBlock <= 23; iBlock++) {
			DrawX1 = lins(0, leftLimit, 24, rightLimit, iBlock);
			DrawX2 = lins(0, leftLimit, 24, rightLimit, iBlock + 1);
			DrawY1 = lins(0, 72, ddifG1, 38, iBlock);
			DrawY2 = lins(0, 72, ddifG1, 38, iBlock + 1);
			RecRescaleDrawLine(DrawX1, DrawY1, DrawX2, DrawY2, Cr);
		}
	}

public:
	void ViewScoreBar(const rec_play_userpal_t *userpal, const rec_play_time_set_t *time,
		const rec_map_detail_t *mapdata, int Hscore, bool holdG)
	{
		RecRescaleDrawGraph(0, 0, this->baseImg.handle(), TRUE);
		this->ViewScore(userpal->score, Hscore, time->now);
		this->ViewLife(userpal->life, userpal->Exlife);
		this->ViewDist(userpal->status, &userpal->Dscore, time, &mapdata->mpal.time, holdG);
		RecRescaleDrawGraph(0, 0, this->sbbarimg.handle(), TRUE);
		this->ViewRunStatus(userpal, mapdata->notes, Hscore);
	}
};

class rec_play_gapbar_c {
private:
	dxcur_pic_c gapbarimg  = dxcur_pic_c(L"picture/GapBer.png");
	dxcur_pic_c gaplineimg = dxcur_pic_c(L"picture/GapBerLine.png");

public:
	void ViewGapBar(const cur_deviation_c *gap) const {
		int No = gap->GetHead() % 30;

		RecRescaleDrawGraph(219, 460, this->gapbarimg.handle(), TRUE);

		for (int i = 0; i < 30; i++) {
			No = LOOP_SUB(No, 30);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(0, 255, 29, 0, i));
			/* TODO: オプションで、時間で上下にずらすのもあり? */
			/* TODO: オプションで、左右反転 */
			RecRescaleDrawGraph(318 - gap->GetList(No), 460, this->gaplineimg.handle(), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 225);
		}
	}
};

class rec_play_keyview_c {
private:
	int enable = optiondata.keydetail;
	int count[7] = { 0,0,0,0,0,0,0 };
	dxcur_pic_c KeyViewimg[2];

public:
	rec_play_keyview_c(void) {
		if (this->enable) {
			this->KeyViewimg[0].reload(_T("picture/KeyViewOff.png"));
			this->KeyViewimg[1].reload(_T("picture/KeyViewOn.png"));
		}
	}

	void ViewKeyview(rec_play_key_hold_t *keyhold) {
		if (this->enable) {
			if (keyhold->z == 1) { this->count[0]++; }
			if (keyhold->x == 1) { this->count[1]++; }
			if (keyhold->c == 1) { this->count[2]++; }
			if (keyhold->up == 1) { this->count[3]++; }
			if (keyhold->down == 1) { this->count[4]++; }
			if (keyhold->left == 1) { this->count[5]++; }
			if (keyhold->right == 1) { this->count[6]++; }
			RecRescaleDrawGraph(5, 445, this->KeyViewimg[mins_2(keyhold->z, 1)].handle(), REC_RESCALE_BOTTOM_LEFT);
			RecRescaleDrawGraph(40, 445, this->KeyViewimg[mins_2(keyhold->x, 1)].handle(), REC_RESCALE_BOTTOM_LEFT);
			RecRescaleDrawGraph(75, 445, this->KeyViewimg[mins_2(keyhold->c, 1)].handle(), REC_RESCALE_BOTTOM_LEFT);
			RecRescaleDrawGraph(570, 410, this->KeyViewimg[mins_2(keyhold->up, 1)].handle(), REC_RESCALE_BOTTOM_RIGHT);
			RecRescaleDrawGraph(570, 445, this->KeyViewimg[mins_2(keyhold->down, 1)].handle(), REC_RESCALE_BOTTOM_RIGHT);
			RecRescaleDrawGraph(535, 445, this->KeyViewimg[mins_2(keyhold->left, 1)].handle(), REC_RESCALE_BOTTOM_RIGHT);
			RecRescaleDrawGraph(605, 445, this->KeyViewimg[mins_2(keyhold->right, 1)].handle(), REC_RESCALE_BOTTOM_RIGHT);
			if (this->count[0] == 0) { RecRescaleAnchorDrawString(10, 450, L"Z", COLOR_WHITE, REC_RESCALE_BOTTOM_LEFT); }
			else { RecRescaleAnchorDrawFormatString(10, 450, COLOR_WHITE, REC_RESCALE_BOTTOM_LEFT, L"%2d", this->count[0] % 100); }
			if (this->count[1] == 0) { RecRescaleAnchorDrawString(45, 450, L"X", COLOR_WHITE, REC_RESCALE_BOTTOM_LEFT); }
			else { RecRescaleAnchorDrawFormatString(45, 450, COLOR_WHITE, REC_RESCALE_BOTTOM_LEFT, L"%2d", this->count[1] % 100); }
			if (this->count[2] == 0) { RecRescaleAnchorDrawString(80, 450, L"C", COLOR_WHITE, REC_RESCALE_BOTTOM_LEFT); }
			else { RecRescaleAnchorDrawFormatString(80, 450, COLOR_WHITE, REC_RESCALE_BOTTOM_LEFT, L"%2d", this->count[2] % 100); }
			if (this->count[3] == 0) { RecRescaleAnchorDrawString(575, 415, L"↑", COLOR_WHITE, REC_RESCALE_BOTTOM_RIGHT); }
			else { RecRescaleAnchorDrawFormatString(575, 415, COLOR_WHITE, REC_RESCALE_BOTTOM_RIGHT, L"%2d", this->count[3] % 100); }
			if (this->count[4] == 0) { RecRescaleAnchorDrawString(575, 450, L"↓", COLOR_WHITE, REC_RESCALE_BOTTOM_RIGHT); }
			else { RecRescaleAnchorDrawFormatString(575, 450, COLOR_WHITE, REC_RESCALE_BOTTOM_RIGHT, L"%2d", this->count[4] % 100); }
			if (this->count[5] == 0) { RecRescaleAnchorDrawString(540, 450, L"←", COLOR_WHITE, REC_RESCALE_BOTTOM_RIGHT); }
			else { RecRescaleAnchorDrawFormatString(540, 450, COLOR_WHITE, REC_RESCALE_BOTTOM_RIGHT, L"%2d", this->count[5] % 100); }
			if (this->count[6] == 0) { RecRescaleAnchorDrawString(610, 450, L"→", COLOR_WHITE, REC_RESCALE_BOTTOM_RIGHT); }
			else { RecRescaleAnchorDrawFormatString(610, 450, COLOR_WHITE, REC_RESCALE_BOTTOM_RIGHT, L"%2d", this->count[6] % 100); }
		}
	}
};

class rec_play_item_c {
	std::vector<DxPic_t> vec;

public:
	rec_play_item_c(void) = delete;

	rec_play_item_c(const tstring &folderPath) {
		DxPic_t handbuf;
		tstring strbuf;
		for (size_t i = 0; i < 1000; i++) {
			strbuf = folderPath;
			Getxxxpng(strbuf, i);
			handbuf = LoadGraph(strbuf.c_str());
			if (handbuf == -1) { break; }
			this->vec.push_back(handbuf);
		}
	}

	~rec_play_item_c() {
		for (size_t i = 0; i < this->vec.size(); i++) {
			DeleteGraph(this->vec[i]);
		}
	}

	const std::vector<DxPic_t> &GetItemList(void) const {
		return vec;
	}
};

class rec_play_sitem_c {
	std::vector<DxSnd_t> vec;

public:
	rec_play_sitem_c(void) = delete;

	rec_play_sitem_c(const tstring &folderPath) {
		DxSnd_t handbuf;
		tstring strbuf;
		for (size_t i = 0; i < 999; i++) {
			strbuf = folderPath;
			Getxxxwav(strbuf, i + 1); /* 0番は未使用 */
			handbuf = LoadSoundMem(strbuf.c_str());
			if (handbuf == -1) { break; }
			this->vec.push_back(handbuf);
		}
	}

	~rec_play_sitem_c() {
		for (size_t i = 0; i < this->vec.size(); i++) {
			DeleteSoundMem(this->vec[i]);
		}
	}

	const std::vector<DxSnd_t> &GetSitemList(void) const {
		return vec;
	}
};

#endif /* class */

/* main action */

/**
 * @param[out] ret_map_det map_detail の受け皿
 * @param[out] ret_userpal userpal の受け皿
 * @param[out] ret_nameset nameset の受け皿
 * @param[in] folderPath フォルダーパス
 * @param[in] rrsPath 譜面パス
 * @param[in] HighScore ハイスコア
 * @param[in] AutoFlag オートプレイフラグ
 * @return now_scene_t 次のシーン
 */
now_scene_t RecPlayMain(rec_map_detail_t *ret_map_det, rec_play_userpal_t *ret_userpal,
	rec_play_nameset_t *ret_nameset, const TCHAR *folderPath, const TCHAR *rrsPath, int HighScore,
	int AutoFlag)
{

#if 1 /* num define */

	/* short */
	short i[3];

	/* int */
	int charahit = 0; //キャラがノーツをたたいた後であるかどうか。[1以上で叩いた、0で叩いてない]
	int G[20];
	int holdG = 0;
	int AllNotesHitTime = -1;
	int LaneTrack[3] = { -150,-150,-150 };
	int StopFrag = -1;
	rec_play_chara_hit_attack_t hitatk;
	int fps[62];//0～59=1フレーム間隔の時間,60=次の代入先,61=前回の時間
	short LineMoveN[3] = { 0,0,0 }; //↑のライン表示番号
	rec_play_lanepos_t lanePos;
	unsigned int Cr = GetColor(255, 255, 255);
	unsigned int Crb = GetColor(0, 0, 0);
	unsigned int CrR = GetColor(255, 0, 0);
	int Stime = 0;
	int adif = 0;
	int max_adif = 0;

	/* struct */
	rec_play_key_hold_t keyhold;
	rec_system_t system;
	rec_score_file_t recfp;
	rec_play_userpal_t userpal;

	/* double */
	double GD[5];
	double SumRate[2] = { 0,0 };
	double DifRate; //譜面定数

	/* class */
	rec_play_sbar_c sbarClass;
	rec_play_combo_c comboPicClass;
	rec_play_gapbar_c gapbarClass;
	rec_play_sound_c p_sound;
	rec_play_runner_c runnerClass;
	rec_play_keyview_c keyviewClass;
	rec_play_bonus_c bonusClass;
	rec_play_item_c itemClass(folderPath);
	rec_play_sitem_c sitemClass(folderPath);
	rec_cutin_c cutin;

	/* mat */
	rec_play_back_pic_t backpic;
	dxcur_pic_c dangerimg = dxcur_pic_c(_T("picture/danger.png"));
	dxcur_pic_c dropimg   = dxcur_pic_c(_T("picture/drop.png"));
	dxcur_pic_c filterimg = dxcur_pic_c(_T("picture/Black.png"));
	rec_play_notepic_t noteimg;

#endif /* num define */

	/* ピクチャの用意 */
	ReadyEffPicture();
	ReadyJudgePicture();
	/* action */
	for (i[0] = 0; i[0] <= 59; i[0]++) { fps[i[0]] = 17; }
	fps[60] = 0;
	fps[61] = 0;

	if (optiondata.SEenable == 0) { RecPlayInitMelodySnd(); }

	if (rec_score_fread(&recfp, rrsPath) != 0) { return SCENE_EXIT; } /* TODO: EXITにしてるのは、どうなの? */

	backpic.sky.reload(   recfp.nameset.sky.c_str());
	backpic.ground.reload(recfp.nameset.ground.c_str());
	backpic.water.reload( recfp.nameset.water.c_str());

	//ゲーム開始前の下準備
	recfp.mapdata.notes = notzero(recfp.mapdata.notes);
	GD[0] = recfp.mapdata.mdif / 100.0 - recfp.mapdata.Lv;//mdifと難易度表記の差
	if (recfp.mapdata.Lv == 0) { DifRate = 0; }
	else if (2 <= GD[0]) { DifRate = recfp.mapdata.Lv + 0.9; }
	else if (0 <= GD[0] && GD[0] < 2) { DifRate = recfp.mapdata.Lv + 0.45 * GD[0]; }
	else { DifRate = recfp.mapdata.mdif / 100.0; }
	RecResetPlayObjectNum(&recfp);
	rec_bgm_system_g.SetMem(recfp.nameset.mp3FN);
	rec_bgm_system_g.Play(true, false, true);
	WaitTimer(WAIT_TIME_AFTER_MUSICPLAY);
	Stime = GetNowCount();
	cutin.SetIo(CUT_FRAG_OUT);
	//ゲーム開始
	//終了判定->キー入力->計算->描画->ウェイトの順で処理する
	while (1) {
		//終了判定
		//カットイン再生から2秒以上たったら抜ける。
		if (cutin.IsEndAnim()) {
			rec_bgm_system_g.Stop();
			break;
		}

		//キー入力
		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }
		RecPlayStepAllNum(&recfp, LineMoveN); /* RecPlayGetKeyhold()よりも先に実行しなければならない */
		RecPlayGetKeyhold(&recfp, &keyhold, &holdG, AutoFlag);
		RecPlayActSubKey(&recfp, AutoFlag, &StopFrag, LineMoveN);
		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			rec_bgm_system_g.Stop();
			return SCENE_SERECT;
		}

		//計算
		//カメラ移動
		RecPlaySetCamera(recfp.mapeff.camera, recfp.time.now);
		//lanePos.x(横位置)の計算
		recSetLine(lanePos.x, recfp.mapeff.move.x, recfp.time.now, 3);
		//lanePos.y(縦位置)の計算
		if (optiondata.backbright == 0) {
			recSetLine(lanePos.y, recfp.mapeff.move.y, recfp.time.now, 3);
		}
		else {
			recSetLine(lanePos.y, recfp.mapeff.move.y, recfp.time.now, 5);
		}
		runnerClass.UpdateCharapos(recfp.time.now, recfp.mapdata.note, &keyhold, &hitatk);
		if ((GetNowCount() - charahit > 50) &&
			IS_JUST_PUSH_ANY_ARROWKEY(&keyhold))
		{
			charahit = 0;
		}
		//キー押しヒット解除
		if (IS_JUST_PUSH_ANY_ARROWKEY(&keyhold) || ((hitatk.time + 750) < recfp.time.now)) {
			hitatk.time = -1000;
		}
		//キャッチ判定に使う数値を計算
		RecPlayCalLaneTrack(LaneTrack, &keyhold, runnerClass.pos, recfp.time.now);
		//ヒット
		if (IS_JUST_PUSH_ANY_HITKEY(&keyhold)) { charahit = GetNowCount(); }
		if (charahit + 750 < GetNowCount()) { charahit = 0; }
		/* ノーツ判定 */
		RecJudgeAllNotes(recfp.mapdata.note, recfp.time.now, sitemClass.GetSitemList(),
			&keyhold, &hitatk, LaneTrack, &charahit, runnerClass.pos, &userpal, &p_sound);
		{
			rec_play_score_calculator_c action;
			action.RecPlayCalUserPal(&userpal, recfp.mapdata.notes, &recfp.time, AutoFlag);
		}
		//ノーツが全部なくなった瞬間の時間を記録
		if (GetRemainNotes(userpal.judgeCount, recfp.mapdata.notes) == 0 && AllNotesHitTime < 0) {
			AllNotesHitTime = GetNowCount();
		}
		//時間計算
		if (StopFrag == -1) {
			recfp.time.now = GetNowCount() - Stime + optiondata.offset * 5;
		}
		else {
			Stime = GetNowCount() - recfp.time.now + optiondata.offset * 5;
		}

		bonusClass.update();
		//オートでなく、ノーミス以上を出したら演出
		if (AutoFlag == 0 && AllNotesHitTime + 2000 > GetNowCount()) {
			AllNotesHitTime = 0;
			bonusClass.setBonus(userpal.judgeCount); /* TODO: もっと内部で呼びたい */
		}

		//描画
		ClearDrawScreen(); /* 描画エリアここから */
		//背景表示
		if (optiondata.backbright != 0) {
			rec_play_draw_back_c action;
			action.PlayDrawBackGround(&recfp.mapeff, lanePos.y, &backpic, itemClass.GetItemList());
		}
		//フィルター表示
		switch (optiondata.backbright) {
		case 1:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 170);
			RecRescaleDrawGraph(0, 0, filterimg.handle(), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			break;
		case 2:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 85);
			RecRescaleDrawGraph(0, 0, filterimg.handle(), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			break;
		}
		//アイテム表示
		if (optiondata.backbright != 0) {
			PlayDrawItem(&recfp.mapeff, recfp.time.now, lanePos.x[1], itemClass.GetItemList());
		}
		// view line
		PlayShowAllGuideLine(&recfp, &lanePos, LineMoveN);
		RecPlayDrawGuideBorder(&recfp, &lanePos);
		/* キャラ周り表示 */
		runnerClass.ViewRunner(&recfp.mapeff, &keyhold, &lanePos, charahit, recfp.time.now);
		//コンボ表示
		comboPicClass.ViewCombo(userpal.Ncombo);
		//判定表示
		PlayShowJudge(&lanePos, runnerClass.pos);
		/* 音符表示 */
		{
			rec_play_drawnotes_c action;
			action.RecPlayDrawNoteAll(&lanePos, recfp.mapdata.note, &recfp.mapeff,
				recfp.time.now, &noteimg);
		}
		//ヒットエフェクト表示
		PlayShowHitEffect(&lanePos);
		PlayCheckHitEffect();
		//スコアバー表示
		sbarClass.ViewScoreBar(&userpal, &recfp.time, &recfp.mapdata, HighScore, holdG);
		//判定ずれバー表示
		gapbarClass.ViewGapBar(&userpal.gap);
		//キー押し状況表示(オプション)
		keyviewClass.ViewKeyview(&keyhold);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		//デバッグ表示
		if (holdG >= 1) {
			RecRescaleDrawFormatString(490, 80, Cr, L"mdif:%.2f", recfp.mapdata.mpal.mdif / 100.0);
			RacPlayDrawFieldGrid();
#if 0
			/* エラー表示 */
			if (recfp.outpoint[1] != 0) {
				RecRescaleDrawFormatString(20, 120, CrR, L"MAPERROR");
				RecRescaleDrawLine(lins(recfp.time.offset, 155, recfp.time.end, 446, recfp.outpoint[0]), 71,
					lins(recfp.time.offset, 175, recfp.time.end, 465, recfp.outpoint[0]), 38, CrR);
			}
#endif
		}
		if (AutoFlag == 1) {
			fps[fps[60]++] = recfp.time.now - fps[61];
			if (fps[60] > 59)fps[60] -= 60;
			fps[61] = recfp.time.now;
			G[0] = 0;
			for (i[0] = 0; i[0] <= 59; i[0]++)G[0] += fps[i[0]];
			RecRescaleDrawFormatString(20, 80, Cr, L"FPS: %.1f", DIV_AVOID_ZERO((double)60000, (double)G[0], (double)0));
			RecRescaleDrawFormatString(20, 100, Cr, L"Autoplay");
		}
		RECR_DEBUG(0, RecPlayDebug[0]);
		RECR_DEBUG(1, RecPlayDebug[1]);
		RECR_DEBUG(2, RecPlayDebug[2]);

		//データオーバーフローで警告文表示
#if 0
		if (0 <= recfp.mapdata.note2.up[1999].hittime) {
			RecRescaleDrawFormatString(20, 120, CrR, L"UPPER OVER");
		}
		else if (0 <= recfp.mapdata.note2.mid[1999].hittime) {
			RecRescaleDrawFormatString(20, 120, CrR, L"MIDDLE OVER");
		}
		else if (0 <= recfp.mapdata.note2.low[1999].hittime) {
			RecRescaleDrawFormatString(20, 120, CrR, L"LOWER OVER");
		}
#endif
		if (userpal.status == REC_PLAY_STATUS_DROPED) { RecRescaleDrawGraph(0, 0, dropimg.handle(), TRUE); }
		else if (userpal.life <= 100) { RecRescaleDrawGraph(0, 0, dangerimg.handle(), TRUE); }
		bonusClass.draw();
		//終了時間から5秒以上たって、曲が終了したらカットイン再生。
		if ((cutin.IsClosing() == 0) &&
			(recfp.time.end + 5000 <= recfp.time.now) &&
			(rec_bgm_system_g.RecCheckSoundMem() == 0))
		{
			cutin.SetCutTipFg(CUTIN_TIPS_NONE);
			cutin.SetIo(CUT_FRAG_IN);
		}
		cutin.DrawCut();
		ScreenFlip(); /* 描画エリアここまで */

		//ウェイト
		WaitTimer(WAIT_TIME_ON_GAMELOOP);
	}

	*ret_map_det = recfp.mapdata;
	*ret_userpal = userpal;
	*ret_nameset = recfp.nameset;

	return AutoFlag == 1 ? SCENE_SERECT : SCENE_RESULT;
}

/**
* @param[in] packNo パックナンバー
* @param[in] musicNo 曲ナンバー
* @param[in] difNo 難易度ナンバー
* @param[in] shift マップ生成フラフ
* @param[in] AutoFlag オートプレイフラグ
* @return 次のシーン
*/
now_scene_t play3(int packNo, int musicNo, int difNo, int shift, int AutoFlag) {
	rec_error_t status = REC_ERROR_NONE;
	int HighScore = 0;
	TCHAR mapPath[255];
	TCHAR fileName[255];
	TCHAR folderPath[255];
	rec_map_detail_t map_detail;
	rec_play_userpal_t userpal;
	rec_play_nameset_t nameset;
	now_scene_t ret = SCENE_EXIT;
	FILE *fp = NULL;

	status = RecGetMusicMapRrsPath(mapPath, 255, packNo, musicNo, (rec_dif_t)difNo);
	if (status != REC_ERROR_NONE) { return SCENE_SERECT; }

	/* rrsデータが無い、または作成の指示があれば作る */
	if (shift == 0) { _wfopen_s(&fp, mapPath, L"rb"); } /* TODO: IsExist()関数とかあっていいかも */

	if (fp == NULL) {
		RecordLoad2(packNo, musicNo, difNo);
		// cal_ddif_3(mapPath); /* TODO: 今バグってる。どうせseekの問題 */
	}
	else { fclose(fp); }

	RecGetMusicFolderName(fileName, 255, packNo, musicNo);
	HighScore = GetHighScore(fileName, (rec_dif_t)difNo);
	RecGetMusicFolderPath(folderPath, 255, packNo, musicNo);
	ret = RecPlayMain(&map_detail, &userpal, &nameset, folderPath, mapPath, HighScore, AutoFlag);

	if (ret != SCENE_RESULT) { return ret; }
	else { return result(&map_detail, &userpal, &nameset, (rec_dif_t)difNo, fileName); }
}
