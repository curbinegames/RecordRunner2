
/* TODO: アイテム表示バグが起こってる!!! */
/* TODO: ddifがバグってる!!! */
/* TODO: <全体>cutinを最新にする */
/* TODO: <全体>L"str"を_T("str")に変える" */

#if 1 /* define group */

/* include */

#include <sancur.h>
#include <strcur.h>
#include <dxdraw.h>
#include <dxcur.h>
#include <system.h>
#include <RecScoreFile.h>
#include <RecordLoad2.h>
#include <result.h>
#include <recr_cutin.h>
#include <dxlibcur.h>
#include <RecSystem.h>
#include <recp_cal_ddif_2.h>

#include <playbox.h>
#include <PlayBonus.h>
#include <PlayAuto.h>
#include <PlayCamera.h>
#include <PlayHitEff.h>
#include <PlayNoteJudge.h>
#include <PlayViewJudge.h>

/* define */

#define MODE_CHANGE 1

#define CHARA_POS_UP 0
#define CHARA_POS_MID 1
#define CHARA_POS_DOWN 2

/* debug */
#if 1
#define RECR_DEBUG(ofs, data)											\
		RecRescaleDrawFormatString(20, 120 + ofs * 20, Cr, L#data": %d", data)
#define RECR_DEBUG_LOOP(ofs, n, data_a, data_b)							\
	for (int _rep = 0; _rep < n; _rep++) {								\
		RecRescaleDrawFormatString(20, 120 + _rep * 20 + ofs * 20, Cr,			\
		L#data_a"[%d]"#data_b": %d", _rep, data_a[_rep]data_b);			\
	}
#else
#define RECR_DEBUG(n, data_a, data_b)
#define RECR_DEBUG_LOOP(n, data_a, data_b)
#endif

#endif /* define group */

#if 1 /* typedef group */

/* struct */
typedef struct rec_play_back_pic_s {
	int sky = 0;
	int ground = 0;
	int water = 0;
} rec_play_back_pic_t;

#endif /* typedef group */

static int RecPlayDebug[3] = { 0,0,0 };

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

static void RecResetPlayObjectNum(short int objectN[], const rec_score_file_t *recfp) {
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

static struct score_box GetScore3(struct score_box score,
	struct judge_box judge, const int notes, const int MaxCombo)
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
static void cal_back_x(int *xpos, rec_map_eff_data_t *mapeff, short int speedN[], int cam) {
	const double scrool = mapeff->scrool.data[mapeff->scrool.num].speed;

	xpos[0] -= (int)(100 * mapeff->speedt[3][speedN[3]][1] * scrool);
	while (xpos[0] + 100 * cam / 5 > 0)      { xpos[0] -= 64000; }
	while (xpos[0] + 100 * cam / 5 < -64000) { xpos[0] += 64000; }

	xpos[1] -= (int)(500 * mapeff->speedt[4][speedN[4]][1] * scrool);
	while (xpos[1] + 100 * cam > 0)      { xpos[1] -= 64000; }
	while (xpos[1] + 100 * cam < -64000) { xpos[1] += 64000; }

	xpos[2] = xpos[1];
	return;
}

static int GetCharaPos3(int time, note_box_2_t note[], short int No[],
	rec_play_key_hold_t *keyhold, rec_play_chara_hit_attack_t *hitatk)
{
	int ans = CHARA_POS_MID;
	// near catch/bomb
	for (int i = 0; i < 3; i++) {
		if (note[No[i]].hittime <= time + 40 &&
			(note[No[i]].object == NOTE_CATCH ||
				note[No[i]].object == NOTE_BOMB) &&
			keyhold->up == 0 && keyhold->down == 0)
		{
			return CHARA_POS_MID;
		}
	}
	// hit note
	if (keyhold->up != 1 && keyhold->down != 1 &&
		keyhold->left != 1 && keyhold->right != 1 && hitatk->time != -1000)
	{
		return hitatk->pos;
	}
	// push up
	if (1 <= keyhold->up && 0 == keyhold->down) { ans = CHARA_POS_UP; }
	// push down
	else if (0 == keyhold->up && 1 <= keyhold->down) { ans = CHARA_POS_DOWN; }
	// push up and down
	else if (1 <= keyhold->up && 1 <= keyhold->down) { ans = CHARA_POS_MID; }
	// not hit
	else { ans = CHARA_POS_MID; }
	return ans;
}

static void recSetLine(int line[], rec_move_set_t move[], int Ntime, int loop) {
	for (int iLine = 0; iLine < loop; iLine++) {
		if (move[iLine].d[move[iLine].num].Stime <= Ntime &&
			move[iLine].d[move[iLine].num].Stime >= 0)
		{
			line[iLine] =
				(int)movecal(
					move[iLine].d[move[iLine].num].mode,
					move[iLine].d[move[iLine].num].Stime,
					move[iLine].d[move[iLine].num - 1].pos,
					move[iLine].d[move[iLine].num].Etime,
					move[iLine].d[move[iLine].num].pos, Ntime);
		}
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

static void RecPlayRescaleCurve(int x1, int y1, int x2, int y2, uint cr, int thick) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, x1);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y1);
	drawX2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, x2);
	drawY2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y2);
	DrawLine(drawX, drawY, drawX2, drawY2, cr, thick);
	return;
}

static void DrawLineCurve(int x1, int y1, int x2, int y2, char mode,
	unsigned int color, int thick) {
	int end = x1 + 10;
	switch (mode) {
	case 1: // lin
		RecPlayRescaleCurve(x1, y1, x2, y2, color, thick);
		break;
	case 2: // acc
		for (int i = x1; i <= x2; i++) {
			end = maxs(i + 10, x2);
			RecPlayRescaleCurve(i, pals(x1, y1, x2, y2, i),
				end, pals(x1, y1, x2, y2, end),
				color, thick);
		}
		break;
	case 3: // dec
		for (int i = x1; i <= x2; i++) {
			end = maxs(i + 10, x2);
			RecPlayRescaleCurve(i, pals(x2, y2, x1, y1, i),
				end, pals(x2, y2, x1, y1, end),
				color, thick);
		}
		break;
	}
	return;
}

