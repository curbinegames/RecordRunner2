
#include <DxLib.h>
#include <helpBar.h>
#include <recr_cutin.h>
#include <dxcur.h>
#include <sancur.h>
#include <strcur.h>
#include <dxdraw.h>
#include <system.h>
#include <option.h>
#include <musicserect2.h>
#include <RecWindowRescale.h>
#include <playbox.h>
#include <RecScoreFile.h>

#define PackNumLim 8
#define SongNumLim 64
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

#define SONGDATA_FROM_MAP(songdata, mapNo) ((songdata)->base[(songdata)->mapping[mapNo]])

typedef TCHAR rec_pack_name_set_t[256];

typedef struct music_box {
	int limit;
	int level[6];
	int preview[6][2];
	int Hscore[6];
	int Hdis[6];
	int ClearRank[6]; //0=EX, 1=S, 2=A, 3=B, 4=C, 5=D, 6=not play
	int ClearRate[6]; //0=not play, 1=droped, 2=cleared, 3=no miss!, 4=full combo!!, 5=perfect!!!
	double Hacc[6];
	wchar_t difP[256];
	wchar_t packName[256];
	wchar_t SongName[6][256];
	wchar_t artist[6][256];
	wchar_t SongFileName[6][256];
	wchar_t jacketP[6][256];
	rec_ddif_pal_t mpal[6];
} MUSIC_BOX;

typedef struct rec_serect_music_set_s {
	MUSIC_BOX base[SongNumLim];
	int mapping[SongNumLim];
	int sortMode = REC_SORT_DEFAULT;
	int musicNum = 0;
} rec_serect_music_set_t;
typedef rec_serect_music_set_t songdata_set_t;

#if 1 /* read map data */

static void RecSerectReadMapDataOneDif(TCHAR *path, TCHAR *subpath, MUSIC_BOX *songdata, int dif) {
	DxFile_t fd;
	TCHAR buf[256];
	int lang = optiondata.lang;

	fd = FileRead_open(path);

	if (fd == 0) { return; }

	while (FileRead_eof(fd) == 0) {
		FileRead_gets(buf, 256, fd);
		//曲名を読み込む
		if (strands(buf, L"#TITLE:") &&
			(lang == 0 || strands(songdata->SongName[dif], L"NULL"))) {
			strmods(buf, 7);
			strcopy(buf, songdata->SongName[dif], 1);
		}
		if (strands(buf, L"#E.TITLE:") &&
			(lang == 1 || strands(songdata->SongName[dif], L"NULL"))) {
			strmods(buf, 9);
			strcopy(buf, songdata->SongName[dif], 1);
		}
		//作曲者を読み込む
		if (strands(buf, L"#ARTIST:") &&
			(lang == 0 || strands(songdata->artist[dif], L"NULL"))) {
			strmods(buf, 8);
			strcopy(buf, songdata->artist[dif], 1);
		}
		if (strands(buf, L"#E.ARTIST:") &&
			(lang == 1 || strands(songdata->artist[dif], L"NULL"))) {
			strmods(buf, 10);
			strcopy(buf, songdata->artist[dif], 1);
		}
		//曲ファイル名を読み込む
		if (strands(buf, L"#MUSIC:")) {
			strmods(buf, 7);
			strcopy(subpath, songdata->SongFileName[dif], 1);
			strcats(songdata->SongFileName[dif], buf);
		}
		//難易度を読み込む
		if (strands(buf, L"#LEVEL:")) {
			strmods(buf, 7);
			songdata->level[dif] = strsans(buf);
		}
		//プレビュー時間を読み込む
		if (strands(buf, L"#PREVIEW:")) {
			strmods(buf, 9);
			songdata->preview[dif][0] = (int)((double)strsans(buf) / 1000.0 * 44100.0);
			strnex(buf);
			if (L'0' <= buf[1] && buf[1] <= L'9') {
				songdata->preview[dif][1] = (int)((double)strsans(buf) / 1000.0 * 44100.0);
			}
		}
		//ジャケット写真を読み込む
		if (strands(buf, L"#JACKET:")) {
			strmods(buf, 8);
			strcopy(subpath, songdata->jacketP[dif], 1);
			strcats(songdata->jacketP[dif], buf);
		}
		//差し替えAnotherバーを読み込む
		if (strands(buf, L"#DIFBAR:")) {
			strmods(buf, 8);
			strcopy(subpath, songdata->difP, 1);
			strcats(songdata->difP, buf);
		}
		//マップに入ったら抜ける
		if (strands(buf, L"#MAP:")) { break; }
	}
	FileRead_close(fd);

	return;
}

static void RecSerectReadHighscore(MUSIC_BOX *songdata, TCHAR *songName) {
	TCHAR path[256];
	FILE *fp;

	for (int j = 0; j < 6; j++) {
		songdata->Hscore[j] = 0;
		songdata->Hacc[j] = 0;
		songdata->Hdis[j] = 0;
		songdata->ClearRank[j] = 6;
		songdata->ClearRate[j] = 0;
	}

	strcopy(L"score/", path, 1);
	strcats(path, songName);
	strcats(path, L".dat");

	_wfopen_s(&fp, path, L"rb");
	if (fp != NULL) {
		fread(&songdata->Hscore, sizeof(int), 6, fp);
		fread(&songdata->Hacc, sizeof(double), 6, fp);
		fread(&songdata->Hdis, sizeof(int), 6, fp);
		fread(&songdata->ClearRank, sizeof(int), 6, fp);
		fread(&songdata->ClearRate, sizeof(int), 6, fp);
		fclose(fp);
	}

	return;
}

