#pragma once

#include <stdio.h>
#include <sancur.h>
#include <datacur.h>
#include <RecSystem.h>

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

#if 1 /* note_material */

typedef enum note_material {
	NOTE_NONE = -1,
	NOTE_HIT = 1,
	NOTE_CATCH,
	NOTE_UP,
	NOTE_DOWN,
	NOTE_LEFT,
	NOTE_RIGHT,
	NOTE_BOMB,
	NOTE_GHOST,
	NOTE_END
} note_material;
#define IS_NOTE_HITTING_GROUP(mat) ((mat) == NOTE_HIT || (mat) == NOTE_UP || (mat) == NOTE_DOWN || (mat) == NOTE_LEFT || (mat) == NOTE_RIGHT)
#define IS_NOTE_ARROW_GROUP(mat) ((mat) == NOTE_UP || (mat) == NOTE_DOWN || (mat) == NOTE_LEFT || (mat) == NOTE_RIGHT)

/* 指定のノーツがgap[ms]以内にあったらtrue */
#define IS_NEAR_NOTE(notedata, mat, Ntime, gap)                             \
	((notedata)->object == (mat) && (notedata)->hittime - (Ntime) <= (gap))

/* bombノーツが40ms以内にあったらtrue */
#define IS_NEAR_NOTE_BOMB(notedata, Ntime) IS_NEAR_NOTE(notedata, NOTE_BOMB, Ntime, 40)

/* 指定のノーツがgap[ms]以内にどこかのレーンにあったらtrue */
#define IS_NEAR_NOTE_ANYLANE(notedata, mat, Ntime, gap)         \
	(IS_NEAR_NOTE(&(notedata[0]).nowData(), mat, Ntime, gap) || \
	 IS_NEAR_NOTE(&(notedata[1]).nowData(), mat, Ntime, gap) || \
	 IS_NEAR_NOTE(&(notedata[2]).nowData(), mat, Ntime, gap))

/* arrowノーツがgap[ms]以内にどこかのレーンにあったらtrue */
#define IS_NEAR_NOTE_ARROW_ANYLANE(notedata, Ntime, gap)       \
	(IS_NEAR_NOTE_ANYLANE(notedata, NOTE_UP,    Ntime, gap) || \
	 IS_NEAR_NOTE_ANYLANE(notedata, NOTE_DOWN,  Ntime, gap) || \
	 IS_NEAR_NOTE_ANYLANE(notedata, NOTE_LEFT,  Ntime, gap) || \
	 IS_NEAR_NOTE_ANYLANE(notedata, NOTE_RIGHT, Ntime, gap))

/* actorノーツがgap[ms]以内にどこかのレーンにあったらtrue */
#define IS_NEAR_NOTE_ACTOR_ANYLANE(notedata, Ntime, gap)       \
	(IS_NEAR_NOTE_ANYLANE(notedata, NOTE_CATCH, Ntime, gap) || \
	 IS_NEAR_NOTE_ANYLANE(notedata, NOTE_BOMB,  Ntime, gap))

#endif /* note_material */

template<typename T>
using cvec = datacur_cursor_vector<T>;

template<typename T>
using tvec = datacur_timeline_vector<T>;

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
	tstring sky = _T("picture/backskynoamal.png"); /* 空背景のファイル名 */
	tstring ground = _T("picture/groundnaturenormal.png"); /* 地面背景のファイル名 */
	tstring water = _T("picture/waternormal.png"); /* 水中背景のファイル名 */
	tstring mp3FN = _T("song/"); /* 曲のファイル名 */
	tstring songN; /* 曲名 */
	tstring songNE; /* 曲名(英語) */
	tstring DifFN = _T("picture/difanother.png"); /* 第4難易度バー画像のファイル名 */
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

typedef struct rec_ddif_time_s {
	uint notes = 0; //HIT,ARROWの密度     青
	uint arrow = 0; //ARROWの密度         赤
	uint chord = 0; //同時押しの密度      緑
	uint chain = 0; //縦連密度            紫
	uint trill = 0; //トリルの密度        黄色
	uint meldy = 0; //乱打密度            ピンク
	uint actor = 0; //CATCH,BOMBの密度    オレンジ
	uint trick = 0; //ARROWひっかけの密度 茶色
} rec_ddif_time_t;

typedef struct rec_ddif_pal_s {
	uint notes = 0; //HIT,ARROWの密度     青
	uint arrow = 0; //ARROWの密度         赤
	uint chord = 0; //同時押しの密度      緑
	uint chain = 0; //縦連密度            紫
	uint trill = 0; //トリルの密度        黄色
	uint meldy = 0; //乱打密度            ピンク
	uint actor = 0; //CATCH,BOMBの密度    オレンジ
	uint trick = 0; //リズム難の密度      茶色
	intx100_t mdif = 0; //全体の難易度
	rec_ddif_time_t time;
} rec_ddif_pal_t;

typedef struct rec_map_detail_s {
	double bpm = 120;
	short Lv = 0;
	cvec<note_box_2_t> note[3] = { cvec<note_box_2_t>(2000) };
	short notes = 0;
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

typedef struct rec_scrool_data_s {
	double basetime = -1; /* 始点となる時間 */
	double speed = -1; /* 速度 */
} rec_scrool_data_t;

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

typedef struct rec_notelock_allset_s {
	tvec<bool> x{99};
	tvec<bool> y{99};
} rec_notelock_allset_t;

typedef struct rec_mapeff_move_s {
	int Stime = -10000;
	int pos = -10000;
	int Etime = -10000;
	int mode = -10000;
} rec_mapeff_move_st;

typedef struct rec_move_all_set_s {
	cvec<rec_mapeff_move_st> x[3] = { cvec<rec_mapeff_move_st>(999) };
	cvec<rec_mapeff_move_st> y[5] = { cvec<rec_mapeff_move_st>(999) };
} rec_move_all_set_t;

typedef struct rec_map_eff_data_s { /* TODO: 中身の並べ替え */
	cvec<rec_camera_data_t> camera{256}; /* カメラ移動タイミング */
	tvec<rec_scrool_data_t> scrool{99}; /* 譜面のスクロール速度切り替えタイミング */
	cvec<item_box> Movie{999}; /* 画像アイテムリスト */
	tvec<double> v_BPM{99}; /* 表示上のBPM */
	tvec<int> viewT{99}; /* ノーツの表示時間 */
	tvec<bool> carrow{99}; /* キャラの向き */ /* TODO: boolにする */
	rec_notelock_allset_t lock;
	rec_move_all_set_t move;
	tvec<int> chamo[3] = { tvec<int>(99) }; /* キャラグラ変更タイミング */
	tvec<int> fall{99}; /* 落ち物背景アイテム番号 */
	tvec<bool> viewLine{99}; /* レーンガイド表示フラグ */
	tvec<double> speedt[5] = { tvec<double>(99) }; /* ノーツ速度 */
	tvec<note_box_2_t> gnote{999}; /* ゴーストノート */
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
extern int rec_score_fwrite(const rec_score_file_t *recfp, const TCHAR *path);
extern int RecScoreReadForDdif(rec_score_file_row_t *recfp, const TCHAR *path);
extern int RecScoreReadSongName(TCHAR *songName, const TCHAR *path);
extern int RecScoreReadDdif(rec_ddif_pal_t *ddif, const TCHAR *path);
extern int RecScoreWriteDdif(rec_ddif_pal_t *ddif, const TCHAR *path);
