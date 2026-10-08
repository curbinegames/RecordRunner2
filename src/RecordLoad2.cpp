/* TODO: Dogbite, 末尾のノーツ位置がバグってる */

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
#include <RecordLoad2.h>

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
void RecMapLoadSetMove(cvec<rec_mapeff_move_st> &move,
	double StartTime, double MovePos, double EndTime, int MoveMode, double bpmG,
	double timer)
{
	rec_mapeff_move_st buf;
	double Spos = (move.lastData().pos - 100.0) / 50.0;
	switch (MoveMode) {
	case REC_MAP_MOVE_CODE_LIN:
	case REC_MAP_MOVE_CODE_ACC:
	case REC_MAP_MOVE_CODE_DEC:
		SETMove(&buf, StartTime, MovePos, EndTime, MoveMode, bpmG, timer);
		move.push_back(buf);
		break;
	case REC_MAP_MOVE_CODE_MOM:
		RecMapLoadSetMove(move, StartTime, MovePos,
			EndTime, REC_MAP_MOVE_CODE_LIN, bpmG, timer);
		buf = move.lastData();
		move.pop_back();
		buf.Stime -= 5;
		buf.Etime -= 5;
		move.push_back(buf);
		break;
	case REC_MAP_MOVE_CODE_SLI:
		RecMapLoadSetMove(move, StartTime, (Spos + MovePos) / 2.0,
			(StartTime + EndTime) / 2.0, REC_MAP_MOVE_CODE_ACC, bpmG, timer);
		RecMapLoadSetMove(move, (StartTime + EndTime) / 2.0,
			MovePos, EndTime, REC_MAP_MOVE_CODE_DEC, bpmG, timer);
		break;
	case REC_MAP_MOVE_CODE_PAL:
		RecMapLoadSetMove(move, StartTime, MovePos,
			(StartTime + EndTime) / 2.0, REC_MAP_MOVE_CODE_DEC, bpmG, timer);
		RecMapLoadSetMove(move, (StartTime + EndTime) / 2.0,
			Spos, EndTime, REC_MAP_MOVE_CODE_ACC, bpmG, timer);
		break;
	case REC_MAP_MOVE_CODE_EDG:
		RecMapLoadSetMove(move, StartTime, MovePos,
			(StartTime + EndTime) / 2.0, REC_MAP_MOVE_CODE_ACC, bpmG, timer);
		RecMapLoadSetMove(move, (StartTime + EndTime) / 2.0,
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

static bool RecMapencSplitMovieData(item_box &dest1, item_box &dest2, const item_box &src) {
	switch (src.movemode) {
	case 1: /* lin */
	case 2: /* acc */
	case 3: /* dec */
	default:
		dest1 = src;
		return false;
	case 5: /* sli */
		dest1 = src;
		dest2 = src;

		dest1.ID         = src.ID;
		dest2.ID         = src.ID;
		dest1.movemode   = 2; /* acc */
		dest2.movemode   = 3; /* dec */

		dest1.starttime  = src.starttime;
		dest1.endtime    = (src.starttime  + src.endtime ) / 2;
		dest2.starttime  = (src.starttime  + src.endtime ) / 2;
		dest2.endtime    = src.endtime;

		dest1.startXpos  = src.startXpos;
		dest1.endXpos    = (src.startXpos  + src.endXpos ) / 2;
		dest2.startXpos  = (src.startXpos  + src.endXpos ) / 2;
		dest2.endXpos    = src.endXpos;

		dest1.startYpos  = src.startYpos;
		dest1.endYpos    = (src.startYpos  + src.endYpos ) / 2;
		dest2.startYpos  = (src.startYpos  + src.endYpos ) / 2;
		dest2.endYpos    = src.endYpos;

		dest1.startsize  = src.startsize;
		dest1.endsize    = (src.startsize  + src.endsize ) / 2;
		dest2.startsize  = (src.startsize  + src.endsize ) / 2;
		dest2.endsize    = src.endsize;

		dest1.startrot   = src.startrot;
		dest1.endrot     = (src.startrot   + src.endrot  ) / 2;
		dest2.startrot   = (src.startrot   + src.endrot  ) / 2;
		dest2.endrot     = src.endrot;

		dest1.startalpha = src.startalpha;
		dest1.endalpha   = (src.startalpha + src.endalpha) / 2;
		dest2.startalpha = (src.startalpha + src.endalpha) / 2;
		dest2.endalpha   = src.endalpha;

		dest1.eff        = src.eff;
		dest2.eff        = src.eff;
		break;
	case 6: /* pal */
		dest1 = src;
		dest2 = src;

		dest1.ID         = src.ID;
		dest2.ID         = src.ID;
		dest1.movemode   = 3; /* dec */
		dest2.movemode   = 2; /* acc */

		dest1.starttime  = src.starttime;
		dest1.endtime    = (src.starttime  + src.endtime ) / 2;
		dest2.starttime  = (src.starttime  + src.endtime ) / 2;
		dest2.endtime    = src.endtime;

		dest1.startXpos  = src.startXpos;
		dest1.endXpos    = src.endXpos;
		dest2.startXpos  = src.endXpos;
		dest2.endXpos    = src.startXpos;

		dest1.startYpos  = src.startYpos;
		dest1.endYpos    = src.endYpos;
		dest2.startYpos  = src.endYpos;
		dest2.endYpos    = src.startYpos;

		dest1.startsize  = src.startsize;
		dest1.endsize    = src.endsize;
		dest2.startsize  = src.endsize;
		dest2.endsize    = src.startsize;

		dest1.startrot   = src.startrot;
		dest1.endrot     = src.endrot;
		dest2.startrot   = src.endrot;
		dest2.endrot     = src.startrot;

		dest1.startalpha = src.startalpha;
		dest1.endalpha   = src.endalpha;
		dest2.startalpha = src.endalpha;
		dest2.endalpha   = src.startalpha;

		dest1.eff        = src.eff;
		dest2.eff        = src.eff;
		break;
	case 7: /* edg */
		dest1 = src;
		dest2 = src;

		dest1.ID         = src.ID;
		dest2.ID         = src.ID;
		dest1.movemode   = 2; /* acc */
		dest2.movemode   = 3; /* dec */

		dest1.starttime  = src.starttime;
		dest1.endtime    = (src.starttime  + src.endtime ) / 2;
		dest2.starttime  = (src.starttime  + src.endtime ) / 2;
		dest2.endtime    = src.endtime;

		dest1.startXpos  = src.startXpos;
		dest1.endXpos    = src.endXpos;
		dest2.startXpos  = src.endXpos;
		dest2.endXpos    = src.startXpos;

		dest1.startYpos  = src.startYpos;
		dest1.endYpos    = src.endYpos;
		dest2.startYpos  = src.endYpos;
		dest2.endYpos    = src.startYpos;

		dest1.startsize  = src.startsize;
		dest1.endsize    = src.endsize;
		dest2.startsize  = src.endsize;
		dest2.endsize    = src.startsize;

		dest1.startrot   = src.startrot;
		dest1.endrot     = src.endrot;
		dest2.startrot   = src.endrot;
		dest2.endrot     = src.startrot;

		dest1.startalpha = src.startalpha;
		dest1.endalpha   = src.endalpha;
		dest2.startalpha = src.endalpha;
		dest2.endalpha   = src.startalpha;

		dest1.eff        = src.eff;
		dest2.eff        = src.eff;
		break;
	}

	return true;
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
		RecMapLoadSetMove(recfp->mapeff.move.y[iLane], Stime,
			pos + Gap * iLane - Gap, Etime, mode, mapenc->bpmG, mapenc->timer[0]);
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
		RecMapLoadSetMove(recfp->mapeff.move.x[iLane], Stime,
			pos + Gap * iLane - Gap, Etime, mode, mapenc->bpmG, mapenc->timer[0]);
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

	cvec<rec_mapeff_move_st> &dest = recfp->mapeff.move.y[lane];
	if (Yflag) {
		dest = recfp->mapeff.move.y[lane];
	}
	else {
		dest = recfp->mapeff.move.x[lane];
	}

	int bpos = dest.lastData().pos;
	for (uint inum = 0; inum < count; inum++) {
		RecMapLoadSetMove(
			dest, Stime, pos,
			Stime + Onetime, 1, mapenc->bpmG, mapenc->timer[0]
		);
		RecMapLoadSetMove(
			dest, Stime + Onetime, (bpos - 100.0) / 50.0,
			Stime + Onetime * 2, 1, mapenc->bpmG, mapenc->timer[0]
		);
		Stime += Onetime * 2;
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
	RecMapLoadSetMove(recfp->mapeff.move.y[3],
		Stime, pos, Etime, mode, mapenc->bpmG, mapenc->timer[0]);
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
	item_box buf2;
	item_box buf3;
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
	case L'm': /* L'l'として処理 */
		buf.movemode = 1;
		break;
	case L's':
		buf.movemode = 5;
		break;
	case L'p':
		buf.movemode = 6;
		break;
	case L'e':
		buf.movemode = 7;
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

	switch (buf.movemode) {
	case 1: /* lin */
	case 2: /* acc */
	case 3: /* dec */
	default:
		recfp->mapeff.Movie.push_back(buf);
		recfp->allnum.movienum++;
		break;
	case 5: /* sli */
	case 6: /* pal */
	case 7: /* edg */
		RecMapencSplitMovieData(buf2, buf3, buf);
		recfp->mapeff.Movie.push_back(buf2);
		recfp->mapeff.Movie.push_back(buf3);
		recfp->allnum.movienum += 2;
		break;
	}

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

	uint itemNo = 0;
	item_box stack1;
	item_box stack2;
	bool spe_flag = false;

	rec_map_eff_data_t *mapeff = &recfp->mapeff;

	strmods(GT1, 10);
	itemNo = strsans(GT1);
	strnex(GT1);
	switch (GT1[0]) {
	case L'l':
		stack1.movemode = 1;
		break;
	case L'a':
		stack1.movemode = 2;
		break;
	case L'd':
		stack1.movemode = 3;
		break;
	case L'm': /* L'l'として処理 */
		stack1.movemode = 1;
		break;
	case L's':
		stack1.movemode = 5;
		spe_flag = true;
		break;
	case L'p':
		stack1.movemode = 6;
		spe_flag = true;
		break;
	case L'e':
		stack1.movemode = 7;
		spe_flag = true;
		break;
	}

	strnex(GT1);
	stack1.starttime  = shifttime(strsans2(GT1), mapenc->bpmG, mapenc->timer[0]); /* stime */
	strnex(GT1);
	stack1.endtime    = shifttime(strsans2(GT1), mapenc->bpmG, mapenc->timer[0]); /* etime */
	strnex(GT1);
	stack1.startXpos  = strsans2(GT1) * 50 + 115; /* sx */
	strnex(GT1);
	stack1.endXpos    = strsans2(GT1) * 50 + 115; /* ex */
	strnex(GT1);
	stack1.startYpos  = strsans2(GT1) * 50 + 115; /* sy */
	strnex(GT1);
	stack1.endYpos    = strsans2(GT1) * 50 + 115; /* ey */
	strnex(GT1);
	stack1.startsize  = strsans2(GT1) * 100; /* ss */
	strnex(GT1);
	stack1.endsize    = strsans2(GT1) * 100; /* es */
	strnex(GT1);
	stack1.startrot   = strsans(GT1); /* sr */
	strnex(GT1);
	stack1.endrot     = strsans(GT1); /* er */
	strnex(GT1);
	stack1.startalpha = strsans2(GT1) * 255.0; /* sa */
	strnex(GT1);
	stack1.endalpha   = strsans2(GT1) * 255.0; /* ea */

	if (spe_flag)  {
		item_box stack3 = stack1;
		RecMapencSplitMovieData(stack1, stack2, stack3);
	}

	for (uint inum = 0; inum < mapenc->item_set[itemNo].num; inum++) {
		item_box buf;
		buf.ID        = mapenc->item_set[itemNo].picID[inum].picID;
		buf.movemode  = stack1.movemode;
		buf.eff       = mapenc->item_set[itemNo].picID[inum].eff;
		buf.starttime = stack1.starttime;
		buf.endtime   = stack1.endtime;
		buf.startXpos = mapenc->item_set[itemNo].picID[inum].Xpos * stack1.startsize / 100;
		buf.endXpos   = mapenc->item_set[itemNo].picID[inum].Xpos * stack1.endsize   / 100;
		buf.startYpos = mapenc->item_set[itemNo].picID[inum].Ypos * stack1.startsize / 100;
		buf.endYpos   = mapenc->item_set[itemNo].picID[inum].Ypos * stack1.endsize   / 100;
		rot_xy_pos(stack1.startrot, &buf.startXpos, &buf.startYpos);
		rot_xy_pos(stack1.startrot, &buf.endXpos,   &buf.endYpos);
		buf.startXpos += stack1.startXpos;
		buf.endXpos   += stack1.endXpos;
		buf.startYpos += stack1.startYpos;
		buf.endYpos   += stack1.endYpos;
		buf.startsize  = stack1.startsize  * mapenc->item_set[itemNo].picID[inum].size  / 100;
		buf.endsize    = stack1.endsize    * mapenc->item_set[itemNo].picID[inum].size  / 100;
		buf.startrot   = stack1.startrot   + mapenc->item_set[itemNo].picID[inum].rot;
		buf.endrot     = stack1.endrot     + mapenc->item_set[itemNo].picID[inum].rot;
		buf.startalpha = stack1.startalpha * mapenc->item_set[itemNo].picID[inum].alpha / 255;
		buf.endalpha   = stack1.endalpha   * mapenc->item_set[itemNo].picID[inum].alpha / 255;
		recfp->mapeff.Movie.push_back(buf);
		recfp->allnum.movienum++;
	}

	if (spe_flag) {
		for (uint inum = 0; inum < mapenc->item_set[itemNo].num; inum++) {
			item_box buf;
			buf.ID        = mapenc->item_set[itemNo].picID[inum].picID;
			buf.movemode  = stack2.movemode;
			buf.eff       = mapenc->item_set[itemNo].picID[inum].eff;
			buf.starttime = stack2.starttime;
			buf.endtime   = stack2.endtime;
			buf.startXpos = mapenc->item_set[itemNo].picID[inum].Xpos * stack2.startsize / 100;
			buf.endXpos   = mapenc->item_set[itemNo].picID[inum].Xpos * stack2.endsize   / 100;
			buf.startYpos = mapenc->item_set[itemNo].picID[inum].Ypos * stack2.startsize / 100;
			buf.endYpos   = mapenc->item_set[itemNo].picID[inum].Ypos * stack2.endsize   / 100;
			rot_xy_pos(stack2.startrot, &buf.startXpos, &buf.startYpos);
			rot_xy_pos(stack2.startrot, &buf.endXpos,   &buf.endYpos);
			buf.startXpos += stack2.startXpos;
			buf.endXpos   += stack2.endXpos;
			buf.startYpos += stack2.startYpos;
			buf.endYpos   += stack2.endYpos;
			buf.startsize  = stack2.startsize  * mapenc->item_set[itemNo].picID[inum].size  / 100;
			buf.endsize    = stack2.endsize    * mapenc->item_set[itemNo].picID[inum].size  / 100;
			buf.startrot   = stack2.startrot   + mapenc->item_set[itemNo].picID[inum].rot;
			buf.endrot     = stack2.endrot     + mapenc->item_set[itemNo].picID[inum].rot;
			buf.startalpha = stack2.startalpha * mapenc->item_set[itemNo].picID[inum].alpha / 255;
			buf.endalpha   = stack2.endalpha   * mapenc->item_set[itemNo].picID[inum].alpha / 255;
			recfp->mapeff.Movie.push_back(buf);
			recfp->allnum.movienum++;
		}
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

rec_error_t RecMapencGetBaseData(rec_mapenc_basedata_st &dest, const tstring &path) {
	rec_error_t status = REC_ERROR_NONE;
	tstring folderpath = path;
	TCHAR GT1[256];
	DxFile_t fd = DXLIB_FILE_NULL;
	fd = FileRead_open(path.c_str());
	if (fd == DXLIB_FILE_NULL) { return REC_ERROR_FILE_EXIST; }

	{
		auto pos = folderpath.find_last_of('/');
		if (pos != std::string::npos) {
			folderpath.erase(pos);
			folderpath += _T("/");
		}
	}

	//テキストデータを読む
	while (FileRead_eof(fd) == 0) {
		FileRead_gets(GT1, 256, fd);

		//譜面に入ったら終わり
		if (strands_direct(GT1, L"#MAP:")) { break; }

		//音楽ファイルを読み込む
		if (strands_direct(GT1, L"#MUSIC:")) {
			strmods(GT1, 7);
			dest.music_path  = folderpath;
			dest.music_path += GT1;
		}
		//曲名を読み込む
		else if (strands_direct(GT1, L"#TITLE:")) {
			strmods(GT1, 7);
			dest.music_name.set_str_jp(GT1);
			if (dest.music_name.get_str() == _T("")) {
				dest.music_name.set_str_en(GT1);
			}
		}
		//英語
		else if (strands_direct(GT1, L"#E.TITLE:")) {
			strmods(GT1, 7);
			dest.music_name.set_str_en(GT1);
			if (dest.music_name.get_str() == _T("")) {
				dest.music_name.set_str_jp(GT1);
			}
		}
		//作曲者を読み込む
		else if (strands_direct(GT1, L"#ARTIST:")) {
			strmods(GT1, 8);
			dest.artist_name.set_str_jp(GT1);
			if (dest.artist_name.get_str() == _T("")) {
				dest.artist_name.set_str_en(GT1);
			}
		}
		//英語
		else if (strands_direct(GT1, L"#E.ARTIST:")) {
			strmods(GT1, 10);
			dest.artist_name.set_str_en(GT1);
			if (dest.artist_name.get_str() == _T("")) {
				dest.artist_name.set_str_jp(GT1);
			}
		}
		//レベルを読み込む
		else if (strands_direct(GT1, L"#LEVEL:")) {
			strmods(GT1, 7);
			dest.level = strsans(GT1);
		}
		//BPMを読み込む
		else if (strands_direct(GT1, L"#BPM:")) {
			strmods(GT1, 5);
			dest.bpm = strsans2(GT1);
		}
		//ノートのオフセットを読み込む
		else if (strands_direct(GT1, L"#NOTEOFFSET:")) {
			strmods(GT1, 12);
			dest.note_offset = strsans(GT1);
		}
		//プレビュー時間を読み込む
		else if (strands_direct(GT1, L"#PREVIEW:")) {
			strmods(GT1, 9);
			dest.preview[0] = (int)((double)strsans(GT1) / 1000.0 * REC_DEFAULT_MUSIC_SAMPLE_RATE);
			strnex(GT1);
			if (L'0' <= GT1[1] && GT1[1] <= L'9') {
				dest.preview[1] = (int)((double)strsans(GT1) / 1000.0 * REC_DEFAULT_MUSIC_SAMPLE_RATE);
			}
		}
		//ジャケット写真を読み込む
		else if (strands_direct(GT1, L"#JACKET:")) {
			strmods(GT1, 8);
			dest.jacket_path  = folderpath;
			dest.jacket_path += GT1;
		}
		//空の背景を読み込む
		else if (strands_direct(GT1, L"#SKY:")) {
			strmods(GT1, 5);
			dest.sky_path  = _T("picture/play/");
			dest.sky_path += GT1;
		}
		//地面の画像を読み込む
		else if (strands_direct(GT1, L"#FIELD:")) {
			strmods(GT1, 7);
			dest.field_path  = _T("picture/play/");
			dest.field_path += GT1;
		}
		//水中の画像を読み込む
		else if (strands_direct(GT1, L"#WATER:")) {
			strmods(GT1, 7);
			dest.water_path  = _T("picture/play/");
			dest.water_path += GT1;
		}
		//難易度バー(another)を読み込む
		else if (strands_direct(GT1, L"#DIFBAR:")) {
			strmods(GT1, 8);
			dest.difbar_path  = folderpath;
			dest.difbar_path += GT1;
		}
	}

	FileRead_close(fd);
	return REC_ERROR_NONE;
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
			recfp->nameset.sky  = _T("picture/play/");
			recfp->nameset.sky += GT1;
		}
		//地面の画像を読み込む
		else if (strands_direct(GT1, L"#FIELD:")) {
			strmods(GT1, 7);
			recfp->nameset.ground  = _T("picture/play/");
			recfp->nameset.ground += GT1;
		}
		//水中の画像を読み込む
		else if (strands_direct(GT1, L"#WATER:")) {
			strmods(GT1, 7);
			recfp->nameset.water  = _T("picture/play/");
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
	tstring folderPath; // フォルダのパス
	tstring mapPath; // マップのパス

	rec_score_file_t recfp;

	status = RecGetMusicFolderPath(folderPath, packNo, songNo);
	if (status != REC_ERROR_NONE) { return status; }
	status = RecGetMusicMapTxtPath(mapPath, packNo, songNo, (rec_dif_t)difNo);
	if (status != REC_ERROR_NONE) { return status; }
	RecMapLoad_EncodeMap(&recfp, mapPath.c_str(), folderPath.c_str());

	status = RecGetMusicMapRrsPath(mapPath, packNo, songNo, (rec_dif_t)difNo);
	if (status != REC_ERROR_NONE) { return status; }
	rec_score_fwrite(recfp, mapPath);
	return REC_ERROR_NONE;
}
