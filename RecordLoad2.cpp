
#include <DxLib.h>
#include <strcur.h>
#include <sancur.h>
#include <system.h>
#include <RecScoreFile.h>
#include <playbox.h>
#include <recp_cal_ddif_2.h>

#if 1 /* typedef */

typedef enum rrs_obj_code_e {
	OBJ_CODE_NONE = -1,
	OBJ_CODE_MEMO = 0,
	OBJ_CODE_SPEED,
	OBJ_CODE_BPM,
	OBJ_CODE_VBPM,
	OBJ_CODE_CHARA,
	OBJ_CODE_MOVE,
	OBJ_CODE_XMOV,
	OBJ_CODE_DIV,
	OBJ_CODE_GMOVE,
	OBJ_CODE_XLOCK,
	OBJ_CODE_YLOCK,
	OBJ_CODE_CARROW,
	OBJ_CODE_FALL,
	OBJ_CODE_VIEW,
	OBJ_CODE_MOVIE,
	OBJ_CODE_INIT_ITEM_SET,
	OBJ_CODE_ADD_ITEM_SET,
	OBJ_CODE_ITEM_SET,
	OBJ_CODE_CAMERA,
	OBJ_CODE_CAMMOVE,
	OBJ_CODE_SCROOL,
	OBJ_CODE_CUSTOM,
	OBJ_CODE_END,
	OBJ_CODE_SPACE,
} rrs_obj_code_t;

typedef enum rec_map_move_code_e {
	REC_MAP_MOVE_CODE_LIN = 1,
	REC_MAP_MOVE_CODE_ACC,
	REC_MAP_MOVE_CODE_DEC,
	REC_MAP_MOVE_CODE_MOM,
	REC_MAP_MOVE_CODE_SLI,
	REC_MAP_MOVE_CODE_PAL,
	REC_MAP_MOVE_CODE_EDG,
} rec_map_move_code_t;

typedef struct ddef_box {
	int maxdif = 0;
	int lastdif = 0;
	int nowdifsection = 1;
	int datanum = -1;
} ddef_box;

typedef struct rec_map_move_pal_s {
	int StartLine = 0;
	int EndLine = 0;
	int AllGap = 0;
	int MoveMode = 0;
	double StartTime = 0;
	double EndTime = 0;
	double MovePos = 0;
} rec_map_move_pal_t;

#endif /* typedef */

#if 1 /* proto */

int IsNoteCode(wchar_t c);
int cal_ddif(int num, int const *difkey, int Etime, int noteoff, int difsec, int voidtime);
int cal_nowdif_m(int *difkey, int num, int now, int voidtime);
rrs_obj_code_t check_obj_code(wchar_t const *const s);
void set_item_set(item_box *const Movie, short *const MovieN,
	playnum_box *allnum, wchar_t *const s, item_set_box const *const item_set,
	double bpmG, double timer);
item_eff_box set_pic_mat(wchar_t *s);
int MapErrorCheck(int nownote, int nowtime, int befnote, int beftime, int dif, int wl);

#endif /* proto */

#if 1 /* sub action 2 */

double SETbpm(wchar_t *p1) {
	strmods(p1, 5);
	return strsans2(p1);
}

int SEToffset(wchar_t *p1) {
	strmods(p1, 12);
	return strsans(p1);
}

int SETLv(wchar_t *p1) {
	strmods(p1, 7);
	return strsans(p1);
}

int shifttime(double n, double bpm, int time) {
	return (int)(time + 240000.0 * (n - 1.0) / (bpm * 16.0));
}

void SETMove(rec_move_data_t *Buff, double StartTime, double MovePoint,
	double EndTime, double MoveType, double bpm, double NowTime)
{
	Buff->Stime = shifttime(StartTime, bpm, (int)NowTime);
	Buff->pos = (int)(MovePoint * 50.0 + 100.0);
	Buff->Etime = shifttime(EndTime, bpm, (int)NowTime) - 5;
	Buff->mode = (int)MoveType;
}

static note_material GetNoteObjMat(TCHAR code) {
	switch (code) {
	case L'H':
		return NOTE_HIT;
	case L'C':
		return NOTE_CATCH;
	case L'U':
		return NOTE_UP;
	case L'D':
		return NOTE_DOWN;
	case L'L':
		return NOTE_LEFT;
	case L'R':
		return NOTE_RIGHT;
	case L'B':
		return NOTE_BOMB;
	case L'G':
		return NOTE_GHOST;
	}
	return NOTE_NONE;
}

static void CalNoteViewTime(note_box_2_t *note, rec_scrool_set_t *scrool) {
	int num = 0;
	while (0 <= scrool->data[num + 1].starttime &&
		scrool->data[num + 1].starttime <= note->hittime) {
		num++;
	}
	note->viewtime = note->hittime * scrool->data[num].speed + scrool->data[num].basetime;
	return;
}

void RecMapLoadSetMove(rec_move_set_t *move, unsigned int *allnum, int iLine,
	double StartTime, double MovePos, double EndTime, int MoveMode, double bpmG,
	double timer[])
{
	double Spos = (move->d[move->num - 1].pos - 100.0) / 50.0;
	switch (MoveMode) {
	case REC_MAP_MOVE_CODE_LIN:
	case REC_MAP_MOVE_CODE_ACC:
	case REC_MAP_MOVE_CODE_DEC:
		SETMove(&move->d[move->num], StartTime, MovePos, EndTime, MoveMode, bpmG, timer[0]);
		move->num++;
		allnum[iLine]++;
		break;
	case REC_MAP_MOVE_CODE_MOM:
		RecMapLoadSetMove(move, allnum, iLine, StartTime, MovePos,
			EndTime, REC_MAP_MOVE_CODE_LIN, bpmG, timer);
		move->d[move->num - 1].Stime -= 5;
		move->d[move->num - 1].Etime -= 5;
		break;
	case REC_MAP_MOVE_CODE_SLI:
		RecMapLoadSetMove(move, allnum, iLine, StartTime, (Spos + MovePos) / 2.0,
			(StartTime + EndTime) / 2.0, REC_MAP_MOVE_CODE_ACC, bpmG, timer);
		RecMapLoadSetMove(move, allnum, iLine, (StartTime + EndTime) / 2.0,
			MovePos, EndTime, REC_MAP_MOVE_CODE_DEC, bpmG, timer);
		break;
	case REC_MAP_MOVE_CODE_PAL:
		RecMapLoadSetMove(move, allnum, iLine, StartTime, MovePos,
			(StartTime + EndTime) / 2.0, REC_MAP_MOVE_CODE_DEC, bpmG, timer);
		RecMapLoadSetMove(move, allnum, iLine, (StartTime + EndTime) / 2.0,
			Spos, EndTime, REC_MAP_MOVE_CODE_ACC, bpmG, timer);
		break;
	case REC_MAP_MOVE_CODE_EDG:
		RecMapLoadSetMove(move, allnum, iLine, StartTime, MovePos,
			(StartTime + EndTime) / 2.0, REC_MAP_MOVE_CODE_ACC, bpmG, timer);
		RecMapLoadSetMove(move, allnum, iLine, (StartTime + EndTime) / 2.0,
			Spos, EndTime, REC_MAP_MOVE_CODE_DEC, bpmG, timer);
		break;
	}
	return;
}

