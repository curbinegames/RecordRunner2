
#include "playbox.h"
#include "define.h"

#define ADIF_NUM 50

typedef struct autodif_s autodif_t;

struct autodif_s {
	int time = -1;
	int key = 0;
	int def = 0;
	autodif_t *next = NULL;
	autodif_t *prev = NULL;
};

static autodif_t adif[ADIF_NUM];
static int nowNo = 0;

void InitAdif() {
	for (int i = 0; i < ADIF_NUM; i++) {
		adif[i].time = -1;
		adif[i].key = 0;
		adif[i].def = 0;
		adif[i].next = &adif[(i + 1) % ADIF_NUM];
		adif[i].prev = &adif[(i + ADIF_NUM - 1) % ADIF_NUM];
	}
}

int GetAdif() {
	int ret = 0;
	double mod = 1;
	for (int i = 0; i < 50; i++) {
		ret += adif[(nowNo + i) % 50].def * mod;
		mod *= 0.91;
	}
	return ret;
}

static void AutoBefTate(rec_play_key_hold_t *key, note_box_2_t note[],
	short int objectNG[], int Ntime)
{
	for (int i = 0; i < 3; i++) {
		if (note[objectNG[i]].hittime - Ntime <= 40) {
			switch (note[objectNG[i]].object) {
			case NOTE_UP:
				key->up = 0;
				break;
			case NOTE_DOWN:
				key->down = 0;
				break;
			case NOTE_LEFT:
				key->left = 0;
				break;
			case NOTE_RIGHT:
				key->right = 0;
				break;
			default:
				break;
			}
		}
	}
	return;
}

static void AutoHit(rec_play_key_hold_t *key, note_box_2_t note[],
	short int objectNG[], int Ntime)
{
	int hitcount = 0;
	int G[1] = { 0 };
	for (int j = 0; j < 3; j++) {
		G[0] = objectNG[j];
		for (int i = 0; i < 3; i++) {
			if (note[G[0]].object == NOTE_HIT &&
				note[G[0]].hittime - Ntime <= 8) {
				hitcount++;
			}
			G[0] = note[G[0]].next;
			if (G[0] == -1) { break; }
		}
	}
	if (hitcount == 1) {
		if (key->c == 0) {
			key->c = 1;
			key->z = 0;
		}
		else {
			key->z = 1;
			key->c = 0;
		}
		key->x = 0;
	}
	else if (hitcount == 2) {
		if (key->c == 0) {
			key->c = 1;
			key->z = 0;
		}
		else {
			key->z = 1;
			key->c = 0;
		}
		key->x = 1;
	}
	else if (hitcount >= 3) {
		key->z = 1;
		key->x = 1;
		key->c = 1;
	}
	return;
}

static int AutoArrowLR(rec_play_key_hold_t *key, note_box_2_t note[],
	short int objectNG[], int Ntime)
{
	int hitFG = 0;
	if (note[objectNG[0]].object == 5 && note[objectNG[0]].hittime - Ntime <= 8 ||
		note[objectNG[1]].object == 5 && note[objectNG[1]].hittime - Ntime <= 8 ||
		note[objectNG[2]].object == 5 && note[objectNG[2]].hittime - Ntime <= 8) {
		if (hitFG == 0) {
			key->up = 0;
			key->down = 0;
			key->left = 0;
			key->right = 0;
		}
		hitFG = 1;
		key->left = 1;
	}
	if (note[objectNG[0]].object == 6 && note[objectNG[0]].hittime - Ntime <= 8 ||
		note[objectNG[1]].object == 6 && note[objectNG[1]].hittime - Ntime <= 8 ||
		note[objectNG[2]].object == 6 && note[objectNG[2]].hittime - Ntime <= 8) {
		if (hitFG == 0) {
			key->up = 0;
			key->down = 0;
			key->left = 0;
			key->right = 0;
		}
		hitFG = 1;
		key->right = 1;
	}
	return hitFG;
}

