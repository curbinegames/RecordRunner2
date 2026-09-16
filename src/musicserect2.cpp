/**
 * やりたいこと
 * フォルダー機能
 * リロード中の背景
 * マウス選択機能の復活
 */

#if 1 /* include */

#include <queue>

/* base include */
#include <DxLib.h>

/* curbine code include */
#include <dxcur.h>
#include <dxdraw.h>
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <RecSystem.h>
#include <option.h>
#include <playbox.h>
#include <recr_cutin.h>
#include <RecSave.h>
#include <RecScoreFile.h>
#include <RecWindowRescale.h>

/* others include */
#include <RecordLoad2.h>
#include <recp_cal_ddif_2.h>

/* own include */
#include <musicserect2.h>

#endif /* include */

#if 1 /* define */

#define PackNumLim 16

#define MUSE_FADTM 250
#define MUSE_KEYTM 500

#define REC_SELECT_VECT_UP    -1
#define REC_SELECT_VECT_DOWN   1
#define REC_SELECT_VECT_LEFT   1
#define REC_SELECT_VECT_RIGHT -1

#define REC_SELECT_DEFAULT_SAMPLE_RATE 44100

#endif /* define */

typedef enum rec_select_key_e {
	REC_SELECT_KEY_NONE   = 0,
	REC_SELECT_KEY_RETURN = 1,
	REC_SELECT_KEY_BACK   = 2,
	REC_SELECT_KEY_UP     = 3,
	REC_SELECT_KEY_DOWN   = 4,
	REC_SELECT_KEY_LEFT   = 5,
	REC_SELECT_KEY_RIGHT  = 6,
	REC_SELECT_KEY_SORT   = 7,
	REC_SELECT_KEY_RELORD = 8
} rec_select_key_et;

enum class rec_select_sorttype_ec {
	DEFAULT,
	SCORE,
	LEVEL
};

rec_select_sorttype_ec &operator++(rec_select_sorttype_ec &val) {
    switch (val) {
    case rec_select_sorttype_ec::LEVEL: /* 上限ループ */
        val = rec_select_sorttype_ec::DEFAULT;
        break;
    default:
        val = static_cast<rec_select_sorttype_ec>(static_cast<int>(val) + 1);
    }
    return val;
}

typedef struct music_box_2 {
	int level      = -1;
	int preview[2] = {0, 10000};
	int Hscore     =  0;
	int Hdis       =  0;
	int packNo     =  0;
	int musicNo    =  0;
	double Hacc    =  0.0;
	int levelList[4] = {-1, -1, -1, -1}; //0=easy, 1=normal, 2=hard, 3=another
	rec_dif_t LvType = REC_DIF_EASY;
	rec_score_rate_t ScoreRate = REC_SCORE_RATE_NO_PLAY;
	rec_clear_rank_t ClearRank = REC_CLEAR_RANK_NO_PLAY;
	rec_system_langstr_c SongName;
	rec_system_langstr_c artist;
	tstring difP         = _T("");
	tstring packName     = _T("");
	tstring SongFileName = _T("");
	tstring jacketP      = _T("");
	rec_ddif_pal_t mpal;
} MUSIC_BOX_2;

typedef struct rec_select_command_s {
	int music = 0;
	int dif   = 1;
} rec_select_command_st;

class rec_select_musiclist_c {
private:
	std::vector<MUSIC_BOX_2> data;

private: /* 初期化系 */
	rec_error_t ReadMusicOneDif(
		MUSIC_BOX_2 &songdata, const tstring &path, const tstring &subpath,
		const tstring &packName, int packNum, int musicNo, rec_dif_t dif
	) {
		DxFile_t fd;
		TCHAR buf[256];
		int lang = optiondata.lang;

		fd = FileRead_open(path.c_str());

		if (fd == DXLIB_FILE_NULL) { return REC_ERROR_FILE_EXIST; }

		//初期値定義
		songdata.LvType       = dif;
		songdata.level        = -1;
		songdata.preview[0]   = REC_SELECT_DEFAULT_SAMPLE_RATE * 10;
		songdata.preview[1]   = REC_SELECT_DEFAULT_SAMPLE_RATE * 60;
		songdata.packNo       = packNum;
		songdata.musicNo      = musicNo;
		songdata.packName     = packName;
		songdata.SongFileName = _T("NULL");
		songdata.jacketP      = _T("picture/NULL jucket.png");

		while (FileRead_eof(fd) == 0) {
			FileRead_gets(buf, 256, fd);
			//曲名を読み込む
			if (strands_direct(buf, L"#TITLE:")) {
				strmods(buf, 7);
				songdata.SongName.set_str_jp(buf);
				if (songdata.SongName.get_str() == _T("")) {
					songdata.SongName.set_str_en(buf);
				}
			}
			else if (strands_direct(buf, L"#E.TITLE:")) {
				strmods(buf, 9);
				songdata.SongName.set_str_en(buf);
				if (songdata.SongName.get_str() == _T("")) {
					songdata.SongName.set_str_jp(buf);
				}
			}
			//作曲者を読み込む
			else if (strands_direct(buf, L"#ARTIST:")) {
				strmods(buf, 8);
				songdata.artist.set_str_jp(buf);
				if (songdata.artist.get_str() == _T("")) {
					songdata.artist.set_str_en(buf);
				}
			}
			else if (strands_direct(buf, L"#E.ARTIST:")) {
				strmods(buf, 10);
				songdata.artist.set_str_en(buf);
				if (songdata.artist.get_str() == _T("")) {
					songdata.artist.set_str_jp(buf);
				}
			}
			//曲ファイル名を読み込む
			else if (strands_direct(buf, L"#MUSIC:")) {
				strmods(buf, 7);
				songdata.SongFileName  = subpath;
				songdata.SongFileName += buf;
			}
			//難易度を読み込む
			else if (strands_direct(buf, L"#LEVEL:")) {
				strmods(buf, 7);
				songdata.level = strsans(buf);
			}
			//プレビュー時間を読み込む
			else if (strands_direct(buf, L"#PREVIEW:")) {
				strmods(buf, 9);
				songdata.preview[0] = (int)((double)strsans(buf) / 1000.0 * REC_SELECT_DEFAULT_SAMPLE_RATE);
				strnex(buf);
				if (L'0' <= buf[1] && buf[1] <= L'9') {
					songdata.preview[1] = (int)((double)strsans(buf) / 1000.0 * REC_SELECT_DEFAULT_SAMPLE_RATE);
				}
			}
			//ジャケット写真を読み込む
			else if (strands_direct(buf, L"#JACKET:")) {
				strmods(buf, 8);
				songdata.jacketP  = subpath;
				songdata.jacketP += buf;
			}
			//差し替えAnotherバーを読み込む
			else if (strands_direct(buf, L"#DIFBAR:")) {
				strmods(buf, 8);
				songdata.difP  = subpath;
				songdata.difP += buf;
			}
			//マップに入ったら抜ける
			else if (strands_direct(buf, L"#MAP:")) { break; }
		}
		FileRead_close(fd);

		return REC_ERROR_NONE;
	}

