
/* rec system include */
#include <playbox.h>
#include <RecScoreFile.h>

typedef rec_play_key_hold_t key_hold_t;

static void AutoBeforeChain(key_hold_t *key, cvec<note_box_2_t> note[], int Ntime) {
	if (IS_NEAR_NOTE_ARROW_ANYLANE(note, Ntime, 40)) {
		key->up = 0;
		key->down = 0;
		key->left = 0;
		key->right = 0;
	}
	return;
}

static void AutoHit(key_hold_t *key, cvec<note_box_2_t> note[], int Ntime) {
	int hitcount = 0;
	int G = 0;
	for (int j = 0; j < 3; j++) {
		G = note[j].nowNo();
		for (int i = 0; i < 3; i++) {
			if (IS_NEAR_NOTE(&note[j][G], NOTE_HIT, Ntime, 8)) {
				hitcount++;
			}
			G++;
			if (note[j].size() <= G + 1) { break; }
		}
	}
	if (hitcount == 1) {
		key->x = 0;
		if (key->c == 0) {
			key->c = 1;
			key->z = 0;
		}
		else {
			key->z = 1;
			key->c = 0;
		}
	}
	else if (hitcount == 2) {
 		key->x = 1;
		if (key->c == 0) {
			key->c = 1;
			key->z = 0;
		}
		else {
			key->c = 0;
			key->z = 1;
		}
	}
	else if (hitcount >= 3) {
		key->z = 1;
		key->x = 1;
		key->c = 1;
	}
	return;
}

static void AutoArrowLR(key_hold_t *key, cvec<note_box_2_t> note[], int Ntime) {
	if (IS_NEAR_NOTE_ANYLANE(note, NOTE_LEFT,  Ntime, 8)) { key->left  = 1; }
	if (IS_NEAR_NOTE_ANYLANE(note, NOTE_RIGHT, Ntime, 8)) { key->right = 1; }
	return;
}

static void AutoBomb(key_hold_t *key, cvec<note_box_2_t> note[], int Ntime) {
	if (key->down > 0) {
		if (IS_NEAR_NOTE_BOMB(&note[2].nowData(), Ntime)) { key->down = 0; }
		if (IS_NEAR_NOTE_BOMB(&note[0].nowData(), Ntime)) { key->up = 0; }
		if (IS_NEAR_NOTE_BOMB(&note[1].nowData(), Ntime)) {
			if (IS_NEAR_NOTE(&note[0].nowData(), NOTE_CATCH, Ntime, 8) ||
				IS_NEAR_NOTE_BOMB(&note[2].nowData(), Ntime))
			{
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
		if (IS_NEAR_NOTE_BOMB(&note[0].nowData(), Ntime)) { key->up = 0; }
		if (IS_NEAR_NOTE_BOMB(&note[2].nowData(), Ntime)) { key->down = 0; }
		if (IS_NEAR_NOTE_BOMB(&note[1].nowData(), Ntime)) {
			if (IS_NEAR_NOTE_BOMB(&note[0].nowData(), Ntime) ||
				IS_NEAR_NOTE(&note[2].nowData(), NOTE_CATCH, Ntime, 8))
			{
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

static void AutoArrowUD(key_hold_t *key, cvec<note_box_2_t> note[], int Ntime) {
	if (IS_NEAR_NOTE_ANYLANE(note, NOTE_UP, Ntime, 8)) {
		key->up = 1;
		if (IS_NEAR_NOTE_BOMB(&note[0].nowData(), Ntime)) { key->down = 1; }
	}
	if (IS_NEAR_NOTE_ANYLANE(note, NOTE_DOWN, Ntime, 8)) {
		key->down = 1;
		if (IS_NEAR_NOTE_BOMB(&note[2].nowData(), Ntime)) { key->up = 1; }
	}
	return;
}

static void AutoCatch(key_hold_t *key, cvec<note_box_2_t> note[], int Ntime) {
	if (key->up > 0) {
		if (IS_NEAR_NOTE(&note[2].nowData(), NOTE_CATCH, Ntime, 8)) {
			key->up = 0;
			(key->down)++;
		}
		if (IS_NEAR_NOTE(&note[1].nowData(), NOTE_CATCH, Ntime, 8)) {
			key->up = 0;
			key->down = 0;
		}
		if (IS_NEAR_NOTE(&note[0].nowData(), NOTE_CATCH, Ntime, 8)) {
			(key->up)++;
			key->down = 0;
		}
	}
	else {
		if (IS_NEAR_NOTE(&note[0].nowData(), NOTE_CATCH, Ntime, 8)) {
			(key->up)++;
			key->down = 0;
		}
		if (IS_NEAR_NOTE(&note[1].nowData(), NOTE_CATCH, Ntime, 8)) {
			key->up = 0;
			key->down = 0;
		}
		if (IS_NEAR_NOTE(&note[2].nowData(), NOTE_CATCH, Ntime, 8)) {
			key->up = 0;
			(key->down)++;
		}
	}
	return;
}

static void AutoReleaseKey(key_hold_t *key, cvec<note_box_2_t> note[], int Ntime) {
	if (key->z > 10) { key->z = 0; }
	if (key->x > 10) { key->x = 0; }
	if (key->c > 10) { key->c = 0; }
	if (key->left > 10) { key->left = 0; }
	if (key->right > 10) { key->right = 0; }
	if (!(IS_NEAR_NOTE_ACTOR_ANYLANE(note, Ntime, 1000))) {
		if (key->up > 10) { key->up = 0; }
		if (key->down > 10) { key->down = 0; }
	}
}

void AutoAution(key_hold_t *key, cvec<note_box_2_t> note[], int Ntime) {
	int hitFG = 0;
	// 押しっぱなし処理。AutoReleaseKeyでリリースするので永久ホールドはない。
	if (key->z > 0) { (key->z)++; }
	if (key->x > 0) { (key->x)++; }
	if (key->c > 0) { (key->c)++; }
	if (key->up > 0) { (key->up)++; }
	if (key->down > 0) { (key->down)++; }
	if (key->left > 0) { (key->left)++; }
	if (key->right > 0) { (key->right)++; }
	// ノーツ処理
	AutoBeforeChain(key, note, Ntime);
	AutoHit(        key, note, Ntime);
	AutoArrowLR(    key, note, Ntime);
	AutoBomb(       key, note, Ntime);
	AutoArrowUD(    key, note, Ntime);
	AutoCatch(      key, note, Ntime);
	AutoReleaseKey( key, note, Ntime);
	return;
}
