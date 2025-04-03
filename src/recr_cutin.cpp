
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <option.h>
#include <RecWindowRescale.h>

/* own include */
#include <recr_cutin.h>

#define CUT_MES_POSX 75

#define CUT_FRAG_OUT 0
#define CUT_FRAG_IN 1

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
static int s_cutIoFg = CUT_FRAG_OUT;
static int s_cutStime = 0;

/* TODO: 編集いるよ(1.5.5でもいいね) */
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
static char const tipNum = ARRAY_COUNT(tip);

#if 1 /* action */

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
	ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, snd_cutin[0]);
	ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, snd_cutin[1]);
}

void SetCutSong(wchar_t* songName, wchar_t* picName) {
	strcopy_2(songName, CutSongName, 255);
	strcopy_2(picName, SongJucketName, 255);
	pic_cutin[3] = LoadGraph(picName);
}

void SetCutTipFg(cutin_tips_e Fg) {
	CutFg = Fg;
}

void SetTipNo() {
	TipNo = (char)GetRand(tipNum - 1);
}

static void RecCutDrawShut(int EffTime) {
	int PosY = 0;
	if (s_cutIoFg == CUT_FRAG_OUT) {
		PosY = pals(0, 0, 500, 360, EffTime);
	}
	else {
		PosY = pals(500, 0, 0, 360, EffTime);
	}
	RecRescaleDrawGraph(0, 0 - PosY, pic_cutin[0], TRUE);
	RecRescaleDrawGraph(0, 240 + PosY, pic_cutin[1], TRUE);
	return;
}

static void RecCutDrawDisk(int EffTime) {
	int DrawX = 0;
	int Rot = 0;
	int PosY = 0;

	if (s_cutIoFg == CUT_FRAG_OUT) {
		PosY = pals(0, 0, 500, 360, EffTime);
		DrawX = 320 + pals(0, 0, 500, 360, EffTime);
		Rot = pals(0, 0, 500, 300, EffTime);
	}
	else {
		PosY = pals(500, 0, 0, 360, EffTime);
		DrawX = 320 - pals(500, 0, 0, 360, EffTime);
		Rot = pals(500, 0, 0, -300, EffTime);
	}
	RecRescaleDrawRotaGraph(DrawX, 240 - PosY, 1, (double)Rot / 50.0, pic_cutin[2], TRUE);
	return;
}

static void RecCutDrawJacket(int EffTime) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;
	int PosY = 0;
	int Alpha = 0;

	if (s_cutIoFg == CUT_FRAG_OUT) {
		PosY = pals(0, 0, 500, 360, EffTime);
		Alpha = lins(500, 0, 0, 255, EffTime);
	}
	else {
		PosY = pals(500, 0, 0, 360, EffTime);
		Alpha = lins(500, 255, 0, 0, EffTime);
	}

	drawX = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 200 - PosY);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 120 - PosY);
	drawX2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 440 + PosY);
	drawY2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 360 + PosY);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
	DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic_cutin[3], TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	return;
}

static void RecCutDrawTips(int EffTime, const TCHAR *str) {
	int DrawX = 0;
	int PosY = 0;

	if (s_cutIoFg == CUT_FRAG_OUT) {
		PosY = pals(0, 0, 500, 360, EffTime);
		DrawX = CUT_MES_POSX - PosY * 640 / 360;
	}
	else {
		PosY = pals(500, 0, 0, 360, EffTime);
		DrawX = CUT_MES_POSX + PosY * 640 / 360;
	}

	RecRescaleDrawString(DrawX, 430, str, COLOR_BLACK);
	return;
}

static void RecCutDrawSide(int EffTime) {
	int PosS = 0;
	if (s_cutIoFg == CUT_FRAG_OUT) {
		PosS = pals(150, 0, 500, 100, betweens(0, EffTime, 500));
	}
	else {
		PosS = pals(350, 0, 0, 100, betweens(0, EffTime, 500));
	}
	RecRescaleDrawGraph(0 - PosS, 0, pic_cutin[4], TRUE);
	RecRescaleDrawGraph(590 + PosS, 0, pic_cutin[4], TRUE);
	return;
}

