
#include "DxLib.h"
#include "general/sancur.h"
#include "general/strcur.h"
#include "recr_cutin.h"
#include "RecWindowRescale.h"

#define CUT_MES_POSX 75

#define RECR_DEBUG(ofs, data)											\
		RecRescaleDrawFormatString(20, 120 + ofs * 20, 0xffffffff, L#data": %d", data)

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
	L"「分かってはいたけど、ランナーって結構動くなぁ」", /* ピッカーのセリフ */
	L"「あぁ、風が気持ち良い…」",
	L"「ゲーターさん、耳引っ張るのだけはやめて」",
	L"「テイラーさんって、どのくらいすごいんだろう…?」",
	L"「お父さん、どこで何やってるんだろう…」",
	L"「パフェ取るまで何度でもチャレンジだっ!」", /* ゲーターのセリフ */
	L"「んがぁ～…、目覚ましがうるせぇ…」",
	L"「ピッカーは新人だし、俺が色々教えなきゃな」",
	L"「テイラー、たまにはお前も働いてくれねーか?」",
	L"「絶対あいつらに見返してやるんだ」",
	L"「私はただの音楽好きのドラゴンさ♪」", /* テイラーのセリフ */
	L"「きれいなものと可愛いものに目がなかったりする♪」",
	L"「ピッカー君は色々と期待できる子だよ♪」",
	L"「ゲーター、ピッカー君にあまり意地悪しないでね♪」",
	L"「私だって、本当は働きたいんだよ」",
	L"HITノーツはキャラを動かさなくて良い", /* ゲームの仕様 */
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
	L"上下同時押ししてるときは、キャラは真ん中に来る",
	L"HITノーツは、同時押しOK、餡蜜OK", /* ゲームのコツ */
	L"両手をZXCキーに置くのも手の内",
	L"両手を十字操作キーに置くのも手の内",
	L"CATCHノーツは、上下キーをガチャガチャしても取れる",
	L"全押ししても良いけど、巻き込み注意ね",
	L"オプションで、プレイするキャラクターを変えれるよ", /* オプション周り */
	L"ノーツと音楽が合ってないときは、オプションで調節しよう",
	L"プレイ中の効果音は、オプションで消せるよ",
	L"オプションで、プレイ中の背景を消せるよ",
	L"オプションで、プレイ中にキー押し状態を表示できるよ",
	L"オプションで、プレイ中の判定の位置を変えれるよ",
	L"Record Runner の元ネタは、ビブリボン", /* レコランの雑話 */
	L"オートプレイは、まだ完全ではない",
	L"ARROWの3種類同時押しは禁止されている",
	L"Muse Dash? いや…、当時そのゲーム知らなかったし…",
	L"長押しノーツ? 追加しないよそんなもん",
	L"まだまだ追加したい機能はあるんだ",
	L"譜面のレギュレーションは一応ある",
	L"選曲画面の時、マウスも反応する",
	L"カービンはケモナー", /* カービンのこと */
	L"カービンのtwitterもよろしくね",
	L"カービンの英語表記は、curbine",
	L"作曲、イラスト、プログラム、全部やってるのがカービン",
	L"クソ譜面はほどほどにな", /* カービンの落書き */
	L"チュウニズムは早くLv18を出せ",
	L"音ゲーにクソゲーはない",
};
static char const tipNum = sizeof(tip) / sizeof(tip[0]);

void CutinReadyPic() {
	pic_cutin[0] = LoadGraph(L"picture/cutin/cutinU.png");
	pic_cutin[1] = LoadGraph(L"picture/cutin/cutinD.png");
	pic_cutin[2] = LoadGraph(L"picture/cutin/cutinDisk.png");
	pic_cutin[3] = LoadGraph(SongJucketName);
	pic_cutin[4] = LoadGraph(L"picture/cutin/cutinS.png");
}

void CutinReadySnd() {
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
	RecRescaleDrawGraph(0, 0 - PosY, pic_cutin[0], TRUE);
	RecRescaleDrawGraph(0, 240 + PosY, pic_cutin[1], TRUE);
	RecRescaleDrawRotaGraph(320 - PosY, 240 - PosY, 1,
		(double)Rot / 50.0, pic_cutin[2], TRUE);
	switch (CutFg) {
	case CUTIN_TIPS_ON:
		RecRescaleDrawString(CUT_MES_POSX + PosY * 640 / 360, 430, tip[TipNo], 0xFF000000);
		break;
	case CUTIN_TIPS_SONG:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
		RecRescaleDrawExtendGraph(200 - PosY, 120 - PosY, 440 + PosY, 360 + PosY, pic_cutin[3], TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		RecRescaleDrawString(CUT_MES_POSX + PosY * 640 / 360, 430, CutSongName, 0xFF000000);
		break;
	default:
		/* none */
		break;
	}
	RecRescaleDrawGraph(0 - PosS, 0, pic_cutin[4], TRUE);
	RecRescaleDrawGraph(590 + PosS, 0, pic_cutin[4], TRUE);
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
	RecRescaleDrawGraph(0, 0 - PosY, pic_cutin[0], TRUE);
	RecRescaleDrawGraph(0, 240 + PosY, pic_cutin[1], TRUE);
	RecRescaleDrawRotaGraph(320 + PosY, 240 - PosY, 1,
		(double)Rot / 50.0, pic_cutin[2], TRUE);
	switch (CutFg) {
	case CUTIN_TIPS_ON:
		RecRescaleDrawString(CUT_MES_POSX - PosY * 640 / 360, 430, tip[TipNo], 0xFF000000);
		break;
	case CUTIN_TIPS_SONG:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
		RecRescaleDrawExtendGraph(200 - PosY, 120 - PosY, 440 + PosY, 360 + PosY, pic_cutin[3], TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		RecRescaleDrawString(CUT_MES_POSX - PosY * 640 / 360, 430, CutSongName, 0xFF000000);
		break;
	default:
		/* none */
		break;
	}
	RecRescaleDrawGraph(0 - PosS, 0, pic_cutin[4], TRUE);
	RecRescaleDrawGraph(590 + PosS, 0, pic_cutin[4], TRUE);
	if (CutOutSndLastPlayTime + 3000 < Ntime) {
		PlaySoundMem(snd_cutin[1], DX_PLAYTYPE_BACK);
		CutOutSndLastPlayTime = Ntime;
	}
	return;
}
