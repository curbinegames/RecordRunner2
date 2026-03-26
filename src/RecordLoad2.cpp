
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <playbox.h>
#include <RecScoreFile.h>
#include <recp_cal_ddif_2.h>
#include <RecSystem.h>

#define REC_MAPENC_BLANK_CHAR     ( _T('0') )
#define REC_MAPENC_HITNOTE_CHAR   ( _T('H') )
#define REC_MAPENC_CATCHNOTE_CHAR ( _T('C') )
#define REC_MAPENC_UPNOTE_CHAR    ( _T('U') )
#define REC_MAPENC_DOWNNOTE_CHAR  ( _T('D') )
#define REC_MAPENC_LEFTNOTE_CHAR  ( _T('L') )
#define REC_MAPENC_RIGHTNOTE_CHAR ( _T('R') )
#define REC_MAPENC_BOMBNOTE_CHAR  ( _T('B') )
#define REC_MAPENC_GHOSTNOTE_CHAR ( _T('G') )
#define REC_MAPENC_RANDOM1_CHAR   ( _T('?') )
#define REC_MAPENC_RANDOM2_CHAR   ( _T('!') )

#define shifttime(num, bpm, Ntime) ( (Ntime) + 240000 * ((num) - 1) / (double)((bpm) * 16) )

#if 1 /* typedef */

typedef enum rec_map_move_code_e {
	REC_MAP_MOVE_CODE_LIN = 1,
	REC_MAP_MOVE_CODE_ACC,
	REC_MAP_MOVE_CODE_DEC,
	REC_MAP_MOVE_CODE_MOM,
	REC_MAP_MOVE_CODE_SLI,
	REC_MAP_MOVE_CODE_PAL,
	REC_MAP_MOVE_CODE_EDG,
} rec_map_move_code_t;

typedef struct item_set_ID {
	short picID = -1;
	item_eff_box eff;
	int Xpos = 0;
	int Ypos = 0;
	int size = 100;
	int rot = 0;
	int alpha = 255;
} item_set_ID;

typedef struct item_set_box {
	item_set_ID picID[10];
	unsigned char num = 0;
	char movemode = 0;
	int starttime = -1000;
	int endtime = -1000;
	int startXpos = 0;
	int endXpos = 0;
	int startYpos = 0;
	int endYpos = 0;
	int startsize = 100;
	int endsize = 100;
	int startrot = 0;
	int endrot = 0;
	int startalpha = 255;
	int endalpha = 255;
} item_set_box;

struct custom_note_box {
	wchar_t note = L'\0';
	int color = 0; /*(only hit note)0=green, 1=red, 2=blue, 3=yellow, 4=black, 5=white*/
	int sound = 0;
	int rand  = 0; /* bit: 8:0, 7:H, 6:C, 5:U, 4:D, 3:L, 2:R, 1:B, 0:G */
	enum melodysound melody = MELODYSOUND_NONE;
};

typedef struct rec_mapenc_data_s {
	double bpmG = 120.0;
	double timer[3] = { 0,0,0 };
	short lockN[2] = { 1,1 };
	short MovieN = 0;
	item_set_box item_set[99];
	struct custom_note_box customnote[9];
	int objectN = 0;
	int noteLaneNo[3] = { 5999,5999,5999 };
	short YmoveN2[3] = { 0,0,0 };
	short XmoveN2[3] = { 0,0,0 };
	DxFile_t songdata = 0;
} rec_mapenc_data_t;

typedef void (*rec_mapenc_noteact_f)(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str);

#endif /* typedef */

#if 1 /* sub action 2 */

static int IsNoteCode(TCHAR c) {
	if ((c == REC_MAPENC_HITNOTE_CHAR) ||
		(c == REC_MAPENC_CATCHNOTE_CHAR) ||
		(c == REC_MAPENC_UPNOTE_CHAR) ||
		(c == REC_MAPENC_DOWNNOTE_CHAR) ||
		(c == REC_MAPENC_LEFTNOTE_CHAR) ||
		(c == REC_MAPENC_RIGHTNOTE_CHAR) ||
		(c == REC_MAPENC_BOMBNOTE_CHAR) ||
		(c == REC_MAPENC_GHOSTNOTE_CHAR) ||
		(c == REC_MAPENC_RANDOM1_CHAR) ||
		(c == REC_MAPENC_RANDOM2_CHAR) ||
		(L'1' <= c && c <= L'9'))
	{
		return 1;
	}
	return 0;
}

static item_eff_box set_pic_mat(TCHAR *s) {
	item_eff_box eff;
	while (s[0] != L'\0' && s[0] != L'\n') {
		if (strands_direct(s, L"bpm_a")) {
			eff.bpm_alphr = 1;
		}
		else if (strands_direct(s, L"bpm_s")) {
			eff.bpm_size = 1;
		}
		else if (strands_direct(s, L"lock")) {
			eff.lock = 1;
		}
		else if (strands_direct(s, L"cha_a")) {
			eff.chara_alphr = 1;
		}
		else if (strands_direct(s, L"edge_s")) {
			eff.edge_size = 1;
		}
		else {
			break;
		}
		strnex(s);
	}
	return eff;
}

static int strrans(const TCHAR *p1) {
	int a, b;
	TCHAR buf[16];
	strcopy_2(p1, buf, 16);
	strmods(buf, 2);
	a = strsans(buf);
	strnex_EX(buf, _T(','));
	b = maxs_2(strsans(buf), a);
	return GetRand(b - a) + a;
}

void SETMove(rec_mapeff_move_st *Buff, double StartTime, double MovePoint,
	double EndTime, double MoveType, double bpm, double NowTime)
{
	Buff->Stime = shifttime(StartTime, bpm, NowTime);
	Buff->pos = (int)(MovePoint * 50.0 + 100.0);
	Buff->Etime = maxs_2(shifttime(EndTime, bpm, NowTime) - 5, Buff->Stime);
	Buff->mode = (int)MoveType;
}