static int cal_nowdif_p(int *ddif, rec_play_time_set_t *time) {
	int ret = 0;
	int sect = 0;
	int stime = 0;
	if (time->now - time->offset <= 0) {
		ret = ddif[0];
	}
	else if (time->now - time->end >= 0) {
		ret = ddif[24];
	}
	else {
		sect = (time->now - time->offset) * 24 / (time->end - time->offset);
		stime = (time->now - time->offset) % ((time->end - time->offset) / 24);
		ret = lins(0, ddif[sect], (time->end - time->offset) / 24, ddif[sect + 1], stime);
	}
	ret = lins(379 * 50, 100, 34733 * 50, 800, ret);
	return ret;
}

static void PlayDrawItem(rec_map_eff_data_t *mapeff,
	short int MovieN, int Ntime, int Xmidline, int item[])
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
		if (pMovie->starttime <= Ntime && pMovie->endtime >= Ntime) {
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
			DrawDeformationPic(drawX, drawY, drawS / 100.0, drawS / 100.0, drawR, item[pMovie->ID]);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
	}
}

static void RecPlayCalUserPal(rec_play_userpal_t *userpal, short notes, rec_play_time_set_t *time, int AutoFg) {
	userpal->Mcombo = mins(userpal->Mcombo, userpal->Ncombo);
	//ライフが0未満の時、1毎に減点スコアを20増やす。
	if (userpal->life < 0) {
		userpal->score.loss = maxs(userpal->score.loss - userpal->life * 20, userpal->score.normal + userpal->score.combo);
		userpal->life = 0;
	}
	//ライフ上限
	userpal->life = maxs(userpal->life, 500);
	//スコア計算
	userpal->score = GetScore3(userpal->score, userpal->judgeCount, notes, userpal->Mcombo);
	//ステータス
	if (userpal->life <= 0 && userpal->status == REC_PLAY_STATUS_PLAYING && AutoFg == 0) {
		userpal->status = REC_PLAY_STATUS_DROPED;
		userpal->Dscore.add_save = userpal->Dscore.add;
		userpal->Dscore.dis_save = mins(time->now - time->offset, 0);
	}
	else if (userpal->status != REC_PLAY_STATUS_DROPED &&
		mins(time->now - time->offset, 0) > time->end - time->offset)
	{ //CLEARED
		userpal->status = REC_PLAY_STATUS_CLEARED;
	}
	//距離計算
	switch (userpal->status) {
	case REC_PLAY_STATUS_PLAYING:
		userpal->Dscore.now_dis = mins(time->now - time->offset, 0);
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

static void RecPlayGetMapFileNames(TCHAR *songPath, TCHAR *songName,
	int packNo, int musicNo, int LvNo)
{
	int fd = -1;
	TCHAR packName[255];
	TCHAR buf[255];

	/* パックの名前を取得する */
	fd = FileRead_open(L"RecordPack.txt");
	for (int i = 0; i <= packNo; i++) { FileRead_gets(packName, 256, fd); }
	FileRead_close(fd);

	strcopy_2(L"record/", songPath, 255); /* songPath = record/ */
	strcats_2(songPath, 255, packName);   /* songPath = record/<パック名> */
	stradds_2(songPath, 255, L'/');       /* songPath = record/<パック名>/ */

	strcopy_2(songPath, buf, 255);    /* buf = record/<パック名>/ */
	strcats_2(buf, 255, L"list.txt"); /* buf = record/<パック名>/list.txt */

	/* 曲名を取得する */
	fd = FileRead_open(buf);
	for (int i = 0; i <= musicNo; i++) FileRead_gets(songName, 256, fd); /* songName = <曲名> */
	FileRead_close(fd);

	strcats_2(songPath, 255, songName); /* songPath = record/<パック名>/<曲名> */
	stradds_2(songPath, 255, L'/');     /* songPath = record/<パック名>/<曲名>/ */

	return;
}

static int GetRemainNotes(struct judge_box judge, int Notes) {
	return Notes - judge.just - judge.good - judge.safe - judge.miss;
}

static int GetHighScore(const TCHAR *songName, int dif) {
	FILE *fp;
	int buf[7] = { 0,0,0,0,0,0,0 };
	TCHAR DataFN[255] = _T("score/");

	strcats(DataFN, songName);
	strcats(DataFN, _T(".dat"));

	_wfopen_s(&fp, DataFN, _T("rb"));
	if (fp != NULL) {
		fread(&buf, sizeof(int), 6, fp);
		fclose(fp);
	}

	return buf[dif];
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
	short speedN, int Ntime)
{
	int ret = 0;
	double sppedt_temp[99];
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
	for (int i = 0; i < 99; i++) {
		sppedt_temp[i] = mapeff->speedt[iLine][0][i * 2];
	}
	while (note->hittime >= sppedt_temp[speedN + *SpeedNoAdd + 1] &&
		sppedt_temp[speedN + *SpeedNoAdd + 1] >= 0) {
		(*SpeedNoAdd)++;
	}
	return 0;
}

static void CalPalCrawNote(int *DrawX, int *DrawY, int *DrawC, int lock0, int lock1,
	note_box_2_t *note, int Xline, int Yline, double speedt, rec_scrool_set_t *scrool, int Ntime)
{
	const double scroolSpeed = scrool->data[scrool->num].speed;
	const double scroolBaseTime = scrool->data[scrool->num].basetime;

	//縦位置
	*DrawY = ((lock1 == 1) ? note->ypos : Yline);
	//横位置
	*DrawX = (int)((speedt * 20 * (note->viewtime -
		(scroolSpeed * Ntime + scroolBaseTime)) + 5000) / 50) + 50;
	*DrawX += ((lock0 == 1) ? note->xpos - 150 : Xline - 150);
	//色
	*DrawC = note->color;
}

/**
 * return 0 = normal, 1 = continue, 2 = break;
 */
static int DrawNoteOne(int *viewTadd, int *XLockNoAdd, int *YLockNoAdd, int *SpeedNoAdd,
	note_box_2_t *note, rec_map_eff_data_t *mapeff, short viewTN, short lockN[], int iLine,
	short speedN, int Ntime, int Xline, int Yline, struct note_img *noteimg)
{
	int ret = 0;
	int DrawX = 0;
	int DrawY = 0;
	int DrawC = 0;
	int DrawID = 0;
	ret = StepNoDrawNote(viewTadd, XLockNoAdd, YLockNoAdd, SpeedNoAdd,
		note, mapeff, viewTN, lockN, iLine, speedN, Ntime);
	if (ret == 1) { return 1; }
	else if (ret == 2) { return 2; }
	CalPalCrawNote(&DrawX, &DrawY, &DrawC, mapeff->lock[0][0][lockN[0] + *XLockNoAdd],
		mapeff->lock[1][0][lockN[1] + *YLockNoAdd], note, Xline, Yline,
		mapeff->speedt[iLine][0][(speedN + *SpeedNoAdd) * 2 + 1], &mapeff->scrool, Ntime);
	switch (note->object) {
	case 1:
	case 3:
	case 4:
	case 5:
	case 6:
		DrawGraphRecField(DrawX, DrawY, noteimg->notebase);
		break;
	}
	switch (note->object) {
	case 1:
		DrawID = noteimg->hitcircle[DrawC];
		break;
	case 2:
		DrawID = noteimg->catchi;
		break;
	case 3:
		DrawID = noteimg->up;
		break;
	case 4:
		DrawID = noteimg->down;
		break;
	case 5:
		DrawID = noteimg->left;
		break;
	case 6:
		DrawID = noteimg->right;
		break;
	case 7:
		DrawID = noteimg->bomb;
		break;
	case 8:
		DrawID = noteimg->goust;
		break;
	}
	DrawGraphRecField(DrawX, DrawY, DrawID);
	return 0;
}

static void RecPlayDrawNoteAll(short int objectN[], note_box_2_t note[],
	rec_map_eff_data_t *mapeff, short int viewTN, short int *lockN, short int speedN[],
	int Ntime, int Xline[], int Yline[], struct note_img *noteimg)
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
			ret = DrawNoteOne(&viewTadd, &XLockNoAdd, &YLockNoAdd, &SpeedNoAdd,
				&note[iNote], mapeff, viewTN, lockN, iLine, speedN[iLine],
				Ntime, Xline[iLine], Yline[iLine], noteimg);
			if (ret == 1) { continue; }
			else if (ret == 2) { break; }
			if (note[iNote].next == -1) { break; }
		}
	}
	return;
}

