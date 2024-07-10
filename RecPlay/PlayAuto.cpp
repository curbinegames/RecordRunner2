
#include "../define.h"
#include "playbox.h"

#define ADIF_NUM 50

typedef struct autodif_s autodif_t;

#define IS_ACTION_TYPE_ARROW(type) ((type) == ACTION_TYPE_ARROW_UP || (type) == ACTION_TYPE_ARROW_DOWN || (type) == ACTION_TYPE_ARROW_LEFT || (type) == ACTION_TYPE_ARROW_RIGHT)

typedef enum action_type_e {
	ACTION_TYPE_NONE,
	ACTION_TYPE_HIT,
	ACTION_TYPE_ARROW_UP,
	ACTION_TYPE_ARROW_DOWN,
	ACTION_TYPE_ARROW_LEFT,
	ACTION_TYPE_ARROW_RIGHT,
	ACTION_TYPE_CATCH_HIGH,
	ACTION_TYPE_CATCH_MID,
	ACTION_TYPE_CATCH_LOW,
	ACTION_TYPE_BOMB_HIGH,
	ACTION_TYPE_BOMB_MID,
	ACTION_TYPE_BOMB_LOW,
} action_type_t;

struct autodif_s {
	int time = -1;
	action_type_t atype[3] = { ACTION_TYPE_NONE,ACTION_TYPE_NONE,ACTION_TYPE_NONE };
	int def = 0;
	autodif_t *next = NULL;
	autodif_t *prev = NULL;
};

static autodif_t adif[ADIF_NUM];
static int nowNo = 0;
static action_type_t buf_type[3] = { ACTION_TYPE_NONE,ACTION_TYPE_NONE,ACTION_TYPE_NONE };

static void Set_Type(action_type_t set) {
	int FG = 0;
	
	if (set == ACTION_TYPE_NONE) {
		return;
	}

	for (int i = 0; i < 3 && FG == 0; i++) {
		switch (buf_type[i]) {
		case ACTION_TYPE_NONE:
			buf_type[i] = set;
			FG = 1;
			break;
		case ACTION_TYPE_CATCH_HIGH:
			if (set == ACTION_TYPE_ARROW_UP) {
				buf_type[i] = ACTION_TYPE_ARROW_UP;
				FG = 1;
			}
			break;
		case ACTION_TYPE_CATCH_MID:
			if (set == ACTION_TYPE_HIT || set == ACTION_TYPE_ARROW_LEFT || set == ACTION_TYPE_ARROW_RIGHT) {
				buf_type[i] = set;
				FG = 1;
			}
			break;
		case ACTION_TYPE_CATCH_LOW:
			if (set == ACTION_TYPE_ARROW_DOWN) {
				buf_type[i] = ACTION_TYPE_ARROW_DOWN;
				FG = 1;
			}
			break;
		case ACTION_TYPE_BOMB_HIGH:
			if (set == ACTION_TYPE_ARROW_UP) {
				buf_type[i] = ACTION_TYPE_ARROW_UP;
				FG = 1;
			}
			break;
		case ACTION_TYPE_BOMB_MID:
			if (set == ACTION_TYPE_HIT || set == ACTION_TYPE_ARROW_LEFT || set == ACTION_TYPE_ARROW_RIGHT) {
				buf_type[i] = set;
				FG = 1;
			}
			break;
		case ACTION_TYPE_BOMB_LOW:
			if (set == ACTION_TYPE_ARROW_DOWN) {
				buf_type[i] = ACTION_TYPE_ARROW_DOWN;
				FG = 1;
			}
			break;
		}
	}
	return;
}