static int RecMapLoadGetc(TCHAR c, int istr, note_box_2_t note[], int *objectN, int iLine,
	double timer[], int noteLaneNo[], double bpmG, int BlockNoteNum,
	struct custom_note_box customnote[], rec_scrool_set_t *scrool, rec_move_all_set_t *move,
	short int YmoveN2[], short int XmoveN2[], short int *notes, playnum_box *allnum)
{
	TCHAR strcode = L'0';
	if (IsNoteCode(c) == 0) { return -1; }
	if (0 <= noteLaneNo[iLine]) {
		note[noteLaneNo[iLine]].next = *objectN;
	}
	noteLaneNo[iLine] = *objectN;
	switch (iLine) {
	case 0:
		note[*objectN].lane = NOTE_LANE_UP;
		break;
	case 1:
		note[*objectN].lane = NOTE_LANE_MID;
		break;
	case 2:
		note[*objectN].lane = NOTE_LANE_LOW;
		break;
	}
	note[*objectN].hittime = timer[iLine] + 240000 * istr / (bpmG * BlockNoteNum);
	if (L'1' <= c && c <= L'9') {
		strcode = customnote[c - L'1'].note;
	}
	else {
		strcode = c;
	}
	if (strcode == L'?') {
		switch (GetRand(4)) {
		case 0:
			strcode = L'H';
			break;
		case 1:
			strcode = L'U';
			break;
		case 2:
			strcode = L'D';
			break;
		case 3:
			strcode = L'L';
			break;
		case 4:
			strcode = L'R';
			break;
		}
	}
	if (strcode == L'!') {
		switch (GetRand(7)) {
		case 0:
			strcode = L'H';
			break;
		case 1:
			strcode = L'U';
			break;
		case 2:
			strcode = L'D';
			break;
		case 3:
			strcode = L'L';
			break;
		case 4:
			strcode = L'R';
			break;
		case 5:
			strcode = L'C';
			break;
		case 6:
			strcode = L'B';
			break;
		case 7:
			strcode = L'G';
			break;
		}
	}
	note[*objectN].object = GetNoteObjMat(strcode);
	//viewtimeを計算する
	CalNoteViewTime(&note[*objectN], scrool);
	note[*objectN].ypos = 50 * iLine + 300;
	note[*objectN].xpos = 150;
	//縦位置を計算する
	while (move->y[iLine].d[YmoveN2[iLine]].Etime <= note[*objectN].hittime &&
		move->y[iLine].d[YmoveN2[iLine]].Etime >= 0) {
		YmoveN2[iLine]++;
	}
	if (move->y[iLine].d[YmoveN2[iLine]].Stime >= 0 &&
		move->y[iLine].d[YmoveN2[iLine]].Stime <= note[*objectN].hittime &&
		move->y[iLine].d[YmoveN2[iLine]].Etime > note[*objectN].hittime) {
		note[*objectN].ypos = movecal(move->y[iLine].d[YmoveN2[iLine]].mode,
			move->y[iLine].d[YmoveN2[iLine]].Stime,
			move->y[iLine].d[YmoveN2[iLine] - 1].pos,
			move->y[iLine].d[YmoveN2[iLine]].Etime, move->y[iLine].d[YmoveN2[iLine]].pos,
			note[*objectN].hittime);
	}
	else {
		note[*objectN].ypos = move->y[iLine].d[YmoveN2[iLine] - 1].pos;
	}
	//横位置を計算する
	while (move->x[iLine].d[XmoveN2[iLine]].Etime <= note[*objectN].hittime &&
		move->x[iLine].d[XmoveN2[iLine]].Etime >= 0) {
		XmoveN2[iLine]++;
	}
	if (move->x[iLine].d[XmoveN2[iLine]].Stime >= 0 &&
		move->x[iLine].d[XmoveN2[iLine]].Stime <= note[*objectN].hittime &&
		move->x[iLine].d[XmoveN2[iLine]].Etime > note[*objectN].hittime) {
		note[*objectN].xpos = movecal(move->x[iLine].d[XmoveN2[iLine]].mode,
			move->x[iLine].d[XmoveN2[iLine]].Stime,
			move->x[iLine].d[XmoveN2[iLine] - 1].pos,
			move->x[iLine].d[XmoveN2[iLine]].Etime, move->x[iLine].d[XmoveN2[iLine]].pos,
			note[*objectN].hittime);
	}
	else {
		note[*objectN].xpos = move->x[iLine].d[XmoveN2[iLine] - 1].pos;
	}
	//効果音を設定する
	if (L'1' <= c && c <= L'9') {
		note[*objectN].sound = customnote[c - L'1'].sound;
		note[*objectN].melody = customnote[c - L'1'].melody;
	}
	else {
		note[*objectN].sound = 0;
	}
	//色を設定する
	if (L'1' <= c && c <= L'9') {
		note[*objectN].color = customnote[c - L'1'].color;
	}
	else {
		note[*objectN].color = 0;
	}
	if (note[*objectN].object != 8) {
		(*notes)++;
	}
	(*objectN)++;
	allnum->notenum[iLine]++;
	return 0;
}

enum melodysound RecMapLoad_GetMelSnd(TCHAR str[]) {
	enum melodysound ret;
	switch (str[1]) {
	case L'F':
		ret = (enum melodysound)(LOW_F + (str[0] == L'H' ? 12 : 0) + (str[2] == L'#' ? 1 : 0));
		break;
	case L'G':
		ret = (enum melodysound)(LOW_G + (str[0] == L'H' ? 12 : 0) + (str[2] == L'#' ? 1 : 0));
		break;
	case L'A':
		ret = (enum melodysound)(LOW_A + (str[0] == L'H' ? 12 : 0) + (str[2] == L'#' ? 1 : 0));
		break;
	case L'B':
		ret = (enum melodysound)(LOW_B + (str[0] == L'H' ? 12 : 0));
		break;
	case L'C':
		ret = (enum melodysound)(LOW_C + (str[0] == L'H' ? 12 : 0) + (str[2] == L'#' ? 1 : 0));
		break;
	case L'D':
		ret = (enum melodysound)(LOW_D + (str[0] == L'H' ? 12 : 0) + (str[2] == L'#' ? 1 : 0));
		break;
	case L'E':
		ret = (enum melodysound)(LOW_E + (str[0] == L'H' ? 12 : 0));
		break;
	default:
		ret = MELODYSOUND_NONE;
		break;
	}
	return ret;
}

void RecMapLoad_ComCustomNote(TCHAR str[], struct custom_note_box customnote[]) {
	int No = 0;
	struct custom_note_box *ptr;
	strmods(str, 8);
	No = strsans2(str) - 1;
	ptr = &customnote[No];
	ptr->color = 0;
	ptr->melody = MELODYSOUND_NONE;
	ptr->note = 0;
	ptr->sound = 0;
	strnex(str);
	while (str[0] != L'\0') {
		if (strands(str, L"NOTE=")) {
			strmods(str, 5);
			ptr->note = str[0];
		}
		else if (strands(str, L"SOUND=")) {
			strmods(str, 6);
			if (str[0] == L'L' || str[0] == L'H') {
				ptr->melody = RecMapLoad_GetMelSnd(str);
			}
			else {
				ptr->sound = strsans2(str);
			}
		}
		else if (strands(str, L"COLOR=")) {
			strmods(str, 6);
			ptr->color = strsans2(str);
		}
		else {
			break;
		}
		strnex(str);
	}
	return;
}

#endif /* sub action 2 */

#if 1 /* sub action */

