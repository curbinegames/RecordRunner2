#pragma once

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

struct judge_box {
	int pjust = 0;
	int just = 0;
	int good = 0;
	int safe = 0;
	int miss = 0;
};

typedef struct gap_box_t {
	int view[30] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	int sum = 0;
	int ssum = 0;
	int count = 0;
} gap_box;

struct score_box {
	int normal = 0;
	int combo = 0;
	int loss = 0;
	int sum = 0;
	int pjust = 0;
	int before = 0;
	int time = 0;
};

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
	struct score_box score;
	distance_score_t Dscore;
	rec_play_status_t status = REC_PLAY_STATUS_PLAYING;
	int Ncombo = 0;
	int Mcombo = 0;
	struct judge_box judgeCount;
	int life = 500;
	gap_box gap;
} rec_play_userpal_t;

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
