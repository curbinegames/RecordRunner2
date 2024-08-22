
#include <DxLib.h>
#include "serectbase.h"
#include "recr_cutin.h"
#include "general/dxcur.h"
#include "general/sancur.h"
#include "general/strcur.h"
#include "general/dxdraw.h"
#include "system.h"

#define MUSE_FADTM 250
#define MUSE_KEYTM 500

typedef TCHAR rec_pack_name_set_t[256];

static void RecSerectReadMapDataOneDif(TCHAR *path, TCHAR *subpath, MUSIC_BOX *songdata, int dif, int lang)
{
	DxFile_t fd;
	TCHAR buf[256];

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

static void RecSerectReadMapDataOneSong(MUSIC_BOX *songdata, TCHAR *packName, TCHAR *songName, int lang) {
	TCHAR path[256];
	TCHAR subPath[256];

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
		strcats(path, _T(".txt")); //"record/<パック名>/<曲名>/<難易度番号>.txt"
		//初期値定義(ファイルがなくても代入する)
		strcopy(L"NULL", songdata->SongName[iDif], 1);
		strcopy(L"NULL", songdata->artist[iDif], 1);
		strcopy(L"NULL", songdata->SongFileName[iDif], 1);
		strcopy(L"picture/NULL jucket.png", songdata->jacketP[iDif], 1);
		strcopy(packName, songdata->packName, 1);
		songdata->level[iDif] = -1;
		songdata->preview[iDif][0] = 441000;
		songdata->preview[iDif][1] = 2646000;
		RecSerectReadMapDataOneDif(path, subPath, songdata, iDif, lang);
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
 * @param[out] PackName ret
 * @param[out] PackFirstNum ret
 * @param[out] Mapping ret
 * @param[in] lang 言語
 * @return int 読み込んだ曲数
 */
static int RecSerectReadMapData(MUSIC_BOX songdata[], rec_pack_name_set_t PackName[],
	int PackFirstNum[], int Mapping[], int lang)
{
	int packNum = 0;
	int songCount = 0;
	DxFile_t fd;
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
			RecSerectReadMapDataOneSong(&songdata[songCount], PackName[i], songName, lang);
			//後処理
			Mapping[songCount] = songCount;
			songCount++;
		}
		FileRead_close(fd);
	}
	return songCount;
}

static class rec_serect_musicbar_c {
private:
	int UD = 1;
	int startC = -MUSE_FADTM;
	DxPic_t bar[2];
	DxPic_t CRate[5];
	DxPic_t rankP[6];

	void DrawClear(int x, int y, int clearNo) {
		if (0 <= clearNo && clearNo <= 4) {
			RecRescaleDrawGraph(x, y, this->CRate[clearNo], TRUE);
		}
	}

	void DrawRack(int x, int y, int rankNo) {
		if (0 <= rankNo && rankNo <= 5) {
			RecRescaleDrawGraph(x, y, this->rankP[rankNo], TRUE);
		}
	}

	void DrawMainOne(int dif, int BasePosX, int BasePosY, MUSIC_BOX *songdata) {
		RecRescaleDrawGraph(BasePosX - 120, BasePosY - 170, this->bar[1], TRUE);
		RecRescaleDrawString(BasePosX - 30, BasePosY - 157, songdata->SongName[dif], COLOR_BLACK);
		RecRescaleDrawString(BasePosX - 30, BasePosY - 129, songdata->artist[dif], COLOR_BLACK);
		this->DrawClear(BasePosX + 156, BasePosY - 132, songdata->ClearRate[dif] - 1);
		this->DrawRack(BasePosX + 156, BasePosY - 132, songdata->ClearRank[dif]);
		for (int idif = 0; idif < 3; idif++) {
			RecRescaleDrawFormatString(BasePosX - 25 + idif * 70, BasePosY - 97,
				COLOR_BLACK, L"%2d", songdata->level[1 + idif]);
		}
	}