static void RecMapLoad_SetInitRecfp(rec_score_file_t *recfp) {
	recfp->mapeff.camera.data[0].starttime = 0;
	recfp->mapeff.camera.data[0].endtime = 0;
	recfp->mapeff.camera.data[0].xpos = 0;
	recfp->mapeff.camera.data[0].ypos = 0;
	recfp->mapeff.camera.data[0].zoom = 1;
	recfp->mapeff.camera.data[0].rot = 0;
	recfp->mapeff.camera.data[0].mode = 0;
	recfp->mapeff.camera.num = 1;
	recfp->mapeff.scrool.data[0].starttime = 0;
	recfp->mapeff.scrool.data[0].basetime = 0;
	recfp->mapeff.scrool.data[0].speed = 1;
	recfp->mapeff.scrool.num = 1;
	recfp->mapeff.viewT[0][0] = 0;
	recfp->mapeff.viewT[1][0] = 3000;
	recfp->mapeff.carrow.d[0].data = 1;
	recfp->mapeff.carrow.d[0].time = 0;
	recfp->mapeff.carrow.num = 1;
	recfp->mapeff.lock[0][0][0] = -1;
	recfp->mapeff.lock[0][1][0] = 0;
	recfp->mapeff.lock[1][0][0] = 1;
	recfp->mapeff.lock[1][1][0] = 0;
	recfp->mapeff.move.y[0].d[0].Stime = 0;
	recfp->mapeff.move.y[0].d[0].pos = 300;
	recfp->mapeff.move.y[0].d[0].Etime = 0;
	recfp->mapeff.move.y[0].d[0].mode = 1;
	recfp->mapeff.move.y[0].num = 1;
	recfp->mapeff.move.y[1].d[0].Stime = 0;
	recfp->mapeff.move.y[1].d[0].pos = 350;
	recfp->mapeff.move.y[1].d[0].Etime = 0;
	recfp->mapeff.move.y[1].d[0].mode = 1;
	recfp->mapeff.move.y[1].num = 1;
	recfp->mapeff.move.y[2].d[0].Stime = 0;
	recfp->mapeff.move.y[2].d[0].pos = 400;
	recfp->mapeff.move.y[2].d[0].Etime = 0;
	recfp->mapeff.move.y[2].d[0].mode = 1;
	recfp->mapeff.move.y[2].num = 1;
	recfp->mapeff.move.y[3].d[0].Stime = 0;
	recfp->mapeff.move.y[3].d[0].pos = 350;
	recfp->mapeff.move.y[3].d[0].Etime = 0;
	recfp->mapeff.move.y[3].d[0].mode = 1;
	recfp->mapeff.move.y[3].num = 1;
	recfp->mapeff.move.y[4].d[0].Stime = 0;
	recfp->mapeff.move.y[4].d[0].pos = 600;
	recfp->mapeff.move.y[4].d[0].Etime = 0;
	recfp->mapeff.move.y[4].d[0].mode = 1;
	recfp->mapeff.move.y[4].num = 1;
	recfp->mapeff.move.x[0].d[0].Stime = 0;
	recfp->mapeff.move.x[0].d[0].pos = 150;
	recfp->mapeff.move.x[0].d[0].Etime = 0;
	recfp->mapeff.move.x[0].d[0].mode = 1;
	recfp->mapeff.move.x[0].num = 1;
	recfp->mapeff.move.x[1].d[0].Stime = 0;
	recfp->mapeff.move.x[1].d[0].pos = 150;
	recfp->mapeff.move.x[1].d[0].Etime = 0;
	recfp->mapeff.move.x[1].d[0].mode = 1;
	recfp->mapeff.move.x[1].num = 1;
	recfp->mapeff.move.x[2].d[0].Stime = 0;
	recfp->mapeff.move.x[2].d[0].pos = 150;
	recfp->mapeff.move.x[2].d[0].Etime = 0;
	recfp->mapeff.move.x[2].d[0].mode = 1;
	recfp->mapeff.move.x[2].num = 1;
	recfp->mapeff.chamo[0].gra[0] = 0;
	recfp->mapeff.chamo[0].time[0] = 0;
	recfp->mapeff.chamo[0].num = 1;
	recfp->mapeff.chamo[1].gra[0] = 1;
	recfp->mapeff.chamo[1].time[0] = 0;
	recfp->mapeff.chamo[1].num = 1;
	recfp->mapeff.chamo[2].gra[0] = 1;
	recfp->mapeff.chamo[2].time[0] = 0;
	recfp->mapeff.chamo[2].num = 1;
	recfp->mapeff.fall.num = 1;
	recfp->mapeff.speedt[0][0][0] = 0;
	recfp->mapeff.speedt[0][0][1] = 1;
	recfp->mapeff.speedt[1][0][0] = 0;
	recfp->mapeff.speedt[1][0][1] = 1;
	recfp->mapeff.speedt[2][0][0] = 0;
	recfp->mapeff.speedt[2][0][1] = 1;
	recfp->mapeff.speedt[3][0][0] = 0;
	recfp->mapeff.speedt[3][0][1] = 1;
	recfp->mapeff.speedt[4][0][0] = 0;
	recfp->mapeff.speedt[4][0][1] = 1;
	return;
}

static void RecMapLoad_SetEndRecfp(rec_score_file_t *recfp, double *timer, uint objectN[],
	uint lockN0, uint lockN1)
{
	//譜面の最後にgoustを置く
#if SWITCH_NOTE_BOX_2 == 1
	recfp->mapdata.note[*objectN].lane = NOTE_LANE_MID;
	recfp->mapdata.note[*objectN].hittime = timer[0];
	recfp->mapdata.note[*objectN + 1].hittime = -1;
	recfp->mapdata.note[*objectN].object = NOTE_GHOST;
	recfp->mapdata.note[*objectN].ypos = 1000;
#else
	for (uint iLane = 0; iLane < 3; iLane++) {
		recfp->mapdata.note[iLane][objectN[iLine]].hittime = timer[iLane];
		recfp->mapdata.note[iLane][objectN[iLine] + 1].hittime = -1;
		recfp->mapdata.note[iLane][objectN[iLine]].object = NOTE_GHOST;
		recfp->mapdata.note[iLane][objectN[iLine]].ypos = 1000;
	}
#endif
	recfp->mapeff.lock[0][0][lockN0] = 1;
	recfp->mapeff.lock[0][1][lockN0] = -1;
	recfp->mapeff.lock[1][0][lockN1] = -1;
	recfp->mapeff.lock[1][1][lockN1] = -1;
#if SWITCH_NOTE_BOX_2 == 1
	recfp->allnum.notenum[1]++;
#else
	recfp->allnum.notenum[0]++;
	recfp->allnum.notenum[1]++;
	recfp->allnum.notenum[2]++;
#endif
	recfp->time.end = timer[0];
	return;
}

static void RecMapLoad_SaveMap(const TCHAR *dataE, rec_score_file_t *recfp, int difNo) {
	int dummy = 0;
	TCHAR RRS[255]; //PC用譜面データの保存場所
	FILE *fp;

	//ここからPC用譜面データのファイルの作成(セーブ作業)
	strcopy(dataE, RRS, 1); //"record/<パック名>/<曲名>/"
	stradds_2(RRS, 255, (TCHAR)(_T('0') + difNo)); //"record/<パック名>/<曲名>/<難易度>"
	strcats_2(RRS, 255, _T(".rrs")); //"record/<パック名>/<曲名>/<難易度>.rrs"

	_wfopen_s(&fp, RRS, L"wb");
	fwrite(&recfp->allnum, sizeof(playnum_box), 1, fp);//各データの個数
	fwrite(&recfp->nameset.mp3FN, 255, 1, fp);//音楽ファイル名
	fwrite(&recfp->mapdata.bpm, sizeof(double), 1, fp);//BPM
	fwrite(&recfp->time.offset, sizeof(int), 1, fp);//offset
	fwrite(&recfp->nameset.sky, 255, 1, fp);//空背景名
	fwrite(&recfp->nameset.ground, 255, 1, fp);//地面画像名
	fwrite(&recfp->nameset.water, 255, 1, fp);//水中画像名
	fwrite(&recfp->nameset.songN, 255, 1, fp);//曲名
	fwrite(&recfp->nameset.songNE, 255, 1, fp);//曲名(英語)
	fwrite(&recfp->mapdata.Lv, sizeof(short int), 1, fp);//レベル
	//fwrite(&item, sizeof(int), 99, fp);//アイテム画像データ(動作未確認)
	{
		int buf[99][2];
		for (int inum = 0; inum < 99; inum++) {
			buf[inum][0] = recfp->mapeff.fall.d[inum].No;
			buf[inum][1] = recfp->mapeff.fall.d[inum].time;
		}
		fwrite(&buf, sizeof(int), 198, fp);//落ち物背景切り替えタイミング
	}
	fwrite(&recfp->mapeff.speedt, sizeof(double), 990, fp);//レーン速度
	{
		int buf[3][99][2];
		for (int ilane = 0; ilane < 3; ilane++) {
			for (int inum = 0; inum < 99; inum++) {
				buf[ilane][inum][0] = recfp->mapeff.chamo[ilane].gra[inum];
				buf[ilane][inum][1] = recfp->mapeff.chamo[ilane].time[inum];
			}
		}
		fwrite(&buf, sizeof(int), 594, fp);//キャラグラ変換タイミング
	}
	fwrite(&recfp->mapeff.move.y[0].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[0], fp);//上レーン縦位置移動タイミング
	fwrite(&recfp->mapeff.move.y[1].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[1], fp);//中レーン縦位置移動タイミング
	fwrite(&recfp->mapeff.move.y[2].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[2], fp);//下レーン縦位置移動タイミング
	fwrite(&recfp->mapeff.move.y[3].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[3], fp);//地面縦位置移動タイミング
	fwrite(&recfp->mapeff.move.y[4].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[4], fp);//水面縦位置移動タイミング
	fwrite(&recfp->mapeff.move.x[0].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[0], fp);//上レーン横位置移動タイミング
	fwrite(&recfp->mapeff.move.x[1].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[1], fp);//中レーン横位置移動タイミング
	fwrite(&recfp->mapeff.move.x[2].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[2], fp);//下レーン横位置移動タイミング
	fwrite(&recfp->mapeff.lock, sizeof(int), 396, fp);//ノーツ固定切り替えタイミング
	{
		int buf[2][99];
		for (int inum = 0; inum < 99; inum++) {
			buf[0][inum] = recfp->mapeff.carrow.d[inum].data;
			buf[1][inum] = recfp->mapeff.carrow.d[inum].time;
		}
		fwrite(&buf, sizeof(int), 198, fp);//キャラ向き切り替えタイミング
	}
	fwrite(&recfp->mapeff.viewT, sizeof(int), 198, fp);//ノーツ表示時間変換タイミング
#if SWITCH_NOTE_BOX_2
	fwrite(&recfp->mapdata.note, sizeof(note_box_2_t), recfp->allnum.notenum[0] + recfp->allnum.notenum[1] + recfp->allnum.notenum[2], fp); /* ノーツデータ */
#else
	fwrite(&recfp->mapdata.note[0], sizeof(struct note_box), recfp->allnum.notenum[0], fp); /* 上レーンノーツデータ */
	fwrite(&recfp->mapdata.note[1], sizeof(struct note_box), recfp->allnum.notenum[1], fp); /* 中レーンノーツデータ */
	fwrite(&recfp->mapdata.note[2], sizeof(struct note_box), recfp->allnum.notenum[2], fp); /* 下レーンノーツデータ */
#endif
	fwrite(&recfp->mapdata.notes, sizeof(short int), 1, fp);//ノーツ数
	fwrite(&recfp->time.end, sizeof(int), 1, fp);//曲終了時間
	fwrite(&dummy, sizeof(int), 1, fp);//最高難易度
	fwrite(&dummy, sizeof(int), 1, fp);//最終難易度
	fwrite(&recfp->mapdata.ddif, sizeof(int), 25, fp);//各区間難易度データ
	fwrite(&dummy, sizeof(int), 1, fp);//各区間難易度データ
	fwrite(&recfp->mapdata.ddifG[1], sizeof(int), 1, fp);//各区間難易度データ
	fwrite(&recfp->nameset.DifFN, 255, 1, fp);//難易度バー名
	fwrite(&recfp->mapeff.Movie, sizeof(item_box), recfp->allnum.movienum, fp);//動画データ
	fwrite(&recfp->mapeff.camera, sizeof(rec_camera_data_t), 255, fp);//カメラデータ
	fwrite(&recfp->mapeff.scrool, sizeof(rec_scrool_data_t), 99, fp);//スクロールデータ
	fwrite(&recfp->mapeff.v_BPM.data[0], sizeof(view_BPM_box), recfp->allnum.v_BPMnum, fp);//見た目のBPMデータ
	fwrite(&recfp->outpoint, sizeof(int), 2, fp);//譜面エラー
	fclose(fp);
	return;
}

