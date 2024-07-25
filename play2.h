
/* TODO: �m�[�c���ʒu�Œ�o�O���N�����Ă�!!! */
/* TODO: �A�C�e���\���o�O���N�����Ă�!!! */

#if 1 /* define group */

/* include */

#include "general/sancur.h"
#include "general/strcur.h"
#include "general/dxdraw.h"
#include "system.h"
#include "RecScoreFile.h"
#include "RecordLoad2.h"
#include "result.h"
#include "recr_cutin.h"
#include "dxlibcur.h"
#include "define.h"
#include "RecSystem.h"

#include "RecPlay/playbox.h"
#include "RecPlay/PlayBonus.h"
#include "RecPlay/PlayAuto.h"
#include "RecPlay/PlayCamera.h"
#include "RecPlay/PlayHitEff.h"
#include "RecPlay/PlayNoteJudge.h"
#include "RecPlay/PlayViewJudge.h"

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

/* enum */

/* struct */
typedef struct rec_play_back_pic_s {
	int sky = 0;
	int ground = 0;
	int water = 0;
} rec_play_back_pic_t;

#endif /* typedef group */

#if 1 /* proto */

/* proto */
int GetHighScore(wchar_t pas[255], int dif);
struct score_box GetScore3(struct score_box score, struct judge_box judge,
	const int notes, const int MaxCombo);
void Getxxxpng(wchar_t *str, int num);
void Getxxxwav(wchar_t *str, int num);
void ShowCombo(int combo, int *pic);

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

