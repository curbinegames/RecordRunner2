
#include "sancur.h"
#include "playbox.h"
#include "recp_cal_ddif.h"

/* 間隔が5ms以下のノートは同時押し扱い。hitは1.3倍、arrowは1.6倍 */
#define MULTI_ARROW_MLP(x) ((x) * 16 / 10)
#define MULTI_NOTE_MLP(x) ((x) * 13 / 10)

 /* 2個前と違うキーの時は1.2倍(全キー対象) */
#define DIF_BBEF_MLP(x) ((x) * 12 / 10)

 /* arrowキーは1.2倍 */
#define ARROW_MLP(x) ((x) * 12 / 10)

typedef struct ddif_box_s ddif_box_t;
struct ddif_box_s {
	int time = 0;
	int note = 0;
	int dif = 0;
	ddif_box_t* bef = NULL;
	ddif_box_t* bbef = NULL;
};

static int cal_ddif_solo_push(ddif_box_t ddif) {
	int gap = ddif.time - ddif.bef->time;
	int ret = 3000000 / gap;
	/* arrowの縦連 */
	if (ddif.note == ddif.bef->note && 3 <= ddif.note && ddif.note <= 6) {
		if (gap <= 75) {
			ret *= 2;
		}
		else if (75 < gap && gap < 150) {
			ret = lins(75, 200 * ret, 150, 100 * ret, gap) / 100;
		}
	}
	/* notトリル判定 */
	if (ddif.note != ddif.bbef->note) {
		ret = DIF_BBEF_MLP(ret);
	}
	/* arrow補正 */
	if (ddif.note >= 3 && ddif.note <= 6) {
		ret = ARROW_MLP(ret);
	}
	return ret;
}

static int cal_ddif_mix_push(ddif_box_t ddif) {
	if (3 <= ddif.note && ddif.note <= 6) {
		return MULTI_ARROW_MLP(ddif.bef->dif);
	}
	return MULTI_NOTE_MLP(ddif.bef->dif);
}

static int CalGhostCount(note_box note[]) {
	int num = 0;
	while (note[num].object == 8 && note[num].hittime >= 0) { num++; }
	return num;
}

static int cal_ddif(int num, int const* difkey, int Etime, int noteoff, int difsec,
	int voidtime) {
	int ret = 0;
	int count = 0;
	if (num >= 50) { num = 49; }
	for (int i = 0; i < num; i++) {
		if (difkey[i * 4 + 1] > (Etime - noteoff) / 25 *
			difsec - voidtime + noteoff) {
			count++;
			ret += difkey[i * 4 + 2];
		}
	}
	if (count == 0) { return 0; }
	else { return ret * 50 / count; }
}

static char DdifCatchFix(char lane, int* NdifCom, int* BdifCom) {
	if (lane != 1) { *NdifCom = lane / 2 + 3; }
	if (*NdifCom == *BdifCom) { return 1; } /* continue */
	else if (*NdifCom == 3 && (*BdifCom == 8 || *BdifCom == 9)) {
		*BdifCom = 3;
		return 1; /* continue */
	}
	else if (*NdifCom == 2 &&
		(*BdifCom == 1 || *BdifCom == 5 || *BdifCom == 6)) {
		return 1; /* continue */
	}
	else if (*NdifCom == 2 && (*BdifCom == 7 || *BdifCom == 9)) {
		*BdifCom = 2;
		return 1; /* continue */
	}
	else if (*NdifCom == 4 && (*BdifCom == 7 || *BdifCom == 8)) {
		*BdifCom = 4;
		return 1; /* continue */
	}
	return 0;
}

static char DdifBombFix(char lane, int* NdifCom, int* BdifCom) {
	if (lane != 0) { *NdifCom = lane + 7; }
	if (*NdifCom == *BdifCom) { return 1; } /* continue */
	else if (*NdifCom == 7 && *BdifCom == 8 || *NdifCom == 8 && *BdifCom == 7) {
		*BdifCom = 4;
		return 1; /* continue */
	}
	else if (*NdifCom == 7 && *BdifCom == 9 || *NdifCom == 9 && *BdifCom == 7) {
		*BdifCom = 2;
		return 1; /* continue */
	}
	else if (*NdifCom == 8 && *BdifCom == 9 || *NdifCom == 9 && *BdifCom == 8) {
		*BdifCom = 3;
		return 1; /* continue */
	}
	else if (*NdifCom == 7 && (*BdifCom != 3)) { return 1; } /* continue */
	else if (*NdifCom == 8 && (*BdifCom == 3 || *BdifCom == 4)) { return 1; } /* continue */
	else if (*NdifCom == 9 && (*BdifCom != 4)) { return 1; } /* continue */
	return 0;
}