static note_material GetNoteObjMat(TCHAR code) {
	switch (code) {
	case REC_MAPENC_HITNOTE_CHAR:
		return NOTE_HIT;
	case REC_MAPENC_CATCHNOTE_CHAR:
		return NOTE_CATCH;
	case REC_MAPENC_UPNOTE_CHAR:
		return NOTE_UP;
	case REC_MAPENC_DOWNNOTE_CHAR:
		return NOTE_DOWN;
	case REC_MAPENC_LEFTNOTE_CHAR:
		return NOTE_LEFT;
	case REC_MAPENC_RIGHTNOTE_CHAR:
		return NOTE_RIGHT;
	case REC_MAPENC_BOMBNOTE_CHAR:
		return NOTE_BOMB;
	case REC_MAPENC_GHOSTNOTE_CHAR:
		return NOTE_GHOST;
	}
	return NOTE_NONE;
}

/* 再帰関数になっている、最大2回呼ばれる */
void RecMapLoadSetMove(cvec<rec_mapeff_move_st> &move, unsigned int *allnum, int iLine,
	double StartTime, double MovePos, double EndTime, int MoveMode, double bpmG,
	double timer[])
{
	rec_mapeff_move_st buf;
	double Spos = (move.lastData().pos - 100.0) / 50.0;
	switch (MoveMode) {
	case REC_MAP_MOVE_CODE_LIN:
	case REC_MAP_MOVE_CODE_ACC:
	case REC_MAP_MOVE_CODE_DEC:
		SETMove(&buf, StartTime, MovePos, EndTime, MoveMode, bpmG, timer[0]);
		move.push_back(buf);
		break;
	case REC_MAP_MOVE_CODE_MOM:
		RecMapLoadSetMove(move, allnum, iLine, StartTime, MovePos,
			EndTime, REC_MAP_MOVE_CODE_LIN, bpmG, timer);
		buf = move.lastData();
		move.pop_back();
		buf.Stime -= 5;
		buf.Etime -= 5;
		move.push_back(buf);
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

static TCHAR RecEncNoteGetStrcode(TCHAR c, struct custom_note_box customnote[]) {
	TCHAR strcode = REC_MAPENC_BLANK_CHAR;

	if (L'1' <= c && c <= L'9') {
		if (customnote[c - L'1'].rand == 0) {
			strcode = customnote[c - L'1'].note;
		}
		else {
			uint ret = GetRand(8);
			while ((customnote[c - L'1'].rand & (1 << ret)) == 0) {
				ret = GetRand(8);
			}
			switch (ret) {
			case 8:
				strcode = REC_MAPENC_BLANK_CHAR;
				break;
			case 7:
				strcode = REC_MAPENC_HITNOTE_CHAR;
				break;
			case 6:
				strcode = REC_MAPENC_CATCHNOTE_CHAR;
				break;
			case 5:
				strcode = REC_MAPENC_UPNOTE_CHAR;
				break;
			case 4:
				strcode = REC_MAPENC_DOWNNOTE_CHAR;
				break;
			case 3:
				strcode = REC_MAPENC_LEFTNOTE_CHAR;
				break;
			case 2:
				strcode = REC_MAPENC_RIGHTNOTE_CHAR;
				break;
			case 1:
				strcode = REC_MAPENC_BOMBNOTE_CHAR;
				break;
			case 0:
				strcode = REC_MAPENC_GHOSTNOTE_CHAR;
				break;
			}
		}
	}
	else {
		strcode = c;
	}
	if (strcode == REC_MAPENC_RANDOM1_CHAR) {
		switch (GetRand(4)) {
		case 0:
			strcode = REC_MAPENC_HITNOTE_CHAR;
			break;
		case 1:
			strcode = REC_MAPENC_UPNOTE_CHAR;
			break;
		case 2:
			strcode = REC_MAPENC_DOWNNOTE_CHAR;
			break;
		case 3:
			strcode = REC_MAPENC_LEFTNOTE_CHAR;
			break;
		case 4:
			strcode = REC_MAPENC_RIGHTNOTE_CHAR;
			break;
		}
	}
	if (strcode == REC_MAPENC_RANDOM2_CHAR) {
		switch (GetRand(7)) {
		case 0:
			strcode = REC_MAPENC_HITNOTE_CHAR;
			break;
		case 1:
			strcode = REC_MAPENC_UPNOTE_CHAR;
			break;
		case 2:
			strcode = REC_MAPENC_DOWNNOTE_CHAR;
			break;
		case 3:
			strcode = REC_MAPENC_LEFTNOTE_CHAR;
			break;
		case 4:
			strcode = REC_MAPENC_RIGHTNOTE_CHAR;
			break;
		case 5:
			strcode = REC_MAPENC_CATCHNOTE_CHAR;
			break;
		case 6:
			strcode = REC_MAPENC_BOMBNOTE_CHAR;
			break;
		case 7:
			strcode = REC_MAPENC_GHOSTNOTE_CHAR;
			break;
		}
	}
	return strcode;
}

static int RecMapLoadGetc(TCHAR c, int istr, rec_score_file_t *recfp, rec_mapenc_data_t *mapenc,
	int iLine, int BlockNoteNum)
{
	int objectN = mapenc->objectN;
	note_box_2_t buf;

	TCHAR strcode = REC_MAPENC_BLANK_CHAR;
	if (IsNoteCode(c) == 0) { return -1; }
	mapenc->noteLaneNo[iLine] = objectN;
	switch (iLine) {
	case 0:
		buf.lane = NOTE_LANE_UP;
		break;
	case 1:
		buf.lane = NOTE_LANE_MID;
		break;
	case 2:
		buf.lane = NOTE_LANE_LOW;
		break;
	}
	buf.hittime = mapenc->timer[iLine] + 240000 * istr / (mapenc->bpmG * BlockNoteNum);
	strcode = RecEncNoteGetStrcode(c, mapenc->customnote);
	buf.object = GetNoteObjMat(strcode);
	//viewtimeを計算する
	buf.viewtime =
		buf.hittime * recfp->mapeff.scrool.searchData(buf.hittime).speed +
		recfp->mapeff.scrool.searchData(buf.hittime).basetime;
	buf.ypos = 50 * iLine + 300;
	buf.xpos = 150;
	/* 縦位置を計算する */ {
		cvec<rec_mapeff_move_st> &temp = recfp->mapeff.move.y[iLine];
		while (!temp.isEndNo() && (temp.offsetData(1).Stime < buf.hittime + 5)) {
			temp.stepNo();
		}
		if (buf.hittime < temp.nowData().Etime) { /* 移動中 */
			buf.ypos = movecal(
				temp.nowData().mode, temp.nowData().Stime, temp.offsetData(-1).pos,
				temp.nowData().Etime, temp.nowData().pos, buf.hittime
			);
		}
		else { buf.ypos = temp.nowData().pos; } /* 移動後 */
	}
	/* 横位置を計算する */ {
		cvec<rec_mapeff_move_st> &temp = recfp->mapeff.move.x[iLine];
		while (!temp.isEndNo() && (temp.offsetData(1).Stime < buf.hittime + 5)) {
			temp.stepNo();
		}
		if (buf.hittime < temp.nowData().Etime) { /* 移動中 */
			buf.xpos = movecal(
				temp.nowData().mode, temp.nowData().Stime, temp.offsetData(-1).pos,
				temp.nowData().Etime, temp.nowData().pos, buf.hittime
			);
		}
		else { buf.xpos = temp.nowData().pos; } /* 移動後 */
	}
	//効果音を設定する
	if (L'1' <= c && c <= L'9') {
		buf.sound  = mapenc->customnote[c - L'1'].sound;
		buf.melody = mapenc->customnote[c - L'1'].melody;
	}
	else { buf.sound = 0; }
	//色を設定する
	if (L'1' <= c && c <= L'9') { buf.color = mapenc->customnote[c - L'1'].color; }
	else { buf.color = 0; }
	if (buf.object != 8) { (recfp->mapdata.notes)++; }
	if (buf.object != 8) { recfp->mapdata.note[iLine].push_back(buf); }
	else { recfp->mapeff.gnote.insert(buf.hittime, buf); }
	recfp->allnum.notenum[iLine]++;
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

static void RecEncCustomSetNoteMat(struct custom_note_box *ret, TCHAR str[]) {
	ret->rand = 0;

	if (strands(str, L"RAND(")) {
		strmods(str, 5);
		int i = 0;
		bool loopFg = true;
		while (loopFg) {
			switch (str[i]) {
			case REC_MAPENC_BLANK_CHAR:
				ret->rand |= (1 << 8);
				break;
			case REC_MAPENC_HITNOTE_CHAR:
				ret->rand |= (1 << 7);
				break;
			case REC_MAPENC_CATCHNOTE_CHAR:
				ret->rand |= (1 << 6);
				break;
			case REC_MAPENC_UPNOTE_CHAR:
				ret->rand |= (1 << 5);
				break;
			case REC_MAPENC_DOWNNOTE_CHAR:
				ret->rand |= (1 << 4);
				break;
			case REC_MAPENC_LEFTNOTE_CHAR:
				ret->rand |= (1 << 3);
				break;
			case REC_MAPENC_RIGHTNOTE_CHAR:
				ret->rand |= (1 << 2);
				break;
			case REC_MAPENC_BOMBNOTE_CHAR:
				ret->rand |= (1 << 1);
				break;
			case REC_MAPENC_GHOSTNOTE_CHAR:
				ret->rand |= (1 << 0);
				break;
			default:
				loopFg = false;
				break;
			}
			i++;
		}
		return;
	}

	ret->note = str[0];
	return ;
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
		if (strands_direct(str, L"NOTE=")) {
			strmods(str, 5);
			RecEncCustomSetNoteMat(ptr, str);
		}
		else if (strands_direct(str, L"SOUND=")) {
			strmods(str, 6);
			if (str[0] == L'L' || str[0] == L'H') {
				ptr->melody = RecMapLoad_GetMelSnd(str);
			}
			else {
				ptr->sound = strsans2(str);
			}
		}
		else if (strands_direct(str, L"COLOR=")) {
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

#if 1 /* rec_mapenc_noteact_f */

static void RecMapencSetSpeed(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	int time_buf;
	double data_buf;
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	uint lane = betweens(0, GT1[6] - '1', 4);

	strmods(GT1, 8);
	data_buf = strsans2(GT1);
	strnex(GT1);
	if (GT1[0] >= L'0' && GT1[0] <= L'9' || GT1[0] == L'-') {
		time_buf = mapenc->timer[lane] + 240000 * (data_buf - 1) / (mapenc->bpmG * 16) - 10;
		data_buf = strsans2(GT1);
	}
	else {
		time_buf = mapenc->timer[lane] - 10;
	}
	recfp->mapeff.speedt[lane].push_back(time_buf, data_buf);
	return;
}

static void RecMapencSetBpm(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	strmods(GT1, 5);
	mapenc->bpmG = strsans2(GT1);
	return;
}

static void RecMapencSetVBpm(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	int time_buf;
	double data_buf;
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	strmods(GT1, 7);
	time_buf = shifttime(strsans(GT1), mapenc->bpmG, mapenc->timer[0]);
	strnex(GT1);
	data_buf = strsans(GT1);
	recfp->mapeff.v_BPM.push_back(time_buf, data_buf);
	return;
}

static void RecMapencSetChara(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	int time_buf;
	int data_buf;
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	uint lane = GT1[6] - 49;

	strmods(GT1, 8);
	data_buf = betweens(0, strsans(GT1), 2);
	time_buf = (int)mapenc->timer[lane];
	recfp->mapeff.chamo[lane].push_back(time_buf, data_buf);
	return;
}

static void RecMapencSetMove(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	uint   Slane = 0;
	uint   Elane = 0;
	uint   Gap   = 0;
	uint   mode  = 1;
	double Stime = 1.0;
	double Etime = 1.0;
	double pos   = 5.0;

	if (GT1[8] == L'A') {
		Slane = 0;
		Elane = 2;
		Gap   = 0;
	}
	else if (GT1[8] == L'B') {
		Slane = 0;
		Elane = 2;
		Gap   = 1;
	}
	else if (GT1[8] == L'C') {
		Slane = 0;
		Elane = 2;
		Gap   = 2;
	}
	else if (GT1[8] == L'D') {
		Slane = 0;
		Elane = 2;
		Gap   = 3;
	}
	else {
		Elane = Slane = betweens(0, GT1[8] - 49, 4);
		Gap = 0;
	}
	switch (GT1[5]) {
	case('l'):
		mode = 1;
		break;
	case('a'):
		mode = 2;
		break;
	case('d'):
		mode = 3;
		break;
	case('m'):
		mode = 4;
		break;
	case('s'):
		mode = 5;
		break;
	case('p'):
		mode = 6;
		break;
	case('e'):
		mode = 7;
		break;
	}
	strmods(GT1, 10);
	Stime = strsans2(GT1);
	strnex(GT1);
	if (GT1[0] == _T('R')) { pos = strrans(GT1); }
	else { pos = strsans2(GT1); }
	strnex(GT1);
	Etime = strsans2(GT1);
	for (uint iLane = Slane; iLane <= Elane; iLane++) {
		RecMapLoadSetMove(recfp->mapeff.move.y[iLane], recfp->allnum.Ymovenum, iLane, Stime,
			pos + Gap * iLane - Gap, Etime, mode, mapenc->bpmG, mapenc->timer);
	}
	return;
}

static void RecMapencSetXMove(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	uint   Slane = 0;
	uint   Elane = 0;
	uint   Gap   = 0;
	uint   mode  = 1;
	double Stime = 1.0;
	double Etime = 1.0;
	double pos   = 5.0;

	if (GT1[8] == L'A') {
		Slane = 0;
		Elane = 2;
		Gap   = 0;
	}
	else if (GT1[8] == L'B') {
		Slane = 0;
		Elane = 2;
		Gap   = 1;
	}
	else if (GT1[8] == L'C') {
		Slane = 0;
		Elane = 2;
		Gap   = 2;
	}
	else if (GT1[8] == L'D') {
		Slane = 0;
		Elane = 2;
		Gap   = 3;
	}
	else {
		Elane = Slane = betweens(0, GT1[8] - 49, 2);
		Gap = 0;
	}
	switch (GT1[5]) {
	case('l'):
		mode = 1;
		break;
	case('a'):
		mode = 2;
		break;
	case('d'):
		mode = 3;
		break;
	case('m'):
		mode = 4;
		break;
	case('s'):
		mode = 5;
		break;
	case('p'):
		mode = 6;
		break;
	case('e'):
		mode = 7;
		break;
	}
	strmods(GT1, 10);
	Stime = strsans2(GT1);
	strnex(GT1);
	if (GT1[0] == _T('R')) { pos = strrans(GT1); }
	else { pos = strsans2(GT1); }
	strnex(GT1);
	Etime = strsans2(GT1);
	for (uint iLane = Slane; iLane <= Elane; iLane++) {
		RecMapLoadSetMove(recfp->mapeff.move.x[iLane], recfp->allnum.Xmovenum, iLane, Stime,
			pos + Gap * iLane - Gap, Etime, mode, mapenc->bpmG, mapenc->timer);
	}
	return;
}

static void RecMapencSetDiv(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	bool   Yflag   = false;
	uint   lane    = betweens(0, GT1[5] - L'1', 2);
	double Stime   = 1.0;
	double Onetime = 1.0;
	double pos     = 5.0;
	double count   = 1.0;
	if (GT1[4] == L'Y') { Yflag = true; }

	strmods(GT1, 7);
	Stime = strsans2(GT1);//開始時間
	strnex(GT1);
	pos = strsans2(GT1);//振動位置
	strnex(GT1);
	Onetime = strsans2(GT1) / 2.0;//往復時間
	strnex(GT1);
	count = strsans2(GT1);//往復回数
	if (Yflag) {
		for (uint inum = 0; inum < count; inum++) {
			/* TODO: RecMapLoadSetMoveで書けない? */
			rec_mapeff_move_st buf;
			SETMove(&buf, Stime,
				pos, Stime + Onetime, 1, mapenc->bpmG, mapenc->timer[0]);
			recfp->mapeff.move.y[lane].push_back(buf);
			SETMove(&buf, Stime + Onetime,
				(recfp->mapeff.move.y[lane].lastData().pos - 100.0) / 50.0,
				Stime + Onetime * 2, 1, mapenc->bpmG, mapenc->timer[0]);
			recfp->mapeff.move.y[lane].push_back(buf);
			Stime += Onetime * 2;
			recfp->allnum.Ymovenum[lane] += 2;
		}
	}
	else {
		for (uint inum = 0; inum < count; inum++) {
			rec_mapeff_move_st buf;
			SETMove(&buf, Stime,
				pos, Stime + Onetime, 1, mapenc->bpmG, mapenc->timer[0]);
			recfp->mapeff.move.x[lane].push_back(buf);
			SETMove(&buf, Stime + Onetime,
				(recfp->mapeff.move.x[lane].lastData().pos - 100.0) / 50.0,
				Stime + Onetime * 2, 1, mapenc->bpmG, mapenc->timer[0]);
			recfp->mapeff.move.x[lane].push_back(buf);
			Stime += Onetime * 2;
			recfp->allnum.Xmovenum[lane] += 2;
		}
	}
	return;
}

static void RecMapencSetGMove(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	uint   mode  = 1;
	double Stime = 1.0;
	double Etime = 1.0;
	double pos   = 5.0;

	switch (GT1[6]) {
	case('l'):
		mode = 1;
		break;
	case('a'):
		mode = 2;
		break;
	case('d'):
		mode = 3;
		break;
	case('m'):
		mode = 4;
		break;
	case('s'):
		mode = 5;
		break;
	case('p'):
		mode = 6;
		break;
	}
	strmods(GT1, 10);
	Stime = strsans2(GT1);
	strnex(GT1);
	if (GT1[0] == _T('R')) { pos = strrans(GT1); }
	else { pos = strsans2(GT1); }
	strnex(GT1);
	Etime = strsans2(GT1);
	RecMapLoadSetMove(recfp->mapeff.move.y[3], recfp->allnum.Ymovenum, 3,
		Stime, pos, Etime, mode, mapenc->bpmG, mapenc->timer);
	return;
}

static void RecMapencSetXLock(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	int time_buf;
	double data_buf;
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	strmods(GT1, 7);
	data_buf = !(recfp->mapeff.lock.x.lastData());
	time_buf = shifttime(strsans(GT1), mapenc->bpmG, mapenc->timer[0]);
	recfp->mapeff.lock.x.push_back(time_buf, data_buf);
	return;
}

static void RecMapencSetYLock(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	int time_buf;
	double data_buf;
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	strmods(GT1, 7);
	data_buf = !(recfp->mapeff.lock.y.lastData());
	time_buf = shifttime(strsans(GT1), mapenc->bpmG, mapenc->timer[0]);
	recfp->mapeff.lock.y.push_back(time_buf, data_buf);
	return;
}

static void RecMapencSetCArrow(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	int time_buf;
	int data_buf;
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	strmods(GT1, 8);
	data_buf = !(recfp->mapeff.carrow.lastData());
	time_buf = shifttime(strsans(GT1), mapenc->bpmG, mapenc->timer[0]);
	recfp->mapeff.carrow.push_back(time_buf, data_buf);
	return;
}

static void RecMapencSetFall(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	int time_buf;
	double data_buf;
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	strmods(GT1, 6);
	data_buf = strsans(GT1);
	strnex(GT1);
	time_buf = shifttime(strsans(GT1), mapenc->bpmG, mapenc->timer[0]);
	recfp->mapeff.fall.push_back(time_buf, data_buf);
	return;
}

static void RecMapencSetView(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	int time_buf;
	int data_buf;
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	strmods(GT1, 6);
	time_buf = shifttime(strsans(GT1), mapenc->bpmG, mapenc->timer[0]);
	strnex(GT1);
	data_buf = strsans(GT1);
	recfp->mapeff.viewT.push_back(time_buf, data_buf);
	return;
}

static void RecMapencSetVLane(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	int time_buf;
	bool data_buf;
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	strmods(GT1, 8);
	data_buf = (GT1[0] == _T('1'));
	strnex(GT1);
	time_buf = shifttime(strsans(GT1), mapenc->bpmG, mapenc->timer[0]);
	recfp->mapeff.viewLine.push_back(time_buf, data_buf);
	return;
}

static void RecMapencSetMovie(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	item_box buf;
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	strmods(GT1, 7);
	buf.ID = strsans(GT1);
	strnex(GT1);
	switch (GT1[0]) {
	case L'l':
		buf.movemode = 1;
		break;
	case L'a':
		buf.movemode = 2;
		break;
	case L'd':
		buf.movemode = 3;
		break;
	}
	strnex(GT1);
	buf.starttime = shifttime(strsans2(GT1), mapenc->bpmG, mapenc->timer[0]);
	strnex(GT1);
	buf.endtime = shifttime(strsans2(GT1), mapenc->bpmG, mapenc->timer[0]);
	strnex(GT1);
	buf.startXpos = (int)(strsans2(GT1) * 50 + 115);
	strnex(GT1);
	buf.endXpos = (int)(strsans2(GT1) * 50 + 115);
	strnex(GT1);
	buf.startYpos = (int)(strsans2(GT1) * 50 + 115);
	strnex(GT1);
	buf.endYpos = (int)(strsans2(GT1) * 50 + 115);
	strnex(GT1);
	buf.startsize = (int)(strsans2(GT1) * 100);
	strnex(GT1);
	buf.endsize = (int)(strsans2(GT1) * 100);
	strnex(GT1);
	buf.startrot = strsans(GT1);
	strnex(GT1);
	buf.endrot = strsans(GT1);
	strnex(GT1);
	buf.startalpha = (int)(strsans2(GT1) * 255.0);
	strnex(GT1);
	buf.endalpha = (int)(strsans2(GT1) * 255.0);
	strnex(GT1);
	buf.eff = set_pic_mat(GT1);
	recfp->mapeff.Movie.push_back(buf);
	recfp->allnum.movienum++;
	return;
}

static void RecMapencInitItemSet(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	strmods(GT1, 15);
	mapenc->item_set[strsans(GT1)].num = 0;
	return;
}

static void RecMapencAddItemSet(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	strmods(GT1, 14);
	uint No = strsans(GT1);

	if (10 < mapenc->item_set[No].num) { return; }

	strnex(GT1);
	mapenc->item_set[No].picID[mapenc->item_set[No].num].picID = strsans(GT1);
	strnex(GT1);
	mapenc->item_set[No].picID[mapenc->item_set[No].num].Xpos = (int)(strsans2(GT1) * 50);
	strnex(GT1);
	mapenc->item_set[No].picID[mapenc->item_set[No].num].Ypos = (int)(strsans2(GT1) * 50);
	strnex(GT1);
	mapenc->item_set[No].picID[mapenc->item_set[No].num].size = (int)(strsans2(GT1) * 100);
	strnex(GT1);
	mapenc->item_set[No].picID[mapenc->item_set[No].num].rot = strsans(GT1);
	strnex(GT1);
	mapenc->item_set[No].picID[mapenc->item_set[No].num].alpha = (int)(strsans2(GT1) * 255);
	strnex(GT1);
	mapenc->item_set[No].picID[mapenc->item_set[No].num].eff = set_pic_mat(GT1);
	mapenc->item_set[No].num++;
	return;
}

static void RecMapencSetItemGroup(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	int G[15];
	uint itemNo = 0;
	uint mode   = 1;

	rec_map_eff_data_t *mapeff = &recfp->mapeff;

	strmods(GT1, 10);
	itemNo = strsans(GT1);
	strnex(GT1);
	switch (GT1[0]) {
	case L'l':
		mode = 1;
		break;
	case L'a':
		mode = 2;
		break;
	case L'd':
		mode = 3;
		break;
	}

	strnex(GT1);
	G[2] = shifttime(strsans2(GT1), mapenc->bpmG, mapenc->timer[0]); /* stime */
	strnex(GT1);
	G[3] = shifttime(strsans2(GT1), mapenc->bpmG, mapenc->timer[0]); /* etime */
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
	for (uint inum = 0; inum < mapenc->item_set[itemNo].num; inum++) {
		item_box buf;
		buf.ID = mapenc->item_set[itemNo].picID[inum].picID;
		buf.movemode = mode;
		buf.eff = mapenc->item_set[itemNo].picID[inum].eff;
		buf.starttime = G[2];
		buf.endtime = G[3];
		buf.startXpos = mapenc->item_set[itemNo].picID[inum].Xpos * G[8] / 100;
		buf.endXpos = mapenc->item_set[itemNo].picID[inum].Xpos * G[9] / 100;
		buf.startYpos = mapenc->item_set[itemNo].picID[inum].Ypos * G[8] / 100;
		buf.endYpos = mapenc->item_set[itemNo].picID[inum].Ypos * G[9] / 100;
		rot_xy_pos(G[10], &buf.startXpos, &buf.startYpos);
		rot_xy_pos(G[10], &buf.endXpos, &buf.endYpos);
		buf.startXpos += G[4];
		buf.endXpos += G[5];
		buf.startYpos += G[6];
		buf.endYpos += G[7];
		buf.startsize = G[8] * mapenc->item_set[itemNo].picID[inum].size / 100;
		buf.endsize = G[9] * mapenc->item_set[itemNo].picID[inum].size / 100;
		buf.startrot = G[10] + mapenc->item_set[itemNo].picID[inum].rot;
		buf.endrot = G[11] + mapenc->item_set[itemNo].picID[inum].rot;
		buf.startalpha = G[12] * mapenc->item_set[itemNo].picID[inum].alpha / 255;
		buf.endalpha = G[13] * mapenc->item_set[itemNo].picID[inum].alpha / 255;
		recfp->mapeff.Movie.push_back(buf);
		recfp->allnum.movienum++;
	}
	return;
}

static void RecMapencSetCamera(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	rec_camera_data_t buf;
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	strmods(GT1, 8);
	buf.starttime = shifttime(strsans2(GT1), mapenc->bpmG, mapenc->timer[0]);
	strnex(GT1);
	buf.endtime = shifttime(strsans2(GT1), mapenc->bpmG, mapenc->timer[0]);
	strnex(GT1);
	buf.xpos = strsans2(GT1) * 50;
	strnex(GT1);
	buf.ypos = strsans2(GT1) * 50;
	strnex(GT1);
	buf.zoom = strsans2(GT1);
	strnex(GT1);
	buf.rot = strsans2(GT1);
	strnex(GT1);
	switch (GT1[0]) {
	case L'a':
		buf.mode = 2;
		break;
	case L'd':
		buf.mode = 3;
		break;
	default:
		buf.mode = 1;
		break;
	}
	recfp->mapeff.camera.push_back(buf);
	return;
}

static void RecMapencSetCamMove(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	rec_camera_data_t buf;
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	if (strands_direct(GT1, L"#CMOV:")) { strmods(GT1, 6); }
	if (strands_direct(GT1, L"#CAMMOVE:")) { strmods(GT1, 9); }
	buf.starttime = shifttime(strsans2(GT1), mapenc->bpmG, mapenc->timer[0]);
	strnex(GT1);
	buf.endtime = shifttime(strsans2(GT1), mapenc->bpmG, mapenc->timer[0]);
	strnex(GT1);
	buf.xpos = strsans2(GT1) * 50;
	strnex(GT1);
	buf.ypos = strsans2(GT1) * 50;
	strnex(GT1);
	switch (GT1[0]) {
	case L'a':
		buf.mode = 2;
		break;
	case L'd':
		buf.mode = 3;
		break;
	default:
		buf.mode = 1;
		break;
	}
	recfp->mapeff.camera.push_back(buf);
	return;
}

static void RecMapencSetScrool(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	int time_buf;
	rec_scrool_data_t data_buf;
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	int temp = 0;

	strmods(GT1, 8);
	time_buf = shifttime(strsans2(GT1), mapenc->bpmG, mapenc->timer[0]);
	strnex(GT1);
	data_buf.speed = strsans2(GT1);
	temp = recfp->mapeff.scrool.lastData().speed *
		time_buf + recfp->mapeff.scrool.lastData().basetime;
	data_buf.basetime = temp - data_buf.speed * time_buf;
	recfp->mapeff.scrool.push_back(time_buf, data_buf);
	return;
}

static void RecMapencSetCustomNote(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	RecMapLoad_ComCustomNote(GT1, mapenc->customnote);
	return;
}

static void RecMapencSetNotes(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc, const TCHAR *str) {
	TCHAR GT1[255];
	strcopy_2(str, GT1, ARRAY_COUNT(GT1));

	for (int iLine = 0; iLine < 3; iLine++) {
		int BlockNoteNum = 0;
		while (GT1[BlockNoteNum] != L'\0' && GT1[BlockNoteNum] != L',') { BlockNoteNum++; }
		for (int istr = 0; istr < BlockNoteNum; istr++) {
			RecMapLoadGetc(GT1[istr], istr, recfp, mapenc, iLine, BlockNoteNum);
		}
		if (iLine <= 1) { FileRead_gets(GT1, 256, mapenc->songdata); }
	}
	mapenc->timer[0] = mapenc->timer[1] = mapenc->timer[2] += 240000.0 / mapenc->bpmG;
	return;
}

#endif /* rec_mapenc_noteact_f */

#if 1 /* sub action */

static void RecMapLoad_SetInitRecfp(rec_score_file_t *recfp) {
	recfp->mapeff.camera.push_back({ 0,0,0,0,1,0,0 });
	recfp->mapeff.scrool.push_back(0, { 0,1 });
	recfp->mapeff.viewT.push_back(0, 3000);
	recfp->mapeff.carrow.push_back(0, true);
	recfp->mapeff.lock.x.push_back(0, false);
	recfp->mapeff.lock.y.push_back(0, true);
	recfp->mapeff.move.y[0].push_back({ 0, 300, 0, 1 });
	recfp->mapeff.move.y[1].push_back({ 0, 350, 0, 1 });
	recfp->mapeff.move.y[2].push_back({ 0, 400, 0, 1 });
	recfp->mapeff.move.y[3].push_back({ 0, 350, 0, 1 });
	recfp->mapeff.move.y[4].push_back({ 0, 600, 0, 1 });
	recfp->mapeff.move.x[0].push_back({ 0, 150, 0, 1 });
	recfp->mapeff.move.x[1].push_back({ 0, 150, 0, 1 });
	recfp->mapeff.move.x[2].push_back({ 0, 150, 0, 1 });
	recfp->mapeff.chamo[0].push_back(0, 0);
	recfp->mapeff.chamo[1].push_back(0, 1);
	recfp->mapeff.chamo[2].push_back(0, 1);
	recfp->mapeff.fall.push_back(0, -1);
	recfp->mapeff.speedt[0].push_back(0, 1.0);
	recfp->mapeff.speedt[1].push_back(0, 1.0);
	recfp->mapeff.speedt[2].push_back(0, 1.0);
	recfp->mapeff.speedt[3].push_back(0, 1.0);
	recfp->mapeff.speedt[4].push_back(0, 1.0);
	recfp->mapeff.viewLine.push_back(0, true);
	return;
}

static void RecMapLoad_SetEndRecfp(rec_score_file_t *recfp, rec_mapenc_data_t *mapenc) {
	//譜面の最後にendを置く
	recfp->mapdata.note[0].push_back({
		(int)mapenc->timer[0], -1, NOTE_END, NOTE_LANE_MID, -1, 1000, 0, MELODYSOUND_NONE, 0, 0}
	);
	recfp->mapdata.note[1].push_back({
		(int)mapenc->timer[0], -1, NOTE_END, NOTE_LANE_MID, -1, 1000, 0, MELODYSOUND_NONE, 0, 0}
	);
	recfp->mapdata.note[2].push_back({
		(int)mapenc->timer[0], -1, NOTE_END, NOTE_LANE_MID, -1, 1000, 0, MELODYSOUND_NONE, 0, 0}
	);
	recfp->mapeff.lock.x.push_back(mapenc->timer[0], true);
	recfp->mapeff.lock.y.push_back(mapenc->timer[0], false);
	recfp->allnum.notenum[1]++;
	recfp->time.end = mapenc->timer[0];
	return;
}

static void RecMapLoad_EncodeMap(rec_score_file_t *recfp, const TCHAR *mapPath, const TCHAR *folderPath) {
	const static struct {
		TCHAR cmd[32];
		rec_mapenc_noteact_f func;
	} noteact_table[] = {
		{ _T(";"),               NULL },
		{ _T("#SPEED"),          RecMapencSetSpeed      },
		{ _T("#BPM:"),           RecMapencSetBpm        },
		{ _T("#V-BPM:"),         RecMapencSetVBpm       },
		{ _T("#CHARA"),          RecMapencSetChara      },
		{ _T("#MOVE"),           RecMapencSetMove       },
		{ _T("#XMOV"),           RecMapencSetXMove      },
		{ _T("#DIV"),            RecMapencSetDiv        },
		{ _T("#GMOVE"),          RecMapencSetGMove      },
		{ _T("#XLOCK"),          RecMapencSetXLock      },
		{ _T("#YLOCK"),          RecMapencSetYLock      },
		{ _T("#CARROW"),         RecMapencSetCArrow     },
		{ _T("#FALL"),           RecMapencSetFall       },
		{ _T("#VIEW:"),          RecMapencSetView       },
		{ _T("#V-LANE:"),        RecMapencSetVLane      },
		{ _T("#MOVIE:"),         RecMapencSetMovie      },
		{ _T("#INIT_ITEM_SET:"), RecMapencInitItemSet   },
		{ _T("#ADD_ITEM_SET:"),  RecMapencAddItemSet    },
		{ _T("#ITEM_SET:"),      RecMapencSetItemGroup  },
		{ _T("#CAMERA:"),        RecMapencSetCamera     },
		{ _T("#CMOV"),           RecMapencSetCamMove    },
		{ _T("#CAMMOVE:"),       RecMapencSetCamMove    },
		{ _T("#SCROOL:"),        RecMapencSetScrool     },
		{ _T("#CUSTOM:"),        RecMapencSetCustomNote }
	};

	int waningLv = 2;
	TCHAR GT1[255];
	rec_mapenc_data_t mapenc;

	RecMapLoad_SetInitRecfp(recfp);
	mapenc.songdata = FileRead_open(mapPath);
	if (mapenc.songdata == 0) { return; }

	//テキストデータを読む
	while (FileRead_eof(mapenc.songdata) == 0) {
		FileRead_gets(GT1, 256, mapenc.songdata);
		//音楽ファイルを読み込む
		if (strands_direct(GT1, L"#MUSIC:")) {
			strmods(GT1, 7);
			recfp->nameset.mp3FN  = folderPath;
			recfp->nameset.mp3FN += GT1;
		}
		//BPMを読み込む
		else if (strands_direct(GT1, L"#BPM:")) {
			strmods(GT1, 5);
			mapenc.bpmG = recfp->mapdata.bpm = strsans2(GT1);
			recfp->mapeff.v_BPM.push_back(recfp->time.offset, recfp->mapdata.bpm);
		}
		//ノートのオフセットを読み込む
		else if (strands_direct(GT1, L"#NOTEOFFSET:")) {
			strmods(GT1, 12);
			mapenc.timer[0] = mapenc.timer[1] = mapenc.timer[2] = recfp->time.offset = strsans(GT1);
		}
		//空の背景を読み込む
		else if (strands_direct(GT1, L"#SKY:")) {
			strmods(GT1, 5);
			recfp->nameset.sky  = _T("picture/");
			recfp->nameset.sky += GT1;
		}
		//地面の画像を読み込む
		else if (strands_direct(GT1, L"#FIELD:")) {
			strmods(GT1, 7);
			recfp->nameset.ground  = _T("picture/");
			recfp->nameset.ground += GT1;
		}
		//水中の画像を読み込む
		else if (strands_direct(GT1, L"#WATER:")) {
			strmods(GT1, 7);
			recfp->nameset.water  = _T("picture/");
			recfp->nameset.water += GT1;
		}
		//難易度バー(another)を読み込む
		else if (strands_direct(GT1, L"#DIFBAR:")) {
			strmods(GT1, 8);
			recfp->nameset.DifFN  = folderPath;
			recfp->nameset.DifFN += GT1;
		}
		//曲名を読み込む
		else if (strands_direct(GT1, L"#TITLE:")) {
			strmods(GT1, 7);
			recfp->nameset.songN = GT1;
		}
		//英語
		else if (strands_direct(GT1, L"#E.TITLE:")) {
			strmods(GT1, 7);
			recfp->nameset.songNE = GT1;
		}
		//レベルを読み込む
		else if (strands_direct(GT1, L"#LEVEL:")) {
			strmods(GT1, 7);
			recfp->mapdata.Lv = strsans(GT1);
		}
		//落ち物背景指定
		else if (strands_direct(GT1, L"#FALL:")) {
			strmods(GT1, 6);
			recfp->mapeff.fall.push_back(0, strsans(GT1));
		}
		//譜面難易度フィルタのレベル
		else if (strands_direct(GT1, L"#WANING:")) {
			strmods(GT1, 8);
			waningLv = strsans(GT1);
		}
		//譜面を読み込む
		else if (strands_direct(GT1, L"#MAP:")) {
			break;
		}
	}

	while (1) {
		FileRead_gets(GT1, 256, mapenc.songdata);
		if (FileRead_eof(mapenc.songdata) != 0 || strands_direct(GT1, L"#END") != 0) { break; }

		int hitnum = -1;
		for (uint inum = 0; inum < ARRAY_COUNT(noteact_table); inum++) {
			if (GT1[0] == _T('\0')) { break; }
			if (strands_2(GT1, ARRAY_COUNT(GT1), noteact_table[inum].cmd, ARRAY_COUNT(noteact_table[inum].cmd))) {
				hitnum = inum;
				break;
			}
		}

		if (hitnum != -1) {
			if (noteact_table[hitnum].func != NULL) { noteact_table[hitnum].func(recfp, &mapenc, GT1); }
		}
		else {
			RecMapencSetNotes(recfp, &mapenc, GT1);
		}
	}

	FileRead_close(mapenc.songdata);
	RecMapLoad_SetEndRecfp(recfp, &mapenc);
	return;
}

#endif /* sub action */

/* main action */
rec_error_t RecordLoad2(int packNo, int songNo, int difNo) {
	rec_error_t status = REC_ERROR_NONE;
	TCHAR folderPath[255]; // フォルダのパス
	TCHAR mapPath[255]; // マップのパス

	rec_score_file_t recfp;

	status = RecGetMusicFolderPath(folderPath, 255, packNo, songNo);
	if (status != REC_ERROR_NONE) { return status; }
	status = RecGetMusicMapTxtPath(mapPath, 255, packNo, songNo, (rec_dif_t)difNo);
	if (status != REC_ERROR_NONE) { return status; }
	RecMapLoad_EncodeMap(&recfp, mapPath, folderPath);

	status = RecGetMusicMapRrsPath(mapPath, 255, packNo, songNo, (rec_dif_t)difNo);
	if (status != REC_ERROR_NONE) { return status; }
	rec_score_fwrite(&recfp, mapPath);
	return REC_ERROR_NONE;
}
