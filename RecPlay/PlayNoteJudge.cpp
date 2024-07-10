
#include <DxLib.h>
#include "../general/sancur.h"
#include "../system.h"
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

typedef enum chara_pos_e {
	RECR_CHARP_U = 0,
	RECR_CHARP_M,
	RECR_CHARP_D
} chara_pos_t;

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
static int RecPlayFindNearNote(const note_box_2_t note[], short int noteNo[], note_material mat, int Ntime) {
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
	if (-JUST_TIME <= gap && gap <= JUST_TIME) {
		return NOTE_JUDGE_JUST;
	}
	else if (-GOOD_TIME <= gap && gap <= GOOD_TIME) {
		return NOTE_JUDGE_GOOD;
	}
	else if (-SAFE_TIME <= gap && gap <= SAFE_TIME) {
		return NOTE_JUDGE_SAFE;
	}
	else if (gap <= F_MISS_TIME) {
		return NOTE_JUDGE_MISS;
	}
	else {
		return NOTE_JUDGE_NONE;
	}
}

/**
 * arrow ノートをたたいたかどうかの判定を返す
 * @param[in] mat ノートの種類
 * @param[in] gap 判定の差[ms]
 * @param[in] key 押しているキーの情報
 * @return たたいていたら1, たたいていなかったら0
 */