	void DrawSubOne(int dif, int BasePosX, int BasePosY, MUSIC_BOX *songdata) {
		RecRescaleDrawGraph(BasePosX - 120, BasePosY - 170, this->bar[0], TRUE);
		RecRescaleDrawString(BasePosX - 30, BasePosY - 157, songdata->SongName[dif], COLOR_WHITE);
		RecRescaleDrawString(BasePosX - 30, BasePosY - 129, songdata->artist[dif], COLOR_WHITE);
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

	void DrawAll(int cmd1, int dif, int SongNumCount, MUSIC_BOX *songdata, int *Mapping) {
		int BasePosX = 0;
		int BasePosY = 0;
		int slide = 0;
		int picsong = 0;
		int moveC = 0;

		moveC = mins(-1 * (GetNowCount() - this->startC) + MUSE_FADTM, 0);
		picsong = (cmd1 + SongNumCount - 3) % SongNumCount;

		for (int count = 0; count < 7; count++) {
			slide = pals(0, 0, 250, this->UD * 80, moveC);
			if (count < 3) {
				BasePosY = slide + count * 80 + 120;
			}
			else if (count == 3) {
				BasePosY = slide + count * 120;
			}
			else {
				BasePosY = slide + count * 80 + 160;
			}
			BasePosX = lins(480, 80, 240, 40, BasePosY);

			if (count == 3) {
				this->DrawMainOne(dif, BasePosX, BasePosY, &songdata[Mapping[picsong]]);
			}
			else {
				this->DrawSubOne(dif, BasePosX, BasePosY, &songdata[Mapping[picsong]]);
			}

			picsong = (picsong + 1) % SongNumCount;
		}
	}
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

	void DrawNamePlate() {
		DrawGraphAnchor(-175, 5, this->rateBar, DXDRAW_ANCHOR_TOP_RIGHT);
		DrawGraphAnchor(-145, 5, this->runner, DXDRAW_ANCHOR_TOP_RIGHT);
		DrawFormatStringToHandleAnchor(-365, 17, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT, L"Lv:%2d", this->Lv);
		DrawFormatStringToHandleAnchor(-360, 42, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT, L"RATE:%d.%02d", this->rate / 100, this->rate % 100);
	}

	void DrawDisk() {
		int moveC = 0;

		moveC = mins(-1 * (GetNowCount() - this->startC) + MUSE_FADTM, 0);
		if (this->UD == 1) { this->Nrot += pals(0, 2, MUSE_FADTM, -75, moveC) / 100.0; }
		else { this->Nrot += pals(0, 2, MUSE_FADTM, 75, moveC) / 100.0; }
		if (this->Nrot > 6.28) { this->Nrot -= 6.28; }
		else if (this->Nrot < 0) { this->Nrot += 6.28; }
		DrawRotaGraphAnchor(-30, 25, 1, this->Nrot, this->disk, DXDRAW_ANCHOR_TOP_RIGHT, TRUE);
	}

	void DrawSort(int mode, int lan) {
		if (lan == 1) {
			switch (mode) {
			case SORT_DEFAULT:
				DrawStringToHandleAnchor(-90, 110, L"default", COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
				break;
			case SORT_LEVEL:
				DrawStringToHandleAnchor(-90, 110, L"level", COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
				break;
			case SORT_SCORE:
				DrawStringToHandleAnchor(-90, 110, L"score", COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
				break;
			}
		}
		else {
			switch (mode) {
			case SORT_DEFAULT:
				DrawStringToHandleAnchor(-90, 110, L"デフォルト", COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
				break;
			case SORT_LEVEL:
				DrawStringToHandleAnchor(-90, 110, L"レベル順", COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
				break;
			case SORT_SCORE:
				DrawStringToHandleAnchor(-90, 110, L"スコア順", COLOR_WHITE, SmallFontData, DXDRAW_ANCHOR_TOP_RIGHT);
				break;
			}
		}
	}

public:
	rec_serect_disk_c(int CharNo) {
		FILE *fp = NULL;

		_wfopen_s(&fp, L"save/chap.dat", L"rb");
		if (fp != NULL) {
			int buf[3];
			fread(buf, sizeof(int), 3, fp);
			this->Lv = buf[CharNo] + 1;
			fclose(fp);
		}

		this->rate = GetRate();

		switch (CharNo) {
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

	void DrawDiskSet(int mode, int lang) {
		this->DrawNamePlate();
		this->DrawDisk();
		this->DrawSort(mode, lang);
	}
};

static class rec_serect_detail_c {
private:
	int LR = 1;
	int XstartC = -MUSE_FADTM;
	TCHAR viewingDifBar[255] = { L"NULL" };
	DxPic_t difbar[6];
	DxPic_t detail;
	DxPic_t difC[10];

	void DrawDifMark(MUSIC_BOX *songdata, int comdif) {
		int posX = 0;
		int posY = 0;
		for (int i = 0; i < 5; i++) {
			posY = 0;
			if (comdif == i) { posY = 1; }
			if (comdif <= i) { posX = 1; }
			if (strands(songdata->SongFileName[i], L"NULL") == 0 && i <= songdata->limit) {
				DrawGraphAnchor(11 * posX + 16 * i - 80, -165, this->difC[i * 2 + posY], DXDRAW_ANCHOR_BOTTOM_RIGHT);
			}
		}
	}

	void DrawDifBar(int dif) {
		int XmoveC = mins(-1 * (GetNowCount() - this->XstartC) + MUSE_FADTM, 0);

		if (this->LR == 1) {
			XmoveC = pals(0, 640, MUSE_FADTM, 460, XmoveC);
			DrawGraphAnchor(5, -120, this->difbar[dif], DXDRAW_ANCHOR_BOTTOM_RIGHT);
			DrawGraphAnchor(XmoveC - 640 + 184, -120, this->difbar[dif + 1], DXDRAW_ANCHOR_BOTTOM_RIGHT);
		}
		else if (this->LR == -1) {
			XmoveC = pals(0, 460, MUSE_FADTM, 640, XmoveC);
			DrawGraphAnchor(5, -120, this->difbar[dif - 1], DXDRAW_ANCHOR_BOTTOM_RIGHT);
			DrawGraphAnchor(XmoveC - 640 + 184, -120, this->difbar[dif], DXDRAW_ANCHOR_BOTTOM_RIGHT);
		}
	}

	void DrawDetail(MUSIC_BOX *songdata, int dif) {
		DrawGraphAnchor(20, 70, this->detail, DXDRAW_ANCHOR_BOTTOM_RIGHT);
		DrawFormatStringToHandleAnchor(-310, -100, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_BOTTOM_RIGHT, L"%s", songdata->packName);
		DrawFormatStringToHandleAnchor(-305, -75, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_BOTTOM_RIGHT, L"Lv.%2d", songdata->level[dif]);
		for (int i = 0; i < 15; i++) {
			if (10 <= i && songdata->level[dif] <= i) {
				break;
			}
			if (i < songdata->level[dif]) {
				DrawStringToHandleAnchor(16 * i - 250, -75, L"★", COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_BOTTOM_RIGHT);
			}
			else {
				DrawStringToHandleAnchor(16 * i - 250, -75, L"☆", COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_BOTTOM_RIGHT);
			}
		}
		DrawFormatStringToHandleAnchor(-300, -50, COLOR_BLACK, SmallFontData, DXDRAW_ANCHOR_BOTTOM_RIGHT, L"HighSCORE:%6d/%6.2f%%/%5.3fkm",
			songdata->Hscore[dif], songdata->Hacc[dif], songdata->Hdis[dif] / 1000.0);
	}

public:
	rec_serect_detail_c() {
		this->difbar[0] = LoadGraph(L"picture/difauto.png");
		this->difbar[1] = LoadGraph(L"picture/difeasy.png");
		this->difbar[2] = LoadGraph(L"picture/difnormal.png");
		this->difbar[3] = LoadGraph(L"picture/difhard.png");
		this->difbar[4] = LoadGraph(L"picture/difanother.png");
		this->difbar[5] = this->difbar[4];
		this->detail = LoadGraph(L"picture/detail.png");
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
		DeleteGraph(this->difbar[0]);
		DeleteGraph(this->difbar[1]);
		DeleteGraph(this->difbar[2]);
		DeleteGraph(this->difbar[3]);
		DeleteGraph(this->difbar[4]);
		for (int i = 0; i < 10; i++) {
			DeleteGraph(this->difC[i]);
		}
	}

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

	void DrawDetailAll(MUSIC_BOX *songdata, int dif) {
		this->DrawDifMark(songdata, dif);
		this->DrawDifBar(dif);
		this->DrawDetail(songdata, dif);
	}
};

now_scene_t musicserect2(int *p1) {
	FILE *fp;
	char closeFg = 0;
	int NTime = 0;
	int e;
	int G[10];
	int command[2] = { 0,1 };
	int SongNumCount = 0;
	int preSC = 0;
	int ShiftKey = 0;
	int AutoFlag = 0;
	int SongPreSTime = 0;
	int SongPrePat = 0;
	int SortMode = SORT_DEFAULT;
	int PackFirstNum[PackNumLim];
	int Mapping[SongNumLim];
	int	lan[6] = { 0,0,0,2,0,0 }; //使うのは[0:キャラ, 4:言語]だけ
	int backpos = 0;
	int CutTime = 0;
	const int keyCB[7] = {
		KEY_INPUT_RETURN, KEY_INPUT_BACK, KEY_INPUT_UP,
		KEY_INPUT_DOWN, KEY_INPUT_LEFT, KEY_INPUT_RIGHT, KEY_INPUT_Z
	};
	//wchar_t変数定義
	rec_pack_name_set_t PackName[PackNumLim];
	wchar_t viewingjacket[255] = { L"picture/NULL jucket.png" };
	wchar_t playingsong[255] = { L"NULL" };
	now_scene_t next = SCENE_EXIT;
	MUSIC_BOX songdata[SongNumLim];
	int back = LoadGraph(L"picture/MSback.png");
	int jacketpic = LoadGraph(L"picture/NULL jucket.png");
	int help = LoadGraph(L"picture/help.png");
	int previewM = LoadSoundMem(L"null.mp3");
	int select = LoadSoundMem(L"sound/arrow.wav");
	rec_serect_musicbar_c musicbarClass;
	/* extern rec_serect_disk_c dickClass(); */
	rec_serect_detail_c detailClass;
	/* TODO: システム情報を取得する関数を別ファイルに作る */
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&lan, sizeof(int), 6, fp);
		fclose(fp);
	}
	rec_serect_disk_c diskClass(lan[0]);
	G[0] = _wfopen_s(&fp, L"save/SongSelect2.dat", L"rb");
	if (G[0] == 0) {
		fread(&command, sizeof(int), 2, fp);
		fread(&SortMode, sizeof(int), 1, fp);
		fclose(fp);
	}

	SongNumCount = RecSerectReadMapData(songdata, PackName, PackFirstNum, Mapping, lan[4]);

	//曲のソート
	AvoidKeyBug();
	G[0] = Mapping[command[0]];
	SortSong(songdata, Mapping, SortMode, command[1], SongNumCount);
	for (int i = 0; i < SongNumCount; i++) {
		if (Mapping[i] == G[0]) {
			command[0] = i;
		}
	}
	G[0] += 0;
	GetMouseWheelRotVol();
	while (GetMouseInputLog2(NULL, NULL, NULL, NULL, true) == 0) {}
	G[0] = Mapping[command[0]];
	G[1] = songdata[G[0]].preview[command[1]][0];
	// 開始位置に曲ファイルがあれば再生
	if (strands(songdata[G[0]].SongFileName[command[1]], L"NULL") == 0) {
		SongPrePat = 0; // 再生位置パターンを0に設定
		strcopy(songdata[G[0]].SongFileName[command[1]], playingsong, 1); // 曲名取得
		previewM = LoadSoundMem(playingsong); // 曲をfdに読み込み
		SetCurrentPositionSoundMem(G[1], previewM); // 再生位置
		ChangeVolumeSoundMem(0, previewM); // 音量
		PlaySoundMem(previewM, DX_PLAYTYPE_BACK, FALSE); // 再生
		WaitTimer(10); // ウェイト
		SongPreSTime = GetNowCount(); // 再生開始時間を保存
	}
	detailClass.FetchDifPic(songdata[Mapping[command[0]]].difP);
	CutTime = GetNowCount();
	while (1) {
		NTime = GetNowCount();
		ClearDrawScreen();
		//背景の表示
		RecRescaleDrawGraph(backpos, 0, back, TRUE);
		RecRescaleDrawGraph(backpos + 640, 0, back, TRUE);
		backpos = (backpos - 2) % 640;
		//曲一覧、ディスク表示
		musicbarClass.DrawAll(command[0], command[1], SongNumCount, songdata, Mapping);
		diskClass.DrawDiskSet(SortMode, lan[4]);
		//ジャケット表示
		if (strands(viewingjacket, songdata[Mapping[command[0]]].jacketP[command[1]]) == 0) {
			DeleteGraph(jacketpic);
			strcopy(songdata[Mapping[command[0]]].jacketP[command[1]], viewingjacket, 1);
			jacketpic = LoadGraph(viewingjacket);
		}
		RecRescaleDrawExtendGraph(305, 75, 545, 315, jacketpic, TRUE);
		//詳細表示
		detailClass.DrawDetailAll(&songdata[Mapping[command[0]]], command[1]);
		//プレビューを流す
		if (preSC + MUSE_KEYTM < NTime
			&& strands(songdata[Mapping[command[0]]].SongFileName[command[1]], L"NULL") == 0
			&& strands(playingsong, songdata[Mapping[command[0]]].SongFileName[command[1]]) == 0) {
			StopSoundMem(previewM);
			DeleteSoundMem(previewM);
			SongPrePat = 0;
			strcopy(songdata[Mapping[command[0]]].SongFileName[command[1]], playingsong, 1);
			previewM = LoadSoundMem(playingsong);
			SetCurrentPositionSoundMem(songdata[Mapping[command[0]]].preview[command[1]][0], previewM);
			ChangeVolumeSoundMem(0, previewM);
			PlaySoundMem(previewM, DX_PLAYTYPE_BACK, FALSE);
			WaitTimer(30);
			NTime = GetNowCount();
			SongPreSTime = NTime;
		}
		if (NTime - SongPreSTime < 500) {
			ChangeVolumeSoundMem(lins(0, 0, 500, 255, NTime - SongPreSTime), previewM);
		}
		else if (500 <= NTime - SongPreSTime && NTime - SongPreSTime < 14500) {
			ChangeVolumeSoundMem(255, previewM);
		}
		else if (14500 <= NTime - SongPreSTime && NTime - SongPreSTime < 15000) {
			ChangeVolumeSoundMem(lins(14500, 255, 15000, 0, NTime - SongPreSTime), previewM);
		}
		else if (15000 <= NTime - SongPreSTime) {
			StopSoundMem(previewM);
			SongPrePat++;
			SetCurrentPositionSoundMem(songdata[Mapping[command[0]]].preview[command[1]][SongPrePat % 2], previewM);
			ChangeVolumeSoundMem(0, previewM);
			PlaySoundMem(previewM, DX_PLAYTYPE_BACK, FALSE);
			WaitTimer(30);
			SongPreSTime = NTime;
		}
		//TODO: 別に関数がある
		//操作説明を表示する
		ShowHelpBar(COLOR_WHITE, help, lan[4]);
		//デバッグ(320,410スタート)
		//RecRescaleDrawFormatString(320, 410, COLOR_WHITE, L"%d", SortMode);
		//TODO: カットインをclassにする
		if (closeFg == 0) {
			ViewCutOut(CutTime);
		}
		if (closeFg == 1) {
			ViewCutIn(CutTime);
		}
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_LSHIFT) == 1 || CheckHitKey(KEY_INPUT_RSHIFT) == 1) { ShiftKey = 1; }
		else { ShiftKey = 0; }
		if (CheckHitKey(KEY_INPUT_P) == 1) { AutoFlag = 1; }
		else { AutoFlag = 0; }
		G[0] = 0;
		/* マウス入力 */
		G[1] = 0;
		G[2] = 0;
		G[3] = 0;
		G[4] = 0;
		while (GetMouseInputLog2(&G[1], &G[2], &G[3], &G[4], true) == 0) {}
		if (G[1] == MOUSE_INPUT_LEFT && G[4] == MOUSE_INPUT_LOG_UP) {
			if (5 <= G[2] && G[2] <= 280 && 195 <= G[3] && G[3] <= 290) {
				G[0] = 1;
			}
			if (5 <= G[2] && G[2] <= 245 && 5 <= G[3] && G[3] <= 175) {
				G[0] = 3;
			}
			if (5 <= G[2] && G[2] <= 305 && 310 <= G[3] && G[3] <= 475) {
				G[0] = 4;
			}
			if (505 <= G[2] && G[2] <= 635 && 5 <= G[3] && G[3] <= 130) {
				G[0] = 7;
			}
		}
		/* ホイール入力 */
		G[1] = GetMouseWheelRotVol();
		if (1 <= G[1]) { /* 奥回し */
			G[0] = 3;
		}
		if (G[1] <= -1) { /* 手前回し */
			G[0] = 4;
		}
		/* キー入力 */
		G[1] = keycur(keyCB, sizeof(keyCB) / sizeof(keyCB[0]));
		switch (G[1]) {
		case KEY_INPUT_RETURN:
			G[0] = 1;
			break;
		case KEY_INPUT_BACK:
			G[0] = 2;
			break;
		case KEY_INPUT_UP:
			G[0] = 3;
			break;
		case KEY_INPUT_DOWN:
			G[0] = 4;
			break;
		case KEY_INPUT_LEFT:
			G[0] = 5;
			break;
		case KEY_INPUT_RIGHT:
			G[0] = 6;
			break;
		case KEY_INPUT_Z:
			G[0] = 7;
			break;
		default:
			break;
		}
		/* 操作の結果反映 */
		if (G[0] == 1) { /* 曲決定 */
			//Lvが0以上であるか
			if (0 <= songdata[Mapping[command[0]]].level[command[1]]) {
				//隠し曲用
				if (command[1] == 3 && songdata[Mapping[command[0]]].Hscore[3] >= 90000
					&& strands(songdata[Mapping[command[0]]].SongFileName[5], L"NULL") == 0
					&& songdata[Mapping[command[0]]].Hscore[5] <= 0) {
					G[0] = 0;
					if (songdata[Mapping[command[0]]].Hscore[3] >= 90000
						&& songdata[Mapping[command[0]]].Hscore[3] < 92500) {
						G[0] = pals(90000, 0, 92500, 25, songdata[Mapping[command[0]]].Hscore[3]);
					}
					else if (songdata[Mapping[command[0]]].Hscore[3] >= 92500
						&& songdata[Mapping[command[0]]].Hscore[3] < 95000) {
						G[0] = pals(95000, 50, 92500, 25, songdata[Mapping[command[0]]].Hscore[3]);
					}
					else if (songdata[Mapping[command[0]]].Hscore[3] >= 95000
						&& songdata[Mapping[command[0]]].Hscore[3] < 98000) {
						G[0] = pals(95000, 50, 98000, 750, songdata[Mapping[command[0]]].Hscore[3]);
					}
					else if (songdata[Mapping[command[0]]].Hscore[3] >= 98000
						&& songdata[Mapping[command[0]]].Hscore[3] < 99000) {
						G[0] = pals(99000, 1000, 98000, 750, songdata[Mapping[command[0]]].Hscore[3]);
					}
					else if (songdata[Mapping[command[0]]].Hscore[3] >= 99000) {
						G[0] = 1000;
					}
					if (GetRand(1000) <= G[0]) { command[1] = 5; }
				}
				G[0] = 0;
				for (G[0] = PackNumLim; G[0] >= 0; G[0]--) {
					if (PackFirstNum[G[0]] >= 0 && PackFirstNum[G[0]] <= Mapping[command[0]]) {
						*p1 = G[0];
						break;
					}
				}
				p1++;
				*p1 = Mapping[command[0]] - PackFirstNum[G[0]];
				p1++;
				*p1 = command[1];
				p1++;
				*p1 = ShiftKey;
				p1++;
				*p1 = AutoFlag;
				next = SCENE_MUSIC;
				SetCutTipFg(CUTIN_TIPS_SONG);
				SetCutSong(songdata[Mapping[command[0]]].SongName[command[1]],
					songdata[Mapping[command[0]]].jacketP[command[1]]);
				closeFg = 1;
				CutTime = NTime;
			}
		}
		else if (G[0] == 2) { /* 戻る */
			next = SCENE_MENU;
			SetTipNo();
			SetCutTipFg(CUTIN_TIPS_ON);
			closeFg = 1;
			CutTime = NTime;
		}
		else {
			switch (G[0]) {
			case 3: /* 曲選択上 */
				command[0]--;
				//縦コマンド(曲)の端を過ぎたとき、もう片方の端に移動する
				if (command[0] < 0) { command[0] = SongNumCount - 1; }
				if (command[1] > songdata[Mapping[command[0]]].limit) {
					command[1] = songdata[Mapping[command[0]]].limit;
					detailClass.SlideDif(1);
					SortSong(songdata, Mapping, SortMode, command[1], SongNumCount);
				}
				detailClass.FetchDifPic(songdata[Mapping[command[0]]].difP);
				musicbarClass.SlideBar(-1);
				diskClass.SlideDisk(-1);
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				preSC = NTime;
				//デフォルトソートで、今選んだ曲の難易度に譜面が無かったら、譜面がある難易度を探す。
				if (SortMode == SORT_DEFAULT && strands(songdata[Mapping[command[0]]].SongName[command[1]], L"NULL")) {
					if (strands(songdata[Mapping[command[0]]].SongName[0], L"NULL") != 1) {
						command[1] = 0;
					}
					for (int i = 1; i <= 3; i++) {
						if (strands(songdata[Mapping[command[0]]].SongName[i], L"NULL") != 1) {
							command[1] = i;
							break;
						}
					}
				}
				break;
			case 4: /* 曲選択下 */
				command[0]++;
				//縦コマンド(曲)の端を過ぎたとき、もう片方の端に移動する
				if (command[0] >= SongNumCount) command[0] = 0;
				if (command[1] > songdata[Mapping[command[0]]].limit) {
					command[1] = songdata[Mapping[command[0]]].limit;
					detailClass.SlideDif(1);
					SortSong(songdata, Mapping, SortMode, command[1], SongNumCount);
				}
				detailClass.FetchDifPic(songdata[Mapping[command[0]]].difP);
				musicbarClass.SlideBar(1);
				diskClass.SlideDisk(1);
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				preSC = NTime;
				//デフォルトソートで、今選んだ曲の難易度に譜面が無かったら、譜面がある難易度を探す。
				if (SortMode == SORT_DEFAULT &&
					strands(songdata[Mapping[command[0]]].SongName[command[1]], L"NULL")) {
					if (strands(songdata[Mapping[command[0]]].SongName[0], L"NULL") != 1) command[1] = 0;
					for (int i = 1; i <= 3; i++) {
						if (strands(songdata[Mapping[command[0]]].SongName[i], L"NULL") != 1) {
							command[1] = i;
							break;
						}
					}
				}
				break;
			case 5: /* 難易度下降 */
				command[1]--;
				if (command[1] < 0) {
					command[1] = 0;
				}
				else {
					detailClass.SlideDif(1);
				}
				preSC = NTime;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				if (SortMode == SORT_LEVEL || SortMode == SORT_SCORE) {
					G[0] = Mapping[command[0]];
					SortSong(songdata, Mapping, SortMode, command[1], SongNumCount);
					for (int i = 0; i < SongNumCount; i++) {
						if (Mapping[i] == G[0]) {
							command[0] = i;
						}
					}
				}
				break;
			case 6: /* 難易度上昇 */
				command[1]++;
				if (command[1] > songdata[Mapping[command[0]]].limit) {
					command[1] = songdata[Mapping[command[0]]].limit;
				}
				else {
					detailClass.SlideDif(-1);
				}
				preSC = NTime;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				if (SortMode == SORT_LEVEL || SortMode == SORT_SCORE) {
					G[0] = Mapping[command[0]];
					SortSong(songdata, Mapping, SortMode, command[1], SongNumCount);
					for (int i = 0; i < SongNumCount; i++) {
						if (Mapping[i] == G[0]) {
							command[0] = i;
						}
					}
				}
				break;
			case 7: /* 曲並び替え */
				ChangeSortMode(&SortMode);
				G[0] = Mapping[command[0]];
				SortSong(songdata, Mapping, SortMode, command[1], SongNumCount);
				for (int i = 0; i < SongNumCount; i++) {
					if (Mapping[i] == G[0]) {
						command[0] = i;
					}
				}
				break;
			default:
				break;
			}
		}
		if (closeFg == 1 && CutTime + 2000 <= NTime) {
			StopSoundMem(previewM);
			ClearDrawScreen();
			INIT_MAT();
			break;
		}
		if (GetWindowUserCloseFlag(TRUE)) {
			next = SCENE_EXIT;
			break;
		}
		WaitTimer(5);
	}
	//選択曲の保存
	G[0] = _wfopen_s(&fp, L"save/SongSelect2.dat", L"wb");
	if (fp != NULL) {
		fwrite(&Mapping[command[0]], sizeof(int), 1, fp);
		fwrite(&command[1], sizeof(int), 1, fp);
		fwrite(&SortMode, sizeof(int), 1, fp);
		fclose(fp);
	}
	return next;
}