
/* base include */
#include <DxLib.h>

/* rec system include */
#include <playbox.h>
#include <PlayCamera.h>
#include <system.h>

/* own include */
#include <PlayHitEff.h>

typedef struct rec_play_hit_effect_picture_s {
	int hit[5];
	int up[5];
	int down[5];
	int left[5];
	int right[5];
	int bomb[5];
} rec_play_hit_effect_picture_t;

typedef struct view_jug_eff_s {
	int time = 0;
	note_judge judge = NOTE_JUDGE_NONE;
	note_material note = NOTE_NONE;
} view_jug_eff_t;

static rec_play_hit_effect_picture_t effimg;

/* init */
void ReadyEffPicture() {
	LoadDivGraph(L"picture/hiteff.png", 5, 5, 1, 50, 50, effimg.hit);
	LoadDivGraph(L"picture/upeff.png", 5, 5, 1, 50, 50, effimg.up);
	LoadDivGraph(L"picture/downeff.png", 5, 5, 1, 50, 50, effimg.down);
	LoadDivGraph(L"picture/lefteff.png", 5, 5, 1, 50, 50, effimg.left);
	LoadDivGraph(L"picture/righteff.png", 5, 5, 1, 50, 50, effimg.right);
	LoadDivGraph(L"picture/bombeff.png", 5, 5, 1, 50, 50, effimg.bomb);
	return;
}

static view_jug_eff_t EffState[3];

/* action */
void PlaySetHitEffect(note_judge judge, note_material notemat, int LineNo) {
	EffState[LineNo].time = GetNowCount();
	EffState[LineNo].judge = judge;
	EffState[LineNo].note = notemat;
	RecPlayDebug[0] = EffState[0].time;
	RecPlayDebug[1] = EffState[0].judge;
	RecPlayDebug[2] = EffState[0].note;
	return;
}

void PlayCheckHitEffect() {
	for (int lineNo = 0; lineNo < 3; lineNo++) {
		if (EffState[lineNo].time + 750 < GetNowCount()) {
			EffState[lineNo].time = 0;
			EffState[lineNo].judge = NOTE_JUDGE_NONE;
			EffState[lineNo].note = NOTE_NONE;
		}
	}
	return;
}

static void PlayShowHitEffectCap1(int *xline, int *yline, int lineNo) {
	note_material notemat = NOTE_NONE;
	int *img = NULL;
	int frame = 0;
	int xpos = xline[lineNo] - 10;
	int ypos = yline[lineNo] - 10;

	switch (EffState[lineNo].note - 1) {
	case 0:
		notemat = NOTE_HIT;
		break;
	case 1:
		notemat = NOTE_CATCH;
		break;
	case 2:
		notemat = NOTE_UP;
		break;
	case 3:
		notemat = NOTE_DOWN;
		break;
	case 4:
		notemat = NOTE_LEFT;
		break;
	case 5:
		notemat = NOTE_RIGHT;
		break;
	case 6:
		notemat = NOTE_BOMB;
		break;
	default:
		return;
	}

	if (EffState[lineNo].time + 250 <= GetNowCount()) {
		return;
	}

	switch (notemat) {
	case NOTE_HIT:
	case NOTE_CATCH:
	case NOTE_UP:
	case NOTE_DOWN:
	case NOTE_LEFT:
	case NOTE_RIGHT:
		if ((EffState[lineNo].judge < NOTE_JUDGE_JUST ||
			EffState[lineNo].judge > NOTE_JUDGE_SAFE) &&
			EffState[lineNo].judge != NOTE_JUDGE_PJUST)
		{
			return;
		}
		break;
	case NOTE_BOMB:
		if (EffState[lineNo].judge != NOTE_JUDGE_MISS) {
			return;
		}
		break;
	default:
		return;
	}

	switch (notemat) {
	case NOTE_HIT:
	case NOTE_CATCH:
		img = effimg.hit;
		break;
	case NOTE_UP:
		img = effimg.up;
		break;
	case NOTE_DOWN:
		img = effimg.down;
		break;
	case NOTE_LEFT:
		img = effimg.left;
		break;
	case NOTE_RIGHT:
		img = effimg.right;
		break;
	case NOTE_BOMB:
		img = effimg.bomb;
		break;
	default:
		return;
	}

	frame = (GetNowCount() - EffState[lineNo].time + 250) / 50 % 5;

	DrawGraphRecField(xpos, ypos, img[frame]);
	return;
}

void PlayShowHitEffect(int *xline, int *yline) {
	PlayShowHitEffectCap1(xline, yline, 0);
	PlayShowHitEffectCap1(xline, yline, 1);
	PlayShowHitEffectCap1(xline, yline, 2);
	return;
}