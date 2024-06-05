#include "playbox.h"
#include "RecScoreFile.h"
#include "recp_cal_ddif.h"
#include "recp_cal_difkey.h"
#include "recp_cal_ddif_2.h"
#include "define.h"

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

int IsNoteCode(wchar_t c);
int cal_ddif(int num, int const *difkey, int Etime, int noteoff, int difsec, int voidtime);
int cal_nowdif_m(int *difkey, int num, int now, int voidtime);
rrs_obj_code_t check_obj_code(wchar_t const *const s);
void set_item_set(item_box* const Movie, short* const MovieN,
	playnum_box* allnum, wchar_t* const s, item_set_box const* const item_set,
	double bpmG, double timer);
item_eff_box set_pic_mat(wchar_t *s);
int MapErrorCheck(int nownote, int nowtime, int befnote, int beftime, int dif, int wl);

/* proto */
double SETbpm(wchar_t* p1) {
	strmods(p1, 5);
	return strsans2(p1);
}

int SEToffset(wchar_t* p1) {
	strmods(p1, 12);
	return strsans(p1);
}

int SETLv(wchar_t* p1) {
	strmods(p1, 7);
	return strsans(p1);
}

int shifttime(double n, double bpm, int time) {
	return (int)(time + 240000.0 * (n - 1.0) / (bpm * 16.0));
}

void SETMove(double NowTime, double StartTime, double MovePoint,
	double MoveType, double EndTime, double bpm, rec_move_data_t *Buff)
{
	Buff->Stime = shifttime(StartTime, bpm, (int)NowTime);
	Buff->pos = (int)(MovePoint * 50.0 + 100.0);
	Buff->Etime = shifttime(EndTime, bpm, (int)NowTime) - 5;
	Buff->mode = (int)MoveType;
}

/* sub action */
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

#if SWITCH_NOTE_BOX_2
static void CalNoteViewTime(note_box_2_t *note, scrool_box scrool[]) {
	int num = 0;
	while (0 <= scrool[num + 1].starttime &&
		scrool[num + 1].starttime <= note->hittime) {
		num++;
	}
	note->viewtime = note->hittime * scrool[num].speed + scrool[num].basetime;
}
#else
static void CalNoteViewTime(note_box *note, scrool_box scrool[]) {
	int num = 0;
	while (0 <= scrool[num + 1].starttime &&
		scrool[num + 1].starttime <= note->hittime) {
		num++;
	}
	note->viewtime = note->hittime * scrool[num].speed + scrool[num].basetime;
}
#endif

void RecMapLoadSetMove(rec_move_set_t *move, unsigned int *allnum, int iLine,
	double StartTime, double MovePos, double EndTime, int MoveMode, double bpmG,
	double timer[])
{
	double Spos = (move->d[move->num - 1].pos - 100.0) / 50.0;
	switch (MoveMode) {
	case REC_MAP_MOVE_CODE_LIN:
	case REC_MAP_MOVE_CODE_ACC:
	case REC_MAP_MOVE_CODE_DEC:
		SETMove(timer[0], StartTime, MovePos, MoveMode, EndTime, bpmG, &move->d[move->num]);
		break;
	case REC_MAP_MOVE_CODE_MOM:
		SETMove(timer[0], StartTime, MovePos, 1, EndTime, bpmG, &move->d[move->num]);
		move->d[move->num].Stime -= 5;
		move->d[move->num].Etime -= 5;
		break;
	case REC_MAP_MOVE_CODE_SLI:
		SETMove(timer[0], StartTime, (Spos + MovePos) / 2.0, 2,
			(EndTime + StartTime) / 2.0, bpmG, &move->d[move->num]);
		move->num++;
		allnum[iLine]++;
		SETMove(timer[0], (EndTime + StartTime) / 2.0, MovePos, 3, EndTime, bpmG, &move->d[move->num]);
		break;
	case REC_MAP_MOVE_CODE_PAL:
		SETMove(timer[0], StartTime, MovePos, 3, (EndTime + StartTime) / 2.0, bpmG, &move->d[move->num]);
		move->num++;
		allnum[iLine]++;
		SETMove(timer[0], (EndTime + StartTime) / 2.0, Spos, 2, EndTime, bpmG, &move->d[move->num]);
		break;
	case REC_MAP_MOVE_CODE_EDG:
		SETMove(timer[0], StartTime, MovePos, 2, (EndTime + StartTime) / 2.0, bpmG, &move->d[move->num]);
		move->num++;
		allnum[iLine]++;
		SETMove(timer[0], (EndTime + StartTime) / 2.0, Spos, 3, EndTime, bpmG, &move->d[move->num]);
		break;
	}
	move->num++;
	allnum[iLine]++;
}

int RecMapLoadGetc(TCHAR c, int istr, note_box_2_t note[], int *objectN, int iLine, double timer[],
	int noteLaneNo[], double bpmG, int BlockNoteNum, struct custom_note_box customnote[],
	struct scrool_box scrool[], rec_move_all_set_t *move, short int YmoveN2[], short int XmoveN2[],
	short int *notes, playnum_box *allnum)
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
	//viewtime���v�Z����
	CalNoteViewTime(&note[*objectN], scrool);
	note[*objectN].ypos = 50 * iLine + 300;
	note[*objectN].xpos = 150;
	//�c�ʒu���v�Z����
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
	//���ʒu���v�Z����
	while (move->x[iLine].d[XmoveN2[iLine]].Etime <= note[*objectN].hittime &&
		move->x[iLine].d[XmoveN2[iLine]].Etime >= 0) {
		XmoveN2[iLine]++;
	}
	if (move->x[iLine].d[XmoveN2[iLine]].Stime >= 0 &&
		move->x[iLine].d[XmoveN2[iLine]].Stime <= note[*objectN].hittime &&
		move->x[iLine].d[XmoveN2[iLine]].Stime > note[*objectN].hittime) {
		note[*objectN].xpos = movecal(move->x[iLine].d[XmoveN2[iLine]].mode,
			move->x[iLine].d[XmoveN2[iLine]].Stime, 
			move->x[iLine].d[XmoveN2[iLine] - 1].pos,
			move->x[iLine].d[XmoveN2[iLine]].Etime, move->x[iLine].d[XmoveN2[iLine]].pos,
			note[*objectN].hittime);
	}
	else {
		note[*objectN].xpos = move->x[iLine].d[XmoveN2[iLine] - 1].pos;
	}
	//���ʉ���ݒ肷��
	if (L'1' <= c && c <= L'9') {
		note[*objectN].sound = customnote[c - L'1'].sound;
		note[*objectN].melody = customnote[c - L'1'].melody;
	}
	else {
		note[*objectN].sound = 0;
	}
	//�F��ݒ肷��
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

