
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <sancur.h>
#include <strcur.h>
#include <stdcur.h>

/* rec system include */
#include <option.h>
#include <RecWindowRescale.h>

/* own include */
#include <recr_cutin.h>

#define CUT_MES_POSX 75

static char TipNo = 0;
static dxcur_pic_c pic_cutin[5];
static int snd_cutin[2];
static int CutInSndLastPlayTime = 0;
static int CutOutSndLastPlayTime = 0;
static tstring SongJucketName = _T("NULL");
static tstring CutSongName    = _T("NULL");
static cutin_tips_e CutFg = CUTIN_TIPS_NONE;
static cutin_io_t s_cutIoFg = CUT_FRAG_OUT;
static int s_cutStime = 0;

/* TODO: 編集いるよ(1.5.5でもいいね) */
/* TODO: rec_system_langstr_cにする。 */
/* tipの最大文字数は30文字(NULL終端除く) */
static rec_system_langstr_c const tip[] = {
	/* レコランの世界観 */
	rec_system_langstr_c(_T("音楽は万物に影響を与える"), _T("Music affects everything")),
	rec_system_langstr_c(_T("音楽は人の心を動かす"), _T("Music moves people's hearts")),
	rec_system_langstr_c(_T("ランナーは、音楽犯罪を防止する役目もある"), _T("Runners also have the role of preventing music crimes")),
	rec_system_langstr_c(_T("ピッカーたちのいる地域は、比較的治安が良い"), _T("The area where Picker are located is relatively safe")),
	rec_system_langstr_c(_T("ゴールドランナーは全体の5%しかいない"), _T("Only 5% of all runners are Gold Runners")),
	rec_system_langstr_c(_T("技術とは、扱い次第で善にも悪にもなる"), _T("Technology can be good or bad depending on how it is handled")),
	/* ピッカーのセリフ */
	rec_system_langstr_c(_T("「分かってはいたけど、ランナーって結構動くなぁ」"), _T("I knew it, but runners move quite a bit")),
	rec_system_langstr_c(_T("「あぁ、風が気持ち良い…」"), _T("Ah, the wind feels good...")),
	rec_system_langstr_c(_T("「ゲーターさん、耳引っ張るのだけはやめて」"), _T("Gator, please don't pull my ears")),
	rec_system_langstr_c(_T("「テイラーさんって、どのくらいすごいんだろう…?」"), _T("I wonder how amazing Taylor is...?")),
	rec_system_langstr_c(_T("「お父さん、どこで何やってるんだろう…」"), _T("I wonder where Dad is and what he's doing...")),
	/* ゲーターのセリフ */
	rec_system_langstr_c(_T("「パフェ取るまで何度でもチャレンジだっ!」"), _T("I'll keep trying until I get the perfect!")),
	rec_system_langstr_c(_T("「んがぁ～…、目覚ましがうるせぇ…」"), _T("Nngaa~... the alarm clock is noisy...")),
	rec_system_langstr_c(_T("「ピッカーは新人だし、俺が色々教えなきゃな」"), _T("Picker is a rookie, so I have to teach him various things")),
	rec_system_langstr_c(_T("「テイラー、たまにはお前も働いてくれねーか?」"), _T("Taylor, can you work once in a while?")),
	rec_system_langstr_c(_T("「絶対あいつらに見返してやるんだ」"), _T("I will definitely get back at them")),
	/* テイラーのセリフ */
	rec_system_langstr_c(_T("「私はただの音楽好きのドラゴンさ♪」"), _T("I am just a music-loving dragon♪")),
	rec_system_langstr_c(_T("「きれいなものと可愛いものに目がなかったりする♪」"), _T("I have a weakness for beautiful and cute things♪")),
	rec_system_langstr_c(_T("「ピッカー君は色々と期待できる子だよ♪」"), _T("Picker is a child who can expect various things♪")),
	rec_system_langstr_c(_T("「ゲーター、ピッカー君にあまり意地悪しないでね♪」"), _T("Gator, don't be too mean to Picker♪")),
	rec_system_langstr_c(_T("「私だって、本当は働きたいんだよ」"), _T("I really want to work too")),
	/* ゲームの仕様 */
	rec_system_langstr_c(_T("HITノーツはキャラを動かさなくて良い"), _T("HIT notes don't require moving the character")),
	rec_system_langstr_c(_T("HITノーツは、同時押しOK、餡蜜OK"), _T("HIT notes can be pressed simultaneously, and anmitsu is OK")),
	rec_system_langstr_c(_T("CATCHノーツの判定は前後にたっぷりある"), _T("The judgment of CATCH notes is plenty before and after")),
	rec_system_langstr_c(_T("CATCHノーツは、上下キーをガチャガチャしても取れる"), _T("CATCH notes can be taken even if you press the up and down keys randomly")),
	rec_system_langstr_c(_T("BOMBノーツの判定は極端に狭い"), _T("The judgment of BOMB notes is extremely narrow")),
	rec_system_langstr_c(_T("色違いのノーツもあるけど、操作は変わらない"), _T("There are also notes of different colors, but the operation does not change")),
	rec_system_langstr_c(_T("レートとスコアは無関係"), _T("Rate and score are unrelated")),
	rec_system_langstr_c(_T("SAFE判定だとコンボが増えない"), _T("SAFE judgment does not increase the combo")),
	rec_system_langstr_c(_T("MISS判定でLIFEが減る"), _T("MISS judgment reduces LIFE")),
	rec_system_langstr_c(_T("LIFEが無い時にMISSを出すと、代わりにスコアが減る"), _T("When you get a MISS when you have no LIFE, your score will decrease instead")),
	rec_system_langstr_c(_T("一度LIFEが0になると、走行距離が増えなくなる"), _T("Once LIFE reaches 0, the running distance will no longer increase")),
	rec_system_langstr_c(_T("一度LIFEが0になるとクリア失敗"), _T("Once LIFE reaches 0, you fail to clear")),
	rec_system_langstr_c(_T("簡単な譜面にLv10とかやっても、レートは上がらないよ"), _T("Even if you do Lv10 on an easy music, your rate will not go up")),
	rec_system_langstr_c(_T("上下同時押ししてるときは、キャラは真ん中に来る"), _T("When pressing up and down at the same time, the character comes to the center")),
	rec_system_langstr_c(_T("両手をZXCキーに置くのも手の内"), _T("Placing both hands on the ZXC keys is also an option")),
	rec_system_langstr_c(_T("両手を十字操作キーに置くのも手の内"), _T("Placing both hands on the cross operation keys is also an option")),
	rec_system_langstr_c(_T("全押ししても良いけど、巻き込み注意ね"), _T("You can press all the keys, but be careful of entanglement")),
	/* ノーツレーダーのこと */
	rec_system_langstr_c(_T("ノーツレーダーもどきは、まだ完全ではない"), _T("The note radar is not yet complete")),
	rec_system_langstr_c(_T("NOTESはノーツ密度のこと、catchとbombは無視"), _T("NOTES is the note density, ignoring catch and bomb")),
	rec_system_langstr_c(_T("ARROWはアロー密度のこと"), _T("ARROW is the arrow density")),
	rec_system_langstr_c(_T("CHORDは同時押し密度のこと"), _T("CHORD is the simultaneous pressing density")),
	rec_system_langstr_c(_T("CHAINは縦連密度のこと、HIT連打はトリル扱い"), _T("CHAIN is the vertical connection density, HIT repetition is treated as trill")),
	rec_system_langstr_c(_T("TRILLはトリル密度のこと、ARROWノーツも対象"), _T("TRILL is the trill density, ARROW notes are also targeted")),
	rec_system_langstr_c(_T("MELDYは乱打密度のこと"), _T("MELDY is the density of random hits")),
	rec_system_langstr_c(_T("ACTORはCATCH/BOMB密度のこと、複雑なほど増える"), _T("ACTOR is the density of CATCH/BOMB, the more complex it is, the more it increases")),
	rec_system_langstr_c(_T("TRICKはリズム難密度のこと"), _T("TRICK is the density of rhythm difficulty")),
	/* オプション周り */
	rec_system_langstr_c(_T("オプションで、プレイするキャラクターを変えれるよ"), _T("You can change the character you play in the options")),
	rec_system_langstr_c(_T("ノーツと音楽が合ってないときは、オプションで調節しよう"), _T("If the notes and music don't match, adjust them in the options")),
	rec_system_langstr_c(_T("プレイ中の効果音は、オプションで消せるよ"), _T("You can turn off sound effects during play in the options")),
	rec_system_langstr_c(_T("オプションで、プレイ中の背景を消せるよ"), _T("You can turn off the background during play in the options")),
	rec_system_langstr_c(_T("オプションで、プレイ中にキー押し状態を表示できるよ"), _T("You can display the key press status during play in the options")),
	rec_system_langstr_c(_T("オプションで、プレイ中の判定の位置を変えれるよ"), _T("You can change the position of the judgment during play in the options")),
	rec_system_langstr_c(_T("オプションで、曲の音量を変えれるよ"), _T("You can change the volume of the music in the options")),
	rec_system_langstr_c(_T("オプションで、効果音の音量を変えれるよ"), _T("You can change the volume of sound effects in the options")),
	/* レコランの雑話 */
	rec_system_langstr_c(_T("Record Runner の元ネタは、ビブリボン"), _T("The origin of Record Runner is Viv-riboon")),
	rec_system_langstr_c(_T("オートプレイは、まだ完全ではない"), _T("Auto play is not yet complete")),
	rec_system_langstr_c(_T("ARROWの3種類同時押しは禁止されている"), _T("Pressing three types of ARROW at the same time is prohibited")),
	rec_system_langstr_c(_T("Muse Dash? いや…、当時そのゲーム知らなかったし…"), _T("Muse Dash? Ah... I didn't know that game at the time...")),
	rec_system_langstr_c(_T("長押しノーツ? 追加しないよそんなもん"), _T("Long press notes? I won't add that kind of thing")),
	rec_system_langstr_c(_T("まだまだ追加したい機能はあるんだ"), _T("There are still features I want to add")),
	rec_system_langstr_c(_T("譜面のレギュレーションは一応ある"), _T("There are regulations for the score, for the time being")),
	rec_system_langstr_c(_T("昔のレコランのソースコード、マジでひどかったよw"), _T("The source code of the old Record Runner was really terrible lol")),
	rec_system_langstr_c(_T("昔のレコランはARROWノーツの巻き込みがひどかったw"), _T("The old Record Runner had terrible entanglement of ARROW notes lol")),
	rec_system_langstr_c(_T("HITノーツはキャラ位置も合わせる時期もあった"), _T("There was a time when HIT notes also matched the character position")),
	rec_system_langstr_c(_T("古い譜面のリメイクもたまにやってるよ"), _T("I sometimes remake old scores")),
	rec_system_langstr_c(_T("難易度表記に文句あり? 改善案はあるから待ってて"), _T("Do you have any complaints about the difficulty notation? There are improvements, so wait")),
	rec_system_langstr_c(_T("レコランの元ネタの1割はグルコス"), _T("A bit of the origin of Record Runner is Groove Coaster")),
	rec_system_langstr_c(_T("ストーリー更新しろって? 待ってよ、今いいとこだから"), _T("Update the story? Wait, it's a good place now")),
	/* カービンのこと */
	rec_system_langstr_c(_T("カービンはケモナー"), _T("Curbine is a furry")),
	rec_system_langstr_c(_T("カービンのtwitterもよろしくね"), _T("Please follow Curbine's twitter too")),
	rec_system_langstr_c(_T("カービンの英語表記は、curbine"), _T("I'm curbine, the game developer of Record Runner")),
	rec_system_langstr_c(_T("カービンのこともっと知りたいならホームページに"), _T("If you want to know more about Curbine, please visit to the my homepage")),
	rec_system_langstr_c(_T("作曲、イラスト、プログラム、全部やってるのがカービン"), _T("Curbine does everything, including composition, illustration, and programming")),
};
static char const tipNum = ARRAY_COUNT(tip);

