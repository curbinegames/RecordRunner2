#pragma once

#include <stdio.h>
#include <sancur.h>
#include <system.h>

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
#define IS_NOTE_ARROW_GROUP(mat) ((mat) == NOTE_UP || (mat) == NOTE_DOWN || (mat) == NOTE_LEFT || (mat) == NOTE_RIGHT)

typedef enum note_lane_num_e {
	NOTE_LANE_UP = 0,
	NOTE_LANE_MID,
	NOTE_LANE_LOW,
	NOTE_LANE_UM,
	NOTE_LANE_ML,
} note_lane_num_t;

typedef struct playnum_box {
	unsigned int notenum[3] = { 0,0,0 };
	unsigned int Ymovenum[5] = { 1,1,1,1,1 };
	unsigned int Xmovenum[3] = { 1,1,1 };
	unsigned int movienum = 0;
	unsigned int v_BPMnum = 1;
} playnum_box;

typedef struct rec_play_nameset_s {
	wchar_t sky[255] = L"picture/backskynoamal.png";
	wchar_t ground[255] = L"picture/groundnaturenormal.png";
	wchar_t water[255] = L"picture/waternormal.png";
	wchar_t mp3FN[255] = L"song/";
	wchar_t songN[255];
	wchar_t songNE[255];
	wchar_t DifFN[255] = L"picture/difanother.png";
} rec_play_nameset_t;

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

typedef struct rec_ddif_pal_s {
	uint notes = 0; //HIT,ARROWの密度     青
	uint arrow = 0; //ARROWの密度         赤
	uint chord = 0; //同時押しの密度      緑
	uint chain = 0; //縦連密度            紫
	uint trill = 0; //トリルの密度        黄色
	uint meldy = 0; //乱打密度            ピンク
	uint actor = 0; //CATCH,BOMBの密度    オレンジ
	uint trick = 0; //ARROWひっかけの密度 茶色
	intx100_t mdif = 0; //全体の難易度
} rec_ddif_pal_t;

typedef struct rec_map_detail_s {
	double bpm = 120;
	short int Lv = 0;
	note_box_2_t note[6000];
	short int notes = 0;
	intx100_t mdif = 0;
	int ldif = 0;
	int ddif[25] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//各区間の難易度
	int ddifG[2] = { 1,25 };//0=今いる区間番号(1～25),1=最大値
	rec_ddif_pal_t mpal;//新ddif
} rec_map_detail_t;

#if 1 /* rec_map_eff_data_t */

typedef struct rec_camera_data_s {
	int starttime = -1;
	int endtime = -1;
	int xpos = 0;
	int ypos = 0;
	double zoom = 1.0;
	int rot = 0;
	int mode = 0;
} rec_camera_data_t;

typedef struct rec_camera_set_s {
	rec_camera_data_t data[256];
	uint num = 0;
} rec_camera_set_t;

typedef struct rec_scrool_data_s {
	int starttime = -1;
	double basetime = -1;
	double speed = -1;
} rec_scrool_data_t;

typedef struct rec_scrool_set_s {
	rec_scrool_data_t data[99];
	uint num = 0;
} rec_scrool_set_t;

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

typedef struct view_BPM_box {
	int time = -1000;
	unsigned short BPM = 1;
} view_BPM_box;

typedef struct rec_view_bpm_set_s {
	view_BPM_box data[100];
	int num = 0;
} rec_view_bpm_set_t;

typedef struct rec_chara_arrow_s {
	struct {
		int data = 0;
		int time = -10000;
	} d[99];
	int num = 0;
} rec_chara_arrow_t;

typedef struct rec_move_data_s {
	int Stime = -10000;
	int pos = -10000;
	int Etime = -10000;
	int mode = -10000;
} rec_move_data_t;

typedef struct rec_move_set_s {
	rec_move_data_t d[999];
	int num = 0;
} rec_move_set_t;

typedef struct rec_move_all_set_s {
	rec_move_set_t x[3];
	rec_move_set_t y[5];
} rec_move_all_set_t;

typedef struct rec_chara_gra_data_s {
	int gra[99];
	int time[99];
	int num = 0;
} rec_chara_gra_data_t[3];

typedef struct rec_fall_data_s {
	struct {
		int No = -10000;
		int time = -10000;
	} d[99];
	int num = 0;
} rec_fall_data_t;

typedef struct rec_viewline_data_s {
	int time = -1;
	bool enable = true;
} rec_viewline_data_t;

typedef struct rec_viewline_dataset_s {
	rec_viewline_data_t d[99];
	uint num = 0;
} rec_viewline_dataset_t;

typedef struct rec_mapeff_speedt_data_s {
	int time = -1;
	double speed = 1.0;
} rec_mapeff_speedt_data_t;

typedef struct rec_mapeff_speedt_dataset_s {
	rec_mapeff_speedt_data_t d[99];
	int num = 0;
} rec_mapeff_speedt_dataset_t;

typedef struct rec_map_eff_data_s {
	rec_camera_set_t camera;
	rec_scrool_set_t scrool;
	item_box Movie[999];
	rec_view_bpm_set_t v_BPM;
	int viewT[2][99];//[音符表示時間,実行時間,[0]=現ナンバー]
	rec_chara_arrow_t carrow;
	int lock[2][2][99]; //lock = [横,縦]の音符の位置を[(1=固定する,-1以外=固定しない),時間]
	rec_move_all_set_t move;
	rec_chara_gra_data_t chamo;
	rec_fall_data_t fall;
	rec_viewline_dataset_t viewLine;
	rec_mapeff_speedt_dataset_t speedt[5];
} rec_map_eff_data_t;

#endif /* rec_map_eff_data_t */

typedef struct rec_paly_time_set_s {
	int now = 0;
	int end = 0;
	int offset = 0;
} rec_play_time_set_t;

typedef struct rec_score_file_s {
	playnum_box allnum;
	rec_play_nameset_t nameset;
	rec_map_detail_t mapdata;
	rec_map_eff_data_t mapeff;
	rec_play_time_set_t time; /* ? */
	int outpoint[2] = { -1,0 };
} rec_score_file_t;

typedef struct rec_score_file_row_s {
	playnum_box allnum;
	rec_map_detail_t mapdata;
} rec_score_file_row_t;

extern int rec_score_fread(rec_score_file_t *recfp, const TCHAR *path);
extern int rec_score_fwrite(rec_score_file_t *recfp, const TCHAR *path);
extern int RecScoreReadForDdif(rec_score_file_row_t *recfp, const TCHAR *path);
extern int RecScoreReadSongName(TCHAR *songName, const TCHAR *path);
extern int RecScoreReadDdif(rec_ddif_pal_t *ddif, const TCHAR *path);
extern int RecScoreWriteDdif(rec_ddif_pal_t *ddif, const TCHAR *path);