static void RecMapLoad_EncodeMap(rec_score_file_t *recfp, const TCHAR *mapPath, const TCHAR *folderPath, int o) {
	//o: 難易度ナンバー
	short int i[2] = { 0,0 };
#if 0 /* fixing... */
	int G[4] = { 0,0,0,0 };
#else
	int G[14] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
#endif
	int noteoff = 0; //ノーツのオフセット
	int Etime = 0; //譜面の終わりの時間
	int waningLv = 2;
	double GD[5] = { 0,0,0,0,0 };
	//int item[99]; //アイテムのアドレス、DrawGraphで呼べる。
	//short int itemN = 0; //↑の番号
	short int YmoveN2[3] = { 0,0,0 };
	short int XmoveN2[3] = { 0,0,0 };
	short int lockN[2] = { 1,1 }; //↑の番号
	short int viewTN = 1;
	item_set_box item_set[99];
	short int MovieN = 0;
	struct custom_note_box customnote[9];
	int objectN = 0; //↑の番号
	int noteLaneNo[3] = { -1,-1,-1 };
	int difkey[50][4];//難易度計算に使う[番号][入力キー,時間,難易度点,[0]個数上限:[1]今の番号:[2]1個前の番号:[3]2個前の番号:[4]最高点:[5]データ個数:[6]最後50個の合計:[7]計算から除外する時間]
	difkey[0][2] = 0;
	difkey[1][2] = 0;
	difkey[1][3] = 0;
	difkey[2][3] = -1;
	difkey[3][3] = -2;
	difkey[4][3] = 0;
	ddef_box ddif2;
	double bpmG = 120;
	double timer[3] = { 0,0,0 }; //[上, 中, 下]レーンの時間
	short int speedN[5] = { 1,1,1,1,1 }; //↑の番号
	TCHAR RRS[255]; //PC用譜面データの保存場所
	TCHAR GT1[255];
	DxFile_t songdata = 0;
	FILE *fp;

	RecMapLoad_SetInitRecfp(recfp);
	songdata = FileRead_open(mapPath);
	if (songdata == 0) { return; }

	//テキストデータを読む
	while (FileRead_eof(songdata) == 0) {
		FileRead_gets(GT1, 256, songdata);
		//音楽ファイルを読み込む
		if (strands(GT1, L"#MUSIC:")) {
			strmods(GT1, 7);
			strcopy(folderPath, recfp->nameset.mp3FN, 1);
			strcats(recfp->nameset.mp3FN, GT1);
		}
		//BPMを読み込む
		else if (strands(GT1, L"#BPM:")) {
			bpmG = recfp->mapdata.bpm = SETbpm(GT1);
			recfp->mapeff.v_BPM.data[0].time = recfp->time.offset;
			recfp->mapeff.v_BPM.data[0].BPM = (unsigned short)recfp->mapdata.bpm;
		}
		//ノートのオフセットを読み込む
		else if (strands(GT1, L"#NOTEOFFSET:")) {
			timer[0] = timer[1] = timer[2] = recfp->time.offset = SEToffset(GT1);
		}
		//空の背景を読み込む
		else if (strands(GT1, L"#SKY:")) {
			strcopy(L"picture/", recfp->nameset.sky, 1);
			strmods(GT1, 5);
			strcats(recfp->nameset.sky, GT1);
		}
		//地面の画像を読み込む
		else if (strands(GT1, L"#FIELD:")) {
			strcopy(L"picture/", recfp->nameset.ground, 1);
			strmods(GT1, 7);
			strcats(recfp->nameset.ground, GT1);
		}
		//水中の画像を読み込む
		else if (strands(GT1, L"#WATER:")) {
			strcopy(L"picture/", recfp->nameset.water, 1);
			strmods(GT1, 7);
			strcats(recfp->nameset.water, GT1);
		}
		//難易度バー(another)を読み込む
		else if (strands(GT1, L"#DIFBAR:")) {
			strcopy(folderPath, recfp->nameset.DifFN, 1);
			strmods(GT1, 8);
			strcats(recfp->nameset.DifFN, GT1);
		}
		//曲名を読み込む
		else if (strands(GT1, L"#TITLE:")) {
			strmods(GT1, 7);
			strcopy(GT1, recfp->nameset.songN, 1);
		}
		//英語
		else if (strands(GT1, L"#E.TITLE:")) {
			strmods(GT1, 7);
			strcopy(GT1, recfp->nameset.songNE, 1);
		}
		//レベルを読み込む
		else if (strands(GT1, L"#LEVEL:")) recfp->mapdata.Lv = SETLv(GT1);
		//落ち物背景指定
		else if (strands(GT1, L"#FALL:")) {
			strmods(GT1, 6);
			recfp->mapeff.fall.d[0].No = strsans(GT1);
			recfp->mapeff.fall.d[0].time = 0;
		}
		//譜面難易度フィルタのレベル
		else if (strands(GT1, L"#WANING:")) {
			strmods(GT1, 8);
			waningLv = strsans(GT1);
		}
		//譜面を読み込む
		else if (strands(GT1, L"#MAP:")) {
			break;
		}
	}

	FileRead_gets(GT1, 256, songdata);
	while (FileRead_eof(songdata) == 0 && strands(GT1, L"#END") == 0) {
		switch (check_obj_code(GT1)) {
		case OBJ_CODE_MEMO: //コメント
		case OBJ_CODE_SPACE: //空白
			break;
		case OBJ_CODE_SPEED: //ノーツの速度変化
			G[0] = betweens(0, GT1[6] - 49, 4);
			strmods(GT1, 8);
			recfp->mapeff.speedt[G[0]][speedN[G[0]]][1] = strsans2(GT1);
			strnex(GT1);
			if (GT1[0] >= L'0' && GT1[0] <= L'9' || GT1[0] == L'-') {
				recfp->mapeff.speedt[G[0]][speedN[G[0]]][0] = timer[G[0]] + 240000 * (recfp->mapeff.speedt[G[0]][speedN[G[0]]][1] - 1) / (bpmG * 16) - 10;
				recfp->mapeff.speedt[G[0]][speedN[G[0]]][1] = strsans2(GT1);
			}
			else {
				recfp->mapeff.speedt[G[0]][speedN[G[0]]][0] = timer[G[0]] - 10;
			}
			speedN[G[0]]++;
			break;
		case OBJ_CODE_BPM: //データ処理のBPM変化
			bpmG = SETbpm(GT1);
			break;
		case OBJ_CODE_VBPM: //見た目のBPM変化
			strmods(GT1, 7);
			recfp->mapeff.v_BPM.data[recfp->allnum.v_BPMnum].time = shifttime(strsans(GT1), bpmG, (int)timer[0]);
			strnex(GT1);
			recfp->mapeff.v_BPM.data[recfp->allnum.v_BPMnum].BPM = strsans(GT1);
			recfp->allnum.v_BPMnum++;
			break;
		case OBJ_CODE_CHARA: //キャラグラ変化
			G[0] = GT1[6] - 49;
			strmods(GT1, 8);
			recfp->mapeff.chamo[G[0]].gra[recfp->mapeff.chamo[G[0]].num] = betweens(0, strsans(GT1), 2);
			recfp->mapeff.chamo[G[0]].time[recfp->mapeff.chamo[G[0]].num] = (int)timer[G[0]];
			recfp->mapeff.chamo[G[0]].num++;
			break;
		case OBJ_CODE_MOVE: //縦移動
			if (GT1[8] == L'A') {
				G[0] = 0;
				G[2] = 2;
				G[3] = 0;
			}
			else if (GT1[8] == L'B') {
				G[0] = 0;
				G[2] = 2;
				G[3] = 1;
			}
			else if (GT1[8] == L'C') {
				G[0] = 0;
				G[2] = 2;
				G[3] = 2;
			}
			else if (GT1[8] == L'D') {
				G[0] = 0;
				G[2] = 2;
				G[3] = 3;
			}
			else {
				G[2] = G[0] = betweens(0, GT1[8] - 49, 4);
				G[3] = 0;
			}
			switch (GT1[5]) {
			case('l'):
				G[1] = 1;
				break;
			case('a'):
				G[1] = 2;
				break;
			case('d'):
				G[1] = 3;
				break;
			case('m'):
				G[1] = 4;
				break;
			case('s'):
				G[1] = 5;
				break;
			case('p'):
				G[1] = 6;
				break;
			case('e'):
				G[1] = 7;
				break;
			}
			strmods(GT1, 10);
			GD[0] = strsans2(GT1);
			strnex(GT1);
			GD[1] = strsans2(GT1);
			strnex(GT1);
			GD[2] = strsans2(GT1);
			for (i[0] = G[0]; i[0] <= G[2]; i[0]++) {
				RecMapLoadSetMove(&recfp->mapeff.move.y[i[0]], recfp->allnum.Ymovenum, i[0], GD[0],
					GD[1] + G[3] * i[0] - G[3], GD[2], G[1], bpmG, timer);
			}
			break;
		case OBJ_CODE_XMOV: //横移動
			if (GT1[8] == L'A') {
				G[0] = 0;
				G[2] = 2;
				G[3] = 0;
			}
			else if (GT1[8] == L'B') {
				G[0] = 0;
				G[2] = 2;
				G[3] = 1;
			}
			else if (GT1[8] == L'C') {
				G[0] = 0;
				G[2] = 2;
				G[3] = 2;
			}
			else if (GT1[8] == L'D') {
				G[0] = 0;
				G[2] = 2;
				G[3] = 3;
			}
			else {
				G[2] = G[0] = betweens(0, GT1[8] - 49, 2);
				G[3] = 0;
			}
			switch (GT1[5]) {
			case('l'):
				G[1] = 1;
				break;
			case('a'):
				G[1] = 2;
				break;
			case('d'):
				G[1] = 3;
				break;
			case('m'):
				G[1] = 4;
				break;
			case('s'):
				G[1] = 5;
				break;
			case('p'):
				G[1] = 6;
				break;
			case('e'):
				G[1] = 7;
				break;
			}
			strmods(GT1, 10);
			GD[0] = strsans2(GT1);
			strnex(GT1);
			GD[1] = strsans2(GT1);
			strnex(GT1);
			GD[2] = strsans2(GT1);
			for (i[0] = G[0]; i[0] <= G[2]; i[0]++) {
				RecMapLoadSetMove(&recfp->mapeff.move.x[i[0]], recfp->allnum.Xmovenum, i[0], GD[0],
					GD[1] + G[3] * i[0] - G[3], GD[2], G[1], bpmG, timer);
			}
			break;
		case OBJ_CODE_DIV: //振動
			G[0] = betweens(0, GT1[5] - L'1', 2);
			G[1] = 0;
			if (GT1[4] == L'Y') { G[1] = 1; }
			strmods(GT1, 7);
			GD[0] = strsans2(GT1);//開始時間
			strnex(GT1);
			GD[1] = strsans2(GT1);//振動位置
			strnex(GT1);
			GD[2] = strsans2(GT1) / 2.0;//往復時間
			strnex(GT1);
			GD[3] = strsans2(GT1);//往復回数
			if (G[1] == 1) {
				for (i[0] = 0; i[0] < GD[3]; i[0]++) {
					SETMove(&recfp->mapeff.move.y[G[0]].d[recfp->mapeff.move.y[G[0]].num], GD[0],
						GD[1], GD[0] + GD[2], 1, bpmG, timer[0]);
					SETMove(&recfp->mapeff.move.y[G[0]].d[recfp->mapeff.move.y[G[0]].num + 1], GD[0] + GD[2],
						(recfp->mapeff.move.y[G[0]].d[recfp->mapeff.move.y[G[0]].num - 1].pos - 100.0) / 50.0,
						GD[0] + GD[2] * 2, 1, bpmG, timer[0]);
					GD[0] += GD[2] * 2;
					recfp->mapeff.move.y[G[0]].num += 2;
					recfp->allnum.Ymovenum[G[0]] += 2;
				}
			}
			else {
				for (i[0] = 0; i[0] < GD[3]; i[0]++) {
					SETMove(&recfp->mapeff.move.x[G[0]].d[recfp->mapeff.move.x[G[0]].num], GD[0],
						GD[1], GD[0] + GD[2], 1, bpmG, timer[0]);
					SETMove(&recfp->mapeff.move.x[G[0]].d[recfp->mapeff.move.x[G[0]].num] + 1, GD[0] + GD[2],
						(recfp->mapeff.move.x[G[0]].d[recfp->mapeff.move.x[G[0]].num - 1].pos - 100.0) / 50.0,
						GD[0] + GD[2] * 2, 1, bpmG, timer[0]);
					GD[0] += GD[2] * 2;
					recfp->mapeff.move.x[G[0]].num += 2;
					recfp->allnum.Xmovenum[G[0]] += 2;
				}
			}
			break;
		case OBJ_CODE_GMOVE: //GMOVE
			G[0] = 3;
			switch (GT1[6]) {
			case('l'):
				G[1] = 1;
				break;
			case('a'):
				G[1] = 2;
				break;
			case('d'):
				G[1] = 3;
				break;
			case('m'):
				G[1] = 4;
				break;
			case('s'):
				G[1] = 5;
				break;
			case('p'):
				G[1] = 6;
				break;
			}
			strmods(GT1, 10);
			GD[0] = strsans2(GT1);
			strnex(GT1);
			GD[1] = strsans2(GT1);
			strnex(GT1);
			GD[2] = strsans2(GT1);
			RecMapLoadSetMove(&recfp->mapeff.move.y[3], recfp->allnum.Ymovenum, 3,
				GD[0], GD[1], GD[2], G[1], bpmG, timer);
			break;
		case OBJ_CODE_XLOCK: //横ロック
			strmods(GT1, 7);
			recfp->mapeff.lock[0][0][lockN[0]] = recfp->mapeff.lock[0][0][lockN[0] - 1] * -1;
			recfp->mapeff.lock[0][1][lockN[0]] = shifttime(strsans(GT1), bpmG, (int)timer[0]);
			lockN[0]++;
			break;
		case OBJ_CODE_YLOCK: //縦ロック
			strmods(GT1, 7);
			recfp->mapeff.lock[1][0][lockN[1]] = recfp->mapeff.lock[1][0][lockN[1] - 1] * -1;
			recfp->mapeff.lock[1][1][lockN[1]] = shifttime(strsans(GT1), bpmG, (int)timer[0]);
			lockN[1]++;
			break;
		case OBJ_CODE_CARROW: //キャラ向き変化
			strmods(GT1, 8);
			recfp->mapeff.carrow.d[recfp->mapeff.carrow.num].data = recfp->mapeff.carrow.d[recfp->mapeff.carrow.num - 1].data * -1;
			recfp->mapeff.carrow.d[recfp->mapeff.carrow.num].time = shifttime(strsans(GT1), bpmG, (int)timer[0]);
			recfp->mapeff.carrow.num++;
			break;
		case OBJ_CODE_FALL: //落ち物背景切り替え
			strmods(GT1, 6);
			recfp->mapeff.fall.d[recfp->mapeff.fall.num].No = strsans(GT1);
			strnex(GT1);
			recfp->mapeff.fall.d[recfp->mapeff.fall.num].time = shifttime(strsans(GT1), bpmG, (int)timer[0]);
			recfp->mapeff.fall.num++;
			break;
		case OBJ_CODE_VIEW: //音符表示時間
			strmods(GT1, 6);
			recfp->mapeff.viewT[0][viewTN] = shifttime(strsans(GT1), bpmG, (int)timer[0]);
			strnex(GT1);
			recfp->mapeff.viewT[1][viewTN] = strsans(GT1);
			viewTN++;
			break;
		case OBJ_CODE_MOVIE: //アイテム表示
			strmods(GT1, 7);
			recfp->mapeff.Movie[MovieN].ID = strsans(GT1);
			strnex(GT1);
			switch (GT1[0]) {
			case L'l':
				recfp->mapeff.Movie[MovieN].movemode = 1;
				break;
			case L'a':
				recfp->mapeff.Movie[MovieN].movemode = 2;
				break;
			case L'd':
				recfp->mapeff.Movie[MovieN].movemode = 3;
				break;
			}
			strnex(GT1);
			recfp->mapeff.Movie[MovieN].starttime = shifttime(strsans2(GT1), bpmG, (int)timer[0]);
			strnex(GT1);
			recfp->mapeff.Movie[MovieN].endtime = shifttime(strsans2(GT1), bpmG, (int)timer[0]);
			strnex(GT1);
			recfp->mapeff.Movie[MovieN].startXpos = (int)(strsans2(GT1) * 50 + 115);
			strnex(GT1);
			recfp->mapeff.Movie[MovieN].endXpos = (int)(strsans2(GT1) * 50 + 115);
			strnex(GT1);
			recfp->mapeff.Movie[MovieN].startYpos = (int)(strsans2(GT1) * 50 + 115);
			strnex(GT1);
			recfp->mapeff.Movie[MovieN].endYpos = (int)(strsans2(GT1) * 50 + 115);
			strnex(GT1);
			recfp->mapeff.Movie[MovieN].startsize = (int)(strsans2(GT1) * 100);
			strnex(GT1);
			recfp->mapeff.Movie[MovieN].endsize = (int)(strsans2(GT1) * 100);
			strnex(GT1);
			recfp->mapeff.Movie[MovieN].startrot = strsans(GT1);
			strnex(GT1);
			recfp->mapeff.Movie[MovieN].endrot = strsans(GT1);
			strnex(GT1);
			recfp->mapeff.Movie[MovieN].startalpha = (int)(strsans2(GT1) * 255.0);
			strnex(GT1);
			recfp->mapeff.Movie[MovieN].endalpha = (int)(strsans2(GT1) * 255.0);
			strnex(GT1);
			recfp->mapeff.Movie[MovieN].eff = set_pic_mat(GT1);
			MovieN++;
			recfp->allnum.movienum++;
			break;
		case OBJ_CODE_INIT_ITEM_SET: //アイテムセット初期化
			strmods(GT1, 15);
			G[0] = strsans(GT1);
			item_set[G[0]].num = 0;
			break;
		case OBJ_CODE_ADD_ITEM_SET: //アイテムセット追加
			if (item_set[G[0]].num <= 10) {
				strmods(GT1, 14);
				G[0] = strsans(GT1); /* G[0] = item setの番号 */
				strnex(GT1);
				item_set[G[0]].picID[item_set[G[0]].num].picID = strsans(GT1);
				strnex(GT1);
				item_set[G[0]].picID[item_set[G[0]].num].Xpos = (int)(strsans2(GT1) * 50);
				strnex(GT1);
				item_set[G[0]].picID[item_set[G[0]].num].Ypos = (int)(strsans2(GT1) * 50);
				strnex(GT1);
				item_set[G[0]].picID[item_set[G[0]].num].size = (int)(strsans2(GT1) * 100);
				strnex(GT1);
				item_set[G[0]].picID[item_set[G[0]].num].rot = strsans(GT1);
				strnex(GT1);
				item_set[G[0]].picID[item_set[G[0]].num].alpha = (int)(strsans2(GT1) * 255);
				strnex(GT1);
				item_set[G[0]].picID[item_set[G[0]].num].eff = set_pic_mat(GT1);
				item_set[G[0]].num++;
			}
			break;
		case OBJ_CODE_ITEM_SET: //アイテムセット表示
#if 0 /* fixing... */
			set_item_set(&recfp->mapeff.Movie[MovieN], &MovieN, &recfp->allnum, &GT1[0], item_set, bpmG, timer[0]);
#else
			strmods(GT1, 10);
			G[0] = strsans(GT1); /* G[0] = item boxの番号 */
			strnex(GT1);
			switch (GT1[0]) {
			case L'l':
				G[1] = 1;
				break;
			case L'a':
				G[1] = 2;
				break;
			case L'd':
				G[1] = 3;
				break;
			} /* G[1] = 移動モード */
			strnex(GT1);
			G[2] = shifttime(strsans2(GT1), bpmG, (int)timer[0]); /* stime */
			strnex(GT1);
			G[3] = shifttime(strsans2(GT1), bpmG, (int)timer[0]); /* etime */
			strnex(GT1);
			G[4] = (int)(strsans2(GT1) * 50 + 115); /* sx */
			strnex(GT1);
			G[5] = strsans2(GT1) * 50 + 115; /* ex */
			strnex(GT1);
			G[6] = strsans2(GT1) * 50 + 115; /* sy */
			strnex(GT1);
			G[7] = strsans2(GT1) * 50 + 115; /* ey */
			strnex(GT1);
			G[8] = strsans2(GT1) * 100; /* ss */
			strnex(GT1);
			G[9] = strsans2(GT1) * 100; /* es */
			strnex(GT1);
			G[10] = strsans(GT1); /* sr */
			strnex(GT1);
			G[11] = strsans(GT1); /* er */
			strnex(GT1);
			G[12] = strsans2(GT1) * 255.0; /* sa */
			strnex(GT1);
			G[13] = strsans2(GT1) * 255.0; /* ea */
			for (i[0] = 0; i[0] < item_set[G[0]].num; i[0]++) {
				recfp->mapeff.Movie[MovieN].ID = item_set[G[0]].picID[i[0]].picID;
				recfp->mapeff.Movie[MovieN].movemode = G[1];
				recfp->mapeff.Movie[MovieN].eff = item_set[G[0]].picID[i[0]].eff;
				recfp->mapeff.Movie[MovieN].starttime = G[2];
				recfp->mapeff.Movie[MovieN].endtime = G[3];
				recfp->mapeff.Movie[MovieN].startXpos = item_set[G[0]].picID[i[0]].Xpos * G[8] / 100;
				recfp->mapeff.Movie[MovieN].endXpos = item_set[G[0]].picID[i[0]].Xpos * G[9] / 100;
				recfp->mapeff.Movie[MovieN].startYpos = item_set[G[0]].picID[i[0]].Ypos * G[8] / 100;
				recfp->mapeff.Movie[MovieN].endYpos = item_set[G[0]].picID[i[0]].Ypos * G[9] / 100;
				rot_xy_pos(G[10], &recfp->mapeff.Movie[MovieN].startXpos, &recfp->mapeff.Movie[MovieN].startYpos);
				rot_xy_pos(G[10], &recfp->mapeff.Movie[MovieN].endXpos, &recfp->mapeff.Movie[MovieN].endYpos);
				recfp->mapeff.Movie[MovieN].startXpos += G[4];
				recfp->mapeff.Movie[MovieN].endXpos += G[5];
				recfp->mapeff.Movie[MovieN].startYpos += G[6];
				recfp->mapeff.Movie[MovieN].endYpos += G[7];
				recfp->mapeff.Movie[MovieN].startsize = G[8] * item_set[G[0]].picID[i[0]].size / 100;
				recfp->mapeff.Movie[MovieN].endsize = G[9] * item_set[G[0]].picID[i[0]].size / 100;
				recfp->mapeff.Movie[MovieN].startrot = G[10] + item_set[G[0]].picID[i[0]].rot;
				recfp->mapeff.Movie[MovieN].endrot = G[11] + item_set[G[0]].picID[i[0]].rot;
				recfp->mapeff.Movie[MovieN].startalpha = G[12] * item_set[G[0]].picID[i[0]].alpha / 255;
				recfp->mapeff.Movie[MovieN].endalpha = G[13] * item_set[G[0]].picID[i[0]].alpha / 255;
				MovieN++;
				recfp->allnum.movienum++;
			}
#endif
			break;
		case OBJ_CODE_CAMERA: //カメラ移動+ズーム+角度(未実装)
		{
			const uint numC = recfp->mapeff.camera.num;
			strmods(GT1, 8);
			recfp->mapeff.camera.data[numC].starttime = shifttime(strsans2(GT1), bpmG, timer[0]);
			strnex(GT1);
			recfp->mapeff.camera.data[numC].endtime = shifttime(strsans2(GT1), bpmG, timer[0]);
			strnex(GT1);
			recfp->mapeff.camera.data[numC].xpos = strsans2(GT1) * 50;
			strnex(GT1);
			recfp->mapeff.camera.data[numC].ypos = strsans2(GT1) * 50;
			strnex(GT1);
			recfp->mapeff.camera.data[numC].zoom = strsans2(GT1);
			strnex(GT1);
			recfp->mapeff.camera.data[numC].rot = strsans2(GT1);
			strnex(GT1);
			switch (GT1[0]) {
			case L'a':
				recfp->mapeff.camera.data[numC].mode = 2;
				break;
			case L'd':
				recfp->mapeff.camera.data[numC].mode = 3;
				break;
			default:
				recfp->mapeff.camera.data[numC].mode = 1;
				break;
			}
			recfp->mapeff.camera.num++;
			break;
		}
		case OBJ_CODE_CAMMOVE: //カメラ移動
		{
			const uint numC = recfp->mapeff.camera.num;
			if (strands(GT1, L"#CMOV:")) { strmods(GT1, 6); }
			if (strands(GT1, L"#CAMMOVE:")) { strmods(GT1, 9); }
			recfp->mapeff.camera.data[numC].starttime = shifttime(strsans2(GT1), bpmG, timer[0]);
			strnex(GT1);
			recfp->mapeff.camera.data[numC].endtime = shifttime(strsans2(GT1), bpmG, timer[0]);
			strnex(GT1);
			recfp->mapeff.camera.data[numC].xpos = strsans2(GT1) * 50;
			strnex(GT1);
			recfp->mapeff.camera.data[numC].ypos = strsans2(GT1) * 50;
			strnex(GT1);
			switch (GT1[0]) {
			case L'a':
				recfp->mapeff.camera.data[numC].mode = 2;
				break;
			case L'd':
				recfp->mapeff.camera.data[numC].mode = 3;
				break;
			default:
				recfp->mapeff.camera.data[numC].mode = 1;
				break;
			}
			recfp->mapeff.camera.num++;
			break;
		}
		case OBJ_CODE_SCROOL: //スクロール
		{
			const uint numS = recfp->mapeff.scrool.num;
			strmods(GT1, 8);
			recfp->mapeff.scrool.data[numS].starttime = shifttime(strsans2(GT1), bpmG, timer[0]);
			strnex(GT1);
			recfp->mapeff.scrool.data[numS].speed = strsans2(GT1);
			G[0] = recfp->mapeff.scrool.data[numS - 1].speed *
				recfp->mapeff.scrool.data[numS].starttime + recfp->mapeff.scrool.data[numS - 1].basetime;
			recfp->mapeff.scrool.data[numS].basetime = G[0] - recfp->mapeff.scrool.data[numS].speed *
				recfp->mapeff.scrool.data[numS].starttime;
			recfp->mapeff.scrool.num++;
			break;
		}
		case OBJ_CODE_CUSTOM: //カスタムノーツセット
			RecMapLoad_ComCustomNote(GT1, customnote);
			break;
		default:
			//これ以外
			for (int iLine = 0; iLine <= 2; iLine++) {
				int BlockNoteNum = 0;
				G[0] = 0;
				while (GT1[BlockNoteNum] != L'\0' && GT1[BlockNoteNum] != L',') { BlockNoteNum++; }
				for (int istr = 0; istr < BlockNoteNum; istr++) {
					RecMapLoadGetc(GT1[istr], istr, recfp->mapdata.note, &objectN, iLine, timer,
						noteLaneNo, bpmG, BlockNoteNum, customnote, &recfp->mapeff.scrool,
						&recfp->mapeff.move, YmoveN2, XmoveN2, &recfp->mapdata.notes,
						&recfp->allnum);
				}
				if (iLine <= 1) { FileRead_gets(GT1, 256, songdata); }
			}
			timer[0] = timer[1] = timer[2] += 240000.0 / bpmG;
			break;
		}
		FileRead_gets(GT1, 256, songdata);
	}

	FileRead_close(songdata);
	RecMapLoad_SetEndRecfp(recfp, timer, (uint *)&objectN, lockN[0], lockN[1]);
	RecMapLoad_SaveMap(folderPath, recfp, o);
	return;
}

