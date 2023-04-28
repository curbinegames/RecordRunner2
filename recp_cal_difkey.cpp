
#include "DxLib.h"
#include "sancur.h"
#include "recp_cal_ddif.h"
#include "playbox.h"

#define DDIF_NUM 25

#define DIFKEY_STEP_NUM(num) ((num) = ((num) + 1) % difkeySys.limit)

typedef struct difkey2_s difkey2_t;

struct difkey2_s {
	int key = 0; // [0]
	int time = 0; // [1]
	int dif = 0; // [2]
	difkey2_t* bef = NULL;
	difkey2_t* bbef = NULL;
};

static difkey2_t difkey[50];
static char difkeyNo = 0;

static struct {
	char limit = 50; // s[0]
	char No = 0; // s[1]
	int maxdif = 0; // s[4]
	char datanum = 0; // s[5]
	int lastdif = 0; // s[6]
	int avoid = 10000; // s[7]
} difkeySys;

static int CalNowdif() {
	int ret = 0;
	int count = 0;
	for (int i = 0; i <= difkeySys.datanum; i++) {
		if (difkey[i].time > difkey[difkeySys.No].time - difkeySys.avoid) {
			ret += difkey[i].dif;
			count++;
		}
	}
	if (count == 0) { return 0; }
	else { return ret * 50 / count; }
}

void DifkeyCalInsertNote(note_box const* const note, char lane) {
	difkey2_t* p_difkey = &difkey[difkeySys.No];
	difkeySys.datanum = maxs(difkeySys.datanum + 1, difkeySys.limit);
	p_difkey->key = note->object;
	p_difkey->time = note->hittime;
	p_difkey->dif = 0;
	if (difkeySys.datanum < 3) {
		p_difkey->bef = NULL;
		p_difkey->bbef = NULL;
		DIFKEY_STEP_NUM(difkeySys.No);
		return;
	}
	p_difkey->bef = &difkey[(difkeySys.No + 50 - 1) % 50];
	p_difkey->bbef = &difkey[(difkeySys.No + 50 - 2) % 50];
	switch (note->object) {
	case 1: //hitƒm[ƒc•âŠÔ
		if (p_difkey->bef->key == 1 &&
			p_difkey->time - 20 < p_difkey->bef->time) {
			p_difkey->bef->dif *= 1;
			return;
		}
		break;
	case 2: //catchƒm[ƒc•âŠÔ
		if (lane != 1) { p_difkey->key = lane / 2 + 3; }
		if (p_difkey->key == p_difkey->bef->key) { return; }
		else if (p_difkey->key == 3 &&
			(p_difkey->bef->key == 8 || p_difkey->bef->key == 9)) {
			p_difkey->bef->key = 3;
			return;
		}
		else if (p_difkey->key == 2 &&
			(p_difkey->bef->key == 1 || p_difkey->bef->key == 5 ||
			p_difkey->bef->key == 6)) {
			return;
		}
		else if (p_difkey->key == 2 &&
			(p_difkey->bef->key == 7 || p_difkey->bef->key == 9)) {
			p_difkey->bef->key = 2;
			return;
		}
		else if (p_difkey->key == 4 &&
			(p_difkey->bef->key == 7 || p_difkey->bef->key == 8)) {
			p_difkey->bef->key = 4;
			return;
		}
		break;
	case 5: //leftƒm[ƒc•âŠÔ
		if (p_difkey->bef->key == 2 || p_difkey->bef->key == 7 ||
			p_difkey->bef->key == 9) {
			p_difkey->bef->key = 5;
			return;
		}
		break;
	case 6: //rightƒm[ƒc•âŠÔ
		if (p_difkey->bef->key == 2 || p_difkey->bef->key == 7 ||
			p_difkey->bef->key == 9) {
			p_difkey->bef->key = 6;
			return;
		}
		break;
	case 7: //bombƒm[ƒc•âŠÔ
		if (lane != 0) { p_difkey->key = lane + 7; }
		if (p_difkey->key == p_difkey->bef->key) { return; }
		else if ((p_difkey->key == 7 && p_difkey->bef->key == 8) ||
			(p_difkey->key == 8 && p_difkey->bef->key == 7)) {
			p_difkey->bef->key = 4;
			return;
		}
		else if ((p_difkey->key == 7 && p_difkey->bef->key == 9) ||
			(p_difkey->key == 9 && p_difkey->bef->key == 7)) {
			p_difkey->bef->key = 2;
			return;
		}
		else if ((p_difkey->key == 8 && p_difkey->bef->key == 9) ||
			(p_difkey->key == 9 && p_difkey->bef->key == 8)) {
			p_difkey->bef->key = 3;
			return;
		}
		else if (p_difkey->key == 7 && (p_difkey->bef->key != 3)) { return; }
		else if (p_difkey->key == 8 &&
			(p_difkey->bef->key == 3 || p_difkey->bef->key == 4)) {
			return;
		}
		else if (p_difkey->key == 9 && p_difkey->bef->key != 4) { return; }
		break;
	}
	p_difkey->dif = cal_difkey(p_difkey->time, p_difkey->bef->time,
		p_difkey->key, p_difkey->bef->key, p_difkey->bbef->key,
		p_difkey->bef->dif);
	difkeySys.maxdif = mins(difkeySys.maxdif, CalNowdif());
	DIFKEY_STEP_NUM(difkeySys.No);
	return;
}

void DifkeyCalInit(int notes, int songlength) {
	difkeySys.limit = (char)maxs(notes, 50);
	difkeySys.avoid = mins(songlength * 2 / DDIF_NUM, 10000);
	return;
}