static int MapErrorCheck(int nownote, int nowtime, int befnote, int beftime,
	int dif, int wl) {
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

static void CheckDdif(int hittime, int Etime, int noteoff, ddef_box* ddif2,
	int ddif[], int* Fdif, int voidtime) {
	while (hittime >=
		(Etime - noteoff) / 25 * ddif2->nowdifsection + noteoff) {
		ddif[ddif2->nowdifsection - 1] = cal_ddif(ddif2->datanum, Fdif,
			Etime, noteoff, ddif2->nowdifsection, voidtime);
		ddif2->nowdifsection++;
	}
	return;
}

static int cal_nowdif_m(int* difkey, int num, int now, int voidtime) {
	int ret = 0;
	int count = 0;
	for (int i = 0; i <= num; i++) {
		if (difkey[i * 4 + 2] < 0) { break; }
		if (difkey[i * 4 + 1] > difkey[now * 4 + 1] - voidtime) {
			ret += difkey[i * 4 + 2];
			count++;
		}
	}
	if (count == 0) { return 0; }
	else { return ret * 50 / count; }
}

static void DdifCal5(int Ndif[], int Bdif[], note_box *note, char dif,
	int waningLv, int outpoint[]) {
	int G[1];
	//difkeyへのデータ登録
	Ndif[0] = note->object;
	Ndif[1] = note->hittime;
	//譜面規約チェック
	G[0] = MapErrorCheck(Ndif[0], Ndif[1], Bdif[0], Bdif[1], dif, waningLv);
	if (G[0] != 0 && outpoint[1] == 0) {
		outpoint[0] = Ndif[1];
		outpoint[1] = G[0];
	}
	return;
}

static char CalFindNearNote(note_box note1[], note_box note2[],
	note_box note3[]) {
	note_box* note[3] = { note1,note2,note3 };
	char ret = 0;
	for (char i = 0; i < 3; i++) {
		if (ret != i && note[ret]->hittime > note[i]->hittime &&
			note[i]->hittime >= 0) {
			ret = i;
		}
		else if (ret != i && note[ret]->hittime == note[i]->hittime &&
			note[ret]->object == 2 && note[i]->object != 2 &&
			note[i]->hittime >= 0) {
			ret = i;
		}
	}
	return ret;
}

static void DdifCal6(note_box* note, int Etime, int noteoff, ddef_box* ddif2,
	int* ddif, int* Fdif, int voidtime, int* Ndif, int* Bdif, char dif,
	int waningLv, int outpoint[]) {
	CheckDdif(note->hittime, Etime, noteoff, ddif2, ddif, Fdif, voidtime);
	DdifCal5(Ndif, Bdif, note, dif, waningLv, outpoint);
	return;
}

static int cal_difkey(int n_time, int b_time, int n_note, int b_note,
	int bb_note, int b_dif) {
	ddif_box_t bb_ddif;
	ddif_box_t b_ddif;
	ddif_box_t ddif;
	ddif.time = n_time;
	ddif.note = n_note;
	b_ddif.time = b_time;
	b_ddif.note = b_note;
	b_ddif.dif = b_dif;
	bb_ddif.note = bb_note;
	ddif.bef = &b_ddif;
	ddif.bbef = &bb_ddif;
	/* 間隔が5ms以下で、ひとつ前と違うノーツまたは両方hitなら同時押し判定 */
	if (ddif.time - ddif.bef->time <= 5 &&
		(ddif.note != ddif.bef->note ||
			ddif.note == 1 && ddif.bef->note == 1)) {
		return cal_ddif_mix_push(ddif);
	}
	/* 単体判定 */
	return cal_ddif_solo_push(ddif);
}

static void DdifCal3(note_box note1[], note_box note2[], note_box note3[],
	char lane, short objNo[]) {
	note_box* note[3] = { note1,note2,note3 };
	for (int i = 0; i < 3; i++) {
		if (
#if 0
#define IS_ARROW_NOTE(note) ((note).object >= 3 && (note).object <= 6)
#define ARE_SAME_NOTE(note, lane1, lane2)									\
	((note)[lane1].object == (note)[lane2].object)
#define ARE_NEAR_NOTE(note, lane1, lane2)									\
	((note)[lane1].hittime + 5 >= (note)[lane2].hittime)
#define ARE_VERT_ARROW(note, lane1, lane2)									\
	IS_ARROW_NOTE((note)[lane1]) && lane1 != lane2 &&						\
	ARE_SAME_NOTE(note, lane1, lane2) && ARE_NEAR_NOTE(note, lane1, lane2)
			ARE_VERT_ARROW(*note, lane, i)
#else
			note[lane]->object >= 3 && note[lane]->object <= 6 && lane != i &&
			note[lane]->object == note[i]->object &&
			note[lane]->hittime + 5 >= note[i]->hittime
#endif
			) {
			objNo[i]++;
		}
	}
	return;
}

static void DdifCal4(short objNo[], char* lane, ddef_box* ddif2, int* Fdif,
	int limitNo, int* NdifNo, int* BdifNo, int* BBdifNo, int voidtime) {
	objNo[*lane]++;
	ddif2->datanum++;
	*lane = 0;
	ddif2->maxdif = mins(ddif2->maxdif,
		cal_nowdif_m(Fdif, limitNo, *NdifNo, voidtime));
	(*NdifNo)++;
	if (*NdifNo > limitNo) { *NdifNo = 0; }
	(*BdifNo)++;
	if (*BdifNo > limitNo) { *BdifNo = 0; }
	(*BBdifNo)++;
	if (*BBdifNo > limitNo) { *BBdifNo = 0; }
	return;
}

void CalGhostSkip(note_box note1[], note_box note2[], note_box note3[],
	short objNo[]) {
	objNo[0] += (short)CalGhostCount(&note1[objNo[0]]);
	objNo[1] += (short)CalGhostCount(&note2[objNo[1]]);
	objNo[2] += (short)CalGhostCount(&note3[objNo[2]]);
	return;
}

char DdifNoteFix(int Ndif[], int Bdif[], short objNo[], char lane) {
	switch (Ndif[0]) {
	case 1:
		if (Bdif[0] == 1 && Ndif[1] - 20 < Bdif[1]) {
			Bdif[2] *= 1;
			objNo[lane]++;
			return 1; /* continue */
		}
		break;
	case 2:
		if (DdifCatchFix(lane, &Ndif[0], &Bdif[0]) == 1) {
			objNo[lane]++;
			return 1; /* continue */
		}
		break;
	case 5:
		if (Bdif[0] == 2 || Bdif[0] == 7 || Bdif[0] == 9) {
			Bdif[0] = 5;
			return 1; /* continue */
		}
		break;
	case 6:
		if (Bdif[0] == 2 || Bdif[0] == 7 || Bdif[0] == 9) {
			Bdif[0] = 6;
			return 1; /* continue */
		}
		break;
	case 7:
		if (DdifBombFix(lane, &Ndif[0], &Bdif[0]) == 1) {
			objNo[lane]++;
			return 1; /* continue */
		}
		break;
	}
	return 0;
}

void DdifCal1(char* lane, note_box note1[], note_box note2[],
	note_box note3[], int Etime, int noteoff, ddef_box* ddif2, int* ddif,
	int* Fdif, int voidtime, int* Ndif, int* Bdif, char dif, int waningLv,
	int* outpoint) {
	note_box* note[3] = { note1,note2,note3 };
	//一番早いノーツを探して*laneに代入
	*lane = CalFindNearNote(note[0], note[1], note[2]);
	DdifCal6(note[*lane], Etime, noteoff, ddif2, ddif, Fdif, voidtime, Ndif,
		Bdif, dif, waningLv, outpoint);
	return;
}

void DdifCal2(int* NdifNo, int* BdifNo, int* BBdifNo, int Ndif[], int Bdif[],
	int BBdif[], int* lane, note_box note1[], note_box note2[],
	note_box note3[], short objNo[], ddef_box* ddif2, int* Fdif, int voidtime,
	int limitNo) {
	note_box* note[3] = { note1,note2,note3 };
	/* calculate difkey */
	if (*BdifNo != -1 && *BBdifNo != -1) {
		Ndif[2] = cal_difkey(Ndif[1], Bdif[1], Ndif[0], Bdif[0], BBdif[0],
			Bdif[2]);
	}
	// 縦置きアロースキップ
	DdifCal3(note[0], note[1], note[2], *lane, objNo);
	DdifCal4(objNo, (char*)lane, ddif2, Fdif, limitNo, NdifNo, BdifNo, BBdifNo,
		voidtime);
	return;
}
