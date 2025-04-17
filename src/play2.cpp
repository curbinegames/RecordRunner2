
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
#include <option.h>
#include <RecSystem.h>
#include <recr_cutin.h>
#include <RecSave.h>
#include <RecScoreFile.h>
#include <RecSystem.h>
#include <RecWindowRescale.h>

/* rec play include */
#include <playbox.h>
#include <PlayBonus.h>
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

#define MODE_CHANGE 1

#define CHARA_POS_UP 0
#define CHARA_POS_MID 1
#define CHARA_POS_DOWN 2

#endif /* define group */

#if 1 /* typedef group */

/* struct */
typedef struct rec_play_back_pic_s {
	dxcur_pic_c sky;
	dxcur_pic_c ground;
	dxcur_pic_c water;
} rec_play_back_pic_t;

typedef struct play_key_stat_s {
	char z = 0;
	char x = 0;
	char c = 0;
	char up = 0;
	char down = 0;
	char left = 0;
	char right = 0;
} play_key_stat_t;

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

#if 1 /* sub action */

static void RecResetPlayPartNum(short *itemN, short *SitemN, short LineMoveN[], short lockN[],
	short *viewTN, short *MovieN)
{
	*itemN = 0;
	*SitemN = 0;
	LineMoveN[0] = 0;
	LineMoveN[1] = 0;
	LineMoveN[2] = 0;
	lockN[0] = 0;
	lockN[1] = 0;
	*viewTN = 0;
	*MovieN = 0;
	return;
}

static void RecResetPlayRecfpMapeffNum(rec_map_eff_data_t *mapeff) {
	mapeff->camera.num = 0;
	mapeff->scrool.num = 0;
	mapeff->carrow.num = 0;
	mapeff->move.y[0].num = 0;
	mapeff->move.y[1].num = 0;
	mapeff->move.y[2].num = 0;
	mapeff->move.y[3].num = 0;
	mapeff->move.y[4].num = 0;
	mapeff->move.x[0].num = 0;
	mapeff->move.x[1].num = 0;
	mapeff->move.x[2].num = 0;
	mapeff->chamo[0].num = 0;
	mapeff->chamo[1].num = 0;
	mapeff->chamo[2].num = 0;
	mapeff->fall.num = 0;
	return;
}

static void RecResetPlayObjectNum(short objectN[], const rec_score_file_t *recfp) {
	uint allnum = recfp->allnum.notenum[0] + recfp->allnum.notenum[1] + recfp->allnum.notenum[2];
	for (uint iView = 0; iView < allnum; iView++) {
		if (recfp->mapdata.note[iView].lane == NOTE_LANE_UP) {
			objectN[0] = iView;
			break;
		}
	}
	for (uint iView = 0; iView < allnum; iView++) {
		if (recfp->mapdata.note[iView].lane == NOTE_LANE_MID) {
			objectN[1] = iView;
			break;
		}
	}
	for (uint iView = 0; iView < allnum; iView++) {
		if (recfp->mapdata.note[iView].lane == NOTE_LANE_LOW) {
			objectN[2] = iView;
			break;
		}
	}
	return;
}

/* TODO: 戻り値に構造体使うのは不適切 */
static rec_play_score_t GetScore3(rec_play_score_t score,
	rec_play_judge_t judge, const int notes, const int MaxCombo)
{
	score.normal = (judge.just * 90000 + judge.good * 86667 + judge.safe * 45000) / notes;
	score.combo = MaxCombo * 10000 / notes;
	if (score.normal + score.combo - score.loss == 100000) {
		score.pjust = maxs_2(maxs_2(100 - (notes - judge.pjust), (judge.pjust * 100 / (double)notes) - 65), 0);
	}
	else {
		score.pjust = 0;
	}
	score.sum = score.normal + score.combo - score.loss + score.pjust;
	return score;
}

static void Getxxxpng(wchar_t *str, int num) {
	*str = num / 100 + '0';
	str++;
	*str = num / 10 % 10 + '0';
	str++;
	*str = num % 10 + '0';
	str++;
	*str = '.';
	str++;
	*str = 'p';
	str++;
	*str = 'n';
	str++;
	*str = 'g';
	str++;
	*str = '\0';
	return;
}

static void Getxxxwav(wchar_t *str, int num) {
	*str = num / 100 + '0';
	str++;
	*str = num / 10 % 10 + '0';
	str++;
	*str = num % 10 + '0';
	str++;
	*str = '.';
	str++;
	*str = 'w';
	str++;
	*str = 'a';
	str++;
	*str = 'v';
	str++;
	*str = '\0';
	return;
}

/* (ret / 100) */
static void cal_back_x(int *xpos, rec_map_eff_data_t *mapeff, int cam) {
	const double scrool = mapeff->scrool.data[mapeff->scrool.num].speed;
	rec_mapeff_speedt_dataset_t *p_speedt = mapeff->speedt;
	double speed3 = p_speedt[3].d[p_speedt[3].num].speed;
	double speed4 = p_speedt[4].d[p_speedt[4].num].speed;

	xpos[0] -= (int)(100 * speed3 * scrool);
	while (xpos[0] + 100 * cam / 5 > 0)      { xpos[0] -= 64000; }
	while (xpos[0] + 100 * cam / 5 < -64000) { xpos[0] += 64000; }

	xpos[1] -= (int)(500 * speed4 * scrool);
	while (xpos[1] + 100 * cam > 0)      { xpos[1] -= 64000; }
	while (xpos[1] + 100 * cam < -64000) { xpos[1] += 64000; }

	xpos[2] = xpos[1];
	return;
}