/* main action */
void RecordLoad2(int p, int n, int o) {
	//n: �ȃi���o�[
	//o: ��Փx�i���o�[
	short int i[2] = { 0,0 };
	short int Lv = 0;
	short int notes = 0;
#if 0 /* fixing... */
	int G[4] = { 0,0,0,0 };
#else
	int G[14] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
#endif
	int songT;
	int noteoff = 0; //�m�[�c�̃I�t�Z�b�g
	int Etime = 0; //���ʂ̏I���̎���
	int songdata = 0;
	int waningLv = 2;
	double GD[5] = { 0,0,0,0,0 };
	//int item[99]; //�A�C�e���̃A�h���X�ADrawGraph�ŌĂׂ�B
	//short int itemN = 0; //���̔ԍ�
	int chamo[3][99][2]; //�L������[0:��,1:��,2:��]��[0:�O���t�B�b�N,1:�؂�ւ�����]
	chamo[0][0][0] = 0;
	chamo[0][0][1] = 0;
	chamo[1][0][0] = 1;
	chamo[1][0][1] = 0;
	chamo[2][0][0] = 1;
	chamo[2][0][1] = 0;
	short int chamoN[3] = { 1,1,1 }; //���̔ԍ�
	int fall[99][2]; //�������w�i��[0:�ԍ�,1:����]
	fall[0][0] = 0;
	fall[0][1] = -1;
	short int fallN = 1; //���̔ԍ�
	rec_move_all_set_t move; // ���[���ړ��Z�b�g
	move.y[0].d[0].Stime = 0;
	move.y[0].d[0].pos = 300;
	move.y[0].d[0].Etime = 0;
	move.y[0].d[0].mode = 1;
	move.y[0].num = 1;
	move.y[1].d[0].Stime = 0;
	move.y[1].d[0].pos = 350;
	move.y[1].d[0].Etime = 0;
	move.y[1].d[0].mode = 1;
	move.y[1].num = 1;
	move.y[2].d[0].Stime = 0;
	move.y[2].d[0].pos = 400;
	move.y[2].d[0].Etime = 0;
	move.y[2].d[0].mode = 1;
	move.y[2].num = 1;
	move.y[3].d[0].Stime = 0;
	move.y[3].d[0].pos = 350;
	move.y[3].d[0].Etime = 0;
	move.y[3].d[0].mode = 1;
	move.y[3].num = 1;
	move.y[4].d[0].Stime = 0;
	move.y[4].d[0].pos = 600;
	move.y[4].d[0].Etime = 0;
	move.y[4].d[0].mode = 1;
	move.y[4].num = 1;
	move.x[0].d[0].Stime = 0;
	move.x[0].d[0].pos = 150;
	move.x[0].d[0].Etime = 0;
	move.x[0].d[0].mode = 1;
	move.x[0].num = 1;
	move.x[1].d[0].Stime = 0;
	move.x[1].d[0].pos = 150;
	move.x[1].d[0].Etime = 0;
	move.x[1].d[0].mode = 1;
	move.x[1].num = 1;
	move.x[2].d[0].Stime = 0;
	move.x[2].d[0].pos = 150;
	move.x[2].d[0].Etime = 0;
	move.x[2].d[0].mode = 1;
	move.x[2].num = 1;
	short int YmoveN2[3] = { 0,0,0 };
	short int XmoveN2[3] = { 0,0,0 };
	int lock[2][2][99]; //lock = [��,�c]�̉����̈ʒu��[(1=�Œ肷��,-1�ȊO=�Œ肵�Ȃ�),����]
	lock[0][0][0] = -1;
	lock[0][1][0] = 0;
	lock[1][0][0] = 1;
	lock[1][1][0] = 0;
	short int lockN[2] = { 1,1 }; //���̔ԍ�
	int carrow[2][99];
	carrow[0][0] = 1;
	carrow[1][0] = 0;
	short int carrowN = 1;
	int viewT[2][99];//[�����\������,���s����,[0]=���i���o�[]
	viewT[0][0] = 0;
	viewT[1][0] = 3000;
	short int viewTN = 1;
	item_box Movie[999];//�A�C�e���\��[�A�C�e���ԍ�,�ړ��`��,�J�n����,�I������,�J�nx�ʒu,�I��x�ʒu,�J�ny�ʒu,�I��y�ʒu,�J�n�T�C�Y,�I���T�C�Y,�J�n�p�x,�I���p�x,�J�n�����x,�I�������x]
	item_set_box item_set[99];
	short int MovieN = 0;
	struct camera_box camera[255];
	camera[0].starttime = 0;
	camera[0].endtime = 0;
	camera[0].xpos = 0;
	camera[0].ypos = 0;
	camera[0].zoom = 1;
	camera[0].rot = 0;
	camera[0].mode = 0;
	short int cameraN = 1; //���̔ԍ�
	struct custom_note_box customnote[9];
	struct scrool_box scrool[99];
	scrool[0].starttime = 0;
	scrool[0].basetime = 0;
	scrool[0].speed = 1;
	short int scroolN = 1;
#if SWITCH_NOTE_BOX_2
	note_box_2_t note[6000];
	int objectN = 0; //���̔ԍ�
	int noteLaneNo[3] = { -1,-1,-1 };
#else
	struct note_box note[3][2000];//[��,��,��]���[���̃m�[�c[�ԍ�]
	note[0][0].ypos = 300;
	note[1][0].ypos = 350;
	note[2][0].ypos = 400;
	short int objectN[3] = { 0,0,0 }; //���̔ԍ�
#endif
	int difkey[50][4];//��Փx�v�Z�Ɏg��[�ԍ�][���̓L�[,����,��Փx�_,[0]�����:[1]���̔ԍ�:[2]1�O�̔ԍ�:[3]2�O�̔ԍ�:[4]�ō��_:[5]�f�[�^��:[6]�Ō�50�̍��v:[7]�v�Z���珜�O���鎞��]
	difkey[0][2] = 0;
	difkey[1][2] = 0;
	difkey[1][3] = 0;
	difkey[2][3] = -1;
	difkey[3][3] = -2;
	difkey[4][3] = 0;
	int ddif[25] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//�e��Ԃ̓�Փx
	int ddifG[2] = { 1,1 };//0=�������Ԕԍ�(1�`25),1=�ő�l
	ddef_box ddif2;
	int outpoint[2] = { 0, 0 }; /* 0=����, 1=�G���[�ԍ� */
	view_BPM_box v_bpm[100];
	double bpm = 120, bpmG = 120;
	double timer[3] = { 0,0,0 }; //[��, ��, ��]���[���̎���
	double speedt[5][99][2]; //[��, ��, ��, (�n��), (����)]���[����[0:�؂�ւ�����,1:���x]
	speedt[0][0][0] = 0;
	speedt[0][0][1] = 1;
	speedt[1][0][0] = 0;
	speedt[1][0][1] = 1;
	speedt[2][0][0] = 0;
	speedt[2][0][1] = 1;
	speedt[3][0][0] = 0;
	speedt[3][0][1] = 1;
	speedt[4][0][0] = 0;
	speedt[4][0][1] = 1;
	short int speedN[5] = { 1,1,1,1,1 }; //���̔ԍ�
	wchar_t songN[255];
	wchar_t songNE[255];
	songN[0] = L'\0';
	wchar_t fileN[255];
	wchar_t dataE[255] = L"record/";//�t�H���_�̖��O
	wchar_t RRS[255]; //PC�p���ʃf�[�^�̕ۑ��ꏊ
	wchar_t mp3FN[255];
	wchar_t skyFN[255] = L"picture/backskynoamal.png";
	wchar_t groundFN[255] = L"picture/groundnaturenormal.png";
	wchar_t waterFN[255] = L"picture/waternormal.png";
	wchar_t DifFN[255] = L"picture/difanother.png";
	wchar_t GT1[255];
	wchar_t GT2[255];
	wchar_t GT24[] = L"picture/";
	wchar_t GT25[6][7] = { L"/0.txt" ,L"/1.txt" ,L"/2.txt" ,L"/3.txt" ,L"/4.txt" ,L"/5.txt" };
	wchar_t GT26[6][7] = { L"/0.rrs" ,L"/1.rrs" ,L"/2.rrs" ,L"/3.rrs" ,L"/4.rrs" ,L"/5.rrs" };
	wchar_t ST1[] = L"record/";
	wchar_t ST2[] = L"list.txt";
	playnum_box allnum;
	FILE *fp;
	songT = FileRead_open(L"RecordPack.txt");
	for (i[0] = 0; i[0] <= p; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcats(dataE, GT1); //"record/<�p�b�N��>"
	stradds(dataE, L'/'); //"record/<�p�b�N��>/"
	strcopy(dataE, GT2, 1);
	strcats(GT2, ST2); //"record/<�p�b�N��>/list.txt"
	songT = FileRead_open(GT2);
	for (i[0] = 0; i[0] <= n; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcopy(GT1, fileN, 1); //"<�Ȗ�>"
	strcats(dataE, GT1); //"record/<�p�b�N��>/<�Ȗ�>"
	strcopy(dataE, GT1, 1);
	strcats(GT1, GT25[o]); //"record/<�p�b�N��>/<�Ȗ�>/<��Փx>.txt"
	songdata = FileRead_open(GT1);
	if (songdata == 0) {
		return;
	}
	//�e�L�X�g�f�[�^��ǂ�
	while (FileRead_eof(songdata) == 0) {
		FileRead_gets(GT1, 256, songdata);
		//���y�t�@�C����ǂݍ���
		if (strands(GT1, L"#MUSIC:")) {
			strmods(GT1, 7);
			strcopy(dataE, mp3FN, 1);
			strcats(mp3FN, L"/");
			strcats(mp3FN, GT1);
		}
		//BPM��ǂݍ���
		else if (strands(GT1, L"#BPM:")) {
			bpmG = bpm = SETbpm(GT1);
			v_bpm[0].time = noteoff;
			v_bpm[0].BPM = (unsigned short)bpm;
		}
		//�m�[�g�̃I�t�Z�b�g��ǂݍ���
		else if (strands(GT1, L"#NOTEOFFSET:")) {
			timer[0] = timer[1] = timer[2] = noteoff = SEToffset(GT1);
		}
		//��̔w�i��ǂݍ���
		else if (strands(GT1, L"#SKY:")) {
			strcopy(GT24, skyFN, 1);
			strmods(GT1, 5);
			strcats(skyFN, GT1);
		}
		//�n�ʂ̉摜��ǂݍ���
		else if (strands(GT1, L"#FIELD:")) {
			strcopy(GT24, groundFN, 1);
			strmods(GT1, 7);
			strcats(groundFN, GT1);
		}
		//�����̉摜��ǂݍ���
		else if (strands(GT1, L"#WATER:")) {
			strcopy(GT24, waterFN, 1);
			strmods(GT1, 7);
			strcats(waterFN, GT1);
		}
		//��Փx�o�[(another)��ǂݍ���
		else if (strands(GT1, L"#DIFBAR:")) {
			strcopy(dataE, DifFN, 1);
			strmods(GT1, 8);
			stradds(DifFN, L'/');
			strcats(DifFN, GT1);
		}
		//�Ȗ���ǂݍ���
		else if (strands(GT1, L"#TITLE:")) {
			strmods(GT1, 7);
			strcopy(GT1, songN, 1);
		}
		//�p��
		else if (strands(GT1, L"#E.TITLE:")) {
			strmods(GT1, 7);
			strcopy(GT1, songNE, 1);
		}
		//���x����ǂݍ���
		else if (strands(GT1, L"#LEVEL:")) Lv = SETLv(GT1);
		//�������w�i�w��
		else if (strands(GT1, L"#FALL:")) {
			strmods(GT1, 6);
			fall[0][0] = strsans(GT1);
			fall[0][1] = 0;
		}
		//���ʓ�Փx�t�B���^�̃��x��
		else if (strands(GT1, L"#WANING:")) {
			strmods(GT1, 8);
			waningLv = strsans(GT1);
		}
		//���ʂ�ǂݍ���
		else if (strands(GT1, L"#MAP:")) {
			FileRead_gets(GT1, 256, songdata);
			while (FileRead_eof(songdata) == 0 && strands(GT1, L"#END") == 0) {
				switch (check_obj_code(GT1)) {
				case OBJ_CODE_MEMO: //�R�����g
				case OBJ_CODE_SPACE: //��
					break;
				case OBJ_CODE_SPEED: //�m�[�c�̑��x�ω�
					G[0] = maxs(mins(GT1[6] - 49, 0), 4);
					strmods(GT1, 8);
					speedt[G[0]][speedN[G[0]]][1] = strsans2(GT1);
					strnex(GT1);
					if (GT1[0] >= L'0' && GT1[0] <= L'9' || GT1[0] == L'-') {
						speedt[G[0]][speedN[G[0]]][0] = timer[G[0]] + 240000 * (speedt[G[0]][speedN[G[0]]][1] - 1) / (bpmG * 16) - 10;
						speedt[G[0]][speedN[G[0]]][1] = strsans2(GT1);
					}
					else {
						speedt[G[0]][speedN[G[0]]][0] = timer[G[0]] - 10;
					}
					speedN[G[0]]++;
					break;
				case OBJ_CODE_BPM: //�f�[�^������BPM�ω�
					bpmG = SETbpm(GT1);
					break;
				case OBJ_CODE_VBPM: //�����ڂ�BPM�ω�
					strmods(GT1, 7);
					v_bpm[allnum.v_BPMnum].time = shifttime(strsans(GT1), bpmG, (int)timer[0]);
					strnex(GT1);
					v_bpm[allnum.v_BPMnum].BPM = strsans(GT1);
					allnum.v_BPMnum++;
					break;
				case OBJ_CODE_CHARA: //�L�����O���ω�
					G[0] = GT1[6] - 49;
					strmods(GT1, 8);
					chamo[G[0]][chamoN[G[0]]][0] = maxs(mins(strsans(GT1), 0), 2);
					chamo[G[0]][chamoN[G[0]]][1] = (int)timer[G[0]];
					chamoN[G[0]]++;
					break;
				case OBJ_CODE_MOVE: //�c�ړ�
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
						G[2] = G[0] = maxs(mins(GT1[8] - 49, 0), 4);
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
						RecMapLoadSetMove(&move.y[i[0]], allnum.Ymovenum, i[0], GD[0],
							GD[1] + G[3] * i[0] - G[3], GD[2], G[1], bpmG, timer);
					}
					break;
				case OBJ_CODE_XMOV: //���ړ�
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
						G[2] = G[0] = maxs(mins(GT1[8] - 49, 0), 2);
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
						RecMapLoadSetMove(&move.x[i[0]], allnum.Xmovenum, i[0], GD[0],
							GD[1] + G[3] * i[0] - G[3], GD[2], G[1], bpmG, timer);
					}
					break;
				case OBJ_CODE_DIV: //�U��
					G[0] = betweens(0, GT1[5] - L'1', 2);
					G[1] = 0;
					if (GT1[4] == L'Y') { G[1] = 1; }
					strmods(GT1, 7);
					GD[0] = strsans2(GT1);//�J�n����
					strnex(GT1);
					GD[1] = strsans2(GT1);//�U���ʒu
					strnex(GT1);
					GD[2] = strsans2(GT1) / 2.0;//��������
					strnex(GT1);
					GD[3] = strsans2(GT1);//������
					if (G[1] == 1) {
						for (i[0] = 0; i[0] < GD[3]; i[0]++) {
							SETMove(timer[0], GD[0], GD[1], 1, GD[0] + GD[2],
								bpmG, &move.y[G[0]].d[move.y[G[0]].num]);
							SETMove(timer[0], GD[0] + GD[2],
								(move.y[G[0]].d[move.y[G[0]].num - 1].pos - 100.0) / 50.0,
								1, GD[0] + GD[2] * 2, bpmG, &move.y[G[0]].d[move.y[G[0]].num + 1]);
							GD[0] += GD[2] * 2;
							move.y[G[0]].num += 2;
							allnum.Ymovenum[G[0]] += 2;
						}
					}
					else {
						for (i[0] = 0; i[0] < GD[3]; i[0]++) {
							SETMove(timer[0], GD[0], GD[1], 1, GD[0] + GD[2],
								bpmG, &move.x[G[0]].d[move.x[G[0]].num]);
							SETMove(timer[0], GD[0] + GD[2],
								(move.x[G[0]].d[move.x[G[0]].num - 1].pos - 100.0) /
								50.0, 1, GD[0] + GD[2] * 2, bpmG, &move.x[G[0]].d[move.x[G[0]].num] + 1);
							GD[0] += GD[2] * 2;
							move.x[G[0]].num += 2;
							allnum.Xmovenum[G[0]] += 2;
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
					RecMapLoadSetMove(&move.y[3], allnum.Ymovenum, 3,
						GD[0], GD[1], GD[2], G[1], bpmG, timer);
					break;
				case OBJ_CODE_XLOCK: //�����b�N
					strmods(GT1, 7);
					lock[0][0][lockN[0]] = lock[0][0][lockN[0] - 1] * -1;
					lock[0][1][lockN[0]] = shifttime(strsans(GT1), bpmG, (int)timer[0]);
					lockN[0]++;
					break;
				case OBJ_CODE_YLOCK: //�c���b�N
					strmods(GT1, 7);
					lock[1][0][lockN[1]] = lock[1][0][lockN[1] - 1] * -1;
					lock[1][1][lockN[1]] = shifttime(strsans(GT1), bpmG, (int)timer[0]);
					lockN[1]++;
					break;
				case OBJ_CODE_CARROW: //�L���������ω�
					strmods(GT1, 8);
					carrow[0][carrowN] = carrow[0][carrowN - 1] * -1;
					carrow[1][carrowN] = shifttime(strsans(GT1), bpmG, (int)timer[0]);
					carrowN++;
					break;
				case OBJ_CODE_FALL: //�������w�i�؂�ւ�
					strmods(GT1, 6);
					fall[fallN][0] = strsans(GT1);
					strnex(GT1);
					fall[fallN][1] = strsans(GT1);
					fallN++;
					break;
				case OBJ_CODE_VIEW: //�����\������
					strmods(GT1, 6);
					viewT[0][viewTN] = shifttime(strsans(GT1), bpmG, (int)timer[0]);
					strnex(GT1);
					viewT[1][viewTN] = strsans(GT1);
					viewTN++;
					break;
				case OBJ_CODE_MOVIE: //�A�C�e���\��
					strmods(GT1, 7);
					Movie[MovieN].ID = strsans(GT1);
					strnex(GT1);
					switch (GT1[0]) {
					case L'l':
						Movie[MovieN].movemode = 1;
						break;
					case L'a':
						Movie[MovieN].movemode = 2;
						break;
					case L'd':
						Movie[MovieN].movemode = 3;
						break;
					}
					strnex(GT1);
					Movie[MovieN].starttime = shifttime(strsans2(GT1), bpmG, (int)timer[0]);
					strnex(GT1);
					Movie[MovieN].endtime = shifttime(strsans2(GT1), bpmG, (int)timer[0]);
					strnex(GT1);
					Movie[MovieN].startXpos = (int)(strsans2(GT1) * 50 + 115);
					strnex(GT1);
					Movie[MovieN].endXpos = (int)(strsans2(GT1) * 50 + 115);
					strnex(GT1);
					Movie[MovieN].startYpos = (int)(strsans2(GT1) * 50 + 115);
					strnex(GT1);
					Movie[MovieN].endYpos = (int)(strsans2(GT1) * 50 + 115);
					strnex(GT1);
					Movie[MovieN].startsize = (int)(strsans2(GT1) * 100);
					strnex(GT1);
					Movie[MovieN].endsize = (int)(strsans2(GT1) * 100);
					strnex(GT1);
					Movie[MovieN].startrot = strsans(GT1);
					strnex(GT1);
					Movie[MovieN].endrot = strsans(GT1);
					strnex(GT1);
					Movie[MovieN].startalpha = (int)(strsans2(GT1) * 255.0);
					strnex(GT1);
					Movie[MovieN].endalpha = (int)(strsans2(GT1) * 255.0);
					strnex(GT1);
					Movie[MovieN].eff = set_pic_mat(GT1);
					MovieN++;
					allnum.movienum++;
					break;
				case OBJ_CODE_INIT_ITEM_SET: //�A�C�e���Z�b�g������
					strmods(GT1, 15);
					G[0] = strsans(GT1);
					item_set[G[0]].num = 0;
					break;
				case OBJ_CODE_ADD_ITEM_SET: //�A�C�e���Z�b�g�ǉ�
					if (item_set[G[0]].num <= 10) {
						strmods(GT1, 14);
						G[0] = strsans(GT1); /* G[0] = item set�̔ԍ� */
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
				case OBJ_CODE_ITEM_SET: //�A�C�e���Z�b�g�\��
#if 0 /* fixing... */
					set_item_set(&Movie[MovieN], &MovieN, &allnum, &GT1[0], item_set, bpmG, timer[0]);
#else
					strmods(GT1, 10);
					G[0] = strsans(GT1); /* G[0] = item box�̔ԍ� */
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
					} /* G[1] = �ړ����[�h */
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
						Movie[MovieN].ID = item_set[G[0]].picID[i[0]].picID;
						Movie[MovieN].movemode = G[1];
						Movie[MovieN].eff = item_set[G[0]].picID[i[0]].eff;
						Movie[MovieN].starttime = G[2];
						Movie[MovieN].endtime = G[3];
						Movie[MovieN].startXpos = item_set[G[0]].picID[i[0]].Xpos * G[8] / 100;
						Movie[MovieN].endXpos = item_set[G[0]].picID[i[0]].Xpos * G[9] / 100;
						Movie[MovieN].startYpos = item_set[G[0]].picID[i[0]].Ypos * G[8] / 100;
						Movie[MovieN].endYpos = item_set[G[0]].picID[i[0]].Ypos * G[9] / 100;
						rot_xy_pos(G[10], &Movie[MovieN].startXpos, &Movie[MovieN].startYpos);
						rot_xy_pos(G[10], &Movie[MovieN].endXpos, &Movie[MovieN].endYpos);
						Movie[MovieN].startXpos += G[4];
						Movie[MovieN].endXpos += G[5];
						Movie[MovieN].startYpos += G[6];
						Movie[MovieN].endYpos += G[7];
						Movie[MovieN].startsize = G[8] * item_set[G[0]].picID[i[0]].size / 100;
						Movie[MovieN].endsize = G[9] * item_set[G[0]].picID[i[0]].size / 100;
						Movie[MovieN].startrot = G[10] + item_set[G[0]].picID[i[0]].rot;
						Movie[MovieN].endrot = G[11] + item_set[G[0]].picID[i[0]].rot;
						Movie[MovieN].startalpha = G[12] * item_set[G[0]].picID[i[0]].alpha / 255;
						Movie[MovieN].endalpha = G[13] * item_set[G[0]].picID[i[0]].alpha / 255;
						MovieN++;
						allnum.movienum++;
					}
#endif
					break;
				case OBJ_CODE_CAMERA: //�J�����ړ�+�Y�[��+�p�x(������)
					strmods(GT1, 8);
					camera[cameraN].starttime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					camera[cameraN].endtime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					camera[cameraN].xpos = strsans2(GT1) * 50;
					strnex(GT1);
					camera[cameraN].ypos = strsans2(GT1) * 50;
					strnex(GT1);
					camera[cameraN].zoom = strsans2(GT1);
					strnex(GT1);
					camera[cameraN].rot = strsans2(GT1);
					strnex(GT1);
					switch (GT1[0]) {
					case L'a':
						camera[cameraN].mode = 2;
						break;
					case L'd':
						camera[cameraN].mode = 3;
						break;
					default:
						camera[cameraN].mode = 1;
						break;
					}
					cameraN++;
					break;
				case OBJ_CODE_CAMMOVE: //�J�����ړ�
					if (strands(GT1, L"#CMOV:")) { strmods(GT1, 6); }
					if (strands(GT1, L"#CAMMOVE:")) { strmods(GT1, 9); }
					camera[cameraN].starttime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					camera[cameraN].endtime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					camera[cameraN].xpos = strsans2(GT1) * 50;
					strnex(GT1);
					camera[cameraN].ypos = strsans2(GT1) * 50;
					strnex(GT1);
					switch (GT1[0]) {
					case L'a':
						camera[cameraN].mode = 2;
						break;
					case L'd':
						camera[cameraN].mode = 3;
						break;
					default:
						camera[cameraN].mode = 1;
						break;
					}
					cameraN++;
					break;
				case OBJ_CODE_SCROOL: //�X�N���[��
					strmods(GT1, 8);
					scrool[scroolN].starttime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					scrool[scroolN].speed = strsans2(GT1);
					G[0] = scrool[scroolN - 1].speed *
						scrool[scroolN].starttime + scrool[scroolN - 1].basetime;
					scrool[scroolN].basetime = G[0] - scrool[scroolN].speed *
						scrool[scroolN].starttime;
					scroolN++;
					break;
				case OBJ_CODE_CUSTOM: //�J�X�^���m�[�c�Z�b�g
					RecMapLoad_ComCustomNote(GT1, customnote);
					break;
				default:
					//����ȊO
					for (int iLine = 0; iLine <= 2; iLine++) {
						int BlockNoteNum = 0;
						G[0] = 0;
						while (GT1[BlockNoteNum] != L'\0' && GT1[BlockNoteNum] != L',') { BlockNoteNum++; }
						for (int istr = 0; istr < BlockNoteNum; istr++) {
							RecMapLoadGetc(GT1[istr], istr, note, &objectN, iLine, timer,
								noteLaneNo, bpmG, BlockNoteNum, customnote, scrool, &move, YmoveN2,
								XmoveN2, &notes, &allnum);
						}
						if (iLine <= 1) { FileRead_gets(GT1, 256, songdata); }
					}
					timer[0] = timer[1] = timer[2] += 240000.0 / bpmG;
					break;
				}
				FileRead_gets(GT1, 256, songdata);
			}
		}
	}
	FileRead_close(songdata);
	//���ʂ̍Ō��goust��u��
#if SWITCH_NOTE_BOX_2 == 1
	note[objectN].lane = NOTE_LANE_MID;
	note[objectN].hittime = timer[i[0]];
	note[objectN + 1].hittime = -1;
	note[objectN].object = NOTE_GHOST;
	note[objectN].ypos = 1000;
#else
	for (i[0] = 0; i[0] < 3; i[0]++) {
		note[i[0]][objectN[i[0]]].hittime = timer[i[0]];
		note[i[0]][objectN[i[0]] + 1].hittime = -1;
		note[i[0]][objectN[i[0]]].object = NOTE_GHOST;
		note[i[0]][objectN[i[0]]].ypos = 1000;
	}
#endif
	lock[0][0][lockN[0]] = 1;
	lock[0][1][lockN[0]] = -1;
	lock[1][0][lockN[1]] = -1;
	lock[1][1][lockN[1]] = -1;
#if SWITCH_NOTE_BOX_2 == 1
	allnum.notenum[1]++;
#else
	allnum.notenum[0]++;
	allnum.notenum[1]++;
	allnum.notenum[2]++;
#endif
	Etime = timer[0];

	/*��Փx�v�Z
	(3000000/�ЂƂO�̃m�[�c�Ƃ̊Ԋu)���A���̃m�[�c�̓�Փx�_���Ƃ���B
	�@�����L�[��50�L�����A���̑g�ݍ��킹�łł����ō��l���Ȃ̓�Փx�B
	1�O�Ɠ����L�[�̎���(�Ώۂ̃L�[��arrow�̂�)
	���Ԋu��150ms�ȏ�̎��͕ϓ��Ȃ�(BPM200��8��)
	���Ԋu��150ms�ȉ�75ms�ȏ�̎��͓��_("�Ԋu"/(-75)+3)�{(1�`2�{)
	���Ԋu��75ms�ȉ��̎��͓��_2�{(BPM200��16��)
	1=hit,2=non,3=up,4=down,5=left,6=right,7=non or down,8=up or down,9=up or non
	*/

#define ARROW_TRICK_MLP(base) (base * 18 / 10) /* arrow�Ђ�������1.8�{ */
#define DIF_BBEF_MLP(base) (base * 12 / 10) /* 2�O�ƈႤ�L�[�̎��͓��_1.2�{(�S�L�[�Ώ�) */
#define ARROW_MLP(base) (base * 12 / 10) /* arrow�L�[�͓��_1.2�{ */

	/* ���̑��{����recp_cal_ddif.cpp�ɋL�� */

#if 0
	objectN[0] = 0;
	objectN[1] = 0;
	objectN[2] = 0;
	difkey[0][3] = notes;
	if (difkey[0][3] > 49)difkey[0][3] = 49;
	difkey[7][3] = (Etime - noteoff) / 25 * 2;
	if (difkey[7][3] < 10000)difkey[7][3] = 10000;
	DifkeyCalInit(notes, Etime - noteoff);

	//�m�[�c���Ȃ��Ȃ�܂ŌJ��Ԃ�
	while (note[0][objectN[0]].hittime >= 0 ||
		note[1][objectN[1]].hittime >= 0 ||
		note[2][objectN[2]].hittime >= 0) {
		//GHOST�m�[�c���X�L�b�v
		for (i[0] = 0; i[0] < 3; i[0]++) {
			while (note[i[0]][objectN[i[0]]].object == 8 &&
				note[i[0]][objectN[i[0]]].hittime >= 0) {
				objectN[i[0]]++;
			}
		}
		G[0] = -1;
		//��ԑ����m�[�c��T����G[0]�ɑ��
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (note[i[0]][objectN[i[0]]].hittime >= 0) {
				G[0] = i[0];
				break;
			}
		}
		//����������u���[�N
		if (G[0] == -1) break;
		//��ԑ����m�[�c��T����G[0]�ɑ��
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (G[0] != i[0] && note[G[0]][objectN[G[0]]].hittime >
				note[i[0]][objectN[i[0]]].hittime &&
				note[i[0]][objectN[i[0]]].hittime >= 0) {
				G[0] = i[0];
			}
			else if (G[0] != i[0] &&
				note[G[0]][objectN[G[0]]].hittime ==
				note[i[0]][objectN[i[0]]].hittime &&
				note[G[0]][objectN[G[0]]].object == 2 &&
				note[i[0]][objectN[i[0]]].object != 2 &&
				note[i[0]][objectN[i[0]]].hittime >= 0) {
				G[0] = i[0];
			}
		}
		DifkeyCalInsertNote(&note[G[0]][objectN[G[0]]], G[0]);
		//ddif�̌v�Z
		while (note[G[0]][objectN[G[0]]].hittime >=
			(Etime - noteoff) / 25 * ddif2.nowdifsection + noteoff) {
			ddif[ddif2.nowdifsection - 1] = cal_ddif(ddif2.datanum, difkey[0],
				Etime, noteoff, ddif2.nowdifsection, difkey[7][3]);
			ddif2.nowdifsection++;
		}
		difkey[difkey[1][3]][0] = note[G[0]][objectN[G[0]]].object;
		difkey[difkey[1][3]][1] = note[G[0]][objectN[G[0]]].hittime;

		G[2] = MapErrorCheck(difkey[difkey[1][3]][0], difkey[difkey[1][3]][1],
			difkey[difkey[2][3]][0], difkey[difkey[2][3]][1], o, waningLv);
		if (G[2] != 0 && outpoint[1] == 0) {
			outpoint[0] = difkey[difkey[1][3]][1];
			outpoint[1] = G[2];
		}
		switch (difkey[difkey[1][3]][0]) {
		case 1: //hit�m�[�c���
			if (difkey[difkey[2][3]][0] == 1 &&
				difkey[difkey[1][3]][1] - 20 < difkey[difkey[2][3]][1]) {
				difkey[difkey[2][3]][2] *= 1;
				objectN[G[0]]++;
				continue;
			}
			break;
		case 2: //catch�m�[�c���
			if (G[0] != 1) difkey[difkey[1][3]][0] = G[0] / 2 + 3;
			if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0]) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 3 &&
				(difkey[difkey[2][3]][0] == 8 ||
				difkey[difkey[2][3]][0] == 9)) {
				difkey[difkey[2][3]][0] = 3;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 2 && (difkey[difkey[2][3]][0] == 1 || difkey[difkey[2][3]][0] == 5 || difkey[difkey[2][3]][0] == 6)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 2 && (difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9)) {
				difkey[difkey[2][3]][0] = 2;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 4 && (difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 8)) {
				difkey[difkey[2][3]][0] = 4;
				objectN[G[0]]++;
				continue;
			}
			break;
		case 5: //left�m�[�c���
			if (difkey[difkey[2][3]][0] == 2 || difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9) {
				difkey[difkey[2][3]][0] = 5;
				continue;
			}
			break;
		case 6: //right�m�[�c���
			if (difkey[difkey[2][3]][0] == 2 || difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9) {
				difkey[difkey[2][3]][0] = 6;
				continue;
			}
			break;
		case 7: //bomb�m�[�c���
			if (G[0] != 0)difkey[difkey[1][3]][0] = G[0] + 7;
			if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0]) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && difkey[difkey[2][3]][0] == 8 || difkey[difkey[1][3]][0] == 8 && difkey[difkey[2][3]][0] == 7) {
				difkey[difkey[2][3]][0] = 4;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && difkey[difkey[2][3]][0] == 9 || difkey[difkey[1][3]][0] == 9 && difkey[difkey[2][3]][0] == 7) {
				difkey[difkey[2][3]][0] = 2;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 8 && difkey[difkey[2][3]][0] == 9 || difkey[difkey[1][3]][0] == 9 && difkey[difkey[2][3]][0] == 8) {
				difkey[difkey[2][3]][0] = 3;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && (difkey[difkey[2][3]][0] != 3)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 8 && (difkey[difkey[2][3]][0] == 3 || difkey[difkey[2][3]][0] == 4)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 9 && (difkey[difkey[2][3]][0] != 4)) {
				objectN[G[0]]++;
				continue;
			}
			break;
		}
		/* calculate difkey */
		if (difkey[2][3] != -1 && difkey[3][3] != -1) {
			difkey[difkey[1][3]][2] = cal_difkey(difkey[difkey[1][3]][1],
				difkey[difkey[2][3]][1], difkey[difkey[1][3]][0],
				difkey[difkey[2][3]][0], difkey[difkey[3][3]][0],
				difkey[difkey[2][3]][2]);
		}
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (note[G[0]][objectN[G[0]]].object >= 3 && note[G[0]][objectN[G[0]]].object <= 6 &&
				G[0] != i[0] && note[G[0]][objectN[G[0]]].object == note[i[0]][objectN[i[0]]].object &&
				note[G[0]][objectN[G[0]]].hittime + 5 >= note[i[0]][objectN[i[0]]].hittime) {
				objectN[i[0]]++;
			}
		}
		objectN[G[0]]++;
		ddif2.datanum++;
		G[0] = 0;
		ddif2.maxdif = mins(ddif2.maxdif,
			cal_nowdif_m(difkey[0], difkey[0][3], difkey[1][3], difkey[7][3]));
		for (i[0] = 1; i[0] < 4; i[0]++) {
			difkey[i[0]][3]++;
			if (difkey[i[0]][3] > difkey[0][3])difkey[i[0]][3] = 0;
		}
	}
	ddif2.datanum++;
	for (i[0] = 0; i[0] < 2; i[0]++) {
		if (difkey[i[0]][2] == 0 && difkey[2][2] > 0) { ddif2.datanum--; }
	}
	if (ddif2.datanum < 1) { ddif2.datanum = 1; }
	if (ddif2.datanum > 50) { ddif2.datanum = 50; }
	ddifG[1] = ddif2.maxdif;
	if (ddifG[1] <= 0) { ddifG[1] = 1; }
	ddif2.maxdif /= 50;
	ddif[ddif2.nowdifsection - 1] = 0;
	for (i[0] = 0; i[0] < ddif2.datanum; i[0]++) {
		if (difkey[i[0]][1] > Etime - difkey[7][3]) {
			ddif[ddif2.nowdifsection - 1] += difkey[i[0]][2];
		}
	}
	for (i[0] = ddif2.nowdifsection - 1; i[0] <= 24; i[0]++) {
		ddif[i[0]] = ddif[ddif2.nowdifsection - 1];
	}
	ddif2.lastdif = ddif[ddif2.nowdifsection - 1] / 50;
	//NEEDYOU:Lv.1.0->2713, Co-op katyohugetsu:Lv.8.0->34733
	ddif2.maxdif = lins(2713, 100, 34733, 800, ddif2.maxdif);
	ddif2.lastdif = lins(2713, 100, 34733, 800, ddif2.lastdif);