#endif /* Notes Picture */

#if 1 /* Back Ground */

static void DrawFallBack(int Yline, int item[], rec_fall_data_t *falleff) {
	static int baseX = 0;
	static int baseY = 0;
	for (int ix = 0; ix < 2; ix++) {
		for (int iy = 0; iy < 3; iy++) {
			RecRescaleDrawGraph(baseX + ix * 640, baseY + Yline - iy * 480,
				item[falleff->d[falleff->num].No], TRUE);
		}
	}
	baseX -= 5;
	baseY += 2;
	if (baseX <= -640) { baseX += 640; }
	if (baseY >= 640) { baseY -= 480; }
}

static void PlayDrawBackGround(rec_map_eff_data_t *mapeff, short int *speedN,
	int Yline[], rec_play_back_pic_t *backpic, int *item)
{
	static int bgp[3] = { 0,0,0 };
	int camX = 0;
	RecPlayGetCameraPos(&camX, NULL);
	cal_back_x(bgp, mapeff, speedN, camX);
	//draw background picture
	for (int loop = bgp[0] / 100; loop + camX / 5 < 70000; loop += 640) {
		DrawGraphRecBackField(loop, Yline[3] / 5 - 160, backpic->sky);
	}
	for (int loop = bgp[1] / 100; loop + camX < 70000; loop += 640) {
		DrawGraphRecField(loop, Yline[3] - 400, backpic->ground);
		DrawGraphRecField(loop, Yline[4] - 400, backpic->water);
	}
	//落ち物背景表示
	if (mapeff->fall.d[mapeff->fall.num].No >= 0) {
		DrawFallBack(Yline[3], item, &mapeff->fall);
	}
	return;
}

#endif /* Back Ground */

#if 1 /* Guide Line */