static int CheckArrowInJudge(note_material mat, int gap, rec_play_key_hold_t *key) {
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

static void SetHitPosByHit(rec_play_chara_hit_attack_t *hitatk, char const hitflag, int Ntime) {
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

/* TODO: soundflagを管理する関数を作る */
/* TODO: 効果音処理を別ファイルに */
static char PlayNoteHitSound(note_box_2_t note, int *MelodySnd, int *Sitem, char seflag, int notemat) {
	if (note.melody != MELODYSOUND_NONE) {
		PlaySoundMem(MelodySnd[note.melody], DX_PLAYTYPE_BACK);
	}
	else if (note.sound != 0) {
		PlaySoundMem(Sitem[note.sound - 1], DX_PLAYTYPE_BACK);
	}
	else {
		seflag |= notemat;
	}
	return seflag;
}

static void note_judge_event(note_judge judge, distance_score_t *Dscore,
	note_box_2_t const *const noteinfo, int* const Sitem, int Ntime,
	int Jtime, int lineNo, judge_action_box* const judgeA)
{
	/* TODO: judgeAの整理 */
	if (judge == NOTE_JUDGE_NONE) { return; }
	int* const combo = judgeA->combo;
	int* const SoundEnFg = judgeA->soundEnFg;
	gap_box* const gap = judgeA->gap;
	struct judge_box* const judge_b = judgeA->judge;
	int* const life = judgeA->life;
	int* const MelodySnd = judgeA->melody_snd;
	play_sound_t* const sound = judgeA->p_sound;
	struct score_box* const score = judgeA->score;
	note_material note = noteinfo->object;
	PlaySetJudge(judge);
	PlaySetHitEffect(judge, note, lineNo);

	/* 全ノーツ共通 */
	if (judge != NOTE_JUDGE_MISS) {
		score->before = pals(500, score->sum, 0, score->before, maxs(Ntime - score->time, 500));
		score->time = Ntime;
	}

	/* 判定ごとの処理 */
	switch (judge) {
	case NOTE_JUDGE_JUST:
		(*combo)++;
		*life += 2;
		(Dscore->add)++;
		(judge_b->just)++;
		break;
	case NOTE_JUDGE_GOOD:
		(*combo)++;
		(*life)++;
		// (Dscore->add) += 0;
		(judge_b->good)++;
		break;
	case NOTE_JUDGE_SAFE:
		// *combo += 0;
		// *life += 0;
		// (Dscore->add) += 0;
		(judge_b->safe)++;
		break;
	case NOTE_JUDGE_MISS:
		*combo = 0;
		*life -= 20;
		// (Dscore->add) += 0;
		(judge_b->miss)++;
		break;
	default:
		/* none */
		break;
	}

	/* ノートごとの処理 */
	switch (note) {
	case NOTE_HIT:
		AddGap(gap, Jtime); // slow miss はやらない
		if (-P_JUST_TIME <= Jtime && Jtime <= P_JUST_TIME) {
			(judge_b->pjust)++;
		}
		if (judge != NOTE_JUDGE_MISS && *SoundEnFg == 0) {
			sound->flag = PlayNoteHitSound(*noteinfo, MelodySnd, Sitem, sound->flag, SE_HIT);
		}
		break;
	case NOTE_CATCH:
		if (judge == NOTE_JUDGE_JUST) {
			(judge_b->pjust)++;
			if (*SoundEnFg == 0) {
				sound->flag = PlayNoteHitSound(*noteinfo, MelodySnd, Sitem, sound->flag, SE_CATCH);
			}
		}
		break;
	case NOTE_UP:
	case NOTE_DOWN:
	case NOTE_LEFT:
	case NOTE_RIGHT:
		AddGap(gap, Jtime); // slow miss はやらない
		if (-P_JUST_TIME <= Jtime && Jtime <= P_JUST_TIME) {
			(judge_b->pjust)++;
		}
		if (judge != NOTE_JUDGE_MISS && *SoundEnFg == 0) {
			sound->flag = PlayNoteHitSound(*noteinfo, MelodySnd, Sitem, sound->flag, SE_ARROW);
		}
		break;
	case NOTE_BOMB:
		switch (judge) {
		case NOTE_JUDGE_JUST:
			(judge_b->pjust)++;
			break;
		case NOTE_JUDGE_MISS:
			if (*SoundEnFg == 0) {
				sound->flag = PlayNoteHitSound(*noteinfo, MelodySnd, Sitem, sound->flag, SE_BOMB);
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
	int Ntime, note_judge judge, distance_score_t *Dscore, int Sitem[], int Line,
	judge_action_box *judgeA, int *charahit, rec_play_chara_hit_attack_t *hitatk,
	play_sound_t *p_sound, int MelodySnd[])
{
	/* パラメータチェック */
	if ((mat == NOTE_CATCH || mat == NOTE_BOMB) && Dscore == NULL) { return; }
	if ((mat == NOTE_CATCH || mat == NOTE_BOMB) && judgeA == NULL) { return; }
	if (mat == NOTE_CATCH && charahit == NULL) { return; }
	if (mat == NOTE_CATCH && hitatk == NULL) { return; }
	if (mat == NOTE_GHOST && MelodySnd == NULL) { return; }
	if (mat == NOTE_GHOST && p_sound == NULL) { return; }

	switch (mat) {
	case NOTE_CATCH:
		while (note[objectN[Line]].hittime - Ntime <= 0 &&
			0 <= note[objectN[Line]].hittime &&
			note[objectN[Line]].object == NOTE_CATCH)
		{
			note_judge_event(judge, Dscore,
				&note[objectN[Line]], Sitem, Ntime, 0,
				Line, judgeA);
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
			note_judge_event(judge, Dscore,
				&note[objectN[Line]], Sitem, Ntime, -JUST_TIME - 1,
				Line, judgeA);
			objectN[Line] = note[objectN[Line]].next;
		}
		break;
	case NOTE_GHOST:
		while (note[objectN[Line]].hittime - Ntime < 0 &&
			note[objectN[Line]].object == NOTE_GHOST) {
			p_sound->flag = PlayNoteHitSound(note[objectN[Line]],
				MelodySnd, Sitem, p_sound->flag, SE_GHOST);
			objectN[Line] = note[objectN[Line]].next;
		}
		break;
	}
	return;
}

static void RecJudgeHit(note_box_2_t note[], short int noteNo[], int Ntime, distance_score_t *Dscore,
	int Sitem[], judge_action_box *judgeA, rec_play_key_hold_t *keyhold,
	rec_play_chara_hit_attack_t *hitatk, play_sound_t *p_sound)
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
				p_sound->flag |= SE_SWING;
			}
			break;
		}
		gap = note[noteNo[near_lane]].hittime - Ntime;
		hitatk2 |= (1 << near_lane);
		NJ = CheckJudge(gap);
		if (NJ == NOTE_JUDGE_MISS) {
			p_sound->flag |= SE_SWING;
		}
		note_judge_event(NJ, Dscore, &note[noteNo[near_lane]],
			Sitem, Ntime, gap, near_lane, judgeA);
		noteNo[near_lane] = note[noteNo[near_lane]].next;
	}
	SetHitPosByHit(hitatk, hitatk2, Ntime);
}

static void RecJudgeArrow(note_box_2_t note[], short int noteNo[], int Ntime, distance_score_t *Dscore,
	int Sitem[], judge_action_box *judgeA, rec_play_key_hold_t *keyhold)
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
		if (avoidFg[iLine] == 1) {
			continue;
		}

		GapTime = note[noteNo[iLine]].hittime - Ntime;

		switch (note[noteNo[iLine]].object) {
		case NOTE_UP:
		case NOTE_DOWN:
		case NOTE_LEFT:
		case NOTE_RIGHT:
			if (CheckArrowInJudge(note[noteNo[iLine]].object, GapTime, keyhold)) {
				note_judge_event(CheckJudge(GapTime), Dscore,
					&note[noteNo[iLine]], Sitem, Ntime, GapTime, iLine, judgeA);
				noteNo[iLine] = note[noteNo[iLine]].next;
			}
			break;
		}
	}
	return;
}