static void recSetLine(int line[], rec_move_set_t move[], int Ntime, int loop) {
	for (int iLine = 0; iLine < loop; iLine++) {
		if (IS_BETWEEN(0, move[iLine].d[move[iLine].num].Stime, Ntime)) {
			line[iLine] =
				(int)movecal(
					move[iLine].d[move[iLine].num].mode,
					move[iLine].d[move[iLine].num].Stime,
					move[iLine].d[move[iLine].num - 1].pos,
					move[iLine].d[move[iLine].num].Etime,
					move[iLine].d[move[iLine].num].pos, Ntime);
		}
		/* TODO: このwhile文の中はこの関数内でやることではないような... */
		while (
			move[iLine].d[move[iLine].num].Etime <= Ntime &&
			move[iLine].d[move[iLine].num].Stime >= 0 ||
			move[iLine].d[move[iLine].num].mode == 4)
		{
			line[iLine] = move[iLine].d[move[iLine].num].pos;
			move[iLine].num++;
		}
	}
	return;
}

static void PlayDrawItem(rec_map_eff_data_t *mapeff,
	short MovieN, int Ntime, int Xmidline, dxcur_pic_c item[])
{
	view_BPM_box *v_BPM = &mapeff->v_BPM.data[mapeff->v_BPM.num];
	int drawA;
	int drawX;
	int drawY;
	int drawS;
	int drawR;
	rec_play_xy_set_t camera;
	RecPlayGetCameraPos(&camera.x, &camera.y);
	for (item_box *pMovie = &mapeff->Movie[MovieN]; pMovie->endtime > -500; pMovie++) {
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
			drawA = lins(0, drawA, 60000 / v_BPM->BPM, 0,
				(Ntime - v_BPM->time) % (60000 / v_BPM->BPM));
		}
		if (pMovie->eff.chara_alphr == 1) {
			drawA = lins(320, drawA, 60, 0, betweens(60, abss(Xmidline, drawX), 320));
		}
		if (pMovie->eff.bpm_size == 1) {
			drawS = pals(60000 / v_BPM->BPM, drawS / 2, 0, drawS,
				(Ntime - v_BPM->time) % (60000 / v_BPM->BPM));
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
		DrawDeformationPic(drawX, drawY, drawS / 100.0, drawS / 100.0, drawR, item[pMovie->ID].handle());
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	}
}