int GetCharaPos3(int time, note_box_2_t note[], short int No[],
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

void RecPlayCalUserPal(rec_play_userpal_t *userpal, short notes, rec_play_time_set_t *time) {
	userpal->Mcombo = mins(userpal->Mcombo, userpal->Ncombo);
	//���C�t��0�����̎��A1���Ɍ��_�X�R�A��20���₷�B
	if (userpal->life < 0) {
		userpal->score.loss = maxs(userpal->score.loss - userpal->life * 20, userpal->score.normal + userpal->score.combo);
		userpal->life = 0;
	}
	//���C�t���
	userpal->life = maxs(userpal->life, 500);
	//�X�R�A�v�Z
	userpal->score = GetScore3(userpal->score, userpal->judgeCount, notes, userpal->Mcombo);
	//�X�e�[�^�X
	if (userpal->life <= 0 && userpal->status == REC_PLAY_STATUS_PLAYING) {
		userpal->status = REC_PLAY_STATUS_DROPED;
		userpal->Dscore.add_save = userpal->Dscore.add;
		userpal->Dscore.dis_save = mins(time->now - time->offset, 0);
	}
	else if (userpal->status != REC_PLAY_STATUS_DROPED &&
		mins(time->now - time->offset, 0) > time->end - time->offset)
	{ //CLEARED
		userpal->status = REC_PLAY_STATUS_CLEARED;
	}
	//�����v�Z
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

static void RecPlayGetMapFileNames(TCHAR *mapPath, TCHAR *songPath, TCHAR *songName,
	int packNo, int musicNo, int LvNo)
{
	int fd = -1;
	TCHAR packName[255];
	TCHAR buf[255];

	/* �p�b�N�̖��O���擾���� */
	fd = FileRead_open(L"RecordPack.txt");
	for (int i = 0; i <= packNo; i++) { FileRead_gets(packName, 256, fd); }
	FileRead_close(fd);

	strcopy_2(L"record/", songPath, 255); /* songPath = record/ */
	strcats_2(songPath, 255, packName);   /* songPath = record/<�p�b�N��> */
	stradds_2(songPath, 255, L'/');       /* songPath = record/<�p�b�N��>/ */

	strcopy_2(songPath, buf, 255);    /* buf = record/<�p�b�N��>/ */
	strcats_2(buf, 255, L"list.txt"); /* buf = record/<�p�b�N��>/list.txt */

	/* �Ȗ����擾���� */
	fd = FileRead_open(buf);
	for (int i = 0; i <= musicNo; i++) FileRead_gets(songName, 256, fd);
	FileRead_close(fd);

	strcats_2(songPath, 255, songName);                 /* songPath = record/<�p�b�N��>/<�Ȗ�> */

	strcopy_2(songPath, mapPath, 255);                  /* mapPath = record/<�p�b�N��>/<�Ȗ�> */
	stradds_2(mapPath, 255, L'/');                      /* mapPath = record/<�p�b�N��>/<�Ȗ�>/ */
	stradds_2(mapPath, 255, (TCHAR)((int)L'0' + LvNo)); /* mapPath = record/<�p�b�N��>/<�Ȗ�>/<��Փx�i���o�[> */
	strcats_2(mapPath, 255, L".rrs");                   /* mapPath = record/<�p�b�N��>/<�Ȗ�>/<��Փx�i���o�[>.rrs */

	return;
}

static void RecGetMapPath(TCHAR *mapPath, int packNo, int musicNo, int LvNo) {
	TCHAR songPath[255];
	TCHAR songName[255];
	RecPlayGetMapFileNames(mapPath, songPath, songName, packNo, musicNo, LvNo);
	return;
}

static int GetRemainNotes2(struct judge_box judge, int Notes) {
	return Notes - judge.just - judge.good - judge.safe - judge.miss;
}

#endif /* sub action */

#if 1 /* Notes Picture */

/**
 * return 0 = normal, 1 = continue, 2 = break;
 */
static int StepViewNoDrawNote(int hittime, rec_map_eff_data_t *mapeff,
	short viewTN, int *viewTadd, int Ntime)
{
	//�\��/��\���i���o�[��i�߂�
	if (hittime >= mapeff->viewT[0][viewTN + *viewTadd + 1] &&
		mapeff->viewT[0][viewTN + *viewTadd + 1] >= 0) {
		(*viewTadd)++;
	}
	//��\���X�L�b�v
	if (hittime - Ntime >= mapeff->viewT[1][viewTN + *viewTadd]) {
		return 1;
	}
	//3�b�u���[�N
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
	//�m�[�c���b�N�i���o�[��i�߂�
	if (note->hittime >= mapeff->lock[0][1][lockN[0] + *XLockNoAdd + 1] &&
		mapeff->lock[0][1][lockN[0] + *XLockNoAdd + 1] >= 0) {
		(*XLockNoAdd)++;
	}
	if (note->hittime >= mapeff->lock[1][1][lockN[1] + *YLockNoAdd + 1] &&
		mapeff->lock[1][1][lockN[1] + *YLockNoAdd + 1] >= 0) {
		(*YLockNoAdd)++;
	}
	// �X�s�[�h�i���o�[��i�߂�
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
	//�c�ʒu
	*DrawY = ((lock1 == 1) ? note->ypos : Yline);
	//���ʒu
	*DrawX = (int)((speedt * 20 * (note->viewtime -
		(scrool->speed * Ntime + scrool->basetime)) + 5000) / 50) + 50;
	*DrawX += ((lock0 == 1) ? note->xpos - 150 : Xline - 150);
	//�F
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
	//�������w�i�\��
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

int CalPosScore2(struct score_box score, int RemainNotes, int Notes, int combo, int MaxCombo) {
	int PosCombo = mins(combo + RemainNotes, MaxCombo);
	return score.normal + 90000 * RemainNotes / Notes + 10000 * PosCombo / Notes;
}

void RunningStats2(struct judge_box judge, int PosScore, int HighScore) {
	const int x1 = 6;
	const int y1 = 6;
	const int x2 = 188;
	const int y2 = 6;
	const int x3 = 150;
	const int y3 = 69;
	const int x4 = 6;
	const int y4 = 69;
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

/* main action */

/**
 * @param[out] ret_map_det map_detail �̎󂯎M
 * @param[out] ret_userpal userpal �̎󂯎M
 * @param[out] ret_nameset nameset �̎󂯎M
 * @param[out] ret_fileN �t�@�C�����̎󂯎M
 * @param[in] p �p�b�N�i���o�[
 * @param[in] n �ȃi���o�[
 * @param[in] o ��Փx�i���o�[
 * @param[in] shift �}�b�v�����t���t
 * @param[in] AutoFlag �I�[�g�v���C�t���O
 * @return now_scene_t ���̃V�[��
 */
now_scene_t RecPlayMain(rec_map_detail_t *ret_map_det, rec_play_userpal_t *ret_userpal,
	rec_play_nameset_t *ret_nameset, TCHAR *ret_fileN, int p, int n, int o, int shift, int AutoFlag)
{
#if 1 /* filter3 */
	/* char */
	char key[256];
	char closeFg = 0;
	/* short */
	short int i[3];
	short int charaput = 1; //�L�����̍��̈ʒu[0�ŏ�,1�Œ�,2�ŉ�]
	short int KeyPushCount[7] = { 0,0,0,0,0,0,0 };
	short int cameraN = 0;
	/* int */
	int charahit = 0; //�L�������m�[�c������������ł��邩�ǂ����B[1�ȏ�Œ@�����A0�Œ@���ĂȂ�]
	int G[20];
	uint UG[5];
	int holdG = 0;
	int key2 = 1;
	int key3 = 1;
	int AllNotesHitTime = -1;
	int LaneTrack[3] = { -150,-150,-150 };
	int StopFrag = -1;
	int scroolN = 0;
	int HighSrore; //�n�C�X�R�A
	rec_play_chara_hit_attack_t hitatk;
	int fps[62];//0�`59=1�t���[���Ԋu�̎���,60=���̑����,61=�O��̎���
	short LineMoveN[3] = { 0,0,0 }; //���̃��C���\���ԍ�
	short int lockN[2] = { 0,0 }; //���̔ԍ�
	short int viewTN = 0;
	int Yline[5] = { 300,350,400,350,600 };//[��,��,��,�n��,����]���[���̏c�ʒu
	int Xline[3] = { 150,150,150 };//[��,��,��]���[���̉��ʒu
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
	short int speedN[5] = { 0,0,0,0,0 }; //���̔ԍ�
	double DifRate; //���ʒ萔
	/* wchar_t */
	wchar_t fileN[255];
	wchar_t dataE[255];
	wchar_t DataFN[255] = L"score/";
	wchar_t GT1[255];
	wchar_t GT2[255];
	wchar_t ST3[] = L".dat";
	int item[99]; //�A�C�e����fd�ADrawGraph�ŌĂׂ�B
	short int itemN = 0; //���̔ԍ�
	int Sitem[99]; //�T�E���h�A�C�e����fd
	short int SitemN = 0; //���̔ԍ�
#endif /* filter3 */
	/* �O���t�B�b�N�ƌ��ʉ� */
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
	int musicmp3;
#define DIV_X 6
#define DIV_Y 6
#define PIC_NUM (DIV_X * DIV_Y)
#define PIC_SIZE_X 160
#define PIC_SIZE_Y 160
	int	charaimg[PIC_NUM];
	/* �s�N�`���̗p�� */
	ReadyBonusPsmat();
	ReadyEffPicture();
	ReadyJudgePicture();
	/* adif�̃��Z�b�g */
	InitAdif();
	/* FILE */
	FILE *fp = NULL;
	/* action */
	for (i[0] = 0; i[0] <= 59; i[0]++) { fps[i[0]] = 17; }
	fps[60] = 0;
	fps[61] = 0;
	//�V�X�e�����[�h
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
		RecPlayInitMelodySnd();
		RecPlayInitPsound();
	}

	RecPlayGetMapFileNames(GT1, dataE, fileN, p, n, o);

	/* rrs�f�[�^�̓��e��ǂݍ��� */
	if (rec_score_fread(&recfp, GT1) != 0) {
		/* �ǂݍ��ݎ��s */
		return SCENE_EXIT;
	}

	musicmp3 = LoadSoundMem(recfp.nameset.mp3FN);
	backpic.sky = LoadGraph(recfp.nameset.sky);
	backpic.ground = LoadGraph(recfp.nameset.ground);
	backpic.water = LoadGraph(recfp.nameset.water);
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
	//�Q�[���J�n�O�̉�����
	recfp.mapdata.notes = notzero(recfp.mapdata.notes);
	GD[0] = recfp.mapdata.mdif / 100.0 - recfp.mapdata.Lv;//mdif�Ɠ�Փx�\�L�̍�
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
	//�Q�[���J�n
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

		//�L�[�ݒ�
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
		//�I�[�g�v���C�p�R�[�h
		else if (AutoFlag == 1) {
			if (key[KEY_INPUT_G] == 0) { holdG = 0; }
			else if (key[KEY_INPUT_G] == 1) { holdG++; }
			AutoAution(&keyhold, recfp.mapdata.note, objectNG, recfp.time.now);
		}

		//�J�����ړ�
		RecPlaySetCamera(recfp.mapeff.camera, cameraN, recfp.time.now);
		//Xline(���ʒu)�̌v�Z
		recSetLine(Xline, recfp.mapeff.move.x, recfp.time.now, 3);
		//Yline(�c�ʒu)�̌v�Z
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
		//�L�[�����q�b�g����
		if (1 == keyhold.up || 1 == keyhold.down || 1 == keyhold.left || 1 == keyhold.right || hitatk.time + 750 < recfp.time.now) {
			hitatk.time = -1000;
		}
		//�L���b�`����Ɏg�����l���v�Z
		LaneTrack[charaput] = recfp.time.now;
		if (keyhold.up == 0 && keyhold.down == 0 || keyhold.up > 0 && keyhold.down > 0) { LaneTrack[1] = recfp.time.now; }
		else if (keyhold.up > 0 && keyhold.down == 0) { LaneTrack[0] = recfp.time.now; }
		else if (keyhold.up == 0 && keyhold.down > 0) { LaneTrack[2] = recfp.time.now; }
		if (LaneTrack[0] <= LaneTrack[2]) { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[0]); }
		else { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[2]); }
		//�q�b�g
		if (keyhold.z == 1 || keyhold.x == 1 || keyhold.c == 1) { charahit = GetNowCount(); }
		if (charahit + 750 < GetNowCount()) { charahit = 0; }
		/* �m�[�c���� */
		RecJudgeAllNotes(recfp.mapdata.note, objectN, recfp.time.now, Sitem,
			system.soundEn, &keyhold, &hitatk, LaneTrack, &charahit, charaput, &userpal);
		RecPlayCalUserPal(&userpal, recfp.mapdata.notes, &recfp.time);

		ClearDrawScreen(); /* �`��G���A�������� */
		//�w�i�\��
		if (system.backLight != 0) {
			PlayDrawBackGround(&recfp.mapeff, speedN, scroolN, Yline, &backpic, item);
		}
		//�t�B���^�[�\��
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
		//�A�C�e���\��
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
		//����}�[�J�[�̕\��
		for (i[0] = 0; i[0] < 3; i[0]++) {
			DrawGraphRecField(Xline[i[0]], Yline[i[0]], judghimg);
		}
		/* �L�����\�� */
		PlayDrawChara(&keyhold, charahit, Xline, Yline,
			charaput, recfp.time.now, &recfp.mapeff, charaimg);
		//�R���{�\��
		ShowCombo(userpal.Ncombo, ComboFontimg);
		//����\��
		PlayShowJudge(system.judgePos, Xline[charaput], Yline[charaput]);
		/* �����\�� */
		RecPlayDrawNoteAll(objectN, recfp.mapdata.note, &recfp.mapeff, viewTN,
			lockN, speedN, recfp.time.now, Xline, Yline, scroolN, &noteimg);
		//�q�b�g�G�t�F�N�g�\��
		PlayShowHitEffect(Xline, Yline);
		PlayCheckHitEffect();
		//�X�R�A�o�[�\��
		RecRescaleDrawGraph(0, 0, sbarimg, TRUE);
		//�X�R�A�\��
		ShowScore2(userpal.score, HighSrore, recfp.time.now);
		//���C�t�\��
		G[0] = lins(0, -114, 500, 177, userpal.life);
		if (userpal.life > 100) {
			RecRescaleDrawGraph(G[0], 3, Lbarimg[0], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(100, 255, 500, 0, userpal.life));
			RecRescaleDrawGraph(G[0], 3, Lbarimg[1], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
		else {
			RecRescaleDrawGraph(G[0], 3, Lbarimg[2], TRUE);
		}
		RecRescaleDrawFormatString(440, 10, 0xffffffff, L"%3d", userpal.life);
		//�����\��
		UG[0] = 0xffffffff;
		G[1] = 0;
		if (userpal.status == REC_PLAY_STATUS_CLEARED) {
			G[1] = 1;
			UG[0] = 0xff000000;
		}
		G[0] = (291 * userpal.Dscore.now_dis - 136 * recfp.time.end + 136 * recfp.time.offset) / (recfp.time.end - recfp.time.offset);
		RecRescaleDrawGraph(G[0], 38, Tbarimg[G[1]], TRUE);
		RecRescaleDrawFormatString(180, 45, UG[0], L"%.3fkm", userpal.Dscore.point / 1000.0);
		//�X�R�A�o�[�B���\��
		RecRescaleDrawGraph(0, 0, sbbarimg, TRUE);
		//�����j���O�X�e�[�^�X�\��
		G[0] = GetRemainNotes2(userpal.judgeCount, recfp.mapdata.notes);
		G[1] = CalPosScore2(userpal.score, G[0], recfp.mapdata.notes, userpal.Ncombo, userpal.Mcombo);
		RunningStats2(userpal.judgeCount, G[1], HighSrore);
		//������Փx�\�� (only auto mode)
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
			/* �G���[�\�� */
			if (recfp.outpoint[1] != 0) {
				RecRescaleDrawFormatString(20, 120, CrR, L"MAPERROR");
				RecRescaleDrawLine(lins(recfp.time.offset, 155, recfp.time.end, 446, recfp.outpoint[0]), 71,
					lins(recfp.time.offset, 175, recfp.time.end, 465, recfp.outpoint[0]), 38, CrR);
			}
#endif
		}
		//���肸��o�[�\��
		RecRescaleDrawGraph(219, 460, gapbarimg, TRUE);
		G[0] = userpal.gap.count % 30;
		for (i[0] = 0; i[0] < 30; i[0]++) {
			G[0]--;
			if (G[0] < 0) G[0] += 30;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (510 - G[0] * 17) / 2);
			RecRescaleDrawGraph(318 - userpal.gap.view[i[0]], 460, gaplineimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 225);
		}
		//�L�[�����󋵕\��(�I�v�V����)
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
			if (KeyPushCount[3] == 0) { RecRescaleAnchorDrawString(575, 415, L"��", Cr, REC_RESCALE_BOTTOM_RIGHT); }
			else { RecRescaleAnchorDrawFormatString(575, 415, Cr, REC_RESCALE_BOTTOM_RIGHT, L"%2d", KeyPushCount[3] % 100); }
			if (KeyPushCount[4] == 0) { RecRescaleAnchorDrawString(575, 450, L"��", Cr, REC_RESCALE_BOTTOM_RIGHT); }
			else { RecRescaleAnchorDrawFormatString(575, 450, Cr, REC_RESCALE_BOTTOM_RIGHT, L"%2d", KeyPushCount[4] % 100); }
			if (KeyPushCount[5] == 0) { RecRescaleAnchorDrawString(540, 450, L"��", Cr, REC_RESCALE_BOTTOM_RIGHT); }
			else { RecRescaleAnchorDrawFormatString(540, 450, Cr, REC_RESCALE_BOTTOM_RIGHT, L"%2d", KeyPushCount[5] % 100); }
			if (KeyPushCount[6] == 0) { RecRescaleAnchorDrawString(610, 450, L"��", Cr, REC_RESCALE_BOTTOM_RIGHT); }
			else { RecRescaleAnchorDrawFormatString(610, 450, Cr, REC_RESCALE_BOTTOM_RIGHT, L"%2d", KeyPushCount[6] % 100); }
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		//�f�o�b�N
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
		//�f�[�^�I�[�o�[�t���[�Ōx�����\��
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
		//�m�[�c���S���Ȃ��Ȃ����u�Ԃ̎��Ԃ��L�^
		if (GetRemainNotes2(userpal.judgeCount, recfp.mapdata.notes) == 0 && AllNotesHitTime < 0) {
			AllNotesHitTime = GetNowCount();
		}
		//�I�[�g�łȂ��A�m�[�~�X�ȏ���o�����牉�o
		if (AutoFlag == 0 && AllNotesHitTime + 2000 > GetNowCount()) {
			ShowBonusEff(userpal.judgeCount, AllNotesHitTime);
		}
		//�I�����Ԃ���5�b�ȏソ���āA�Ȃ��I��������J�b�g�C���Đ��B
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

		//�J�b�g�C���Đ�����2�b�ȏソ�����甲����B
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

	*ret_map_det = recfp.mapdata;
	*ret_userpal = userpal;
	*ret_nameset = recfp.nameset;
	strcopy_2(fileN, ret_fileN, 255);

	return SCENE_SERECT;
}

/**
 * @param[in] packNo �p�b�N�i���o�[
 * @param[in] musicNo �ȃi���o�[
 * @param[in] difNo ��Փx�i���o�[
 * @param[in] shift �}�b�v�����t���t
 * @param[in] AutoFlag �I�[�g�v���C�t���O
 * @return ���̃V�[��
 */
now_scene_t play3(int packNo, int musicNo, int difNo, int shift, int AutoFlag) {
	TCHAR mapPath[255];
	TCHAR fileName[255];
	rec_map_detail_t map_detail;
	rec_play_userpal_t userpal;
	rec_play_nameset_t nameset;
	now_scene_t ret = SCENE_EXIT;
	FILE *fp = NULL;

	/* rrs�f�[�^�������A�܂��͍쐬�̎w��������΍�� */
	if (shift == 0) {
		RecGetMapPath(mapPath, packNo, musicNo, difNo);
		_wfopen_s(&fp, mapPath, L"rb");
	}
	if (fp == NULL) {
		RecordLoad2(packNo, musicNo, difNo);
		cal_ddif_3(mapPath);
	}
	else {
		fclose(fp);
	}

	/* TODO: RecPlayMain �̕Ԃ�l�� int �ɕς��� */
	ret = RecPlayMain(&map_detail, &userpal, &nameset,
		fileName, packNo, musicNo, difNo, shift, AutoFlag);

	if (ret == SCENE_EXIT) { return SCENE_EXIT; }
	if (AutoFlag == 1) { return SCENE_SERECT; }

	return result(&map_detail, &userpal, &nameset, difNo, fileName);
}

#if 1 /* under proto group */

/* proto */

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

#endif /* under proto group */