int PlayShowGuideLine(int Ntime, int Line,
	rec_move_set_t Ymove[], int Xline[], int Yline[], int iDraw)
{
	int drawLeft = 0;
	int drawRight = 0;
	int drawY1 = 0;
	int drawY2 = 0;
	int drawC = 0;
	rec_play_xy_set_t camera;
	RecPlayGetCameraPos(&camera.x, &camera.y);

	// color code
	switch (Line) {
	case 0:
		drawC = 0xffff0000;
		break;
	case 1:
		drawC = 0xff00ff00;
		break;
	case 2:
		drawC = 0xff0000ff;
		break;
	}
	if (Ymove[Line].d[iDraw].Stime < 0) {
		drawLeft = (Ymove[Line].d[iDraw - 1].Etime - Ntime) / 2.1 + Xline[Line] + 15 + camera.x;
		drawRight = 640;
		drawY1 = Ymove[Line].d[iDraw - 1].pos + 15 + camera.y;
		drawY2 = Ymove[Line].d[iDraw - 1].pos + 15 + camera.y;
		RecRescaleDrawLine(drawLeft, drawY1, drawRight, drawY2, drawC, 2);
		return 1;
	}
	// cal Xpos1
	if (iDraw < 1) {
		drawLeft = Xline[Line] + Xline[Line] + 15;
		drawRight = (Ymove[Line].d[iDraw].Stime - Ntime) / 2.1 + Xline[Line] + 15;
		drawY1 = Yline[Line] + 15;
		drawY2 = Yline[Line] + 15;
		DrawLineRecField(drawLeft, drawY1, drawRight, drawY2, drawC, 2);
	}
	else if (Ntime < Ymove[Line].d[iDraw].Etime) {
		drawLeft = (Ymove[Line].d[iDraw - 1].Etime - Ntime) / 2.1 + Xline[Line] + 15;
		drawRight = (Ymove[Line].d[iDraw].Stime - Ntime) / 2.1 + Xline[Line] + 15;
		drawY1 = Ymove[Line].d[iDraw - 1].pos + 15;
		drawY2 = Ymove[Line].d[iDraw - 1].pos + 15;
		DrawLineRecField(drawLeft, drawY1, drawRight, drawY2, drawC, 2);
	}
	drawLeft = (Ymove[Line].d[iDraw].Stime - Ntime) / 2.1 + Xline[Line] + 15 + camera.x;
	if (960 < drawLeft) {
		return 1;
	}
	drawRight = (Ymove[Line].d[iDraw].Etime - Ntime) / 2.1 + Xline[Line] + 15 + camera.x;
	drawY1 = Ymove[Line].d[iDraw - 1].pos + 15 + camera.y;
	drawY2 = Ymove[Line].d[iDraw].pos + 15 + camera.y;
	// wiew
	DrawLineCurve(drawLeft, drawY1, drawRight, drawY2, Ymove[Line].d[iDraw].mode, drawC, 2);
	return 0;
}

void PlayShowAllGuideLine(short LineMoveN[], int Ntime,
	rec_move_set_t Ymove[], int Xline[], int Yline[])
{
	int flag = 0;
	for (int iLine = 0; iLine < 3; iLine++) {
		for (int iDraw = LineMoveN[iLine]; 1; iDraw++) {
			flag = PlayShowGuideLine(Ntime, iLine, Ymove, Xline, Yline, iDraw);
			if (flag != 0) { break; }
		}
	}
	return;
}

#endif /* Guide Line */

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
	DxPic_t charaguideimg;
	DxPic_t judghimg;

	void PlayDrawChara(rec_play_key_hold_t *key, int charahit, int Xline[], int Yline[],
		short int charaput, int Ntime, rec_map_eff_data_t *mapeff)
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
		drawY = Yline[charaput] - 75;
		if (charahit > 0) {
			drawX = Xline[charaput] + hitoffset;
			picID = betweens(24 + hitpose * 6,
				(GetNowCount() - charahit) / 125 + 24 + hitpose * 6, 29 + hitpose * 6);
		}
		else {
			drawX = Xline[charaput];

			picID = Ntime * mapeff->v_BPM.data[mapeff->v_BPM.num].BPM /
				20000 % 6 + mapeff->chamo[charaput].gra[mapeff->chamo[charaput].num] * 6;
		}
		if (mapeff->carrow.d[mapeff->carrow.num].data == 1) {
			DrawGraphRecField(drawX - 160, drawY, this->charaimg[picID]);
		}
		else {
			DrawTurnGraphRecField(drawX + 30, drawY, this->charaimg[picID]);
		}
	}

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
		this->charaguideimg = LoadGraph(L"picture/Cguide.png");
		this->judghimg = LoadGraph(L"picture/Marker.png");
	}

	~rec_play_runner_c() {
		for (int i = 0; i < PIC_NUM; i++) {
			DeleteGraph(this->charaimg[i]);
		}
		DeleteGraph(this->charaguideimg);
		DeleteGraph(this->judghimg);
	}

	void ViewRunner(rec_map_eff_data_t *mapeff, rec_play_key_hold_t *keyhold,
		int charaput, int charahit, int Xline[], int Yline[], int Ntime)
	{
		// view chara pos guide
		if (mapeff->carrow.d[mapeff->carrow.num].data == 1) {
			DrawGraphRecField(Xline[charaput] - 4, Yline[charaput] - 4, this->charaguideimg);
		}
		else {
			DrawTurnGraphRecField(Xline[charaput] - 56, Yline[charaput] - 4, this->charaguideimg);
		}
		//判定マーカーの表示
		for (int i = 0; i < 3; i++) {
			DrawGraphRecField(Xline[i], Yline[i], this->judghimg);
		}
		/* キャラ表示 */
		this->PlayDrawChara(keyhold, charahit, Xline, Yline, charaput, Ntime, mapeff);
	}

#undef DIV_X
#undef DIV_Y
#undef PIC_NUM
#undef PIC_SIZE_X
#undef PIC_SIZE_Y
};

static class rec_play_sbar_c {
private:
	DxPic_t baseImg;
	struct {
		DxPic_t Green;
		DxPic_t Yellow;
		DxPic_t Red;
	} Lbarimg;
	DxPic_t Tbarimg[2];
	DxPic_t sbbarimg;

	int CalPosScore(struct score_box score, int RemainNotes, int Notes, int combo, int MaxCombo) {
		int PosCombo = maxs_2(combo + RemainNotes, MaxCombo);
		return score.normal + 90000 * RemainNotes / Notes + 10000 * PosCombo / Notes;
	}

