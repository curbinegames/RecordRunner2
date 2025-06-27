#pragma once

/**
 * mapenc,play,result‚ÅŽg‚¤
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

#if 1 /*struct */

typedef struct judge_box {
	int pjust = 0;
	int just = 0;
	int good = 0;
	int safe = 0;
	int miss = 0;
} rec_play_judge_t;

typedef struct rec_play_score_s {
	int normal = 0;
	int combo = 0;
	int loss = 0;
	int sum = 0;
	int pjust = 0;
	int before = 0;
	int time = 0;
} rec_play_score_t;

typedef struct rec_play_xy_set_s {
	int x;
	int y;
} rec_play_xy_set_t;

/* <=-1: just release, 0: no push, 1: just push, 2<=: hold */
typedef struct rec_play_key_hold_s {
	int z = 0;
	int x = 0;
	int c = 0;
	int up = 0;
	int down = 0;
	int left = 0;
	int right = 0;
} rec_play_key_hold_t;
#define IS_PUSH_UPDOWN(keyhold)            ( ((keyhold)->up >= 1) && ((keyhold)->down >= 1) )
#define IS_PUSH_ANY_ARROWKEY(keyhold)      ( ((keyhold)->up >= 1) || ((keyhold)->down >= 1) || ((keyhold)->left >= 1) || ((keyhold)->right >= 1) )
#define IS_JUST_PUSH_ANY_ARROWKEY(keyhold) ( ((keyhold)->up == 1) || ((keyhold)->down == 1) || ((keyhold)->left == 1) || ((keyhold)->right == 1) )
#define IS_JUST_PUSH_ANY_HITKEY(keyhold)   ( ((keyhold)->z  == 1) || ((keyhold)->x    == 1) || ((keyhold)->c    == 1) )
#define IS_CHARAUP_KEY(keyhold)            ( ((keyhold)->up >= 1) && ((keyhold)->down == 0) )
#define IS_CHARAMID_KEY(keyhold)           ( ((keyhold)->up == 0) && ((keyhold)->down == 0) || ((keyhold)->up >= 1) && ((keyhold)->down >= 1) )
#define IS_CHARALOW_KEY(keyhold)           ( ((keyhold)->up == 0) && ((keyhold)->down >= 1) )

typedef struct distance_score_s {
	int add = 0;
	int add_save = 0;
	int now_dis = 0;
	int dis_save = 0;
	int point = 0;
} distance_score_t;

typedef struct rec_play_chara_hit_attack_s {
	int pos = 1;
	int time = -1000;
} rec_play_chara_hit_attack_t;

typedef struct rec_play_userpal_s {
	rec_play_score_t score;
	distance_score_t Dscore;
	rec_play_status_t status = REC_PLAY_STATUS_PLAYING;
	int Ncombo = 0;
	int Mcombo = 0;
	rec_play_judge_t judgeCount;
	int life = 500;
	int Exlife = 500;
	cur_deviation_c gap;
} rec_play_userpal_t;

typedef struct rec_play_lanepos_s {
	int x[3] = { 150,150,150 };
	int y[5] = { 300,350,400,350,600 };
} rec_play_lanepos_t;

#endif /*struct */

class rec_play_sound_c {
private:
	dxcur_snd_c att;
	dxcur_snd_c cat;
	dxcur_snd_c arw;
	dxcur_snd_c bom;
	dxcur_snd_c swi;

public:

	rec_play_sound_c() {
		this->att.SetSound(_T("sound/attack.wav"));
		this->cat.SetSound(_T("sound/catch.wav"));
		this->arw.SetSound(_T("sound/arrow.wav"));
		this->bom.SetSound(_T("sound/bomb.wav"));
		this->swi.SetSound(_T("sound/swing.wav"));
		this->att.SetVolume(optiondata.SEvolume * 255 / 10);
		this->cat.SetVolume(optiondata.SEvolume * 255 / 10);
		this->arw.SetVolume(optiondata.SEvolume * 255 / 10);
		this->bom.SetVolume(optiondata.SEvolume * 255 / 10);
		this->swi.SetVolume(optiondata.SEvolume * 255 / 10);
	}

	~rec_play_sound_c() {}

	void PlayNoteSound(note_material mat) {
		switch (mat) {
		case NOTE_HIT:
			this->att.PlaySound();
			break;
		case NOTE_CATCH:
			this->cat.PlaySound();
			break;
		case NOTE_UP:
		case NOTE_DOWN:
		case NOTE_LEFT:
		case NOTE_RIGHT:
			this->arw.PlaySound();
			break;
		case NOTE_BOMB:
			this->bom.PlaySound();
			break;
		}
	}

	void PlaySwing() {
		this->swi.PlaySound();
	}
};