	void ReadHighscore(MUSIC_BOX_2 &songdata, const tstring &songName, rec_dif_t dif) {
		rec_save_score_t score;
		RecSaveReadScoreOneDif(&score, songName.c_str(), dif);
		songdata.Hscore    = score.score;
		songdata.Hacc      = score.acc;
		songdata.Hdis      = score.dist;
		songdata.ScoreRate = score.scoreRate;
		songdata.ClearRank = score.clearRank;
		return;
	}

	void ReadMusic(
		const tstring &packName, const tstring &songName, int packNum, int musicNo
	) {
		int levelList[4] = {-1, -1, -1, -1}; //0=easy, 1=normal, 2=hard, 3=another
		std::queue<MUSIC_BOX_2> buf_list;
		for (int iDif = 0; iDif < 6; iDif++) {
			rec_error_t status = REC_ERROR_NONE;
			tstring txtpath = _T("record/");
			tstring subPath;
			tstring rrsPath;
			MUSIC_BOX_2 buf;
			txtpath += packName; //"record/<パック名>"
			txtpath += _T('/'); //"record/<パック名>/"
			txtpath += songName; //"record/<パック名>/<曲名>"
			txtpath += _T('/'); //"record/<パック名>/<曲名>/"
			subPath  = txtpath; //subPathにコピー
			txtpath += std::to_wstring(iDif); //"record/<パック名>/<曲名>/<難易度番号>"
			rrsPath  = txtpath; //rrsPathにコピー
			txtpath += _T(".txt"); //"record/<パック名>/<曲名>/<難易度番号>.txt"
			rrsPath += _T(".rrs"); //"record/<パック名>/<曲名>/<難易度番号>.rrs"
			status = this->ReadMusicOneDif(
				buf, txtpath, subPath, packName, packNum, musicNo, (rec_dif_t)iDif
			);
			if (status == REC_ERROR_NONE) {
				RecScoreReadDdif(buf.mpal, rrsPath);
				this->ReadHighscore(buf, songName, (rec_dif_t)iDif);
				switch (iDif) {
				case 1:
					levelList[0] = buf.level;
					break;
				case 2:
					levelList[1] = buf.level;
					break;
				case 3:
					levelList[2] = buf.level;
					break;
				case 4:
					levelList[3] = buf.level;
					break;
				}
				buf_list.push(buf);
			}
		}

		while (!buf_list.empty()) {
			MUSIC_BOX_2 buf = buf_list.front();
			buf.levelList[0] = levelList[0];
			buf.levelList[1] = levelList[1];
			buf.levelList[2] = levelList[2];
			buf.levelList[3] = levelList[3];
			this->data.push_back(buf);
			buf_list.pop();
		}
	}

	/**
	 * @brief 指定されたパック内の曲のデータをsongdataに格納する。
	 * @param[in] packname パック名
	 * @param[in] packIndex パックのインデックス
	 */
	void ReadPack(const tstring &packname, size_t packIndex) {
		tstring listpath = _T("record/") + packname;
		std::vector<tstring> musiclist;

		// GetFolderListWchar(musiclist, listpath);
		RecGetMusicList(musiclist, packname);
		if (musiclist.empty()) { return; }

		for (size_t i = 0; i < musiclist.size(); i++) {
			this->ReadMusic(packname, musiclist[i], packIndex, i);
		}
	}

public: /* 初期化系 */
	/**
	 * @brief recordフォルダ内のサブフォルダを取得し、各サブフォルダ内のlist.txtを読み込む。
	 * list.txtの内容はsongdataに格納される。取得に失敗した場合は何もせずに終了する。
	 */
	rec_select_musiclist_c(void) {
		int packNum = 0;
		int musicNo = 0;
		std::vector<tstring> packlist;

		// GetFolderListWchar(packlist, L"record");
		RecGetPackList(packlist);
		if (packlist.empty()) { return; }

		for (size_t i = 0; i < packlist.size(); i++) {
			this->ReadPack(packlist[i], i);
		}
	}

public: /* 番地検索系 */
	const MUSIC_BOX_2& operator[](int n) const {
		return this->data[betweens(0, n, data.size() - 1)];
	}

	const MUSIC_BOX_2& at(int n) const {
		return this->data.at(betweens(0, n, data.size() - 1));
	}

	size_t size(void) const {
		return this->data.size();
	}
};

class rec_serect_music_set_c {
public:
	rec_select_musiclist_c detail;
	std::vector<uint> sort;
	rec_select_sorttype_ec sortMode = rec_select_sorttype_ec::DEFAULT;

public: /* 並び替え系 */
	/**
	 * @brief 現在の this->sort の内容をハイスコア順に並び替える
	 * @param なし
	 * @return なし
	 */
	void SortByHScore(void) {
		if (this->sort.empty()) { return; }
		std::sort(this->sort.begin(), this->sort.end(), [this](uint a, uint b) {
			return this->detail[a].Hscore > this->detail[b].Hscore;
		});
	}

	/**
	 * @brief 現在の this->sort の内容を難易度順に並び替える
	 * @param なし
	 * @return なし
	 */
	void SortByLevel(void) {
		if (this->sort.empty()) { return; }
		std::sort(this->sort.begin(), this->sort.end(), [this](uint a, uint b) {
			return this->detail[a].level < this->detail[b].level;
		});
	}

	/**
	 * @brief 現在の this->sort の内容をデフォルト順に並び替える
	 * @param なし
	 * @return なし
	 */
	void SortByDefault(void) {
		if (this->sort.empty()) { return; }
		std::sort(this->sort.begin(), this->sort.end(), [this](uint a, uint b) {
			return a < b;
		});
	}

	/**
	 * @brief 現在の this->sort の内容を this->sortMode に従って並び替える
	 */
	void SortByNowMode(void) {
		switch (this->sortMode) {
		case rec_select_sorttype_ec::LEVEL:
			this->SortByLevel();
			break;
		case rec_select_sorttype_ec::SCORE:
			this->SortByHScore();
			break;
		case rec_select_sorttype_ec::DEFAULT:
			this->SortByDefault();
			break;
		}
	}

public: /* 絞り込み系 */
	void Search(bool (*filter_func)(const MUSIC_BOX_2 &detail, int view_dif), int view_dif) {
		if (filter_func == nullptr) { return; }
		this->sort.clear();
		for (size_t i = 0; i < detail.size(); i++) {
			if (filter_func(this->detail[i], view_dif)) {
				this->sort.push_back(i);
			}
		}
	}

public: /* 番地検索系 */
	const MUSIC_BOX_2& operator[](int n) const {
		return this->detail[sort[betweens(0, n, sort.size() - 1)]];
	}

	const MUSIC_BOX_2& at(int n) const {
		return this->detail.at(sort[betweens(0, n, sort.size() - 1)]);
	}
};
typedef rec_serect_music_set_c songdata_set_t;

#if 1 /* all relord */

