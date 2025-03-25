#pragma once

#include <stdio.h>
#include <strcur.h>
#include <RecSystem.h>
#include <playbox.h>
#include <option.h>

#define OLD_RATE_NUM 10
#define OLD_RATE_FILE_NAME_S L"save/rateS.dat" /* 名前 */
#define OLD_RATE_FILE_NAME_N L"save/rateN.dat" /* 数値 */

#define RATE_NUM 20
#define RATE_FILE_NAME L"save/Prate.dat" /* 名前+数値 */

typedef struct rec_score_savefile_s {
	int score  = 0;
	int dist   = 0;
	double acc = 0;
	rec_clear_rank_t clearRank = REC_CLEAR_RANK_NO_PLAY;
	rec_score_rate_t scoreRate = REC_SCORE_RATE_NO_PLAY;
} rec_save_score_t;

typedef struct rec_user_data_s {
	int playCount = 0;
	int dropCount = 0;
	int a; /* ??? */
	int clearCount = 0;
	int NMCount = 0;
	int FCCount = 0;
	int PFcount = 0;
	int mileage = 0;
} rec_user_data_t;

typedef struct rec_save_charaplay_s {
	int picker   = 0;
	int mapgator = 0;
	int taylor   = 0;
} rec_save_charaplay_t;

typedef struct play_rate_s {
	wchar_t name[255] = L"\0";
	double num = 0;
} play_rate_t;

extern int RecSaveReadScoreAllDif(rec_save_score_t dest[], const TCHAR *songname);
extern int RecSaveWriteScoreAllDif(const rec_save_score_t src[], const TCHAR *songname);
extern int RecSaveReadScoreOneDif(rec_save_score_t *dest, const TCHAR *songname, rec_dif_t dif);
extern int RecSaveWriteScoreOneDif(const rec_save_score_t *src, const TCHAR *songname, rec_dif_t dif);
extern int RecSaveUpdateScoreOneDif(const rec_save_score_t *src, const TCHAR *songname, rec_dif_t dif);

extern int RecSaveReadUserPlay(rec_user_data_t *dest);
extern int RecSaveWriteUserPlay(const rec_user_data_t *src);
extern int RecSaveUpdateUserPlay(const rec_play_userpal_t *userpal);

extern int RecSaveReadCharaPlay(rec_save_charaplay_t *dest);
extern int RecSaveWriteCharaPlay(const rec_save_charaplay_t *src);
extern int RecSaveUpdateCharaPlay(rec_nowchara_t nowChara);

extern int RecSaveReadRunnerRate(play_rate_t *data);
extern int RecSaveWriteRunnerRate(const play_rate_t *src);
extern int RecSaveUpdateRunnerRate(const TCHAR *songname, double rate);
extern double RecSaveGetFullRunnerRate(void);