#endif

	//��������PC�p���ʃf�[�^�̃t�@�C���̍쐬(�Z�[�u���)
	strcopy(dataE, RRS, 1);
	strcats(RRS, GT26[o]);
	G[2] = _wfopen_s(&fp, RRS, L"wb");
	fwrite(&allnum, sizeof(playnum_box), 1, fp);//�e�f�[�^�̌�
	fwrite(&mp3FN, 255, 1, fp);//���y�t�@�C����
	fwrite(&bpm, sizeof(double), 1, fp);//BPM
	fwrite(&noteoff, sizeof(int), 1, fp);//offset
	fwrite(&skyFN, 255, 1, fp);//��w�i��
	fwrite(&groundFN, 255, 1, fp);//�n�ʉ摜��
	fwrite(&waterFN, 255, 1, fp);//�����摜��
	fwrite(&songN, 255, 1, fp);//�Ȗ�
	fwrite(&songNE, 255, 1, fp);//�Ȗ�(�p��)
	fwrite(&Lv, sizeof(short int), 1, fp);//���x��
	//fwrite(&item, sizeof(int), 99, fp);//�A�C�e���摜�f�[�^(���얢�m�F)
	fwrite(&fall, sizeof(int), 198, fp);//�������w�i�؂�ւ��^�C�~���O
	fwrite(&speedt, sizeof(double), 990, fp);//���[�����x
	fwrite(&chamo, sizeof(int), 594, fp);//�L�����O���ϊ��^�C�~���O
	fwrite(&move.y[0].d, sizeof(rec_move_data_t), allnum.Ymovenum[0], fp);//�ヌ�[���c�ʒu�ړ��^�C�~���O
	fwrite(&move.y[1].d, sizeof(rec_move_data_t), allnum.Ymovenum[1], fp);//�����[���c�ʒu�ړ��^�C�~���O
	fwrite(&move.y[2].d, sizeof(rec_move_data_t), allnum.Ymovenum[2], fp);//�����[���c�ʒu�ړ��^�C�~���O
	fwrite(&move.y[3].d, sizeof(rec_move_data_t), allnum.Ymovenum[3], fp);//�n�ʏc�ʒu�ړ��^�C�~���O
	fwrite(&move.y[4].d, sizeof(rec_move_data_t), allnum.Ymovenum[4], fp);//���ʏc�ʒu�ړ��^�C�~���O
	fwrite(&move.x[0].d, sizeof(rec_move_data_t), allnum.Xmovenum[0], fp);//�ヌ�[�����ʒu�ړ��^�C�~���O
	fwrite(&move.x[1].d, sizeof(rec_move_data_t), allnum.Xmovenum[1], fp);//�����[�����ʒu�ړ��^�C�~���O
	fwrite(&move.x[2].d, sizeof(rec_move_data_t), allnum.Xmovenum[2], fp);//�����[�����ʒu�ړ��^�C�~���O
	fwrite(&lock, sizeof(int), 396, fp);//�m�[�c�Œ�؂�ւ��^�C�~���O
	fwrite(&carrow, sizeof(int), 198, fp);//�L���������؂�ւ��^�C�~���O
	fwrite(&viewT, sizeof(int), 198, fp);//�m�[�c�\�����ԕϊ��^�C�~���O
