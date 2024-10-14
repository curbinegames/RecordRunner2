#pragma once

#include <tchar.h>
#include <DxLib.h>
#include "../general/dxcur.h"

#define SE_HIT (1 << 0)
#define SE_CATCH (1 << 1)
#define SE_ARROW (1 << 2)
#define SE_BOMB (1 << 3)
#define SE_GHOST (1 << 4)
#define SE_SWING (1 << 5)

typedef enum note_lane_num_e {
	NOTE_LANE_UP = 0,
	NOTE_LANE_MID,
	NOTE_LANE_LOW,
	NOTE_LANE_UM,
	NOTE_LANE_ML,
} note_lane_num_t;

typedef enum note_judge {
	NOTE_JUDGE_NONE = -1,
	NOTE_JUDGE_JUST = 0,
	NOTE_JUDGE_GOOD,
	NOTE_JUDGE_SAFE,
	NOTE_JUDGE_MISS
} note_judge;

typedef enum note_material {
	NOTE_NONE = -1,
	NOTE_HIT = 1,
	NOTE_CATCH,
	NOTE_UP,
	NOTE_DOWN,
	NOTE_LEFT,
	NOTE_RIGHT,
	NOTE_BOMB,
	NOTE_GHOST
} note_material;

enum maperror_enum {
	HITNOTETOONEAR = 1,
};
enum melodysound {
	MELODYSOUND_NONE = -1,
	LOW_F,
	LOW_Fs,
	LOW_G,
	LOW_Gs,
	LOW_A,
	LOW_As,
	LOW_B,
	LOW_C,
	LOW_Cs,
	LOW_D,
	LOW_Ds,
	LOW_E,
	HIGH_F,
	HIGH_Fs,
	HIGH_G,
	HIGH_Gs,
	HIGH_A,
	HIGH_As,
	HIGH_B,
	HIGH_C,
	HIGH_Cs,
	HIGH_D,
	HIGH_Ds,
	HIGH_E
};

typedef enum rec_play_status_e {
	REC_PLAY_STATUS_PLAYING = 0,
	REC_PLAY_STATUS_CLEARED,
	REC_PLAY_STATUS_DROPED,
} rec_play_status_t;

struct camera_box {
	int starttime = -1;
	int endtime = -1;
	int xpos = -1;
	int ypos = -1;
	double zoom = -1;
	int rot = -1;
	int mode = -1;
};
struct custom_note_box {
	wchar_t note = L'\0';
	int color = 0;/*(only hit note)0=green, 1=red, 2=blue, 3=yellow, 4=black, 5=white*/
	int sound = 0;
	enum melodysound melody = MELODYSOUND_NONE;
};

struct judge_box {
	int pjust = 0;
	int just = 0;
	int good = 0;
	int safe = 0;
	int miss = 0;
};

struct note_img {
	int notebase = LoadGraph(L"picture/hit.png");
	int hitcircle[6] = {
		LoadGraph(L"picture/hitc-G.png"),
		LoadGraph(L"picture/hitc-R.png"),
		LoadGraph(L"picture/hitc-B.png"),
		LoadGraph(L"picture/hitc-Y.png"),
		LoadGraph(L"picture/hitc-X.png"),
		LoadGraph(L"picture/hitc-W.png"),
	};
	int catchi = LoadGraph(L"picture/catch.png");
	int up = LoadGraph(L"picture/up.png");
	int down = LoadGraph(L"picture/down.png");
	int left = LoadGraph(L"picture/left.png");
	int right = LoadGraph(L"picture/right.png");
	int bomb = LoadGraph(L"picture/bomb.png");
	int goust = LoadGraph(L"picture/goust.png");
};

typedef struct note_box {
	int hittime = -1;
	int viewtime = -1;
	note_material object = NOTE_NONE;
	int xpos = -1;
	int ypos = -1;
	int sound = 0;
	enum melodysound melody = MELODYSOUND_NONE;
	int color = 0;
} note_box_t;

typedef struct note_box_2_s {
	int hittime = -1;
	int viewtime = -1;
	note_material object = NOTE_NONE;
	note_lane_num_t lane = NOTE_LANE_MID;
	int xpos = -1;
	int ypos = -1;
	int sound = 0;
	enum melodysound melody = MELODYSOUND_NONE;
	int color = 0;
	int next = 5999;
} note_box_2_t;

typedef struct note_lane_s {
	note_box_t up[2000];
	note_box_t mid[2000];
	note_box_t low[2000];
} note_lane_t;

typedef struct play_key_stat_s {
	char z = 0;
	char x = 0;
	char c = 0;
	char up = 0;
	char down = 0;
	char left = 0;
	char right = 0;
} play_key_stat_t;
typedef struct playnum_box {
	unsigned int notenum[3] = { 0,0,0 };
	unsigned int Ymovenum[5] = { 1,1,1,1,1 };
	unsigned int Xmovenum[3] = { 1,1,1 };
	unsigned int movienum = 0;
	unsigned int v_BPMnum = 1;
} playnum_box;
typedef struct item_eff_box {
	unsigned char bpm_alphr = 0;
	unsigned char bpm_size = 0;
	unsigned char edge_size = 0;
	unsigned char lock = 0;
	unsigned char chara_alphr = 0;
} item_eff_box;
typedef struct item_box {
	short ID = -1;
	char movemode = 0;
	item_eff_box eff;
	int starttime = -1000;
	int endtime = -1000;
	int startXpos = 0;
	int endXpos = 0;
	int startYpos = 0;
	int endYpos = 0;
	int startsize = 100;
	int endsize = 100;
	int startrot = 0;
	int endrot = 0;
	int startalpha = 255;
	int endalpha = 255;
} item_box;
typedef struct item_set_ID {
	short picID = -1;
	item_eff_box eff;
	int Xpos = 0;
	int Ypos = 0;
	int size = 100;
	int rot = 0;
	int alpha = 255;
} item_set_ID;
typedef struct item_set_box {
	item_set_ID picID[10];
	unsigned char num = 0;
	char movemode = 0;
	int starttime = -1000;
	int endtime = -1000;
	int startXpos = 0;
	int endXpos = 0;
	int startYpos = 0;
	int endYpos = 0;
	int startsize = 100;
	int endsize = 100;
	int startrot = 0;
	int endrot = 0;
	int startalpha = 255;
	int endalpha = 255;
} item_set_box;
typedef struct view_BPM_box {
	int time = -1000;
	unsigned short BPM = 1;
} view_BPM_box;
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

struct scrool_box {
	int starttime = -1;
	double basetime = -1;
	double speed = -1;
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

typedef struct judge_action_box_t {
	int *combo;
	int *soundEnFg;
	gap_box *gap;
	struct judge_box *judge;
	int *life;
	int *melody_snd;
	rec_play_sound_c *p_sound;
	struct score_box *score;
} judge_action_box;