#if 1 /* action */

static void RecCutDrawShut(int EffTime) {
	int PosY = 0;
	if (s_cutIoFg == CUT_FRAG_OUT) {
		PosY = pals(0, 0, 500, 360, EffTime);
	}
	else {
		PosY = pals(500, 0, 0, 360, EffTime);
	}
	RecRescaleDrawGraph(0, 0 - PosY, pic_cutin[0].handle(), TRUE);
	RecRescaleDrawGraph(0, 240 + PosY, pic_cutin[1].handle(), TRUE);
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
	RecRescaleDrawRotaGraph(DrawX, 240 - PosY, 1, (double)Rot / 50.0, pic_cutin[2].handle(), TRUE);
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
	DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic_cutin[3].handle(), TRUE);
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
	RecRescaleDrawGraph(0 - PosS, 0, pic_cutin[4].handle(), TRUE);
	RecRescaleDrawGraph(590 + PosS, 0, pic_cutin[4].handle(), TRUE);
	return;
}

static void ViewCutIn(int Stime) {
	int Ntime = GetNowCount();
	int EffTime = mins_2(Ntime - Stime, 500);
	int PosY = pals(500, 0, 0, 360, EffTime);
	int Rot = pals(500, 0, 0, -300, EffTime);
	int Alpha = lins(500, 255, 0, 0, EffTime);
	s_cutIoFg = CUT_FRAG_IN;
	RecRescaleDrawGraph(0, 0 - PosY, pic_cutin[0].handle(), TRUE);
	RecRescaleDrawGraph(0, 240 + PosY, pic_cutin[1].handle(), TRUE);
	RecRescaleDrawRotaGraph(320 - PosY, 240 - PosY, 1,
		(double)Rot / 50.0, pic_cutin[2].handle(), TRUE);
	switch (CutFg) {
	case CUTIN_TIPS_ON:
		RecRescaleDrawString(CUT_MES_POSX + PosY * 640 / 360, 430, tip[TipNo].get_str().c_str(), COLOR_BLACK);
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
			DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic_cutin[3].handle(), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			RecRescaleDrawString(CUT_MES_POSX + PosY * 640 / 360, 430, CutSongName.c_str(), COLOR_BLACK);
		}
		break;
	default:
		/* none */
		break;
	}
	RecCutDrawSide(EffTime);
	return;
}

