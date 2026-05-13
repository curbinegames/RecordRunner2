#pragma once

#include <DxLib.h>
#include <sancur.h>
#include <stdcur.h>
#include <datacur.h>
#include <recr_cutin.h>

#define REC_DEBUG 0 // 0or1 1でデバッグ用コードがビルドされる

#define WINDOW_SIZE_X 1280                     // ウィンドウの横のサイズ
#define WINDOW_SIZE_Y (WINDOW_SIZE_X * 9 / 16) // ウィンドウの縦のサイズ 720

#define OLD_WINDOW_SIZE_X 640                         // 前バージョンのウィンドウの横のサイズ
#define OLD_WINDOW_SIZE_Y (OLD_WINDOW_SIZE_X * 3 / 4) // 前バージョンのウィンドウの縦のサイズ 480

#define WAIT_TIME_AFTER_MUSICPLAY 30 /* 音楽再生直後のウェイト時間 */
#define WAIT_TIME_ON_GAMELOOP      1 /* ゲーム処理ループ中のウェイト時間 */

#define INIT_SND() InitSoundMem()

/* debug */
#if 0
#define RECR_DEBUG(ofs, data)											\
		RecRescaleDrawFormatString(20, 120 + ofs * 20, Cr, L#data": %d", data)
#define RECR_DEBUG_LOOP(ofs, n, data_a, data_b)							\
	for (int _rep = 0; _rep < n; _rep++) {								\
		RecRescaleDrawFormatString(20, 120 + _rep * 20 + ofs * 20, Cr,			\
		L#data_a"[%d]"#data_b": %d", _rep, data_a[_rep]data_b);			\
	}
#else
#define RECR_DEBUG(n, data_a, data_b)
#define RECR_DEBUG_LOOP(n, data_a, data_b)
#endif

template<typename T>
using cvec = datacur_cursor_vector<T>;

template<typename T>
using tvec = datacur_timeline_vector<T>;

#if 1 /* enum */

typedef enum rec_chara_e {
	REC_CHARA_PICKER,
	REC_CHARA_GATOR,
	REC_CHARA_TAYLOR
} rec_chara_et;

typedef enum rec_play_rank_e {
	REC_SCORE_RATE_EX = 0,
	REC_SCORE_RATE_S,
	REC_SCORE_RATE_A,
	REC_SCORE_RATE_B,
	REC_SCORE_RATE_C,
	REC_SCORE_RATE_D,
	REC_SCORE_RATE_NO_PLAY,
	REC_SCORE_RATE_F,
} rec_score_rate_t;

typedef enum rec_play_rank2_e {
	REC_SCORE_RATE2_NO_PLAY,
	REC_SCORE_RATE2_F,
	REC_SCORE_RATE2_D,
	REC_SCORE_RATE2_C,
	REC_SCORE_RATE2_B,
	REC_SCORE_RATE2_A,
	REC_SCORE_RATE2_S,
	REC_SCORE_RATE2_X,
} rec_score_rate2_et;

typedef enum now_scene_e {
	SCENE_TITLE = 0,
	SCENE_MENU,
	SCENE_SERECT,
	SCENE_COLLECTION,
	SCENE_OPTION,
	SCENE_EXIT = 5,
	SCENE_MUSIC,
	SCENE_RESULT,
	SCENE_RELOAD,
} now_scene_t; /* = next */

typedef enum rec_clear_rank_e {
	REC_CLEAR_RANK_NO_PLAY = 0,
	REC_CLEAR_RANK_DROPED = 1, /* 実装の都合上1スタート */
	REC_CLEAR_RANK_CLEARED,
	REC_CLEAR_RANK_NOMISS,
	REC_CLEAR_RANK_FULLCOMBO,
	REC_CLEAR_RANK_PERFECT,
} rec_clear_rank_t;

typedef enum rec_clear_type2_e {
	REC_CLEAR_TYPE2_NO_PLAY = 0,
	REC_CLEAR_TYPE2_CRASHED,
	REC_CLEAR_TYPE2_DROPED,
	REC_CLEAR_TYPE2_CLEARED,
	REC_CLEAR_TYPE2_COZYCLEAR,
	REC_CLEAR_TYPE2_MISSLESS,
	REC_CLEAR_TYPE2_NOMISS,
	REC_CLEAR_TYPE2_FULLCOMBO,
	REC_CLEAR_TYPE2_PERFECT,
	REC_CLEAR_TYPE2_FULLPERFECT
} rec_clear_type2_et;

typedef enum rec_dif_e {
	REC_DIF_AUTO = 0,
	REC_DIF_EASY,
	REC_DIF_NORMAL,
	REC_DIF_HARD,
	REC_DIF_ANOTHER,
	REC_DIF_SECRET,
} rec_dif_t;

typedef enum rec_error_e {
	REC_ERROR_NONE = 0,
	REC_ERROR_FILE_EXIST = 100, /* ファイルがなかった */
	REC_ERROR_FILE_NUM   = 101, /* 指定ナンバーが大きすぎた */
	REC_ERROR_MAP_COUNT  = 200, /* オブジェクト数が多すぎた */
} rec_error_t;

#endif /* enum */

typedef struct rec_to_play_set_s {
	int packNo = 0;
	int musicNo = 0;
	int dif = 0;
	int shift = 0;
	int autoFg = 0;
} rec_to_play_set_t;

typedef struct rec_system_s {
	int chara = 0;
	int offset = 0;
	int soundEn = 0;
	int backLight = 2;
	int language = 0;
	int keyViewEn = 0;
	int judgePos = 0;
} rec_system_t;

class rec_system_langstr_c {
private:
	tstring jp;
	tstring en;

public:
	rec_system_langstr_c(void) = delete;
	rec_system_langstr_c(tstring a_jp, tstring a_en);

	const tstring &get_str(void) const;
};

#if 1 /* helpbar */

enum class rec_helpbar_type_ec {
	NONE,
	MENU,
	SELECT,
	COLLECT,
	COLLECT_ITEM,
	COLLECT_STORY,
	COLLECT_STORY_READ,
	OPTION
};

class rec_helpbar_c {
private:
	const int draw_strX =   5;
	const int draw_strY = 460;

#if 1 /* str系 */
	const std::vector<rec_system_langstr_c> str_menu = {
		rec_system_langstr_c{
			_T("左右キー:選択   Enterキー:決定"),
			_T("←→key:choose   Enter key:enter")
		}
	};

	const std::vector<rec_system_langstr_c> str_select = {
		rec_system_langstr_c{
			_T("Enterキー: 決定, BackSpaceキー: 戻る"),
			_T("Enter key: start, BackSpace key: back to menu")
		},
		rec_system_langstr_c{
			_T("上下キー: 曲選択, 左右キー: 難易度選択"),
			_T("↑↓key: music select, ←→key: dif select")
		},
		rec_system_langstr_c{
			_T("Zキー: 楽曲を並び替える, Pキー+Enterキー: オートプレイ"),
			_T("Z key: Sort Songs, P and Enter key: Auto Play")
		}
	};

	const std::vector<rec_system_langstr_c> str_collect = {
		rec_system_langstr_c{
			_T("左右キー:選択   Enterキー:決定   BackSpaceキー:戻る"),
			_T("←→key:select   Enter key:enter   BackSpace key:back to menu")
		}
	};

	const std::vector<rec_system_langstr_c> str_collect_item = {
		rec_system_langstr_c{
			_T("上下左右キー:選択   BackSpaceキー:戻る"),
			_T("←↓↑→key:select   BackSpace key:back")
		}
	};

	const std::vector<rec_system_langstr_c> str_collect_story = {
		rec_system_langstr_c{
			_T("上下左右キー:選択   Enterキー:決定   BackSpaceキー:戻る"),
			_T("←↓↑→key:select   Enter key:enter   BackSpace key:back")
		}
	};

	const std::vector<rec_system_langstr_c> str_collect_story_read = {
		rec_system_langstr_c{
			_T("上下キー:ページ選択   BackSpaceキー:戻る"),
			_T("↑↓key:select   BackSpace key:back")
		}
	};

	const std::vector<rec_system_langstr_c> str_option = {
		rec_system_langstr_c{
			_T("上下左右キー:選択   BackSpaceキー:保存して戻る"),
			_T("←↓↑→ key:choose   BackSpace key:save the option and return")
		}
	};
#endif /* str系 */

	dxcur_pic_c pic = dxcur_pic_c(_T("picture/help.png"));

	tstring GetStr(rec_helpbar_type_ec type) const;

public:
	void DrawHelp(rec_helpbar_type_ec type) const;
};

#endif /* helpbar */

class rec_sys_bgm {
	int s_totalVolume = 255;
	DxSnd_t s_bgm = DXLIB_SND_NULL;
	tstring s_bgmName = _T("");

public:
	void SetVolume(int val);
	void SetMem(const tstring &sndPath);
	bool RecCheckSoundMem(void) const;
	void Play(bool force, bool loop, bool top_position);
	void Delete(void);
	void Stop(void);
	void SetCurrentPosition(int val);
};

extern int RecPlayDebug[3];

extern int SmallFontData;
extern int LargeFontData;

extern rec_sys_bgm rec_bgm_system_g;

#if 1 /* dirent系 */

extern bool GetFolderList(std::vector<std::string> &list, const std::string &path);
extern bool GetFolderListWchar(std::vector<std::wstring> &list, const std::wstring &path);

#endif /* dirent系 */

/* これらの関数は、呼ばれるたびにリソースを全探索する(o(n)である)ので、*/
/* for文などで曲の全探索をするとo(n^2)になって処理が遅くなる。やめてね */
/* TODO: 多分vector<string>でo(n)にできる */
extern rec_error_t RecGetPackFolderPath(TCHAR *ret, size_t size, uint packNo);
extern rec_error_t RecGetMusicFolderPath(TCHAR *ret, size_t size, uint packNo, uint songNo);
extern rec_error_t RecGetMusicFolderName(TCHAR *ret, size_t size, uint packNo, uint songNo);
extern rec_error_t RecGetMusicMapRrsPath(TCHAR *ret, size_t size, uint packNo, uint songNo,
	rec_dif_t difNo);
extern rec_error_t RecGetMusicMapTxtPath(TCHAR *ret, size_t size, uint packNo, uint songNo,
	rec_dif_t difNo);
