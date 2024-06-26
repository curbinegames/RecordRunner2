
#if 1 /* define group */

/* include */

#include "RecScoreFile.h"
#include "RecordLoad2.h"
#include "result.h"
#include "playbox.h"
#include "recr_cutin.h"
#include "PlayBonus.h"
#include "PlayHitEff.h"
#include "PlayViewJudge.h"
#include "PlayAuto.h"
#include "dxlibcur.h"
#include "define.h"
#include "RecSystem.h"
#include "PlayCamera.h"

/* define */

#define MODE_CHANGE 1

#define CHARA_POS_UP 0
#define CHARA_POS_MID 1
#define CHARA_POS_DOWN 2
#define SE_HIT (1 << 0)
#define SE_CATCH (1 << 1)
#define SE_ARROW (1 << 2)
#define SE_BOMB (1 << 3)
#define SE_GHOST (1 << 4)
#define SE_SWING (1 << 5)

#define P_JUST_TIME 15
#define JUST_TIME 40
#define GOOD_TIME 70
#define SAFE_TIME 100
#define F_MISS_TIME 200

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

/* enum */

typedef enum chara_pos_e {
	RECR_CHARP_U = 0,
	RECR_CHARP_M,
	RECR_CHARP_D
} chara_pos_t;

/* struct */
typedef struct distance_score_s {
	int add = 0;
	int add_save = 0;
	int now_dis = 0;
	int dis_save = 0;
	int point = 0;
} distance_score_t;

typedef struct rec_play_back_pic_s {
	int sky = 0;
	int ground = 0;
	int water = 0;
} rec_play_back_pic_t;

typedef struct rec_play_chara_hit_attack_s {
	int pos = 1;
	int time = -1000;
} rec_play_chara_hit_attack_t;

#endif /* typedef group */

#if 1 /* proto */

/* proto */

int CheckNearHitNote(
#if SWITCH_NOTE_BOX_2 == 1
	note_box_2_t *const unote, note_box_2_t *const mnote,
	note_box_2_t *const dnote,
#else
	struct note_box *const unote, struct note_box *const mnote,
	struct note_box *const dnote,
#endif
	int Ntime);
int GetHighScore(wchar_t pas[255], int dif);
int GetRemainNotes2(struct judge_box judge, int Notes);
struct score_box GetScore3(struct score_box score, struct judge_box judge,
	const int notes, const int MaxCombo);
void Getxxxpng(wchar_t *str, int num);
void Getxxxwav(wchar_t *str, int num);
int CalPosScore2(struct score_box score, int RemainNotes, int Notes, int combo, int MaxCombo);
void ShowCombo(int combo, int *pic);
void ShowScore2(struct score_box score, int Hscore, int time);
void RunningStats2(struct judge_box judge, int PosScore, int HighScore);
void PlayNoteHitSound2(play_sound_t* const sound);

#endif /* proto */

#if 1 /* sub action */

/* (ret / 100) */
void cal_back_x(int *xpos, rec_map_eff_data_t *mapeff, short int speedN[], int scroolN, int cam) {
	xpos[0] -= (int)(100 * mapeff->speedt[3][speedN[3]][1] * mapeff->scrool[scroolN].speed);
	while (xpos[0] + 100 * cam / 5 > 0) {
		xpos[0] -= 64000;
	}
	while (xpos[0] + 100 * cam / 5 < -64000) {
		xpos[0] += 64000;
	}
	xpos[1] -= (int)(500 * mapeff->speedt[4][speedN[4]][1] * mapeff->scrool[scroolN].speed);
	while (xpos[1] + 100 * cam > 0) {
		xpos[1] -= 64000;
	}
	while (xpos[1] + 100 * cam < -64000) {
		xpos[1] += 64000;
	}
	xpos[2] = xpos[1];
	return;
}

void SetHitPosByHit(rec_play_chara_hit_attack_t *hitatk, char const hitflag, int Ntime) {
	int n = 0;
	int ret = 0;
	for (int i = 0; i < 3; i++) {
		if (hitflag & 1 << i) {
			n++;
			ret = i;
		}
	}
	if (n == 0) { return; }
	if (n >= 2) {
		hitatk->pos = RECR_CHARP_M;
		hitatk->time = Ntime;
		return;
	}
	switch (ret) {
	case 0:
		hitatk->pos = RECR_CHARP_U;
		break;
	case 1:
		hitatk->pos = RECR_CHARP_M;
		break;
	case 2:
		hitatk->pos = RECR_CHARP_D;
		break;
	}
	hitatk->time = Ntime;
	return;
}

int GetCharaPos3(int time, note_box_2_t note[], short int No[],
	rec_play_key_hold_t *keyhold, rec_play_chara_hit_attack_t *hitatk)
{
	int ans = CHARA_POS_MID;
	// near catch/bomb
	for (int i = 0; i < 3; i++) {
		if (note[No[i]].hittime <= time + 40 &&
			(note[No[i]].object == NOTE_CATCH ||
				note[No[i]].object == NOTE_BOMB) &&
			keyhold->up == 0 && keyhold->down == 0) {
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

void recSetLine(int line[], rec_move_set_t move[], int Ntime, int loop) {
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

void DrawLineCurve(int x1, int y1, int x2, int y2, char mode,
	unsigned int color, int thick) {
	int end = x1 + 10;
	switch (mode) {
	case 1: // lin
		RecRescaleDrawLine(x1, y1, x2, y2, color, thick);
		break;
	case 2: // acc
		for (int i = x1; i <= x2; i++) {
			end = maxs(i + 10, x2);
			RecRescaleDrawLine(i, pals(x1, y1, x2, y2, i),
				end, pals(x1, y1, x2, y2, end),
				color, thick);
		}
		break;
	case 3: // dec
		for (int i = x1; i <= x2; i++) {
			end = maxs(i + 10, x2);
			RecRescaleDrawLine(i, pals(x2, y2, x1, y1, i),
				end, pals(x2, y2, x1, y1, end),
				color, thick);
		}
		break;
	}
	return;
}

int cal_nowdif_p(int *ddif, rec_play_time_set_t *time) {
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

void PlayDrawItem(rec_map_eff_data_t *mapeff,
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
			//drawing
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, drawA);
			DrawDeformationPic(drawX, drawY, drawS / 100.0, drawS / 100.0, drawR,
				item[pMovie->ID]);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
	}
}

void PlayDrawChara(rec_play_key_hold_t *key, int charahit, int Xline[], int Yline[],
	short int charaput, int Ntime, rec_map_eff_data_t *mapeff, int pic[])
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
		DrawGraphRecField(drawX - 160, drawY, pic[picID]);
	}
	else {
		DrawTurnGraphRecField(drawX + 30, drawY, pic[picID]);
	}
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
	note_box_2_t *note, int Xline, int Yline, double speedt, struct scrool_box *scrool, int Ntime)
{
	//縦位置
	*DrawY = ((lock1 == 1) ? note->ypos : Yline);
	//横位置
	*DrawX = (int)((speedt * 20 * (note->viewtime -
		(scrool->speed * Ntime + scrool->basetime)) + 5000) / 50) + 50;
	*DrawX += ((lock0 == 1) ? note->xpos - 150 : Xline - 150);
	//色
	*DrawC = note->color;
}

/**
 * return 0 = normal, 1 = continue, 2 = break;
 */
static int DrawNoteOne(int *viewTadd, int *XLockNoAdd, int *YLockNoAdd, int *SpeedNoAdd,
	note_box_2_t *note, rec_map_eff_data_t *mapeff, short viewTN, short lockN[], int iLine,
	short speedN, int Ntime, int Xline, int Yline, int scroolN, struct note_img *noteimg)
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
		mapeff->speedt[iLine][0][(speedN + *SpeedNoAdd) * 2 + 1],
		&mapeff->scrool[scroolN], Ntime);
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

