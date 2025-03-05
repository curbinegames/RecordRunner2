
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <dxcur.h>
#include <sancur.h>
#include <system.h>

/* rec system include */
#include <option.h>
#include <playbox.h>
#include <PlayHitEff.h>
#include <PlayViewJudge.h>

/* own include */
#include <PlayNoteJudge.h>

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
static rec_play_sound_c *sp_sound = NULL;
static int *s_soundItem = NULL;
static int s_Ntime = 0;

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
	for (uint inum = 0; inum < 24; inum++) {
		ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, MelodySnd[inum]);
	}
	return;
}

static void PlayNoteHitSound(const note_box_2_t *note) {
	if (note->melody != MELODYSOUND_NONE) {
		PlaySoundMem(MelodySnd[note->melody], DX_PLAYTYPE_BACK);
	}
	else if (note->sound != 0) {
		PlaySoundMem(s_soundItem[note->sound - 1], DX_PLAYTYPE_BACK);
	}
	else {
		sp_sound->PlayNoteSound(note->object);
	}
	return;
}

static void PlayNoteHitSoundGeneral(note_judge judge, const note_box_2_t *noteinfo) {
	if (optiondata.SEenable != 0) { return; }

	switch (noteinfo->object) {
	case NOTE_HIT:
	case NOTE_CATCH:
	case NOTE_UP:
	case NOTE_DOWN:
	case NOTE_LEFT:
	case NOTE_RIGHT:
		if (judge != NOTE_JUDGE_MISS) {
			PlayNoteHitSound(noteinfo);
		}
		break;
	case NOTE_BOMB:
		if (judge == NOTE_JUDGE_MISS) {
			PlayNoteHitSound(noteinfo);
		}
		break;
	}
	return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**
 * HITノートがどのレーンにあるかを調べる
 * @param[in] note ノーツ情報
 * @param[in] noteNo ノーツの番号
 * @param[in] Ntime 曲が開始してからの時間[ms]
 * @return レーンナンバー、見つからない場合は-1
 */
static int RecPlayFindNearHitNote(const note_box_2_t note[], short noteNo[]) {
	int ans = -1;
	int mintime = 200;
	int gap = 300;

	for (int i = 0; i < 3; i++) {
		gap = note[noteNo[i]].hittime - s_Ntime;
		if (note[noteNo[i]].object == NOTE_HIT && gap < mintime) {
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
	if (IS_BETWEEN(-(P_JUST_TIME), gap, P_JUST_TIME)) { return NOTE_JUDGE_PJUST; }
	else if (IS_BETWEEN(-(JUST_TIME), gap, JUST_TIME)) { return NOTE_JUDGE_JUST; }
	else if (IS_BETWEEN(-(GOOD_TIME), gap, GOOD_TIME)) { return NOTE_JUDGE_GOOD; }
	else if (IS_BETWEEN(-(SAFE_TIME), gap, SAFE_TIME)) { return NOTE_JUDGE_SAFE; }
	else if (gap <= F_MISS_TIME) { return NOTE_JUDGE_MISS; }
	else { return NOTE_JUDGE_NONE; }
}

/**
 * arrow ノートをたたいたかどうかの判定を返す
 * @param[in] mat ノートの種類
 * @param[in] key 押しているキーの情報
 * @return たたいていたらtrue, たたいていなかったらfalse
 */
static bool CheckArrowInJudge(note_material mat, key_hold_t *key) {
	switch (mat) {
	case NOTE_UP:
		if (key->up == 1) { return true; }
		break;
	case NOTE_DOWN:
		if (key->down == 1) { return true; }
		break;
	case NOTE_LEFT:
		if (key->left == 1) { return true; }
		break;
	case NOTE_RIGHT:
		if (key->right == 1) { return true; }
		break;
	}
	return false;
}

static void SetHitPosByHit(hitatt_t *hitatk, const char hitflag) {
	int n = 0;
	int ret = 0;
	for (int i = 0; i < 3; i++) {
		if (hitflag & (1 << i)) {
			n++;
			ret = i;
		}
	}
	if (n == 0) { return; }
	if (n >= 2) {
		hitatk->pos = RECR_CHARP_M;
		hitatk->time = s_Ntime;
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
	hitatk->time = s_Ntime;
	return;
}

/**
 * Hitキーが押された個数を返す
 * @param[in] keyhold 押しているキーの情報
 * @return Hitキーが押された個数
 */
static uint RecLPlayGetHitKeyPushCount(const key_hold_t *keyhold) {
	uint ret = 0;
	if (keyhold == NULL) { return 0; }
	if (keyhold->z == 1) { ret++; }
	if (keyhold->x == 1) { ret++; }
	if (keyhold->c == 1) { ret++; }
	return ret;
}

#if 1 /* RecNoteJudgeEventAll */

/**
 * userpalの値を更新します
 * @param[out] userpal  更新するuserpalへのポインタ
 * @param[in] Ntime     Ntimeの値
 * @param[in] setBscore trueを入れるとBscoreを更新します
 * @param[in] PjustFg   pjustであるかをtrue/falseで入力してください
 * @param[in] addLife   増減するlifeの値
 * @param[in] addDscore 増減するDscoreの値
 * @param[in] addCombo  増減するcomboの値、0未満を入れるとコンボが0になります
 */
static void RecJudgeSetUserpal(userpal_t *userpal, bool setBscore, bool PjustFg,
	int addLife, int addDscore, int addCombo)
{
	if (setBscore) { userpal->score.before = pals(500, userpal->score.sum, 0, userpal->score.before, mins_2(s_Ntime - userpal->score.time, 500)); }
	userpal->score.time = s_Ntime;
	if (PjustFg) { (userpal->judgeCount.pjust)++; }
	(userpal->life) += addLife;
	(userpal->Dscore.add) += addDscore;
	if (addCombo < 0) { userpal->Ncombo = 0; }
	else { (userpal->Ncombo) += addCombo; }
	return;
}

static void note_judge_event(note_judge judge, userpal_t *userpal) {
	switch (judge) {
	case NOTE_JUDGE_PJUST:
	case NOTE_JUDGE_JUST:
		(userpal->judgeCount.just)++;
		break;
	case NOTE_JUDGE_GOOD:
		(userpal->judgeCount.good)++;
		break;
	case NOTE_JUDGE_SAFE:
		(userpal->judgeCount.safe)++;
		break;
	case NOTE_JUDGE_MISS:
		(userpal->judgeCount.miss)++;
		break;
	}

	switch (judge) {
	/*      userpal, setBscore, Pjust, +Life, +Dscore, +Combo); */
	case NOTE_JUDGE_PJUST:
		RecJudgeSetUserpal(
			userpal,      true,  true,     2,       2,      1);
		break;
	case NOTE_JUDGE_JUST:
		RecJudgeSetUserpal(
			userpal,      true, false,     2,       2,      1);
		break;
	case NOTE_JUDGE_GOOD:
		RecJudgeSetUserpal(
			userpal,      true, false,     1,       1,      1);
		break;
	case NOTE_JUDGE_SAFE:
		RecJudgeSetUserpal(
			userpal,      true, false,     0,       0,      0);
		break;
	case NOTE_JUDGE_MISS:
		RecJudgeSetUserpal(
			userpal,     false, false,   -20,       0,     -1);
		break;
	}

	return;
}

/* 目標の関数の形: RecNoteJudgeEventAll(判定, ノーツの種類); */
static void RecNoteJudgeEventAll(note_judge judge, const note_box_2_t note[], int lineNo,
	userpal_t *userpal, short noteNo[])
{
	int GapTime = 0;
	const note_box_2_t *noteinfo = &note[noteNo[lineNo]];
	if (judge == NOTE_JUDGE_NONE) { return; }

	PlaySetJudge(judge);
	PlaySetHitEffect(judge, noteinfo->object, lineNo);
	note_judge_event(judge, userpal);
	PlayNoteHitSoundGeneral(judge, noteinfo);

	/* gapのデータ(ズレの平均と偏差値)を更新する */
	GapTime = noteinfo->hittime - s_Ntime;
	if ((-SAFE_TIME) <= GapTime && GapTime <= SAFE_TIME) {
		switch (noteinfo->object) {
		case NOTE_HIT:
		case NOTE_UP:
		case NOTE_DOWN:
		case NOTE_LEFT:
		case NOTE_RIGHT:
			userpal->gap.view[userpal->gap.count % 30] = GapTime;
			if ((int)(userpal->gap.ssum + GapTime * GapTime) < (int)(userpal->gap.ssum)) {
				userpal->gap.sum /= 2;
				userpal->gap.ssum /= 2;
				userpal->gap.count /= 2;
			}
			userpal->gap.sum += GapTime;
			userpal->gap.ssum += GapTime * GapTime;
			userpal->gap.count++;
			break;
		}
	}

	noteNo[lineNo] = noteinfo->next;
	return;
}

#endif /* RecNoteJudgeEventAll */

#if 1 /* RecJudge */

static void RecJudgeHitNote(note_box_2_t note[], short noteNo[], userpal_t *userpal,
	hitatt_t *hitatk, int push_key_count)
{
	int near_lane = -1;
	char hitflag = 0; //hit event, bit unit: 0: upper hit, 1: middle hit, 2: lower hit, 3~7: reserved
	int gap = 0;
	note_judge NJ = NOTE_JUDGE_PJUST;
	
	for (int iPush = 0; iPush < push_key_count; iPush++) {
		near_lane = RecPlayFindNearHitNote(note, noteNo);
		if (near_lane == -1) {
			if (iPush == 0) {
				sp_sound->PlaySwing();
			}
			break;
		}
		gap = note[noteNo[near_lane]].hittime - s_Ntime;
		hitflag |= (1 << near_lane);
		NJ = CheckJudge(gap);
		if (NJ == NOTE_JUDGE_MISS) {
			sp_sound->PlaySwing();
		}
		RecNoteJudgeEventAll(NJ, note, near_lane, userpal, noteNo);
	}
	SetHitPosByHit(hitatk, hitflag);
}

static void RecJudgeArrowNote(note_box_2_t note[], short noteNo[], userpal_t *userpal,
	key_hold_t *keyhold)
{
	int avoidFg[3] = { 0,0,0 };
	note_box_2_t buf[3];

	buf[0] = note[noteNo[0]];
	buf[1] = note[noteNo[1]];
	buf[2] = note[noteNo[2]];

	/* アローのAVOID_ARROW_DIV_TIME以上のズレを検出する */
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
		note_judge NJ = NOTE_JUDGE_NONE;

		/* ズレを検出しているレーンは無視 */
		if (avoidFg[iLine] == 1) { continue; }

		GapTime = note[noteNo[iLine]].hittime - s_Ntime;
		NJ = CheckJudge(GapTime);
		if (NJ == NOTE_JUDGE_NONE) { continue; }

		if (CheckArrowInJudge(note[noteNo[iLine]].object, keyhold)) {
			RecNoteJudgeEventAll(NJ, note, iLine, userpal, noteNo);
		}
	}
	return;
}

static void RecJudgeCatchNote(note_box_2_t note[], short noteNo[], userpal_t *userpal,
	hitatt_t *hitatk, int *charahit, int LaneTrack[])
{
	int GapTime = 0;

	for (int iLine = 0; iLine < 3; iLine++) {
		while (IS_BETWEEN(0, note[noteNo[iLine]].hittime, LaneTrack[iLine] + SAFE_TIME) &&
			IS_BETWEEN(0, note[noteNo[iLine]].hittime, s_Ntime) &&
			note[noteNo[iLine]].object == NOTE_CATCH)
		{
			RecNoteJudgeEventAll(NOTE_JUDGE_PJUST, note, iLine, userpal, noteNo);
			*charahit = 0;
			hitatk->time = -1000;
		}
	}
	return;
}

static void RecJudgeBombNote(note_box_2_t note[], short noteNo[], userpal_t *userpal,
	hitatt_t *hitatk, int *charahit, short charaput)
{
	int GapTime = 0;

	for (int iLine = 0; iLine < 3; iLine++) {
		while (IS_BETWEEN(0, note[noteNo[iLine]].hittime, s_Ntime) &&
			note[noteNo[iLine]].object == NOTE_BOMB)
		{
			GapTime = note[noteNo[iLine]].hittime - s_Ntime;
			note_judge b_judge =
				(iLine == charaput && GapTime <= 0) ? NOTE_JUDGE_MISS : NOTE_JUDGE_PJUST;
			RecNoteJudgeEventAll(b_judge, note, iLine, userpal, noteNo);
			*charahit = 0;
			hitatk->time = -1000;
		}
	}
	return;
}

static void RecJudgeGhostNote(note_box_2_t note[], short noteNo[])
{
	for (int iLine = 0; iLine < 3; iLine++) {
		while (IS_BETWEEN(0, note[noteNo[iLine]].hittime, s_Ntime) &&
			note[noteNo[iLine]].object == NOTE_GHOST)
		{
			PlayNoteHitSound(&note[noteNo[iLine]]);
			noteNo[iLine] = note[noteNo[iLine]].next;
		}
	}
	return;
}

//HIT/CATCH/ARROWノーツslowmiss
static void RecJudgeSlowMiss(note_box_2_t note[], short noteNo[], userpal_t *userpal) {
	int GapTime = 0;

	for (int iLine = 0; iLine < 3; iLine++) {
		GapTime = note[noteNo[iLine]].hittime - s_Ntime;
		while (
			IS_BETWEEN(-1000000, GapTime, -(SAFE_TIME)) &&
			IS_BETWEEN(NOTE_HIT, note[noteNo[iLine]].object, NOTE_RIGHT))
		{
			RecNoteJudgeEventAll(NOTE_JUDGE_MISS, note, iLine, userpal, noteNo);
			GapTime = note[noteNo[iLine]].hittime - s_Ntime;
		}
	}
	return;
}

void RecJudgeAllNotes(note_box_2_t note[], short noteNo[], int Ntime, int *Sitem,
	key_hold_t *keyhold, hitatt_t *hitatk, int LaneTrack[], int *charahit, short charaput,
	userpal_t *userpal, rec_play_sound_c *p_sound)
{
	int push_key_count = RecLPlayGetHitKeyPushCount(keyhold);
	sp_sound = p_sound;
	s_soundItem = Sitem;
	s_Ntime = Ntime;
	
	RecJudgeHitNote(  note, noteNo, userpal, hitatk, push_key_count);
	RecJudgeArrowNote(note, noteNo, userpal, keyhold);
	RecJudgeCatchNote(note, noteNo, userpal, hitatk, charahit, LaneTrack);
	RecJudgeBombNote( note, noteNo, userpal, hitatk, charahit, charaput);
	RecJudgeGhostNote(note, noteNo);
	RecJudgeSlowMiss( note, noteNo, userpal);
	return;
}

#endif /* RecJudge */