static void ViewCutOut(int Stime) {
	int Ntime = GetNowCount();
	int EffTime = Ntime - Stime;
	if (500 < EffTime) {
		return;
	}
	int PosY = pals(0, 0, 500, 360, EffTime);
	int Rot = pals(0, 0, 500, 300, EffTime);
	int Alpha = lins(500, 0, 0, 255, EffTime);
	s_cutIoFg = CUT_FRAG_OUT;
	RecRescaleDrawGraph(0, 0 - PosY, pic_cutin[0].handle(), TRUE);
	RecRescaleDrawGraph(0, 240 + PosY, pic_cutin[1].handle(), TRUE);
	RecRescaleDrawRotaGraph(320 + PosY, 240 - PosY, 1,
		(double)Rot / 50.0, pic_cutin[2].handle(), TRUE);
	switch (CutFg) {
	case CUTIN_TIPS_ON:
		RecRescaleDrawString(CUT_MES_POSX - PosY * 640 / 360, 430, tip[TipNo].get_str().c_str(), COLOR_BLACK);
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
			DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic_cutin[3].handle(), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			RecRescaleDrawString(CUT_MES_POSX + PosY * 640 / 360, 430, CutSongName.c_str(), COLOR_BLACK);
		}
		break;
	default:
		/* none */
		break;
	}
	RecCutDrawSide(EffTime);
	return;
}

