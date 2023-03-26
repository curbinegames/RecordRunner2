
#ifndef PLAY_BOX

#define PLAY_BOX 1

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
struct note_box {
	int hittime = -1;
	int viewtime = -1;
	int object = NOTE_NONE;
	int xpos = -1;
	int ypos = -1;
	int sound = 0;
	enum melodysound melody = MELODYSOUND_NONE;
	int color = 0;
};
typedef struct play_sound_s {
	int att;
	int cat;
	int arw;
	int bom;
	int swi;
	char flag = 0;//reserved, reserved, reserved, reserved, bomb, arrow, catch, hit
} play_sound_t;
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

typedef struct judge_action_box_t {
	int* combo;
	gap_box* gap;
	struct judge_box* judge;
	int* life;
	int* melody_snd;
	play_sound_t* p_sound;
	struct score_box* score;
} judge_action_box;

#endif // PLAY_BOX