static void RecSelectAllRelordDrawInfo(const tstring &musicName, uint iDif) {
	const tstring difName[4] = { _T("AUTO"), _T("EASY"), _T("NORMAL"), _T("HARD") }; /* anotherは非表示 */
	static DxTime_t Btime = 0;

	if (((Btime + 50) >= GetNowCount()) || (iDif >= 4)) { return; }

	Btime = GetNowCount();
	ClearDrawScreen();
	DrawFormatString(5, 5, COLOR_WHITE, L"all record relording...\n%s[%s]", musicName.c_str(), difName[iDif].c_str());
	ScreenFlip();
	return;
}

static void RecSelectAllRelord(void) {
	tstring path;
	DxTime_t next_time = 0;
	std::vector<tstring> pack_list;
	if (RecGetPackList(pack_list) == false) { return; }

	for (uint iPack = 0; iPack < pack_list.size(); iPack++) {
		tstring pack_path = L"record/" + pack_list[iPack];
		std::vector<tstring> music_list;
		if (RecGetMusicList(music_list, pack_list[iPack]) == false) { continue; }
		for (uint iSong = 0; iSong < music_list.size(); iSong++) {
			for (uint iDif = 0; iDif < 5; iDif++) {
				if (next_time <= GetNowCount()) {
					RecSelectAllRelordDrawInfo(music_list[iSong], iDif);
					next_time = GetNowCount() + 100;
				}
				if (RecordLoad2(iPack, iSong, iDif) != REC_ERROR_NONE) {
					continue;
				}
				if (RecGetMusicMapRrsPath(path, iPack, iSong, (rec_dif_t)iDif) != 0) { continue; }
				cal_ddif_3(path.c_str());
			}
		}
	}
	return;
}

#endif

#if 1 /* sub action */

static rec_select_key_et RecSerectKeyCheck() {
	rec_select_key_et ret = REC_SELECT_KEY_NONE;
	int mouseBtn = 0;
	int mouseX = 0;
	int mouseY = 0;
	int mouseAct = 0;
	int mouseHoil = 0;

#if 0 /* TODO: ウィンドウサイズ変更により座標がずれたので、一時的に消去 */
	/* マウス入力 */
	mouseAct = 0;
	while (GetMouseInputLog2(&mouseBtn, &mouseX, &mouseY, &mouseAct, true) == 0) {}
	if (mouseBtn == MOUSE_INPUT_LEFT && mouseAct == MOUSE_INPUT_LOG_UP) {
		if (5 <= mouseX && mouseX <= 280 && 195 <= mouseY && mouseY <= 290) {
			ret = REC_SELECT_KEY_RETURN;
		}
		if (5 <= mouseX && mouseX <= 245 && 5 <= mouseY && mouseY <= 175) {
			ret = REC_SERECT_KEY_UP;
		}
		if (5 <= mouseX && mouseX <= 305 && 310 <= mouseY && mouseY <= 475) {
			ret = REC_SERECT_KEY_DOWN;
		}
		if (505 <= mouseX && mouseX <= 635 && 5 <= mouseY && mouseY <= 130) {
			ret = REC_SERECT_KEY_SORT;
		}
	}
#endif

	/* ホイール入力 */
	mouseHoil = GetMouseWheelRotVol();
	if (1 <= mouseHoil)  { ret = REC_SELECT_KEY_UP;   } /* 奥回し */
	if (mouseHoil <= -1) { ret = REC_SELECT_KEY_DOWN; } /* 手前回し */

	/* キー入力 */
	InputAllKeyHold();
	switch (GetKeyPushOnce()) {
	case KEY_INPUT_RETURN:
		ret = REC_SELECT_KEY_RETURN;
		break;
	case KEY_INPUT_BACK:
		ret = REC_SELECT_KEY_BACK;
		break;
	case KEY_INPUT_UP:
		ret = REC_SELECT_KEY_UP;
		break;
	case KEY_INPUT_DOWN:
		ret = REC_SELECT_KEY_DOWN;
		break;
	case KEY_INPUT_LEFT:
		ret = REC_SELECT_KEY_LEFT;
		break;
	case KEY_INPUT_RIGHT:
		ret = REC_SELECT_KEY_RIGHT;
		break;
	case KEY_INPUT_Z:
		ret = REC_SELECT_KEY_SORT;
		break;
	case KEY_INPUT_F5:
		ret = REC_SELECT_KEY_RELORD;
		break;
	default:
		break;
	}

	return ret;
}

static bool RecSerectTrySecret(int Hscore) {
	bool ret = false;
	int rate = 0;
	if (Hscore < 90000) { return false; }
	if (Hscore >= 99000) { return true; }
	if (Hscore >= 90000 && Hscore < 92500) {
		rate = pals(90000, 0, 92500, 25, Hscore);
	}
	else if (Hscore >= 92500 && Hscore < 95000) {
		rate = pals(95000, 50, 92500, 25, Hscore);
	}
	else if (Hscore >= 95000 && Hscore < 98000) {
		rate = pals(95000, 50, 98000, 750, Hscore);
	}
	else if (Hscore >= 98000 && Hscore < 99000) {
		rate = pals(99000, 1000, 98000, 750, Hscore);
	}
	if (GetRand(1000) <= rate) { ret = true; }
	return ret;
}

#if 0
static int RecSerectTrySecret2(int AutoFlag, int dif, MUSIC_BOX *songdata) {
	if (AutoFlag == 0 && dif == 3 &&
		songdata->Hscore[3] >= 90000 &&
		strands_direct(songdata->SongFileName[5], L"NULL") == 0 &&
		songdata->Hscore[5] <= 0)
	{
		if (RecSerectTrySecret(songdata->Hscore[3]) == true) {
			return 1;
		}
	}
	return 0;
}
#endif

static bool Rec_Select_DifFilter(const MUSIC_BOX_2 &detail, int view_dif) {
	return (detail.LvType == view_dif);
}

/**
 * 曲リストを並び替えします
 * @param[in] songdata 曲データ
 * @param[in] dif 難易度
 * TODO: songdata_set_tの中に入れる。
 */
static void SortSong(songdata_set_t &songdata, int dif) {
	songdata.Search(Rec_Select_DifFilter, dif);
	songdata.SortByNowMode();
}

/**
 * SortSongで並び替えをした後、cmdを前と同じ曲に合わせます
 * @param[in] songdata 曲データ
 * @param[in] mode 並び替えの方法
 * @param[in] dif 難易度
 * @param[out] cmd commandの格納場所
 */
static void SortSongWithSave(songdata_set_t &songdata, int dif, int &cmd) {
	tstring save;

	save = songdata[betweens(0, cmd, songdata.sort.size() - 1)].SongName.get_str();
	SortSong(songdata, dif);
	for (int i = 0; i < songdata.sort.size(); i++) {
		if (save == songdata[i].SongName.get_str()) {
			cmd = i;
			break;
		}
	}
	cmd = min(cmd, songdata.sort.size() - 1);
	return;
}

#if 0
/**
 * 今選んでいる難易度に譜面がなかったら、ほかの難易度を探しに行く
 * @param[in] songdata 今選んでいる曲データ
 * @param[in] dif 今選んでいる難易度
 * @param[in] SortMode 今の並び順
 * @return int 見つかった難易度
 */
