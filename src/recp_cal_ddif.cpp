
#include <sancur.h>
#include <recp_cal_ddif.h>

/* 間隔が5ms以下のノートは同時押し扱い。hitは1.3倍、arrowは1.6倍 */
#define MULTI_ARROW_MLP(x) ((x) * 16 / 10)
#define MULTI_NOTE_MLP(x) ((x) * 13 / 10)

 /* 2個前と違うキーの時は1.2倍(全キー対象) */
#define DIF_BBEF_MLP(x) ((x) * 12 / 10)

 /* arrowキーは1.2倍 */
#define ARROW_MLP(x) ((x) * 12 / 10)

int cal_ddif_solo_push(ddif_box_t ddif) {
	int gap = mins(ddif.time - ddif.bef->time, 1);
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
	/* 間隔が5ms以下で、ひとつ前と違うノーツまたは両方hitなら同時押し判定 */
	if (ddif.time - ddif.bef->time <= 5 &&
		(ddif.note != ddif.bef->note ||
		ddif.note == 1 && ddif.bef->note == 1)) {
		return cal_ddif_mix_push(ddif);
	}
	/* 単体判定 */
	return cal_ddif_solo_push(ddif);
}
