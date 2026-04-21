
/* base include */
#include <queue>
#include <DxLib.h>

/* curbine code include */
#include <dxcur.h>
#include <sancur.h>
#include <RecSystem.h>

/* rec system include */
#include <option.h>
#include <playbox.h>
#include <PlayHitEff.h>
#include <PlayViewJudge.h>
#include <RecScoreFile.h>

/* own include */
#include <PlayNoteJudge.h>

#define P_JUST_TIME 15
#define JUST_TIME 40
#define GOOD_TIME 70
#define SAFE_TIME 100
#define F_MISS_TIME 200

#define AVOID_ARROW_DIV_TIME 10

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

static void PlayNoteHitSound(const note_box_2_t *note, rec_play_sound_c *p_sound, const std::vector<DxSnd_t> &soundItem) {
	if (note->melody != MELODYSOUND_NONE) {
		PlaySoundMem(MelodySnd[note->melody], DX_PLAYTYPE_BACK);
	}
	else if (note->sound != 0) {
		PlaySoundMem(soundItem[note->sound - 1], DX_PLAYTYPE_BACK);
	}
	else {
		p_sound->PlayNoteSound(note->object);
	}
	return;
}

static void PlayNoteHitSoundGeneral(
	note_judge judge, const note_box_2_t *noteinfo, rec_play_sound_c *p_sound, const std::vector<DxSnd_t> soundItem
) {
	if (optiondata.SEenable != 0) { return; }

	switch (noteinfo->object) {
	case NOTE_HIT:
	case NOTE_CATCH:
	case NOTE_UP:
	case NOTE_DOWN:
	case NOTE_LEFT:
	case NOTE_RIGHT:
		if (judge != NOTE_JUDGE_MISS) {
			PlayNoteHitSound(noteinfo, p_sound, soundItem);
		}
		break;
	case NOTE_BOMB:
		if (judge == NOTE_JUDGE_MISS) {
			PlayNoteHitSound(noteinfo, p_sound, soundItem);
		}
		break;
	}
	return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**
 * HITノートがどのレーンにあるかを調べる
 * @param[in] note ノーツ情報
 * @param[in] Ntime 曲が開始してからの時間[ms]
 * @return レーンナンバー、見つからない場合は-1
 */
static int RecPlayFindNearHitNote(const cvec<note_box_2_t> note[]) {
	int ans = -1;
	int mintime = 200;
	int gap = 300;

	for (int i = 0; i < 3; i++) {
		gap = note[i].nowData().hittime - s_Ntime;
		if (note[i].nowData().object == NOTE_HIT && gap < mintime) {
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

static void SetHitPosByHit(rec_hitatk_event_ec &hitatk, const char hitflag) {
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
		hitatk = REC_HITATK_EVENT_MID;
		return;
	}
	switch (ret) {
	case 0:
		hitatk = REC_HITATK_EVENT_UP;
		break;
	case 1:
		hitatk = REC_HITATK_EVENT_MID;
		break;
	case 2:
		hitatk = REC_HITATK_EVENT_DOWN;
		break;
	}
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
 * @param[in] addExLife 増減するExlifeの値
 * @param[in] addDscore 増減するDscoreの値
 * @param[in] addCombo  増減するcomboの値、0未満を入れるとコンボが0になります
 */
static void RecJudgeSetUserpal(userpal_t *userpal, bool setBscore, bool PjustFg,
	int addLife, int addExLife, int addDscore, int addCombo)
{
	if (setBscore) { userpal->score.before = pals(500, userpal->score.sum, 0, userpal->score.before, mins_2(s_Ntime - userpal->score.time, 500)); }
	userpal->score.time = s_Ntime;
	if (PjustFg) { (userpal->judgeCount.pjust)++; }
	(userpal->life) += addLife;
	if (0 < userpal->Exlife) { (userpal->Exlife) += addExLife; }
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
	/*      userpal, setBscore, Pjust, +Life, +ExLife, +Dscore, +Combo); */
	case NOTE_JUDGE_PJUST:
		RecJudgeSetUserpal(
			userpal,      true,  true,     2,       2,       2,      1);
		break;
	case NOTE_JUDGE_JUST:
		RecJudgeSetUserpal(
			userpal,      true, false,     2,       1,       2,      1);
		break;
	case NOTE_JUDGE_GOOD:
		RecJudgeSetUserpal(
			userpal,      true, false,     1,       0,       1,      1);
		break;
	case NOTE_JUDGE_SAFE:
		RecJudgeSetUserpal(
			userpal,      true, false,     0,      -1,       0,      0);
		break;
	case NOTE_JUDGE_MISS:
		RecJudgeSetUserpal(
			userpal,     false, false,   -20,    -100,       0,     -1);
		break;
	}

	return;
}

typedef struct rec_judge_event_s {
	note_material mat = NOTE_NONE;
	int gaptime = 0;
	note_box_2_t noteinfo;
	note_judge judge;
	int lineNo;
} rec_judge_event_st;

/* TODO: 目標の関数の形: RecNoteJudgeEventAll(判定, ノーツの種類); */
static void RecNoteJudgeEventAll(
	std::queue<rec_judge_event_st> &event_queue,
	note_judge judge, cvec<note_box_2_t> note[], int lineNo
) {
	if (judge == NOTE_JUDGE_NONE) { return; }
	const note_box_2_t *noteinfo = &note[lineNo].nowData();
	event_queue.push({noteinfo->object, noteinfo->hittime - s_Ntime, *noteinfo, judge, lineNo});
	note[lineNo].stepNo();
	return;
}

#endif /* RecNoteJudgeEventAll */

#if 1 /* RecJudge */

static void RecJudgeHitNote(
	std::queue<rec_judge_event_st> &event_queue, cvec<note_box_2_t> note[],
	rec_hitatk_event_ec &hitatk, int push_key_count, rec_play_sound_c *p_sound)
{
	int near_lane = -1;
	char hitflag = 0; //hit event, bit unit: 0: upper hit, 1: middle hit, 2: lower hit, 3~7: reserved
	int gap = 0;
	note_judge NJ = NOTE_JUDGE_PJUST;
	
	for (int iPush = 0; iPush < push_key_count; iPush++) {
		near_lane = RecPlayFindNearHitNote(note);
		if (near_lane == -1) {
			if (iPush == 0) {
				p_sound->PlaySwing();
			}
			break;
		}
		gap = note[near_lane].nowData().hittime - s_Ntime;
		hitflag |= (1 << near_lane);
		NJ = CheckJudge(gap);
		if (NJ == NOTE_JUDGE_MISS) {
			p_sound->PlaySwing();
		}
		RecNoteJudgeEventAll(event_queue, NJ, note, near_lane);
	}
	SetHitPosByHit(hitatk, hitflag);
}

static void RecJudgeArrowNote(
	std::queue<rec_judge_event_st> &event_queue, cvec<note_box_2_t> note[], key_hold_t *keyhold
) {
	int avoidFg[3] = { 0,0,0 };
	note_box_2_t buf[3];

	buf[0] = note[0].nowData();
	buf[1] = note[1].nowData();
	buf[2] = note[2].nowData();

	/* アローのAVOID_ARROW_DIV_TIME以上のズレを検出する */
	if (IS_NOTE_ARROW_GROUP(buf[0].object)) {
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

	if (IS_NOTE_ARROW_GROUP(buf[1].object)) {
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

		GapTime = note[iLine].nowData().hittime - s_Ntime;
		NJ = CheckJudge(GapTime);
		if (NJ == NOTE_JUDGE_NONE) { continue; }

		if (CheckArrowInJudge(note[iLine].nowData().object, keyhold)) {
			RecNoteJudgeEventAll(event_queue, NJ, note, iLine);
		}
	}
	return;
}

static void RecJudgeCatchNote(
	std::queue<rec_judge_event_st> &event_queue, cvec<note_box_2_t> note[],
	rec_hitatk_event_ec &hitatk, int *charahit, int LaneTrack[])
{
	int GapTime = 0;

	for (int iLine = 0; iLine < 3; iLine++) {
		while (!note[iLine].isEndNo() &&
			IS_BETWEEN(0, note[iLine].nowData().hittime, LaneTrack[iLine] + SAFE_TIME) &&
			IS_BETWEEN(0, note[iLine].nowData().hittime, s_Ntime) &&
			note[iLine].nowData().object == NOTE_CATCH)
		{
			RecNoteJudgeEventAll(event_queue, NOTE_JUDGE_PJUST, note, iLine);
			*charahit = 0;
			hitatk = REC_HITATK_EVENT_RESET;
		}
	}
	return;
}

static void RecJudgeBombNote(
	std::queue<rec_judge_event_st> &event_queue, cvec<note_box_2_t> note[],
	rec_hitatk_event_ec &hitatk, int *charahit, short charaput)
{
	int GapTime = 0;

	for (int iLine = 0; iLine < 3; iLine++) {
		while (IS_BETWEEN(0, note[iLine].nowData().hittime, s_Ntime) &&
			note[iLine].nowData().object == NOTE_BOMB)
		{
			GapTime = note[iLine].nowData().hittime - s_Ntime;
			note_judge b_judge =
				(iLine == charaput && GapTime <= 0) ? NOTE_JUDGE_MISS : NOTE_JUDGE_PJUST;
			RecNoteJudgeEventAll(event_queue, b_judge, note, iLine);
			*charahit = 0;
			hitatk = REC_HITATK_EVENT_RESET;
		}
	}
	return;
}

static void RecJudgeGhostNote(cvec<note_box_2_t> note[], rec_play_sound_c *p_sound, const std::vector<DxSnd_t> &soundItem)
{
	for (int iLine = 0; iLine < 3; iLine++) {
		while (IS_BETWEEN(0, note[iLine].nowData().hittime, s_Ntime) &&
			note[iLine].nowData().object == NOTE_GHOST)
		{
			PlayNoteHitSound(&note[iLine].nowData(), p_sound, soundItem);
			note[iLine].stepNo();
		}
	}
	return;
}

//HIT/CATCH/ARROWノーツslowmiss
static void RecJudgeSlowMiss(
	std::queue<rec_judge_event_st> &event_queue, cvec<note_box_2_t> note[]
) {
	int GapTime = 0;

	for (int iLine = 0; iLine < 3; iLine++) {
		GapTime = note[iLine].nowData().hittime - s_Ntime;
		while (
			IS_BETWEEN(-1000000, GapTime, -(SAFE_TIME)) &&
			IS_BETWEEN(NOTE_HIT, note[iLine].nowData().object, NOTE_RIGHT))
		{
			RecNoteJudgeEventAll(event_queue, NOTE_JUDGE_MISS, note, iLine);
			GapTime = note[iLine].nowData().hittime - s_Ntime;
		}
	}
	return;
}

void RecJudgeAllNotes(cvec<note_box_2_t> note[], int Ntime, const std::vector<DxSnd_t> &Sitem,
	key_hold_t *keyhold, rec_hitatk_event_ec &hitatk, int LaneTrack[], int *charahit, short charaput,
	userpal_t *userpal, rec_play_sound_c *p_sound)
{
	int push_key_count = RecLPlayGetHitKeyPushCount(keyhold);
	std::queue<rec_judge_event_st> event_queue;
	s_Ntime = Ntime;
	
	RecJudgeHitNote(  event_queue, note, hitatk, push_key_count, p_sound);
	RecJudgeArrowNote(event_queue, note, keyhold);
	RecJudgeCatchNote(event_queue, note, hitatk, charahit, LaneTrack);
	RecJudgeBombNote( event_queue, note, hitatk, charahit, charaput);
	RecJudgeGhostNote(             note, p_sound, Sitem);
	RecJudgeSlowMiss( event_queue, note);
	while (!event_queue.empty()) {
		/* 判定表示のセット */
		PlaySetJudge(event_queue.front().judge);
		/* エフェクトのセット */
		PlaySetHitEffect(event_queue.front().judge, event_queue.front().mat, event_queue.front().lineNo);
		/* ユーザーデータの更新 */
		note_judge_event(event_queue.front().judge, userpal);
		/* 効果音 */
		PlayNoteHitSoundGeneral(event_queue.front().judge, &event_queue.front().noteinfo, p_sound, Sitem);
		/* gapのデータ(ズレの平均と偏差値)を更新する */
		if (IS_NOTE_HITTING_GROUP(event_queue.front().mat)) {
			if ((-SAFE_TIME) <= event_queue.front().gaptime) {
				userpal->gap.add(event_queue.front().gaptime);
			}
		}
		event_queue.pop();
	}
	return;
}

#endif /* RecJudge */