static void AutoBomb(rec_play_key_hold_t *key, note_box_2_t note[],
	short int objectNG[], int Ntime)
{
	if (key->down > 0) {
		if (note[objectNG[2]].object == 7 && note[objectNG[2]].hittime - Ntime <= 40) {
			key->down = 0;
		}
		if (note[objectNG[0]].object == 7 && note[objectNG[0]].hittime - Ntime <= 40) {
			key->up = 0;
		}
		if (note[objectNG[1]].object == 7 && note[objectNG[1]].hittime - Ntime <= 40) {
			if (note[objectNG[0]].object == 2 && note[objectNG[0]].hittime - Ntime <= 40 ||
				note[objectNG[2]].object == 7 && note[objectNG[2]].hittime - Ntime <= 40) {
				(key->up)++;
				key->down = 0;
			}
			else {
				key->up = 0;
				(key->down)++;
			}
		}
	}
	else {
		if (note[objectNG[0]].object == 7 && note[objectNG[0]].hittime - Ntime <= 40) {
			key->up = 0;
		}
		if (note[objectNG[2]].object == 7 && note[objectNG[2]].hittime - Ntime <= 40) {
			key->down = 0;
		}
		if (note[objectNG[1]].object == 7 && note[objectNG[1]].hittime - Ntime <= 40) {
			if (note[objectNG[0]].object == 7 && note[objectNG[0]].hittime - Ntime <= 40 ||
				note[objectNG[2]].object == 2 && note[objectNG[2]].hittime - Ntime <= 40) {
				key->up = 0;
				(key->down)++;
			}
			else {
				(key->up)++;
				key->down = 0;
			}
		}
	}
}

static void AutoArrowUD(rec_play_key_hold_t *key, note_box_2_t note[],
	short int objectNG[], int Ntime, int hitFG)
{
	if (note[objectNG[0]].object == 3 && note[objectNG[0]].hittime - Ntime <= 8 ||
		note[objectNG[1]].object == 3 && note[objectNG[1]].hittime - Ntime <= 8 ||
		note[objectNG[2]].object == 3 && note[objectNG[2]].hittime - Ntime <= 8) {
		if (hitFG == 0) {
			key->up = 0;
			key->down = 0;
			key->left = 0;
			key->right = 0;
		}
		hitFG = 1;
		key->up = 1;
		if (note[objectNG[0]].object == 7 && note[objectNG[0]].hittime - Ntime <= 40) {
			key->down = 1;
		}
	}
	if (note[objectNG[0]].object == 4 && note[objectNG[0]].hittime - Ntime <= 8 ||
		note[objectNG[1]].object == 4 && note[objectNG[1]].hittime - Ntime <= 8 ||
		note[objectNG[2]].object == 4 && note[objectNG[2]].hittime - Ntime <= 8) {
		if (hitFG == 0) {
			key->up = 0;
			key->down = 0;
			key->left = 0;
			key->right = 0;
		}
		hitFG = 1;
		key->down = 1;
		if (note[objectNG[2]].object == 7 && note[objectNG[2]].hittime - Ntime <= 40) {
			key->up = 1;
		}
	}
	return;
}

static void AutoCatch(rec_play_key_hold_t *key, note_box_2_t note[],
	short int objectNG[], int Ntime)
{
	if (key->up > 0) {
		if (note[objectNG[2]].object == 2 && note[objectNG[2]].hittime - Ntime <= 8) {
			key->up = 0;
			(key->down)++;
		}
		if (note[objectNG[1]].object == 2 && note[objectNG[1]].hittime - Ntime <= 8) {
			key->up = 0;
			key->down = 0;
		}
		if (note[objectNG[0]].object == 2 && note[objectNG[0]].hittime - Ntime <= 8) {
			(key->up)++;
			key->down = 0;
		}
	}
	else {
		if (note[objectNG[0]].object == 2 && note[objectNG[0]].hittime - Ntime <= 8) {
			(key->up)++;
			key->down = 0;
		}
		if (note[objectNG[1]].object == 2 && note[objectNG[1]].hittime - Ntime <= 8) {
			key->up = 0;
			key->down = 0;
		}
		if (note[objectNG[2]].object == 2 && note[objectNG[2]].hittime - Ntime <= 8) {
			key->up = 0;
			(key->down)++;
		}
	}
	return;
}

static void AutoReleaseKey(rec_play_key_hold_t *key, note_box_2_t note[],
	short int objectNG[], int Ntime)
{
	/* ヒットボタン離し */
	if (key->z > 10) { key->z = 0; }
	if (key->x > 10) { key->x = 0; }
	if (key->c > 10) { key->c = 0; }
	/* 左右ボタン離し */
	if (key->left > 10) { key->left = 0; }
	if (key->right > 10) { key->right = 0; }
	/* 上下ボタン離し */
	if ((note[objectNG[0]].object != 2 && note[objectNG[0]].object != 7 || note[objectNG[0]].hittime - Ntime >= 1000) &&
		(note[objectNG[1]].object != 2 && note[objectNG[1]].object != 7 || note[objectNG[1]].hittime - Ntime >= 1000) &&
		(note[objectNG[2]].object != 2 && note[objectNG[2]].object != 7 || note[objectNG[2]].hittime - Ntime >= 1000)) {
		if (key->up > 10) { key->up = 0; }
		if (key->down > 10) { key->down = 0; }
	}
}

