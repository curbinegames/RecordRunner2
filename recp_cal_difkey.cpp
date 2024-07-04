
/* include */
#include "DxLib.h"
#include "general/sancur.h"
#include "recp_cal_ddif.h"
#include "playbox.h"
#include "recp_cal_difkey.h"

/* define */
#define DDIF_NUM 25
#define DIFKEY_STEP_NUM(num) ((num) = ((num) + 1) % difkeySys.limit)

/* struct */
typedef struct difkey2_s difkey2_t;
struct difkey2_s {
	int key = 0; // [0]
	int time = 0; // [1]
	int dif = 0; // [2]
	difkey_act_t act = DIFKEY_ACT_NONE; // new
	difkey2_t* bef = NULL;
	difkey2_t* bbef = NULL;
};

/* static */
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

/* function */
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
	case 1: //hitノーツ補間
		if (p_difkey->bef->key == 1 &&
			p_difkey->time - 20 < p_difkey->bef->time) {
			p_difkey->bef->dif *= 1;
			return;
		}
		break;
	case 2: //catchノーツ補間
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
	case 5: //leftノーツ補間
		if (p_difkey->bef->key == 2 || p_difkey->bef->key == 7 ||
			p_difkey->bef->key == 9) {
			p_difkey->bef->key = 5;
			return;
		}
		break;
	case 6: //rightノーツ補間
		if (p_difkey->bef->key == 2 || p_difkey->bef->key == 7 ||
			p_difkey->bef->key == 9) {
			p_difkey->bef->key = 6;
			return;
		}
		break;
	case 7: //bombノーツ補間
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

difkey_act_t DifkeyCalAct(note_box const* const noteUp,
	note_box const* const noteMid, note_box const* const noteLow) {
	char laneFg = 0; // -,-,-,-,-,up,mid,low
	char noteFg = 0; // -,zhit,xhit,chit,up,down,left,right
	char Hcnt = 0;
	char Acnt = 0;
	int time = noteUp->hittime;
	difkey_act_t ret = DIFKEY_ACT_NONE;
	note_material Nnote[3] = { NOTE_NONE, NOTE_NONE, NOTE_NONE };
	// 一番早いノーツを取得
	if (noteMid->hittime < time) { time = noteMid->hittime; }
	if (noteLow->hittime < time) { time = noteLow->hittime; }
	// 同時押しとして処理するノーツを取得
	if (noteUp->hittime <= time + 20) { laneFg |= (1 << 2); }
	if (noteMid->hittime <= time + 20) { laneFg |= (1 << 1); }
	if (noteLow->hittime <= time + 20) { laneFg |= (1 << 0); }
	// 対象ノーツの種類を取得
	if ((laneFg & (1 << 2)) != 0) { Nnote[0] = noteUp->object; }
	if ((laneFg & (1 << 1)) != 0) { Nnote[1] = noteMid->object; }
	if ((laneFg & (1 << 0)) != 0) { Nnote[2] = noteLow->object; }
	// 対象ノーツの種類をフラグに入れる
	for (int i = 0; i < 2; i++) {
		switch (Nnote[i]) {
		case NOTE_HIT:
			if      ((noteFg & (1 << 6)) != 0) { noteFg |= (1 << 6); }
			else if ((noteFg & (1 << 5)) != 0) { noteFg |= (1 << 5); }
			else                               { noteFg |= (1 << 4); }
			break;
		case NOTE_UP:
			noteFg |= (1 << 3);
			break;
		case NOTE_DOWN:
			noteFg |= (1 << 2);
			break;
		case NOTE_LEFT:
			noteFg |= (1 << 1);
			break;
		case NOTE_RIGHT:
			noteFg |= (1 << 0);
			break;
		}
	}
	// キャッチ補正
	if (Nnote[0] == NOTE_CATCH) { noteFg |= (1 << 3); }
	if (Nnote[1] == NOTE_CATCH && (noteFg & (1 << 3)) != 0) {
		noteFg |= (1 << 2);
	}
	if (Nnote[1] == NOTE_CATCH && (noteFg & (1 << 2)) != 0) {
		noteFg |= (1 << 3);
	}
	if (Nnote[2] == NOTE_CATCH) { noteFg |= (1 << 2); }
	// ボム補正
	if (Nnote[0] == NOTE_BOMB && (noteFg & (1 << 3)) != 0) {
		noteFg |= (1 << 2);
	}
	if (Nnote[1] == NOTE_BOMB && (noteFg & (1 << 2)) == 0) {
		noteFg |= (1 << 3);
	}
	if (Nnote[2] == NOTE_BOMB && (noteFg & (1 << 2)) != 0) {
		noteFg |= (1 << 3);
	}
	// キー数を計算
	if      ((noteFg & (1 << 4)) != 0) { Hcnt = 3; }
	else if ((noteFg & (1 << 5)) != 0) { Hcnt = 2; }
	else if ((noteFg & (1 << 6)) != 0) { Hcnt = 1; }
	if ((noteFg & (1 << 3)) != 0) { Acnt++; }
	if ((noteFg & (1 << 2)) != 0) { Acnt++; }
	if ((noteFg & (1 << 1)) != 0) { Acnt++; }
	if ((noteFg & (1 << 0)) != 0) { Acnt++; }
	// 返り値を設定
	if      (Hcnt == 1 && Acnt == 0) { ret = DIFKEY_ACT_1HIT; }
	else if (Hcnt == 0 && Acnt == 1) { ret = DIFKEY_ACT_1ARW; }
	else if (Hcnt == 2 && Acnt == 0) { ret = DIFKEY_ACT_2HIT; }
	else if (Hcnt == 1 && Acnt == 1) { ret = DIFKEY_ACT_1HIT_1ARW; }
	else if (Hcnt == 0 && Acnt == 2) { ret = DIFKEY_ACT_2ARW; }
	else if (Hcnt == 3 && Acnt == 0) { ret = DIFKEY_ACT_3HIT; }
	else if (Hcnt == 2 && Acnt == 1) { ret = DIFKEY_ACT_2HIT_1ARW; }
	else if (Hcnt == 1 && Acnt == 2) { ret = DIFKEY_ACT_2HIT_1ARW; }
	else { ret = DIFKEY_ACT_NONE; }
	return ret;
}

void DifkeyCalInit(int notes, int songlength) {
	difkeySys.limit = (char)maxs(notes, 50);
	difkeySys.avoid = mins(songlength * 2 / DDIF_NUM, 10000);
	return;
}
