
#if 1 /* filter */

/* include */

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
		DrawFormatString(20, 120 + ofs * 20, Cr, L#data": %d", data)
#define RECR_DEBUG_LOOP(ofs, n, data_a, data_b)							\
	for (int _rep = 0; _rep < n; _rep++) {								\
		DrawFormatString(20, 120 + _rep * 20 + ofs * 20, Cr,			\
		L#data_a"[%d]"#data_b": %d", _rep, data_a[_rep]data_b);			\
	}
#else
#define RECR_DEBUG(n, data_a, data_b)
#define RECR_DEBUG_LOOP(n, data_a, data_b)
#endif

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

typedef struct rec_move_data_s {
	int Stime = -10000;
	int pos = -10000;
	int Etime = -10000;
	int mode = -10000;
} rec_move_data_t;
typedef struct rec_move_set_s {
	rec_move_data_t d[999];
	int num = 0;
} rec_move_set_t;
typedef struct rec_move_all_set_s {
	rec_move_set_t x[3];
	rec_move_set_t y[5];
} rec_move_all_set_t;

/* <=-1: just release, 0: no push, 1: just push, 2<=: hold */
typedef struct rec_play_key_hold_s {
	int z = 0;
	int x = 0;
	int c = 0;
	int up = 0;
	int down = 0;
	int left = 0;
	int right = 0;
} rec_play_key_hold_t;

typedef struct rec_paly_time_set_s {
	int now;
	int end;
	int offset;
} rec_play_time_set_t;

typedef struct rec_chara_gra_data_s {
	int gra[99];
	int time[99];
	int num = 0;
} rec_chara_gra_data_t[3];

typedef struct rec_chara_arrow_s {
	struct {
		int data = 0;
		int time = -10000;
	} d[99];
	int num = 0;
} rec_chara_arrow_t;

typedef struct rec_fall_data_s {
	struct {
		int No = -10000;
		int time = -10000;
	} d[99];
	int num = 0;
} rec_fall_data_t;

typedef struct rec_view_bpm_set_s {
	view_BPM_box data[100];
	int num = 0;
} rec_view_bpm_set_t;

#endif /* filter */

typedef struct rec_play_nameset_s {
	wchar_t sky[255] = L"picture/backskynoamal.png";
	wchar_t ground[255] = L"picture/groundnaturenormal.png";
	wchar_t water[255] = L"picture/waternormal.png";
	wchar_t mp3FN[255] = L"song/";
	wchar_t songN[255];
	wchar_t songNE[255];
	wchar_t DifFN[255] = L"picture/difanother.png";
} rec_play_nameset_t;

typedef struct rec_map_eff_data_s {
	struct camera_box camera[255];
	struct scrool_box scrool[99];
	item_box Movie[999];
	rec_view_bpm_set_t v_BPM;
	int viewT[2][99];//[音符表示時間,実行時間,[0]=現ナンバー]
	rec_chara_arrow_t carrow;
	int lock[2][2][99]; //lock = [横,縦]の音符の位置を[(1=固定する,-1以外=固定しない),時間]
	rec_move_all_set_t move;
	rec_chara_gra_data_t chamo;
	rec_fall_data_t fall;
	double speedt[5][99][2]; //[上, 中, 下, (地面), (水中)]レーンの[0:切り替え時間,1:速度]
} rec_map_eff_data_t;

typedef struct rec_map_detail_s {
	double bpm = 120;
	short int Lv = 0;
#if SWITCH_NOTE_BOX_2 == 1
	note_box_2_t note[6000];
#else
	note_lane_t note2;
#endif
	short int notes = 0;
	int mdif = 0;
	int ldif = 0;
	int ddif[25] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//各区間の難易度
	int ddifG[2] = { 1,25 };//0=今いる区間番号(1～25),1=最大値
} rec_map_detail_t;

#if 1 /* filter2 */

/* proto */

void AddGap(gap_box* const box, int data);

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

note_judge CheckJudge(int gap);
int CheckNearHitNote(
#if SWITCH_NOTE_BOX_2 == 1
	note_box_2_t *const unote, note_box_2_t *const mnote,
	note_box_2_t *const dnote,
#else
	struct note_box *const unote, struct note_box *const mnote,
	struct note_box *const dnote,
#endif
	int Ntime);
#if SWITCH_NOTE_BOX_2 == 0
int GetCharaPos2(int time, note_box_t highnote, note_box_t midnote,
	note_box_t lownote, int keyu, int keyd, int keyl, int keyr, int hitatp,
	int hitatt) {
	struct note_box note[3] = { highnote, midnote, lownote };
	int ans = CHARA_POS_MID;
	// near catch/bomb
	for (int i = 0; i < 3; i++) {
		if ((note[i].object == 2 && note[i].hittime <= time + 40 ||
			note[i].object == 7 && note[i].hittime <= time + 40) &&
			keyu == 0 && keyd == 0) {
			return CHARA_POS_MID;
		}
	}
	// hit note
	if (keyu != 1 && keyd != 1 && keyl != 1 && keyr != 1 && hitatt != -1000) {
		return hitatp;
	}
	// push up
	if (1 <= keyu && 0 == keyd) {
		ans = CHARA_POS_UP;
	}
	// push down
	else if (0 == keyu && 1 <= keyd) {
		ans = CHARA_POS_DOWN;
	}
	// push up and down
	else if (1 <= keyu && 1 <= keyd) {
		ans = CHARA_POS_MID;
	}
	// not hit
	else {
		ans = CHARA_POS_MID;
	}
	return ans;
}
#endif
int GetHighScore(wchar_t pas[255], int dif);
int GetRemainNotes2(struct judge_box judge, int Notes);
struct score_box GetScore3(struct score_box score, struct judge_box judge,
	const int notes, const int MaxCombo);
void Getxxxpng(wchar_t *str, int num);
void Getxxxwav(wchar_t *str, int num);
void note_judge_event(note_judge judge, distance_score_t *Dscore,
#if SWITCH_NOTE_BOX_2 == 1
	note_box_2_t const *const noteinfo,
#else
	note_box const *const noteinfo,
#endif
	int *const Sitem, int Ntime, int Jtime, int lineNo,
	judge_action_box *const judgeA);
int CalPosScore2(struct score_box score, int RemainNotes, int Notes, int combo,
	int MaxCombo);
void SetHitPosByHit(int *const hitatk, char const hitflag, int Ntime);
void ShowCombo(int combo, int *pic);

void ShowScore2(struct score_box score, int Hscore, int time);
void RunningStats2(struct judge_box judge, int PosScore, int HighScore);
char PlayNoteHitSound(
#if SWITCH_NOTE_BOX_2 == 1
	note_box_2_t note,
#else
	note_box note,
#endif
	int *MelodySnd, int *Sitem, char seflag,
	int notemat);
void PlayNoteHitSound2(play_sound_t* const sound);

/* sub action */
#if SWITCH_NOTE_BOX_2 == 1
int GetCharaPos3(int time, note_box_2_t note[], short int No[],
	rec_play_key_hold_t *keyhold, int hitatp, int hitatt) {
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
		keyhold->left != 1 && keyhold->right != 1 && hitatt != -1000)
	{
		return hitatp;
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
#endif

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
static int StepNoDrawNote(note_box_2_t *note, rec_map_eff_data_t *mapeff,
	short viewTN, short lockN[], int iLine, short speedN, int Ntime, int G[])
{
	double sppedt_temp[99];
	G[7] = StepViewNoDrawNote(note->hittime, mapeff, viewTN, &G[0], Ntime);
	if (G[7] == 1) { return 1; }
	else if (G[7] == 2) { return 2; }
	//ノーツロックナンバーを進める
	if (note->hittime >= mapeff->lock[0][1][lockN[0] + G[3] + 1] &&
		mapeff->lock[0][1][lockN[0] + G[3] + 1] >= 0) {
		G[3]++;
	}
	if (note->hittime >= mapeff->lock[1][1][lockN[1] + G[4] + 1] &&
		mapeff->lock[1][1][lockN[1] + G[4] + 1] >= 0) {
		G[4]++;
	}
	// スピードナンバーを進める
	for (int i = 0; i < 99; i++) {
		sppedt_temp[i] = mapeff->speedt[iLine][0][i * 2];
	}
	while (note->hittime >= sppedt_temp[speedN + G[5] + 1] &&
		sppedt_temp[speedN + G[5] + 1] >= 0) {
		G[5]++;
	}
	return 0;
}

static void CalPalCrawNote(int lock0, int lock1, note_box_2_t *note, int Xline,
	int Yline, double speedt, struct scrool_box *scrool,
	rec_play_xy_set_t *nowcamera, int Ntime, int G[])
{
	//縦位置
	G[2] = ((lock1 == 1) ? note->ypos : Yline);
	//横位置
	G[1] = (int)((speedt * 20 * (note->viewtime -
		(scrool->speed * Ntime + scrool->basetime)) + 5000) / 50) + 50;
	G[1] += ((lock0 == 1) ? note->xpos - 150 : Xline - 150);
	//色
	G[6] = note->color;
	//カメラ補正
	G[1] += nowcamera->x;
	G[2] += nowcamera->y;
}

/**
 * return 0 = normal, 1 = continue, 2 = break;
 */
static int DrawNoteOne(int G[], note_box_2_t *note, rec_map_eff_data_t *mapeff,
	short viewTN, short lockN[], int iLine, short speedN, int Ntime, int Xline,
	int Yline, int scroolN, rec_play_xy_set_t *nowcamera,
	struct note_img *noteimg)
{
	G[7] = StepNoDrawNote(note, mapeff, viewTN, lockN, iLine, speedN, Ntime, G);
	if (G[7] == 1) { return 1; }
	else if (G[7] == 2) { return 2; }
	CalPalCrawNote(mapeff->lock[0][0][lockN[0] + G[3]], mapeff->lock[1][0][lockN[1] + G[4]],
		note, Xline, Yline, mapeff->speedt[iLine][0][(speedN + G[5]) * 2 + 1],
		&mapeff->scrool[scroolN], nowcamera, Ntime, G);
	switch (note->object) {
	case 1:
		DrawGraph(G[1], G[2], noteimg->notebase, TRUE);
		DrawGraph(G[1], G[2], noteimg->hitcircle[G[6]], TRUE);
		break;
	case 2:
		DrawGraph(G[1], G[2], noteimg->catchi, TRUE);
		break;
	case 3:
		DrawGraph(G[1], G[2], noteimg->notebase, TRUE);
		DrawGraph(G[1], G[2], noteimg->up, TRUE);
		break;
	case 4:
		DrawGraph(G[1], G[2], noteimg->notebase, TRUE);
		DrawGraph(G[1], G[2], noteimg->down, TRUE);
		break;
	case 5:
		DrawGraph(G[1], G[2], noteimg->notebase, TRUE);
		DrawGraph(G[1], G[2], noteimg->left, TRUE);
		break;
	case 6:
		DrawGraph(G[1], G[2], noteimg->notebase, TRUE);
		DrawGraph(G[1], G[2], noteimg->right, TRUE);
		break;
	case 7:
		DrawGraph(G[1], G[2], noteimg->bomb, TRUE);
		break;
	case 8:
		DrawGraph(G[1], G[2], noteimg->goust, TRUE);
		break;
	}
	return 0;
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
		DrawLine(x1, y1, x2, y2, color, thick);
		break;
	case 2: // acc
		for (int i = x1; i <= x2; i++) {
			end = maxs(i + 10, x2);
			DrawLine(i, pals(x1, y1, x2, y2, i),
				end, pals(x1, y1, x2, y2, end),
				color, thick);
		}
		break;
	case 3: // dec
		for (int i = x1; i <= x2; i++) {
			end = maxs(i + 10, x2);
			DrawLine(i, pals(x2, y2, x1, y1, i),
				end, pals(x2, y2, x1, y1, end),
				color, thick);
		}
		break;
	}
	return;
}

int PlayShowGuideLine(int Ntime, int Line, rec_move_set_t Ymove[], int Xline[],
	int Yline[], rec_play_xy_set_t *nowcamera, int iDraw)
{
	int drawLeft = 0;
	int drawRight = 0;
	int drawY1 = 0;
	int drawY2 = 0;
	int drawC = 0;
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
		drawLeft = (Ymove[Line].d[iDraw - 1].Etime - Ntime) / 2.1 + Xline[Line] + 15 + nowcamera->x;
		drawRight = 640;
		drawY1 = Ymove[Line].d[iDraw - 1].pos + 15 + nowcamera->y;
		drawY2 = Ymove[Line].d[iDraw - 1].pos + 15 + nowcamera->y;
		DrawLine(drawLeft, drawY1, drawRight, drawY2, drawC, 2);
		return 1;
	}
	// cal Xpos1
	if (iDraw < 1) {
		drawLeft = Xline[Line] + Xline[Line] + 15 + nowcamera->x;
		drawRight = (Ymove[Line].d[iDraw].Stime - Ntime) / 2.1 + Xline[Line] + 15 + nowcamera->x;
		drawY1 = Yline[Line] + 15 + nowcamera->y;
		drawY2 = Yline[Line] + 15 + nowcamera->y;
		DrawLine(drawLeft, drawY1, drawRight, drawY2, drawC, 2);
	}
	else if (Ntime < Ymove[Line].d[iDraw].Etime) {
		drawLeft = (Ymove[Line].d[iDraw - 1].Etime - Ntime) / 2.1 + Xline[Line] + 15 + nowcamera->x;
		drawRight = (Ymove[Line].d[iDraw].Stime - Ntime) / 2.1 + Xline[Line] + 15 + nowcamera->x;
		drawY1 = Ymove[Line].d[iDraw - 1].pos + 15 + nowcamera->y;
		drawY2 = Ymove[Line].d[iDraw - 1].pos + 15 + nowcamera->y;
		DrawLine(drawLeft, drawY1, drawRight, drawY2, drawC, 2);
	}
	drawLeft = (Ymove[Line].d[iDraw].Stime - Ntime) / 2.1 + Xline[Line] + 15 + nowcamera->x;
	if (640 < drawLeft) {
		return 1;
	}
	drawRight = (Ymove[Line].d[iDraw].Etime - Ntime) / 2.1 + Xline[Line] + 15 + nowcamera->x;
	drawY1 = Ymove[Line].d[iDraw - 1].pos + 15 + nowcamera->y;
	drawY2 = Ymove[Line].d[iDraw].pos + 15 + nowcamera->y;
	// wiew
	DrawLineCurve(drawLeft, drawY1, drawRight, drawY2, Ymove[Line].d[iDraw].mode, drawC, 2);
	return 0;
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

void PlaySetCamera(rec_play_xy_set_t *retcam, struct camera_box camset[], int camN, int Ntime) {
	if (camset[camN].starttime <= Ntime && Ntime <= camset[camN].endtime) {
		retcam->x = (int)movecal(camset[camN].mode,
			camset[camN].starttime, camset[camN - 1].xpos,
			camset[camN].endtime, camset[camN].xpos, Ntime);
		retcam->y = (int)movecal( camset[camN].mode,
			camset[camN].starttime, camset[camN - 1].ypos,
			camset[camN].endtime, camset[camN].ypos, Ntime);
	}
	else {
		retcam->x = camset[camN - 1].xpos;
		retcam->y = camset[camN - 1].ypos;
	}
	return;
}

void PlayDrawItem(rec_map_eff_data_t *mapeff, short int MovieN, int Ntime,
	rec_play_xy_set_t *camera, int Xmidline, int item[])
{
	view_BPM_box *v_BPM = &mapeff->v_BPM.data[mapeff->v_BPM.num];
	int drawA;
	int drawX;
	int drawY;
	int drawS;
	int drawR;
	for (item_box *pMovie = &mapeff->Movie[MovieN]; pMovie->endtime > -500; pMovie++) {
		if (pMovie->starttime <= Ntime && pMovie->endtime >= Ntime) {
			//base setting
			drawA = (int)movecal(pMovie->movemode,
				pMovie->starttime, pMovie->startalpha,
				pMovie->endtime, pMovie->endalpha, Ntime);
			drawX = (int)movecal(pMovie->movemode,
				pMovie->starttime, pMovie->startXpos,
				pMovie->endtime, pMovie->endXpos, Ntime) + camera->x;
			drawY = (int)movecal(pMovie->movemode,
				pMovie->starttime, pMovie->startYpos,
				pMovie->endtime, pMovie->endYpos, Ntime) + camera->y;
			drawS = (int)movecal(pMovie->movemode,
				pMovie->starttime, pMovie->startsize,
				pMovie->endtime, pMovie->endsize, Ntime);
			drawR = (int)movecal(pMovie->movemode,
				pMovie->starttime, pMovie->startrot,
				pMovie->endtime, pMovie->endrot, Ntime);
			//material setting
			if (pMovie->eff.lock == 1) {
				drawX -= camera->x;
			}
			if (pMovie->eff.lock == 1) {
				drawY -= 25 + camera->y;
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

#endif /* filter2 */

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
	short int bgf[2] = { 0,0 }; //落ち物背景の[0:横位置,1:縦位置]
	short int charaput = 1; //キャラの今の位置[0で上,1で中,2で下]
	short int drop = 0;
	short int KeyPushCount[7] = { 0,0,0,0,0,0,0 };
	short int cameraN = 0;
	/* int */
	int bgp[3] = { 0,0,0 }; //[0:sky,1:ground,2:water]の横位置
	int judgh = 0; //ノーツの時間距離
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
	int hitpose = 0;
	int scroolN = 0;
	int HighSrore; //ハイスコア
	int hitatk[2] = { 1,-1000 }; //0:位置, 1:時間
	int fps[62];//0～59=1フレーム間隔の時間,60=次の代入先,61=前回の時間
	short LineMoveN[3] = { 0,0,0 }; //↑のライン表示番号
	short int lockN[2] = { 0,0 }; //↑の番号
	short int viewTN = 0;
	int Yline[5] = { 300,350,400,350,600 };//[上,中,下,地面,水中]レーンの縦位置
	int Xline[3] = { 150,150,150 };//[上,中,下]レーンの横位置
	int outpoint[2] = { -1,0 };
	unsigned int Cr = GetColor(255, 255, 255);
	unsigned int Crb = GetColor(0, 0, 0);
	unsigned int CrR = GetColor(255, 0, 0);
	int CutTime = 0;
	int Stime = 0;
	int adif = 0;
	/* struct */
	rec_map_detail_t mapdata;
	rec_play_nameset_t nameset;
	rec_play_key_hold_t keyhold;
	rec_play_time_set_t time;
	rec_play_xy_set_t nowcamera;
	nowcamera.x = 320;
	nowcamera.y = 240;
	rec_system_t system;
	distance_score_t Dscore;
	play_key_stat_t key_stat;
	gap_box gap2;
	rec_map_eff_data_t mapeff;
	struct judge_box judge;
	struct score_box score;
	short int MovieN = 0;
	rec_view_bpm_set_t v_BPM;
	short int objectN[3] = { 5999,5999,5999 }; //note number
	short int objectNG[3] = { 0,0,0 }; //note number without ghost note
	playnum_box allnum;
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
	int backskyimg = 0;
	int backgroundimg = 0;
	int backwaterimg = 0;
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
		G[2] = _wfopen_s(&fp, GT1, L"rb");//rrsデータを読み込む
	}
	if (G[2] != 0) {
		RecordLoad2(p, n, o);//rrsデータが無い、または作成の指示があれば作る
		cal_ddif_3(GT1);//ddif計算
		G[2] = _wfopen_s(&fp, GT1, L"rb");//rrsデータを読み込む
	}
	if (fp != NULL) {
		fread(&allnum, sizeof(playnum_box), 1, fp);//各データの個数
		fread(&nameset.mp3FN, 255, 1, fp);//音楽ファイル名
		fread(&mapdata.bpm, sizeof(double), 1, fp);//BPM
		fread(&time.offset, sizeof(int), 1, fp);//offset
		fread(&nameset.sky, 255, 1, fp);//空背景名
		fread(&nameset.ground, 255, 1, fp);//地面画像名
		fread(&nameset.water, 255, 1, fp);//水中画像名
		fread(&nameset.songN, 255, 1, fp);//曲名
		fread(&nameset.songNE, 255, 1, fp);//曲名(英語)
		fread(&mapdata.Lv, sizeof(short int), 1, fp);//レベル
		//fread(&item, sizeof(int), 99, fp);//アイテム画像データ(動作未確認)
		{
			int buf[99][2];
			fread(buf, sizeof(int), 198, fp);//落ち物背景切り替えタイミング
			for (int i = 0; i < 99; i++) {
				mapeff.fall.d[i].time = buf[i][0];
				mapeff.fall.d[i].No   = buf[i][1];
			}
		}
		fread(&mapeff.speedt, sizeof(double), 990, fp);//レーン速度
		{
			int buf[3][99][2];
			fread(buf, sizeof(int), 594, fp);//キャラグラ変換タイミング
			for (int i = 0; i < 99; i++) {
				mapeff.chamo[0].gra[i]  = buf[0][i][0];
				mapeff.chamo[0].time[i] = buf[0][i][1];
				mapeff.chamo[1].gra[i]  = buf[1][i][0];
				mapeff.chamo[1].time[i] = buf[1][i][1];
				mapeff.chamo[2].gra[i]  = buf[2][i][0];
				mapeff.chamo[2].time[i] = buf[2][i][1];
			}
		}
		{
			int buf[999][4];
			fread(buf, sizeof(int), allnum.Ymovenum[0] * 4, fp);//上レーン縦位置移動タイミング
			for (int i = 0; i < allnum.Ymovenum[0]; i++) {
				mapeff.move.y[0].d[i].Stime = buf[i][0];
				mapeff.move.y[0].d[i].pos   = buf[i][1];
				mapeff.move.y[0].d[i].Etime = buf[i][2];
				mapeff.move.y[0].d[i].mode  = buf[i][3];
			}
			fread(buf, sizeof(int), allnum.Ymovenum[1] * 4, fp);//中レーン縦位置移動タイミング
			for (int i = 0; i < allnum.Ymovenum[1]; i++) {
				mapeff.move.y[1].d[i].Stime = buf[i][0];
				mapeff.move.y[1].d[i].pos   = buf[i][1];
				mapeff.move.y[1].d[i].Etime = buf[i][2];
				mapeff.move.y[1].d[i].mode  = buf[i][3];
			}
			fread(buf, sizeof(int), allnum.Ymovenum[2] * 4, fp);//下レーン縦位置移動タイミング
			for (int i = 0; i < allnum.Ymovenum[2]; i++) {
				mapeff.move.y[2].d[i].Stime = buf[i][0];
				mapeff.move.y[2].d[i].pos   = buf[i][1];
				mapeff.move.y[2].d[i].Etime = buf[i][2];
				mapeff.move.y[2].d[i].mode  = buf[i][3];
			}
			fread(buf, sizeof(int), allnum.Ymovenum[3] * 4, fp);//地面縦位置移動タイミング
			for (int i = 0; i < allnum.Ymovenum[3]; i++) {
				mapeff.move.y[3].d[i].Stime = buf[i][0];
				mapeff.move.y[3].d[i].pos   = buf[i][1];
				mapeff.move.y[3].d[i].Etime = buf[i][2];
				mapeff.move.y[3].d[i].mode  = buf[i][3];
			}
			fread(buf, sizeof(int), allnum.Ymovenum[4] * 4, fp);//水面縦位置移動タイミング
			for (int i = 0; i < allnum.Ymovenum[4]; i++) {
				mapeff.move.y[4].d[i].Stime = buf[i][0];
				mapeff.move.y[4].d[i].pos   = buf[i][1];
				mapeff.move.y[4].d[i].Etime = buf[i][2];
				mapeff.move.y[4].d[i].mode  = buf[i][3];
			}
			fread(buf, sizeof(int), allnum.Xmovenum[0] * 4, fp);//上レーン横位置移動タイミング
			for (int i = 0; i < allnum.Xmovenum[0]; i++) {
				mapeff.move.x[0].d[i].Stime = buf[i][0];
				mapeff.move.x[0].d[i].pos   = buf[i][1];
				mapeff.move.x[0].d[i].Etime = buf[i][2];
				mapeff.move.x[0].d[i].mode  = buf[i][3];
			}
			fread(buf, sizeof(int), allnum.Xmovenum[1] * 4, fp);//中レーン横位置移動タイミング
			for (int i = 0; i < allnum.Xmovenum[1]; i++) {
				mapeff.move.x[1].d[i].Stime = buf[i][0];
				mapeff.move.x[1].d[i].pos   = buf[i][1];
				mapeff.move.x[1].d[i].Etime = buf[i][2];
				mapeff.move.x[1].d[i].mode  = buf[i][3];
			}
			fread(buf, sizeof(int), allnum.Xmovenum[2] * 4, fp);//下レーン横位置移動タイミング
			for (int i = 0; i < allnum.Xmovenum[2]; i++) {
				mapeff.move.x[2].d[i].Stime = buf[i][0];
				mapeff.move.x[2].d[i].pos   = buf[i][1];
				mapeff.move.x[2].d[i].Etime = buf[i][2];
				mapeff.move.x[2].d[i].mode  = buf[i][3];
			}
		}
		fread(&mapeff.lock, sizeof(int), 396, fp);//ノーツ固定切り替えタイミング
		{
			int buf[2][99];
			fread(buf, sizeof(int), 198, fp);//キャラ向き切り替えタイミング
			for (int i = 0; i < 99; i++) {
				mapeff.carrow.d[i].data = buf[0][i];
				mapeff.carrow.d[i].time = buf[1][i];
			}
		}
		fread(&mapeff.viewT, sizeof(int), 198, fp);//ノーツ表示時間変換タイミング
#if SWITCH_NOTE_BOX_2 == 1
		fread(&mapdata.note, sizeof(note_box_2_t),
			allnum.notenum[0] + allnum.notenum[1] + allnum.notenum[2], fp); /* ノーツデータ */
#else
		fread(&mapdata.note2.up[0], sizeof(struct note_box), allnum.notenum[0], fp); /* 上レーンノーツデータ */
		fread(&mapdata.note2.mid[0], sizeof(struct note_box), allnum.notenum[1], fp); /* 中レーンノーツデータ */
		fread(&mapdata.note2.low[0], sizeof(struct note_box), allnum.notenum[2], fp); /* 下レーンノーツデータ */
#endif
		fread(&mapdata.notes, sizeof(short int), 1, fp);//ノーツ数
		fread(&time.end, sizeof(int), 1, fp);//曲終了時間
		{
			int buf[2];
			fread(buf, sizeof(int), 2, fp);
			mapdata.mdif = buf[0];//最高難易度
			mapdata.ldif = buf[1];//最終難易度
		}
		fread(&mapdata.ddif, sizeof(int), 25, fp);//各区間難易度データ
		fread(&mapdata.ddifG, sizeof(int), 2, fp);//各区間難易度データ
		fread(&nameset.DifFN, 255, 1, fp);//難易度バー名
		fread(&mapeff.Movie, sizeof(item_box), allnum.movienum, fp);//アイテムデータ
		fread(&mapeff.camera, sizeof(struct camera_box), 255, fp);//カメラデータ
		fread(&mapeff.scrool, sizeof(struct scrool_box), 99, fp);//スクロールデータ
		fread(&mapeff.v_BPM.data[0], sizeof(view_BPM_box), allnum.v_BPMnum, fp);//見た目のBPMデータ
		fread(&outpoint, sizeof(int), 2, fp);//エラーデータ
	}
	musicmp3 = LoadSoundMem(nameset.mp3FN);
	backskyimg = LoadGraph(nameset.sky);
	backgroundimg = LoadGraph(nameset.ground);
	backwaterimg = LoadGraph(nameset.water);
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
	mapdata.notes = notzero(mapdata.notes);
	GD[0] = mapdata.mdif / 100.0 - mapdata.Lv;//mdifと難易度表記の差
	if (mapdata.Lv == 0) { DifRate = 0; }
	else if (2 <= GD[0]) { DifRate = mapdata.Lv + 0.9; }
	else if (0 <= GD[0] && GD[0] < 2) { DifRate = mapdata.Lv + 0.45 * GD[0]; }
	else { DifRate = mapdata.mdif / 100.0; }
#if SWITCH_NOTE_BOX_2 == 1
	for (i[0] = 0; i[0] < allnum.notenum[0] + allnum.notenum[1] + allnum.notenum[2]; i[0]++) {
		if (mapdata.note[i[0]].lane == NOTE_LANE_UP) {
			objectN[0] = i[0];
			break;
		}
	}
	for (i[0] = 0; i[0] < allnum.notenum[0] + allnum.notenum[1] + allnum.notenum[2]; i[0]++) {
		if (mapdata.note[i[0]].lane == NOTE_LANE_MID) {
			objectN[1] = i[0];
			break;
		}
	}
	for (i[0] = 0; i[0] < allnum.notenum[0] + allnum.notenum[1] + allnum.notenum[2]; i[0]++) {
		if (mapdata.note[i[0]].lane == NOTE_LANE_LOW) {
			objectN[2] = i[0];
			break;
		}
	}
#endif
	PlaySoundMem(musicmp3, DX_PLAYTYPE_BACK);
	WaitTimer(10);
	CutTime = Stime = GetNowCount();
	//ゲーム開始
	while (1) {
		ClearDrawScreen();
		GetHitKeyStateAll(key);
		// number step
		for (int iLine = 0; iLine < 3; iLine++) {
			objectNG[iLine] = mins(objectNG[iLine], objectN[iLine]);
			while (mapdata.note[objectNG[iLine]].object == NOTE_GHOST) {
				objectNG[iLine] = mapdata.note[objectNG[iLine]].next;
			}
			while (0 <= mapeff.chamo[iLine].time[mapeff.chamo[iLine].num + 1] &&
						mapeff.chamo[iLine].time[mapeff.chamo[iLine].num + 1] <= time.now) {
				mapeff.chamo[iLine].num++;
			}
			while (0 <= mapeff.speedt[iLine][speedN[iLine] + 1][0] &&
						mapeff.speedt[iLine][speedN[iLine] + 1][0] <= time.now) {
				speedN[iLine]++;
			}
		}
		while (-1000 < mapeff.v_BPM.data[mapeff.v_BPM.num + 1].time &&
					   mapeff.v_BPM.data[mapeff.v_BPM.num + 1].time <= time.now) {
			mapeff.v_BPM.num++;
		}
		while (0 <= mapeff.camera[cameraN].endtime &&
					mapeff.camera[cameraN].endtime < time.now) {
			cameraN++;
		}
		while (0 <= mapeff.scrool[scroolN + 1].starttime &&
					mapeff.scrool[scroolN + 1].starttime <= time.now) {
			scroolN++;
		}
		if (system.backLight != 0) {
			while (-500 < mapeff.Movie[MovieN].endtime &&
						  mapeff.Movie[MovieN].endtime < time.now) {
				MovieN++;
			}
		}
		if (AutoFlag == 1) {
			for (i[0] = 0; i[0] < 3; i[0]++) {
				while ((0 <= mapeff.move.y[i[0]].d[LineMoveN[i[0]]].Stime &&
							 mapeff.move.y[i[0]].d[LineMoveN[i[0]]].Etime <= time.now) ||
							 mapeff.move.y[i[0]].d[LineMoveN[i[0]]].mode == 4) {
					LineMoveN[i[0]]++;
				}
			}
		}
		while (0 <= mapeff.carrow.d[mapeff.carrow.num + 1].time &&
					mapeff.carrow.d[mapeff.carrow.num + 1].time < time.now) {
			mapeff.carrow.num++;
		}
		for (i[0] = 0; i[0] < 2; i[0]++) {
			while (0 <= mapeff.lock[i[0]][1][lockN[i[0]] + 1] &&
						mapeff.lock[i[0]][1][lockN[i[0]] + 1] <= time.now) {
				lockN[i[0]]++;
			}
		}
		while (0 <= mapeff.viewT[0][viewTN + 1] &&
					mapeff.viewT[0][viewTN + 1] <= time.now) {
			viewTN++;
		}
		//カメラ移動
		PlaySetCamera(&nowcamera, mapeff.camera, cameraN, time.now);
		//背景表示
		if (system.backLight != 0) {
			if (mapeff.speedt[3][speedN[3] + 1][0] < time.now &&
				mapeff.speedt[3][speedN[3] + 1][0] >= 0)
			{
				speedN[3]++;
			}
			cal_back_x(bgp, &mapeff, speedN, scroolN, nowcamera.x);
			G[18] = 0;
			G[19] = bgp[1];
			//draw background picture
			G[0] = bgp[0] / 100;
			while (G[0] + nowcamera.x / 5 < 70000) {
				DrawGraph(G[0] + nowcamera.x / 5,
					Yline[3] / 5 - 160 + nowcamera.y / 5,
					backskyimg, TRUE);
				G[0] += 640;
			}
			G[0] = bgp[1] / 100;
			while (G[0] + nowcamera.x < 70000) {
				DrawGraph(G[0] + nowcamera.x,
					Yline[3] - 400 + nowcamera.y, backgroundimg,
					TRUE);
				DrawGraph(G[0] + nowcamera.x,
					Yline[4] - 400 + nowcamera.y, backwaterimg,
					TRUE);
				G[0] += 640;
			}
			//落ち物背景表示
			if (time.now >= mapeff.fall.d[mapeff.fall.num + 1].time &&
				mapeff.fall.d[mapeff.fall.num + 1].time >= 0)
			{
				mapeff.fall.num++;
			}
			if (mapeff.fall.d[mapeff.fall.num].No >= 0) {
				G[0] = bgf[0];//横
				G[1] = bgf[1] + Yline[3];//縦
				for (i[0] = 0; i[0] < 2; i[0]++) {
					for (i[1] = 0; i[1] < 3; i[1]++) {
						DrawGraph(G[0] + i[0] * 640, G[1] - i[1] * 480,
							item[mapeff.fall.d[mapeff.fall.num].No], TRUE);
					}
				}
				bgf[0] -= 5;
				bgf[1] += 2;
			}
			if (bgf[0] <= -640)bgf[0] += 640;
			if (bgf[1] >= 640)bgf[1] -= 480;
		}
		//フィルター表示
		switch (system.backLight) {
		case 1:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 170);
			DrawGraph(0, 0, filterimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			break;
		case 2:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 85);
			DrawGraph(0, 0, filterimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			break;
		}
		//アイテム表示
		if (system.backLight != 0) {
			PlayDrawItem(&mapeff, MovieN, time.now, &nowcamera, Xline[1], item);
		}
		// view line
		if (AutoFlag == 1) {
			for (i[0] = 0; i[0] < 3; i[0]++) {
				for (int iDraw = LineMoveN[i[0]]; 1; iDraw++) {
					if (PlayShowGuideLine(time.now, i[0], mapeff.move.y, Xline, Yline, &nowcamera, iDraw) != 0) {
						break;
					}
				}
			}
		}
		// view chara pos guide
		if (mapeff.carrow.d[mapeff.carrow.num].data == 1) {
			DrawGraph(Xline[charaput] - 4 + nowcamera.x, Yline[charaput] - 4 + nowcamera.y,
				charaguideimg, TRUE);
		}
		else {
			DrawTurnGraph(Xline[charaput] - 56 + nowcamera.x, Yline[charaput] - 4 + nowcamera.y,
				charaguideimg, TRUE);
		}
		//Xline(横位置)の計算
		recSetLine(Xline, mapeff.move.x, time.now, 3);
		//Yline(縦位置)の計算
		if (system.backLight == 0) {
			recSetLine(Yline, mapeff.move.y, time.now, 3);
		}
		else {
			recSetLine(Yline, mapeff.move.y, time.now, 5);
		}
		//判定マーカーの表示
		for (i[0] = 0; i[0] < 3; i[0]++) {
			DrawGraph(Xline[i[0]] + nowcamera.x, Yline[i[0]] + nowcamera.y, judghimg, TRUE);
		}
		//キャラグラ変換
		G[3] = 0;
		//get chara position
		charaput = GetCharaPos3(time.now, mapdata.note, objectNG, &keyhold, hitatk[0], hitatk[1]);
		G[4] = Yline[charaput];
		//キャラグラフィックを表示
		if (keyhold.z == 1) {
			hitpose = (hitpose + 1) % 2;
		}
		if (keyhold.x == 1) {
			hitpose = (hitpose + 1) % 2;
		}
		if (keyhold.c == 1) {
			hitpose = (hitpose + 1) % 2;
		}
		if ((GetNowCount() - charahit > 50) &&
			(keyhold.up == 1 || keyhold.down == 1 || keyhold.left == 1 || keyhold.right == 1)) {
			charahit = 0;
		}
		if (GetNowCount() - charahit > 250) { G[5] = 0; }
		else { G[5] = pals(250, 0, 0, 50, GetNowCount() - charahit); }
		if (charahit > 0) {
			G[0] = Xline[charaput] + G[5] + nowcamera.x;
			G[1] = G[4] - 75 + nowcamera.y;
			G[2] = betweens(24 + hitpose * 6, (GetNowCount() - charahit) / 125 + 24 + hitpose * 6, 29 + hitpose * 6);
			if (mapeff.carrow.d[mapeff.carrow.num].data == 1) {
				DrawGraph(G[0] - 160, G[1], charaimg[G[2]], TRUE);
			}
			else {
				DrawTurnGraph(G[0] + 30, G[1], charaimg[G[2]], TRUE);
			}
		}
		else {
			if (mapeff.carrow.d[mapeff.carrow.num].data == 1) {
				DrawGraph(Xline[charaput] - 160 + nowcamera.x, G[4] - 75 + nowcamera.y,
					charaimg[time.now * mapeff.v_BPM.data[mapeff.v_BPM.num].BPM / 20000 % 6 +
					mapeff.chamo[charaput].gra[mapeff.chamo[charaput].num] * 6], TRUE);
			}
			else {
				DrawTurnGraph(Xline[0] + 30 + nowcamera.x, G[4] - 75 + nowcamera.y,
					charaimg[time.now * mapeff.v_BPM.data[mapeff.v_BPM.num].BPM / 20000 % 6 +
					mapeff.chamo[charaput].gra[mapeff.chamo[charaput].num] * 6], TRUE);
			}
		}
		//キー設定
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
			AutoAution(&keyhold.z, &keyhold.x, &keyhold.c, &keyhold.up, &keyhold.down,
				&keyhold.left, &keyhold.right, mapdata.note, objectNG, time.now);
		}
		//キー押しヒット解除
		if (1 == keyhold.up || 1 == keyhold.down || 1 == keyhold.left || 1 == keyhold.right || hitatk[1] + 750 < time.now) {
			hitatk[1] = -1000;
		}
		if (key[KEY_INPUT_G] == 0) { holdG = 0; }
		else if (key[KEY_INPUT_G] == 1) { holdG++; }
		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }
		//キャッチ判定に使う数値を計算
		LaneTrack[charaput] = time.now;
		if (keyhold.up == 0 && keyhold.down == 0 || keyhold.up > 0 && keyhold.down > 0) { LaneTrack[1] = time.now; }
		else if (keyhold.up > 0 && keyhold.down == 0) { LaneTrack[0] = time.now; }
		else if (keyhold.up == 0 && keyhold.down > 0) { LaneTrack[2] = time.now; }
		if (LaneTrack[0] <= LaneTrack[2]) { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[0]); }
		else { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[2]); }
		//ヒット
		if (keyhold.z == 1 || keyhold.x == 1 || keyhold.c == 1) charahit = GetNowCount();
		if (charahit + 750 < GetNowCount()) charahit = 0;
		//コンボ表示
		ShowCombo(combo, ComboFontimg);
		//判定表示
		PlayShowJudge(system.judgePos, Xline[charaput], Yline[charaput], &nowcamera);
		/* 音符表示 */
		/* G[0] = viewN+
		 * G[1] = 横位置
		 * G[2] = 縦位置
		 * G[3] = XlockN+
		 * G[4] = YlockN+
		 * G[5] = speedN+
		 * G[6] = color
		 * G[7] = continue or break
		 * i[0] = レーンループ
		 * i[1] = ノーツループ
		 */
		for (int iLine = 0; iLine < 3; iLine++) {
			G[0] = G[3] = G[4] = G[5] = 0;
			{
#if SWITCH_NOTE_BOX_2 == 1
				note_box_2_t *temp = NULL;
				temp = &mapdata.note[0];
#else
				note_box_t *temp = NULL;
				switch (iLine) {
				case 0:
					temp = &mapdata.note2.up[0];
					break;
				case 1:
					temp = &mapdata.note2.mid[0];
					break;
				case 2:
					temp = &mapdata.note2.low[0];
					break;
				}
#endif
				for (int iNote = objectN[iLine]; mapdata.note[iNote].hittime > 0;
#if SWITCH_NOTE_BOX_2 == 1
					iNote = mapdata.note[iNote].next
#else
					iNote++
#endif
					)
				{
					G[7] = DrawNoteOne(G, &mapdata.note[iNote], &mapeff, viewTN, lockN,
						iLine, speedN[iLine], time.now, Xline[iLine], Yline[iLine],
						scroolN, &nowcamera, &noteimg);
					if (G[7] == 1) { continue; }
					else if (G[7] == 2) { break; }
#if SWITCH_NOTE_BOX_2 == 1
					if (mapdata.note[iNote].next == -1) { break; }
#endif
				}
			}
		}
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
#if SWITCH_NOTE_BOX_2 == 1
			G[1] = CheckNearHitNote(&mapdata.note[objectN[0]],
				&mapdata.note[objectN[1]], &mapdata.note[objectN[2]], time.now);
#else
			G[1] = CheckNearHitNote(&mapdata.note2.up[objectN[0]],
				&mapdata.note2.mid[objectN[1]], &mapdata.note2.low[objectN[2]], time.now);
#endif
			if (G[1] == -1) {
				if (i[0] == 0) { p_sound.flag |= SE_SWING; }
				break;
			}
#if SWITCH_NOTE_BOX_2 == 1
			G[2] = mapdata.note[objectN[G[1]]].hittime - time.now;
#else
			switch (G[1]) {
			case 0:
				G[2] = mapdata.note2.up[objectN[G[1]]].hittime - time.now;
				break;
			case 1:
				G[2] = mapdata.note2.mid[objectN[G[1]]].hittime - time.now;
				break;
			case 2:
				G[2] = mapdata.note2.low[objectN[G[1]]].hittime - time.now;
				break;
			}
#endif
			hitatk2 |= 1 << G[1];
			NJ = CheckJudge(G[2]);
			if (NJ == NOTE_JUDGE_MISS) { p_sound.flag |= SE_SWING; }
			note_judge_event(NJ, &Dscore, &mapdata.note[objectN[G[1]]], Sitem, time.now,
				G[2], G[1], &judgeA);
			objectN[G[1]] = mapdata.note[objectN[G[1]]].next;
		}
		SetHitPosByHit(&hitatk[0], hitatk2, time.now);
		for (i[0] = 0; i[0] < 3; i[0]++) {
			/* i[0] = レーンループ */
			judgh = mapdata.note[objectN[i[0]]].hittime - time.now;
			switch (mapdata.note[objectN[i[0]]].object) {
			case NOTE_CATCH:
				if (LaneTrack[i[0]] + SAFE_TIME >=
					mapdata.note[objectN[i[0]]].hittime) {
					while (mapdata.note[objectN[i[0]]].hittime - time.now <= 0 &&
						0 <= mapdata.note[objectN[i[0]]].hittime) {
						note_judge_event(NOTE_JUDGE_JUST, &Dscore,
							&mapdata.note[objectN[i[0]]], Sitem, time.now, 0, i[0],
							&judgeA);
						charahit = 0;
						hitatk[1] = -1000;
						objectN[i[0]] = mapdata.note[objectN[i[0]]].next;
					}
				}
				break;
			case NOTE_UP:
			case NOTE_DOWN:
			case NOTE_LEFT:
			case NOTE_RIGHT:
				if ((CheckJudge(judgh) != NOTE_JUDGE_NONE) && (
					(keyhold.up == 1 &&
						mapdata.note[objectN[i[0]]].object == NOTE_UP) ||
					(keyhold.down == 1 &&
						mapdata.note[objectN[i[0]]].object == NOTE_DOWN) ||
					(keyhold.left == 1 &&
						mapdata.note[objectN[i[0]]].object == NOTE_LEFT) ||
					(keyhold.right == 1 &&
						mapdata.note[objectN[i[0]]].object == NOTE_RIGHT))) {
					note_judge_event(CheckJudge(judgh), &Dscore,
						&mapdata.note[objectN[i[0]]], Sitem, time.now, judgh, i[0],
						&judgeA);
					objectN[i[0]] = mapdata.note[objectN[i[0]]].next;
				}
				break;
			case NOTE_BOMB:
				if (i[0] == charaput && judgh <= 0) {
					note_judge_event(NOTE_JUDGE_MISS, &Dscore,
						&mapdata.note[objectN[i[0]]], Sitem, time.now, 0, i[0], &judgeA);
					objectN[i[0]]++;
				}
				else while (mapdata.note[objectN[i[0]]].hittime - time.now < 0) {
					note_judge_event(NOTE_JUDGE_JUST, &Dscore,
						&mapdata.note[objectN[i[0]]], Sitem, time.now, -JUST_TIME - 1,
						i[0], &judgeA);
					objectN[i[0]] = mapdata.note[objectN[i[0]]].next;
				}
				break;
			case NOTE_GHOST:
				if (judgh < 0) {
					p_sound.flag = PlayNoteHitSound(mapdata.note[objectN[i[0]]],
						MelodySnd, Sitem, p_sound.flag, SE_GHOST);
					objectN[i[0]] = mapdata.note[objectN[i[0]]].next;
				}
				break;
			case NOTE_HIT:
			default:
				/* none */
				break;
			}
			//全ノーツslowmiss
		while (judgh <= -SAFE_TIME && judgh >= -1000000 &&
			mapdata.note[objectN[i[0]]].object >= NOTE_HIT &&
			mapdata.note[objectN[i[0]]].object <= NOTE_RIGHT) {
			note_judge_event(NOTE_JUDGE_MISS, &Dscore, &mapdata.note[objectN[i[0]]],
				Sitem, time.now, -SAFE_TIME, i[0], &judgeA);
				objectN[i[0]] = mapdata.note[objectN[i[0]]].next;
				judgh = mapdata.note[objectN[i[0]]].hittime - time.now;
			}
		}
		PlayNoteHitSound2(&p_sound);
		Mcombo = mins(Mcombo, combo);
		//ヒットエフェクト表示
		PlayShowHitEffect(Xline, Yline, &nowcamera);
		PlayCheckHitEffect();
		//ライフが0未満の時、1毎に減点スコアを20増やす。
		if (life < 0) {
			score.loss = maxs(score.loss - life * 20, score.normal + score.combo);
			life = 0;
		}
		//ライフ上限
		life = maxs(life, 500);
		//スコア計算
		score = GetScore3(score, judge, mapdata.notes, Mcombo);
		//距離計算
		if (drop != 0) { //DROPED
			Dscore.now_dis = Dscore.dis_save;
		}
		else if (mins(time.now - time.offset, 0) > time.end - time.offset) { //CLEARED
			Dscore.now_dis = time.end - time.offset;
			Dscore.add_save = Dscore.add;
		}
		else { //PLAYING
			Dscore.now_dis = mins(time.now - time.offset, 0);
			Dscore.add_save = Dscore.add;
		}
		//スコアバー表示
		DrawGraph(0, 0, sbarimg, TRUE);
		//スコア表示
		ShowScore2(score, HighSrore, time.now);
		//ライフ表示
		G[0] = lins(0, -114, 500, 177, life);
		if (life > 100) {
			DrawGraph(G[0], 3, Lbarimg[0], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(100, 255, 500, 0, life));
			DrawGraph(G[0], 3, Lbarimg[1], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
		else {
			DrawGraph(G[0], 3, Lbarimg[2], TRUE);
		}
		DrawFormatString(440, 10, 0xffffffff, L"%3d", life);
		//距離表示
		UG[0] = 0xffffffff;
		G[1] = 0;
		if ((drop == 0) && (mins(time.now - time.offset, 0) > time.end - time.offset)) {
			G[1] = 1;
			UG[0] = 0xff000000;
		}
		G[0] = (291 * Dscore.now_dis - 136 * time.end + 136 * time.offset) / (time.end - time.offset);
		GD[0] = Dscore.now_dis / 100000.0;
		DrawGraph(G[0], 38, Tbarimg[G[1]], TRUE);
		DrawFormatString(180, 45, UG[0], L"%.3fkm", GD[0] + Dscore.add_save / 1000.0);
		Dscore.point = (int)(GD[0] * 1000 + Dscore.add_save);
		//スコアバー隠し表示
		DrawGraph(0, 0, sbbarimg, TRUE);
		//ランニングステータス表示
		G[0] = GetRemainNotes2(judge, mapdata.notes);
		G[1] = CalPosScore2(score, G[0], mapdata.notes, combo, Mcombo);
		RunningStats2(judge, G[1], HighSrore);
		//部分難易度表示 (only auto mode)
		if (holdG >= 1 && AutoFlag == 1) {
			G[0] = mapdata.ddif[0] * 20 / notzero(mapdata.ddifG[1]) + 155;
			G[1] = mapdata.ddif[24] * 20 / notzero(mapdata.ddifG[1]) + 447;
			for (i[0] = 0; i[0] <= 23; i[0]++)
				DrawLine((G[0] * (24 - i[0]) + G[1] * i[0]) / 24,
					-mapdata.ddif[i[0]] * 34 / notzero(mapdata.ddifG[1]) + 72,
					(G[0] * (23 - i[0]) + G[1] * (1 + i[0])) / 24,
					-mapdata.ddif[i[0] + 1] * 34 / notzero(mapdata.ddifG[1]) + 72, Cr);
			DrawFormatString(490, 80, Cr, L"mdif:%.2f", mapdata.mdif / 100.0);
			DrawFormatString(490, 100, Cr, L"ldif:%.2f", mapdata.ldif / 100.0);
			DrawFormatString(490, 120, Cr, L"mrat:%.2f", DifRate);
			DrawFormatString(490, 140, Cr, L"ndif:%.2f",
				cal_nowdif_p(mapdata.ddif, &time) / 100.0);
			DrawFormatString(490, 160, Cr, L"adif:%.2f",
				(double)lins(0, 0, 1500000, 800, GetAdif()) / 100.0);
#if 0
			/* エラー表示 */
			if (outpoint[1] != 0) {
				DrawFormatString(20, 120, CrR, L"MAPERROR");
				DrawLine(lins(time.offset, 155, time.end, 446, outpoint[0]), 71,
					lins(time.offset, 175, time.end, 465, outpoint[0]), 38, CrR);
			}
#endif
		}
		//判定ずれバー表示
		DrawGraph(219, 460, gapbarimg, TRUE);
		G[0] = gap2.count % 30;
		for (i[0] = 0; i[0] < 30; i[0]++) {
			G[0]--;
			if (G[0] < 0) G[0] += 30;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (510 - G[0] * 17) / 2);
			DrawGraph(318 - gap2.view[i[0]], 460, gaplineimg, TRUE);
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
			DrawGraph(5, 445, KeyViewimg[maxs(keyhold.z, 1)], TRUE);
			DrawGraph(40, 445, KeyViewimg[maxs(keyhold.x, 1)], TRUE);
			DrawGraph(75, 445, KeyViewimg[maxs(keyhold.c, 1)], TRUE);
			DrawGraph(570, 410, KeyViewimg[maxs(keyhold.up, 1)], TRUE);
			DrawGraph(570, 445, KeyViewimg[maxs(keyhold.down, 1)], TRUE);
			DrawGraph(535, 445, KeyViewimg[maxs(keyhold.left, 1)], TRUE);
			DrawGraph(605, 445, KeyViewimg[maxs(keyhold.right, 1)], TRUE);
			if (KeyPushCount[0] == 0) { DrawString(10, 450, L"Z", Cr); }
			else { DrawFormatString(10, 450, Cr, L"%2d", KeyPushCount[0] % 100); }
			if (KeyPushCount[1] == 0) { DrawString(45, 450, L"X", Cr); }
			else { DrawFormatString(45, 450, Cr, L"%2d", KeyPushCount[1] % 100); }
			if (KeyPushCount[2] == 0) { DrawString(80, 450, L"C", Cr); }
			else { DrawFormatString(80, 450, Cr, L"%2d", KeyPushCount[2] % 100); }
			if (KeyPushCount[3] == 0) { DrawString(575, 415, L"↑", Cr); }
			else { DrawFormatString(575, 415, Cr, L"%2d", KeyPushCount[3] % 100); }
			if (KeyPushCount[4] == 0) { DrawString(575, 450, L"↓", Cr); }
			else { DrawFormatString(575, 450, Cr, L"%2d", KeyPushCount[4] % 100); }
			if (KeyPushCount[5] == 0) { DrawString(540, 450, L"←", Cr); }
			else { DrawFormatString(540, 450, Cr, L"%2d", KeyPushCount[5] % 100); }
			if (KeyPushCount[6] == 0) { DrawString(610, 450, L"→", Cr); }
			else { DrawFormatString(610, 450, Cr, L"%2d", KeyPushCount[6] % 100); }
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		//デバック
		fps[fps[60]++] = time.now - fps[61];
		if (fps[60] > 59)fps[60] -= 60;
		fps[61] = time.now;
		G[0] = 0;
		for (i[0] = 0; i[0] <= 59; i[0]++)G[0] += fps[i[0]];
		if (AutoFlag == 1) {
			DrawFormatString(20, 80, Cr, L"FPS: %.1f", 60000.0 / notzero(G[0]));
			DrawFormatString(20, 100, Cr, L"Autoplay");
		}
		//RECR_DEBUG(0, speedN[0]);
		//RECR_DEBUG(1, speedN[1]);
		//RECR_DEBUG(2, speedN[2]);
		//データオーバーフローで警告文表示
#if 0
		if (0 <= mapdata.note2.up[1999].hittime) {
			DrawFormatString(20, 120, CrR, L"UPPER OVER");
		}
		else if (0 <= mapdata.note2.mid[1999].hittime) {
			DrawFormatString(20, 120, CrR, L"MIDDLE OVER");
		}
		else if (0 <= mapdata.note2.low[1999].hittime) {
			DrawFormatString(20, 120, CrR, L"LOWER OVER");
		}
#endif
		//ライフが20%以下の時、危険信号(ピクチャ)を出す
		if (life <= 100 && drop == 0) DrawGraph(0, 0, dangerimg, TRUE);
		//ライフがなくなったらDROPED扱い
		if (life <= 0 && drop == 0 && AutoFlag == 0) {
			drop = 1;
			Dscore.add_save = Dscore.add;
			Dscore.dis_save = mins(time.now - time.offset, 0);
		}
		if (drop) { DrawGraph(0, 0, dropimg, TRUE); }
		//ノーツが全部なくなった瞬間の時間を記録
		if (GetRemainNotes2(judge, mapdata.notes) == 0 && AllNotesHitTime < 0) {
			AllNotesHitTime = GetNowCount();
		}
		//オートでなく、ノーミス以上を出したら演出
		if (AutoFlag == 0 && AllNotesHitTime + 2000 > GetNowCount()) {
			ShowBonusEff(judge, AllNotesHitTime);
		}
		//終了時間から5秒以上たって、曲が終了したらカットイン再生。
		if (closeFg == 0 &&
			time.end + 5000 <= time.now &&
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
						SetCurrentPositionSoundMem((int)((double)time.now / 1000.0 * 44100.0), musicmp3);
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
					time.now = mins(time.now - 10000, 0);
#if SWITCH_NOTE_BOX_2 == 1
					for (i[0] = 0; i[0] < 3; i[0]++) {
						for (objectN[i[0]] = 0; mapdata.note[objectN[i[0]]].lane != NOTE_LANE_UP; objectN[i[0]] += 0) {
							objectN[i[0]]++;
						}
						while (mapdata.note[objectN[i[0]]].hittime < time.now) {
							objectN[i[0]]++;
						}
					}
#else
					while (time.now < mapdata.note2.up[objectN[0]].hittime ||
						mapdata.note2.up[objectN[0]].hittime < 0) {
						objectN[0]--;
					}
					while (time.now < mapdata.note2.mid[objectN[1]].hittime ||
						mapdata.note2.mid[objectN[1]].hittime < 0) {
						objectN[1]--;
					}
					while (time.now < mapdata.note2.low[objectN[2]].hittime ||
						mapdata.note2.low[objectN[2]].hittime < 0) {
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
					mapeff.chamo[0].num = 0;
					mapeff.chamo[1].num = 0;
					mapeff.chamo[2].num = 0;
					mapeff.fall.num = 0;
					mapeff.move.y[0].num = 0;
					mapeff.move.y[1].num = 0;
					mapeff.move.y[2].num = 0;
					mapeff.move.y[3].num = 0;
					mapeff.move.y[4].num = 0;
					mapeff.move.x[0].num = 0;
					mapeff.move.x[1].num = 0;
					mapeff.move.x[2].num = 0;
					LineMoveN[0] = 0;
					LineMoveN[1] = 0;
					LineMoveN[2] = 0;
					lockN[0] = 0;
					lockN[1] = 0;
					mapeff.carrow.num = 0;
					viewTN = 0;
					MovieN = 0;
				}
				key3 = 1;
			}
			else if (CheckHitKey(KEY_INPUT_RIGHT) == 1) {
				if (key3 == 0) {
					time.now += 10000;
#if SWITCH_NOTE_BOX_2 == 1
					for (i[0] = 0; i[0] < 3; i[0]++) {
						while (mapdata.note[objectN[i[0]]].hittime < time.now) {
							objectN[i[0]]++;
						}
					}
#else
					while (mapdata.note2.up[objectN[0]].hittime < time.now) {
						objectN[0]++;
					}
					while (mapdata.note2.mid[objectN[1]].hittime < time.now) {
						objectN[1]++;
					}
					while (mapdata.note2.low[objectN[2]].hittime < time.now) {
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
					mapeff.chamo[0].num = 0;
					mapeff.chamo[1].num = 0;
					mapeff.chamo[2].num = 0;
					mapeff.fall.num = 0;
					mapeff.move.y[0].num = 0;
					mapeff.move.y[1].num = 0;
					mapeff.move.y[2].num = 0;
					mapeff.move.y[3].num = 0;
					mapeff.move.y[4].num = 0;
					mapeff.move.x[0].num = 0;
					mapeff.move.x[1].num = 0;
					mapeff.move.x[2].num = 0;
					LineMoveN[0] = 0;
					LineMoveN[1] = 0;
					LineMoveN[2] = 0;
					lockN[0] = 0;
					lockN[1] = 0;
					mapeff.carrow.num = 0;
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
			time.now = GetNowCount() - Stime + system.offset * 5;
		}
		else {
			Stime = GetNowCount() - time.now + system.offset * 5;
		}
		ScreenFlip();
	}
	INIT_PIC();
	if (AutoFlag == 1) { return SCENE_SERECT; }
	else {
		ret_gap[0] = gap2.sum;
		ret_gap[1] = gap2.count;
		ret_gap[2] = gap2.ssum;
		return result(o, mapdata.Lv, drop, mapdata.mdif, nameset.songN, nameset.DifFN, fileN, judge, score.sum, Mcombo, mapdata.notes, ret_gap, Dscore.point);
	}
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

int CalPosScore2(struct score_box score, int RemainNotes, int Notes, int combo, int MaxCombo) {
	int PosCombo = mins(combo + RemainNotes, MaxCombo);
	return score.normal + 90000 * RemainNotes / Notes + 10000 * PosCombo / Notes;
}

void SetHitPosByHit(int *const hitatk, char const hitflag, int Ntime) {
	int n = 0;
	int ret = 0;
	for (int i = 0; i < 3; i++) {
		if (hitflag & 1 << i) {
			n++;
			ret = i;
		}
	}
	if (n == 0) {
		return;
	}
	if (n >= 2) {
		hitatk[0] = RECR_CHARP_M;
		hitatk[1] = Ntime;
		return;
	}
	switch (ret) {
	case 0:
		hitatk[0] = RECR_CHARP_U;
		break;
	case 1:
		hitatk[0] = RECR_CHARP_M;
		break;
	case 2:
		hitatk[0] = RECR_CHARP_D;
		break;
	}
	hitatk[1] = Ntime;
	return;
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
		DrawGraph(t * CHARA_WIDTH / 2 - CHARA_WIDTH / 2 - i * CHARA_WIDTH - xx / 2 + ROCATION_X, ROCATION_Y, pic[s], TRUE);
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
	DrawFormatString(490, 20, Cr, L"SCORE:%d", s_score);
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
		DrawTriangle(x1, y1, x2, y2, x3, y3, CrG, TRUE);
	}
	else if (judge.safe > 0) {
		DrawTriangle(x1, y1, x2, y2, x3, y3, CrD, TRUE);
	}
	else if (judge.good > 0) {
		DrawTriangle(x1, y1, x2, y2, x3, y3, CrY, TRUE);
	}
	else {
		DrawTriangle(x1, y1, x2, y2, x3, y3, CrC, TRUE);
	}
	if (PosScore < HighScore) {
		DrawTriangle(x1, y1, x3, y3, x4, y4, CrG, TRUE);
	}
	else {
		DrawTriangle(x1, y1, x3, y3, x4, y4, GetColor(lins(HighScore, 255, 100000, 0, PosScore), 255, lins(HighScore, 0, 100000, 255, PosScore)), TRUE);
	}
}