static void RecJudgeOthersNote(note_box_2_t note[], short int noteNo[], int Ntime, distance_score_t *Dscore,
	int Sitem[], judge_action_box *judgeA, rec_play_chara_hit_attack_t *hitatk,
	play_sound_t *p_sound, int LaneTrack[], int *charahit, int MelodySnd[], short int charaput)
{
	int GapTime = 0;

	for (int iLine = 0; iLine < 3; iLine++) {
		GapTime = note[noteNo[iLine]].hittime - Ntime;
		switch (note[noteNo[iLine]].object) {
		case NOTE_CATCH:
			if (LaneTrack[iLine] + SAFE_TIME >= note[noteNo[iLine]].hittime) {
				note_judge_while_event(NOTE_CATCH, note, noteNo, Ntime,
					NOTE_JUDGE_JUST, Dscore, Sitem, iLine, judgeA, charahit, hitatk, NULL, NULL);
			}
			break;
		case NOTE_BOMB:
			if (iLine == charaput && GapTime <= 0) {
				note_judge_while_event(NOTE_BOMB, note, noteNo, Ntime,
					NOTE_JUDGE_MISS, Dscore, Sitem, iLine, judgeA, NULL, NULL, NULL, NULL);
			}
			else {
				note_judge_while_event(NOTE_BOMB, note, noteNo, Ntime,
					NOTE_JUDGE_JUST, Dscore, Sitem, iLine, judgeA, NULL, NULL, NULL, NULL);
			}
			break;
		case NOTE_GHOST:
			if (GapTime < 0) {
				note_judge_while_event(NOTE_GHOST, note, noteNo, Ntime,
					NOTE_JUDGE_NONE, NULL, Sitem, iLine, NULL, NULL, NULL, p_sound, MelodySnd);
			}
			break;
		}

		//全ノーツslowmiss
		while (GapTime <= -SAFE_TIME && GapTime >= -1000000 &&
			note[noteNo[iLine]].object >= NOTE_HIT &&
			note[noteNo[iLine]].object <= NOTE_RIGHT)
		{
			note_judge_event(NOTE_JUDGE_MISS, Dscore, &note[noteNo[iLine]],
				Sitem, Ntime, -SAFE_TIME, iLine, judgeA);
			noteNo[iLine] = note[noteNo[iLine]].next;
			GapTime = note[noteNo[iLine]].hittime - Ntime;
		}
	}
	return;
}

void RecJudgeAllNotes(note_box_2_t note[], short int noteNo[], int Ntime, distance_score_t *Dscore,
	int Sitem[], judge_action_box *judgeA, rec_play_key_hold_t *keyhold,
	rec_play_chara_hit_attack_t *hitatk, play_sound_t *p_sound, int LaneTrack[], int *charahit,
	int MelodySnd[], short int charaput)
{
	RecJudgeHit(note, noteNo, Ntime, Dscore, Sitem, judgeA, keyhold, hitatk, p_sound);
	RecJudgeArrow(note, noteNo, Ntime, Dscore, Sitem, judgeA, keyhold);
	RecJudgeOthersNote(note, noteNo, Ntime, Dscore, Sitem, judgeA, hitatk, p_sound,
		LaneTrack, charahit, MelodySnd, charaput);
	return;
}