#endif /* action */

rec_cutin_c::rec_cutin_c() {
	DeleteSoundMem(snd_cutin[0]);
	DeleteSoundMem(snd_cutin[1]);
	pic_cutin[0].reload(L"picture/cutin/cutinU.png");
	pic_cutin[1].reload(L"picture/cutin/cutinD.png");
	pic_cutin[2].reload(L"picture/cutin/cutinDisk.png");
	pic_cutin[3].reload(SongJucketName.c_str());
	pic_cutin[4].reload(L"picture/cutin/cutinS.png");
	snd_cutin[0] = LoadSoundMem(L"sound/IN.wav");
	snd_cutin[1] = LoadSoundMem(L"sound/OUT.wav");
	ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, snd_cutin[0]);
	ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, snd_cutin[1]);
}

rec_cutin_c::~rec_cutin_c() {
	DeleteSoundMem(snd_cutin[0]);
	DeleteSoundMem(snd_cutin[1]);
}

void rec_cutin_c::SetCutSong(const tstring &songName, const tstring &picName) {
	CutSongName    = songName;
	SongJucketName = picName;
	pic_cutin[3].reload(SongJucketName.c_str());
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

void rec_cutin_c::DrawCut() const {
	if (s_cutIoFg == CUT_FRAG_OUT) { ViewCutOut(s_cutStime); }
	if (s_cutIoFg == CUT_FRAG_IN) { ViewCutIn(s_cutStime); }
	return;
}

void rec_cutin_c::SetIo(cutin_io_t val) {
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

int rec_cutin_c::IsClosing() const {
	return s_cutIoFg;
}

int rec_cutin_c::IsEndAnim() const {
	return (s_cutIoFg == 1 && s_cutStime + 2000 <= GetNowCount());
}