static void RecSerectReadMapDataOneSong(MUSIC_BOX *songdata, TCHAR *packName, TCHAR *songName) {
	TCHAR path[256];
	TCHAR subPath[256];
	TCHAR rrsPath[256];

	songdata->limit = 3;
	strcopy(L"NULL", songdata->difP, 1);

	for (int iDif = 0; iDif < 6; iDif++) {
		strcopy(L"record/", path, 1); //"record/"
		strcats(path, packName); //"record/<パック名>"
		stradds(path, L'/'); //"record/<パック名>/"
		strcats(path, songName); //"record/<パック名>/<曲名>"
		stradds(path, L'/'); //"record/<パック名>/<曲名>/"
		strcopy(path, subPath, 1); //subPathにコピー
		stradds(path, (TCHAR)((int)_T('0') + iDif)); //"record/<パック名>/<曲名>/<難易度番号>"
		strcopy(path, rrsPath, 1); //rrsPathにコピー
		strcats(path, _T(".txt")); //"record/<パック名>/<曲名>/<難易度番号>.txt"
		strcats(rrsPath, _T(".rrs")); //"record/<パック名>/<曲名>/<難易度番号>.txt"
		//初期値定義(ファイルがなくても代入する)
		strcopy(L"NULL", songdata->SongName[iDif], 1);
		strcopy(L"NULL", songdata->artist[iDif], 1);
		strcopy(L"NULL", songdata->SongFileName[iDif], 1);
		strcopy(L"picture/NULL jucket.png", songdata->jacketP[iDif], 1);
		strcopy(packName, songdata->packName, 1);
		songdata->level[iDif] = -1;
		songdata->preview[iDif][0] = 441000;
		songdata->preview[iDif][1] = 2646000;
		RecSerectReadMapDataOneDif(path, subPath, songdata, iDif);
		RecScoreReadDdif(&songdata->mpal[iDif], rrsPath);
	}

	RecSerectReadHighscore(songdata, songName);

	//難易度リミットの処理
	if (strands(songdata->SongName[4], L"NULL") == 0 &&
		(strands(songdata->SongName[5], L"NULL") ||
			songdata->Hscore[5] >= 1))
	{
		songdata->limit = 4;
	}
	if (songdata->limit == 3) {
		strcopy(L"NULL", songdata->SongName[4], 1);
		strcopy(L"NULL", songdata->artist[4], 1);
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
	int songCount = 0;
	DxFile_t fd;
	rec_pack_name_set_t PackName[PackNumLim];
	TCHAR songName[256];

	fd = FileRead_open(L"RecordPack.txt");
	while (FileRead_eof(fd) == 0) {
		FileRead_gets(songName, 256, fd);
		strcopy(songName, PackName[packNum], 1);
		packNum++;
	}
	FileRead_close(fd);

	songCount = 0;
	for (int i = 0; i < packNum; i++) {
		strcopy(L"record/", songName, 1); //"record/"
		strcats(songName, PackName[i]); //"record/<パック名>"
		strcats(songName, L"/list.txt"); //"record/<パック名>/list.txt"
		fd = FileRead_open(songName);
		PackFirstNum[i] = songCount;
		while (FileRead_eof(fd) == 0) {
			FileRead_gets(songName, 256, fd);
			RecSerectReadMapDataOneSong(&songdata->base[songCount], PackName[i], songName);
			//後処理
			songdata->mapping[songCount] = songCount;
			songCount++;
		}
		FileRead_close(fd);
	}

	songdata->musicNum = songCount;
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

static int GetRate() {
	int e;
	int ans = 0;
	play_rate_t prate[RATE_NUM];
	FILE *fp;
	e = _wfopen_s(&fp, RATE_FILE_NAME, L"rb");
	if (fp == NULL) {
		return 0;
	}
	fread(&prate, sizeof(play_rate_t), RATE_NUM, fp);
	fclose(fp);
	for (int i = 0; i < RATE_NUM; i++) {
		if (0 < prate[i].num) {
			ans += prate[i].num * 100;
		}
	}
	return ans / 2;
}

static int RecSerectKeyCheck() {
	int ret = 0;
	int mouseBtn = 0;
	int mouseX = 0;
	int mouseY = 0;
	int mouseAct = 0;
	int mouseHoil = 0;

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

static int RecSerectTrySecret2(int AutoFlag, int dif, MUSIC_BOX *songdata) {
	if (AutoFlag == 0 && dif == 3 &&
		songdata->Hscore[3] >= 90000 &&
		strands(songdata->SongFileName[5], L"NULL") == 0 &&
		songdata->Hscore[5] <= 0)
	{
		if (RecSerectTrySecret(songdata->Hscore[3]) == 1) {
			return 1;
		}
	}
	return 0;
}

/**
 * 曲リストを並び替えします
 * @param[in] songdata 曲データ
 * @param[in] mode 並び替えの方法
 * @param[in] dif 難易度
 */
static void SortSong(songdata_set_t *songdata, int mode, int dif) {
	int n = 0;
	int m = songdata->musicNum;
	int o = 0;
	int p = 1;
	switch (mode) {
	case REC_SORT_DEFAULT:
		for (int i = 0; i < songdata->musicNum; i++) {
			songdata->mapping[i] = i;
		}
		break;
	case SORT_LEVEL:
		while (p) {
			p = 0;
			for (int i = 0; i < songdata->musicNum - 1; i += 2) {
				if (SONGDATA_FROM_MAP(songdata, i).level[dif] > SONGDATA_FROM_MAP(songdata, i + 1).level[dif]) {
					o = songdata->mapping[i];
					songdata->mapping[i] = songdata->mapping[i + 1];
					songdata->mapping[i + 1] = o;
					p = 1;
				}
			}
			for (int i = 1; i < songdata->musicNum - 1; i += 2) {
				if (SONGDATA_FROM_MAP(songdata, i).level[dif] > SONGDATA_FROM_MAP(songdata, i + 1).level[dif]) {
					o = songdata->mapping[i];
					songdata->mapping[i] = songdata->mapping[i + 1];
					songdata->mapping[i + 1] = o;
					p = 1;
				}
			}
		}
		break;
	case SORT_SCORE:
		while (p) {
			p = 0;
			for (int i = 0; i < songdata->musicNum - 1; i += 2) {
				if (SONGDATA_FROM_MAP(songdata, i).Hscore[dif] < SONGDATA_FROM_MAP(songdata, i + 1).Hscore[dif]) {
					o = songdata->mapping[i];
					songdata->mapping[i] = songdata->mapping[i + 1];
					songdata->mapping[i + 1] = o;
					p = 1;
				}
			}
			for (int i = 1; i < songdata->musicNum - 1; i += 2) {
				if (SONGDATA_FROM_MAP(songdata, i).Hscore[dif] < SONGDATA_FROM_MAP(songdata, i + 1).Hscore[dif]) {
					o = songdata->mapping[i];
					songdata->mapping[i] = songdata->mapping[i + 1];
					songdata->mapping[i + 1] = o;
					p = 1;
				}
			}
		}
		break;
	}
	songdata->sortMode = mode;
	return;
}

/**
 * SortSongで並び替えをした後、cmdを前と同じ曲に合わせます
 * @param[in] songdata 曲データ
 * @param[in] mode 並び替えの方法
 * @param[in] dif 難易度
 * @param[out] cmd commandの格納場所
 */
static void SortSongWithSave(songdata_set_t *songdata, int mode, int dif, int *cmd) {
	int save = 0;

	save = songdata->mapping[*cmd];
	SortSong(songdata, mode, dif);
	for (int i = 0; i < songdata->musicNum; i++) {
		if (songdata->mapping[i] == save) {
			*cmd = i;
		}
	}
	return;
}

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
	if (strands(songdata->SongName[dif], L"NULL") == 0) { return ret; }

	if (strands(songdata->SongName[0], L"NULL") != 1) { ret = 0; }
	switch (dif) {
	case 1:
		if (strands(songdata->SongName[2], L"NULL") != 1) { ret = 2; }
		else if (strands(songdata->SongName[3], L"NULL") != 1) { ret = 3; }
		break;
	case 2:
		if (strands(songdata->SongName[1], L"NULL") != 1) { ret = 1; }
		else if (strands(songdata->SongName[3], L"NULL") != 1) { ret = 3; }
		break;
	case 3:
		if (strands(songdata->SongName[2], L"NULL") != 1) { ret = 2; }
		else if (strands(songdata->SongName[1], L"NULL") != 1) { ret = 1; }
		break;
	default:
		if (strands(songdata->SongName[3], L"NULL") != 1) { ret = 3; }
		else if (strands(songdata->SongName[2], L"NULL") != 1) { ret = 2; }
		else if (strands(songdata->SongName[1], L"NULL") != 1) { ret = 1; }
		break;
	}

	return ret;
}

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
	for (inum = PackNumLim; inum >= 0; inum--) {
		if (PackFirstNum[inum] >= 0 && PackFirstNum[inum] <= songdata->mapping[cmd[0]]) {
			toPlay->packNo = inum;
			break;
		}
	}
	toPlay->musicNo = songdata->mapping[cmd[0]] - PackFirstNum[inum];
	toPlay->dif = cmd[1];

	//隠し曲用
	if (RecSerectTrySecret2(toPlay->autoFg, cmd[1], &SONGDATA_FROM_MAP(songdata, cmd[0])) == 1) {
		toPlay->dif = 5;
	}
	return;
}

#endif /* sub action */

#if 1 /* ui class */

static class rec_serect_backpic_c {
private:
	DxPic_t back;
	int backpos = 0;

public:
	rec_serect_backpic_c() {
		this->back = LoadGraph(L"picture/MSback.png");
	}

	~rec_serect_backpic_c() {
		DeleteGraph(this->back);
	}

	void DrawBackPic() {
		this->backpos = (this->backpos - 2) % 640;
		RecRescaleDrawGraph(this->backpos, 0, this->back, TRUE);
		RecRescaleDrawGraph(this->backpos + 640, 0, this->back, TRUE);
	}
};

static class rec_serect_preview_sound_c {
private:
	int preSC = 0;
	int SongPrePat = 0;
	int SongPreSTime = 0;
	int preTime[2] = { 441000,2646000 };
	TCHAR playingsong[256] = { L"NULL" };
	DxSnd_t previewM = LoadSoundMem(L"null.mp3");

public:
	void StartSnd() {
		SetCurrentPositionSoundMem(this->preTime[this->SongPrePat], this->previewM);
		ChangeVolumeSoundMem(0, this->previewM);
		PlaySoundMem(this->previewM, DX_PLAYTYPE_BACK, FALSE);
		WaitTimer(30);
		this->SongPreSTime = GetNowCount();
	}

	int UpdateSnd(MUSIC_BOX *songdata, int dif) {
		if (strands(songdata->SongFileName[dif], L"NULL") == 0 &&
			strands(this->playingsong, songdata->SongFileName[dif]) == 0)
		{
			if (this->previewM != -1) {
				StopSoundMem(this->previewM);
				DeleteSoundMem(this->previewM);
			}
			strcopy(songdata->SongFileName[dif], this->playingsong, 1);
			this->previewM = LoadSoundMem(this->playingsong);
			this->SongPrePat = 0;
			this->preTime[0] = songdata->preview[dif][0];
			this->preTime[1] = songdata->preview[dif][1];
			return 1;
		}
		return 0;
	}

	void CheckTime(MUSIC_BOX *songdata, int dif) {
		int Ntime = GetNowCount();
		if (Ntime - this->SongPreSTime < 500) {
			ChangeVolumeSoundMem(lins(0, 0, 500, 255, Ntime - this->SongPreSTime), this->previewM);
		}
		else if (500 <= Ntime - this->SongPreSTime && Ntime - this->SongPreSTime < 14500) {
			ChangeVolumeSoundMem(255, this->previewM);
		}
		else if (14500 <= Ntime - this->SongPreSTime && Ntime - this->SongPreSTime < 15000) {
			ChangeVolumeSoundMem(lins(14500, 255, 15000, 0, Ntime - this->SongPreSTime), this->previewM);
		}
		else if (15000 <= Ntime - this->SongPreSTime) {
			StopSoundMem(this->previewM);
			this->SongPrePat = (this->SongPrePat + 1) % 2;
			this->StartSnd();
		}
	}

	void CheckSnd(MUSIC_BOX *songdata, int dif) {
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

	int UD = 1;
	int startC = -MUSE_FADTM;
	DxPic_t bar[2];
	DxPic_t CRate[5];
	DxPic_t rankP[6];

	void DrawClear(int x, int y, int clearNo) {
		if (0 <= clearNo && clearNo <= 4) {
			DrawGraph(x, y, this->CRate[clearNo], TRUE);
		}
	}

	void DrawRack(int x, int y, int rankNo) {
		if (0 <= rankNo && rankNo <= 5) {
			DrawGraph(x, y, this->rankP[rankNo], TRUE);
		}
	}

	void DrawMainOne(int dif, int BasePosX, int BasePosY, MUSIC_BOX *songdata) {
		DrawGraph(BasePosX - 120, BasePosY - 170, this->bar[1], TRUE);
		DrawStringToHandle(BasePosX - 30, BasePosY - 157, songdata->SongName[dif], COLOR_BLACK, SmallFontData);
		DrawStringToHandle(BasePosX - 30, BasePosY - 129, songdata->artist[dif], COLOR_BLACK, SmallFontData);
		this->DrawClear(BasePosX + 156, BasePosY - 132, songdata->ClearRate[dif] - 1);
		this->DrawRack(BasePosX + 156, BasePosY - 132, songdata->ClearRank[dif]);
		for (int idif = 0; idif < 3; idif++) {
			DrawFormatStringToHandle(BasePosX - 25 + idif * 70, BasePosY - 97,
				COLOR_BLACK, SmallFontData, L"%2d", songdata->level[1 + idif]);
		}
	}

	void DrawSubOne(int dif, int BasePosX, int BasePosY, MUSIC_BOX *songdata) {
		DrawGraph(BasePosX - 120, BasePosY - 170, this->bar[0], TRUE);
		DrawStringToHandle(BasePosX - 30, BasePosY - 157, songdata->SongName[dif], COLOR_WHITE, SmallFontData);
		DrawStringToHandle(BasePosX - 30, BasePosY - 129, songdata->artist[dif], COLOR_WHITE, SmallFontData);
		this->DrawClear(BasePosX + 152, BasePosY - 163, songdata->ClearRate[dif] - 1);
		this->DrawRack(BasePosX + 152, BasePosY - 163, songdata->ClearRank[dif]);
	}

public:
	rec_serect_musicbar_c() {
		this->bar[0] = LoadGraph(L"picture/songbarB.png");
		this->bar[1] = LoadGraph(L"picture/songbarY.png");
		this->CRate[0] = LoadGraph(L"picture/MarkD.png");
		this->CRate[1] = LoadGraph(L"picture/MarkC.png");
		this->CRate[2] = LoadGraph(L"picture/MarkNM.png");
		this->CRate[3] = LoadGraph(L"picture/MarkFC.png");
		this->CRate[4] = LoadGraph(L"picture/MarkP.png");
		this->rankP[0] = LoadGraph(L"picture/MiniEX.png");
		this->rankP[1] = LoadGraph(L"picture/MiniS.png");
		this->rankP[2] = LoadGraph(L"picture/MiniA.png");
		this->rankP[3] = LoadGraph(L"picture/MiniB.png");
		this->rankP[4] = LoadGraph(L"picture/MiniC.png");
		this->rankP[5] = LoadGraph(L"picture/MiniD.png");
	}

	~rec_serect_musicbar_c() {
		DeleteGraph(this->bar[0]);
		DeleteGraph(this->bar[1]);
		DeleteGraph(this->CRate[0]);
		DeleteGraph(this->CRate[1]);
		DeleteGraph(this->CRate[2]);
		DeleteGraph(this->CRate[3]);
		DeleteGraph(this->CRate[4]);
		DeleteGraph(this->rankP[0]);
		DeleteGraph(this->rankP[1]);
		DeleteGraph(this->rankP[2]);
		DeleteGraph(this->rankP[3]);
		DeleteGraph(this->rankP[4]);
		DeleteGraph(this->rankP[5]);
	}

	void SlideBar(int vect) {
		this->UD = vect;
		this->startC = GetNowCount();
	}

	void DrawAll(int Ypos, int cmd[], songdata_set_t *songdata) {
		int BasePosX = 0;
		int BasePosY = 0;
		int slide = 0;
		int picsong = 0;
		int moveC = 0;

		moveC = mins(-1 * (GetNowCount() - this->startC) + MUSE_FADTM, 0);
		picsong = (cmd[0] + songdata->musicNum - (VIEW_COUNT / 2)) % songdata->musicNum;

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
				this->DrawMainOne(cmd[1], BasePosX, BasePosY, &SONGDATA_FROM_MAP(songdata, picsong));
			}
			else {
				this->DrawSubOne(cmd[1], BasePosX, BasePosY, &SONGDATA_FROM_MAP(songdata, picsong));
			}

			picsong = (picsong + 1) % songdata->musicNum;
		}
	}

#undef VIEW_COUNT
};