	void RunningStats(struct judge_box judge, int PosScore, int HighScore) {
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

	void ViewScore(score_box score, int Hscore, int time) {
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

	void ViewLife(int life) {
		int DrawX = lins(0, -114, 500, 177, life);

		if (100 < life) {
			RecRescaleDrawGraph(DrawX, 3, this->Lbarimg.Green, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(100, 255, 500, 0, life));
			RecRescaleDrawGraph(DrawX, 3, this->Lbarimg.Yellow, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
		else {
			RecRescaleDrawGraph(DrawX, 3, this->Lbarimg.Red, TRUE);
		}
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
		RecRescaleDrawGraph(DrawX, 38, this->Tbarimg[DrawNo], TRUE);
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
	rec_play_sbar_c() {
		this->baseImg = LoadGraph(_T("picture/scoreber.png"));
		this->Lbarimg.Green = LoadGraph(_T("picture/LIFEbar.png"));
		this->Lbarimg.Yellow = LoadGraph(_T("picture/LIFEbar2.png"));
		this->Lbarimg.Red = LoadGraph(_T("picture/LIFEbar3.png"));
		this->Tbarimg[0] = LoadGraph(_T("picture/TIMEbar.png"));
		this->Tbarimg[1] = LoadGraph(_T("picture/TIMEbar2.png"));
		this->sbbarimg = LoadGraph(_T("picture/scoreber2.png"));
	}

	~rec_play_sbar_c() {
		DeleteGraph(this->baseImg);
		DeleteGraph(this->Lbarimg.Green);
		DeleteGraph(this->Lbarimg.Yellow);
		DeleteGraph(this->Lbarimg.Red);
		DeleteGraph(this->Tbarimg[0]);
		DeleteGraph(this->Tbarimg[1]);
		DeleteGraph(this->sbbarimg);
	}

	void ViewScoreBar(const rec_play_userpal_t *userpal, const rec_play_time_set_t *time,
		const rec_map_detail_t *mapdata, int Hscore, int holdG)
	{
		RecRescaleDrawGraph(0, 0, this->baseImg, TRUE);
		this->ViewScore(userpal->score, Hscore, time->now);
		this->ViewLife(userpal->life);
		this->ViewDist(userpal->status, &userpal->Dscore, time);
		RecRescaleDrawGraph(0, 0, this->sbbarimg, TRUE);
		this->ViewRunStatus(userpal, mapdata->notes, Hscore);
		if (holdG >= 1) {
			this->ViewDdif(mapdata->ddif, mapdata->ddifG[1]);
		}
	}
};

class rec_play_gapbar_c {
private:
	int gapbarimg;
	int gaplineimg;

public:
	rec_play_gapbar_c() {
		this->gapbarimg = LoadGraph(L"picture/GapBer.png");
		this->gaplineimg = LoadGraph(L"picture/GapBerLine.png");
	}

	~rec_play_gapbar_c() {
		DeleteGraph(this->gapbarimg);
		DeleteGraph(this->gaplineimg);
	}

	void ViewGapBar(gap_box *gap) {
		int No = gap->count % 30;

		RecRescaleDrawGraph(219, 460, this->gapbarimg, TRUE);

		for (int i = 0; i < 30; i++) {
			No--;
			if (No < 0) { No += 30; }
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (510 - No * 17) / 2);
			RecRescaleDrawGraph(318 - gap->view[i], 460, this->gaplineimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 225);
		}
	}
};

class rec_play_keyview_c {
private:
	int enable = 0;
	int count[7] = { 0,0,0,0,0,0,0 };
	DxPic_t KeyViewimg[2];

public:
	rec_play_keyview_c(void) {
		this->enable = optiondata.keydetail;
		if (this->enable) {
			this->KeyViewimg[0] = LoadGraph(L"picture/KeyViewOff.png");
			this->KeyViewimg[1] = LoadGraph(L"picture/KeyViewOn.png");
		}
	}

	~rec_play_keyview_c() {
		if (this->enable) {
			DeleteGraph(this->KeyViewimg[0]);
			DeleteGraph(this->KeyViewimg[1]);
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
			RecRescaleDrawGraph(5, 445, this->KeyViewimg[maxs(keyhold->z, 1)], REC_RESCALE_BOTTOM_LEFT);
			RecRescaleDrawGraph(40, 445, this->KeyViewimg[maxs(keyhold->x, 1)], REC_RESCALE_BOTTOM_LEFT);
			RecRescaleDrawGraph(75, 445, this->KeyViewimg[maxs(keyhold->c, 1)], REC_RESCALE_BOTTOM_LEFT);
			RecRescaleDrawGraph(570, 410, this->KeyViewimg[maxs(keyhold->up, 1)], REC_RESCALE_BOTTOM_RIGHT);
			RecRescaleDrawGraph(570, 445, this->KeyViewimg[maxs(keyhold->down, 1)], REC_RESCALE_BOTTOM_RIGHT);
			RecRescaleDrawGraph(535, 445, this->KeyViewimg[maxs(keyhold->left, 1)], REC_RESCALE_BOTTOM_RIGHT);
			RecRescaleDrawGraph(605, 445, this->KeyViewimg[maxs(keyhold->right, 1)], REC_RESCALE_BOTTOM_RIGHT);
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
	short int i[3];
	short int charaput = 1; //キャラの今の位置[0で上,1で中,2で下]

	/* int */
	int charahit = 0; //キャラがノーツをたたいた後であるかどうか。[1以上で叩いた、0で叩いてない]
	int G[20];
	int holdG = 0;
	int key2 = 1;
	int key3 = 1;
	int AllNotesHitTime = -1;
	int LaneTrack[3] = { -150,-150,-150 };
	int StopFrag = -1;
	int HighScore; //ハイスコア
	rec_play_chara_hit_attack_t hitatk;
	int fps[62];//0～59=1フレーム間隔の時間,60=次の代入先,61=前回の時間
	short LineMoveN[3] = { 0,0,0 }; //↑のライン表示番号
	short int lockN[2] = { 0,0 }; //↑の番号
	short int viewTN = 0;
	int Yline[5] = { 300,350,400,350,600 };//[上,中,下,地面,水中]レーンの縦位置
	int Xline[3] = { 150,150,150 };//[上,中,下]レーンの横位置
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
	short int MovieN = 0;
	rec_view_bpm_set_t v_BPM;
	short int objectN[3] = { 5999,5999,5999 }; //note number
	short int objectNG[3] = { 0,0,0 }; //note number without ghost note
	rec_score_file_t recfp;
	rec_play_userpal_t userpal;

	/* double */
	double GD[5];
	double SumRate[2] = { 0,0 };
	short int speedN[5] = { 0,0,0,0,0 }; //↑の番号
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
	int item[99]; //アイテムのfd、DrawGraphで呼べる。
	short int itemN = 0; //↑の番号
	int Sitem[99]; //サウンドアイテムのfd
	short int SitemN = 0; //↑の番号
	rec_play_back_pic_t backpic;
	int dangerimg = LoadGraph(L"picture/danger.png");
	int dropimg = LoadGraph(L"picture/drop.png");
	int filterimg = LoadGraph(L"picture/Black.png");
	struct note_img noteimg;
	int musicmp3;

#endif /* num define */

	/* ピクチャの用意 */
	ReadyBonusPsmat();
	ReadyEffPicture();
	ReadyJudgePicture();
	/* adifのリセット */
	InitAdif();
	/* action */
	for (i[0] = 0; i[0] <= 59; i[0]++) { fps[i[0]] = 17; }
	fps[60] = 0;
	fps[61] = 0;

	if (optiondata.SEenable == 0) { RecPlayInitMelodySnd(); }

	RecPlayGetMapFileNames(dataE, ret_fileN, p, n, o);
	strcopy_2(dataE, GT1, 255);                  /* GT1 = record/<パック名>/<曲名>/ */
	stradds_2(GT1, 255, (TCHAR)((int)L'0' + o)); /* GT1 = record/<パック名>/<曲名>/<難易度ナンバー> */
	strcats_2(GT1, 255, L".rrs");                /* GT1 = record/<パック名>/<曲名>/<難易度ナンバー>.rrs */

	/* rrsデータの内容を読み込む */
	if (rec_score_fread(&recfp, GT1) != 0) { return SCENE_EXIT; }

	musicmp3 = LoadSoundMem(recfp.nameset.mp3FN);
	backpic.sky = LoadGraph(recfp.nameset.sky);
	backpic.ground = LoadGraph(recfp.nameset.ground);
	backpic.water = LoadGraph(recfp.nameset.water);

	HighScore = GetHighScore(ret_fileN, o);

	for (i[0] = 0; i[0] < 100; i[0]++) {
		strcopy(dataE, GT1, 1);
		Getxxxpng(&GT2[0], i[0]);
		strcats(GT1, GT2);
		item[i[0]] = LoadGraph(GT1);
		if (item[i[0]] == -1) { break; }
	}
	for (i[0] = 1; i[0] < 100; i[0]++) {
		strcopy(dataE, GT1, 1);
		Getxxxwav(&GT2[0], i[0]);
		strcats(GT1, GT2);
		Sitem[i[0] - 1] = LoadSoundMem(GT1);
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
	PlaySoundMem(musicmp3, DX_PLAYTYPE_BACK);
	WaitTimer(10);
	Stime = GetNowCount();
	cutin.SetIo(0);
	//ゲーム開始
	while (1) {
		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }

		// number step
		for (int iLine = 0; iLine < 3; iLine++) {
			objectNG[iLine] = mins(objectNG[iLine], objectN[iLine]);
			while (recfp.mapdata.note[objectNG[iLine]].object == NOTE_GHOST) {
				objectNG[iLine] = recfp.mapdata.note[objectNG[iLine]].next;
			}
			while (0 <= recfp.mapeff.chamo[iLine].time[recfp.mapeff.chamo[iLine].num + 1] &&
				recfp.mapeff.chamo[iLine].time[recfp.mapeff.chamo[iLine].num + 1] <= recfp.time.now) {
				recfp.mapeff.chamo[iLine].num++;
			}
			while (0 <= recfp.mapeff.speedt[iLine][speedN[iLine] + 1][0] &&
				recfp.mapeff.speedt[iLine][speedN[iLine] + 1][0] <= recfp.time.now) {
				speedN[iLine]++;
			}
		}
		while (-1000 < recfp.mapeff.v_BPM.data[recfp.mapeff.v_BPM.num + 1].time &&
			recfp.mapeff.v_BPM.data[recfp.mapeff.v_BPM.num + 1].time <= recfp.time.now) {
			recfp.mapeff.v_BPM.num++;
		}
		{
			uint *numC = &recfp.mapeff.camera.num;
			while (
				recfp.mapeff.camera.data[*numC].endtime >= 0 &&
				recfp.mapeff.camera.data[*numC].endtime < recfp.time.now)
			{
				(*numC)++;
			}

			numC = &recfp.mapeff.scrool.num;
			while (
				recfp.mapeff.scrool.data[*numC + 1].starttime >= 0 &&
				recfp.mapeff.scrool.data[*numC + 1].starttime <= recfp.time.now)
			{
				(*numC)++;
			}
		}
		if (optiondata.backbright != 0) {
			while (recfp.mapeff.Movie[MovieN].endtime < recfp.time.now &&
				recfp.mapeff.Movie[MovieN].endtime > -500)
			{
				MovieN++;
			}
			while (recfp.mapeff.fall.d[recfp.mapeff.fall.num + 1].time <= recfp.time.now &&
				recfp.mapeff.fall.d[recfp.mapeff.fall.num + 1].time >= 0)
			{
				recfp.mapeff.fall.num++;
			}
			if (recfp.mapeff.speedt[3][speedN[3] + 1][0] < recfp.time.now &&
				recfp.mapeff.speedt[3][speedN[3] + 1][0] >= 0)
			{
				speedN[3]++;
			}
		}
		if (AutoFlag == 1) {
			for (i[0] = 0; i[0] < 3; i[0]++) {
				while ((0 <= recfp.mapeff.move.y[i[0]].d[LineMoveN[i[0]]].Stime &&
					recfp.mapeff.move.y[i[0]].d[LineMoveN[i[0]]].Etime <= recfp.time.now) ||
					recfp.mapeff.move.y[i[0]].d[LineMoveN[i[0]]].mode == 4) {
					LineMoveN[i[0]]++;
				}
			}
		}
		while (0 <= recfp.mapeff.carrow.d[recfp.mapeff.carrow.num + 1].time &&
			recfp.mapeff.carrow.d[recfp.mapeff.carrow.num + 1].time < recfp.time.now) {
			recfp.mapeff.carrow.num++;
		}
		for (i[0] = 0; i[0] < 2; i[0]++) {
			while (0 <= recfp.mapeff.lock[i[0]][1][lockN[i[0]] + 1] &&
				recfp.mapeff.lock[i[0]][1][lockN[i[0]] + 1] <= recfp.time.now) {
				lockN[i[0]]++;
			}
		}
		while (0 <= recfp.mapeff.viewT[0][viewTN + 1] &&
			recfp.mapeff.viewT[0][viewTN + 1] <= recfp.time.now) {
			viewTN++;
		}

		//キー設定
		GetHitKeyStateAll(key);
		if (AutoFlag == 0) {
			if (key[KEY_INPUT_Z] == 0) keyhold.z = 0;
			else if (key[KEY_INPUT_Z] == 1) keyhold.z++;
			if (key[KEY_INPUT_X] == 0) keyhold.x = 0;
			else if (key[KEY_INPUT_X] == 1) keyhold.x++;
			if (key[KEY_INPUT_C] == 0) keyhold.c = 0;
			else if (key[KEY_INPUT_C] == 1) keyhold.c++;
			if (key[KEY_INPUT_UP] == 0) keyhold.up = 0;
			else if (key[KEY_INPUT_UP] == 1) keyhold.up++;
			if (key[KEY_INPUT_LEFT] == 0) keyhold.left = 0;
			else if (key[KEY_INPUT_LEFT] == 1) keyhold.left++;
			if (key[KEY_INPUT_RIGHT] == 0) keyhold.right = 0;
			else if (key[KEY_INPUT_RIGHT] == 1) keyhold.right++;
			if (key[KEY_INPUT_DOWN] == 0) keyhold.down = 0;
			else if (key[KEY_INPUT_DOWN] == 1) keyhold.down++;
		}
		//オートプレイ用コード
		else if (AutoFlag == 1) {
			if (key[KEY_INPUT_G] == 0) { holdG = 0; }
			else if (key[KEY_INPUT_G] == 1) { holdG++; }
			AutoAution(&keyhold, recfp.mapdata.note, objectNG, recfp.time.now);
		}

		//カメラ移動
		RecPlaySetCamera(&recfp.mapeff.camera, recfp.time.now);
		//Xline(横位置)の計算
		recSetLine(Xline, recfp.mapeff.move.x, recfp.time.now, 3);
		//Yline(縦位置)の計算
		if (optiondata.backbright == 0) {
			recSetLine(Yline, recfp.mapeff.move.y, recfp.time.now, 3);
		}
		else {
			recSetLine(Yline, recfp.mapeff.move.y, recfp.time.now, 5);
		}
		charaput = GetCharaPos3(recfp.time.now, recfp.mapdata.note, objectNG, &keyhold, &hitatk);
		if ((GetNowCount() - charahit > 50) &&
			(keyhold.up == 1 || keyhold.down == 1 ||
			keyhold.left == 1 || keyhold.right == 1))
		{
			charahit = 0;
		}
		//キー押しヒット解除
		if (1 == keyhold.up || 1 == keyhold.down || 1 == keyhold.left || 1 == keyhold.right || hitatk.time + 750 < recfp.time.now) {
			hitatk.time = -1000;
		}
		//キャッチ判定に使う数値を計算
		LaneTrack[charaput] = recfp.time.now;
		if (keyhold.up == 0 && keyhold.down == 0 || keyhold.up > 0 && keyhold.down > 0) { LaneTrack[1] = recfp.time.now; }
		else if (keyhold.up > 0 && keyhold.down == 0) { LaneTrack[0] = recfp.time.now; }
		else if (keyhold.up == 0 && keyhold.down > 0) { LaneTrack[2] = recfp.time.now; }
		if (LaneTrack[0] <= LaneTrack[2]) { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[0]); }
		else { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[2]); }
		//ヒット
		if (keyhold.z == 1 || keyhold.x == 1 || keyhold.c == 1) { charahit = GetNowCount(); }
		if (charahit + 750 < GetNowCount()) { charahit = 0; }
		/* ノーツ判定 */
		RecJudgeAllNotes(recfp.mapdata.note, objectN, recfp.time.now, Sitem,
			&keyhold, &hitatk, LaneTrack, &charahit, charaput, &userpal, &p_sound);
		RecPlayCalUserPal(&userpal, recfp.mapdata.notes, &recfp.time, AutoFlag);

		ClearDrawScreen(); /* 描画エリアここから */
		//背景表示
		if (optiondata.backbright != 0) {
			PlayDrawBackGround(&recfp.mapeff, speedN, Yline, &backpic, item);
		}
		//フィルター表示
		switch (optiondata.backbright) {
		case 1:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 170);
			RecRescaleDrawGraph(0, 0, filterimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			break;
		case 2:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 85);
			RecRescaleDrawGraph(0, 0, filterimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			break;
		}
		//アイテム表示
		if (optiondata.backbright != 0) {
			PlayDrawItem(&recfp.mapeff, MovieN, recfp.time.now, Xline[1], item);
		}
		// view line
		if (AutoFlag == 1) {
			PlayShowAllGuideLine(LineMoveN, recfp.time.now, recfp.mapeff.move.y, Xline, Yline);
		}
		/* キャラ周り表示 */
		runnerClass.ViewRunner(&recfp.mapeff, &keyhold, charaput, charahit, Xline, Yline, recfp.time.now);
		//コンボ表示
		comboPicClass.ViewCombo(userpal.Ncombo);
		//判定表示
		PlayShowJudge(Xline[charaput], Yline[charaput]);
		/* 音符表示 */
		RecPlayDrawNoteAll(objectN, recfp.mapdata.note, &recfp.mapeff, viewTN,
			lockN, speedN, recfp.time.now, Xline, Yline, &noteimg);
		//ヒットエフェクト表示
		PlayShowHitEffect(Xline, Yline);
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
			RecRescaleDrawFormatString(490, 80, Cr, L"mdif:%.2f", recfp.mapdata.mdif / 100.0);
			RecRescaleDrawFormatString(490, 100, Cr, L"ldif:%.2f", recfp.mapdata.ldif / 100.0);
			RecRescaleDrawFormatString(490, 120, Cr, L"mrat:%.2f", DifRate);
			RecRescaleDrawFormatString(490, 140, Cr, L"ndif:%.2f",
				cal_nowdif_p(recfp.mapdata.ddif, &recfp.time) / 100.0);
			RecRescaleDrawFormatString(490, 160, Cr, L"adif:%.2f",
				(double)lins(0, 0, 453007, 950, GetAdif()) / 100.0);
			max_adif = mins(max_adif, GetAdif());
			RecRescaleDrawFormatString(490, 180, Cr, L"madif:%d", max_adif);
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
			RecRescaleDrawFormatString(20, 80, Cr, L"FPS: %.1f", 60000.0 / notzero(G[0]));
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
		if (userpal.status == REC_PLAY_STATUS_DROPED) { RecRescaleDrawGraph(0, 0, dropimg, TRUE); }
		else if (userpal.life <= 100) { RecRescaleDrawGraph(0, 0, dangerimg, TRUE); }
		//ノーツが全部なくなった瞬間の時間を記録
		if (GetRemainNotes(userpal.judgeCount, recfp.mapdata.notes) == 0 && AllNotesHitTime < 0) {
			AllNotesHitTime = GetNowCount();
		}
		//オートでなく、ノーミス以上を出したら演出
		if (AutoFlag == 0 && AllNotesHitTime + 2000 > GetNowCount()) {
			ShowBonusEff(userpal.judgeCount, AllNotesHitTime);
		}
		//終了時間から5秒以上たって、曲が終了したらカットイン再生。
		if (cutin.IsClosing() == 0 &&
			recfp.time.end + 5000 <= recfp.time.now &&
			(musicmp3 == -1 || CheckSoundMem(musicmp3) == 0)) {
			SetCutTipFg(CUTIN_TIPS_NONE);
			cutin.SetIo(1);
		}
		cutin.DrawCut();

		//カットイン再生から2秒以上たったら抜ける。
		if (cutin.IsEndAnim()) {
			StopSoundMem(musicmp3);
			DeleteSoundMem(musicmp3);
			break;
		}

		/* キー入力 */
		InputAllKeyHold();
		if (AutoFlag == 1) {
			switch (GetKeyPushOnce()) {
			case KEY_INPUT_SPACE:
				StopFrag *= -1;
				if (StopFrag == 1) { StopSoundMem(musicmp3); }
				else {
					SetCurrentPositionSoundMem((int)((double)recfp.time.now / 1000.0 * 44100.0), musicmp3);
					PlaySoundMem(musicmp3, DX_PLAYTYPE_BACK, FALSE);
				}
				break;
			default:
				break;
			}
		}
		if (StopFrag == 1) {
			switch (GetKeyPushOnce()) {
			case KEY_INPUT_LEFT:
				recfp.time.now = mins(recfp.time.now - 10000, 0);
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
			StopSoundMem(musicmp3);
			DeleteSoundMem(musicmp3);
			INIT_PIC();
			return SCENE_SERECT;
		}
		WaitTimer(5);
		if (StopFrag == -1) {
			recfp.time.now = GetNowCount() - Stime + optiondata.offset * 5;
		}
		else {
			Stime = GetNowCount() - recfp.time.now + optiondata.offset * 5;
		}
		ScreenFlip();
	}

	INIT_PIC();

	*ret_map_det = recfp.mapdata;
	*ret_userpal = userpal;
	*ret_nameset = recfp.nameset;

	return SCENE_SERECT;
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
		RecGetMusicPath(mapPath, 255, packNo, musicNo);
		stradds_2(mapPath, 255, (TCHAR)((int)L'0' + difNo));
		strcats_2(mapPath, 255, L".rrs");
		_wfopen_s(&fp, mapPath, L"rb");
	}

	if (fp == NULL) {
		RecordLoad2(packNo, musicNo, difNo);
		RecGetMusicPath(mapPath, 255, packNo, musicNo);
		stradds_2(mapPath, 255, (TCHAR)((int)L'0' + difNo));
		strcats_2(mapPath, 255, L".rrs");
		cal_ddif_3(mapPath);
	}
	else {
		fclose(fp);
	}

	/* TODO: RecPlayMain の返り値を int に変える */
	ret = RecPlayMain(&map_detail, &userpal, &nameset, fileName, packNo, musicNo, difNo, AutoFlag);

	if (ret == SCENE_EXIT) { return SCENE_EXIT; }
	if (AutoFlag == 1) { return SCENE_SERECT; }

	return result(&map_detail, &userpal, &nameset, difNo, fileName);
}