#endif /* sub action */

/* main action */
void RecordLoad2(int packNo, int songNo, int difNo) {
	TCHAR folderPath[255]; // フォルダのパス
	TCHAR mapPath[255]; // マップのパス

	rec_score_file_t recfp;

	RecGetMusicFolderPath(folderPath, 255, packNo, songNo);
	strcopy_2(folderPath, mapPath, 255);
	stradds_2(mapPath, 255, (TCHAR)(_T('0') + difNo)); //"record/<パック名>/<曲名>/<難易度>"
	strcats_2(mapPath, 255, _T(".txt")); //"record/<パック名>/<曲名>/<難易度>.txt"

	RecMapLoad_EncodeMap(&recfp, mapPath, folderPath, difNo);
	return;
}

#if 1 /* under */

int IsNoteCode(wchar_t c) {
	if (c == L'H' || c == L'C' || c == L'U' || c == L'D' || c == L'L' || c == L'R' || c == L'B' ||
		c == L'G' || c == L'?' || c == L'!' || (L'1' <= c && c <= L'9')) {
		return 1;
	}
	return 0;
}

int cal_ddif(int num, int const *difkey, int Etime, int noteoff, int difsec, int voidtime) {
	int ret = 0;
	int count = 0;
	if (num >= 50) {
		num = 49;
	}
	for (int i = 0; i < num; i++) {
		if (difkey[i * 4 + 1] > (Etime - noteoff) / 25 *
			difsec - voidtime + noteoff) {
			count++;
			ret += difkey[i * 4 + 2];
		}
	}
	if (count == 0) {
		return 0;
	}
	else {
		return ret * 50 / count;
	}
}