static int RecSerectFetchDif(const MUSIC_BOX *songdata, int dif, int SortMode) {
	int ret = dif;

	if (SortMode != REC_SORT_DEFAULT) { return ret; }
	if (strands_direct(songdata->SongName[dif], L"NULL") == 0) { return ret; }

	if (strands_direct(songdata->SongName[0], L"NULL") != 1) { ret = 0; }
	switch (dif) {
	case REC_DIF_AUTO:
		if (strands_direct(songdata->SongName[1], L"NULL") != 1) { ret = 1; }
		else if (strands_direct(songdata->SongName[2], L"NULL") != 1) { ret = 2; }
		else if (strands_direct(songdata->SongName[3], L"NULL") != 1) { ret = 3; }
		break;
	case REC_DIF_EASY:
		if (strands_direct(songdata->SongName[2], L"NULL") != 1) { ret = 2; }
		else if (strands_direct(songdata->SongName[3], L"NULL") != 1) { ret = 3; }
		break;
	case REC_DIF_NORMAL:
		if (strands_direct(songdata->SongName[1], L"NULL") != 1) { ret = 1; }
		else if (strands_direct(songdata->SongName[3], L"NULL") != 1) { ret = 3; }
		break;
	case REC_DIF_HARD:
		if (strands_direct(songdata->SongName[2], L"NULL") != 1) { ret = 2; }
		else if (strands_direct(songdata->SongName[1], L"NULL") != 1) { ret = 1; }
		break;
	default:
		if (strands_direct(songdata->SongName[3], L"NULL") != 1) { ret = 3; }
		else if (strands_direct(songdata->SongName[2], L"NULL") != 1) { ret = 2; }
		else if (strands_direct(songdata->SongName[1], L"NULL") != 1) { ret = 1; }
		break;
	}

	return ret;
}
#endif

static void RecSerectLoadBefCmd(rec_select_command_st &cmd, rec_select_sorttype_ec &sortMode) {
	int buf[2] = {0, 1};
	FILE *fp;
	_wfopen_s(&fp, L"save/SongSelect2.dat", L"rb");
	if (fp != NULL) {
		fread(&buf, sizeof(int), 2, fp);
		fread(&sortMode, sizeof(rec_select_sorttype_ec), 1, fp);
		fclose(fp);
	}
	cmd.music = buf[0];
	cmd.dif   = buf[1];
	return;
}

static void RecSerectSaveBefCmd(const rec_select_command_st &cmd, rec_select_sorttype_ec sortMode) {
	FILE *fp;
	_wfopen_s(&fp, L"save/SongSelect2.dat", L"wb");
	if (fp != NULL) {
		fwrite(&cmd, sizeof(int), 2, fp);
		fwrite(&sortMode, sizeof(rec_select_sorttype_ec), 1, fp);
		fclose(fp);
	}
	return;
}

static void RecSerectSetToPlay(rec_to_play_set_t &toPlay, const rec_select_command_st &cmd,
	const songdata_set_t &songdata)
{
	int inum = 0;

	if (CheckHitKey(KEY_INPUT_LSHIFT) == 1 || CheckHitKey(KEY_INPUT_RSHIFT) == 1) {
		toPlay.shift = 1;
	}
	else { toPlay.shift = 0; }
	if (CheckHitKey(KEY_INPUT_P) == 1) { toPlay.autoFg = 1; }
	else { toPlay.autoFg = 0; }
	toPlay.packNo  = songdata[cmd.music].packNo;
	toPlay.musicNo = songdata[cmd.music].musicNo;
	toPlay.dif     = songdata[cmd.music].LvType;

#if 0
	//隠し曲用
	if (RecSerectTrySecret2(toPlay->autoFg, cmd[1], &(*songdata)[cmd[0]]) == 1) {
		toPlay->dif = 5;
	}
#endif
	return;
}

#endif /* sub action */

#if 1 /* ui class */

static class rec_serect_backpic_c {
private:
	dxcur_pic_c back = dxcur_pic_c(_T("picture/select/MSback.png"));
	int backpos = 0;

public:
	inline void DrawBackPic(void) {
		this->backpos = (this->backpos - 2) % 640; /* TODO: 別関数にできる */
		RecRescaleDrawGraph(this->backpos, 0, this->back.handle(), TRUE);
		RecRescaleDrawGraph(this->backpos + 640, 0, this->back.handle(), TRUE);
	}
};

static class rec_serect_preview_sound_c {
private:
	int preSC = 0;
	int SongPrePat = 0;
	int SongPreSTime = 0;
	int preTime[2] = {
		REC_SELECT_DEFAULT_SAMPLE_RATE * 10,
		REC_SELECT_DEFAULT_SAMPLE_RATE * 60
	};
	tstring playingsong = { L"NULL" };

public:
	void StartSnd() {
		rec_bgm_system_g.SetCurrentPosition(this->preTime[this->SongPrePat]);
		rec_bgm_system_g.SetVolume(0);
		rec_bgm_system_g.Play(true, false, false);
		WaitTimer(WAIT_TIME_AFTER_MUSICPLAY);
		this->SongPreSTime = GetNowCount();
	}

	int UpdateSnd(const MUSIC_BOX_2 &songdata, int dif) {
		if ((strands_direct(songdata.SongFileName.c_str(), L"NULL") != 0) ||
			(strands(this->playingsong.c_str(), songdata.SongFileName.c_str()) != 0))
		{
			return 0;
		}
		rec_bgm_system_g.Delete();
		this->playingsong = songdata.SongFileName;
		rec_bgm_system_g.SetMem(this->playingsong);
		this->SongPrePat = 0;
		this->preTime[0] = songdata.preview[0];
		this->preTime[1] = songdata.preview[1];
		return 1;
	}

	void CheckTime(void) {
		int Ntime = GetNowCount();
		if (Ntime - this->SongPreSTime < 500) {
			rec_bgm_system_g.SetVolume(lins(0, 0, 500, 255, Ntime - this->SongPreSTime));
		}
		else if (IS_BETWEEN_RIGHT_LESS(500, Ntime - this->SongPreSTime, 14500)) {
			rec_bgm_system_g.SetVolume(255);
		}
		else if (IS_BETWEEN_RIGHT_LESS(14500, Ntime - this->SongPreSTime, 15000)) {
			rec_bgm_system_g.SetVolume(lins(14500, 255, 15000, 0, Ntime - this->SongPreSTime));
		}
		else if (15000 <= Ntime - this->SongPreSTime) {
			rec_bgm_system_g.Stop();
			this->SongPrePat = (this->SongPrePat + 1) % 2;
			this->StartSnd();
		}
	}

	void CheckSnd(const MUSIC_BOX_2 &songdata, int dif) {
		if (this->preSC + MUSE_KEYTM < GetNowCount()) {
			if (this->UpdateSnd(songdata, dif) == 1) {
				this->StartSnd();
			}
		}
	}

	void SetPresc(int Ntime) {
		this->preSC = Ntime;
	}
};

static class rec_serect_musicbar_c {
private:
#define VIEW_COUNT 9

