
#include "playbox.h"
#include "define.h"

static void AutoBefTate(int *keyu, int *keyd, int *keyl, int *keyr,
	note_material umat, note_material mmat, note_material lmat,
	int uhittime, int mhittime, int lhittime, int Ntime) {
	if (uhittime - Ntime <= 40) {
		switch (umat) {
		case NOTE_UP:
			*keyu = 0;
			break;
		case NOTE_DOWN:
			*keyd = 0;
			break;
		case NOTE_LEFT:
			*keyl = 0;
			break;
		case NOTE_RIGHT:
			*keyr = 0;
			break;
		default:
			break;
		}
	}
	if (mhittime - Ntime <= 40) {
		switch (mmat) {
		case NOTE_UP:
			*keyu = 0;
			break;
		case NOTE_DOWN:
			*keyd = 0;
			break;
		case NOTE_LEFT:
			*keyl = 0;
			break;
		case NOTE_RIGHT:
			*keyr = 0;
			break;
		default:
			break;
		}
	}
	if (lhittime - Ntime <= 40) {
		switch (lmat) {
		case NOTE_UP:
			*keyu = 0;
			break;
		case NOTE_DOWN:
			*keyd = 0;
			break;
		case NOTE_LEFT:
			*keyl = 0;
			break;
		case NOTE_RIGHT:
			*keyr = 0;
			break;
		default:
			break;
		}
	}
	return;
}

static void AutoHit(int *keya, int *keyb, int *keyc,
#if SWITCH_NOTE_BOX_2 == 1
	note_box_2_t note[], short int objectN[],
#else
	struct note_box unote[], struct note_box mnote[], struct note_box lnote[],
#endif
	int Ntime) {
	int hitcount = 0;
#if SWITCH_NOTE_BOX_2 == 1
	int G[1] = { 0 };
	for (int j = 0; j < 3; j++) {
		G[0] = objectN[j];
		for (int i = 0; i < 3; i++) {
			if (note[G[0]].object == NOTE_HIT &&
				note[G[0]].hittime - Ntime <= 8) {
				hitcount++;
			}
			G[0] = note[G[0]].next;
			if (G[0] == -1) { break; }
		}
	}
#else
	for (int i = 0; i < 3; i++) {
		if (unote[i].object == 1 && unote[i].hittime - Ntime <= 8) {
			hitcount++;
		}
		if (mnote[i].object == 1 && mnote[i].hittime - Ntime <= 8) {
			hitcount++;
		}
		if (lnote[i].object == 1 && lnote[i].hittime - Ntime <= 8) {
			hitcount++;
		}
	}
#endif
	if (hitcount == 1) {
		if (*keyc == 0) {
			*keyc = 1;
			*keya = 0;
		}
		else {
			*keya = 1;
			*keyc = 0;
		}
		*keyb = 0;
	}
	else if (hitcount == 2) {
		if (*keyc == 0) {
			*keyc = 1;
			*keya = 0;
		}
		else {
			*keya = 1;
			*keyc = 0;
		}
		*keyb = 1;
	}
	else if (hitcount >= 3) {
		*keya = 1;
		*keyb = 1;
		*keyc = 1;
	}
	return;
}

static int AutoArrowLR(int *keyu, int *keyd, int *keyl, int *keyr,
	note_material umat, note_material mmat, note_material lmat,
	int uhittime, int mhittime, int lhittime, int Ntime) {
	int hitFG = 0;
	if (umat == 5 && uhittime - Ntime <= 8 ||
		mmat == 5 && mhittime - Ntime <= 8 ||
		lmat == 5 && lhittime - Ntime <= 8) {
		if (hitFG == 0) {
			*keyu = 0;
			*keyd = 0;
			*keyl = 0;
			*keyr = 0;
		}
		hitFG = 1;
		*keyl = 1;
	}
	if (umat == 6 && uhittime - Ntime <= 8 ||
		mmat == 6 && mhittime - Ntime <= 8 ||
		lmat == 6 && lhittime - Ntime <= 8) {
		if (hitFG == 0) {
			*keyu = 0;
			*keyd = 0;
			*keyl = 0;
			*keyr = 0;
		}
		hitFG = 1;
		*keyr = 1;
	}
	return hitFG;
}

