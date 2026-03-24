
#if 1 /* include */

/* base include */
#include <DxLib.h>

/* curbine code include */
#include <dxcur.h>
#include <dxdraw.h>
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <helpBar.h>
#include <option.h>
#include <playbox.h>
#include <recr_cutin.h>
#include <RecSave.h>
#include <RecScoreFile.h>
#include <RecSystem.h>
#include <RecWindowRescale.h>
#include <RecSystem.h>

/* others include */
#include <RecordLoad2.h>
#include <recp_cal_ddif_2.h>

/* own include */
#include <musicserect2.h>

#endif /* include */

#if 1 /* define */

/* TODO: レベルフィルター周りの実装 */
#define PackNumLim 16
#define MapNumLim  128 /* とりあえず128譜面とする、現時点収録で結構ギリギリ */
#define REC_SORT_DEFAULT 0
#define SORT_LEVEL 1
#define SORT_SCORE 2

#define MUSE_FADTM 250
#define MUSE_KEYTM 500

#define REC_SERECT_KEY_RETURN 1
#define REC_SERECT_KEY_BACK   2
#define REC_SERECT_KEY_UP     3
#define REC_SERECT_KEY_DOWN   4
#define REC_SERECT_KEY_LEFT   5
#define REC_SERECT_KEY_RIGHT  6
#define REC_SERECT_KEY_SORT   7
#define REC_SERECT_KEY_RELORD 8

#define REC_SERECT_VECT_UP    -1
#define REC_SERECT_VECT_DOWN   1
#define REC_SERECT_VECT_LEFT   1
#define REC_SERECT_VECT_RIGHT -1

#endif /* define */

typedef TCHAR rec_pack_name_set_t[256];

typedef struct music_box_2 {
	int level      = -1;
	int LvType     =  1; // 0=auto, 1=easy, 2=normal, 3=hard, 4=another, 5=secret
	int preview[2] = {0, 10000};
	int Hscore     =  0;
	int Hdis       =  0;
	int ScoreRate  =  6; //0=EX, 1=S, 2=A, 3=B, 4=C, 5=D, 6=not play
	int ClearRank  =  0; //0=not play, 1=droped, 2=cleared, 3=no miss!, 4=full combo!!, 5=perfect!!!
	int packNo     =  0;
	int musicNo    =  0;
	double Hacc    =  0.0;
	TCHAR difP[256]         = _T("");
	TCHAR packName[64]      = _T("");
	TCHAR SongName[64]      = _T("");
	TCHAR artist[64]        = _T("");
	TCHAR SongFileName[256] = _T("");
	TCHAR jacketP[256]      = _T("");
	rec_ddif_pal_t mpal;
} MUSIC_BOX_2;

/* TODO: 難易度切り替えで名前解決できない。ジャケット更新されない。もっとひどいと範囲外アクセスしようとする。 */
class rec_serect_music_set_c {
public:
	std::vector<MUSIC_BOX_2> detail;
	std::vector<uint> sort;
	int sortMode = REC_SORT_DEFAULT;

public: /* 並び替え系 */
	/**
	 * @brief 現在の this->sort の内容をハイスコア順に並び替える
	 * @param なし
	 * @return なし
	 */
	void SortByHScore(void) {
		if (this->sort.empty()) { return; }
		for (int is = 0; is + 1 < (this->sort.size()); is++) {
			for (int ie = is + 1; ie < this->sort.size(); ie++) {
				if (this->detail[this->sort[is]].Hscore >
					this->detail[this->sort[ie]].Hscore)
				{
					uint temp = this->sort[is];
					this->sort[is] = this->sort[ie];
					this->sort[ie] = temp;
				}
			}
		}
	}

