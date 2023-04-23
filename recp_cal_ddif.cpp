
#include "sancur.h"
#include "playbox.h"
#include "recp_cal_ddif.h"

/* ŠÔŠu‚ª5msˆÈ‰º‚Ìƒm[ƒg‚Í“¯Žž‰Ÿ‚µˆµ‚¢Bhit‚Í1.3”{Aarrow‚Í1.6”{ */
#define MULTI_ARROW_MLP(x) ((x) * 16 / 10)
#define MULTI_NOTE_MLP(x) ((x) * 13 / 10)

 /* 2ŒÂ‘O‚Æˆá‚¤ƒL[‚ÌŽž‚Í1.2”{(‘SƒL[‘ÎÛ) */
#define DIF_BBEF_MLP(x) ((x) * 12 / 10)

 /* arrowƒL[‚Í1.2”{ */
#define ARROW_MLP(x) ((x) * 12 / 10)

typedef struct ddif_box_s ddif_box_t;
struct ddif_box_s {
	int time = 0;
	int note = 0;
	int dif = 0;
	ddif_box_t *bef = NULL;
	ddif_box_t *bbef = NULL;
};

static int cal_ddif_solo_push(ddif_box_t ddif) {
	int gap = ddif.time - ddif.bef->time;
	int ret = 3000000 / gap;
	/* arrow‚Ìc˜A */
	if (ddif.note == ddif.bef->note && 3 <= ddif.note && ddif.note <= 6) {
		if (gap <= 75) {
			ret *= 2;
		}
		else if (75 < gap && gap < 150) {
			ret = lins(75, 200 * ret, 150, 100 * ret, gap) / 100;
		}
	}
	/* notƒgƒŠƒ‹”»’è */
	if (ddif.note != ddif.bbef->note) {
		ret = DIF_BBEF_MLP(ret);
	}
	/* arrow•â³ */
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

int cal_difkey(int n_time, int b_time, int n_note, int b_note, int bb_note, int
	b_dif) {
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
	/* ŠÔŠu‚ª5msˆÈ‰º‚ÅA‚Ð‚Æ‚Â‘O‚Æˆá‚¤ƒm[ƒc‚Ü‚½‚Í—¼•ûhit‚È‚ç“¯Žž‰Ÿ‚µ”»’è */
	if (ddif.time - ddif.bef->time <= 5 &&
		(ddif.note != ddif.bef->note ||
		ddif.note == 1 && ddif.bef->note == 1)) {
		return cal_ddif_mix_push(ddif);
	}
	/* ’P‘Ì”»’è */
	return cal_ddif_solo_push(ddif);
}

static int CalGhostCount(note_box note[]) {
	int num = 0;
	while (note[num].object == 8 && note[num].hittime >= 0) { num++; }
	return num;
}

void CalGhostSkip(note_box note1[], note_box note2[], note_box note3[],
	short objNo[]) {
	objNo[0] += CalGhostCount(&note1[objNo[0]]);
	objNo[1] += CalGhostCount(&note2[objNo[1]]);
	objNo[2] += CalGhostCount(&note3[objNo[2]]);
	return;
}

char CalFindNearNote(note_box note1[], note_box note2[], note_box note3[]) {
	note_box* note[3] = { note1,note2,note3 };
	char ret = 0;
	for (int i = 0; i < 3; i++) {
		if (ret != i && note[ret]->hittime >
			note[i]->hittime &&
			note[i]->hittime >= 0) {
			ret = i;
		}
		else if (ret != i &&
			note[ret]->hittime ==
			note[i]->hittime &&
			note[ret]->object == 2 &&
			note[i]->object != 2 &&
			note[i]->hittime >= 0) {
			ret = i;
		}
	}
	return ret;
}

int cal_ddif(int num, int const* difkey, int Etime, int noteoff, int difsec,
	int voidtime) {
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