void ViewCutIn(int Stime) {
	int Ntime = GetNowCount();
	int EffTime = mins_2(Ntime - Stime, 500);
	int PosY = pals(500, 0, 0, 360, EffTime);
	int Rot = pals(500, 0, 0, -300, EffTime);
	int Alpha = lins(500, 255, 0, 0, EffTime);
	s_cutIoFg = CUT_FRAG_IN;
	RecRescaleDrawGraph(0, 0 - PosY, pic_cutin[0], TRUE);
	RecRescaleDrawGraph(0, 240 + PosY, pic_cutin[1], TRUE);
	RecRescaleDrawRotaGraph(320 - PosY, 240 - PosY, 1,
		(double)Rot / 50.0, pic_cutin[2], TRUE);
	switch (CutFg) {
	case CUTIN_TIPS_ON:
		RecRescaleDrawString(CUT_MES_POSX + PosY * 640 / 360, 430, tip[TipNo], COLOR_BLACK);
		break;
	case CUTIN_TIPS_SONG:
		{
			int drawX = 0;
			int drawY = 0;
			int drawX2 = 0;
			int drawY2 = 0;
			drawX = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 200 - PosY) + 160;
			drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 120 - PosY);
			drawX2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 440 + PosY) + 160;
			drawY2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 360 + PosY);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
			DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic_cutin[3], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			RecRescaleDrawString(CUT_MES_POSX + PosY * 640 / 360, 430, CutSongName, COLOR_BLACK);
		}
		break;
	default:
		/* none */
		break;
	}
	RecCutDrawSide(EffTime);

#if 0
	if (CutInSndLastPlayTime + 3000 < Ntime) {
		PlaySoundMem(snd_cutin[0], DX_PLAYTYPE_BACK);
		CutInSndLastPlayTime = Ntime;
	}
#endif
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
	int Alpha = lins(500, 0, 0, 255, EffTime);
	s_cutIoFg = CUT_FRAG_OUT;
	RecRescaleDrawGraph(0, 0 - PosY, pic_cutin[0], TRUE);
	RecRescaleDrawGraph(0, 240 + PosY, pic_cutin[1], TRUE);
	RecRescaleDrawRotaGraph(320 + PosY, 240 - PosY, 1,
		(double)Rot / 50.0, pic_cutin[2], TRUE);
	switch (CutFg) {
	case CUTIN_TIPS_ON:
		RecRescaleDrawString(CUT_MES_POSX - PosY * 640 / 360, 430, tip[TipNo], COLOR_BLACK);
		break;
	case CUTIN_TIPS_SONG:
		{
			int drawX = 0;
			int drawY = 0;
			int drawX2 = 0;
			int drawY2 = 0;
			drawX = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 200 - PosY) + 160;
			drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 120 - PosY);
			drawX2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 440 + PosY) + 160;
			drawY2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 360 + PosY);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
			DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic_cutin[3], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			RecRescaleDrawString(CUT_MES_POSX + PosY * 640 / 360, 430, CutSongName, COLOR_BLACK);
		}
		break;
	default:
		/* none */
		break;
	}
	RecCutDrawSide(EffTime);

#if 0
	if (CutOutSndLastPlayTime + 3000 < Ntime) {
		PlaySoundMem(snd_cutin[1], DX_PLAYTYPE_BACK);
		CutOutSndLastPlayTime = Ntime;
	}
#endif
	return;
}

void RecViewCut2() {
	if (s_cutIoFg == CUT_FRAG_OUT) { ViewCutOut(s_cutStime); }
	if (s_cutIoFg == CUT_FRAG_IN) { ViewCutIn(s_cutStime); }
	return;
}

