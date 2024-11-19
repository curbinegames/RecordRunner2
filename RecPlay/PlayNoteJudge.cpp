
#include <DxLib.h>
#include <sancur.h>
#include <dxcur.h>
#include <system.h>
#include "PlayNoteJudge.h"
#include "playbox.h"
#include "PlayViewJudge.h"
#include "PlayHitEff.h"

#define P_JUST_TIME 15
#define JUST_TIME 40
#define GOOD_TIME 70
#define SAFE_TIME 100
#define F_MISS_TIME 200

#define AVOID_ARROW_DIV_TIME 10

#define IS_REC_ARROW(mat) (((mat) == NOTE_UP) || ((mat) == NOTE_DOWN) || ((mat) == NOTE_LEFT) || ((mat) == NOTE_RIGHT))

typedef rec_play_chara_hit_attack_t hitatt_t;
typedef rec_play_key_hold_t key_hold_t;
typedef rec_play_userpal_t userpal_t;

typedef enum chara_pos_e {
	RECR_CHARP_U = 0,
	RECR_CHARP_M,
	RECR_CHARP_D
} chara_pos_t;

/* DOTO: 効果音系を別ファイルに移動する */
static int MelodySnd[24];

void RecPlayInitMelodySnd() {
	MelodySnd[0] = LoadSoundMem(L"sound/melody/lowF.wav");
	MelodySnd[1] = LoadSoundMem(L"sound/melody/lowF#.wav");
	MelodySnd[2] = LoadSoundMem(L"sound/melody/lowG.wav");
	MelodySnd[3] = LoadSoundMem(L"sound/melody/lowG#.wav");
	MelodySnd[4] = LoadSoundMem(L"sound/melody/lowA.wav");
	MelodySnd[5] = LoadSoundMem(L"sound/melody/lowA#.wav");
	MelodySnd[6] = LoadSoundMem(L"sound/melody/lowB.wav");
	MelodySnd[7] = LoadSoundMem(L"sound/melody/lowC.wav");
	MelodySnd[8] = LoadSoundMem(L"sound/melody/lowC#.wav");
	MelodySnd[9] = LoadSoundMem(L"sound/melody/lowD.wav");
	MelodySnd[10] = LoadSoundMem(L"sound/melody/lowD#.wav");
	MelodySnd[11] = LoadSoundMem(L"sound/melody/lowE.wav");
	MelodySnd[12] = LoadSoundMem(L"sound/melody/highF.wav");
	MelodySnd[13] = LoadSoundMem(L"sound/melody/highF#.wav");
	MelodySnd[14] = LoadSoundMem(L"sound/melody/highG.wav");
	MelodySnd[15] = LoadSoundMem(L"sound/melody/highG#.wav");
	MelodySnd[16] = LoadSoundMem(L"sound/melody/highA.wav");
	MelodySnd[17] = LoadSoundMem(L"sound/melody/highA#.wav");
	MelodySnd[18] = LoadSoundMem(L"sound/melody/highB.wav");
	MelodySnd[19] = LoadSoundMem(L"sound/melody/highC.wav");
	MelodySnd[20] = LoadSoundMem(L"sound/melody/highC#.wav");
	MelodySnd[21] = LoadSoundMem(L"sound/melody/highD.wav");
	MelodySnd[22] = LoadSoundMem(L"sound/melody/highD#.wav");
	MelodySnd[23] = LoadSoundMem(L"sound/melody/highE.wav");
	return;
}