void RecPlayDrawNoteAll(short int objectN[], note_box_2_t note[],
	rec_map_eff_data_t *mapeff, short int viewTN, short int *lockN, short int speedN[],
	int Ntime, int Xline[], int Yline[], int scroolN, struct note_img *noteimg)
{
	int ret = 0;
	int viewTadd = 0;
	int XLockNoAdd = 0;
	int YLockNoAdd = 0;
	int SpeedNoAdd = 0;
	int G[10] = { 0,0,0,0,0,0,0,0,0,0 };

	for (int iLine = 0; iLine < 3; iLine++) {
		viewTadd = 0;
		XLockNoAdd = 0;
		YLockNoAdd = 0;
		SpeedNoAdd = 0;
		for (int iNote = objectN[iLine]; note[iNote].hittime > 0; iNote = note[iNote].next) {
			ret = DrawNoteOne(&viewTadd, &XLockNoAdd, &YLockNoAdd, &SpeedNoAdd,
				&note[iNote], mapeff, viewTN, lockN, iLine, speedN[iLine],
				Ntime, Xline[iLine], Yline[iLine], scroolN, noteimg);
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
	int scroolN, int Yline[], rec_play_back_pic_t *backpic, int *item)
{
	static int bgp[3] = { 0,0,0 };
	int camX = 0;
	RecPlayGetCameraPos(&camX, NULL);
	cal_back_x(bgp, mapeff, speedN, scroolN, camX);
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
	if (640 < drawLeft) {
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

#if 1 /* Notes Judge */

note_judge CheckJudge(int gap) {
	if (-JUST_TIME <= gap && gap <= JUST_TIME) {
		return NOTE_JUDGE_JUST;
	}
	else if (-GOOD_TIME <= gap && gap <= GOOD_TIME) {
		return NOTE_JUDGE_GOOD;
	}
	else if (-SAFE_TIME <= gap && gap <= SAFE_TIME) {
		return NOTE_JUDGE_SAFE;
	}
	else if (gap <= F_MISS_TIME) {
		return NOTE_JUDGE_MISS;
	}
	else {
		return NOTE_JUDGE_NONE;
	}
}

int CheckArrowInJudge(note_material mat, int judge, rec_play_key_hold_t *key) {
	if (CheckJudge(judge) == NOTE_JUDGE_NONE) { return 0; }
	switch (mat) {
	case NOTE_UP:
		if (key->up == 1) { return 1; }
		break;
	case NOTE_DOWN:
		if (key->down == 1) { return 1; }
		break;
	case NOTE_LEFT:
		if (key->left == 1) { return 1; }
		break;
	case NOTE_RIGHT:
		if (key->right == 1) { return 1; }
		break;
	}
	return 0;
}

void AddGap(gap_box* const box, int data){
	box->view[box->count % 30] = data;
	if (box->ssum + data * data < box->ssum) {
		box->sum /= 2;
		box->ssum /= 2;
		box->count /= 2;
	}
	box->sum += data;
	box->ssum += data * data;
	box->count++;
	return;
}

char PlayNoteHitSound(
#if SWITCH_NOTE_BOX_2 == 1
	note_box_2_t note,
#else
	note_box note,
#endif
	int *MelodySnd, int *Sitem, char seflag,
	int notemat) {
	if (note.melody != MELODYSOUND_NONE) {
		PlaySoundMem(MelodySnd[note.melody], DX_PLAYTYPE_BACK);
	}
	else if (note.sound != 0) {
		PlaySoundMem(Sitem[note.sound - 1], DX_PLAYTYPE_BACK);
	}
	else {
		seflag |= notemat;
	}
	return seflag;
}

/**
 * 注) note_judge_event()で判定しているのは、
 * p_just, just, good, safe, fast missのみ。
 * slow miss は別関数で判定している。
 * (そのうち slow miss もこっちで判定するようにしたい)
 */
void note_judge_event(note_judge judge, distance_score_t *Dscore,
#if SWITCH_NOTE_BOX_2 == 1
	note_box_2_t const *const noteinfo,
#else
	note_box const *const noteinfo,
#endif
	int* const Sitem, int Ntime, int Jtime, int lineNo,
	judge_action_box* const judgeA) {
	if (judge == NOTE_JUDGE_NONE) { return; }
	int* const combo = judgeA->combo;
	int* const SoundEnFg = judgeA->soundEnFg;
	gap_box* const gap = judgeA->gap;
	struct judge_box* const judge_b = judgeA->judge;
	int* const life = judgeA->life;
	int* const MelodySnd = judgeA->melody_snd;
	play_sound_t* const sound = judgeA->p_sound;
	struct score_box* const score = judgeA->score;
	note_material note = noteinfo->object;
	PlaySetJudge(judge);
	PlaySetHitEffect(judge, note, lineNo);
	/* 全ノーツ共通 */
	if (judge != NOTE_JUDGE_MISS) {
		score->before = pals(500, score->sum, 0, score->before,
			maxs(Ntime - score->time, 500));
		score->time = Ntime;
	}
	switch (judge) {
	case NOTE_JUDGE_JUST:
		(*combo)++;
		*life += 2;
		(Dscore->add)++;
		(judge_b->just)++;
		break;
	case NOTE_JUDGE_GOOD:
		(*combo)++;
		(*life)++;
		// (Dscore->add) += 0;
		(judge_b->good)++;
		break;
	case NOTE_JUDGE_SAFE:
		// *combo += 0;
		// *life += 0;
		// (Dscore->add) += 0;
		(judge_b->safe)++;
		break;
	case NOTE_JUDGE_MISS:
		*combo = 0;
		*life -= 20;
		// (Dscore->add) += 0;
		(judge_b->miss)++;
		break;
	default:
		/* none */
		break;
	}
	/* ノートごとの処理 */
	switch (note) {
	case NOTE_HIT:
		AddGap(gap, Jtime); // slow miss はやらない
		if (-P_JUST_TIME <= Jtime && Jtime <= P_JUST_TIME) {
			(judge_b->pjust)++;
		}
		if (judge != NOTE_JUDGE_MISS && *SoundEnFg == 0) {
			sound->flag = PlayNoteHitSound(*noteinfo, MelodySnd, Sitem,
				sound->flag, SE_HIT);
		}
		break;
	case NOTE_CATCH:
		if (judge == NOTE_JUDGE_JUST) {
			(judge_b->pjust)++;
			if (*SoundEnFg == 0) {
				sound->flag = PlayNoteHitSound(*noteinfo, MelodySnd, Sitem,
					sound->flag, SE_CATCH);
			}
		}
		break;
	case NOTE_UP:
	case NOTE_DOWN:
	case NOTE_LEFT:
	case NOTE_RIGHT:
		AddGap(gap, Jtime); // slow miss はやらない
		if (-P_JUST_TIME <= Jtime && Jtime <= P_JUST_TIME) {
			(judge_b->pjust)++;
		}
		if (judge != NOTE_JUDGE_MISS && *SoundEnFg == 0) {
			sound->flag = PlayNoteHitSound(*noteinfo, MelodySnd, Sitem,
				sound->flag, SE_ARROW);
		}
		break;
	case NOTE_BOMB:
		switch (judge) {
		case NOTE_JUDGE_JUST:
			(judge_b->pjust)++;
			break;
		case NOTE_JUDGE_MISS:
			if (*SoundEnFg == 0) {
				sound->flag = PlayNoteHitSound(*noteinfo, MelodySnd, Sitem,
					sound->flag, SE_BOMB);
			}
			break;
		default:
			/* nope */
			break;
		}
		break;
	default:
		/* nope */
		break;
	}
	return;
}

void note_judge_while_event(note_material mat, note_box_2_t note[], short int objectN[], int Ntime,
	note_judge judge, distance_score_t *Dscore, int Sitem[], int Line, judge_action_box *judgeA,
	int *charahit, rec_play_chara_hit_attack_t *hitatk, play_sound_t *p_sound, int MelodySnd[])
{
	if ((mat == NOTE_CATCH || mat == NOTE_BOMB) && Dscore == NULL) { return; }
	if ((mat == NOTE_CATCH || mat == NOTE_BOMB) && judgeA == NULL) { return; }
	if (mat == NOTE_CATCH && charahit == NULL) { return; }
	if (mat == NOTE_CATCH && hitatk == NULL) { return; }
	if (mat == NOTE_GHOST && MelodySnd == NULL) { return; }
	if (mat == NOTE_GHOST && p_sound == NULL) { return; }

	switch (mat) {
	case NOTE_CATCH:
		while (note[objectN[Line]].hittime - Ntime <= 0 &&
			0 <= note[objectN[Line]].hittime &&
			note[objectN[Line]].object == NOTE_CATCH)
		{
			note_judge_event(judge, Dscore,
				&note[objectN[Line]], Sitem, Ntime, 0,
				Line, judgeA);
			*charahit = 0;
			hitatk->time = -1000;
			objectN[Line] = note[objectN[Line]].next;
		}
		break;
	case NOTE_BOMB:
		while (note[objectN[Line]].hittime - Ntime <= 0 &&
			0 <= note[objectN[Line]].hittime &&
			note[objectN[Line]].object == NOTE_BOMB)
		{
			note_judge_event(judge, Dscore,
				&note[objectN[Line]], Sitem, Ntime, -JUST_TIME - 1,
				Line, judgeA);
			objectN[Line] = note[objectN[Line]].next;
		}
		break;
	case NOTE_GHOST:
		while (note[objectN[Line]].hittime - Ntime < 0 &&
			note[objectN[Line]].object == NOTE_GHOST) {
			p_sound->flag = PlayNoteHitSound(note[objectN[Line]],
				MelodySnd, Sitem, p_sound->flag, SE_GHOST);
			objectN[Line] = note[objectN[Line]].next;
		}
		break;
	}
	return;
}

#endif /* Notes Judge */

/* main action */
now_scene_t play3(int p, int n, int o, int shift, int AutoFlag) {
#if 1 /* filter3 */
	/*---用語定義-----
	ユーザー用譜面データ: ユーザーが作った譜面データ。ユーザーに分かりやすい。
	PC用譜面データ: ユーザー用譜面データから計算で作られた、PC専用の譜面データ。PCに分かりやすい。
	----------------*/
	//p: パックナンバー
	//n: 曲ナンバー
	//o: 難易度ナンバー
	/* char */
	char hitatk2 = 0; //hit event, bit unit: 0: upper hit, 1: middle hit, 2: lower hit, 3~7: reserved
	char key[256];
	char closeFg = 0;
	/* short */
	short int i[3];
	short int charaput = 1; //キャラの今の位置[0で上,1で中,2で下]
	short int drop = 0;
	short int KeyPushCount[7] = { 0,0,0,0,0,0,0 };
	short int cameraN = 0;
	/* int */
	int charahit = 0; //キャラがノーツをたたいた後であるかどうか。[1以上で叩いた、0で叩いてない]
	int G[20], songT;
	unsigned int UG[5];
	int holdG = 0;
	int key2 = 1;
	int key3 = 1;
	int combo = 0;
	int AllNotesHitTime = -1;
	int LaneTrack[3] = { -150,-150,-150 };
	int Mcombo = 0;
	int judghcount[4] = { 0,0,0,0 };
	int life = 500;
	int ret_gap[3] = { 0,0,0 };
	int StopFrag = -1;
	int scroolN = 0;
	int HighSrore; //ハイスコア
	rec_play_chara_hit_attack_t hitatk;
	int fps[62];//0〜59=1フレーム間隔の時間,60=次の代入先,61=前回の時間
	short LineMoveN[3] = { 0,0,0 }; //↑のライン表示番号
	short int lockN[2] = { 0,0 }; //↑の番号
	short int viewTN = 0;
	int Yline[5] = { 300,350,400,350,600 };//[上,中,下,地面,水中]レーンの縦位置
	int Xline[3] = { 150,150,150 };//[上,中,下]レーンの横位置
	unsigned int Cr = GetColor(255, 255, 255);
	unsigned int Crb = GetColor(0, 0, 0);
	unsigned int CrR = GetColor(255, 0, 0);
	int CutTime = 0;
	int Stime = 0;
	int adif = 0;
	int max_adif = 0;
	/* struct */
	rec_play_key_hold_t keyhold;
	rec_system_t system;
	distance_score_t Dscore;
	play_key_stat_t key_stat;
	gap_box gap2;
	struct judge_box judge;
	struct score_box score;
	short int MovieN = 0;
	rec_view_bpm_set_t v_BPM;
	short int objectN[3] = { 5999,5999,5999 }; //note number
	short int objectNG[3] = { 0,0,0 }; //note number without ghost note
	rec_score_file_t recfp;
	/* double */
	double GD[5];
	double SumRate[2] = { 0,0 };
	short int speedN[5] = { 0,0,0,0,0 }; //↑の番号
	double DifRate; //譜面定数
	/* wchar_t */
	wchar_t fileN[255];
	wchar_t dataE[255] = L"record/";
	wchar_t DataFN[255] = L"score/";
	wchar_t GT1[255];
	wchar_t GT2[255];
	wchar_t ST1[] = L"record/";
	wchar_t ST2[] = L"/list.txt";
	wchar_t ST3[] = L".dat";
	wchar_t GT26[6][7] = { L"/0.rrs" ,L"/1.rrs" ,L"/2.rrs" ,L"/3.rrs" ,L"/4.rrs" ,L"/5.rrs" };
	int item[99]; //アイテムのfd、DrawGraphで呼べる。
	short int itemN = 0; //↑の番号
	int Sitem[99]; //サウンドアイテムのfd
	short int SitemN = 0; //↑の番号
#endif /* filter3 */
	/* グラフィックと効果音 */
	int MelodySnd[24] = {
		LoadSoundMem(L"sound/melody/lowF.wav"),
		LoadSoundMem(L"sound/melody/lowF#.wav"),
		LoadSoundMem(L"sound/melody/lowG.wav"),
		LoadSoundMem(L"sound/melody/lowG#.wav"),
		LoadSoundMem(L"sound/melody/lowA.wav"),
		LoadSoundMem(L"sound/melody/lowA#.wav"),
		LoadSoundMem(L"sound/melody/lowB.wav"),
		LoadSoundMem(L"sound/melody/lowC.wav"),
		LoadSoundMem(L"sound/melody/lowC#.wav"),
		LoadSoundMem(L"sound/melody/lowD.wav"),
		LoadSoundMem(L"sound/melody/lowD#.wav"),
		LoadSoundMem(L"sound/melody/lowE.wav"),
		LoadSoundMem(L"sound/melody/highF.wav"),
		LoadSoundMem(L"sound/melody/highF#.wav"),
		LoadSoundMem(L"sound/melody/highG.wav"),
		LoadSoundMem(L"sound/melody/highG#.wav"),
		LoadSoundMem(L"sound/melody/highA.wav"),
		LoadSoundMem(L"sound/melody/highA#.wav"),
		LoadSoundMem(L"sound/melody/highB.wav"),
		LoadSoundMem(L"sound/melody/highC.wav"),
		LoadSoundMem(L"sound/melody/highC#.wav"),
		LoadSoundMem(L"sound/melody/highD.wav"),
		LoadSoundMem(L"sound/melody/highD#.wav"),
		LoadSoundMem(L"sound/melody/highE.wav")
	};
	int judghimg = LoadGraph(L"picture/Marker.png");
	rec_play_back_pic_t backpic;
	int dangerimg = LoadGraph(L"picture/danger.png");
	int dropimg = LoadGraph(L"picture/drop.png");
	int sbarimg = LoadGraph(L"picture/scoreber.png");
	int sbbarimg = LoadGraph(L"picture/scoreber2.png");
	int filterimg = LoadGraph(L"picture/Black.png");
	int charaguideimg = LoadGraph(L"picture/Cguide.png");
	int gapbarimg = LoadGraph(L"picture/GapBer.png");
	int gaplineimg = LoadGraph(L"picture/GapBerLine.png");
	int Lbarimg[3] = {
		LoadGraph(L"picture/LIFEbar.png"),
		LoadGraph(L"picture/LIFEbar2.png"),
		LoadGraph(L"picture/LIFEbar3.png")
	};
	int Tbarimg[2] = {
		LoadGraph(L"picture/TIMEbar.png"),
		LoadGraph(L"picture/TIMEbar2.png")
	};
	int KeyViewimg[2];
	int Rchaimg;
	int ComboFontimg[10];
	LoadDivGraph(L"picture/NumberComboBlue.png", 10, 5, 2, 80, 100, ComboFontimg);
	struct note_img noteimg;
	play_sound_t p_sound;
	int musicmp3;
#define DIV_X 6
#define DIV_Y 6
#define PIC_NUM (DIV_X * DIV_Y)
#define PIC_SIZE_X 160
#define PIC_SIZE_Y 160
	int	charaimg[PIC_NUM];
	/* ピクチャの用意 */
	ReadyBonusPsmat();
	ReadyEffPicture();
	ReadyJudgePicture();
	/* adifのリセット */
	InitAdif();
	/* address box */
	judge_action_box judgeA;
	judgeA.combo = &combo;
	judgeA.soundEnFg = &system.soundEn;
	judgeA.gap = &gap2;
	judgeA.judge = &judge;
	judgeA.life = &life;
	judgeA.p_sound = &p_sound;
	judgeA.score = &score;
	judgeA.melody_snd = &MelodySnd[0];
	/* FILE */
	FILE *fp = NULL;
	/* action */
	for (i[0] = 0; i[0] <= 59; i[0]++) { fps[i[0]] = 17; }
	fps[60] = 0;
	fps[61] = 0;
	//システムロード
	recSystenLoad(&system);
	if (system.keyViewEn) {
		KeyViewimg[0] = LoadGraph(L"picture/KeyViewOff.png");
		KeyViewimg[1] = LoadGraph(L"picture/KeyViewOn.png");
	}
	switch (system.chara) {
	case 0:
		LoadDivGraph(L"picture/Picker.png",
			PIC_NUM, DIV_X, DIV_Y, PIC_SIZE_X, PIC_SIZE_Y, charaimg);
		Rchaimg = LoadGraph(L"picture/RePicker.png");
		break;
	case 1:
		LoadDivGraph(L"picture/Gator.png",
			PIC_NUM, DIV_X, DIV_Y, PIC_SIZE_X, PIC_SIZE_Y, charaimg);
		Rchaimg = LoadGraph(L"picture/ReGator.png");
		break;
	case 2:
		LoadDivGraph(L"picture/Taylor.png",
			PIC_NUM, DIV_X, DIV_Y, PIC_SIZE_X, PIC_SIZE_Y, charaimg);
		Rchaimg = LoadGraph(L"picture/ReTaylor.png");
		break;
	}
#undef DIV_X
#undef DIV_Y
#undef PIC_NUM
#undef PIC_SIZE_X
#undef PIC_SIZE_Y
	if (system.soundEn == 0) {
		p_sound.att = LoadSoundMem(L"sound/attack.wav");
		p_sound.cat = LoadSoundMem(L"sound/catch.wav");
		p_sound.arw = LoadSoundMem(L"sound/arrow.wav");
		p_sound.swi = LoadSoundMem(L"sound/swing.wav");
		p_sound.bom = LoadSoundMem(L"sound/bomb.wav");
	}
	songT = FileRead_open(L"RecordPack.txt");
	for (i[0] = 0; i[0] <= p; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcats(dataE, GT1);
	stradds(dataE, L'/');
	strcopy(ST1, GT2, 1);
	strcats(GT2, GT1);
	strcats(GT2, ST2);
	songT = FileRead_open(GT2);
	for (i[0] = 0; i[0] <= n; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcopy(GT1, fileN, 1);
	strcats(dataE, GT1);
	strcopy(dataE, GT1, 1);
	strcats(GT1, GT26[o]);
	G[2] = -1;
	if (shift == 0) {
		G[2] = _wfopen_s(&fp, GT1, L"rb");//rrsデータを開く
	}
	if (G[2] != 0) {
		RecordLoad2(p, n, o);//rrsデータが無い、または作成の指示があれば作る
		cal_ddif_3(GT1);//ddif計算
		G[2] = _wfopen_s(&fp, GT1, L"rb");//rrsデータを開く
	}
	/* rrsデータの内容を読み込む */
	if (rec_score_fread(&recfp, fp) != 0) {
		/* 読み込み失敗 */
		return SCENE_EXIT;
	}

	musicmp3 = LoadSoundMem(recfp.nameset.mp3FN);
	backpic.sky = LoadGraph(recfp.nameset.sky);
	backpic.ground = LoadGraph(recfp.nameset.ground);
	backpic.water = LoadGraph(recfp.nameset.water);
	fclose(fp);
	strcats(DataFN, fileN);
	strcats(DataFN, ST3);
	HighSrore = GetHighScore(DataFN, o);
	for (i[0] = 0; i[0] < 100; i[0]++) {
		strcopy(dataE, GT1, 1);
		stradds(GT1, L'/');
		Getxxxpng(&GT2[0], i[0]);
		strcats(GT1, GT2);
		item[i[0]] = LoadGraph(GT1);
		if (item[i[0]] == -1) { break; }
	}
	for (i[0] = 1; i[0] < 100; i[0]++) {
		strcopy(dataE, GT1, 1);
		stradds(GT1, L'/');
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
#if SWITCH_NOTE_BOX_2 == 1
	for (i[0] = 0; i[0] < recfp.allnum.notenum[0] + recfp.allnum.notenum[1] + recfp.allnum.notenum[2]; i[0]++) {
		if (recfp.mapdata.note[i[0]].lane == NOTE_LANE_UP) {
			objectN[0] = i[0];
			break;
		}
	}
	for (i[0] = 0; i[0] < recfp.allnum.notenum[0] + recfp.allnum.notenum[1] + recfp.allnum.notenum[2]; i[0]++) {
		if (recfp.mapdata.note[i[0]].lane == NOTE_LANE_MID) {
			objectN[1] = i[0];
			break;
		}
	}
	for (i[0] = 0; i[0] < recfp.allnum.notenum[0] + recfp.allnum.notenum[1] + recfp.allnum.notenum[2]; i[0]++) {
		if (recfp.mapdata.note[i[0]].lane == NOTE_LANE_LOW) {
			objectN[2] = i[0];
			break;
		}
	}
#endif
	CutTime = GetNowCount();
	PlaySoundMem(musicmp3, DX_PLAYTYPE_BACK);
	WaitTimer(10);
	Stime = GetNowCount();
	//ゲーム開始
	while (1) {
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
		while (0 <= recfp.mapeff.camera[cameraN].endtime &&
			recfp.mapeff.camera[cameraN].endtime < recfp.time.now) {
			cameraN++;
		}
		while (0 <= recfp.mapeff.scrool[scroolN + 1].starttime &&
			recfp.mapeff.scrool[scroolN + 1].starttime <= recfp.time.now) {
			scroolN++;
		}
		if (system.backLight != 0) {
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

		//カメラ移動
		RecPlaySetCamera(recfp.mapeff.camera, cameraN, recfp.time.now);
		//Xline(横位置)の計算
		recSetLine(Xline, recfp.mapeff.move.x, recfp.time.now, 3);
		//Yline(縦位置)の計算
		if (system.backLight == 0) {
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

		ClearDrawScreen();
		//背景表示
		if (system.backLight != 0) {
			PlayDrawBackGround(&recfp.mapeff, speedN, scroolN, Yline, &backpic, item);
		}
		//フィルター表示
		switch (system.backLight) {
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
		if (system.backLight != 0) {
			PlayDrawItem(&recfp.mapeff, MovieN, recfp.time.now, Xline[1], item);
		}
		// view line
		if (AutoFlag == 1) {
			PlayShowAllGuideLine(LineMoveN, recfp.time.now, recfp.mapeff.move.y, Xline, Yline);
		}
		// view chara pos guide
		if (recfp.mapeff.carrow.d[recfp.mapeff.carrow.num].data == 1) {
			DrawGraphRecField(Xline[charaput] - 4, Yline[charaput] - 4, charaguideimg);
		}
		else {
			DrawTurnGraphRecField(Xline[charaput] - 56, Yline[charaput] - 4, charaguideimg);
		}
		//判定マーカーの表示
		for (i[0] = 0; i[0] < 3; i[0]++) {
			DrawGraphRecField(Xline[i[0]], Yline[i[0]], judghimg);
		}
		/* キャラ表示 */
		PlayDrawChara(&keyhold, charahit, Xline, Yline,
			charaput, recfp.time.now, &recfp.mapeff, charaimg);
		//コンボ表示
		ShowCombo(combo, ComboFontimg);
		//判定表示
		PlayShowJudge(system.judgePos, Xline[charaput], Yline[charaput]);
		/* 音符表示 */
		RecPlayDrawNoteAll(objectN, recfp.mapdata.note, &recfp.mapeff, viewTN,
			lockN, speedN, recfp.time.now, Xline, Yline, scroolN, &noteimg);

		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }

		//判定
		//ヒットノーツ
		G[0] = 0;
		if (keyhold.z == 1) { G[0]++; }
		if (keyhold.x == 1) { G[0]++; }
		if (keyhold.c == 1) { G[0]++; }
		hitatk2 = 0;
		for (i[0] = 0; i[0] < G[0]; i[0]++) {
			/* i[0] = 押しボタンループ
			 * G[0] = 押したボタンの数
			 * G[1] = 一番近いHITノーツの位置
			 * G[2] = 一番近いHITノーツのギャップ */
			note_judge NJ = NOTE_JUDGE_JUST;
			G[1] = CheckNearHitNote(&recfp.mapdata.note[objectN[0]],
				&recfp.mapdata.note[objectN[1]], &recfp.mapdata.note[objectN[2]], recfp.time.now);
			if (G[1] == -1) {
				if (i[0] == 0) { p_sound.flag |= SE_SWING; }
				break;
			}
			G[2] = recfp.mapdata.note[objectN[G[1]]].hittime - recfp.time.now;
			hitatk2 |= 1 << G[1];
			NJ = CheckJudge(G[2]);
			if (NJ == NOTE_JUDGE_MISS) { p_sound.flag |= SE_SWING; }
			note_judge_event(NJ, &Dscore, &recfp.mapdata.note[objectN[G[1]]], Sitem, recfp.time.now,
				G[2], G[1], &judgeA);
			objectN[G[1]] = recfp.mapdata.note[objectN[G[1]]].next;
		}
		SetHitPosByHit(&hitatk, hitatk2, recfp.time.now);
		for (int iLine = 0; iLine < 3; iLine++) {
			int GapTime = recfp.mapdata.note[objectN[iLine]].hittime - recfp.time.now;
			switch (recfp.mapdata.note[objectN[iLine]].object) {
			case NOTE_UP:
			case NOTE_DOWN:
			case NOTE_LEFT:
			case NOTE_RIGHT:
				if (CheckArrowInJudge(recfp.mapdata.note[objectN[iLine]].object, GapTime, &keyhold)) {
					note_judge_event(CheckJudge(GapTime), &Dscore,
						&recfp.mapdata.note[objectN[iLine]], Sitem, recfp.time.now, GapTime, iLine,
						&judgeA);
					objectN[iLine] = recfp.mapdata.note[objectN[iLine]].next;
				}
				break;
			case NOTE_CATCH:
				if (LaneTrack[iLine] + SAFE_TIME >= recfp.mapdata.note[objectN[iLine]].hittime) {
					note_judge_while_event(NOTE_CATCH, recfp.mapdata.note, objectN, recfp.time.now, NOTE_JUDGE_JUST,
						&Dscore, Sitem, iLine, &judgeA, &charahit, &hitatk, NULL, NULL);
				}
				break;
			case NOTE_BOMB:
				if (iLine == charaput && GapTime <= 0) {
					note_judge_while_event(NOTE_BOMB, recfp.mapdata.note, objectN, recfp.time.now, NOTE_JUDGE_MISS,
						&Dscore, Sitem, iLine, &judgeA, NULL, NULL, NULL, NULL);
				}
				else {
					note_judge_while_event(NOTE_BOMB, recfp.mapdata.note, objectN, recfp.time.now, NOTE_JUDGE_JUST,
						&Dscore, Sitem, iLine, &judgeA, NULL, NULL, NULL, NULL);
				}
				break;
			case NOTE_GHOST:
				if (GapTime < 0) {
					note_judge_while_event(NOTE_GHOST, recfp.mapdata.note, objectN, recfp.time.now, NOTE_JUDGE_NONE,
						NULL, Sitem, iLine, NULL, NULL, NULL, &p_sound, MelodySnd);
				}
				break;
			}
			//全ノーツslowmiss
			while (GapTime <= -SAFE_TIME && GapTime >= -1000000 &&
				recfp.mapdata.note[objectN[iLine]].object >= NOTE_HIT &&
				recfp.mapdata.note[objectN[iLine]].object <= NOTE_RIGHT)
			{
				note_judge_event(NOTE_JUDGE_MISS, &Dscore, &recfp.mapdata.note[objectN[iLine]],
					Sitem, recfp.time.now, -SAFE_TIME, iLine, &judgeA);
				objectN[iLine] = recfp.mapdata.note[objectN[iLine]].next;
				GapTime = recfp.mapdata.note[objectN[iLine]].hittime - recfp.time.now;
			}
		}
		PlayNoteHitSound2(&p_sound);
		Mcombo = mins(Mcombo, combo);
		//ヒットエフェクト表示
		PlayShowHitEffect(Xline, Yline);
		PlayCheckHitEffect();
		//ライフが0未満の時、1毎に減点スコアを20増やす。
		if (life < 0) {
			score.loss = maxs(score.loss - life * 20, score.normal + score.combo);
			life = 0;
		}
		//ライフ上限
		life = maxs(life, 500);
		//スコア計算
		score = GetScore3(score, judge, recfp.mapdata.notes, Mcombo);
		//距離計算
		if (drop != 0) { //DROPED
			Dscore.now_dis = Dscore.dis_save;
		}
		else if (mins(recfp.time.now - recfp.time.offset, 0) > recfp.time.end - recfp.time.offset) { //CLEARED
			Dscore.now_dis = recfp.time.end - recfp.time.offset;
			Dscore.add_save = Dscore.add;
		}
		else { //PLAYING
			Dscore.now_dis = mins(recfp.time.now - recfp.time.offset, 0);
			Dscore.add_save = Dscore.add;
		}
		//スコアバー表示
		RecRescaleDrawGraph(0, 0, sbarimg, TRUE);
		//スコア表示
		ShowScore2(score, HighSrore, recfp.time.now);
		//ライフ表示
		G[0] = lins(0, -114, 500, 177, life);
		if (life > 100) {
			RecRescaleDrawGraph(G[0], 3, Lbarimg[0], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(100, 255, 500, 0, life));
			RecRescaleDrawGraph(G[0], 3, Lbarimg[1], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
		else {
			RecRescaleDrawGraph(G[0], 3, Lbarimg[2], TRUE);
		}
		RecRescaleDrawFormatString(440, 10, 0xffffffff, L"%3d", life);
		//距離表示
		UG[0] = 0xffffffff;
		G[1] = 0;
		if ((drop == 0) && (mins(recfp.time.now - recfp.time.offset, 0) > recfp.time.end - recfp.time.offset)) {
			G[1] = 1;
			UG[0] = 0xff000000;
		}
		G[0] = (291 * Dscore.now_dis - 136 * recfp.time.end + 136 * recfp.time.offset) / (recfp.time.end - recfp.time.offset);
		GD[0] = Dscore.now_dis / 100000.0;
		RecRescaleDrawGraph(G[0], 38, Tbarimg[G[1]], TRUE);
		RecRescaleDrawFormatString(180, 45, UG[0], L"%.3fkm", GD[0] + Dscore.add_save / 1000.0);
		Dscore.point = (int)(GD[0] * 1000 + Dscore.add_save);
		//スコアバー隠し表示
		RecRescaleDrawGraph(0, 0, sbbarimg, TRUE);
		//ランニングステータス表示
		G[0] = GetRemainNotes2(judge, recfp.mapdata.notes);
		G[1] = CalPosScore2(score, G[0], recfp.mapdata.notes, combo, Mcombo);
		RunningStats2(judge, G[1], HighSrore);
		//部分難易度表示 (only auto mode)
		if (holdG >= 1 && AutoFlag == 1) {
			G[0] = recfp.mapdata.ddif[0] * 20 / notzero(recfp.mapdata.ddifG[1]) + 155;
			G[1] = recfp.mapdata.ddif[24] * 20 / notzero(recfp.mapdata.ddifG[1]) + 447;
			for (i[0] = 0; i[0] <= 23; i[0]++)
				RecRescaleDrawLine((G[0] * (24 - i[0]) + G[1] * i[0]) / 24,
					-recfp.mapdata.ddif[i[0]] * 34 / notzero(recfp.mapdata.ddifG[1]) + 72,
					(G[0] * (23 - i[0]) + G[1] * (1 + i[0])) / 24,
					-recfp.mapdata.ddif[i[0] + 1] * 34 / notzero(recfp.mapdata.ddifG[1]) + 72, Cr);
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
		//判定ずれバー表示
		RecRescaleDrawGraph(219, 460, gapbarimg, TRUE);
		G[0] = gap2.count % 30;
		for (i[0] = 0; i[0] < 30; i[0]++) {
			G[0]--;
			if (G[0] < 0) G[0] += 30;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (510 - G[0] * 17) / 2);
			RecRescaleDrawGraph(318 - gap2.view[i[0]], 460, gaplineimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 225);
		}
		//キー押し状況表示(オプション)
		if (system.keyViewEn) {
			if (keyhold.z == 1) { KeyPushCount[0]++; }
			if (keyhold.x == 1) { KeyPushCount[1]++; }
			if (keyhold.c == 1) { KeyPushCount[2]++; }
			if (keyhold.up == 1) { KeyPushCount[3]++; }
			if (keyhold.down == 1) { KeyPushCount[4]++; }
			if (keyhold.left == 1) { KeyPushCount[5]++; }
			if (keyhold.right == 1) { KeyPushCount[6]++; }
			RecRescaleDrawGraph(5, 445, KeyViewimg[maxs(keyhold.z, 1)], REC_RESCALE_BOTTOM_LEFT);
			RecRescaleDrawGraph(40, 445, KeyViewimg[maxs(keyhold.x, 1)], REC_RESCALE_BOTTOM_LEFT);
			RecRescaleDrawGraph(75, 445, KeyViewimg[maxs(keyhold.c, 1)], REC_RESCALE_BOTTOM_LEFT);
			RecRescaleDrawGraph(570, 410, KeyViewimg[maxs(keyhold.up, 1)], REC_RESCALE_BOTTOM_RIGHT);
			RecRescaleDrawGraph(570, 445, KeyViewimg[maxs(keyhold.down, 1)], REC_RESCALE_BOTTOM_RIGHT);
			RecRescaleDrawGraph(535, 445, KeyViewimg[maxs(keyhold.left, 1)], REC_RESCALE_BOTTOM_RIGHT);
			RecRescaleDrawGraph(605, 445, KeyViewimg[maxs(keyhold.right, 1)], REC_RESCALE_BOTTOM_RIGHT);
			if (KeyPushCount[0] == 0) { RecRescaleAnchorDrawString(10, 450, L"Z", Cr, REC_RESCALE_BOTTOM_LEFT); }
			else { RecRescaleAnchorDrawFormatString(10, 450, Cr, REC_RESCALE_BOTTOM_LEFT, L"%2d", KeyPushCount[0] % 100); }
			if (KeyPushCount[1] == 0) { RecRescaleAnchorDrawString(45, 450, L"X", Cr, REC_RESCALE_BOTTOM_LEFT); }
			else { RecRescaleAnchorDrawFormatString(45, 450, Cr, REC_RESCALE_BOTTOM_LEFT, L"%2d", KeyPushCount[1] % 100); }
			if (KeyPushCount[2] == 0) { RecRescaleAnchorDrawString(80, 450, L"C", Cr, REC_RESCALE_BOTTOM_LEFT); }
			else { RecRescaleAnchorDrawFormatString(80, 450, Cr, REC_RESCALE_BOTTOM_LEFT, L"%2d", KeyPushCount[2] % 100); }
			if (KeyPushCount[3] == 0) { RecRescaleAnchorDrawString(575, 415, L"↑", Cr, REC_RESCALE_BOTTOM_RIGHT); }
			else { RecRescaleAnchorDrawFormatString(575, 415, Cr, REC_RESCALE_BOTTOM_RIGHT, L"%2d", KeyPushCount[3] % 100); }
			if (KeyPushCount[4] == 0) { RecRescaleAnchorDrawString(575, 450, L"↓", Cr, REC_RESCALE_BOTTOM_RIGHT); }
			else { RecRescaleAnchorDrawFormatString(575, 450, Cr, REC_RESCALE_BOTTOM_RIGHT, L"%2d", KeyPushCount[4] % 100); }
			if (KeyPushCount[5] == 0) { RecRescaleAnchorDrawString(540, 450, L"←", Cr, REC_RESCALE_BOTTOM_RIGHT); }
			else { RecRescaleAnchorDrawFormatString(540, 450, Cr, REC_RESCALE_BOTTOM_RIGHT, L"%2d", KeyPushCount[5] % 100); }
			if (KeyPushCount[6] == 0) { RecRescaleAnchorDrawString(610, 450, L"→", Cr, REC_RESCALE_BOTTOM_RIGHT); }
			else { RecRescaleAnchorDrawFormatString(610, 450, Cr, REC_RESCALE_BOTTOM_RIGHT, L"%2d", KeyPushCount[6] % 100); }
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		//デバック
		fps[fps[60]++] = recfp.time.now - fps[61];
		if (fps[60] > 59)fps[60] -= 60;
		fps[61] = recfp.time.now;
		G[0] = 0;
		for (i[0] = 0; i[0] <= 59; i[0]++)G[0] += fps[i[0]];
		if (AutoFlag == 1) {
			RecRescaleDrawFormatString(20, 80, Cr, L"FPS: %.1f", 60000.0 / notzero(G[0]));
			RecRescaleDrawFormatString(20, 100, Cr, L"Autoplay");
		}
		//RECR_DEBUG(0, speedN[0]);
		//RECR_DEBUG(1, speedN[1]);
		//RECR_DEBUG(2, speedN[2]);
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
		//ライフが20%以下の時、危険信号(ピクチャ)を出す
		if (life <= 100 && drop == 0) RecRescaleDrawGraph(0, 0, dangerimg, TRUE);
		//ライフがなくなったらDROPED扱い
		if (life <= 0 && drop == 0 && AutoFlag == 0) {
			drop = 1;
			Dscore.add_save = Dscore.add;
			Dscore.dis_save = mins(recfp.time.now - recfp.time.offset, 0);
		}
		if (drop) { RecRescaleDrawGraph(0, 0, dropimg, TRUE); }
		//ノーツが全部なくなった瞬間の時間を記録
		if (GetRemainNotes2(judge, recfp.mapdata.notes) == 0 && AllNotesHitTime < 0) {
			AllNotesHitTime = GetNowCount();
		}
		//オートでなく、ノーミス以上を出したら演出
		if (AutoFlag == 0 && AllNotesHitTime + 2000 > GetNowCount()) {
			ShowBonusEff(judge, AllNotesHitTime);
		}
		//終了時間から5秒以上たって、曲が終了したらカットイン再生。
		if (closeFg == 0 &&
			recfp.time.end + 5000 <= recfp.time.now &&
			(musicmp3 == -1 || CheckSoundMem(musicmp3) == 0)) {
			SetCutTipFg(CUTIN_TIPS_NONE);
			closeFg = 1;
			CutTime = GetNowCount();
		}
		if (closeFg == 0) {
			ViewCutOut(CutTime);
		}
		if (closeFg == 1) {
			ViewCutIn(CutTime);
		}
		//カットイン再生から2秒以上たったら抜ける。
		if (closeFg == 1 && CutTime + 2000 <= GetNowCount()) {
			StopSoundMem(musicmp3);
			DeleteSoundMem(musicmp3);
			break;
		}
		if (AutoFlag == 1) {
			if (CheckHitKey(KEY_INPUT_SPACE)) {
				if (key2 == 0) {
					StopFrag *= -1;
					if (StopFrag == 1) {
						StopSoundMem(musicmp3);
					}
					else {
						SetCurrentPositionSoundMem((int)((double)recfp.time.now / 1000.0 * 44100.0), musicmp3);
						PlaySoundMem(musicmp3, DX_PLAYTYPE_BACK, FALSE);
					}
				}
				key2 = 1;
			}
			else {
				key2 = 0;
			}
		}
		if (StopFrag == 1) {
			if (CheckHitKey(KEY_INPUT_LEFT) == 1) {
				if (key3 == 0) {
					recfp.time.now = mins(recfp.time.now - 10000, 0);
#if SWITCH_NOTE_BOX_2 == 1
					for (i[0] = 0; i[0] < 3; i[0]++) {
						for (objectN[i[0]] = 0; recfp.mapdata.note[objectN[i[0]]].lane != NOTE_LANE_UP; objectN[i[0]] += 0) {
							objectN[i[0]]++;
						}
						while (recfp.mapdata.note[objectN[i[0]]].hittime < recfp.time.now) {
							objectN[i[0]]++;
						}
					}
#else
					while (recfp.time.now < recfp.mapdata.note2.up[objectN[0]].hittime ||
						recfp.mapdata.note2.up[objectN[0]].hittime < 0) {
						objectN[0]--;
					}
					while (recfp.time.now < recfp.mapdata.note2.mid[objectN[1]].hittime ||
						recfp.mapdata.note2.mid[objectN[1]].hittime < 0) {
						objectN[1]--;
					}
					while (recfp.time.now < recfp.mapdata.note2.low[objectN[2]].hittime ||
						recfp.mapdata.note2.low[objectN[2]].hittime < 0) {
						objectN[2]--;
					}
#endif
					for (i[0] = 0; i[0] < 3; i[0]++) {
						objectNG[i[0]] = objectN[i[0]];
					}
					cameraN = 0;
					scroolN = 0;
					itemN = 0;
					SitemN = 0;
					recfp.mapeff.chamo[0].num = 0;
					recfp.mapeff.chamo[1].num = 0;
					recfp.mapeff.chamo[2].num = 0;
					recfp.mapeff.fall.num = 0;
					recfp.mapeff.move.y[0].num = 0;
					recfp.mapeff.move.y[1].num = 0;
					recfp.mapeff.move.y[2].num = 0;
					recfp.mapeff.move.y[3].num = 0;
					recfp.mapeff.move.y[4].num = 0;
					recfp.mapeff.move.x[0].num = 0;
					recfp.mapeff.move.x[1].num = 0;
					recfp.mapeff.move.x[2].num = 0;
					LineMoveN[0] = 0;
					LineMoveN[1] = 0;
					LineMoveN[2] = 0;
					lockN[0] = 0;
					lockN[1] = 0;
					recfp.mapeff.carrow.num = 0;
					viewTN = 0;
					MovieN = 0;
				}
				key3 = 1;
			}
			else if (CheckHitKey(KEY_INPUT_RIGHT) == 1) {
				if (key3 == 0) {
					recfp.time.now += 10000;
#if SWITCH_NOTE_BOX_2 == 1
					for (i[0] = 0; i[0] < 3; i[0]++) {
						while (recfp.mapdata.note[objectN[i[0]]].hittime < recfp.time.now) {
							objectN[i[0]]++;
						}
					}
#else
					while (recfp.mapdata.note2.up[objectN[0]].hittime < recfp.time.now) {
						objectN[0]++;
					}
					while (recfp.mapdata.note2.mid[objectN[1]].hittime < recfp.time.now) {
						objectN[1]++;
					}
					while (recfp.mapdata.note2.low[objectN[2]].hittime < recfp.time.now) {
						objectN[2]++;
					}
#endif
					for (i[0] = 0; i[0] < 3; i[0]++) {
						objectNG[i[0]] = objectN[i[0]];
					}
					cameraN = 0;
					scroolN = 0;
					itemN = 0;
					SitemN = 0;
					recfp.mapeff.chamo[0].num = 0;
					recfp.mapeff.chamo[1].num = 0;
					recfp.mapeff.chamo[2].num = 0;
					recfp.mapeff.fall.num = 0;
					recfp.mapeff.move.y[0].num = 0;
					recfp.mapeff.move.y[1].num = 0;
					recfp.mapeff.move.y[2].num = 0;
					recfp.mapeff.move.y[3].num = 0;
					recfp.mapeff.move.y[4].num = 0;
					recfp.mapeff.move.x[0].num = 0;
					recfp.mapeff.move.x[1].num = 0;
					recfp.mapeff.move.x[2].num = 0;
					LineMoveN[0] = 0;
					LineMoveN[1] = 0;
					LineMoveN[2] = 0;
					lockN[0] = 0;
					lockN[1] = 0;
					recfp.mapeff.carrow.num = 0;
					viewTN = 0;
					MovieN = 0;
				}
				key3 = 1;
			}
			else {
				key3 = 0;
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
			recfp.time.now = GetNowCount() - Stime + system.offset * 5;
		}
		else {
			Stime = GetNowCount() - recfp.time.now + system.offset * 5;
		}
		ScreenFlip();
	}
	INIT_PIC();
	if (AutoFlag == 1) { return SCENE_SERECT; }
	else {
		ret_gap[0] = gap2.sum;
		ret_gap[1] = gap2.count;
		ret_gap[2] = gap2.ssum;
		return result(o, recfp.mapdata.Lv, drop, recfp.mapdata.mdif, recfp.nameset.songN, recfp.nameset.DifFN,
			fileN, judge, score.sum, Mcombo, recfp.mapdata.notes, ret_gap, Dscore.point);
	}
}

int CheckNearHitNote(
#if SWITCH_NOTE_BOX_2 == 1
	note_box_2_t *const unote, note_box_2_t *const mnote,
	note_box_2_t *const dnote,
#else
	struct note_box *const unote, struct note_box *const mnote,
	struct note_box *const dnote,
#endif
	int Ntime) {
	int ans = -1;
	int mintime = 200;
	if (unote->object == NOTE_HIT && unote->hittime - Ntime < mintime) {
		ans = 0;
		mintime = unote->hittime - Ntime;
	}
	if (mnote->object == NOTE_HIT && mnote->hittime - Ntime < mintime) {
		ans = 1;
		mintime = mnote->hittime - Ntime;
	}
	if (dnote->object == NOTE_HIT && dnote->hittime - Ntime < mintime) {
		ans = 2;
		mintime = dnote->hittime - Ntime;
	}
	return ans;
}

int GetHighScore(wchar_t pas[255], int dif) {
	FILE *fp;
	int a[7] = { 0,0,0,0,0,0,0 };
	int G = _wfopen_s(&fp, pas, L"rb");
	if (G == 0) {
		fread(&a, sizeof(int), 6, fp);
		fclose(fp);
	}
	return a[dif];
}

int GetRemainNotes2(struct judge_box judge, int Notes) {
	return Notes - judge.just - judge.good - judge.safe - judge.miss;
}

struct score_box GetScore3(struct score_box score, struct judge_box judge, const int notes,
	const int MaxCombo) {
	score.normal = (judge.just * 90000 + judge.good * 86667 + judge.safe * 45000) / notes;
	score.combo = MaxCombo * 10000 / notes;
	if (score.normal + score.combo - score.loss == 100000) {
		score.pjust = mins(100 - notes + judge.pjust, 0);
	}
	else {
		score.pjust = 0;
	}
	score.sum = score.normal + score.combo - score.loss + score.pjust;
	return score;
}

void Getxxxpng(wchar_t *str, int num) {
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

void Getxxxwav(wchar_t *str, int num) {
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

int CalPosScore2(struct score_box score, int RemainNotes, int Notes, int combo, int MaxCombo) {
	int PosCombo = mins(combo + RemainNotes, MaxCombo);
	return score.normal + 90000 * RemainNotes / Notes + 10000 * PosCombo / Notes;
}

void ShowCombo(int combo, int *pic) {
#define ROCATION_X 320
#define ROCATION_Y 150
#define CHARA_WIDTH 50

	if (combo < 10) {
		return;
	}
	int t;
	int xx;
	GetGraphSize(pic[0], &xx, &t);
	t = 0;
	int s;
	for (int i = combo; i > 0; i /= 10) {
		t++;
	}
	for (int i = t - 1; i >= 0; i--) {
		s = combo;
		for (int j = 0; j < i; j++) {
			s /= 10;
		}
		s %= 10;
		RecRescaleDrawGraph(t * CHARA_WIDTH / 2 - CHARA_WIDTH / 2 - i * CHARA_WIDTH - xx / 2 + ROCATION_X, ROCATION_Y, pic[s], TRUE);
	}
}

void ShowScore2(struct score_box score, int Hscore, int time) {
	unsigned int Cr = GetColor(255, 255, 255);
	int s_score = score.sum;
	if (time - score.time < 500) {
		s_score = pals(500, score.sum, 0, score.before, time - score.time);
	}
	if (Hscore <= s_score) {
		Cr = GetColor(255, 255, 0);
	}
	RecRescaleDrawFormatString(490, 20, Cr, L"SCORE:%d", s_score);
}

void PlayNoteHitSound2(play_sound_t* const sound) {
	if ((sound->flag & SE_HIT) != 0) {
		PlaySoundMem(sound->att, DX_PLAYTYPE_BACK);
	}
	if ((sound->flag & SE_CATCH) != 0) {
		PlaySoundMem(sound->cat, DX_PLAYTYPE_BACK);
	}
	if ((sound->flag & SE_ARROW) != 0) {
		PlaySoundMem(sound->arw, DX_PLAYTYPE_BACK);
	}
	if ((sound->flag & SE_BOMB) != 0) {
		PlaySoundMem(sound->bom, DX_PLAYTYPE_BACK);
	}
	if ((sound->flag & SE_SWING) != 0) {
		PlaySoundMem(sound->swi, DX_PLAYTYPE_BACK);
	}
	sound->flag = 0;
	return;
}

void RunningStats2(struct judge_box judge, int PosScore, int HighScore) {
#define x1 6
#define y1 6
#define x2 188
#define y2 6
#define x3 150
#define y3 69
#define x4 6
#define y4 69
	unsigned int CrG = GetColor(63, 63, 63);
	unsigned int CrD = GetColor(255, 63, 127);
	unsigned int CrY = GetColor(255, 255, 0);
	unsigned int CrC = GetColor(0, 255, 255);
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
		RecRescaleDrawTriangle(x1, y1, x3, y3, x4, y4, GetColor(lins(HighScore, 255, 100000, 0, PosScore), 255, lins(HighScore, 0, 100000, 255, PosScore)), TRUE);
	}
}