void RecViewCut3() {
	int EffTime = mins_2(GetNowCount() - s_cutStime, 500);

	if (s_cutIoFg == CUT_FRAG_OUT && 500 <= EffTime) {
		return;
	}

	RecCutDrawShut(EffTime);
	RecCutDrawDisk(EffTime);
	if (CutFg == CUTIN_TIPS_SONG) {
		RecCutDrawJacket(EffTime);
		RecCutDrawTips(EffTime, CutSongName);
	}
	else {
		RecCutDrawTips(EffTime, tip[TipNo]);
	}
	RecCutDrawSide(EffTime);

	return;
}

void RecCutSetIo(int val) {
	s_cutIoFg = val;
	s_cutStime = GetNowCount();
	if (val == CUT_FRAG_IN) {
		PlaySoundMem(snd_cutin[0], DX_PLAYTYPE_BACK);
	}
	else {
		PlaySoundMem(snd_cutin[1], DX_PLAYTYPE_BACK);
	}
	return;
}

int RecCutIsClosing() {
	return s_cutIoFg;
}

int RecCutInEndAnim() {
	return (s_cutIoFg == 1 && s_cutStime + 2000 <= GetNowCount());
}

#endif /* action */

rec_cutin_c::rec_cutin_c() {
	DeleteGraph(pic_cutin[0]);
	DeleteGraph(pic_cutin[1]);
	DeleteGraph(pic_cutin[2]);
	DeleteGraph(pic_cutin[3]);
	DeleteGraph(pic_cutin[4]);
	DeleteGraph(snd_cutin[0]);
	DeleteGraph(snd_cutin[1]);
	pic_cutin[0] = LoadGraph(L"picture/cutin/cutinU.png");
	pic_cutin[1] = LoadGraph(L"picture/cutin/cutinD.png");
	pic_cutin[2] = LoadGraph(L"picture/cutin/cutinDisk.png");
	pic_cutin[3] = LoadGraph(SongJucketName);
	pic_cutin[4] = LoadGraph(L"picture/cutin/cutinS.png");
	snd_cutin[0] = LoadSoundMem(L"sound/IN.wav");
	snd_cutin[1] = LoadSoundMem(L"sound/OUT.wav");
	ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, snd_cutin[0]);
	ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, snd_cutin[1]);
}

rec_cutin_c::~rec_cutin_c() {
	DeleteGraph(pic_cutin[0]);
	DeleteGraph(pic_cutin[1]);
	DeleteGraph(pic_cutin[2]);
	DeleteGraph(pic_cutin[3]);
	DeleteGraph(pic_cutin[4]);
	DeleteGraph(snd_cutin[0]);
	DeleteGraph(snd_cutin[1]);
}

void rec_cutin_c::SetCutSong(TCHAR *songName, TCHAR *picName) {
	strcopy_2(songName, CutSongName, 255);
	strcopy_2(picName, SongJucketName, 255);
	pic_cutin[3] = LoadGraph(picName);
	return;
}

void rec_cutin_c::SetCutTipFg(cutin_tips_e Fg) {
	CutFg = Fg;
	return;
}

void rec_cutin_c::SetTipNo() {
	TipNo = (char)GetRand(tipNum - 1);
	return;
}

void rec_cutin_c::DrawCut() {
	if (s_cutIoFg == CUT_FRAG_OUT) { ViewCutOut(s_cutStime); }
	if (s_cutIoFg == CUT_FRAG_IN) { ViewCutIn(s_cutStime); }
	return;
}

void rec_cutin_c::SetIo(int val) {
	s_cutIoFg = val;
	s_cutStime = GetNowCount();
	if (val == CUT_FRAG_IN) {
		PlaySoundMem(snd_cutin[0], DX_PLAYTYPE_BACK);
	}
	else {
		PlaySoundMem(snd_cutin[1], DX_PLAYTYPE_BACK);
	}
	return;
}

int rec_cutin_c::IsClosing() {
	return s_cutIoFg;
}

int rec_cutin_c::IsEndAnim() {
	return (s_cutIoFg == 1 && s_cutStime + 2000 <= GetNowCount());
}