static class rec_serect_disk_c {
private:
	int Lv = 1;
	int rate = 0;
	int UD = 1;
	int startC = -MUSE_FADTM;
	double Nrot = 0.0;
	DxPic_t disk;
	DxPic_t runner;
	DxPic_t rateBar;

	void DrawNamePlate(int baseX, int baseY) {
		DrawGraphAnchor(baseX - 30, baseY, this->rateBar, DXDRAW_ANCHOR_TOP_RIGHT);
		DrawGraphAnchor(baseX, baseY, this->runner, DXDRAW_ANCHOR_TOP_RIGHT);
		DrawFormatStringToHandleAnchor(baseX - 220, baseY + 12, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT, L"Lv:%2d", this->Lv);
		DrawFormatStringToHandleAnchor(baseX - 215, baseY + 37, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT, L"RATE:%d.%02d", this->rate / 100, this->rate % 100);
	}

	void DrawDisk(int baseX, int baseY) {
		int moveC = 0;

		moveC = mins(-1 * (GetNowCount() - this->startC) + MUSE_FADTM, 0);
		if (this->UD == 1) { this->Nrot += pals(0, 2, MUSE_FADTM, -75, moveC) / 100.0; }
		else { this->Nrot += pals(0, 2, MUSE_FADTM, 75, moveC) / 100.0; }
		if (this->Nrot > 6.28) { this->Nrot -= 6.28; }
		else if (this->Nrot < 0) { this->Nrot += 6.28; }
		DrawRotaGraphAnchor(baseX, baseY, 1, this->Nrot, this->disk, DXDRAW_ANCHOR_TOP_RIGHT, TRUE);
	}