static void PlayNoteHitSound(const note_box_2_t *note, int *Sitem, rec_play_sound_c *p_sound) {
	if (note->melody != MELODYSOUND_NONE) {
		PlaySoundMem(MelodySnd[note->melody], DX_PLAYTYPE_BACK);
	}
	else if (note->sound != 0) {
		PlaySoundMem(Sitem[note->sound - 1], DX_PLAYTYPE_BACK);
	}
	else {
		p_sound->PlayNoteSound(note->object);
	}
	return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void AddGap(gap_box* const box, int data) {
	box->view[box->count % 30] = data;
	if (box->ssum + data * data < box->ssum) {
		box->sum /= 2;
		box->ssum /= 2;
		box->count /= 2;
	}
	box->sum += data;
	box->ssum += data * data;
	box->count++;
	return;
}

/**
 * 指定されたノートがどのレーンにあるかを調べる
 * @param[in] note ノーツ情報
 * @param[in] noteNo ノーツの番号
 * @param[in] mat 探すノートの種類
 * @param[in] Ntime 曲が開始してからの時間[ms]
 * @return レーンナンバー、見つからない場合は-1
 */
static int RecPlayFindNearNote(const note_box_2_t note[],
	short int noteNo[], note_material mat, int Ntime)
{
	int ans = -1;
	int mintime = 200;
	int gap = 300;

	for (int i = 0; i < 3; i++) {
		gap = note[noteNo[i]].hittime - Ntime;
		if (note[noteNo[i]].object == mat && gap < mintime) {
			ans = i;
			mintime = gap;
		}
	}

	return ans;
}

/**
 * gap を判定に変換
 * @param[in] gap 判定の差[ms]
 * @return note_judge
 */
static note_judge CheckJudge(int gap) {
	if (-JUST_TIME <= gap && gap <= JUST_TIME) { return NOTE_JUDGE_JUST; }
	else if (-GOOD_TIME <= gap && gap <= GOOD_TIME) { return NOTE_JUDGE_GOOD; }
	else if (-SAFE_TIME <= gap && gap <= SAFE_TIME) { return NOTE_JUDGE_SAFE; }
	else if (gap <= F_MISS_TIME) { return NOTE_JUDGE_MISS; }
	else { return NOTE_JUDGE_NONE; }
}

/**
 * arrow ノートをたたいたかどうかの判定を返す
 * @param[in] mat ノートの種類
 * @param[in] gap 判定の差[ms]
 * @param[in] key 押しているキーの情報
 * @return たたいていたら1, たたいていなかったら0
 */
static int CheckArrowInJudge(note_material mat, int gap, key_hold_t *key) {
	if (CheckJudge(gap) == NOTE_JUDGE_NONE) { return 0; }
	switch (mat) {
	case NOTE_UP:
		if (key->up == 1) { return 1; }
		break;
	case NOTE_DOWN:
		if (key->down == 1) { return 1; }
		break;
	case NOTE_LEFT:
		if (key->left == 1) { return 1; }
		break;
	case NOTE_RIGHT:
		if (key->right == 1) { return 1; }
		break;
	}
	return 0;
}

static void SetHitPosByHit(hitatt_t *hitatk, char const hitflag, int Ntime) {
	int n = 0;
	int ret = 0;
	for (int i = 0; i < 3; i++) {
		if (hitflag & 1 << i) {
			n++;
			ret = i;
		}
	}
	if (n == 0) { return; }
	if (n >= 2) {
		hitatk->pos = RECR_CHARP_M;
		hitatk->time = Ntime;
		return;
	}
	switch (ret) {
	case 0:
		hitatk->pos = RECR_CHARP_U;
		break;
	case 1:
		hitatk->pos = RECR_CHARP_M;
		break;
	case 2:
		hitatk->pos = RECR_CHARP_D;
		break;
	}
	hitatk->time = Ntime;
	return;
}

static void note_judge_event(note_judge judge, note_box_2_t const *noteinfo, int *Sitem, int Ntime,
	int Jtime, int lineNo, userpal_t *userpal, rec_play_sound_c *p_sound)
{
	if (judge == NOTE_JUDGE_NONE) { return; }
	note_material note = noteinfo->object;
	PlaySetJudge(judge);
	PlaySetHitEffect(judge, note, lineNo);

	/* 全ノーツ共通 */
	if (judge != NOTE_JUDGE_MISS) {
		userpal->score.before =
			pals(500, userpal->score.sum, 0, userpal->score.before,
				maxs(Ntime - userpal->score.time, 500));
		userpal->score.time = Ntime;
	}

	/* 判定ごとの処理 */
	switch (judge) {
	case NOTE_JUDGE_JUST:
		(userpal->Ncombo)++;
		(userpal->life) += 2;
		(userpal->Dscore.add)++;
		(userpal->judgeCount.just)++;
		break;
	case NOTE_JUDGE_GOOD:
		(userpal->Ncombo)++;
		(userpal->life)++;
		// (userpal->Dscore.add) += 0;
		(userpal->judgeCount.good)++;
		break;
	case NOTE_JUDGE_SAFE:
		// (userpal->Ncombo) += 0;
		// (userpal->life) += 0;
		// (userpal->Dscore.add) += 0;
		(userpal->judgeCount.safe)++;
		break;
	case NOTE_JUDGE_MISS:
		(userpal->Ncombo) = 0;
		(userpal->life) -= 20;
		// (userpal->Dscore.add) += 0;
		(userpal->judgeCount.miss)++;
		break;
	default:
		/* none */
		break;
	}

	/* ノートごとの処理 */
	switch (note) {
	case NOTE_HIT:
		AddGap(&userpal->gap, Jtime); // slow miss はやらない
		if (-P_JUST_TIME <= Jtime && Jtime <= P_JUST_TIME) {
			(userpal->judgeCount.pjust)++;
		}
		if (judge != NOTE_JUDGE_MISS && optiondata.SEenable == 0) {
			PlayNoteHitSound(noteinfo, Sitem, p_sound);
		}
		break;
	case NOTE_CATCH:
		if (judge == NOTE_JUDGE_JUST) {
			(userpal->judgeCount.pjust)++;
			if (optiondata.SEenable == 0) {
				PlayNoteHitSound(noteinfo, Sitem, p_sound);
			}
		}
		break;
	case NOTE_UP:
	case NOTE_DOWN:
	case NOTE_LEFT:
	case NOTE_RIGHT:
		AddGap(&userpal->gap, Jtime); // slow miss はやらない
		if (-P_JUST_TIME <= Jtime && Jtime <= P_JUST_TIME) {
			(userpal->judgeCount.pjust)++;
		}
		if (judge != NOTE_JUDGE_MISS && optiondata.SEenable == 0) {
			PlayNoteHitSound(noteinfo, Sitem, p_sound);
		}
		break;
	case NOTE_BOMB:
		switch (judge) {
		case NOTE_JUDGE_JUST:
			(userpal->judgeCount.pjust)++;
			break;
		case NOTE_JUDGE_MISS:
			if (optiondata.SEenable == 0) {
				PlayNoteHitSound(noteinfo, Sitem, p_sound);
			}
			break;
		default:
			/* nope */
			break;
		}
		break;
	default:
		/* nope */
		break;
	}
	return;
}

static void note_judge_while_event(note_material mat, note_box_2_t note[], short int objectN[],
	int Ntime, note_judge judge, int Sitem[], int Line, int *charahit, hitatt_t *hitatk,
	userpal_t *userpal, rec_play_sound_c *p_sound)
{
	/* パラメータチェック */
	if (mat == NOTE_CATCH && charahit == NULL) { return; }
	if (mat == NOTE_CATCH && hitatk == NULL) { return; }

	switch (mat) {
	case NOTE_CATCH:
		while (note[objectN[Line]].hittime - Ntime <= 0 &&
			0 <= note[objectN[Line]].hittime &&
			note[objectN[Line]].object == NOTE_CATCH)
		{
			note_judge_event(judge, &note[objectN[Line]], Sitem, Ntime, 0, Line, userpal, p_sound);
			*charahit = 0;
			hitatk->time = -1000;
			objectN[Line] = note[objectN[Line]].next;
		}
		break;
	case NOTE_BOMB:
		while (note[objectN[Line]].hittime - Ntime <= 0 &&
			0 <= note[objectN[Line]].hittime &&
			note[objectN[Line]].object == NOTE_BOMB)
		{
			note_judge_event(judge, &note[objectN[Line]], Sitem, Ntime, -JUST_TIME - 1, Line,
				userpal, p_sound);
			objectN[Line] = note[objectN[Line]].next;
		}
		break;
	case NOTE_GHOST:
		while (note[objectN[Line]].hittime - Ntime < 0 &&
			note[objectN[Line]].object == NOTE_GHOST)
		{
			PlayNoteHitSound(&note[objectN[Line]], Sitem, p_sound);
			objectN[Line] = note[objectN[Line]].next;
		}
		break;
	}
	return;
}

static void RecJudgeHit(note_box_2_t note[], short int noteNo[], int Ntime, int Sitem[],
	userpal_t *userpal, key_hold_t *keyhold, hitatt_t *hitatk, rec_play_sound_c *p_sound)
{
	int push_key_count = 0;
	int near_lane = -1;
	char hitatk2 = 0; //hit event, bit unit: 0: upper hit, 1: middle hit, 2: lower hit, 3~7: reserved
	int gap = 0;
	note_judge NJ = NOTE_JUDGE_JUST;

	if (keyhold->z == 1) { push_key_count++; }
	if (keyhold->x == 1) { push_key_count++; }
	if (keyhold->c == 1) { push_key_count++; }
	
	for (int iPush = 0; iPush < push_key_count; iPush++) {
		near_lane = RecPlayFindNearNote(note, noteNo, NOTE_HIT, Ntime);
		if (near_lane == -1) {
			if (iPush == 0) {
				p_sound->PlaySwing();
			}
			break;
		}
		gap = note[noteNo[near_lane]].hittime - Ntime;
		hitatk2 |= (1 << near_lane);
		NJ = CheckJudge(gap);
		if (NJ == NOTE_JUDGE_MISS) {
			p_sound->PlaySwing();
		}
		note_judge_event(NJ, &note[noteNo[near_lane]], Sitem, Ntime, gap, near_lane, userpal,
			p_sound);
		noteNo[near_lane] = note[noteNo[near_lane]].next;
	}
	SetHitPosByHit(hitatk, hitatk2, Ntime);
}

static void RecJudgeArrow(note_box_2_t note[], short int noteNo[], int Ntime, int Sitem[],
	userpal_t *userpal, key_hold_t *keyhold, rec_play_sound_c *p_sound)
{
	int avoidFg[3] = { 0,0,0 };
	note_box_2_t buf[3];

	buf[0] = note[noteNo[0]];
	buf[1] = note[noteNo[1]];
	buf[2] = note[noteNo[2]];

	/* アローの微ズレを検出する(AVOID_ARROW_DIV_TIME) */
	if (IS_REC_ARROW(buf[0].object)) {
		if (buf[0].object == buf[1].object) {
			if (buf[0].hittime >= buf[1].hittime + AVOID_ARROW_DIV_TIME) {
				avoidFg[0] = 1;
			}
			if (buf[1].hittime >= buf[0].hittime + AVOID_ARROW_DIV_TIME) {
				avoidFg[1] = 1;
			}
		}

		if (buf[0].object == buf[2].object) {
			if (buf[0].hittime >= buf[2].hittime + AVOID_ARROW_DIV_TIME) {
				avoidFg[0] = 1;
			}
			if (buf[2].hittime >= buf[0].hittime + AVOID_ARROW_DIV_TIME) {
				avoidFg[2] = 1;
			}
		}
	}

	if (IS_REC_ARROW(buf[1].object)) {
		if (buf[1].object == buf[2].object) {
			if (buf[1].hittime >= buf[2].hittime + AVOID_ARROW_DIV_TIME) {
				avoidFg[1] = 1;
			}
			if (buf[2].hittime >= buf[1].hittime + AVOID_ARROW_DIV_TIME) {
				avoidFg[2] = 1;
			}
		}
	}

	for (int iLine = 0; iLine < 3; iLine++) {
		int GapTime = 0;

		/* 微ズレを検出しているレーンは無視 */
		if (avoidFg[iLine] == 1) { continue; }

		GapTime = note[noteNo[iLine]].hittime - Ntime;

		switch (note[noteNo[iLine]].object) {
		case NOTE_UP:
		case NOTE_DOWN:
		case NOTE_LEFT:
		case NOTE_RIGHT:
			if (CheckArrowInJudge(note[noteNo[iLine]].object, GapTime, keyhold)) {
				note_judge_event(CheckJudge(GapTime), &note[noteNo[iLine]], Sitem, Ntime, GapTime,
					iLine, userpal, p_sound);
				noteNo[iLine] = note[noteNo[iLine]].next;
			}
			break;
		}
	}
	return;
}

static void RecJudgeOthersNote(note_box_2_t note[], short int noteNo[], int Ntime, int Sitem[],
	userpal_t *userpal, hitatt_t *hitatk, int LaneTrack[], int *charahit, short int charaput,
	rec_play_sound_c *p_sound)
{
	int GapTime = 0;

	for (int iLine = 0; iLine < 3; iLine++) {
		GapTime = note[noteNo[iLine]].hittime - Ntime;
		switch (note[noteNo[iLine]].object) {
		case NOTE_CATCH:
			if (LaneTrack[iLine] + SAFE_TIME >= note[noteNo[iLine]].hittime) {
				note_judge_while_event(NOTE_CATCH, note, noteNo, Ntime, NOTE_JUDGE_JUST, Sitem,
					iLine, charahit, hitatk, userpal, p_sound);
			}
			break;
		case NOTE_BOMB:
			if (iLine == charaput && GapTime <= 0) {
				note_judge_while_event(NOTE_BOMB, note, noteNo, Ntime, NOTE_JUDGE_MISS, Sitem,
					iLine, NULL, NULL, userpal, p_sound);
			}
			else {
				note_judge_while_event(NOTE_BOMB, note, noteNo, Ntime, NOTE_JUDGE_JUST, Sitem,
					iLine, NULL, NULL, userpal, p_sound);
			}
			break;
		case NOTE_GHOST:
			if (GapTime < 0) {
				note_judge_while_event(NOTE_GHOST, note, noteNo, Ntime, NOTE_JUDGE_NONE, Sitem,
					iLine, NULL, NULL, userpal, p_sound);
			}
			break;
		}

		//全ノーツslowmiss
		while (GapTime <= -SAFE_TIME && GapTime >= -1000000 &&
			note[noteNo[iLine]].object >= NOTE_HIT &&
			note[noteNo[iLine]].object <= NOTE_RIGHT)
		{
			note_judge_event(NOTE_JUDGE_MISS, &note[noteNo[iLine]], Sitem, Ntime, -SAFE_TIME,
				iLine, userpal, p_sound);
			noteNo[iLine] = note[noteNo[iLine]].next;
			GapTime = note[noteNo[iLine]].hittime - Ntime;
		}
	}
	return;
}

void RecJudgeAllNotes(note_box_2_t note[], short int noteNo[], int Ntime, int Sitem[],
	key_hold_t *keyhold, hitatt_t *hitatk, int LaneTrack[], int *charahit, short int charaput,
	userpal_t *userpal, rec_play_sound_c *p_sound)
{
	RecJudgeHit(note, noteNo, Ntime, Sitem, userpal, keyhold, hitatk, p_sound);
	RecJudgeArrow(note, noteNo, Ntime, Sitem, userpal, keyhold, p_sound);
	RecJudgeOthersNote(note, noteNo, Ntime, Sitem, userpal, hitatk, LaneTrack, charahit, charaput,
		p_sound);
	return;
}