static void AutoBomb(int *keyu, int *keyd,
	note_material umat, note_material mmat, note_material lmat,
	int uhittime, int mhittime, int lhittime, int Ntime){
	if (*keyd > 0) {
		if (lmat == 7 && lhittime - Ntime <= 40) {
			*keyd = 0;
		}
		if (umat == 7 && uhittime - Ntime <= 40) {
			*keyu = 0;
		}
		if (mmat == 7 && mhittime - Ntime <= 40) {
			if (umat == 2 && uhittime - Ntime <= 40 ||
				lmat == 7 && lhittime - Ntime <= 40) {
				(*keyu)++;
				*keyd = 0;
			}
			else {
				*keyu = 0;
				(*keyd)++;
			}
		}
	}
	else {
		if (umat == 7 && uhittime - Ntime <= 40) {
			*keyu = 0;
		}
		if (lmat == 7 && lhittime - Ntime <= 40) {
			*keyd = 0;
		}
		if (mmat == 7 && mhittime - Ntime <= 40) {
			if (umat == 7 && uhittime - Ntime <= 40 ||
				lmat == 2 && lhittime - Ntime <= 40) {
				*keyu = 0;
				(*keyd)++;
			}
			else {
				(*keyu)++;
				*keyd = 0;
			}
		}
	}
}

static void AutoArrowUD(int *keyu, int *keyd, int *keyl, int *keyr,
	note_material umat, note_material mmat, note_material lmat,
	int uhittime, int mhittime, int lhittime, int Ntime, int hitFG) {
	if (umat == 3 && uhittime - Ntime <= 8 ||
		mmat == 3 && mhittime - Ntime <= 8 ||
		lmat == 3 && lhittime - Ntime <= 8) {
		if (hitFG == 0) {
			*keyu = 0;
			*keyd = 0;
			*keyl = 0;
			*keyr = 0;
		}
		hitFG = 1;
		*keyu = 1;
		if (umat == 7 && uhittime - Ntime <= 40) {
			*keyd = 1;
		}
	}
	if (umat == 4 && uhittime - Ntime <= 8 ||
		mmat == 4 && mhittime - Ntime <= 8 ||
		lmat == 4 && lhittime - Ntime <= 8) {
		if (hitFG == 0) {
			*keyu = 0;
			*keyd = 0;
			*keyl = 0;
			*keyr = 0;
		}
		hitFG = 1;
		*keyd = 1;
		if (lmat == 7 && lhittime - Ntime <= 40) {
			*keyu = 1;
		}
	}
	return;
}

static void AutoCatch(int *keyu, int *keyd,
	note_material umat, note_material mmat, note_material lmat,
	int uhittime, int mhittime, int lhittime, int Ntime){
	if (*keyu > 0) {
		if (lmat == 2 && lhittime - Ntime <= 8) {
			*keyu = 0;
			(*keyd)++;
		}
		if (mmat == 2 && mhittime - Ntime <= 8) {
			*keyu = 0;
			*keyd = 0;
		}
		if (umat == 2 && uhittime - Ntime <= 8) {
			(*keyu)++;
			*keyd = 0;
		}
	}
	else {
		if (umat == 2 && uhittime - Ntime <= 8) {
			(*keyu)++;
			*keyd = 0;
		}
		if (mmat == 2 && mhittime - Ntime <= 8) {
			*keyu = 0;
			*keyd = 0;
		}
		if (lmat == 2 && lhittime - Ntime <= 8) {
			*keyu = 0;
			(*keyd)++;
		}
	}
	return;
}

static void AutoReleaseKey(int *keya, int *keyb, int *keyc,
	int *keyu, int *keyd, int *keyl, int *keyr,
	note_material umat, note_material mmat, note_material lmat,
	int uhittime, int mhittime, int lhittime, int Ntime) {
	/* �q�b�g�{�^������ */
	if (*keya > 10) { *keya = 0; }
	if (*keyb > 10) { *keyb = 0; }
	if (*keyc > 10) { *keyc = 0; }
	/* ���E�{�^������ */
	if (*keyl > 10) { *keyl = 0; }
	if (*keyr > 10) { *keyr = 0; }
	/* �㉺�{�^������ */
	if ((umat != 2 && umat != 7 || uhittime - Ntime >= 1000) &&
		(mmat != 2 && mmat != 7 || mhittime - Ntime >= 1000) &&
		(lmat != 2 && lmat != 7 || lhittime - Ntime >= 1000)) {
		if (*keyu > 10) { *keyu = 0; }
		if (*keyd > 10) { *keyd = 0; }
	}
}

