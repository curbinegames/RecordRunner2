#pragma once

#include <stdio.h>
#include <playbox.h>

typedef struct rec_play_nameset_s {
	wchar_t sky[255] = L"picture/backskynoamal.png";
	wchar_t ground[255] = L"picture/groundnaturenormal.png";
	wchar_t water[255] = L"picture/waternormal.png";
	wchar_t mp3FN[255] = L"song/";
	wchar_t songN[255];
	wchar_t songNE[255];
	wchar_t DifFN[255] = L"picture/difanother.png";
} rec_play_nameset_t;

typedef struct rec_map_detail_s {
	double bpm = 120;
	short int Lv = 0;
#if SWITCH_NOTE_BOX_2 == 1
	note_box_2_t note[6000];
#else
	note_lane_t note2;
#endif
	short int notes = 0;
	int mdif = 0;
	int ldif = 0;
	int ddif[25] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//各区間の難易度
	int ddifG[2] = { 1,25 };//0=今いる区間番号(1～25),1=最大値
} rec_map_detail_t;

#if 1 /* rec_map_eff_data_t */

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

typedef struct rec_map_eff_data_s {
	struct camera_box camera[255];
	struct scrool_box scrool[99];
	item_box Movie[999];
	rec_view_bpm_set_t v_BPM;
	int viewT[2][99];//[音符表示時間,実行時間,[0]=現ナンバー]
	rec_chara_arrow_t carrow;
	int lock[2][2][99]; //lock = [横,縦]の音符の位置を[(1=固定する,-1以外=固定しない),時間]
	rec_move_all_set_t move;
	rec_chara_gra_data_t chamo;
	rec_fall_data_t fall;
	double speedt[5][99][2]; //[上, 中, 下, (地面), (水中)]レーンの[0:切り替え時間,1:速度]
} rec_map_eff_data_t;

#endif /* rec_map_eff_data_t */

typedef struct rec_paly_time_set_s {
	int now;
	int end;
	int offset;
} rec_play_time_set_t;

typedef struct rec_score_file_s {
	playnum_box allnum;
	rec_play_nameset_t nameset;
	rec_map_detail_t mapdata;
	rec_map_eff_data_t mapeff;
	rec_play_time_set_t time; /* ? */
	int outpoint[2] = { -1,0 };
} rec_score_file_t;

extern int rec_score_fread(rec_score_file_t *recfp, const TCHAR *path);