	/**
	 * @brief 現在の this->sort の内容を難易度順に並び替える
	 * @param なし
	 * @return なし
	 */
	void SortByLevel(void) {
		if (this->sort.empty()) { return; }
		for (int is = 0; is + 1 < (this->sort.size()); is++) {
			for (int ie = is + 1; ie < this->sort.size(); ie++) {
				if (this->detail[this->sort[is]].level >
					this->detail[this->sort[ie]].level)
				{
					uint temp = this->sort[is];
					this->sort[is] = this->sort[ie];
					this->sort[ie] = temp;
				}
			}
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
	MUSIC_BOX_2& operator[](int n) {
		return this->detail[sort[betweens(0, n, sort.size() - 1)]];
	}

	const MUSIC_BOX_2& operator[](int n) const {
		return this->detail[sort[betweens(0, n, sort.size() - 1)]];
	}

	MUSIC_BOX_2& at(int n) {
		return this->detail[sort[betweens(0, n, sort.size() - 1)]];
	}

	const MUSIC_BOX_2& at(int n) const {
		return this->detail[sort[betweens(0, n, sort.size() - 1)]];
	}
};
typedef rec_serect_music_set_c songdata_set_t;

#if 1 /* all relord */

static void RecSelectAllRelordDrawInfo(uint iPack, uint iSong, uint iDif) {
	const TCHAR difName[4][16] = { _T("AUTO"), _T("EASY"), _T("NORMAL"), _T("HARD") }; /* anotherは非表示 */
	static DxTime_t Btime = 0;
	TCHAR songN[255] = _T("");

	if (((Btime + 50) >= GetNowCount()) || (iDif >= 4)) { return; }
	if (RecGetMusicFolderName(songN, ARRAY_COUNT(songN), iPack, iSong) != 0) { return; }

	Btime = GetNowCount();
	ClearDrawScreen();
	/* TODO: 背景くらいは欲しい */
	DrawFormatString(5, 5, COLOR_WHITE, L"all record relording...\n%s[%s]", songN, difName[iDif]);
	ScreenFlip();
	return;
}

static void RecSelectAllRelord(void) {
	TCHAR path[255];

	for (uint iPack = 0; iPack < 10; iPack++) {
		for (uint iSong = 0; iSong < 20; iSong++) {
			for (uint iDif = 0; iDif < 5; iDif++) {
				RecSelectAllRelordDrawInfo(iPack, iSong, iDif);
				RecordLoad2(iPack, iSong, iDif);
				if (RecGetMusicMapRrsPath(path, 255, iPack, iSong, (rec_dif_t)iDif) != 0) { continue; }
				cal_ddif_3(path);
			}
		}
	}
	return;
}

#endif

#if 1 /* read map data */

static rec_error_t RecSerectReadMapDataOneDif(MUSIC_BOX_2 *songdata, const TCHAR *path,
	const TCHAR *subpath, const TCHAR *packName, int packNum, int musicNo, int dif)
{
	DxFile_t fd;
	TCHAR buf[256];
	int lang = optiondata.lang;

	fd = FileRead_open(path);

	if (fd == 0) { return REC_ERROR_FILE_EXIST; }

	//初期値定義
	songdata->LvType     =     dif;
	songdata->level      =      -1;
	songdata->preview[0] =  441000;
	songdata->preview[1] = 2646000;
	songdata->packNo     = packNum;
	songdata->musicNo    = musicNo;
	strcopy_2(L"NULL",                    songdata->SongName,      64);
	strcopy_2(L"NULL",                    songdata->artist,        64);
	strcopy_2(L"NULL",                    songdata->SongFileName, 255);
	strcopy_2(L"picture/NULL jucket.png", songdata->jacketP,      255);
	strcopy_2(packName,                   songdata->packName,      64);

	while (FileRead_eof(fd) == 0) {
		FileRead_gets(buf, 256, fd);
		//曲名を読み込む
		if (strands_direct(buf, L"#TITLE:")) {
			strmods(buf, 7);
			if ((lang == 0) || strands_direct(songdata->SongName, L"NULL")) {
				strcopy_2(buf, songdata->SongName, 64);
			}
		}
		else if (strands_direct(buf, L"#E.TITLE:")) {
			strmods(buf, 9);
			if ((lang == 1) || strands_direct(songdata->SongName, L"NULL")) {
				strcopy_2(buf, songdata->SongName, 64);
			}
		}
		//作曲者を読み込む
		else if (strands_direct(buf, L"#ARTIST:")) {
			strmods(buf, 8);
			if ((lang == 0) || strands_direct(songdata->artist, L"NULL")) {
				strcopy_2(buf, songdata->artist, 64);
			}
		}
		else if (strands_direct(buf, L"#E.ARTIST:")) {
			strmods(buf, 10);
			if ((lang == 1) || strands_direct(songdata->artist, L"NULL")) {
				strcopy_2(buf, songdata->artist, 64);
			}
		}
		//曲ファイル名を読み込む
		else if (strands_direct(buf, L"#MUSIC:")) {
			strmods(buf, 7);
			strcopy_2(subpath, songdata->SongFileName, 255);
			strcats(songdata->SongFileName, buf);
		}
		//難易度を読み込む
		else if (strands_direct(buf, L"#LEVEL:")) {
			strmods(buf, 7);
			songdata->level = strsans(buf);
		}
		//プレビュー時間を読み込む
		else if (strands_direct(buf, L"#PREVIEW:")) {
			strmods(buf, 9);
			songdata->preview[0] = (int)((double)strsans(buf) / 1000.0 * 44100.0);
			strnex(buf);
			if (L'0' <= buf[1] && buf[1] <= L'9') {
				songdata->preview[1] = (int)((double)strsans(buf) / 1000.0 * 44100.0);
			}
		}
		//ジャケット写真を読み込む
		else if (strands_direct(buf, L"#JACKET:")) {
			strmods(buf, 8);
			strcopy_2(subpath, songdata->jacketP, 255);
			strcats(songdata->jacketP, buf);
		}
		//差し替えAnotherバーを読み込む
		else if (strands_direct(buf, L"#DIFBAR:")) {
			strmods(buf, 8);
			strcopy_2(subpath, songdata->difP, 255);
			strcats(songdata->difP, buf);
		}
		//マップに入ったら抜ける
		else if (strands_direct(buf, L"#MAP:")) { break; }
	}
	FileRead_close(fd);

	return REC_ERROR_NONE;
}

/* TODO: 初期値が正しく代入されない */
static void RecSerectReadHighscore(MUSIC_BOX_2 *songdata, const TCHAR *songName, rec_dif_t dif) {
	rec_save_score_t score;
	RecSaveReadScoreOneDif(&score, songName, dif);
	songdata->Hscore    = score.score;
	songdata->Hacc      = score.acc;
	songdata->Hdis      = score.dist;
	songdata->ScoreRate = score.scoreRate;
	songdata->ClearRank = score.clearRank;
	return;
}

static void RecSerectReadMapDataOneSong(songdata_set_t *dataset,
	const TCHAR *packName, const TCHAR *songName, int packNum, int musicNo)
{
	rec_error_t status = REC_ERROR_NONE;
	TCHAR path[256];
	TCHAR subPath[256];
	TCHAR rrsPath[256];
	MUSIC_BOX_2 buf;
	for (int iDif = 0; iDif < 6; iDif++) {
		strcopy_2(L"record/", path, 255); //"record/"
		strcats(path, packName); //"record/<パック名>"
		stradds(path, L'/'); //"record/<パック名>/"
		strcats(path, songName); //"record/<パック名>/<曲名>"
		stradds(path, L'/'); //"record/<パック名>/<曲名>/"
		strcopy_2(path, subPath, 255); //subPathにコピー
		stradds(path, (TCHAR)((int)_T('0') + iDif)); //"record/<パック名>/<曲名>/<難易度番号>"
		strcopy_2(path, rrsPath, 255); //rrsPathにコピー
		strcats(path, _T(".txt")); //"record/<パック名>/<曲名>/<難易度番号>.txt"
		strcats(rrsPath, _T(".rrs")); //"record/<パック名>/<曲名>/<難易度番号>.txt"
		status = RecSerectReadMapDataOneDif(
			&buf, path, subPath, packName, packNum, musicNo, iDif);
		if (status == REC_ERROR_NONE) {
			RecScoreReadDdif(&buf.mpal, rrsPath);
			RecSerectReadHighscore(&buf, songName, (rec_dif_t)iDif);
			dataset->detail.push_back(buf);
		}
	}
	return;
}

/**
 * ソングデータを読み込む
 * @param[out] songdata ret
 * @param[out] PackFirstNum ret
 * @param[out] Mapping ret
 */
static void RecSerectReadMapData(songdata_set_t *songdata, int PackFirstNum[]) {
	int packNum = 0;
	int musicNo = 0;
	int songCount = 0;
	DxFile_t fd;
	rec_pack_name_set_t PackName[PackNumLim];
	TCHAR songName[256];

	fd = FileRead_open(L"RecordPack.txt");
	while (FileRead_eof(fd) == 0) {
		FileRead_gets(songName, 256, fd);
		strcopy_2(songName, PackName[packNum], 255);
		packNum++;
	}
	FileRead_close(fd);

	songCount = 0;
	for (int i = 0; i < packNum; i++) {
		strcopy_2(L"record/", songName, 255); //"record/"
		strcats(songName, PackName[i]); //"record/<パック名>"
		strcats(songName, L"/list.txt"); //"record/<パック名>/list.txt"
		fd = FileRead_open(songName);
		PackFirstNum[i] = songCount;
		musicNo = 0;
		while (FileRead_eof(fd) == 0) {
			FileRead_gets(songName, 256, fd);
			RecSerectReadMapDataOneSong(songdata, PackName[i], songName, i, musicNo);
			musicNo++;
		}
		FileRead_close(fd);
	}

	return;
}

#endif /* read map data */

#if 1 /* sub action */

static void ChangeSortMode(int *mode) {
	*mode = *mode + 1;
	if (*mode >= 3) {
		*mode = 0;
	}
}

static int RecSerectKeyCheck() {
	int ret = 0;
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
			ret = REC_SERECT_KEY_RETURN;
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
	if (1 <= mouseHoil)  { ret = REC_SERECT_KEY_UP;   } /* 奥回し */
	if (mouseHoil <= -1) { ret = REC_SERECT_KEY_DOWN; } /* 手前回し */

	/* キー入力 */
	InputAllKeyHold();
	switch (GetKeyPushOnce()) {
	case KEY_INPUT_RETURN:
		ret = REC_SERECT_KEY_RETURN;
		break;
	case KEY_INPUT_BACK:
		ret = REC_SERECT_KEY_BACK;
		break;
	case KEY_INPUT_UP:
		ret = REC_SERECT_KEY_UP;
		break;
	case KEY_INPUT_DOWN:
		ret = REC_SERECT_KEY_DOWN;
		break;
	case KEY_INPUT_LEFT:
		ret = REC_SERECT_KEY_LEFT;
		break;
	case KEY_INPUT_RIGHT:
		ret = REC_SERECT_KEY_RIGHT;
		break;
	case KEY_INPUT_Z:
		ret = REC_SERECT_KEY_SORT;
		break;
	case KEY_INPUT_F5:
		ret = REC_SERECT_KEY_RELORD;
		break;
	default:
		break;
	}

	return ret;
}

static int RecSerectTrySecret(int Hscore) {
	int ret = 0;
	int rate = 0;
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
	else if (Hscore >= 99000) { rate = 1000; }
	if (GetRand(1000) <= rate) { ret = 1; }
	return ret;
}

#if 0
static int RecSerectTrySecret2(int AutoFlag, int dif, MUSIC_BOX *songdata) {
	if (AutoFlag == 0 && dif == 3 &&
		songdata->Hscore[3] >= 90000 &&
		strands_direct(songdata->SongFileName[5], L"NULL") == 0 &&
		songdata->Hscore[5] <= 0)
	{
		if (RecSerectTrySecret(songdata->Hscore[3]) == 1) {
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
static void SortSong(songdata_set_t *songdata, int dif) {
	songdata->Search(Rec_Select_DifFilter, dif);
	switch (songdata->sortMode) {
	case SORT_LEVEL:
		songdata->SortByLevel();
		break;
	case SORT_SCORE:
		songdata->SortByHScore();
		break;
	case REC_SORT_DEFAULT:
		break;
	}
	return;
}

/**
 * SortSongで並び替えをした後、cmdを前と同じ曲に合わせます
 * @param[in] songdata 曲データ
 * @param[in] mode 並び替えの方法
 * @param[in] dif 難易度
 * @param[out] cmd commandの格納場所
 */
static void SortSongWithSave(songdata_set_t *songdata, int dif, int *cmd) {
	tstring save;

	save = (*songdata)[betweens(0, *cmd, songdata->sort.size() - 1)].SongName;
	SortSong(songdata, dif);
	for (int i = 0; i < songdata->sort.size(); i++) {
		if (save == (*songdata)[i].SongName) {
			*cmd = i;
		}
	}
	(*cmd) = min(*cmd, songdata->sort.size() - 1);
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

static void RecSerectLoadBefCmd(int *cmd, int *sortMode) {
	FILE *fp;
	_wfopen_s(&fp, L"save/SongSelect2.dat", L"rb");
	if (fp != NULL) {
		fread(cmd, sizeof(int), 2, fp);
		fread(sortMode, sizeof(int), 1, fp);
		fclose(fp);
	}
	return;
}

static void RecSerectSaveBefCmd(int *cmd, int sortMode) {
	FILE *fp;
	_wfopen_s(&fp, L"save/SongSelect2.dat", L"wb");
	if (fp != NULL) {
		fwrite(cmd, sizeof(int), 2, fp);
		fwrite(&sortMode, sizeof(int), 1, fp);
		fclose(fp);
	}
	return;
}

static void RecSerectSetToPlay(rec_to_play_set_t *toPlay, int cmd[],
	const int PackFirstNum[], songdata_set_t *songdata)
{
	int inum = 0;

	if (CheckHitKey(KEY_INPUT_LSHIFT) == 1 || CheckHitKey(KEY_INPUT_RSHIFT) == 1) {
		toPlay->shift = 1;
	}
	else { toPlay->shift = 0; }
	if (CheckHitKey(KEY_INPUT_P) == 1) { toPlay->autoFg = 1; }
	else { toPlay->autoFg = 0; }
	toPlay->packNo  = (*songdata)[cmd[0]].packNo;
	toPlay->musicNo = (*songdata)[cmd[0]].musicNo;
	toPlay->dif     = (*songdata)[cmd[0]].LvType;

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
	dxcur_pic_c back = dxcur_pic_c(_T("picture/MSback.png"));
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
	int preTime[2] = { 441000,2646000 };
	TCHAR playingsong[256] = { L"NULL" };

public:
	void StartSnd() {
		rec_bgm_system_g.SetCurrentPosition(this->preTime[this->SongPrePat]);
		rec_bgm_system_g.SetVolume(0);
		rec_bgm_system_g.Play(true, false, false);
		WaitTimer(WAIT_TIME_AFTER_MUSICPLAY);
		this->SongPreSTime = GetNowCount();
	}

	int UpdateSnd(MUSIC_BOX_2 *songdata, int dif) {
		if ((strands_direct(songdata->SongFileName, L"NULL") != 0) ||
			(strands(this->playingsong, songdata->SongFileName) != 0))
		{
			return 0;
		}
		rec_bgm_system_g.Delete();
		strcopy_2(songdata->SongFileName, this->playingsong, 255);
		rec_bgm_system_g.SetMem(this->playingsong);
		this->SongPrePat = 0;
		this->preTime[0] = songdata->preview[0];
		this->preTime[1] = songdata->preview[1];
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

	void CheckSnd(MUSIC_BOX_2 *songdata, int dif) {
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

	int UD = REC_SERECT_VECT_DOWN;
	int startC = -MUSE_FADTM;
	dxcur_pic_c bar[2] = {
		dxcur_pic_c(L"picture/songbarB.png"), dxcur_pic_c(L"picture/songbarY.png")
	};
	dxcur_pic_c folder_bar[2] = {
		dxcur_pic_c(L"picture/songbarB.png"), dxcur_pic_c(L"picture/songbarY.png") /* TODO: 画像作る */
	};
	dxcur_pic_c CRate[5] = {
		dxcur_pic_c(L"picture/MarkD.png"),
		dxcur_pic_c(L"picture/MarkC.png"),
		dxcur_pic_c(L"picture/MarkNM.png"),
		dxcur_pic_c(L"picture/MarkFC.png"),
		dxcur_pic_c(L"picture/MarkP.png")
	};
	dxcur_pic_c rankP[6] = {
		dxcur_pic_c(L"picture/MiniEX.png"),
		dxcur_pic_c(L"picture/MiniS.png"),
		dxcur_pic_c(L"picture/MiniA.png"),
		dxcur_pic_c(L"picture/MiniB.png"),
		dxcur_pic_c(L"picture/MiniC.png"),
		dxcur_pic_c(L"picture/MiniD.png")
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

	void DrawMainFolderOne(int dif, int BasePosX, int BasePosY, const TCHAR *str) const {
		DrawGraph(BasePosX - 120, BasePosY - 170, this->folder_bar[1].handle(), TRUE);
		DrawStringToHandle(BasePosX - 30, BasePosY - 157, str , COLOR_BLACK, SmallFontData);
	}

	void DrawSubFolderOne(int dif, int BasePosX, int BasePosY, const TCHAR *str) const {
		DrawGraph(BasePosX - 120, BasePosY - 170, this->folder_bar[0].handle(), TRUE);
		DrawStringToHandle(BasePosX - 30, BasePosY - 157, str, COLOR_WHITE, SmallFontData);
	}

	void DrawMainOne(int dif, int BasePosX, int BasePosY, const MUSIC_BOX_2 &songdata) const {
		DrawGraph(BasePosX - 120, BasePosY - 170, this->bar[1].handle(), TRUE);
		DrawStringToHandle(BasePosX - 30, BasePosY - 157, songdata.SongName, COLOR_BLACK, SmallFontData);
		DrawStringToHandle(BasePosX - 30, BasePosY - 129, songdata.artist, COLOR_BLACK, SmallFontData);
		this->DrawClear(BasePosX + 156, BasePosY - 132, songdata.ClearRank - 1);
		this->DrawRack(BasePosX + 156, BasePosY - 132, songdata.ScoreRate);
		for (int idif = 0; idif < 3; idif++) {
			DrawFormatStringToHandle(BasePosX - 25 + idif * 70, BasePosY - 97,
				COLOR_BLACK, SmallFontData, L"%2d", songdata.level);
		}
	}

	void DrawSubOne(int dif, int BasePosX, int BasePosY, const MUSIC_BOX_2 &songdata) const {
		DrawGraph(BasePosX - 120, BasePosY - 170, this->bar[0].handle(), TRUE);
		DrawStringToHandle(BasePosX - 30, BasePosY - 157, songdata.SongName, COLOR_WHITE, SmallFontData);
		DrawStringToHandle(BasePosX - 30, BasePosY - 129, songdata.artist, COLOR_WHITE, SmallFontData);
		this->DrawClear(BasePosX + 152, BasePosY - 163, songdata.ClearRank - 1);
		this->DrawRack(BasePosX + 152, BasePosY - 163, songdata.ScoreRate);
	}

public:
	void SlideBar(int vect) {
		this->UD = vect;
		this->startC = GetNowCount();
	}

	void DrawAll(int Ypos, int cmd[], const songdata_set_t &songdata, bool is_folder) const {
		int BasePosX = 0;
		int BasePosY = 0;
		int slide = 0;
		int picsong = 0;
		int moveC = 0;

		moveC = maxs_2(-1 * (GetNowCount() - this->startC) + MUSE_FADTM, 0);
		picsong = (cmd[0] + songdata.sort.size() - (VIEW_COUNT / 2)) % songdata.sort.size();

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
					this->DrawMainFolderOne(cmd[1], BasePosX, BasePosY, this->folder_str[picsong].c_str());
				}
				else {
					this->DrawMainOne(cmd[1], BasePosX, BasePosY, songdata[picsong]);
				}
			}
			else {
				if (is_folder) {
					this->DrawSubFolderOne(cmd[1], BasePosX, BasePosY, this->folder_str[picsong].c_str());
				}
				else {
					this->DrawSubOne(cmd[1], BasePosX, BasePosY, songdata[picsong]);
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
	int UD = REC_SERECT_VECT_DOWN;
	int startC = -MUSE_FADTM;
	intx100_t rate = 0;
	double Nrot = 0.0;
	dxcur_pic_c disk = dxcur_pic_c(_T("picture/disk.png"));
	dxcur_pic_c runner;
	dxcur_pic_c rateBar;

	void DrawNamePlate(int baseX, int baseY) {
		DrawGraphAnchor(baseX - 30, baseY, this->rateBar.handle(), DXDRAW_ANCHOR_TOP_RIGHT);
		DrawGraphAnchor(baseX, baseY, this->runner.handle(), DXDRAW_ANCHOR_TOP_RIGHT);
		DrawFormatStringToHandleAnchor(baseX - 220, baseY + 12, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT, L"Lv:%2d", this->Lv);
		DrawFormatStringToHandleAnchor(baseX - 215, baseY + 37, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT, L"RATE:%d.%02d", this->rate / 100, this->rate % 100);
	}

	void DrawDisk(int baseX, int baseY) {
		int moveC = 0;

		moveC = maxs_2(-1 * (GetNowCount() - this->startC) + MUSE_FADTM, 0);
		if (this->UD == REC_SERECT_VECT_DOWN) { this->Nrot += pals(0, 2, MUSE_FADTM, -75, moveC) / 100.0; }
		else { this->Nrot += pals(0, 2, MUSE_FADTM, 75, moveC) / 100.0; }
		if (this->Nrot > 6.28) { this->Nrot -= 6.28; }
		else if (this->Nrot < 0) { this->Nrot += 6.28; }
		DrawRotaGraphAnchor(baseX, baseY, 1, this->Nrot, this->disk.handle(), DXDRAW_ANCHOR_TOP_RIGHT, TRUE);
	}

	void DrawSort(int baseX, int baseY, int mode) {
		switch (mode) {
		case REC_SORT_DEFAULT:
			DrawStringToHandleAnchor(baseX, baseY, REC_STR_LANG(_T("デフォルト"), _T("default")), COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
			break;
		case SORT_LEVEL:
			DrawStringToHandleAnchor(baseX, baseY, REC_STR_LANG(_T("レベル順"), _T("level")), COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
			break;
		case SORT_SCORE:
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
			this->runner.reload(L"picture/Mpicker.png");
			this->Lv = buf.picker;
			break;
		case 1:
			this->runner.reload(L"picture/Mgator.png");
			this->Lv = buf.mapgator;
			break;
		case 2:
			this->runner.reload(L"picture/Mtaylor.png");
			this->Lv = buf.taylor;
			break;
		default:
			this->Lv = 0;
			break;
		}
		this->Lv++;

		if (this->rate < 2500) {
			this->rateBar.reload(L"picture/MSrate1.png");
		}
		else if (2500 <= this->rate && this->rate < 5500) {
			this->rateBar.reload(L"picture/MSrate2.png");
		}
		else if (5500 <= this->rate && this->rate < 9000) {
			this->rateBar.reload(L"picture/MSrate3.png");
		}
		else if (9000 <= this->rate && this->rate < 12000) {
			this->rateBar.reload(L"picture/MSrate4.png");
		}
		else {
			this->rateBar.reload(L"picture/MSrate5.png");
		}
	}

private:
public:
	void SlideDisk(int vect) {
		this->UD = vect;
		this->startC = GetNowCount();
	}

	void DrawDiskSet(int baseX, int baseY, int mode) {
		this->DrawNamePlate(baseX - 115, baseY - 20);
		this->DrawDisk(baseX, baseY);
		this->DrawSort(baseX - 60, baseY + 85, mode);
	}
};

static class rec_serect_detail_c {
private:
	int LR = REC_SERECT_VECT_LEFT;
	int XstartC = -MUSE_FADTM;
	TCHAR viewingDifBar[255] = { L"NULL" };
	dxcur_pic_c difbar[6] = {
		dxcur_pic_c(L"picture/difauto.png"),
		dxcur_pic_c(L"picture/difeasy.png"),
		dxcur_pic_c(L"picture/difnormal.png"),
		dxcur_pic_c(L"picture/difhard.png"),
		dxcur_pic_c(L"picture/difanother.png"),
		dxcur_pic_c(L"picture/difanother.png")
	};
	dxcur_pic_c detail = dxcur_pic_c(_T("picture/detail.png"));
	dxcur_pic_c mpalNamePic = dxcur_pic_c(_T("picture/mpalName.png"));
	dxcur_pic_c difC[10] = {
		dxcur_pic_c(L"picture/Dif0S.png"),
		dxcur_pic_c(L"picture/Dif0B.png"),
		dxcur_pic_c(L"picture/Dif1S.png"),
		dxcur_pic_c(L"picture/Dif1B.png"),
		dxcur_pic_c(L"picture/Dif2S.png"),
		dxcur_pic_c(L"picture/Dif2B.png"),
		dxcur_pic_c(L"picture/Dif3S.png"),
		dxcur_pic_c(L"picture/Dif3B.png"),
		dxcur_pic_c(L"picture/Dif4S.png"),
		dxcur_pic_c(L"picture/Dif4B.png")
	};

private:
#if 0
	void DrawDifMark(int baseX, int baseY, MUSIC_BOX *songdata, int comdif) const {
		int posX = 0;
		int posY = 0;

		for (int i = 0; i < 5; i++) {
			posY = 0;
			if (comdif == i) { posY = 1; }
			if (comdif <= i) { posX = 1; }
			if (strands_direct(songdata->SongFileName[i], L"NULL") == 0 && i <= songdata->limit) {
				DrawGraphAnchor(baseX + 11 * posX + 16 * i, baseY, this->difC[i * 2 + posY].handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
			}
		}
	}
#endif

	void DrawDifBar(int baseX, int baseY, int dif) const {
		int XmoveC = maxs_2(-1 * (GetNowCount() - this->XstartC) + MUSE_FADTM, 0);

		if (this->LR == REC_SERECT_VECT_LEFT) {
			XmoveC = pals(0, 640, MUSE_FADTM, 460, XmoveC);
			DrawGraphAnchor(baseX, baseY, this->difbar[dif].handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
			DrawGraphAnchor(baseX + XmoveC - 461, baseY, this->difbar[dif + 1].handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
		}
		else if (this->LR == REC_SERECT_VECT_RIGHT) {
			XmoveC = pals(0, 460, MUSE_FADTM, 640, XmoveC);
			DrawGraphAnchor(baseX, baseY, this->difbar[dif - 1].handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
			DrawGraphAnchor(baseX + XmoveC - 461, baseY, this->difbar[dif].handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
		}
	}

	/* TODO: ddifが表示されない */
	void DrawDifMpal(int baseX, int baseY, rec_ddif_pal_t *mpal, int dif) const {
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
			DrawBoxAnchor(baseX - length - 70, baseY - thick - 2 * thick * iPal - thick / 2, baseX - 70, baseY - 2 * thick * iPal - thick / 2, color[7 - iPal], DXDRAW_ANCHOR_BOTTOM_RIGHT, TRUE);
			DrawBoxAnchor(baseX - length - 70, baseY - thick - 2 * thick * iPal - thick / 2, baseX - 70, baseY - 2 * thick * iPal - thick / 2, SubColor[7 - iPal], DXDRAW_ANCHOR_BOTTOM_RIGHT, FALSE);
		}
		DrawGraphAnchor(baseX, baseY, this->mpalNamePic.handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
#if REC_DEBUG == 1
		for (uint iPal = 1; iPal < 11; iPal++) {
			DrawLineAnchor(baseX - lins(0, 0, 900, 450, iPal * 100) - 70, baseY - thick - 2 * thick * 0 - thick / 2, baseX - lins(0, 0, 900, 450, iPal * 100) - 70, baseY - 2 * thick * 7 - thick / 2, COLOR_RED, DXDRAW_ANCHOR_BOTTOM_RIGHT);
		}
#endif
	}

	void DrawDetail(int baseX, int baseY, MUSIC_BOX_2 *songdata, int dif) const {
		const TCHAR starChar[2][2] = { _T("★"),_T("☆")};
		DrawGraphAnchor(baseX, baseY, this->detail.handle(), DXDRAW_ANCHOR_BOTTOM_RIGHT);
		DrawFormatStringToHandleAnchor(baseX - 330, baseY - 170, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_BOTTOM_RIGHT, L"%s", songdata->packName);
		DrawFormatStringToHandleAnchor(baseX - 325, baseY - 145, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_BOTTOM_RIGHT, L"Lv.%2d", songdata->level);
		for (int i = 0; i < 15; i++) {
			int temp = 0;
			if (10 <= i && songdata->level <= i) { break; }
			temp = (i < songdata->level) ? 0 : 1;
			DrawStringToHandleAnchor(baseX + 16 * i - 270, baseY - 145, starChar[temp], COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_BOTTOM_RIGHT);
		}
		DrawFormatStringToHandleAnchor(baseX - 320, baseY - 120, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_BOTTOM_RIGHT, L"HighSCORE:%6d/%6.2f%%/%5.3fkm",
			songdata->Hscore, songdata->Hacc, songdata->Hdis / 1000.0);
	}

public:
	void SlideDif(int vect) {
		this->LR = vect;
		this->XstartC = GetNowCount();
	}

	void FetchDifPic(TCHAR *difpath) {
		if (strands(this->viewingDifBar, difpath) == 1) { return; }

		DeleteGraph(this->difbar[4].handle());
		DeleteGraph(this->difbar[5].handle());

		this->difbar[4].reload(difpath);
		if (this->difbar[4].handle() == -1) {
			DeleteGraph(this->difbar[4].handle());
			this->difbar[4].reload(L"picture/difanother.png");
			this->difbar[5] = this->difbar[4];
			strcopy_2(L"NULL", this->viewingDifBar, 255);
		}
		else {
			this->difbar[5] = this->difbar[4];
			strcopy_2(difpath, this->viewingDifBar, 255);
		}
	}

	void DrawDetailAll(int baseX, int baseY, MUSIC_BOX_2 *songdata, int dif) const {
		// this->DrawDifMark(baseX - 100, baseY - 365, songdata, dif);
		this->DrawDifBar(baseX - 15, baseY - 320, dif);
		this->DrawDifMpal(baseX - 20, baseY - 186, &songdata->mpal, dif);
		this->DrawDetail(baseX, baseY, songdata, dif);
	}
};

static class rec_serect_jacket_c {
private:
	dxcur_pic_c jacketpic = dxcur_pic_c(_T("picture/NULL jucket.png"));
	TCHAR viewingjacket[255] = { L"picture/NULL jucket.png" };

public:
	void UpdateJacket(TCHAR *jacketName) {
		if (strands(this->viewingjacket, jacketName) == 0) {
			DeleteGraph(this->jacketpic.handle());
			strcopy_2(jacketName, this->viewingjacket, 255);
			this->jacketpic.reload(this->viewingjacket);
		}
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

	void InitUi(MUSIC_BOX_2 *songdata, int dif) {
		this->jacket.UpdateJacket(songdata->jacketP);
		this->detail.FetchDifPic(songdata->difP);
		this->previewSnd.UpdateSnd(songdata, dif);
		this->previewSnd.StartSnd();
	}

	void Update4th(TCHAR *jacketName) {
		jacket.UpdateJacket(jacketName);
		previewSnd.SetPresc(GetNowCount());
		snd.PlaySnd();
	}

	void UpdateUD(MUSIC_BOX_2 *songdata, int dif, int vect) {
		this->detail.FetchDifPic(songdata->difP);
		this->musicbar.SlideBar(vect);
		this->disk.SlideDisk(vect);
		this->Update4th(songdata->jacketP);
	}

	void UpdateLR(songdata_set_t &songdata, int *cmd, int dif, int vect) {
		this->detail.SlideDif(vect);
		SortSongWithSave(&songdata, dif, cmd);
		this->Update4th(songdata[*cmd].jacketP);
	}

	void DrawUi(int cmd[], songdata_set_t *songdata) {
		this->backpic.DrawBackPic();
		this->jacket.DrawJacket(380, 85, 500);
		this->musicbar.DrawAll(300, cmd, *songdata, false);
		this->disk.DrawDiskSet(-30, 25, songdata->sortMode);
		this->detail.DrawDetailAll(20, 60, &(*songdata)[cmd[0]], cmd[1]);
		this->previewSnd.CheckTime();
		this->previewSnd.CheckSnd(&(*songdata)[cmd[0]], cmd[1]);
		this->help.DrawHelp(HELP_MAT_MUSIC_SELECT);
		this->cutin.DrawCut();
	}
};

#endif /* ui class */

#if 1 /* after class action */

static void RecSerectDrawAllUi(rec_serect_ui_c *uiClass, int *cmd,
	songdata_set_t *songdata, char closeFg, int CutTime)
{
	ClearDrawScreen();
	uiClass->DrawUi(cmd, songdata);
	ScreenFlip();
	return;
}

static void RecSerectKeyActLR(int cmd[], int vect,
	rec_serect_ui_c *uiClass, songdata_set_t songdata[])
{
	switch (vect) {
	case REC_SERECT_VECT_LEFT:
		cmd[1]--;
		if (cmd[1] < 0) {
			cmd[1] = 0;
			return;
		}
		break;
	case REC_SERECT_VECT_RIGHT:
		cmd[1]++;
		if (cmd[1] > 4) {
			cmd[1] = 4;
			return;
		}
		break;
	default:
		return;
	}

	uiClass->UpdateLR(*songdata, &cmd[0], cmd[1], vect);

	return;
}

static void RecSerectKeyActUD(int cmd[], int vect,
	rec_serect_ui_c *uiClass, songdata_set_t *songdata)
{
	switch (vect) {
	case REC_SERECT_VECT_UP:
		cmd[0]--;
		if (cmd[0] < 0) { cmd[0] = songdata->sort.size() - 1; }
		break;
	case REC_SERECT_VECT_DOWN:
		cmd[0]++;
		if (cmd[0] >= songdata->sort.size()) { cmd[0] = 0; }
		break;
	default:
		return;
	}

	uiClass->UpdateUD(&(*songdata)[cmd[0]], cmd[1], vect);

#if 0
	{
		int diffixBuf = RecSerectFetchDif(&(*songdata)[cmd[0]], cmd[1], songdata->sortMode);
		if (diffixBuf < cmd[1]) {
			cmd[1] = diffixBuf;
			uiClass->UpdateLR(*songdata, &cmd[0], cmd[1], REC_SERECT_VECT_LEFT);
		}
		else if (diffixBuf > cmd[1]) {
			cmd[1] = diffixBuf;
			uiClass->UpdateLR(*songdata, &cmd[0], cmd[1], REC_SERECT_VECT_RIGHT);
		}
	}
#endif

	return;
}

static void RecSerectKeyActAll(now_scene_t *next, rec_to_play_set_t *toPlay, char *closeFg,
	int cmd[], int *CutTime, rec_serect_ui_c *uiClass, songdata_set_t *songdata,
	const int *PackFirstNum)
{
	int key = 0;

	/* 操作検出*/
	if (uiClass->cutin.IsClosing() == 0) { key = RecSerectKeyCheck(); }
	else { key = 0; }

	/* 動作 */
	switch (key) {
	case REC_SERECT_KEY_RETURN:
		// 選択できる曲であるかどうか (Lvが0以上であるかで判定)
		if ((*songdata)[cmd[0]].level < 0) { break; }
		RecSerectSetToPlay(toPlay, cmd, PackFirstNum, songdata);
		*next = SCENE_MUSIC;
		uiClass->cutin.SetCutTipFg(CUTIN_TIPS_SONG);
		uiClass->cutin.SetCutSong((*songdata)[cmd[0]].SongName,
			(*songdata)[cmd[0]].jacketP);
		uiClass->cutin.SetIo(CUT_FRAG_IN);
		break;
	case REC_SERECT_KEY_BACK:
		*next = SCENE_MENU;
		uiClass->cutin.SetTipNo();
		uiClass->cutin.SetCutTipFg(CUTIN_TIPS_ON);
		uiClass->cutin.SetIo(CUT_FRAG_IN);
		break;
	case REC_SERECT_KEY_UP:
		RecSerectKeyActUD(cmd, REC_SERECT_VECT_UP,    uiClass, songdata);
		break;
	case REC_SERECT_KEY_DOWN:
		RecSerectKeyActUD(cmd, REC_SERECT_VECT_DOWN,  uiClass, songdata);
		break;
	case REC_SERECT_KEY_LEFT:
		RecSerectKeyActLR(cmd, REC_SERECT_VECT_LEFT,  uiClass, songdata);
		break;
	case REC_SERECT_KEY_RIGHT:
		RecSerectKeyActLR(cmd, REC_SERECT_VECT_RIGHT, uiClass, songdata);
		break;
	case REC_SERECT_KEY_SORT:
		ChangeSortMode(&songdata->sortMode);
		SortSongWithSave(songdata, cmd[1], &cmd[0]);
		break;
	case REC_SERECT_KEY_RELORD:
		*next = SCENE_RELOAD;
		break;
	default:
		break;
	}

	return;
}

#endif /* after class action */

static now_scene_t musicserect2(rec_to_play_set_t *toPlay) {
	/* char */
	char closeFg = 0;

	/* int */
	int cmd[2] = { 0,1 };
	int buf = 0;
	int PackFirstNum[PackNumLim];
	int CutTime = 0;

	/* typedef */
	now_scene_t next = SCENE_EXIT;
	songdata_set_t songdata;

	/* class */
	rec_serect_ui_c uiClass;

	RecSerectLoadBefCmd(cmd, &songdata.sortMode);
	RecSerectReadMapData(&songdata, PackFirstNum);
	SortSong(&songdata, cmd[1]);
	uiClass.InitUi(&songdata[cmd[0]], cmd[1]);
	AvoidKeyBug();
	GetMouseWheelRotVol();
	while (GetMouseInputLog2(NULL, NULL, NULL, NULL, true) == 0) {}
	uiClass.cutin.SetIo(CUT_FRAG_OUT);

	while (1) {
		RecSerectDrawAllUi(&uiClass, cmd, &songdata, closeFg, CutTime);
		RecSerectKeyActAll(&next, toPlay, &closeFg, cmd, &CutTime,
			&uiClass, &songdata, PackFirstNum);
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
		next = musicserect2(toPlay);
		/* RecSelectAllRelordを実行するためにはmusicserect2から抜ける必要がある。スタックオーバーフローを起こすから */
		if (next == SCENE_RELOAD) { RecSelectAllRelord(); }
		else { break; }
	}
	return next;
}