	int UD = REC_SELECT_VECT_DOWN;
	int startC = -MUSE_FADTM;
	dxcur_pic_c bar[2] = {
		dxcur_pic_c(L"picture/select/songbarB.png"),
		dxcur_pic_c(L"picture/select/songbarY.png")
	};
	dxcur_pic_c folder_bar[2] = {
		dxcur_pic_c(L"picture/select/songbarB.png"),
		dxcur_pic_c(L"picture/select/songbarY.png") /* TODO: 画像作る */
	};
	dxcur_pic_c CRate[5] = {
		dxcur_pic_c(L"picture/select/MarkD.png"),
		dxcur_pic_c(L"picture/select/MarkC.png"),
		dxcur_pic_c(L"picture/select/MarkNM.png"),
		dxcur_pic_c(L"picture/select/MarkFC.png"),
		dxcur_pic_c(L"picture/select/MarkP.png")
	};
	dxcur_pic_c rankP[6] = {
		dxcur_pic_c(L"picture/select/MiniEX.png"),
		dxcur_pic_c(L"picture/select/MiniS.png"),
		dxcur_pic_c(L"picture/select/MiniA.png"),
		dxcur_pic_c(L"picture/select/MiniB.png"),
		dxcur_pic_c(L"picture/select/MiniC.png"),
		dxcur_pic_c(L"picture/select/MiniD.png")
	};
public:
	std::vector<tstring> folder_str;

private:
	void DrawClear(int x, int y, int clearNo) const {
		if (0 <= clearNo && clearNo <= 4) {
			DrawGraph(x, y, this->CRate[clearNo].handle(), TRUE);
		}
	}

	void DrawRack(int x, int y, int rankNo) const {
		if (0 <= rankNo && rankNo <= 5) {
			DrawGraph(x, y, this->rankP[rankNo].handle(), TRUE);
		}
	}

	void DrawMainFolderOne(int dif, int BasePosX, int BasePosY, const tstring &str) const {
		DrawGraph(BasePosX - 120, BasePosY - 170, this->folder_bar[1].handle(), TRUE);
		DrawStringToHandle(BasePosX - 30, BasePosY - 157, str.c_str(), COLOR_BLACK, SmallFontData);
	}

	void DrawSubFolderOne(int dif, int BasePosX, int BasePosY, const tstring &str) const {
		DrawGraph(BasePosX - 120, BasePosY - 170, this->folder_bar[0].handle(), TRUE);
		DrawStringToHandle(BasePosX - 30, BasePosY - 157, str.c_str(), COLOR_WHITE, SmallFontData);
	}

	void DrawMainOne(int dif, int BasePosX, int BasePosY, const MUSIC_BOX_2 &songdata) const {
		DrawGraph(BasePosX - 120, BasePosY - 170, this->bar[1].handle(), TRUE);
		DrawStringToHandle(BasePosX - 30, BasePosY - 157, songdata.SongName.get_str().c_str(), COLOR_BLACK, SmallFontData);
		DrawStringToHandle(BasePosX - 30, BasePosY - 129, songdata.artist.get_str().c_str(), COLOR_BLACK, SmallFontData);
		this->DrawClear(BasePosX + 156, BasePosY - 132, songdata.ClearRank - 1);
		this->DrawRack(BasePosX + 156, BasePosY - 132, songdata.ScoreRate);
		for (int idif = 0; idif < 3; idif++) {
			DrawFormatStringToHandle(BasePosX - 25 + idif * 70, BasePosY - 97,
				COLOR_BLACK, SmallFontData, L"%2d", songdata.levelList[idif]);
		}
	}

	void DrawSubOne(int dif, int BasePosX, int BasePosY, const MUSIC_BOX_2 &songdata) const {
		DrawGraph(BasePosX - 120, BasePosY - 170, this->bar[0].handle(), TRUE);
		DrawStringToHandle(BasePosX - 30, BasePosY - 157, songdata.SongName.get_str().c_str(), COLOR_WHITE, SmallFontData);
		DrawStringToHandle(BasePosX - 30, BasePosY - 129, songdata.artist.get_str().c_str(), COLOR_WHITE, SmallFontData);
		this->DrawClear(BasePosX + 152, BasePosY - 163, songdata.ClearRank - 1);
		this->DrawRack(BasePosX + 152, BasePosY - 163, songdata.ScoreRate);
	}

public:
	void SlideBar(int vect) {
		this->UD = vect;
		this->startC = GetNowCount();
	}

	void DrawAll(int Ypos, const rec_select_command_st &cmd, const songdata_set_t &songdata, bool is_folder) const {
		int BasePosX = 0;
		int BasePosY = 0;
		int slide = 0;
		int picsong = 0;
		int moveC = 0;

		moveC = maxs_2(-1 * (GetNowCount() - this->startC) + MUSE_FADTM, 0);
		picsong = (cmd.music + songdata.sort.size() - (VIEW_COUNT / 2)) % songdata.sort.size();

		for (int count = 0; count < VIEW_COUNT; count++) {
			slide = pals(0, 0, 250, this->UD * 80, moveC);
			BasePosY = slide + count * 80 + Ypos - 170;
			if (count <= (VIEW_COUNT / 2)) {
				BasePosY += lins(5, 180, 7, 100, VIEW_COUNT);
			}
			else {
				BasePosY += lins(5, 220, 7, 140, VIEW_COUNT);
			}
			BasePosX = lins(480, 80, 240, 40, BasePosY);

			if (count == (VIEW_COUNT / 2)) {
				if (is_folder) {
					this->DrawMainFolderOne(cmd.dif, BasePosX, BasePosY, this->folder_str[picsong]);
				}
				else {
					this->DrawMainOne(cmd.dif, BasePosX, BasePosY, songdata[picsong]);
				}
			}
			else {
				if (is_folder) {
					this->DrawSubFolderOne(cmd.dif, BasePosX, BasePosY, this->folder_str[picsong]);
				}
				else {
					this->DrawSubOne(cmd.dif, BasePosX, BasePosY, songdata[picsong]);
				}
			}

			picsong = (picsong + 1) % songdata.sort.size();
		}
	}

#undef VIEW_COUNT
};

static class rec_serect_disk_c {
private:
	int Lv = 1;
	int UD = REC_SELECT_VECT_DOWN;
	int startC = -MUSE_FADTM;
	intx100_t rate = 0;
	double Nrot = 0.0;
	dxcur_pic_c disk = dxcur_pic_c(_T("picture/disk.png"));
	dxcur_pic_c runner;
	dxcur_pic_c rateBar;

	void DrawNamePlate(int baseX, int baseY) {
		DrawGraphAnchor(baseX - 30, baseY, this->rateBar.handle(), DXDRAW_ANCHOR_TOP_RIGHT);
		DrawGraphAnchor(baseX, baseY, this->runner.handle(), DXDRAW_ANCHOR_TOP_RIGHT);
		DrawFormatStringToHandleAnchor(baseX - 220, baseY + 12, COLOR_BLACK, SmallFontData,
			DXDRAW_ANCHOR_TOP_RIGHT, L"Lv:%2d", this->Lv);
		DrawFormatStringToHandleAnchor(baseX - 215, baseY + 37, COLOR_BLACK, SmallFontData,
			DXDRAW_ANCHOR_TOP_RIGHT, L"RATE:%d.%02d", this->rate / 100, this->rate % 100);
	}

