
#include "sancur.h"
#include "recp_cal_ddif.h"

/* �Ԋu��5ms�ȉ��̃m�[�g�͓������������Bhit��1.3�{�Aarrow��1.6�{ */
#define MULTI_ARROW_MLP(x) ((x) * 16 / 10)
#define MULTI_NOTE_MLP(x) ((x) * 13 / 10)

 /* 2�O�ƈႤ�L�[�̎���1.2�{(�S�L�[�Ώ�) */
#define DIF_BBEF_MLP(x) ((x) * 12 / 10)

 /* arrow�L�[��1.2�{ */
#define ARROW_MLP(x) ((x) * 12 / 10)

typedef struct ddif_box_s ddif_box_t;
struct ddif_box_s {
	int time = 0;
	int note = 0;
	int dif = 0;
	ddif_box_t *bef = NULL;
	ddif_box_t *bbef = NULL;
};

int cal_ddif_solo_push(ddif_box_t ddif) {
	int gap = ddif.time - ddif.bef->time;
	int ret = 3000000 / gap;
	/* arrow�̏c�A */
	if (ddif.note == ddif.bef->note && 3 <= ddif.note && ddif.note <= 6) {
		if (gap <= 75) {
			ret *= 2;
		}
		else if (75 < gap && gap < 150) {
			ret = lins(75, 200 * ret, 150, 100 * ret, gap) / 100;
		}
	}
	/* not�g�������� */
	if (ddif.note != ddif.bbef->note) {
		ret = DIF_BBEF_MLP(ret);
	}
	/* arrow�␳ */
	if (ddif.note >= 3 && ddif.note <= 6) {
		ret = ARROW_MLP(ret);
	}
	return ret;
}

int cal_ddif_mix_push(ddif_box_t ddif) {
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
	/* �Ԋu��5ms�ȉ��ŁA�ЂƂO�ƈႤ�m�[�c�܂��͗���hit�Ȃ瓯���������� */
	if (ddif.time - ddif.bef->time <= 5 &&
		(ddif.note != ddif.bef->note ||
		ddif.note == 1 && ddif.bef->note == 1)) {
		return cal_ddif_mix_push(ddif);
	}
	/* �P�̔��� */
	return cal_ddif_solo_push(ddif);
}
