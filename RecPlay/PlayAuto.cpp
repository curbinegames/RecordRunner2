
/* rec system include */
#include <playbox.h>
#include <RecScoreFile.h>

/* 指定のノーツがgap[ms]以内にあったらtrue */
#define IS_NEAR_NOTE(notedata, mat, Ntime, gap)                             \
	((notedata)->object == (mat) && (notedata)->hittime - (Ntime) <= (gap))

/* bombノーツが40ms以内にあったらtrue */
#define IS_NEAR_NOTE_BOMB(notedata, Ntime) IS_NEAR_NOTE(notedata, NOTE_BOMB, Ntime, 40)

/* 指定のノーツがgap[ms]以内にどこかのレーンにあったらtrue */
#define IS_NEAR_NOTE_ANYLANE(notedata, num, mat, Ntime, gap) \
	(IS_NEAR_NOTE(&(notedata)[(num)[0]], mat, Ntime, gap) || \
	 IS_NEAR_NOTE(&(notedata)[(num)[1]], mat, Ntime, gap) || \
	 IS_NEAR_NOTE(&(notedata)[(num)[2]], mat, Ntime, gap))

/* arrowノーツがgap[ms]以内にどこかのレーンにあったらtrue */
#define IS_NEAR_NOTE_ARROW_ANYLANE(notedata, num, Ntime, gap)       \
	(IS_NEAR_NOTE_ANYLANE(notedata, num, NOTE_UP,    Ntime, gap) || \
	 IS_NEAR_NOTE_ANYLANE(notedata, num, NOTE_DOWN,  Ntime, gap) || \
	 IS_NEAR_NOTE_ANYLANE(notedata, num, NOTE_LEFT,  Ntime, gap) || \
	 IS_NEAR_NOTE_ANYLANE(notedata, num, NOTE_RIGHT, Ntime, gap))

/* actorノーツがgap[ms]以内にどこかのレーンにあったらtrue */
#define IS_NEAR_NOTE_ACTOR_ANYLANE(notedata, num, Ntime, gap)       \
	(IS_NEAR_NOTE_ANYLANE(notedata, num, NOTE_CATCH, Ntime, gap) || \
	 IS_NEAR_NOTE_ANYLANE(notedata, num, NOTE_BOMB,  Ntime, gap))

typedef rec_play_key_hold_t key_hold_t;

void InitAdif() { return; }
int GetAdif() { return 0; }

static void AutoBeforeChain(key_hold_t *key, note_box_2_t note[], short objectNG[], int Ntime) {
	if (IS_NEAR_NOTE_ARROW_ANYLANE(note, objectNG, Ntime, 40)) {
		key->up = 0;
		key->down = 0;
		key->left = 0;
		key->right = 0;
	}
	return;
}