int cal_nowdif_m(int *difkey, int num, int now, int voidtime) {
	int ret = 0;
	int count = 0;
	for (int i = 0; i <= num; i++) {
		if (difkey[i * 4 + 2] < 0) {
			break;
		}
		if (difkey[i * 4 + 1] > difkey[now * 4 + 1] - voidtime) {
			ret += difkey[i * 4 + 2];
			count++;
		}
	}
	if (count == 0) {
		return 0;
	}
	else {
		return ret * 50 / count;
	}
}

rrs_obj_code_t check_obj_code(wchar_t const *const s) {
	if (s[0] == L';') { return OBJ_CODE_MEMO; }
	if (strands(s, L"#SPEED")) { return OBJ_CODE_SPEED; }
	if (strands(s, L"#BPM:")) { return OBJ_CODE_BPM; }
	if (strands(s, L"#V-BPM:")) { return OBJ_CODE_VBPM; }
	if (strands(s, L"#CHARA")) { return OBJ_CODE_CHARA; }
	if (strands(s, L"#MOVE")) { return OBJ_CODE_MOVE; }
	if (strands(s, L"#XMOV")) { return OBJ_CODE_XMOV; }
	if (strands(s, L"#CMOV:")) { return OBJ_CODE_CAMMOVE; }
	if (strands(s, L"#DIV")) { return OBJ_CODE_DIV; }
	if (strands(s, L"#GMOVE")) { return OBJ_CODE_GMOVE; }
	if (strands(s, L"#XLOCK")) { return OBJ_CODE_XLOCK; }
	if (strands(s, L"#YLOCK")) { return OBJ_CODE_YLOCK; }
	if (strands(s, L"#CARROW")) { return OBJ_CODE_CARROW; }
	if (strands(s, L"#FALL")) { return OBJ_CODE_FALL; }
	if (strands(s, L"#VIEW:")) { return OBJ_CODE_VIEW; }
	if (strands(s, L"#MOVIE:")) { return OBJ_CODE_MOVIE; }
	if (strands(s, L"#INIT_ITEM_SET:")) { return OBJ_CODE_INIT_ITEM_SET; }
	if (strands(s, L"#ADD_ITEM_SET:")) { return OBJ_CODE_ADD_ITEM_SET; }
	if (strands(s, L"#ITEM_SET:")) { return OBJ_CODE_ITEM_SET; }
	if (strands(s, L"#CAMERA:")) { return OBJ_CODE_CAMERA; }
	if (strands(s, L"#CAMMOVE:")) { return OBJ_CODE_CAMMOVE; }
	if (strands(s, L"#SCROOL:")) { return OBJ_CODE_SCROOL; }
	if (strands(s, L"#CUSTOM:")) { return OBJ_CODE_CUSTOM; }
	if (strands(s, L"#END")) { return OBJ_CODE_END; }
	if (s[0] == L'\0') { return OBJ_CODE_SPACE; }
	return OBJ_CODE_NONE;
}