static int Cal_Type_Mlp() {
	action_type_t *bbef = adif[nowNo].atype;
	action_type_t *bef = adif[nowNo].prev->atype;
	action_type_t *now = adif[nowNo].prev->prev->atype;
	
	/* HIT3同時押し */
	if (now[0] == ACTION_TYPE_HIT && now[1] == ACTION_TYPE_HIT && now[2] == ACTION_TYPE_HIT) {
		return 170;
	}
	/* HIT2ARROW同時押し */
	if (now[0] == ACTION_TYPE_HIT && now[1] == ACTION_TYPE_HIT && IS_ACTION_TYPE_ARROW(now[2]) ||
		now[0] == ACTION_TYPE_HIT && IS_ACTION_TYPE_ARROW(now[1]) && now[2] == ACTION_TYPE_HIT ||
		IS_ACTION_TYPE_ARROW(now[0]) && now[1] == ACTION_TYPE_HIT && now[2] == ACTION_TYPE_HIT) {
		return 210;
	}
	/* HIT1ARROW2同時押し */
	if (now[0] == ACTION_TYPE_HIT && IS_ACTION_TYPE_ARROW(now[1]) && IS_ACTION_TYPE_ARROW(now[2]) ||
		IS_ACTION_TYPE_ARROW(now[0]) && now[1] == ACTION_TYPE_HIT && IS_ACTION_TYPE_ARROW(now[2]) ||
		IS_ACTION_TYPE_ARROW(now[0]) && IS_ACTION_TYPE_ARROW(now[1]) && now[2] == ACTION_TYPE_HIT)
	{
		return 260;
	}

	/* HIT2同時押し */
	if (now[0] == ACTION_TYPE_HIT && now[1] == ACTION_TYPE_HIT || 
		now[0] == ACTION_TYPE_HIT && now[2] == ACTION_TYPE_HIT || 
		now[1] == ACTION_TYPE_HIT && now[2] == ACTION_TYPE_HIT) {
		return 130;
	}
	/* HIT1ARROW1同時押し */
	if (now[0] == ACTION_TYPE_HIT && IS_ACTION_TYPE_ARROW(now[1]) ||
		IS_ACTION_TYPE_ARROW(now[0]) && now[1] == ACTION_TYPE_HIT ||
		now[0] == ACTION_TYPE_HIT && IS_ACTION_TYPE_ARROW(now[2]) ||
		IS_ACTION_TYPE_ARROW(now[0]) && now[2] == ACTION_TYPE_HIT ||
		now[1] == ACTION_TYPE_HIT && IS_ACTION_TYPE_ARROW(now[2]) ||
		IS_ACTION_TYPE_ARROW(now[1]) && now[2] == ACTION_TYPE_HIT) {
		return 160;
	}
	/* ARROW2同時押し */
	if (IS_ACTION_TYPE_ARROW(now[0]) && IS_ACTION_TYPE_ARROW(now[1]) ||
		IS_ACTION_TYPE_ARROW(now[0]) && IS_ACTION_TYPE_ARROW(now[2]) ||
		IS_ACTION_TYPE_ARROW(now[1]) && IS_ACTION_TYPE_ARROW(now[2])) {
		return 200;
	}
	return 100;
}

void InitAdif() {
	for (int i = 0; i < ADIF_NUM; i++) {
		adif[i].time = -1;
		adif[i].atype[0] = ACTION_TYPE_NONE;
		adif[i].atype[1] = ACTION_TYPE_NONE;
		adif[i].atype[2] = ACTION_TYPE_NONE;
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
		Set_Type(ACTION_TYPE_HIT);
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
		Set_Type(ACTION_TYPE_HIT);
		Set_Type(ACTION_TYPE_HIT);
	}
	else if (hitcount >= 3) {
		key->z = 1;
		key->x = 1;
		key->c = 1;
		Set_Type(ACTION_TYPE_HIT);
		Set_Type(ACTION_TYPE_HIT);
		Set_Type(ACTION_TYPE_HIT);
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
		Set_Type(ACTION_TYPE_ARROW_LEFT);
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
		Set_Type(ACTION_TYPE_ARROW_RIGHT);
	}
	return hitFG;
}