	void DrawDisk(int baseX, int baseY) {
		int moveC = 0;

		moveC = maxs_2(-1 * (GetNowCount() - this->startC) + MUSE_FADTM, 0);
		if (this->UD == REC_SELECT_VECT_DOWN) { this->Nrot += pals(0, 2, MUSE_FADTM, -75, moveC) / 100.0; }
		else { this->Nrot += pals(0, 2, MUSE_FADTM, 75, moveC) / 100.0; }
		if (this->Nrot > 6.28) { this->Nrot -= 6.28; }
		else if (this->Nrot < 0) { this->Nrot += 6.28; }
		DrawRotaGraphAnchor(baseX, baseY, 1, this->Nrot, this->disk.handle(), DXDRAW_ANCHOR_TOP_RIGHT, TRUE);
	}

	void DrawSort(int baseX, int baseY, rec_select_sorttype_ec mode) {
		switch (mode) {
		case rec_select_sorttype_ec::DEFAULT:
			DrawStringToHandleAnchor(baseX, baseY, REC_STR_LANG(_T("デフォルト"), _T("default")), COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
			break;
		case rec_select_sorttype_ec::LEVEL:
			DrawStringToHandleAnchor(baseX, baseY, REC_STR_LANG(_T("レベル順"), _T("level")), COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
			break;
		case rec_select_sorttype_ec::SCORE:
			DrawStringToHandleAnchor(baseX, baseY, REC_STR_LANG(_T("スコア順"), _T("score")), COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
			break;
		}
	}

public:
	rec_serect_disk_c() {
		rec_save_charaplay_t buf;
		RecSaveReadCharaPlay(&buf);

		this->rate = RecSaveGetFullRunnerRate() * 100.0;

		switch (optiondata.chara) {
		case 0:
			this->runner.reload(L"picture/select/Mpicker.png");
			this->Lv = buf.picker;
			break;
		case 1:
			this->runner.reload(L"picture/select/Mgator.png");
			this->Lv = buf.mapgator;
			break;
		case 2:
			this->runner.reload(L"picture/select/Mtaylor.png");
			this->Lv = buf.taylor;
			break;
		default:
			this->Lv = 0;
			break;
		}
		this->Lv++;

		if (this->rate < 2500) {
			this->rateBar.reload(L"picture/select/MSrate1.png");
		}
		else if (2500 <= this->rate && this->rate < 5500) {
			this->rateBar.reload(L"picture/select/MSrate2.png");
		}
		else if (5500 <= this->rate && this->rate < 9000) {
			this->rateBar.reload(L"picture/select/MSrate3.png");
		}
		else if (9000 <= this->rate && this->rate < 12000) {
			this->rateBar.reload(L"picture/select/MSrate4.png");
		}
		else {
			this->rateBar.reload(L"picture/select/MSrate5.png");
		}
	}

private:
public:
	void SlideDisk(int vect) {
		this->UD = vect;
		this->startC = GetNowCount();
	}

	void DrawDiskSet(int baseX, int baseY, rec_select_sorttype_ec mode) {
		this->DrawNamePlate(baseX - 115, baseY - 20);
		this->DrawDisk(baseX, baseY);
		this->DrawSort(baseX - 60, baseY + 85, mode);
	}
};

static class rec_serect_detail_c {
private:
	int LR = REC_SELECT_VECT_LEFT;
	int XstartC = -MUSE_FADTM;
	tstring viewingDifBar = { L"NULL" };
	dxcur_pic_c difbar[6] = {
		dxcur_pic_c(L"picture/difauto.png"),
		dxcur_pic_c(L"picture/difeasy.png"),
		dxcur_pic_c(L"picture/difnormal.png"),
		dxcur_pic_c(L"picture/difhard.png"),
		dxcur_pic_c(L"picture/difanother.png"),
		dxcur_pic_c(L"picture/difanother.png")
	};
	dxcur_pic_c detail = dxcur_pic_c(_T("picture/select/detail.png"));
	dxcur_pic_c mpalNamePic = dxcur_pic_c(_T("picture/select/mpalName.png"));
	struct {
		dxcur_pic_c tiny;
		dxcur_pic_c big;
	} difC[4] = {
		{ dxcur_pic_c(L"picture/select/Dif1S.png"), dxcur_pic_c(L"picture/select/Dif1B.png") },
		{ dxcur_pic_c(L"picture/select/Dif2S.png"), dxcur_pic_c(L"picture/select/Dif2B.png") },
		{ dxcur_pic_c(L"picture/select/Dif3S.png"), dxcur_pic_c(L"picture/select/Dif3B.png") },
		{ dxcur_pic_c(L"picture/select/Dif4S.png"), dxcur_pic_c(L"picture/select/Dif4B.png") }
	};

private:
	void DrawDifMark(int baseX, int baseY, const MUSIC_BOX_2 &songdata, int comdif) const {
		int posX = 0;
		int posY = 0;

		for (int i = 0; i < 4; i++) {
			posY = 0;
			if (comdif - 1 == i) { posY = 1; }
			if (comdif - 1 <= i) { posX = 1; }
			if (songdata.levelList[i] >= 0) {
				if (comdif - 1 == i) {
					DrawGraphAnchor(baseX + 11 * posX + 16 * i, baseY,
						this->difC[i].big.handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
				}
				else {
					DrawGraphAnchor(baseX + 11 * posX + 16 * i, baseY,
						this->difC[i].tiny.handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
				}
			}
		}
	}

	void DrawDifBar(int baseX, int baseY, int dif) const {
		int XmoveC = maxs_2(-1 * (GetNowCount() - this->XstartC) + MUSE_FADTM, 0);

		if (this->LR == REC_SELECT_VECT_LEFT) {
			XmoveC = pals(0, 640, MUSE_FADTM, 460, XmoveC);
			DrawGraphAnchor(baseX, baseY, this->difbar[dif].handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
			DrawGraphAnchor(baseX + XmoveC - 461, baseY, this->difbar[dif + 1].handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
		}
		else if (this->LR == REC_SELECT_VECT_RIGHT) {
			XmoveC = pals(0, 460, MUSE_FADTM, 640, XmoveC);
			DrawGraphAnchor(baseX, baseY, this->difbar[dif - 1].handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
			DrawGraphAnchor(baseX + XmoveC - 461, baseY, this->difbar[dif].handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
		}
	}

	void DrawDifMpal(int baseX, int baseY, const rec_ddif_pal_t *mpal, int dif) const {
		const int thick = 8;
		const DxColor_t color[8] = {
			/*        赤,  緑,  青   */
			GetColor(  0,   0, 255), // 青
			GetColor(255,   0,   0), // 赤
			GetColor(  0, 255,   0), // 緑
			GetColor(127,   0, 255), // 紫
			GetColor(255, 255,   0), // 黄色
			GetColor(255, 127, 255), // ピンク
			GetColor(255, 127,   0), // オレンジ
			GetColor(127,   0,   0)	 // 茶色
		};
		const DxColor_t SubColor[8] = {
			/*        赤,  緑,  青   */
			GetColor(  0,   0, 127), // 青
			GetColor(127,   0,   0), // 赤
			GetColor(  0, 127,   0), // 緑
			GetColor( 63,   0, 127), // 紫
			GetColor(127, 127,   0), // 黄色
			GetColor(127,  63, 127), // ピンク
			GetColor(127,  63,   0), // オレンジ
			GetColor( 63,   0,   0)	 // 茶色
		};
#if REC_DEBUG == 1
		DrawBoxAnchor(baseX - lins(0, 0, 900, 450, mpal[dif].mdif) - 70, baseY - thick - 2 * thick * 0 - thick / 2, baseX - 70, baseY - 2 * thick * 7 - thick / 2, COLOR_WHITE, DXDRAW_ANCHOR_BOTTOM_RIGHT, TRUE);
		DrawBoxAnchor(baseX - lins(0, 0, 900, 450, mpal[dif].mdif) - 70, baseY - thick - 2 * thick * 0 - thick / 2, baseX - 70, baseY - 2 * thick * 7 - thick / 2, COLOR_BLACK, DXDRAW_ANCHOR_BOTTOM_RIGHT, FALSE);
#endif
		uint *p_mpal = (uint *)mpal;
		for (uint iPal = 0; iPal < 8; iPal++) {
			uint length = lins(0, 0, 900, 450, p_mpal[7 - iPal]);
			DrawBoxAnchor(
				baseX - length - 70, baseY - thick - 2 * thick * iPal - thick / 2,
				baseX - 70, baseY - 2 * thick * iPal - thick / 2, color[7 - iPal],
				DXDRAW_ANCHOR_BOTTOM_RIGHT, TRUE)
			;
			DrawBoxAnchor(
				baseX - length - 70, baseY - thick - 2 * thick * iPal - thick / 2,
				baseX - 70, baseY - 2 * thick * iPal - thick / 2, SubColor[7 - iPal],
				DXDRAW_ANCHOR_BOTTOM_RIGHT, FALSE
			);
		}
		DrawGraphAnchor(baseX, baseY, this->mpalNamePic.handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
#if REC_DEBUG == 1
		for (uint iPal = 1; iPal < 11; iPal++) {
			DrawLineAnchor(baseX - lins(0, 0, 900, 450, iPal * 100) - 70, baseY - thick - 2 * thick * 0 - thick / 2, baseX - lins(0, 0, 900, 450, iPal * 100) - 70, baseY - 2 * thick * 7 - thick / 2, COLOR_RED, DXDRAW_ANCHOR_BOTTOM_RIGHT);
		}
#endif
	}

	void DrawDetail(int baseX, int baseY, const MUSIC_BOX_2 &songdata, int dif) const {
		const TCHAR starChar[2][2] = { _T("★"),_T("☆")};
		DrawGraphAnchor(baseX, baseY, this->detail.handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
		DrawFormatStringToHandleAnchor(baseX - 330, baseY - 170, COLOR_BLACK, SmallFontData,
			DXDRAW_ANCHOR_BOTTOM_RIGHT, L"%s", songdata.packName.c_str());
		DrawFormatStringToHandleAnchor(baseX - 325, baseY - 145, COLOR_BLACK, SmallFontData,
			DXDRAW_ANCHOR_BOTTOM_RIGHT, L"Lv.%2d", songdata.level);
		for (int i = 0; i < 15; i++) {
			int temp = 0;
			if (10 <= i && songdata.level <= i) { break; }
			temp = (i < songdata.level) ? 0 : 1;
			DrawStringToHandleAnchor(baseX + 16 * i - 270, baseY - 145, starChar[temp],
				COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_BOTTOM_RIGHT);
		}
		DrawFormatStringToHandleAnchor(baseX - 320, baseY - 120, COLOR_BLACK, SmallFontData,
			DXDRAW_ANCHOR_BOTTOM_RIGHT, L"HighSCORE:%6d/%6.2f%%/%5.3fkm",
			songdata.Hscore, songdata.Hacc, songdata.Hdis / 1000.0);
	}

public:
	void SlideDif(int vect) {
		this->LR = vect;
		this->XstartC = GetNowCount();
	}

	void FetchDifPic(const tstring &difpath) {
		if (strands(this->viewingDifBar.c_str(), difpath.c_str()) == 1) { return; }

		DeleteGraph(this->difbar[4].handle());
		DeleteGraph(this->difbar[5].handle());

		this->difbar[4].reload(difpath);
		if (this->difbar[4].handle() == -1) {
			DeleteGraph(this->difbar[4].handle());
			this->difbar[4].reload(L"picture/difanother.png");
			this->difbar[5] = this->difbar[4];
			this->viewingDifBar = _T("NULL");
		}
		else {
			this->difbar[5] = this->difbar[4];
			this->viewingDifBar = difpath;
		}
	}

	void DrawDetailAll(int baseX, int baseY, const MUSIC_BOX_2 &songdata, int dif) const {
		this->DrawDifMark(baseX - 100, baseY - 365,  songdata,      dif);
		this->DrawDifBar( baseX -  15, baseY - 320,                 dif);
		this->DrawDifMpal(baseX -  20, baseY - 186, &songdata.mpal, dif);
		this->DrawDetail( baseX      , baseY      ,  songdata,      dif);
	}
};

static class rec_serect_jacket_c {
private:
	dxcur_pic_c jacketpic = dxcur_pic_c(_T("picture/NULL jucket.png"));
	tstring viewingjacket = { L"picture/NULL jucket.png" };

public:
	void UpdateJacket(const tstring &jacketName) {
		if (strands(this->viewingjacket.c_str(), jacketName.c_str()) != 0) { return; }
		DeleteGraph(this->jacketpic.handle());
		this->viewingjacket = jacketName;
		this->jacketpic.reload(this->viewingjacket);
	}

	void DrawJacket(int baseX, int baseY, int size) const {
		DrawExtendGraph(baseX, baseY, baseX + size, baseY + size, this->jacketpic.handle(), TRUE);
	}
};

static class rec_serect_snd_c {
private:
	dxcur_snd_c sound;

public:
	rec_serect_snd_c() {
		this->sound.SetSound(_T("sound/arrow.wav"));
		this->sound.SetVolume(optiondata.SEvolume * 255 / 10);
	}

	void PlaySnd() {
		this->sound.PlaySound();
	}
};

static class rec_serect_ui_c {
private:

public:
	rec_serect_backpic_c backpic;
	rec_serect_jacket_c jacket;
	rec_serect_musicbar_c musicbar;
	rec_serect_disk_c disk;
	rec_serect_detail_c detail;
	rec_helpbar_c help;
	rec_cutin_c cutin;
	rec_serect_preview_sound_c previewSnd;
	rec_serect_snd_c snd;

	rec_serect_ui_c() {}

	~rec_serect_ui_c() {}

	void InitUi(const MUSIC_BOX_2 &songdata, int dif) {
		this->jacket.UpdateJacket(songdata.jacketP);
		this->detail.FetchDifPic(songdata.difP);
		this->previewSnd.UpdateSnd(songdata, dif);
		this->previewSnd.StartSnd();
	}

	void Update4th(const tstring &jacketName) {
		jacket.UpdateJacket(jacketName);
		previewSnd.SetPresc(GetNowCount());
		snd.PlaySnd();
	}

	void UpdateUD(const MUSIC_BOX_2 &songdata, int dif, int vect) {
		this->detail.FetchDifPic(songdata.difP);
		this->musicbar.SlideBar(vect);
		this->disk.SlideDisk(vect);
		this->Update4th(songdata.jacketP);
	}

	void UpdateLR(songdata_set_t &songdata, int *cmd, int dif, int vect) {
		this->detail.SlideDif(vect);
		SortSongWithSave(songdata, dif, *cmd);
		this->Update4th(songdata[*cmd].jacketP);
	}

	void DrawUi(const rec_select_command_st &cmd, songdata_set_t &songdata) {
		this->backpic.DrawBackPic();
		this->jacket.DrawJacket(380, 85, 500);
		this->musicbar.DrawAll(300, cmd, songdata, false);
		this->disk.DrawDiskSet(-30, 25, songdata.sortMode);
		this->detail.DrawDetailAll(20, 60, songdata[cmd.music], cmd.dif);
		this->previewSnd.CheckTime();
		this->previewSnd.CheckSnd(songdata[cmd.music], cmd.dif);
		this->help.DrawHelp(rec_helpbar_type_ec::SELECT);
		this->cutin.DrawCut();
	}
};

#endif /* ui class */

#if 1 /* after class action */

static void RecSerectKeyActLR(rec_select_command_st &cmd, int vect,
	rec_serect_ui_c &uiClass, songdata_set_t &songdata)
{
	switch (vect) {
	case REC_SELECT_VECT_LEFT:
		cmd.dif--;
		if (cmd.dif < 0) {
			cmd.dif = 0;
			return;
		}
		break;
	case REC_SELECT_VECT_RIGHT:
		cmd.dif++;
		if (cmd.dif > 4) {
			cmd.dif = 4;
			return;
		}
		break;
	default:
		return;
	}

	uiClass.UpdateLR(songdata, &cmd.music, cmd.dif, vect);

	return;
}

static void RecSerectKeyActUD(rec_select_command_st &cmd, int vect,
	rec_serect_ui_c &uiClass, songdata_set_t &songdata)
{
	switch (vect) {
	case REC_SELECT_VECT_UP:
		cmd.music--;
		if (cmd.music < 0) { cmd.music = songdata.sort.size() - 1; }
		break;
	case REC_SELECT_VECT_DOWN:
		cmd.music++;
		if (cmd.music >= songdata.sort.size()) { cmd.music = 0; }
		break;
	default:
		return;
	}

	uiClass.UpdateUD(songdata[cmd.music], cmd.dif, vect);

	return;
}

static void RecSerectKeyActAll(now_scene_t &next, rec_to_play_set_t &toPlay,
	rec_select_command_st &cmd, int &CutTime, rec_serect_ui_c &uiClass, songdata_set_t &songdata)
{
	rec_select_key_et key = REC_SELECT_KEY_NONE;

	/* 操作検出*/
	if (uiClass.cutin.IsClosing() == 0) { key = RecSerectKeyCheck(); }
	else { key = REC_SELECT_KEY_NONE; }

	/* 動作 */
	switch (key) {
	case REC_SELECT_KEY_RETURN:
		// 選択できる曲であるかどうか (Lvが0以上であるかで判定)
		if (songdata[cmd.music].level < 0) { break; }
		RecSerectSetToPlay(toPlay, cmd, songdata);
		next = SCENE_MUSIC;
		uiClass.cutin.SetCutTipFg(CUTIN_TIPS_SONG);
		uiClass.cutin.SetCutSong(songdata[cmd.music].SongName.get_str(),
			songdata[cmd.music].jacketP);
		uiClass.cutin.SetIo(CUT_FRAG_IN);
		break;
	case REC_SELECT_KEY_BACK:
		next = SCENE_MENU;
		uiClass.cutin.SetTipNo();
		uiClass.cutin.SetCutTipFg(CUTIN_TIPS_ON);
		uiClass.cutin.SetIo(CUT_FRAG_IN);
		break;
	case REC_SELECT_KEY_UP:
		RecSerectKeyActUD(cmd, REC_SELECT_VECT_UP,    uiClass, songdata);
		break;
	case REC_SELECT_KEY_DOWN:
		RecSerectKeyActUD(cmd, REC_SELECT_VECT_DOWN,  uiClass, songdata);
		break;
	case REC_SELECT_KEY_LEFT:
		RecSerectKeyActLR(cmd, REC_SELECT_VECT_LEFT,  uiClass, songdata);
		break;
	case REC_SELECT_KEY_RIGHT:
		RecSerectKeyActLR(cmd, REC_SELECT_VECT_RIGHT, uiClass, songdata);
		break;
	case REC_SELECT_KEY_SORT:
		++songdata.sortMode;
		SortSongWithSave(songdata, cmd.dif, cmd.music);
		break;
	case REC_SELECT_KEY_RELORD:
		next = SCENE_RELOAD;
		break;
	default:
		break;
	}

	return;
}

#endif /* after class action */

static now_scene_t musicselect2(rec_to_play_set_t &toPlay) {
	/* int */
	rec_select_command_st cmd;
	int buf = 0;
	int CutTime = 0;

	/* typedef */
	now_scene_t next = SCENE_EXIT;
	songdata_set_t songdata;

	/* class */
	rec_serect_ui_c uiClass;

	RecSerectLoadBefCmd(cmd, songdata.sortMode);
	SortSong(songdata, cmd.dif);
	uiClass.InitUi(songdata[cmd.music], cmd.dif);
	GetMouseWheelRotVol();
	while (GetMouseInputLog2(NULL, NULL, NULL, NULL, true) == 0) {}
	uiClass.cutin.SetIo(CUT_FRAG_OUT);

	while (1) {
		ClearDrawScreen();
		uiClass.DrawUi(cmd, songdata);
		ScreenFlip();
		RecSerectKeyActAll(next, toPlay, cmd, CutTime, uiClass, songdata);
		if (next == SCENE_RELOAD) { return SCENE_RELOAD; }
		if (uiClass.cutin.IsEndAnim()) { break; }
		if (GetWindowUserCloseFlag(TRUE)) {
			next = SCENE_EXIT;
			break;
		}
		WaitTimer(WAIT_TIME_ON_GAMELOOP);
	}

	RecSerectSaveBefCmd(cmd, songdata.sortMode);

	INIT_SND();

	return next;
}

now_scene_t musicserect(rec_to_play_set_t *toPlay) {
	now_scene_t next = SCENE_EXIT;
	while (1) {
		next = musicselect2(*toPlay);
		/* RecSelectAllRelordを実行するためにはmusicserect2から抜ける必要がある。スタックオーバーフローを起こすから */
		if (next == SCENE_RELOAD) { RecSelectAllRelord(); }
		else { break; }
	}
	return next;
}