static void AutoHit(key_hold_t *key, note_box_2_t note[], short objectNG[], int Ntime) {
	int hitcount = 0;
	int G = 0;
	for (int j = 0; j < 3; j++) {
		G = objectNG[j];
		for (int i = 0; i < 3; i++) {
			if (IS_NEAR_NOTE(&note[G], NOTE_HIT, Ntime, 8)) { hitcount++; }
			G = note[G].next;
			if (G == -1) { break; }
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

static void AutoArrowLR(key_hold_t *key, note_box_2_t note[], short objectNG[], int Ntime) {
	if (IS_NEAR_NOTE_ANYLANE(note, objectNG, NOTE_LEFT, Ntime, 8)) { key->left = 1; }
	if (IS_NEAR_NOTE_ANYLANE(note, objectNG, NOTE_RIGHT, Ntime, 8)) { key->right = 1; }
	return;
}

static void AutoBomb(key_hold_t *key, note_box_2_t note[], short objectNG[], int Ntime) {
	if (key->down > 0) {
		if (IS_NEAR_NOTE_BOMB(&note[objectNG[2]], Ntime)) { key->down = 0; }
		if (IS_NEAR_NOTE_BOMB(&note[objectNG[0]], Ntime)) { key->up = 0; }
		if (IS_NEAR_NOTE_BOMB(&note[objectNG[1]], Ntime)) {
			if (IS_NEAR_NOTE(&note[objectNG[0]], NOTE_CATCH, Ntime, 8) ||
				IS_NEAR_NOTE_BOMB(&note[objectNG[2]], Ntime))
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
		if (IS_NEAR_NOTE_BOMB(&note[objectNG[0]], Ntime)) { key->up = 0; }
		if (IS_NEAR_NOTE_BOMB(&note[objectNG[2]], Ntime)) { key->down = 0; }
		if (IS_NEAR_NOTE_BOMB(&note[objectNG[1]], Ntime)) {
			if (IS_NEAR_NOTE_BOMB(&note[objectNG[0]], Ntime) ||
				IS_NEAR_NOTE(&note[objectNG[2]], NOTE_CATCH, Ntime, 8))
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

static void AutoArrowUD(key_hold_t *key, note_box_2_t note[], short objectNG[], int Ntime) {
	if (IS_NEAR_NOTE_ANYLANE(note, objectNG, NOTE_UP, Ntime, 8)) {
		key->up = 1;
		if (IS_NEAR_NOTE_BOMB(&note[objectNG[0]], Ntime)) { key->down = 1; }
	}
	if (IS_NEAR_NOTE_ANYLANE(note, objectNG, NOTE_DOWN, Ntime, 8)) {
		key->down = 1;
		if (IS_NEAR_NOTE_BOMB(&note[objectNG[2]], Ntime)) { key->up = 1; }
	}
	return;
}

static void AutoCatch(key_hold_t *key, note_box_2_t note[], short objectNG[], int Ntime) {
	if (key->up > 0) {
		if (IS_NEAR_NOTE(&note[objectNG[2]], NOTE_CATCH, Ntime, 8)) {
			key->up = 0;
			(key->down)++;
		}
		if (IS_NEAR_NOTE(&note[objectNG[1]], NOTE_CATCH, Ntime, 8)) {
			key->up = 0;
			key->down = 0;
		}
		if (IS_NEAR_NOTE(&note[objectNG[0]], NOTE_CATCH, Ntime, 8)) {
			(key->up)++;
			key->down = 0;
		}
	}
	else {
		if (IS_NEAR_NOTE(&note[objectNG[0]], NOTE_CATCH, Ntime, 8)) {
			(key->up)++;
			key->down = 0;
		}
		if (IS_NEAR_NOTE(&note[objectNG[1]], NOTE_CATCH, Ntime, 8)) {
			key->up = 0;
			key->down = 0;
		}
		if (IS_NEAR_NOTE(&note[objectNG[2]], NOTE_CATCH, Ntime, 8)) {
			key->up = 0;
			(key->down)++;
		}
	}
	return;
}

static void AutoReleaseKey(key_hold_t *key, note_box_2_t note[], short objectNG[], int Ntime) {
	if (key->z > 10) { key->z = 0; }
	if (key->x > 10) { key->x = 0; }
	if (key->c > 10) { key->c = 0; }
	if (key->left > 10) { key->left = 0; }
	if (key->right > 10) { key->right = 0; }
	if (!(IS_NEAR_NOTE_ACTOR_ANYLANE(note, objectNG, Ntime, 1000))) {
		if (key->up > 10) { key->up = 0; }
		if (key->down > 10) { key->down = 0; }
	}
}

void AutoAution(key_hold_t *key, note_box_2_t note[], short objectNG[], int Ntime) {
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
	AutoBeforeChain(key, note, objectNG, Ntime);
	AutoHit(key, note, objectNG, Ntime);
	AutoArrowLR(key, note, objectNG, Ntime);
	AutoBomb(key, note, objectNG, Ntime);
	AutoArrowUD(key, note, objectNG, Ntime);
	AutoCatch(key, note, objectNG, Ntime);
	AutoReleaseKey(key, note, objectNG, Ntime);
	return;
}