#if SWITCH_NOTE_BOX_2
	fwrite(&note, sizeof(note_box_2_t), allnum.notenum[0] + allnum.notenum[1] + allnum.notenum[2], fp); /* �m�[�c�f�[�^ */
#else
	fwrite(&note[0], sizeof(struct note_box), allnum.notenum[0], fp); /* �ヌ�[���m�[�c�f�[�^ */
	fwrite(&note[1], sizeof(struct note_box), allnum.notenum[1], fp); /* �����[���m�[�c�f�[�^ */
	fwrite(&note[2], sizeof(struct note_box), allnum.notenum[2], fp); /* �����[���m�[�c�f�[�^ */
#endif
	fwrite(&notes, sizeof(short int), 1, fp);//�m�[�c��
	fwrite(&Etime, sizeof(int), 1, fp);//�ȏI������
	G[0] = ddif2.maxdif;//�ō���Փx
	G[1] = ddif2.lastdif;//�ŏI��Փx
	fwrite(&G, sizeof(int), 2, fp);
	fwrite(&ddif, sizeof(int), 25, fp);//�e��ԓ�Փx�f�[�^
	fwrite(&ddif2.nowdifsection, sizeof(int), 1, fp);//�e��ԓ�Փx�f�[�^
	fwrite(&ddifG[1], sizeof(int), 1, fp);//�e��ԓ�Փx�f�[�^
	fwrite(&DifFN, 255, 1, fp);//��Փx�o�[��
	fwrite(&Movie, sizeof(item_box), allnum.movienum, fp);//����f�[�^
	fwrite(&camera, sizeof(struct camera_box), 255, fp);//�J�����f�[�^
	fwrite(&scrool, sizeof(struct scrool_box), 99, fp);//�X�N���[���f�[�^
	fwrite(&v_bpm, sizeof(view_BPM_box), allnum.v_BPMnum, fp);//�����ڂ�BPM�f�[�^
	fwrite(&outpoint, sizeof(int), 2, fp);//���ʃG���[
	fclose(fp);
	return;
}

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

void set_item_set(item_box* const Movie, short* const MovieN,
	playnum_box* allnum, wchar_t* const s, item_set_box const* const item_set,
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