static void AutoBomb(rec_play_key_hold_t *key, note_box_2_t note[],
	short int objectNG[], int Ntime)
{
	if (key->down > 0) {
		if (note[objectNG[2]].object == 7 && note[objectNG[2]].hittime - Ntime <= 40) {
			key->down = 0;
			Set_Type(ACTION_TYPE_BOMB_MID);
		}
		if (note[objectNG[0]].object == 7 && note[objectNG[0]].hittime - Ntime <= 40) {
			key->up = 0;
			Set_Type(ACTION_TYPE_BOMB_MID);
		}
		if (note[objectNG[1]].object == 7 && note[objectNG[1]].hittime - Ntime <= 40) {
			if (note[objectNG[0]].object == 2 && note[objectNG[0]].hittime - Ntime <= 40 ||
				note[objectNG[2]].object == 7 && note[objectNG[2]].hittime - Ntime <= 40) {
				(key->up)++;
				key->down = 0;
				if (key->up == 1) {
					Set_Type(ACTION_TYPE_BOMB_HIGH);
				}
			}
			else {
				key->up = 0;
				(key->down)++;
				if (key->down == 1) {
					Set_Type(ACTION_TYPE_BOMB_LOW);
				}
			}
		}
	}
	else {
		if (note[objectNG[0]].object == 7 && note[objectNG[0]].hittime - Ntime <= 40) {
			key->up = 0;
			Set_Type(ACTION_TYPE_BOMB_MID);
		}
		if (note[objectNG[2]].object == 7 && note[objectNG[2]].hittime - Ntime <= 40) {
			key->down = 0;
			Set_Type(ACTION_TYPE_BOMB_MID);
		}
		if (note[objectNG[1]].object == 7 && note[objectNG[1]].hittime - Ntime <= 40) {
			if (note[objectNG[0]].object == 7 && note[objectNG[0]].hittime - Ntime <= 40 ||
				note[objectNG[2]].object == 2 && note[objectNG[2]].hittime - Ntime <= 40) {
				key->up = 0;
				(key->down)++;
				if (key->down == 1) {
					Set_Type(ACTION_TYPE_BOMB_LOW);
				}
			}
			else {
				(key->up)++;
				key->down = 0;
				if (key->up == 1) {
					Set_Type(ACTION_TYPE_BOMB_HIGH);
				}
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
		Set_Type(ACTION_TYPE_ARROW_UP);
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
		Set_Type(ACTION_TYPE_ARROW_DOWN);
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
			if (key->down == 1) {
				Set_Type(ACTION_TYPE_CATCH_LOW);
			}
		}
		if (note[objectNG[1]].object == 2 && note[objectNG[1]].hittime - Ntime <= 8) {
			key->up = 0;
			key->down = 0;
			Set_Type(ACTION_TYPE_CATCH_MID);
		}
		if (note[objectNG[0]].object == 2 && note[objectNG[0]].hittime - Ntime <= 8) {
			(key->up)++;
			key->down = 0;
			if (key->up == 1) {
				Set_Type(ACTION_TYPE_CATCH_HIGH);
			}
		}
	}
	else {
		if (note[objectNG[0]].object == 2 && note[objectNG[0]].hittime - Ntime <= 8) {
			(key->up)++;
			key->down = 0;
			if (key->up == 1) {
				Set_Type(ACTION_TYPE_CATCH_HIGH);
			}
		}
		if (note[objectNG[1]].object == 2 && note[objectNG[1]].hittime - Ntime <= 8) {
			key->up = 0;
			key->down = 0;
			Set_Type(ACTION_TYPE_CATCH_MID);
		}
		if (note[objectNG[2]].object == 2 && note[objectNG[2]].hittime - Ntime <= 8) {
			key->up = 0;
			(key->down)++;
			if (key->down == 1) {
				Set_Type(ACTION_TYPE_CATCH_LOW);
			}
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
	if (buf_type[0] != ACTION_TYPE_NONE) {
		adif[nowNo].time = Ntime;
		adif[nowNo].atype[0] = buf_type[0];
		adif[nowNo].atype[1] = buf_type[1];
		adif[nowNo].atype[2] = buf_type[2];
		if (adif[nowNo].prev->time + 10 <= Ntime) {
			adif[nowNo].def = 3000000 * Cal_Type_Mlp() / ((Ntime - adif[nowNo].prev->time) * 100);
		}
		else {
			adif[nowNo].def = adif[nowNo].prev->def * 12 * Cal_Type_Mlp() / 1000;
		}
		nowNo = (nowNo + 1) % 50;
		buf_type[0] = ACTION_TYPE_NONE;
		buf_type[1] = ACTION_TYPE_NONE;
		buf_type[2] = ACTION_TYPE_NONE;
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