void set_item_set(item_box *const Movie, short *const MovieN,
	playnum_box *allnum, wchar_t *const s, item_set_box const *const item_set,
	double bpmG, double timer) {
	int itemN = 0;
	int moveM = 1;
	int BStime = 0;
	int BEtime = 0;
	int BSXpos = 0;
	int BEXpos = 0;
	int BSYpos = 0;
	int BEYpos = 0;
	int BSsize = 100;
	int BEsize = 100;
	int BSrot = 0;
	int BErot = 0;
	int BSalpha = 255;
	int BEalpha = 255;
	strmods(s, 10);
	itemN = strsans(s);
	strnex(s);
	switch (s[0]) {
	case L'l':
		moveM = 1;
		break;
	case L'a':
		moveM = 2;
		break;
	case L'd':
		moveM = 3;
		break;
	}
	strnex(s);
	BStime = shifttime(strsans2(s), bpmG, (int)timer);
	strnex(s);
	BEtime = shifttime(strsans2(s), bpmG, (int)timer);
	strnex(s);
	BSXpos = (int)(strsans2(s) * 50 + 115);
	strnex(s);
	BEXpos = strsans2(s) * 50 + 115;
	strnex(s);
	BSYpos = strsans2(s) * 50 + 115;
	strnex(s);
	BEYpos = strsans2(s) * 50 + 115;
	strnex(s);
	BSsize = strsans2(s) * 100;
	strnex(s);
	BEsize = strsans2(s) * 100;
	strnex(s);
	BSrot = strsans(s);
	strnex(s);
	BErot = strsans(s);
	strnex(s);
	BSalpha = strsans2(s) * 255.0;
	strnex(s);
	BEalpha = strsans2(s) * 255.0;
	for (int i = 0; i < item_set[itemN].num; i++) {
		Movie[*MovieN].ID = item_set[itemN].picID[i].picID;
		Movie[*MovieN].movemode = moveM;
		Movie[*MovieN].eff = item_set[itemN].picID[i].eff;
		Movie[*MovieN].starttime = BStime;
		Movie[*MovieN].endtime = BEtime;
		Movie[*MovieN].startXpos = item_set[itemN].picID[i].Xpos * BSsize / 100;
		Movie[*MovieN].endXpos = item_set[itemN].picID[i].Xpos * BEsize / 100;
		Movie[*MovieN].startYpos = item_set[itemN].picID[i].Ypos * BSsize / 100;
		Movie[*MovieN].endYpos = item_set[itemN].picID[i].Ypos * BEsize / 100;
		rot_xy_pos(BSrot, &Movie[*MovieN].startXpos, &Movie[*MovieN].startYpos);
		rot_xy_pos(BSrot, &Movie[*MovieN].endXpos, &Movie[*MovieN].endYpos);
		Movie[*MovieN].startXpos += BSXpos;
		Movie[*MovieN].endXpos += BEXpos;
		Movie[*MovieN].startYpos += BSYpos;
		Movie[*MovieN].endYpos += BEYpos;
		Movie[*MovieN].startsize = BSsize * item_set[itemN].picID[i].size / 100;
		Movie[*MovieN].endsize = BEsize * item_set[itemN].picID[i].size / 100;
		Movie[*MovieN].startrot = BSrot + item_set[itemN].picID[i].rot;
		Movie[*MovieN].endrot = BErot + item_set[itemN].picID[i].rot;
		Movie[*MovieN].startalpha = BSalpha * item_set[itemN].picID[i].alpha / 255;
		Movie[*MovieN].endalpha = BEalpha * item_set[itemN].picID[i].alpha / 255;
		(*MovieN)++;
		(allnum->movienum)++;
	}
	return;
}