void AutoAution(int *keya, int *keyb, int *keyc, int *keyu, int *keyd, int *keyl, int *keyr,
#if SWITCH_NOTE_BOX_2 == 1
	note_box_2_t note[], short int objectNG[],
#else
	struct note_box *unote, struct note_box *mnote, struct note_box *lnote,
#endif
	int Ntime) {
	int hitFG = 0;
	if (*keya > 0) { (*keya)++; }
	if (*keyb > 0) { (*keyb)++; }
	if (*keyc > 0) { (*keyc)++; }
	if (*keyu > 0) { (*keyu)++; }
	if (*keyd > 0) { (*keyd)++; }
	if (*keyl > 0) { (*keyl)++; }
	if (*keyr > 0) { (*keyr)++; }
	//�c�A�O�{�^������
	AutoBefTate(keyu, keyd, keyl, keyr,
#if SWITCH_NOTE_BOX_2 == 1
		note[objectNG[0]].object, note[objectNG[1]].object,
		note[objectNG[2]].object, note[objectNG[0]].hittime,
		note[objectNG[1]].hittime, note[objectNG[2]].hittime,
#else
		unote->object, mnote->object, lnote->object,
		unote->hittime, mnote->hittime, lnote->hittime,
#endif
		Ntime);
	//�q�b�g�m�[�c����
	AutoHit(keya, keyb, keyc,
#if SWITCH_NOTE_BOX_2 == 1
		note, objectNG,
#else
		unote, mnote, lnote,
#endif
		Ntime);
	//���E�A���[�m�[�c����
	hitFG = AutoArrowLR(keyu, keyd, keyl, keyr,
#if SWITCH_NOTE_BOX_2 == 1
		note[objectNG[0]].object, note[objectNG[1]].object,
		note[objectNG[2]].object, note[objectNG[0]].hittime,
		note[objectNG[1]].hittime, note[objectNG[2]].hittime,
#else
		unote->object, mnote->object, lnote->object,
		unote->hittime, mnote->hittime, lnote->hittime,
#endif
		Ntime);
	//�{���m�[�c����
	AutoBomb(keyu, keyd,
#if SWITCH_NOTE_BOX_2 == 1
		note[objectNG[0]].object, note[objectNG[1]].object,
		note[objectNG[2]].object, note[objectNG[0]].hittime,
		note[objectNG[1]].hittime, note[objectNG[2]].hittime,
#else
		unote->object, mnote->object, lnote->object,
		unote->hittime, mnote->hittime, lnote->hittime,
#endif
		Ntime);
	//�㉺�A���[�m�[�c����
	AutoArrowUD(keyu, keyd, keyl, keyr,
#if SWITCH_NOTE_BOX_2 == 1
		note[objectNG[0]].object, note[objectNG[1]].object,
		note[objectNG[2]].object, note[objectNG[0]].hittime,
		note[objectNG[1]].hittime, note[objectNG[2]].hittime,
#else
		unote->object, mnote->object, lnote->object,
		unote->hittime, mnote->hittime, lnote->hittime,
#endif
		Ntime, hitFG);
	//�L���b�`�m�[�c����
	AutoCatch(keyu, keyd,
#if SWITCH_NOTE_BOX_2 == 1
		note[objectNG[0]].object, note[objectNG[1]].object,
		note[objectNG[2]].object, note[objectNG[0]].hittime,
		note[objectNG[1]].hittime, note[objectNG[2]].hittime,
#else
		unote->object, mnote->object, lnote->object,
		unote->hittime, mnote->hittime, lnote->hittime,
#endif
		Ntime);
	/* �{�^������ */
	AutoReleaseKey(keya, keyb, keyc, keyu, keyd, keyl, keyr,
#if SWITCH_NOTE_BOX_2 == 1
		note[objectNG[0]].object, note[objectNG[1]].object,
		note[objectNG[2]].object, note[objectNG[0]].hittime,
		note[objectNG[1]].hittime, note[objectNG[2]].hittime,
#else
		unote->object, mnote->object, lnote->object,
		unote->hittime, mnote->hittime, lnote->hittime,
#endif
		Ntime);
	return;
}