static void RecPlayCalUserPal(rec_play_userpal_t *userpal, short notes, rec_play_time_set_t *time, int AutoFg) {
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
	userpal->score = GetScore3(userpal->score, userpal->judgeCount, notes, userpal->Mcombo);
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

static int GetRemainNotes(rec_play_judge_t judge, int Notes) {
	return Notes - judge.just - judge.good - judge.safe - judge.miss;
}

static int GetHighScore(const TCHAR *songName, rec_dif_t dif) {
	rec_save_score_t scoreBuf;
	RecSaveReadScoreOneDif(&scoreBuf, songName, dif);
	return scoreBuf.score;
}

static void RecPlayStepObjectNG(short objectNG[], const note_box_2_t note[], const short objectN[]) {
	for (int iLine = 0; iLine < 3; iLine++) {
		objectNG[iLine] = maxs_2(objectNG[iLine], objectN[iLine]);
		while (note[objectNG[iLine]].object == NOTE_GHOST) {
			objectNG[iLine] = note[objectNG[iLine]].next;
		}
	}
	return;
}

static void RecPlayStepCharaMotionNum(rec_chara_gra_data_t chamo, DxTime_t Ntime) {
	for (int iLine = 0; iLine < 3; iLine++) {
		while (IS_BETWEEN(0, chamo[iLine].time[chamo[iLine].num + 1], Ntime)) {
			chamo[iLine].num++;
		}
	}
	return;
}

static void RecPlayStepSpeedtNum(rec_mapeff_speedt_dataset_t speedt[], DxTime_t Ntime) {
	for (int iLine = 0; iLine < 3; iLine++) {
		while (IS_BETWEEN(0, speedt[iLine].d[speedt[iLine].num + 1].time, Ntime)) {
			speedt[iLine].num++;
		}
	}
	if (optiondata.backbright == 0) { return; }
	while (IS_BETWEEN(0, speedt[3].d[speedt[3].num + 1].time, Ntime)) {
		speedt[3].num++;
	}
	return;
}

static void RecPlayStepViewBpmNum(rec_view_bpm_set_t *v_BPM, DxTime_t Ntime) {
	while (IS_BETWEEN_LEFT_LESS(-1000, v_BPM->data[v_BPM->num + 1].time, Ntime)) {
		v_BPM->num++;
	}
	return;
}

static void RecPlayStepCameraNum(rec_camera_set_t *camera, DxTime_t Ntime) {
	while (IS_BETWEEN_RIGHT_LESS(0, camera->data[camera->num].endtime, Ntime)) {
		camera->num++;
	}
	return;
}

static void RecPlayStepScroolNum(rec_scrool_set_t *scrool, DxTime_t Ntime) {
	while (IS_BETWEEN(0, scrool->data[scrool->num + 1].starttime, Ntime)) {
		scrool->num++;
	}
	return;
}

static void RecPlayStepMovieNum(const item_box movie[], short *movieN, DxTime_t Ntime) {
	if (optiondata.backbright == 0) { return; }
	while (IS_BETWEEN_LESS(-500, movie[*movieN].endtime, Ntime)) { (*movieN)++;}
	return;
}

static void RecPlayStepFallNum(rec_fall_data_t *fall, DxTime_t Ntime) {
	if (optiondata.backbright == 0) { return; }
	while (IS_BETWEEN(0, fall->d[fall->num + 1].time, Ntime)) { fall->num++; }
	return;
}

static void RecPlayStepGuideLineNum(short guideN[], const rec_move_set_t ymove[], DxTime_t Ntime) {
	for (int iLane = 0; iLane < 3; iLane++) {
		while ((ymove[iLane].d[guideN[iLane]].Stime >= 0 &&
			ymove[iLane].d[guideN[iLane]].Etime <= Ntime) ||
			ymove[iLane].d[guideN[iLane]].mode == 4)
		{
			guideN[iLane]++;
		}
	}
	return;
}

static void RecPlayStepCharaArrowNum(rec_chara_arrow_t *carrow, DxTime_t Ntime) {
	while (IS_BETWEEN_RIGHT_LESS(0, carrow->d[carrow->num + 1].time, Ntime)) {
		carrow->num++;
	}
	return;
}

static void RecPlayStepLockNum(short lockN[], const rec_map_eff_data_t *mapeff, DxTime_t Ntime) {
	for (int iVect = 0; iVect < 2; iVect++) {
		while (IS_BETWEEN(0, mapeff->lock[iVect][1][lockN[iVect] + 1], Ntime)) {
			lockN[iVect]++;
		}
	}
	return;
}

static void RecPlayStepViewTimeNum(short *viewTN, const rec_map_eff_data_t *mapeff,
	DxTime_t Ntime)
{
	while (IS_BETWEEN(0, mapeff->viewT[0][*viewTN + 1], Ntime)) { (*viewTN)++; }
	return;
}

static void RecPlayStepViewLineNum(rec_viewline_dataset_t *viewLine, DxTime_t Ntime) {
	while (IS_BETWEEN_LEFT_LESS(0, viewLine->d[viewLine->num + 1].time, Ntime)) {
		viewLine->num++;
	}
	return;
}

static void RecPlayStepAllNum(rec_score_file_t *recfp, short *objectNG, short *movieN,
	short *guideN, short *lockN, short *viewTN, const short *objectN)
{
	RecPlayStepObjectNG(objectNG, recfp->mapdata.note, objectN);
	RecPlayStepCharaMotionNum(recfp->mapeff.chamo, recfp->time.now);
	RecPlayStepSpeedtNum(recfp->mapeff.speedt, recfp->time.now);
	RecPlayStepViewBpmNum(&recfp->mapeff.v_BPM, recfp->time.now);
	RecPlayStepCameraNum(&recfp->mapeff.camera, recfp->time.now);
	RecPlayStepScroolNum(&recfp->mapeff.scrool, recfp->time.now);
	RecPlayStepMovieNum(recfp->mapeff.Movie, movieN, recfp->time.now);
	RecPlayStepFallNum(&recfp->mapeff.fall, recfp->time.now);
	RecPlayStepGuideLineNum(guideN, recfp->mapeff.move.y, recfp->time.now);
	RecPlayStepCharaArrowNum(&recfp->mapeff.carrow, recfp->time.now);
	RecPlayStepLockNum(lockN, &recfp->mapeff, recfp->time.now);
	RecPlayStepViewTimeNum(viewTN, &recfp->mapeff, recfp->time.now);
	RecPlayStepViewLineNum(&recfp->mapeff.viewLine, recfp->time.now);
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

#if 1 /* Notes Picture */

/**
 * return 0 = normal, 1 = continue, 2 = break;
 */
static int StepViewNoDrawNote(int hittime, rec_map_eff_data_t *mapeff,
	short viewTN, int *viewTadd, int Ntime)
{
	//表示/非表示ナンバーを進める
	if (hittime >= mapeff->viewT[0][viewTN + *viewTadd + 1] &&
		mapeff->viewT[0][viewTN + *viewTadd + 1] >= 0) {
		(*viewTadd)++;
	}
	//非表示スキップ
	if (hittime - Ntime >= mapeff->viewT[1][viewTN + *viewTadd]) {
		return 1;
	}
	//3秒ブレーク
	if (hittime - Ntime >= 3000 &&
		3000 >= mapeff->viewT[1][viewTN + *viewTadd]) {
		return 2;
	}
	return 0;
}

/**
 * return 0 = normal, 1 = continue, 2 = break;
 */
static int StepNoDrawNote(int *viewTadd, int *XLockNoAdd, int *YLockNoAdd, int *SpeedNoAdd,
	note_box_2_t *note, rec_map_eff_data_t *mapeff, short viewTN, short lockN[], int iLine,
	int Ntime)
{
	rec_mapeff_speedt_dataset_t *p_speedt = &mapeff->speedt[iLine];

	int ret = 0;
	ret = StepViewNoDrawNote(note->hittime, mapeff, viewTN, viewTadd, Ntime);
	if (ret == 1) { return 1; }
	else if (ret == 2) { return 2; }
	//ノーツロックナンバーを進める
	if (note->hittime >= mapeff->lock[0][1][lockN[0] + *XLockNoAdd + 1] &&
		mapeff->lock[0][1][lockN[0] + *XLockNoAdd + 1] >= 0) {
		(*XLockNoAdd)++;
	}
	if (note->hittime >= mapeff->lock[1][1][lockN[1] + *YLockNoAdd + 1] &&
		mapeff->lock[1][1][lockN[1] + *YLockNoAdd + 1] >= 0) {
		(*YLockNoAdd)++;
	}
	// スピードナンバーを進める
	while (IS_BETWEEN(0, p_speedt->d[p_speedt->num + *SpeedNoAdd + 1].time, note->hittime)) {
		(*SpeedNoAdd)++;
	}
	return 0;
}

static void CalPalCrawNote(int *DrawX, int *DrawY, int *DrawC, rec_play_lanepos_t *lanePos,
	int lock0, int lock1, const note_box_2_t *note, double speedt, const rec_scrool_set_t *scrool,
	int Ntime, int iLine)
{
	const double scroolSpeed = scrool->data[scrool->num].speed;
	const double scroolBaseTime = scrool->data[scrool->num].basetime;

	//縦位置
	*DrawY = ((lock1 == 1) ? note->ypos : lanePos->y[iLine]);
	//横位置
	*DrawX = (2 * speedt * (note->viewtime - scroolSpeed * Ntime - scroolBaseTime) / 5);
	*DrawX += ((lock0 == 1) ? note->xpos : lanePos->x[iLine]);
	//色
	*DrawC = note->color;
}

/**
 * return 0 = normal, 1 = continue, 2 = break;
 */
static int DrawNoteOne(int *viewTadd, int *XLockNoAdd, int *YLockNoAdd, int *SpeedNoAdd,
	note_box_2_t *note, rec_map_eff_data_t *mapeff, rec_play_lanepos_t *lanePos, short viewTN,
	short lockN[], int iLine, int Ntime, rec_play_notepic_t *noteimg)
{
	int ret = 0;
	int DrawX = 0;
	int DrawY = 0;
	int DrawC = 0;
	int DrawID = 0;
	ret = StepNoDrawNote(viewTadd, XLockNoAdd, YLockNoAdd, SpeedNoAdd,
		note, mapeff, viewTN, lockN, iLine, Ntime);
	if (ret == 1) { return 1; }
	else if (ret == 2) { return 2; }
	CalPalCrawNote(&DrawX, &DrawY, &DrawC, lanePos, mapeff->lock[0][0][lockN[0] + *XLockNoAdd],
		mapeff->lock[1][0][lockN[1] + *YLockNoAdd], note,
		mapeff->speedt[iLine].d[mapeff->speedt[iLine].num + *SpeedNoAdd].speed, &mapeff->scrool,
		Ntime, iLine);
	switch (note->object) {
	case 1:
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
	return 0;
}

static void RecPlayDrawNoteAll(rec_play_lanepos_t *lanePos, short objectN[], note_box_2_t note[],
	rec_map_eff_data_t *mapeff, short viewTN, short *lockN, int Ntime, rec_play_notepic_t *noteimg)
{
	int ret = 0;
	int viewTadd = 0;
	int XLockNoAdd = 0;
	int YLockNoAdd = 0;
	int SpeedNoAdd = 0;

	for (int iLine = 0; iLine < 3; iLine++) {
		viewTadd = 0;
		XLockNoAdd = 0;
		YLockNoAdd = 0;
		SpeedNoAdd = 0;
		for (int iNote = objectN[iLine]; note[iNote].hittime > 0; iNote = note[iNote].next) {
			ret = DrawNoteOne(&viewTadd, &XLockNoAdd, &YLockNoAdd, &SpeedNoAdd, &note[iNote],
				mapeff, lanePos, viewTN, lockN, iLine, Ntime, noteimg);
			if (ret == 1) { continue; }
			else if (ret == 2) { break; }
			if (note[iNote].next == -1) { break; }
		}
	}
	return;
}

#endif /* Notes Picture */

#if 1 /* Back Ground */

static void DrawFallBack(int Yline, dxcur_pic_c item[], rec_fall_data_t *falleff) {
	static int baseX = 0;
	static int baseY = 0;
	for (int ix = 0; ix < 2; ix++) {
		for (int iy = 0; iy < 3; iy++) {
			RecRescaleDrawGraph(baseX + ix * 640, baseY + Yline - iy * 480,
				item[falleff->d[falleff->num].No].handle(), TRUE);
		}
	}
	baseX -= 5;
	baseY += 2;
	if (baseX <= -640) { baseX += 640; }
	if (baseY >= 640) { baseY -= 480; }
}

static void PlayDrawBackGround(rec_map_eff_data_t *mapeff, int Yline[],
	rec_play_back_pic_t *backpic, dxcur_pic_c *item)
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
	if (mapeff->fall.d[mapeff->fall.num].No >= 0) {
		DrawFallBack(Yline[3], item, &mapeff->fall);
	}
	return;
}

#endif /* Back Ground */

#if 1 /* Guide Line */

static int RecMapGetPosFromMove(rec_move_set_t move[], int time, int lane) {
	int ret = 0;
	int offset = 1;

	while (IS_BETWEEN(0, move[lane].d[offset].Etime, time)) { offset++; }

	if (move[lane].d[offset].Stime < 0) {
		return move[lane].d[offset - 1].pos;
	}

	if (move[lane].d[offset].Etime <= move[lane].d[offset].Stime) {
		if (time < move[lane].d[offset].Stime) {
			return move[lane].d[offset - 1].pos;
		}
		else {
			return move[lane].d[offset].pos;
		}
	}

	if (IS_BETWEEN(0, move[lane].d[offset].Stime, time) && move[lane].d[offset].Etime > time) {
		ret = movecal(move[lane].d[offset].mode,
			move[lane].d[offset].Stime,
			move[lane].d[offset - 1].pos,
			move[lane].d[offset].Etime, move[lane].d[offset].pos,
			time);
	}
	else {
		ret = move[lane].d[offset - 1].pos;
	}

	return ret;
}

static int RecPlayStepSpeedNum(rec_map_eff_data_t *mapeff, int iLine, int Ptime) {
	rec_mapeff_speedt_dataset_t *p_speedt = &mapeff->speedt[iLine];
	int SpeedNoAdd = 0;
	while (IS_BETWEEN(0, p_speedt->d[p_speedt->num + SpeedNoAdd + 1].time, Ptime)) {
		SpeedNoAdd++;
	}
	return SpeedNoAdd;
}

static void RecPlayGetTimeLanePosBase(int *DrawX, int *DrawY, rec_move_all_set_t *move,
	const rec_play_lanepos_t *lanePos, double speedt, int Ylock, rec_scrool_set_t *scrool, int Ntime, int Ptime, int lane)
{
	/* TODO: 今後使うから消すな */
	const double scroolSpeed = scrool->data[scrool->num].speed;
	const double scroolBaseTime = scrool->data[scrool->num].basetime;

	*DrawY = ((Ylock == 1) ? RecMapGetPosFromMove(move->y, Ptime, lane) : lanePos->y[lane]) + 15;

	/* TODO: スクロールの考慮ができていない、てかVtime作りたい */
	*DrawX = (int)((speedt * 20 * (Ptime - Ntime) + 5000) / 50) + 50;
	*DrawX += lanePos->x[lane] - 150 + 15;
	return;
}

static void RecPlayGetTimeLanePos(int *retX, int *retY, rec_map_eff_data_t *mapeff, const rec_play_lanepos_t *lanePos,
	int iLine, short YlockN, int Ntime, int Ptime)
{
	int SpeedNoAdd = RecPlayStepSpeedNum(mapeff, iLine, Ptime);
	int YlockNoAdd = 0;

	while (IS_BETWEEN(0, mapeff->lock[1][1][YlockN + YlockNoAdd + 1], Ptime)) {
		YlockNoAdd++;
	}

	RecPlayGetTimeLanePosBase(retX, retY, &mapeff->move, lanePos,
		mapeff->speedt[iLine].d[mapeff->speedt[iLine].num + SpeedNoAdd].speed,
		mapeff->lock[1][0][YlockN + YlockNoAdd],
		&mapeff->scrool, Ntime, Ptime,
		iLine);
	return;
}

void RecPlayDrawGuideBorder(rec_score_file_t *recfp, const rec_play_lanepos_t *lanePos, short YlockN) {
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

		uint NlineViewNo = recfp->mapeff.viewLine.num;
		while (IS_BETWEEN_LEFT_LESS(0, recfp->mapeff.viewLine.d[NlineViewNo + 1].time, Ptime)) {
			NlineViewNo++;
		}
		if (!(recfp->mapeff.viewLine.d[NlineViewNo].enable)) { continue; }
		if (recfp->time.end < Ptime) { return; }

		RecPlayGetTimeLanePos(&posX1, &posY1, &recfp->mapeff, lanePos, 0, YlockN, Ntime, Ptime);
		RecPlayGetTimeLanePos(&posX2, &posY2, &recfp->mapeff, lanePos, 1, YlockN, Ntime, Ptime);
		RecPlayGetTimeLanePos(&posX3, &posY3, &recfp->mapeff, lanePos, 2, YlockN, Ntime, Ptime);

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

static void PlayShowGuideLine1(rec_map_eff_data_t *mapeff, const rec_move_data_t *Ymove,
	const rec_play_lanepos_t *lanePos, int Line, int iDraw, int drawC, int Ntime, short YlockN,
	bool viewEn)
{
	if (!viewEn) { return; }

	int drawLeft = 0;
	int drawRight = 0;
	int drawY1 = 0;
	int drawY2 = 0;
	rec_play_xy_set_t camera;

	RecPlayGetCameraPos(&camera.x, &camera.y);
	RecPlayGetTimeLanePos(&drawLeft, &drawY1, mapeff, lanePos, Line, YlockN, Ntime, Ymove[iDraw - 1].Etime);
	drawRight = 1280 - camera.x;
	drawY2    = drawY1;
	DrawLineRecField(drawLeft, drawY1, drawRight, drawY2, drawC, optiondata.lineThick);
	return;
}

static int PlayShowGuideLine2(rec_map_eff_data_t *mapeff, rec_move_data_t *Ymove,
	const rec_play_lanepos_t *lanePos, int Line, int iDraw, int drawC, int Ntime, short YlockN,
	bool viewEn)
{
	int drawLeft = 0;
	int drawRight = 0;
	int drawY1 = 0;
	int drawY2 = 0;
	rec_play_xy_set_t camera;

	if (!viewEn) { return 1; }

	RecPlayGetCameraPos(&camera.x, &camera.y);

	RecPlayGetTimeLanePos(&drawRight, &drawY2, mapeff, lanePos, Line, YlockN, Ntime, Ymove[0].Stime);
	drawLeft = 0 - camera.x;
	drawY1   = drawY2;

	if (960 < drawLeft) { return 2; }

	DrawLineRecField(drawLeft, drawY1, drawRight, drawY2, drawC, optiondata.lineThick);
	return 0;
}

static int PlayShowGuideLine3(rec_map_eff_data_t *mapeff, rec_move_data_t *Ymove,
	const rec_play_lanepos_t *lanePos, int Line, int iDraw, int drawC, int Ntime, short YlockN,
	bool viewEn)
{
	int drawLeft = 0;
	int drawRight = 0;
	int drawY1 = 0;
	int drawY2 = 0;

	if (!viewEn) { return 1; }

	RecPlayGetTimeLanePos(&drawLeft,  &drawY1, mapeff, lanePos, Line, YlockN, Ntime, Ymove[iDraw - 1].Etime);
	RecPlayGetTimeLanePos(&drawRight, &drawY2, mapeff, lanePos, Line, YlockN, Ntime, Ymove[iDraw].Stime);
	drawY2 = drawY1;

	if (960 < drawLeft) { return 2; }

	DrawLineRecField(drawLeft, drawY1, drawRight, drawY2, drawC, optiondata.lineThick);
	return 0;
}

static int PlayShowGuideLine4(rec_map_eff_data_t *mapeff, rec_move_data_t *Ymove,
	const rec_play_lanepos_t *lanePos, int Line, int iDraw, int drawC, int Ntime, short YlockN,
	bool viewEn)
{
	int drawLeft = 0;
	int drawRight = 0;
	int drawY1 = 0;
	int drawY2 = 0;

	if (!viewEn) { return 1; }

	RecPlayGetTimeLanePos(&drawLeft,  &drawY1, mapeff, lanePos, Line, YlockN, Ntime, Ymove[iDraw].Stime);
	RecPlayGetTimeLanePos(&drawRight, &drawY2, mapeff, lanePos, Line, YlockN, Ntime, Ymove[iDraw].Etime);

	if (960 < drawLeft) { return 2; }

	DrawLineCurveRecField(drawLeft, drawY1, drawRight, drawY2, Ymove[iDraw].mode, drawC, optiondata.lineThick);
	return 0;
}

static int PlayShowGuideLine(rec_score_file_t *recfp, const rec_play_lanepos_t *lanePos, int Line, int iDraw,
	short YlockN)
{
	int ret = 0;
	int Ntime = recfp->time.now;
	rec_move_data_t *Ymove = recfp->mapeff.move.y[Line].d;
	rec_map_eff_data_t *mapeff = &recfp->mapeff;

	int drawLeft = 0;
	int drawRight = 0;
	int drawY1 = 0;
	int drawY2 = 0;
	int drawC = 0;
	rec_play_xy_set_t camera;
	RecPlayGetCameraPos(&camera.x, &camera.y);

	uint NlineViewNo = recfp->mapeff.viewLine.num;
	while (IS_BETWEEN_LEFT_LESS(0, recfp->mapeff.viewLine.d[NlineViewNo + 1].time, Ymove[iDraw].Stime)) {
		NlineViewNo++;
	}

	const bool viewEn = recfp->mapeff.viewLine.d[NlineViewNo].enable;

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

	/* TODO: move直角ラインが欲しい */
	if (Ymove[iDraw].Stime < 0) {
		PlayShowGuideLine1(mapeff, Ymove, lanePos, Line, iDraw, drawC, Ntime, YlockN, viewEn); /* ラスライン */
		return 1;
	}

	if (iDraw < 1) {
		ret = PlayShowGuideLine2(mapeff, Ymove, lanePos, Line, iDraw, drawC, Ntime, YlockN, viewEn); /* スタートライン */
		switch (ret) {
		case 1:
			return 0;
		case 2:
			return -1;
		}
	}
	else if (Ntime < Ymove[iDraw].Etime) {
		ret = PlayShowGuideLine3(mapeff, Ymove, lanePos, Line, iDraw, drawC, Ntime, YlockN, viewEn); /* move繋ぎライン */
		switch (ret) {
		case 1:
			return 0;
		case 2:
			return -1;
		}
	}
	ret = PlayShowGuideLine4(mapeff, Ymove, lanePos, Line, iDraw, drawC, Ntime, YlockN, viewEn); /* moveライン */
	if (ret == 2) { return -1; }
	return 0;
}

static void PlayShowAllGuideLine(rec_score_file_t *recfp, const rec_play_lanepos_t *lanePos,
	short LineMoveN[], short YlockN)
{
	int flag = 0;

	for (int iLine = 0; iLine < 3; iLine++) {
		for (int iDraw = LineMoveN[iLine]; 1; iDraw++) {
			flag = PlayShowGuideLine(recfp, lanePos, iLine, iDraw, YlockN);
			if (flag != 0) { break; }
		}
	}
	return;
}

#endif /* Guide Line */

#if 1 /* control */

static void RecPlayGetKeyhold(rec_score_file_t *recfp, rec_play_key_hold_t *keyhold, int *holdG,
	short *objectNG, bool AutoFlag)
{
	char key[256];

	GetHitKeyStateAll(key);
	if (AutoFlag) {
		if (key[KEY_INPUT_G] == 0) { *holdG = 0; }
		else if (key[KEY_INPUT_G] == 1) { (*holdG)++; }
		AutoAution(keyhold, recfp->mapdata.note, objectNG, recfp->time.now);
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

			picID = Ntime * mapeff->v_BPM.data[mapeff->v_BPM.num].BPM /
				20000 % 6 + mapeff->chamo[this->pos].gra[mapeff->chamo[this->pos].num] * 6;
		}
		if (mapeff->carrow.d[mapeff->carrow.num].data == 1) {
			DrawGraphRecField(drawX - 160, drawY, this->charaimg[picID]);
		}
		else {
			DrawTurnGraphRecField(drawX + 30, drawY, this->charaimg[picID]);
		}
	}

public:
	void UpdateCharapos(int time, note_box_2_t note[], short int No[],
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
			if (note[No[i]].hittime <= time + 40 &&
				(note[No[i]].object == NOTE_CATCH ||
					note[No[i]].object == NOTE_BOMB))
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
		if (mapeff->carrow.d[mapeff->carrow.num].data == 1) {
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

	void ViewDist(rec_play_status_t status, const distance_score_t *Dscore, const rec_play_time_set_t *time) {
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
		const rec_map_detail_t *mapdata, int Hscore, int holdG)
	{
		RecRescaleDrawGraph(0, 0, this->baseImg.handle(), TRUE);
		this->ViewScore(userpal->score, Hscore, time->now);
		this->ViewLife(userpal->life, userpal->Exlife);
		this->ViewDist(userpal->status, &userpal->Dscore, time);
		RecRescaleDrawGraph(0, 0, this->sbbarimg.handle(), TRUE);
		this->ViewRunStatus(userpal, mapdata->notes, Hscore);
	}
};

class rec_play_gapbar_c {
private:
	dxcur_pic_c gapbarimg  = dxcur_pic_c(L"picture/GapBer.png");
	dxcur_pic_c gaplineimg = dxcur_pic_c(L"picture/GapBerLine.png");

public:
	void ViewGapBar(gap_box *gap) {
		int No = gap->count % 30;

		RecRescaleDrawGraph(219, 460, this->gapbarimg.handle(), TRUE);

		for (int i = 0; i < 30; i++) {
			No--;
			if (No < 0) { No += 30; }
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (510 - No * 17) / 2);
			RecRescaleDrawGraph(318 - gap->view[i], 460, this->gaplineimg.handle(), TRUE);
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

#endif /* class */

/* main action */

/**
 * @param[out] ret_map_det map_detail の受け皿
 * @param[out] ret_userpal userpal の受け皿
 * @param[out] ret_nameset nameset の受け皿
 * @param[out] ret_fileN ファイル名の受け皿
 * @param[in] p パックナンバー
 * @param[in] n 曲ナンバー
 * @param[in] o 難易度ナンバー
 * @param[in] shift マップ生成フラフ
 * @param[in] AutoFlag オートプレイフラグ
 * @return now_scene_t 次のシーン
 */
now_scene_t RecPlayMain(rec_map_detail_t *ret_map_det, rec_play_userpal_t *ret_userpal,
	rec_play_nameset_t *ret_nameset, TCHAR *ret_fileN, int p, int n, int o, int AutoFlag)
{

#if 1 /* num define */

	/* char */
	char key[256];

	/* short */
	short i[3];

	/* int */
	int charahit = 0; //キャラがノーツをたたいた後であるかどうか。[1以上で叩いた、0で叩いてない]
	int G[20];
	int holdG = 0;
	int AllNotesHitTime = -1;
	int LaneTrack[3] = { -150,-150,-150 };
	int StopFrag = -1;
	int HighScore; //ハイスコア
	rec_play_chara_hit_attack_t hitatk;
	int fps[62];//0～59=1フレーム間隔の時間,60=次の代入先,61=前回の時間
	short LineMoveN[3] = { 0,0,0 }; //↑のライン表示番号
	short lockN[2] = { 0,0 }; //↑の番号
	short viewTN = 0;
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
	play_key_stat_t key_stat;
	short MovieN = 0;
	rec_view_bpm_set_t v_BPM;
	short objectN[3] = { 5999,5999,5999 }; //note number
	short objectNG[3] = { 0,0,0 }; //note number without ghost note
	rec_score_file_t recfp;
	rec_play_userpal_t userpal;

	/* double */
	double GD[5];
	double SumRate[2] = { 0,0 };
	double DifRate; //譜面定数

	/* string */
	TCHAR dataE[255];
	TCHAR GT1[255];
	TCHAR GT2[255];

	/* class */
	rec_play_sbar_c sbarClass;
	rec_play_combo_c comboPicClass;
	rec_play_gapbar_c gapbarClass;
	rec_play_sound_c p_sound;
	rec_play_runner_c runnerClass;
	rec_play_keyview_c keyviewClass;
	rec_cutin_c cutin;

	/* mat */
	dxcur_pic_c item[99]; //アイテムのfd、DrawGraphで呼べる。
	short itemN = 0; //↑の番号
	int Sitem[99]; //サウンドアイテムのfd
	short SitemN = 0; //↑の番号
	rec_play_back_pic_t backpic;
	dxcur_pic_c dangerimg = dxcur_pic_c(_T("picture/danger.png"));
	dxcur_pic_c dropimg   = dxcur_pic_c(_T("picture/drop.png"));
	dxcur_pic_c filterimg = dxcur_pic_c(_T("picture/Black.png"));
	rec_play_notepic_t noteimg;

#endif /* num define */

	/* ピクチャの用意 */
	ReadyBonusPsmat();
	ReadyEffPicture();
	ReadyJudgePicture();
	/* action */
	for (i[0] = 0; i[0] <= 59; i[0]++) { fps[i[0]] = 17; }
	fps[60] = 0;
	fps[61] = 0;

	if (optiondata.SEenable == 0) { RecPlayInitMelodySnd(); }

	RecGetMusicFolderPath(dataE, 255, p, n);
	RecGetMusicFolderName(ret_fileN, 255, p, n);

	RecGetMusicMapRrsPath(GT1, 255, p, n, (rec_dif_t)o);

	/* rrsデータの内容を読み込む */
	if (rec_score_fread(&recfp, GT1) != 0) { return SCENE_EXIT; }

	backpic.sky.reload(   recfp.nameset.sky);
	backpic.ground.reload(recfp.nameset.ground);
	backpic.water.reload( recfp.nameset.water);

	HighScore = GetHighScore(ret_fileN, (rec_dif_t)o);

	for (i[0] = 0; i[0] < 100; i[0]++) {
		strcopy_2(dataE, GT1, 255);
		Getxxxpng(&GT2[0], i[0]);
		strcats(GT1, GT2);
		item[i[0]].reload(GT1);
		if (item[i[0]].handle() == -1) { break; }
	}
	for (i[0] = 1; i[0] < 100; i[0]++) {
		strcopy_2(dataE, GT1, 255);
		Getxxxwav(&GT2[0], i[0]);
		strcats(GT1, GT2);
		Sitem[i[0] - 1] = LoadSoundMem(GT1);
		ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, Sitem[i[0] - 1]);
		if (Sitem[i[0] - 1] == -1) { break; }
	}

	//ゲーム開始前の下準備
	recfp.mapdata.notes = notzero(recfp.mapdata.notes);
	GD[0] = recfp.mapdata.mdif / 100.0 - recfp.mapdata.Lv;//mdifと難易度表記の差
	if (recfp.mapdata.Lv == 0) { DifRate = 0; }
	else if (2 <= GD[0]) { DifRate = recfp.mapdata.Lv + 0.9; }
	else if (0 <= GD[0] && GD[0] < 2) { DifRate = recfp.mapdata.Lv + 0.45 * GD[0]; }
	else { DifRate = recfp.mapdata.mdif / 100.0; }
	RecResetPlayObjectNum(objectN, &recfp);
	RecSysBgmSetMem(recfp.nameset.mp3FN, ARRAY_COUNT(recfp.nameset.mp3FN));
	RecSysBgmPlay(true, false, true);
	WaitTimer(WAIT_TIME_AFTER_MUSICPLAY);
	Stime = GetNowCount();
	cutin.SetIo(0);
	//ゲーム開始
	while (1) {
		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }

		RecPlayStepAllNum(&recfp, objectNG, &MovieN, LineMoveN, lockN, &viewTN, objectN);

		RecPlayGetKeyhold(&recfp, &keyhold, &holdG, objectNG, AutoFlag);

		//カメラ移動
		RecPlaySetCamera(&recfp.mapeff.camera, recfp.time.now);
		//lanePos.x(横位置)の計算
		recSetLine(lanePos.x, recfp.mapeff.move.x, recfp.time.now, 3);
		//lanePos.y(縦位置)の計算
		if (optiondata.backbright == 0) {
			recSetLine(lanePos.y, recfp.mapeff.move.y, recfp.time.now, 3);
		}
		else {
			recSetLine(lanePos.y, recfp.mapeff.move.y, recfp.time.now, 5);
		}
		runnerClass.UpdateCharapos(recfp.time.now, recfp.mapdata.note, objectNG, &keyhold, &hitatk);
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
		RecJudgeAllNotes(recfp.mapdata.note, objectN, recfp.time.now, Sitem,
			&keyhold, &hitatk, LaneTrack, &charahit, runnerClass.pos, &userpal, &p_sound);
		RecPlayCalUserPal(&userpal, recfp.mapdata.notes, &recfp.time, AutoFlag);

		ClearDrawScreen(); /* 描画エリアここから */
		//背景表示
		if (optiondata.backbright != 0) {
			PlayDrawBackGround(&recfp.mapeff, lanePos.y, &backpic, item);
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
			PlayDrawItem(&recfp.mapeff, MovieN, recfp.time.now, lanePos.x[1], item);
		}
		// view line
		PlayShowAllGuideLine(&recfp, &lanePos, LineMoveN, lockN[1]);
		RecPlayDrawGuideBorder(&recfp, &lanePos, lockN[1]);
		/* キャラ周り表示 */
		runnerClass.ViewRunner(&recfp.mapeff, &keyhold, &lanePos, charahit, recfp.time.now);
		//コンボ表示
		comboPicClass.ViewCombo(userpal.Ncombo);
		//判定表示
		PlayShowJudge(&lanePos, runnerClass.pos);
		/* 音符表示 */
		RecPlayDrawNoteAll(&lanePos, objectN, recfp.mapdata.note, &recfp.mapeff, viewTN,
			lockN, recfp.time.now, &noteimg);
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
		//ノーツが全部なくなった瞬間の時間を記録
		if (GetRemainNotes(userpal.judgeCount, recfp.mapdata.notes) == 0 && AllNotesHitTime < 0) {
			AllNotesHitTime = GetNowCount();
		}
		//オートでなく、ノーミス以上を出したら演出
		if (AutoFlag == 0 && AllNotesHitTime + 2000 > GetNowCount()) {
			ShowBonusEff(userpal.judgeCount, AllNotesHitTime);
		}
		//終了時間から5秒以上たって、曲が終了したらカットイン再生。
		if ((cutin.IsClosing() == 0) &&
			(recfp.time.end + 5000 <= recfp.time.now) &&
			(RecSysBgmCheckSoundMem() == 0))
		{
			cutin.SetCutTipFg(CUTIN_TIPS_NONE);
			cutin.SetIo(1);
		}
		cutin.DrawCut();

		//カットイン再生から2秒以上たったら抜ける。
		if (cutin.IsEndAnim()) {
			RecSysBgmStop();
			break;
		}

		/* キー入力 */
		InputAllKeyHold();
		if (AutoFlag == 1) {
			switch (GetKeyPushOnce()) {
			case KEY_INPUT_SPACE:
				StopFrag *= -1;
				if (StopFrag == 1) { RecSysBgmStop(); }
				else {
					RecSysBgmSetCurrentPosition((int)((double)recfp.time.now / 1000.0 * 44100.0));
					RecSysBgmPlay(true, false, false);
				}
				break;
			default:
				break;
			}
		}
		if (StopFrag == 1) {
			switch (GetKeyPushOnce()) {
			case KEY_INPUT_LEFT:
				recfp.time.now = maxs_2(recfp.time.now - 10000, 0);
				RecResetPlayObjectNum(objectN, &recfp);
				RecResetPlayRecfpMapeffNum(&recfp.mapeff);
				RecResetPlayPartNum(&itemN, &SitemN, LineMoveN, lockN, &viewTN, &MovieN);
				for (i[0] = 0; i[0] < 3; i[0]++) {
					while (recfp.mapdata.note[objectN[i[0]]].hittime < recfp.time.now &&
						recfp.mapdata.note[objectN[i[0]]].next != 5999)
					{
						objectN[i[0]] = recfp.mapdata.note[objectN[i[0]]].next;
					}
					objectNG[i[0]] = objectN[i[0]];
				}
				break;
			case KEY_INPUT_RIGHT:
				recfp.time.now += 10000;
				for (i[0] = 0; i[0] < 3; i[0]++) {
					while (recfp.mapdata.note[objectN[i[0]]].hittime < recfp.time.now &&
						recfp.mapdata.note[objectN[i[0]]].next != 5999)
					{
						objectN[i[0]] = recfp.mapdata.note[objectN[i[0]]].next;
					}
					objectNG[i[0]] = objectN[i[0]];
				}
				break;
			default:
				break;
			}
		}

		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			RecSysBgmStop();
			return SCENE_SERECT;
		}
		WaitTimer(WAIT_TIME_ON_GAMELOOP);
		if (StopFrag == -1) {
			recfp.time.now = GetNowCount() - Stime + optiondata.offset * 5;
		}
		else {
			Stime = GetNowCount() - recfp.time.now + optiondata.offset * 5;
		}
		ScreenFlip();
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
	TCHAR mapPath[255];
	TCHAR fileName[255];
	rec_map_detail_t map_detail;
	rec_play_userpal_t userpal;
	rec_play_nameset_t nameset;
	now_scene_t ret = SCENE_EXIT;
	FILE *fp = NULL;

	/* rrsデータが無い、または作成の指示があれば作る */
	if (shift == 0) {
		RecGetMusicMapRrsPath(mapPath, 255, packNo, musicNo, (rec_dif_t)difNo);
		_wfopen_s(&fp, mapPath, L"rb");
	}

	if (fp == NULL) {
		RecordLoad2(packNo, musicNo, difNo);
		RecGetMusicMapRrsPath(mapPath, 255, packNo, musicNo, (rec_dif_t)difNo);
		cal_ddif_3(mapPath);
	}
	else {
		fclose(fp);
	}

	ret = RecPlayMain(&map_detail, &userpal, &nameset, fileName, packNo, musicNo, difNo, AutoFlag);

	if (ret != SCENE_RESULT) { return ret; }
	else { return result(&map_detail, &userpal, &nameset, (rec_dif_t)difNo, fileName); }
}