static void CalAdif(rec_play_key_hold_t *key, int Ntime) {
	if (key->z == 1) {
		adif[nowNo].time = Ntime;
		adif[nowNo].key = 1;
		if (adif[nowNo].prev->time + 10 <= Ntime) {
			adif[nowNo].def = 3000000 / (Ntime - adif[nowNo].prev->time);
		}
		else {
			adif[nowNo].def = adif[nowNo].prev->def * 1.2;
		}
		nowNo = (nowNo + 1) % 50;
	}
	if (key->x == 1) {
		adif[nowNo].time = Ntime;
		adif[nowNo].key = 1;
		if (adif[nowNo].prev->time + 10 <= Ntime) {
			adif[nowNo].def = 3000000 / (Ntime - adif[nowNo].prev->time);
		}
		else {
			adif[nowNo].def = adif[nowNo].prev->def * 1.2;
		}
		nowNo = (nowNo + 1) % 50;
	}
	if (key->c == 1) {
		adif[nowNo].time = Ntime;
		adif[nowNo].key = 1;
		if (adif[nowNo].prev->time + 10 <= Ntime) {
			adif[nowNo].def = 3000000 / (Ntime - adif[nowNo].prev->time);
		}
		else {
			adif[nowNo].def = adif[nowNo].prev->def * 1.2;
		}
		nowNo = (nowNo + 1) % 50;
	}
	if (key->up == 1) {
		adif[nowNo].time = Ntime;
		adif[nowNo].key = 1;
		if (adif[nowNo].prev->time + 10 <= Ntime) {
			adif[nowNo].def = 3000000 / (Ntime - adif[nowNo].prev->time);
		}
		else {
			adif[nowNo].def = adif[nowNo].prev->def * 1.2;
		}
		nowNo = (nowNo + 1) % 50;
	}
	if (key->down == 1) {
		adif[nowNo].time = Ntime;
		adif[nowNo].key = 1;
		if (adif[nowNo].prev->time + 10 <= Ntime) {
			adif[nowNo].def = 3000000 / (Ntime - adif[nowNo].prev->time);
		}
		else {
			adif[nowNo].def = adif[nowNo].prev->def * 1.2;
		}
		nowNo = (nowNo + 1) % 50;
	}
	if (key->left == 1) {
		adif[nowNo].time = Ntime;
		adif[nowNo].key = 1;
		if (adif[nowNo].prev->time + 10 <= Ntime) {
			adif[nowNo].def = 3000000 / (Ntime - adif[nowNo].prev->time);
		}
		else {
			adif[nowNo].def = adif[nowNo].prev->def * 1.2;
		}
		nowNo = (nowNo + 1) % 50;
	}
	if (key->right == 1) {
		adif[nowNo].time = Ntime;
		adif[nowNo].key = 1;
		if (adif[nowNo].prev->time + 10 <= Ntime) {
			adif[nowNo].def = 3000000 / (Ntime - adif[nowNo].prev->time);
		}
		else {
			adif[nowNo].def = adif[nowNo].prev->def * 1.2;
		}
		nowNo = (nowNo + 1) % 50;
	}
	return;
}

void AutoAution(rec_play_key_hold_t *key, note_box_2_t note[],
	short int objectNG[], int Ntime)
{
	int hitFG = 0;
	if (key->z > 0) { (key->z)++; }
	if (key->x > 0) { (key->x)++; }
	if (key->c > 0) { (key->c)++; }
	if (key->up > 0) { (key->up)++; }
	if (key->down > 0) { (key->down)++; }
	if (key->left > 0) { (key->left)++; }
	if (key->right > 0) { (key->right)++; }
	//縦連前ボタン離し
	AutoBefTate(key, note, objectNG, Ntime);
	//ヒットノーツ処理
	AutoHit(key, note, objectNG, Ntime);
	//左右アローノーツ処理
	hitFG = AutoArrowLR(key, note, objectNG, Ntime);
	//ボムノーツ処理
	AutoBomb(key, note, objectNG, Ntime);
	//上下アローノーツ処理
	AutoArrowUD(key, note, objectNG, Ntime, hitFG);
	//キャッチノーツ処理
	AutoCatch(key, note, objectNG, Ntime);
	/* ボタン離し */
	AutoReleaseKey(key, note, objectNG, Ntime);
	/* adif計算 */
	CalAdif(key, Ntime);
	return;
}
