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

typedef enum rec_save_error_e {
	REC_SAVE_ERROR_NONE,
	REC_SAVE_ERROR_OPEN,
	REC_SAVE_ERROR_READ,
	REC_SAVE_ERROR_WRITE,
	REC_SAVE_ERROR_CLOSE,
	REC_SAVE_ERROR_OTHER
} rec_save_error_et;

typedef struct rec_score_savefile_s {
	int score  = 0;
	int dist   = 0;
	double acc = 0;
	rec_clear_rank_t clearRank = REC_CLEAR_RANK_NO_PLAY;
	rec_score_rate_t scoreRate = REC_SCORE_RATE_NO_PLAY;
} rec_save_score_t;

typedef struct rec_score_savefile2_s {
	int score  = 0;
	int dist   = 0;
	double acc = 0;
	rec_clear_type2_et clearType = REC_CLEAR_TYPE2_NO_PLAY;
	rec_score_rate2_et scoreRate = REC_SCORE_RATE2_NO_PLAY;
	int good   = 3000; /* 最低のgood以下の数。good+safe+miss */
	int safe   = 3000; /* 最低のsafe以下の数。safe+miss */
	int miss   = 3000; /* 最低のmiss数 */
} rec_save_score2_st;

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

extern rec_save_error_et RecSaveReadScore2AllDif(rec_save_score2_st (&dest)[6], const tstring &songname);
extern rec_save_error_et RecSaveWriteScore2AllDif(const rec_save_score2_st (&src)[6], const tstring &songname);
extern rec_save_error_et RecSaveReadScore2OneDif(rec_save_score2_st &dest, const tstring &songname, rec_dif_t dif);
extern rec_save_error_et RecSaveWriteScore2OneDif(const rec_save_score2_st &src, const tstring &songname, rec_dif_t dif);
extern rec_save_error_et RecSaveUpdateScore2OneDif(const rec_save_score2_st &src, const tstring &songname, rec_dif_t dif);

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
extern int RecSaveUpdateCharaPlay(rec_chara_et nowChara);

extern int RecSaveReadRunnerRate(play_rate_t *data);
extern int RecSaveWriteRunnerRate(const play_rate_t *src);
extern int RecSaveUpdateRunnerRate(const TCHAR *songname, double rate);
extern double RecSaveGetFullRunnerRate(void);
