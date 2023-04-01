
#include "DxLib.h"
#include "sancur.h"
#include "strcur.h"
#include "recr_cutin.h"

#define CUT_MES_POSX 75

#define RECR_DEBUG(ofs, data)											\
		DrawFormatString(20, 120 + ofs * 20, 0xffffffff, L#data": %d", data)

static char TipNo = 0;
static int pic_cutin[5];
static int snd_cutin[2];
static int CutInSndLastPlayTime = 0;
static int CutOutSndLastPlayTime = 0;
static wchar_t SongJucketName[256] = { L"NULL" };
static wchar_t CutSongName[256] = { L"NULL" };
static cutin_tips_e CutFg = CUTIN_TIPS_NONE;

/* tipの最大文字数は30文字(NULL終端除く) */
static wchar_t const tip[][31] = {
	L"音楽は万物に影響を与える", /* レコランの世界観 */
	L"ランナーは、音楽犯罪を防止する役目もある",
	L"ピッカーたちのいる地域は、比較的治安が良い",
	L"ゴールドランナーは全体の5%しかいない",
	L"HITノーツは同時押しOK、餡蜜OK", /* ゲームの仕様 */
	L"CATCHノーツの判定は前後にある",
	L"BOMBノーツの判定は極端に狭い",
	L"色違いのノーツもあるけど、操作は変わらない",
	L"レートとスコアは無関係",
	L"SAFE判定だとコンボが増えない",
	L"MISS判定でLIFEが減る",
	L"LIFEが無い時にMISSを出すと、代わりにスコアが減る",
	L"一度LIFEが0になると、走行距離が増えなくなる",
	L"一度LIFEが0になるとクリア失敗",
	L"簡単な譜面にLv10とかやっても、レートは上がらないよ",
	L"Record Runner の元ネタは、ビブリボン", /* レコランの裏話 */
	L"オートプレイは、まだ完全ではない",
	L"ARROWの3種類同時押しは禁止されている",
	L"カービンはケモナー", /* カービンのこと */
	L"カービンのtwitterもよろしくね",
	L"カービンの英語表記は、curbine",
	L"作曲、イラスト、プログラム、全部やってるのがカービン",
	L"クソ譜面を許すな", /* カービンの落書き */
	L"チュウニズムは早くLv18を出せ",
	L"音ゲーにクソゲーはない",
};
static char const tipNum = sizeof(tip) / sizeof(tip[0]);

void CutinReady() {
	pic_cutin[0] = LoadGraph(L"picture/cutin/cutinU.png");
	pic_cutin[1] = LoadGraph(L"picture/cutin/cutinD.png");
	pic_cutin[2] = LoadGraph(L"picture/cutin/cutinDisk.png");
	pic_cutin[3] = LoadGraph(SongJucketName);
	pic_cutin[4] = LoadGraph(L"picture/cutin/cutinS.png");
	snd_cutin[0] = LoadSoundMem(L"sound/IN.wav");
	snd_cutin[1] = LoadSoundMem(L"sound/OUT.wav");
}

void SetCutSong(wchar_t* songName, wchar_t* picName) {
	strcopy(songName, CutSongName, 1);
	strcopy(picName, SongJucketName, 1);
	pic_cutin[3] = LoadGraph(picName);
}

void SetCutTipFg(cutin_tips_e Fg) {
	CutFg = Fg;
}

void SetTipNo() {
	TipNo = (char)GetRand(tipNum - 1);
}

void ViewCutIn(int Stime) {
	int Ntime = GetNowCount();
	int EffTime = maxs(Ntime - Stime, 500);
	int PosY = pals(500, 0, 0, 360, EffTime);
	int PosS = pals(350, 0, 0, 100, EffTime);
	int Rot = pals(500, 0, 0, -300, EffTime);
	int Alpha = lins(500, 255, 0, 0, EffTime);
	DrawGraph(0, 0 - PosY, pic_cutin[0], TRUE);
	DrawGraph(0, 240 + PosY, pic_cutin[1], TRUE);
	DrawRotaGraph(320 - PosY, 240 - PosY, 1,
		(double)Rot / 100.0, pic_cutin[2], TRUE);
	switch (CutFg) {
	case CUTIN_TIPS_ON:
		DrawString(CUT_MES_POSX + PosY * 640 / 360, 430, tip[TipNo], 0xFF000000);
		break;
	case CUTIN_TIPS_SONG:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
		DrawExtendGraph(200 - PosY, 120 - PosY, 440 + PosY, 360 + PosY, pic_cutin[3], TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		DrawString(CUT_MES_POSX + PosY * 640 / 360, 430, CutSongName, 0xFF000000);
		break;
	default:
		/* none */
		break;
	}
	DrawGraph(0 - PosS, 0, pic_cutin[4], TRUE);
	DrawGraph(590 + PosS, 0, pic_cutin[4], TRUE);
	if (CutInSndLastPlayTime + 3000 < Ntime) {
		PlaySoundMem(snd_cutin[0], DX_PLAYTYPE_BACK);
		CutInSndLastPlayTime = Ntime;
	}
	return;
}

void ViewCutOut(int Stime) {
	int Ntime = GetNowCount();
	int EffTime = Ntime - Stime;
	if (500 < EffTime) {
		return;
	}
	int PosY = pals(0, 0, 500, 360, EffTime);
	int Rot = pals(0, 0, 500, 300, EffTime);
	int PosS = pals(150, 0, 500, 100, EffTime);
	int Alpha = lins(500, 0, 0, 255, EffTime);
	DrawGraph(0, 0 - PosY, pic_cutin[0], TRUE);
	DrawGraph(0, 240 + PosY, pic_cutin[1], TRUE);
	DrawRotaGraph(320 + PosY, 240 - PosY, 1,
		(double)Rot / 100.0, pic_cutin[2], TRUE);
	switch (CutFg) {
	case CUTIN_TIPS_ON:
		DrawString(CUT_MES_POSX - PosY * 640 / 360, 430, tip[TipNo], 0xFF000000);
		break;
	case CUTIN_TIPS_SONG:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
		DrawExtendGraph(200 - PosY, 120 - PosY, 440 + PosY, 360 + PosY, pic_cutin[3], TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		DrawString(CUT_MES_POSX - PosY * 640 / 360, 430, CutSongName, 0xFF000000);
		break;
	default:
		/* none */
		break;
	}
	DrawGraph(0 - PosS, 0, pic_cutin[4], TRUE);
	DrawGraph(590 + PosS, 0, pic_cutin[4], TRUE);
	if (CutOutSndLastPlayTime + 3000 < Ntime) {
		PlaySoundMem(snd_cutin[1], DX_PLAYTYPE_BACK);
		CutOutSndLastPlayTime = Ntime;
	}
	return;
}
