#pragma once

/**
 * mapenc,play,resultで使う
 */

#include <tchar.h>
#include <DxLib.h>
#include <dxcur.h>
#include <sancur.h>

#include <RecScoreFile.h>
#include <option.h>

#define SE_HIT (1 << 0)
#define SE_CATCH (1 << 1)
#define SE_ARROW (1 << 2)
#define SE_BOMB (1 << 3)
#define SE_GHOST (1 << 4)
#define SE_SWING (1 << 5)

typedef enum note_judge {
	NOTE_JUDGE_NONE = -1,
	NOTE_JUDGE_JUST = 0,
	NOTE_JUDGE_GOOD,
	NOTE_JUDGE_SAFE,
	NOTE_JUDGE_MISS,
	NOTE_JUDGE_PJUST
} note_judge;

typedef enum rec_play_status_e {
	REC_PLAY_STATUS_PLAYING = 0,
	REC_PLAY_STATUS_CLEARED,
	REC_PLAY_STATUS_DROPED,
} rec_play_status_t;

typedef enum rec_hitatk_event_e {
	REC_HITATK_EVENT_NONE,  /* 変更なし */
	REC_HITATK_EVENT_RESET, /* 解除 */
	REC_HITATK_EVENT_UP,    /* 上ヒット */
	REC_HITATK_EVENT_MID,   /* 中ヒット */
	REC_HITATK_EVENT_DOWN   /* 下ヒット */
} rec_hitatk_event_ec;

#if 1 /* struct */

typedef struct judge_box {
	int pjust = 0;
	int just = 0;
	int good = 0;
	int safe = 0;
	int miss = 0;
} rec_play_judge_t;

typedef struct rec_play_xy_set_s {
	int x;
	int y;
} rec_play_xy_set_t;

typedef struct rec_play_chara_hit_attack_s {
	int pos = 1;
	int time = -1000;
} rec_play_chara_hit_attack_t;

class rec_play_score_c {
private:
	int normal = 0; /* 判定によるスコア */
	int combo = 0; /* コンボによるスコア */
	int loss = 0; /* 減点分のスコア。総得点がマイナスにならないように調整される */
	int pjust = 0; /* pjustによるスコア。パフェ時のみ総合点に反映 */

	int before = 0;
	int time = 0;

public:
	void SetScoreFlip(int Ntime);
	void SetJudgeScore(const rec_play_judge_t &judge, int note_count);
	void SetComboScore(int max_combo, int note_count);
	void SetPjustScore(int pjust_count, int note_count);
	void SetScore(const rec_play_judge_t &judge, int max_combo, int note_count);
	void AddLoss(int val);
	int GetJudgeScore(void) const;
	int GetAllScore(void) const;
	int GetViewScore(int Ntime) const;
};

class distance_score_c {
private:
	int add = 0; /* ノーツ判定によって加算された距離 */
	int now_dis = 0; /* 時間経過によって走った距離 */
	bool droped = false; /* drop判定。dropしてたら距離の更新を止める。 */

public:
	void AddDist(int val);
	void SetDist(int val);
	int GetDist(void) const;
	double GetAllPoint(void) const;
	void SetDrop(void);
};

class rec_play_life_c {
private:
	int life = 500; /* 今のライフ */
	int loss =   0; /* 減点分 */

	int pjust_point =   2;
	int  just_point =   2;
	int  good_point =   1;
	int  safe_point =   0;
	int  miss_point = -20;

	bool recover    = true; /* 0になった後も回復するか */

	void fixLife(void);
	void addBase(int val);

public:
	rec_play_life_c(void) = default;
	rec_play_life_c(int pjust, int just, int good, int safe, int miss, bool recover) :
		pjust_point(pjust), just_point(just), good_point(good), safe_point(safe), miss_point(miss), recover(recover) {};

	void setPoint(int pjust, int just, int good, int safe, int miss);
	void applyPJust(void);
	void applyJust(void);
	void applyGood(void);
	void applySafe(void);
	void applyMiss(void);
	int getLife(void) const;
	int getLoss(void) const;
	void resetLoss(void);
};

typedef struct rec_play_userpal_s {
	rec_play_score_c score;
	distance_score_c Dscore;
	rec_play_status_t status = REC_PLAY_STATUS_PLAYING;
	int Ncombo = 0;
	int Mcombo = 0;
	rec_play_judge_t judgeCount;
	rec_play_life_c life  { 2, 2, 1,  0,  -20,  true };
	rec_play_life_c Exlife{ 2, 1, 0, -1, -100, false };
	cur_deviation_c gap;
} rec_play_userpal_t;

typedef struct rec_play_lanepos_s {
	int x[3] = { 150,150,150 };
	int y[5] = { 300,350,400,350,600 };
} rec_play_lanepos_t;

#endif /* struct */

/* <=-1: just release, 0: no push, 1: just push, 2<=: hold */
class rec_play_key_hold_c {
public:
	int z = 0;
	int x = 0;
	int c = 0;
	int up = 0;
	int down = 0;
	int left = 0;
	int right = 0;

	int GetHitPushCount(void) const;
	bool IsPushAnyHit(void) const;
	bool IsJustPushAnyHit(void) const;
	bool IsUpAction(void) const;
	bool IsDownAction(void) const;
	bool IsMidAction(void) const;
	bool IsNoneAction(void) const;
	bool IsPushAnyArrow(void) const;
	bool IsJustPushAnyArrow(void) const;
};