item_eff_box set_pic_mat(wchar_t *s) {
	item_eff_box eff;
	while (s[0] != L'\0' && s[0] != L'\n') {
		if (strands(s, L"bpm_a")) {
			eff.bpm_alphr = 1;
		}
		else if (strands(s, L"bpm_s")) {
			eff.bpm_size = 1;
		}
		else if (strands(s, L"lock")) {
			eff.lock = 1;
		}
		else if (strands(s, L"cha_a")) {
			eff.chara_alphr = 1;
		}
		else if (strands(s, L"edge_s")) {
			eff.edge_size = 1;
		}
		else {
			break;
		}
		strnex(s);
	}
	return eff;
}

int MapErrorCheck(int nownote, int nowtime, int befnote, int beftime, int dif, int wl) {
	if (nowtime <= 0 || beftime <= 0) {
		return 0;
	}
	switch (dif * 10 + wl) {
	case 11:
		if (nownote == 1 && befnote == 1 && nowtime - beftime <= 1200) {
			return HITNOTETOONEAR;
		}
		break;
	case 12:
		if (nownote == 1 && befnote == 1 && nowtime - beftime <= 600) {
			return HITNOTETOONEAR;
		}
		break;
	case 13:
		if (nownote == 1 && befnote == 1 && nowtime - beftime <= 300) {
			return HITNOTETOONEAR;
		}
		break;
	case 21:
		if (nownote == 1 && befnote == 1 && nowtime - beftime <= 600) {
			return HITNOTETOONEAR;
		}
		break;
	case 22:
		if (nownote == 1 && befnote == 1 && nowtime - beftime <= 300) {
			return HITNOTETOONEAR;
		}
		break;
	case 23:
		if (nownote == 1 && befnote == 1 && nowtime - beftime <= 150) {
			return HITNOTETOONEAR;
		}
		break;
	default: break;
	}
	return 0;
}

#endif /* under */