	void DrawSort(int baseX, int baseY, int mode) {
		if (optiondata.lang == 1) {
			switch (mode) {
			case REC_SORT_DEFAULT:
				DrawStringToHandleAnchor(baseX, baseY, L"default", COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
				break;
			case SORT_LEVEL:
				DrawStringToHandleAnchor(baseX, baseY, L"level", COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
				break;
			case SORT_SCORE:
				DrawStringToHandleAnchor(baseX, baseY, L"score", COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
				break;
			}
		}
		else {
			switch (mode) {
			case REC_SORT_DEFAULT:
				DrawStringToHandleAnchor(baseX, baseY, L"デフォルト", COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
				break;
			case SORT_LEVEL:
				DrawStringToHandleAnchor(baseX, baseY, L"レベル順", COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
				break;
			case SORT_SCORE:
				DrawStringToHandleAnchor(baseX, baseY, L"スコア順", COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
				break;
			}
		}
	}

public:
	rec_serect_disk_c() {
		FILE *fp = NULL;

		_wfopen_s(&fp, L"save/chap.dat", L"rb");
		if (fp != NULL) {
			int buf[3];
			fread(buf, sizeof(int), 3, fp);
			this->Lv = buf[optiondata.chara] + 1;
			fclose(fp);
		}

		this->rate = GetRate();

		switch (optiondata.chara) {
		case 0:
			this->runner = LoadGraph(L"picture/Mpicker.png");
			break;
		case 1:
			this->runner = LoadGraph(L"picture/Mgator.png");
			break;
		case 2:
			this->runner = LoadGraph(L"picture/Mtaylor.png");
			break;
		default:
			this->runner = -1;
			break;
		}
		if (this->rate < 2500) {
			this->rateBar = LoadGraph(L"picture/MSrate1.png");
		}
		else if (2500 <= this->rate && this->rate < 5500) {
			this->rateBar = LoadGraph(L"picture/MSrate2.png");
		}
		else if (5500 <= this->rate && this->rate < 9000) {
			this->rateBar = LoadGraph(L"picture/MSrate3.png");
		}
		else if (9000 <= this->rate && this->rate < 12000) {
			this->rateBar = LoadGraph(L"picture/MSrate4.png");
		}
		else {
			this->rateBar = LoadGraph(L"picture/MSrate5.png");
		}
		this->disk = LoadGraph(L"picture/disk.png");
	}

	~rec_serect_disk_c() {
		DeleteGraph(this->disk);
		DeleteGraph(this->rateBar);
		DeleteGraph(this->runner);
	}

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
	int LR = 1;
	int XstartC = -MUSE_FADTM;
	TCHAR viewingDifBar[255] = { L"NULL" };
	DxPic_t difbar[6];
	DxPic_t detail;
	DxPic_t mpalNamePic;
	DxPic_t difC[10];

public:
	rec_serect_detail_c() {
		this->difbar[0] = LoadGraph(L"picture/difauto.png");
		this->difbar[1] = LoadGraph(L"picture/difeasy.png");
		this->difbar[2] = LoadGraph(L"picture/difnormal.png");
		this->difbar[3] = LoadGraph(L"picture/difhard.png");
		this->difbar[4] = LoadGraph(L"picture/difanother.png");
		this->difbar[5] = this->difbar[4];
		this->detail = LoadGraph(L"picture/detail.png");
		this->mpalNamePic = LoadGraph(_T("picture/mpalName.png"));
		this->difC[0] = LoadGraph(L"picture/Dif0S.png");
		this->difC[1] = LoadGraph(L"picture/Dif0B.png");
		this->difC[2] = LoadGraph(L"picture/Dif1S.png");
		this->difC[3] = LoadGraph(L"picture/Dif1B.png");
		this->difC[4] = LoadGraph(L"picture/Dif2S.png");
		this->difC[5] = LoadGraph(L"picture/Dif2B.png");
		this->difC[6] = LoadGraph(L"picture/Dif3S.png");
		this->difC[7] = LoadGraph(L"picture/Dif3B.png");
		this->difC[8] = LoadGraph(L"picture/Dif4S.png");
		this->difC[9] = LoadGraph(L"picture/Dif4B.png");
	}

	~rec_serect_detail_c() {
		DeleteGraph(this->detail);
		DeleteGraph(this->mpalNamePic);
		DeleteGraph(this->difbar[0]);
		DeleteGraph(this->difbar[1]);
		DeleteGraph(this->difbar[2]);
		DeleteGraph(this->difbar[3]);
		DeleteGraph(this->difbar[4]);
		for (int i = 0; i < 10; i++) {
			DeleteGraph(this->difC[i]);
		}
	}

private:
	void DrawDifMark(int baseX, int baseY, MUSIC_BOX *songdata, int comdif) const {
		int posX = 0;
		int posY = 0;

		for (int i = 0; i < 5; i++) {
			posY = 0;
			if (comdif == i) { posY = 1; }
			if (comdif <= i) { posX = 1; }
			if (strands(songdata->SongFileName[i], L"NULL") == 0 && i <= songdata->limit) {
				DrawGraphAnchor(baseX + 11 * posX + 16 * i, baseY, this->difC[i * 2 + posY], DXDRAW_ANCHOR_BOTTOM_RIGHT);
			}
		}
	}

	void DrawDifBar(int baseX, int baseY, int dif) const {
		int XmoveC = mins(-1 * (GetNowCount() - this->XstartC) + MUSE_FADTM, 0);

		if (this->LR == 1) {
			XmoveC = pals(0, 640, MUSE_FADTM, 460, XmoveC);
			DrawGraphAnchor(baseX, baseY, this->difbar[dif], DXDRAW_ANCHOR_BOTTOM_RIGHT);
			DrawGraphAnchor(baseX + XmoveC - 461, baseY, this->difbar[dif + 1], DXDRAW_ANCHOR_BOTTOM_RIGHT);
		}
		else if (this->LR == -1) {
			XmoveC = pals(0, 460, MUSE_FADTM, 640, XmoveC);
			DrawGraphAnchor(baseX, baseY, this->difbar[dif - 1], DXDRAW_ANCHOR_BOTTOM_RIGHT);
			DrawGraphAnchor(baseX + XmoveC - 461, baseY, this->difbar[dif], DXDRAW_ANCHOR_BOTTOM_RIGHT);
		}
	}

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
		uint *p_mpal = (uint *)&mpal[dif];
		for (uint iPal = 0; iPal < 8; iPal++) {
			uint length = lins(0, 0, 900, 450, p_mpal[7 - iPal]);
			DrawBoxAnchor(baseX - length - 70, baseY - thick - 2 * thick * iPal - thick / 2, baseX - 70, baseY - 2 * thick * iPal - thick / 2, color[7 - iPal], DXDRAW_ANCHOR_BOTTOM_RIGHT, TRUE);
			DrawBoxAnchor(baseX - length - 70, baseY - thick - 2 * thick * iPal - thick / 2, baseX - 70, baseY - 2 * thick * iPal - thick / 2, SubColor[7 - iPal], DXDRAW_ANCHOR_BOTTOM_RIGHT, FALSE);
		}
		DrawGraphAnchor(baseX, baseY, this->mpalNamePic, DXDRAW_ANCHOR_BOTTOM_RIGHT);
	}

	void DrawDetail(int baseX, int baseY, MUSIC_BOX *songdata, int dif) const {
		const TCHAR starChar[2][2] = { _T("★"),_T("☆")};
		DrawGraphAnchor(baseX, baseY, this->detail, DXDRAW_ANCHOR_BOTTOM_RIGHT);
		DrawFormatStringToHandleAnchor(baseX - 330, baseY - 170, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_BOTTOM_RIGHT, L"%s", songdata->packName);
		DrawFormatStringToHandleAnchor(baseX - 325, baseY - 145, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_BOTTOM_RIGHT, L"Lv.%2d", songdata->level[dif]);
		for (int i = 0; i < 15; i++) {
			int temp = 0;
			if (10 <= i && songdata->level[dif] <= i) { break; }
			temp = (i < songdata->level[dif]) ? 0 : 1;
			DrawStringToHandleAnchor(baseX + 16 * i - 270, baseY - 145, starChar[temp], COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_BOTTOM_RIGHT);
		}
		DrawFormatStringToHandleAnchor(baseX - 320, baseY - 120, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_BOTTOM_RIGHT, L"HighSCORE:%6d/%6.2f%%/%5.3fkm",
			songdata->Hscore[dif], songdata->Hacc[dif], songdata->Hdis[dif] / 1000.0);
	}

public:
	void SlideDif(int vect) {
		this->LR = vect;
		this->XstartC = GetNowCount();
	}

	void FetchDifPic(TCHAR *difpath) {
		if (strands(this->viewingDifBar, difpath) == 1) { return; }

		DeleteGraph(this->difbar[4]);
		DeleteGraph(this->difbar[5]);

		this->difbar[4] = LoadGraph(difpath);
		if (this->difbar[4] == -1) {
			DeleteGraph(this->difbar[4]);
			this->difbar[4] = LoadGraph(L"picture/difanother.png");
			this->difbar[5] = this->difbar[4];
			strcopy(L"NULL", this->viewingDifBar, 1);
		}
		else {
			this->difbar[5] = this->difbar[4];
			strcopy(difpath, this->viewingDifBar, 1);
		}
	}

	void DrawDetailAll(int baseX, int baseY, MUSIC_BOX *songdata, int dif) const {
		this->DrawDifMark(baseX - 100, baseY - 365, songdata, dif);
		this->DrawDifBar(baseX - 15, baseY - 320, dif);
		this->DrawDifMpal(baseX - 20, baseY - 186, songdata->mpal, dif);
		this->DrawDetail(baseX, baseY, songdata, dif);
	}
};

static class rec_serect_jacket_c {
private:
	DxPic_t jacketpic;
	TCHAR viewingjacket[255] = { L"picture/NULL jucket.png" };

public:
	rec_serect_jacket_c() {
		this->jacketpic = LoadGraph(L"picture/NULL jucket.png");
	}

	~rec_serect_jacket_c() {
		DeleteGraph(this->jacketpic);
	}

	void UpdateJacket(TCHAR *jacketName) {
		if (strands(this->viewingjacket, jacketName) == 0) {
			DeleteGraph(this->jacketpic);
			strcopy(jacketName, this->viewingjacket, 1);
			this->jacketpic = LoadGraph(this->viewingjacket);
		}
	}

	void DrawJacket(int baseX, int baseY, int size) {
		DrawExtendGraph(baseX, baseY, baseX + size, baseY + size, this->jacketpic, TRUE);
	}
};

static class rec_serect_snd_c {
private:
	DxSnd_t select;

public:
	rec_serect_snd_c() {
		this->select = LoadSoundMem(L"sound/arrow.wav");
	}

	~rec_serect_snd_c() {
		StopSoundMem(this->select);
		DeleteSoundMem(this->select);
	}

	void PlaySnd() {
		PlaySoundMem(this->select, DX_PLAYTYPE_BACK);
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

	void InitUi(MUSIC_BOX *songdata, int dif) {
		this->jacket.UpdateJacket(songdata->jacketP[dif]);
		this->detail.FetchDifPic(songdata->difP);
		this->previewSnd.UpdateSnd(songdata, dif);
		this->previewSnd.StartSnd();
	}

	void Update4th(TCHAR *jacketName) {
		jacket.UpdateJacket(jacketName);
		previewSnd.SetPresc(GetNowCount());
		snd.PlaySnd();
	}

	void UpdateUD(MUSIC_BOX *songdata, int dif, int vect) {
		this->detail.FetchDifPic(songdata->difP);
		this->musicbar.SlideBar(vect);
		this->disk.SlideDisk(vect);
		this->Update4th(songdata->jacketP[dif]);
	}

	void UpdateLR(MUSIC_BOX *songdata, int dif, int vect) {
		this->detail.SlideDif(vect);
		this->Update4th(songdata->jacketP[dif]);
	}

	void DrawUi(int cmd[], songdata_set_t *songdata) {
		this->backpic.DrawBackPic();
		this->jacket.DrawJacket(380, 85, 500);
		this->musicbar.DrawAll(300, cmd, songdata);
		this->disk.DrawDiskSet(-30, 25, songdata->sortMode);
		this->detail.DrawDetailAll(20, 60, &SONGDATA_FROM_MAP(songdata, cmd[0]), cmd[1]);
		this->previewSnd.CheckTime(&SONGDATA_FROM_MAP(songdata, cmd[0]), cmd[1]);
		this->previewSnd.CheckSnd(&SONGDATA_FROM_MAP(songdata, cmd[0]), cmd[1]);
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
	if (vect == 1) {
		cmd[1]--;
		if (cmd[1] < 0) {
			cmd[1] = 0;
			return;
		}
	}
	else {
		cmd[1]++;
		if (cmd[1] > SONGDATA_FROM_MAP(songdata, cmd[0]).limit) {
			cmd[1] = SONGDATA_FROM_MAP(songdata, cmd[0]).limit;
			return;
		}
	}

	uiClass->UpdateLR(&SONGDATA_FROM_MAP(songdata, cmd[0]), cmd[1], vect);
	if (songdata->sortMode == SORT_LEVEL || songdata->sortMode == SORT_SCORE) {
		SortSongWithSave(songdata, songdata->sortMode, cmd[1], &cmd[0]);
	}

	return;
}

static void RecSerectKeyActUD(int cmd[], int vect,
	rec_serect_ui_c *uiClass, songdata_set_t *songdata)
{
	if (vect == -1) {
		cmd[0]--;
		if (cmd[0] < 0) { cmd[0] = songdata->musicNum - 1; }
	}
	else {
		cmd[0]++;
		if (cmd[0] >= songdata->musicNum) { cmd[0] = 0; }
	}

	if (cmd[1] > SONGDATA_FROM_MAP(songdata, cmd[0]).limit) {
		RecSerectKeyActLR(cmd, 1, uiClass, songdata);
	}
	uiClass->UpdateUD(&SONGDATA_FROM_MAP(songdata, cmd[0]), cmd[1], vect);
	cmd[1] = RecSerectFetchDif(&SONGDATA_FROM_MAP(songdata, cmd[0]), cmd[1], songdata->sortMode);

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
	case 1: /* 曲決定 */
		// 選択できる曲であるかどうか (Lvが0以上であるかで判定)
		if (SONGDATA_FROM_MAP(songdata, cmd[0]).level[cmd[1]] < 0) { break; }
		RecSerectSetToPlay(toPlay, cmd, PackFirstNum, songdata);
		*next = SCENE_MUSIC;
		uiClass->cutin.SetCutTipFg(CUTIN_TIPS_SONG);
		uiClass->cutin.SetCutSong(SONGDATA_FROM_MAP(songdata, cmd[0]).SongName[cmd[1]],
			SONGDATA_FROM_MAP(songdata, cmd[0]).jacketP[cmd[1]]);
		uiClass->cutin.SetIo(1);
		break;
	case 2: /* 戻る */
		*next = SCENE_MENU;
		uiClass->cutin.SetTipNo();
		uiClass->cutin.SetCutTipFg(CUTIN_TIPS_ON);
		uiClass->cutin.SetIo(1);
		break;
	case 3: /* 曲選択上 */
		RecSerectKeyActUD(cmd, -1, uiClass, songdata);
		break;
	case 4: /* 曲選択下 */
		RecSerectKeyActUD(cmd, 1, uiClass, songdata);
		break;
	case 5: /* 難易度下降 */
		RecSerectKeyActLR(cmd, 1, uiClass, songdata);
		break;
	case 6: /* 難易度上昇 */
		RecSerectKeyActLR(cmd, -1, uiClass, songdata);
		break;
	case 7: /* 曲並び替え */
		ChangeSortMode(&songdata->sortMode);
		SortSongWithSave(songdata, songdata->sortMode, cmd[1], &cmd[0]);
		break;
	default:
		break;
	}

	return;
}

#endif /* after class action */

now_scene_t musicserect(rec_to_play_set_t *toPlay) {
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
	SortSong(&songdata, songdata.sortMode, cmd[1]);
	uiClass.InitUi(&SONGDATA_FROM_MAP(&songdata, cmd[0]), cmd[1]);
	AvoidKeyBug();
	GetMouseWheelRotVol();
	while (GetMouseInputLog2(NULL, NULL, NULL, NULL, true) == 0) {}
	uiClass.cutin.SetIo(0);

	while (1) {
		RecSerectDrawAllUi(&uiClass, cmd, &songdata, closeFg, CutTime);
		RecSerectKeyActAll(&next, toPlay, &closeFg, cmd, &CutTime,
			&uiClass, &songdata, PackFirstNum);
		if (uiClass.cutin.IsEndAnim()) { break; }
		if (GetWindowUserCloseFlag(TRUE)) {
			next = SCENE_EXIT;
			break;
		}
		WaitTimer(5);
	}

	RecSerectSaveBefCmd(cmd, songdata.sortMode);

	INIT_MAT();

	return next;
}
