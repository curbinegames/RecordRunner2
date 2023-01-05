#include "RecordLoad2.h"
#include "result.h"
#include "playbox.h"

#define CHARA_POS_UP 0
#define CHARA_POS_MID 1
#define CHARA_POS_DOWN 2
#define SE_HIT 1
#define SE_CATCH 2
#define SE_ARROW 4
#define SE_BOMB 8

#if 0
#define RECR_DEBUG(ofs, n, data_a, data_b)						\
	for (int _rep = 0; _rep < n; _rep++) {						\
		DrawFormatString(20, 120 + _rep * 20 + ofs * 20, Cr,	\
		L#data_a"[%d]"#data_b": %d", _rep, data_a[_rep]data_b);	\
	}
#else
#define RECR_DEBUG(n, data_a, data_b)
#endif

typedef enum note_judge {
	NOTE_JUDGE_JUST = 0,
	NOTE_JUDGE_GOOD,
	NOTE_JUDGE_SAFE,
	NOTE_JUDGE_MISS,
} note_judge;

struct judge_box AddHitJudge(struct judge_box ans, int gup);
void cal_back_x(int *xpos, double Gspeed, double Wspeed, double scrool,
	int cam);
int cal_nowdif_p(int *ddif, int Ntime, int noteoff, int Etime);
int CheckNearHitNote(int un, int mn, int dn, int ut, int mt, int dt);
int GetCharaPos(int time, struct note_box highnote, struct note_box midnote,
	struct note_box lownote, int keyu, int keyd, int keyl, int keyr, int hitatp,
	int hitatt);
int GetHighScore(wchar_t pas[255], int dif);
int GetRemainNotes2(struct judge_box judge, int Notes);
struct score_box GetScore3(struct score_box score, struct judge_box judge,
	const int notes, const int MaxCombo);
void Getxxxpng(wchar_t *str, int num);
void Getxxxwav(wchar_t *str, int num);
void note_judge_event(note_judge judge, note_material note, int *viewjudge,
	int *judgename, int *combo, int *life, int *Dscore);
int CalPosScore2(struct score_box score, int RemainNotes, int Notes, int combo,
	int MaxCombo);
void ShowCombo(int combo, int *pic);
void ShowBonusEff(struct judge_box judge, int EffStartTime, int *Snd, int *pic,
	int filter, int biglight, int *smalllight, int flash, int ring);
void ShowJudge(const int *viewjudge, const int *judgeimg, const int posX,
	const int posY);
void ShowScore2(struct score_box score, int Hscore, int time);
void RunningStats2(struct judge_box judge, int PosScore, int HighScore);
char PlayNoteHitSound(note_box note, int *MelodySnd, int *Sitem, char seflag,
	int notemat);

int play3(int p, int n, int o, int shift, int AutoFlag) {
	/*---用語定義-----
	ユーザー用譜面データ: ユーザーが作った譜面データ。ユーザーに分かりやすい。
	PC用譜面データ: ユーザー用譜面データから計算で作られた、PC専用の譜面データ。PCに分かりやすい。
	----------------*/
	//p: パックナンバー
	//n: 曲ナンバー
	//o: 難易度ナンバー
	short int i[3];
	short int Lv = 0;
	short int notes = 0;
	int bgp[3] = { 0,0,0 }; //[0:sky,1:ground,2:water]の横位置
	short int bgf[2] = { 0,0 }; //落ち物背景の[0:横位置,1:縦位置]
	short int charaput = 1; //キャラの今の位置[0で上,1で中,2で下]
	short int drop = 0;
	short int rank = 0;
	short int Clear = 0;
	short int KeyPushCount[7] = { 0,0,0,0,0,0,0 };
	int judgh = 0; //ノーツの時間距離
	int charahit = 0; //キャラがノーツをたたいた後であるかどうか。[1以上で叩いた、0で叩いてない]
	int G[20], songT;
	int system[7] = { 0,0,0,2,0,0,0 };
	int noteoff = 0; //ノーツのオフセット
	int Etime = 0; //譜面の終わりの時間
	int Ntime = 0;
	int judghname[3][3]{ 0,0,0,0,0,0,0,0,0 };
	play_key_stat_t key_stat;
	int holda = 0;
	int holdb = 0;
	int holdc = 0;
	int holdu = 0;
	int holdd = 0;
	int holdl = 0;
	int holdr = 0;
	int holdG = 0;
	int key2 = 1;
	int key3 = 1;
	int combo = 0;
	int AllNotesHitTime = -1;
	int LaneTrack[3] = { -150,-150,-150 };
	int Mcombo = 0;
	int Dscore[4] = { 0,0,0,0 }; //距離に当たる部分[加点用,加点保存用,距離保存用,実点数]
	int judghcount[4] = { 0,0,0,0 };
	int life = 500;
	int gap[30] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//gap = 判定ずれ
	int gapa[3] = { 0,0,0 };//gapa = 判定ずれ[合計, 個数, 2乗の合計]
	int StopFrag = -1;
	struct camera_box camera[255];
	short int cameraN = 0;
	int nowcamera[2] = { 320,240 };
	struct judge_box judge;
	struct score_box score;
	struct scrool_box scrool[99];
	int scroolN = 0;
	int HighSrore; //ハイスコア
	int viewjudge[4] = { 0,0,0,0 };
	int hitatk[2] = { 1,-1000 }; //0:位置, 1:時間
	int fps[62];//0〜59=1フレーム間隔の時間,60=次の代入先,61=前回の時間
	for (i[0] = 0; i[0] <= 59; i[0]++)fps[i[0]] = 17;
	fps[60] = 0;
	fps[61] = 0;
	double GD[5];
	int item[99]; //アイテムのアドレス、DrawGraphで呼べる。
	short int itemN = 0; //↑の番号
	int Sitem[99]; //サウンドアイテムのアドレス、DrawGraphで呼べる。
	short int SitemN = 0; //↑の番号
	int chamo[3][99][2]; //キャラの[0:上,1:中,2:下]の[0:グラフィック,1:切り替え時間]
	short int chamoN[3] = { 0,0,0 }; //↑の番号
	int fall[99][2]; //落ち物背景の[0:番号,1:時間]
	short int fallN = 0; //↑の番号
	int Ymove[5][999][4]; //[上, 中, 下, (地面), (水中)]レーン縦移動の[0:開始時間,1:位置,2:終了時間,3:種類]
	short int YmoveN[5] = { 0,0,0,0,0 }; //↑の番号
	short int YmoveN2[3] = { 0,0,0 };
	int Xmove[3][999][4]; //[上, 中, 下]レーン横移動の[0:開始時間,1:位置,2:終了時間,3:種類]
	short int XmoveN[3] = { 0,0,0 }; //↑の番号
	int lock[2][2][99]; //lock = [横,縦]の音符の位置を[(1=固定する,-1以外=固定しない),時間]
	short int lockN[2] = { 0,0 }; //↑の番号
	int carrow[2][99];
	short int carrowN = 0;
	int viewT[2][99];//[音符表示時間,実行時間,[0]=現ナンバー]
	short int viewTN = 0;
	item_box Movie[999];
	short int MovieN = 0;
	view_BPM_box v_bpm[100];
	unsigned int v_bpmN = 0;
	struct note_box note[3][2000];
	short int objectN[3] = { 0,0,0 }; //note number
	short int objectNG[3] = { 0,0,0 }; //note number without ghost note
	int difkey[50][4];//難易度計算に使う[番号][入力キー,時間,難易度点,[0]個数上限:[1]今の番号:[2]1個前の番号:[3]2個前の番号:[4]最高点:[5]データ個数:[6]最後50個の合計:[7]計算から除外する時間]
	int ddif[25] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//各区間の難易度
	int ddifG[2] = { 1,25 };//0=今いる区間番号(1〜25),1=最大値
	int Yline[5] = { 300,350,400,350,600 };//[上,中,下,地面,水中]レーンの縦位置
	int Xline[3] = { 150,150,150 };//[上,中,下]レーンの横位置
	int outpoint[2] = { -1,0 };
	double SumRate[2] = { 0,0 }, bpm = 120, bpmG = 120;
	double speedt[5][99][2]; //[上, 中, 下, (地面), (水中)]レーンの[0:切り替え時間,1:速度]
	double DifRate; //譜面定数
	short int speedN[5] = { 0,0,0,0,0 }; //↑の番号
	char key[256];
	char seflag = 0;//reserved, reserved, reserved, reserved, bomb, arrow, catch, hit
	wchar_t songN[255];
	wchar_t songNE[255];
	wchar_t fileN[255];
	wchar_t dataE[255] = L"record/";
	wchar_t mp3FN[255] = L"song/";
	wchar_t DataFN[255] = L"score/";
	wchar_t skyFN[255] = L"picture/backskynoamal.png";
	wchar_t groundFN[255] = L"picture/groundnaturenormal.png";
	wchar_t waterFN[255] = L"picture/waternormal.png";
	wchar_t DifFN[255] = L"picture/difanother.png";
	wchar_t GT1[255];
	wchar_t GT2[255];
	wchar_t GT3[] = L".png";
	wchar_t ST1[] = L"record/";
	wchar_t ST2[] = L"/list.txt";
	wchar_t ST3[] = L".dat";
	wchar_t GT26[6][7] = { L"/0.rrs" ,L"/1.rrs" ,L"/2.rrs" ,L"/3.rrs" ,L"/4.rrs" ,L"/5.rrs" };
	unsigned int Cr = GetColor(255, 255, 255);
	unsigned int Crb = GetColor(0, 0, 0);
	unsigned int CrR = GetColor(255, 0, 0);
	playnum_box allnum;
	FILE *fp;
	//システムロード
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&system, sizeof(int), 7, fp);
		fclose(fp);
	}
	songT = FileRead_open(L"RecordPack.txt");
	for (i[0] = 0; i[0] <= p; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcats(dataE, GT1);
	stradds(dataE, L'/');
	strcopy(ST1, GT2, 1);
	strcats(GT2, GT1);
	strcats(GT2, ST2);
	songT = FileRead_open(GT2);
	for (i[0] = 0; i[0] <= n; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcopy(GT1, fileN, 1);
	strcats(dataE, GT1);
	strcopy(dataE, GT1, 1);
	strcats(GT1, GT26[o]);
	G[2] = -1;
	if (shift == 0) {
		G[2] = _wfopen_s(&fp, GT1, L"rb");//rrsデータを読み込む
	}
	if (G[2] != 0) {
		RecordLoad2(p, n, o);//rrsデータが無い、または作成の指示があれば作る
		G[2] = _wfopen_s(&fp, GT1, L"rb");//rrsデータを読み込む
	}
	if (G[2] == 0) {
		fread(&allnum, sizeof(playnum_box), 1, fp);//各データの個数
		fread(&mp3FN, 255, 1, fp);//音楽ファイル名
		fread(&bpm, sizeof(double), 1, fp);//BPM
		fread(&noteoff, sizeof(int), 1, fp);//offset
		fread(&skyFN, 255, 1, fp);//空背景名
		fread(&groundFN, 255, 1, fp);//地面画像名
		fread(&waterFN, 255, 1, fp);//水中画像名
		fread(&songN, 255, 1, fp);//曲名
		fread(&songNE, 255, 1, fp);//曲名(英語)
		fread(&Lv, sizeof(short int), 1, fp);//レベル
		//fread(&item, sizeof(int), 99, fp);//アイテム画像データ(動作未確認)
		fread(&fall, sizeof(int), 198, fp);//落ち物背景切り替えタイミング
		fread(&speedt, sizeof(double), 990, fp);//レーン速度
		fread(&chamo, sizeof(int), 594, fp);//キャラグラ変換タイミング
		fread(&Ymove[0], sizeof(int), allnum.Ymovenum[0] * 4, fp);//上レーン縦位置移動タイミング
		fread(&Ymove[1], sizeof(int), allnum.Ymovenum[1] * 4, fp);//中レーン縦位置移動タイミング
		fread(&Ymove[2], sizeof(int), allnum.Ymovenum[2] * 4, fp);//下レーン縦位置移動タイミング
		fread(&Ymove[3], sizeof(int), allnum.Ymovenum[3] * 4, fp);//地面縦位置移動タイミング
		fread(&Ymove[4], sizeof(int), allnum.Ymovenum[4] * 4, fp);//水面縦位置移動タイミング
		fread(&Xmove[0], sizeof(int), allnum.Xmovenum[0] * 4, fp);//上レーン横位置移動タイミング
		fread(&Xmove[1], sizeof(int), allnum.Xmovenum[1] * 4, fp);//中レーン横位置移動タイミング
		fread(&Xmove[2], sizeof(int), allnum.Xmovenum[2] * 4, fp);//下レーン横位置移動タイミング
		fread(&lock, sizeof(int), 396, fp);//ノーツ固定切り替えタイミング
		fread(&carrow, sizeof(int), 198, fp);//キャラ向き切り替えタイミング
		fread(&viewT, sizeof(int), 198, fp);//ノーツ表示時間変換タイミング
		fread(&note[0], sizeof(struct note_box), allnum.notenum[0], fp); /* 上レーンノーツデータ */
		fread(&note[1], sizeof(struct note_box), allnum.notenum[1], fp); /* 中レーンノーツデータ */
		fread(&note[2], sizeof(struct note_box), allnum.notenum[2], fp); /* 下レーンノーツデータ */
		fread(&notes, sizeof(short int), 1, fp);//ノーツ数
		fread(&Etime, sizeof(int), 1, fp);//曲終了時間
		fread(&G, sizeof(int), 2, fp);
		difkey[4][3] = G[0];//最高難易度
		difkey[6][3] = G[1];//最終難易度
		fread(&ddif, sizeof(int), 25, fp);//各区間難易度データ
		fread(&ddifG, sizeof(int), 2, fp);//各区間難易度データ
		fread(&DifFN, 255, 1, fp);//難易度バー名
		fread(&Movie, sizeof(item_box), allnum.movienum, fp);//アイテムデータ
		fread(&camera, sizeof(struct camera_box), 255, fp);//カメラデータ
		fread(&scrool, sizeof(struct scrool_box), 99, fp);//スクロールデータ
		fread(&v_bpm, sizeof(view_BPM_box), allnum.v_BPMnum, fp);//見た目のBPMデータ
		fread(&outpoint, sizeof(int), 2, fp);//エラーデータ
	}
	fclose(fp);
	strcats(DataFN, fileN);
	strcats(DataFN, ST3);
	//ハイスコア取得
	HighSrore = GetHighScore(DataFN, o);
	//オルゴール音声
	int MelodySnd[24] = {
		LoadSoundMem(L"sound/melody/lowF.mp3"),
		LoadSoundMem(L"sound/melody/lowF#.mp3"),
		LoadSoundMem(L"sound/melody/lowG.mp3"),
		LoadSoundMem(L"sound/melody/lowG#.mp3"),
		LoadSoundMem(L"sound/melody/lowA.mp3"),
		LoadSoundMem(L"sound/melody/lowA#.mp3"),
		LoadSoundMem(L"sound/melody/lowB.mp3"),
		LoadSoundMem(L"sound/melody/lowC.mp3"),
		LoadSoundMem(L"sound/melody/lowC#.mp3"),
		LoadSoundMem(L"sound/melody/lowD.mp3"),
		LoadSoundMem(L"sound/melody/lowD#.mp3"),
		LoadSoundMem(L"sound/melody/lowE.mp3"),
		LoadSoundMem(L"sound/melody/highF.mp3"),
		LoadSoundMem(L"sound/melody/highF#.mp3"),
		LoadSoundMem(L"sound/melody/highG.mp3"),
		LoadSoundMem(L"sound/melody/highG#.mp3"),
		LoadSoundMem(L"sound/melody/highA.mp3"),
		LoadSoundMem(L"sound/melody/highA#.mp3"),
		LoadSoundMem(L"sound/melody/highB.mp3"),
		LoadSoundMem(L"sound/melody/highC.mp3"),
		LoadSoundMem(L"sound/melody/highC#.mp3"),
		LoadSoundMem(L"sound/melody/highD.mp3"),
		LoadSoundMem(L"sound/melody/highD#.mp3"),
		LoadSoundMem(L"sound/melody/highE.mp3")
	};
	//ボーナス演出素材
	int Bonusimg[3] = {
		LoadGraph(L"picture/PERFECT.png"),
		LoadGraph(L"picture/FULLCOMBO.png"),
		LoadGraph(L"picture/NOMISS.png")
	};
	int BonusSnd[3] = {
		LoadSoundMem(L"sound/a-perfect.mp3"),
		LoadSoundMem(L"sound/a-fullcombo.mp3"),
		LoadSoundMem(L"sound/a-nomiss.mp3")
	};
	int BigLightimg = LoadGraph(L"picture/Bonus-Biglight.png");
	int SmallLightimg[3] = {
		LoadGraph(L"picture/Bonus-Smalllight3.png"),
		LoadGraph(L"picture/Bonus-Smalllight2.png"),
		LoadGraph(L"picture/Bonus-Smalllight1.png")
	};
	int flashimg = LoadGraph(L"picture/White.png");
	int B_ringimg = LoadGraph(L"picture/Bonus-Ring.png");
	//その他グラフィックと効果音
	int judgeimg[4] = {
		LoadGraph(L"picture/judge-just.png"),
		LoadGraph(L"picture/judge-good.png"),
		LoadGraph(L"picture/judge-safe.png"),
		LoadGraph(L"picture/judge-miss.png")
	};
	int judghimg = LoadGraph(L"picture/Marker.png");
	int backskyimg = LoadGraph(skyFN);
	int backgroundimg = LoadGraph(groundFN);
	int backwaterimg = LoadGraph(waterFN);
	int dangerimg = LoadGraph(L"picture/danger.png");
	int dropimg = LoadGraph(L"picture/drop.png");
	int resultimg = LoadGraph(L"picture/result.png");
	int sbarimg = LoadGraph(L"picture/scoreber.png");
	int sbbarimg = LoadGraph(L"picture/scoreber2.png");
	int filterimg = LoadGraph(L"picture/Black.png");
	int charaguideimg = LoadGraph(L"picture/Cguide.png");
	int gapbarimg = LoadGraph(L"picture/GapBer.png");
	int gaplineimg = LoadGraph(L"picture/GapBerLine.png");
	int difberimg;
	int Lbarimg[3] = {
		LoadGraph(L"picture/LIFEbar.png"),
		LoadGraph(L"picture/LIFEbar2.png"),
		LoadGraph(L"picture/LIFEbar3.png")
	};
	int Tbarimg[2] = {
		LoadGraph(L"picture/TIMEbar.png"),
		LoadGraph(L"picture/TIMEbar2.png")
	};
	int	charaimg[30];
	int rankimg[6] = {
		LoadGraph(L"picture/rankEX.png"),
		LoadGraph(L"picture/rankS.png"),
		LoadGraph(L"picture/rankA.png"),
		LoadGraph(L"picture/rankB.png"),
		LoadGraph(L"picture/rankC.png"),
		LoadGraph(L"picture/rankD.png")
	};
	int coleimg[5] = {
		LoadGraph(L"picture/DROPED.png"),
		LoadGraph(L"picture/CLEARED.png"),
		LoadGraph(L"picture/NOMISS.png"),
		LoadGraph(L"picture/FULLCOMBO.png"),
		LoadGraph(L"picture/PERFECT.png")
	};
	int effimg[7][5];
	int KeyViewimg[2];
	int Rchaimg;
	int ComboFontimg[10];
	//ノーツの画像
	struct note_img noteimg;
	int musicmp3, attack, catchs, arrow, bomb;
	switch (o) {
	case 0:
		difberimg = LoadGraph(L"picture/difauto.png");
		break;
	case 1:
		difberimg = LoadGraph(L"picture/difeasy.png");
		break;
	case 2:
		difberimg = LoadGraph(L"picture/difnormal.png");
		break;
	case 3:
		difberimg = LoadGraph(L"picture/difhard.png");
		break;
	case 4:
	case 5:
		difberimg = LoadGraph(DifFN);
		break;
	}
	LoadDivGraph(L"picture/hiteff.png", 5, 5, 1, 50, 50, effimg[0]);
	LoadDivGraph(L"picture/hiteff.png", 5, 5, 1, 50, 50, effimg[1]);
	LoadDivGraph(L"picture/upeff.png", 5, 5, 1, 50, 50, effimg[2]);
	LoadDivGraph(L"picture/downeff.png", 5, 5, 1, 50, 50, effimg[3]);
	LoadDivGraph(L"picture/lefteff.png", 5, 5, 1, 50, 50, effimg[4]);
	LoadDivGraph(L"picture/righteff.png", 5, 5, 1, 50, 50, effimg[5]);
	LoadDivGraph(L"picture/bombeff.png", 5, 5, 1, 50, 50, effimg[6]);
	LoadDivGraph(L"picture/NumberComboBlue.png", 10, 5, 2, 80, 100, ComboFontimg);
	for (i[0] = 0; i[0] < 100; i[0]++) {
		strcopy(dataE, GT1, 1);
		stradds(GT1, L'/');
		Getxxxpng(&GT2[0], i[0]);
		strcats(GT1, GT2);
		item[i[0]] = LoadGraph(GT1);
		if (item[i[0]] == -1) { break; }
	}
	for (i[0] = 1; i[0] < 100; i[0]++) {
		strcopy(dataE, GT1, 1);
		stradds(GT1, L'/');
		Getxxxwav(&GT2[0], i[0]);
		strcats(GT1, GT2);
		Sitem[i[0] - 1] = LoadSoundMem(GT1);
		if (Sitem[i[0] - 1] == -1) { break; }
	}
	if (system[5]) {
		KeyViewimg[0] = LoadGraph(L"picture/KeyViewOff.png");
		KeyViewimg[1] = LoadGraph(L"picture/KeyViewOn.png");
	}
	switch (system[0]) {
	case 0:
		LoadDivGraph(L"picture/Picker.png", 24, 6, 4, 160, 160, charaimg);
		Rchaimg = LoadGraph(L"picture/RePicker.png");
		break;
	case 1:
		LoadDivGraph(L"picture/Gator.png", 24, 6, 4, 160, 160, charaimg);
		Rchaimg = LoadGraph(L"picture/ReGator.png");
		break;
	case 2:
		LoadDivGraph(L"picture/Taylor.png", 24, 6, 4, 160, 160, charaimg);
		Rchaimg = LoadGraph(L"picture/ReTaylor.png");
		break;
	}
	musicmp3 = LoadSoundMem(mp3FN);
	if (system[2] == 0) {
		attack = LoadSoundMem(L"sound/attack.mp3");
		catchs = LoadSoundMem(L"sound/catch.mp3");
		arrow = LoadSoundMem(L"sound/arrow.ogg");
		bomb = LoadSoundMem(L"sound/bomb.wav");
	}
	else {
		attack = LoadSoundMem(L"sound/non.wav");
		catchs = LoadSoundMem(L"sound/non.wav");
		arrow = LoadSoundMem(L"sound/non.wav");
		bomb = LoadSoundMem(L"sound/non.wav");
	}
	//ゲーム開始前の下準備
	notes = notzero(notes);
	GD[0] = difkey[4][3] / 100.0 - Lv;//mdifと難易度表記の差
	if (Lv == 0) { DifRate = 0; }
	else if (2 <= GD[0]) { DifRate = Lv + 0.9; }
	else if (0 <= GD[0] && GD[0] < 2) { DifRate = Lv + 0.45 * GD[0]; }
	else { DifRate = difkey[4][3] / 100.0; }
	PlaySoundMem(musicmp3, DX_PLAYTYPE_BACK);
	WaitTimer(10);
	int Stime = GetNowCount();
	//ゲーム開始
	while (1) {
		ClearDrawScreen();
		GetHitKeyStateAll(key);
		//ghost skip
		for (int j = 0; j < 3; j++) {
			objectNG[j] = mins(objectNG[j], objectN[j]);
			while (note[j][objectNG[j]].object == NOTE_GHOST &&
				note[j][objectNG[j]].object != NOTE_NONE) {
				objectNG[j]++;
			}
		}
		//見た目BPMの変化
		while (v_bpm[v_bpmN + 1].time <= Ntime && -1000 < v_bpm[v_bpmN + 1].time) {
			v_bpmN++;
		}
		//カメラ移動
		while (0 <= camera[cameraN].endtime && camera[cameraN].endtime < Ntime) {
			cameraN++;
		}
		if (camera[cameraN].starttime <= Ntime && Ntime <= camera[cameraN].endtime) {
			nowcamera[0] = (int)movecal(camera[cameraN].mode, camera[cameraN].starttime,
				camera[cameraN - 1].xpos, camera[cameraN].endtime, camera[cameraN].xpos, Ntime);
			nowcamera[1] = (int)movecal(camera[cameraN].mode, camera[cameraN].starttime,
				camera[cameraN - 1].ypos, camera[cameraN].endtime, camera[cameraN].ypos, Ntime);
		}
		else {
			nowcamera[0] = camera[cameraN - 1].xpos;
			nowcamera[1] = camera[cameraN - 1].ypos;
		}
		//背景表示
		while (0 <= scrool[scroolN + 1].starttime && scrool[scroolN + 1].starttime <= Ntime) {
			scroolN++;
		}
		if (system[3] != 0) {
			if (speedt[3][speedN[3] + 1][0] < Ntime && speedt[3][speedN[3] + 1][0] >= 0) {
				speedN[3]++;
			}
			cal_back_x(bgp, speedt[3][speedN[3]][1], speedt[4][speedN[4]][1],
				scrool[scroolN].speed, nowcamera[0]);
			//背景の縦位置計算
			for (i[0] = 3; i[0] <= 4; i[0]++) {
				if (Ntime >= Ymove[i[0]][YmoveN[i[0]]][0] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0]) {
					Yline[i[0]] = (int)movecal(Ymove[i[0]][YmoveN[i[0]]][3],
						Ymove[i[0]][YmoveN[i[0]]][0],
						Ymove[i[0]][YmoveN[i[0]] - 1][1],
						Ymove[i[0]][YmoveN[i[0]]][2],
						Ymove[i[0]][YmoveN[i[0]]][1], Ntime);
				}
				while (Ntime >= Ymove[i[0]][YmoveN[i[0]]][2] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0] ||
					Ymove[i[0]][YmoveN[i[0]]][3] == 4) {
					Yline[i[0]] = Ymove[i[0]][YmoveN[i[0]]++][1];
				}
			}
			//draw background picture
			G[0] = bgp[0] / 100;
			while (G[0] + nowcamera[0] / 5 < 70000) {
				DrawGraph(G[0] + nowcamera[0] / 5,
					Yline[3] / 5 - 160 + nowcamera[1] / 5,
					backskyimg, TRUE);
				G[0] += 640;
			}
			G[0] = bgp[1] / 100;
			while (G[0] + nowcamera[0] < 70000) {
				DrawGraph(G[0] + nowcamera[0],
					Yline[3] - 400 + nowcamera[1], backgroundimg,
					TRUE);
				DrawGraph(G[0] + nowcamera[0],
					Yline[4] - 400 + nowcamera[1], backwaterimg,
					TRUE);
				G[0] += 640;
			}
			//落ち物背景表示
			if (Ntime >= fall[fallN + 1][0] && fall[fallN + 1][0] >= 0) fallN++;
			if (fall[fallN][1] >= 0) {
				G[0] = bgf[0];//横
				G[1] = bgf[1] + Yline[3];//縦
				for (i[0] = 0; i[0] < 2; i[0]++) {
					for (i[1] = 0; i[1] < 3; i[1]++) {
						DrawGraph(G[0] + i[0] * 640, G[1] - i[1] * 480, item[fall[fallN][1]], TRUE);
					}
				}
				bgf[0] -= 5;
				bgf[1] += 2;
			}
			if (bgf[0] <= -640)bgf[0] += 640;
			if (bgf[1] >= 640)bgf[1] -= 480;
		}
		//フィルター表示
		switch (system[3]) {
		case 1:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 170);
			DrawGraph(0, 0, filterimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			break;
		case 2:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 85);
			DrawGraph(0, 0, filterimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			break;
		}
		//アイテム表示
		if (system[3] != 0) {
			while (Movie[MovieN].endtime < Ntime &&
				Movie[MovieN].endtime > -500) {
				MovieN++;
			}
			G[0] = 0;
			while (Movie[MovieN + G[0]].endtime > -500) {
				if (Movie[MovieN + G[0]].starttime > Ntime ||
					Movie[MovieN + G[0]].endtime < Ntime) {
					G[0]++;
					continue;
				}
				//base setting
				G[1] = (int)movecal(Movie[MovieN + G[0]].movemode,
					Movie[MovieN + G[0]].starttime,
					Movie[MovieN + G[0]].startalpha,
					Movie[MovieN + G[0]].endtime,
					Movie[MovieN + G[0]].endalpha, Ntime);
				G[2] = (int)movecal(Movie[MovieN + G[0]].movemode,
					Movie[MovieN + G[0]].starttime,
					Movie[MovieN + G[0]].startXpos,
					Movie[MovieN + G[0]].endtime,
					Movie[MovieN + G[0]].endXpos, Ntime) + nowcamera[0];
				G[3] = (int)movecal(Movie[MovieN + G[0]].movemode,
					Movie[MovieN + G[0]].starttime,
					Movie[MovieN + G[0]].startYpos,
					Movie[MovieN + G[0]].endtime,
					Movie[MovieN + G[0]].endYpos, Ntime) + nowcamera[1];
				G[4] = (int)movecal(Movie[MovieN + G[0]].movemode,
					Movie[MovieN + G[0]].starttime,
					Movie[MovieN + G[0]].startsize,
					Movie[MovieN + G[0]].endtime,
					Movie[MovieN + G[0]].endsize, Ntime);
				G[5] = G[4];
				G[6] = (int)movecal(Movie[MovieN + G[0]].movemode,
					Movie[MovieN + G[0]].starttime,
					Movie[MovieN + G[0]].startrot,
					Movie[MovieN + G[0]].endtime,
					Movie[MovieN + G[0]].endrot, Ntime);
				//material setting
				if (Movie[MovieN + G[0]].eff.lock == 1) {
					G[2] -= nowcamera[0];
				}
				if (Movie[MovieN + G[0]].eff.lock == 1) {
					G[3] -= 25 + nowcamera[1];
				}
				if (Movie[MovieN + G[0]].eff.bpm_alphr == 1) {
					G[1] = lins(0, G[1], 60000 / v_bpm[v_bpmN].BPM, 0,
						(Ntime - v_bpm[v_bpmN].time) % (60000 / v_bpm[v_bpmN].BPM));
				}
				if (Movie[MovieN + G[0]].eff.chara_alphr == 1) {
					G[1] = lins(320, G[1], 60, 0, betweens(60, abss(Xline[1], G[2]), 320));
				}
				if (Movie[MovieN + G[0]].eff.bpm_size == 1) {
					G[4] = pals(60000 / v_bpm[v_bpmN].BPM, G[4] / 2, 0, G[4],
						(Ntime - v_bpm[v_bpmN].time) % (60000 / v_bpm[v_bpmN].BPM));
				}
				if (Movie[MovieN + G[0]].eff.edge_size == 1) {
					G[4] = betweens(0, lins(540, G[4], 640, 0, G[2]), G[4]);
					G[4] = betweens(0, lins(100, G[4], 0, 0, G[2]), G[4]);
				}
				//drawing
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, G[1]);
				DrawDeformationPic(G[2], G[3], G[4] / 100.0, G[4] / 100.0, G[6],
					item[Movie[MovieN + G[0]].ID]);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				G[0]++;
			}
		}
		//キャラ位置ガイドの表示
		if (carrow[1][carrowN + 1] < Ntime && carrow[1][carrowN + 1] >= 0) {
			carrowN++;
		}
		if (carrow[0][carrowN] == 1) {
			DrawGraph(Xline[charaput] - 4 + nowcamera[0], Yline[charaput] - 4 + nowcamera[1],
				charaguideimg, TRUE);
		}
		else {
			DrawTurnGraph(Xline[charaput] - 56 + nowcamera[0], Yline[charaput] - 4 + nowcamera[1],
				charaguideimg, TRUE);
		}
		//判定マーカーの表示
		for (i[0] = 0; i[0] < 3; i[0]++) {
			//縦移動
			if (Ntime >= Ymove[i[0]][YmoveN[i[0]]][0] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0]) {
				Yline[i[0]] = (int)movecal(Ymove[i[0]][YmoveN[i[0]]][3], Ymove[i[0]][YmoveN[i[0]]][0],
					Ymove[i[0]][YmoveN[i[0]] - 1][1], Ymove[i[0]][YmoveN[i[0]]][2],
					Ymove[i[0]][YmoveN[i[0]]][1], Ntime);
			}
			while (Ntime >= Ymove[i[0]][YmoveN[i[0]]][2] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0] ||
				Ymove[i[0]][YmoveN[i[0]]][3] == 4) {
				Yline[i[0]] = Ymove[i[0]][YmoveN[i[0]]++][1];
			}
			//横移動
			if (Ntime >= Xmove[i[0]][XmoveN[i[0]]][0] && 0 <= Xmove[i[0]][XmoveN[i[0]]][0]) {
				Xline[i[0]] = (int)movecal(Xmove[i[0]][XmoveN[i[0]]][3], Xmove[i[0]][XmoveN[i[0]]][0],
					Xmove[i[0]][XmoveN[i[0]] - 1][1], Xmove[i[0]][XmoveN[i[0]]][2],
					Xmove[i[0]][XmoveN[i[0]]][1], Ntime);
			}
			while (Ntime >= Xmove[i[0]][XmoveN[i[0]]][2] && 0 <= Xmove[i[0]][XmoveN[i[0]]][0] ||
				Xmove[i[0]][XmoveN[i[0]]][3] == 4) {
				Xline[i[0]] = Xmove[i[0]][XmoveN[i[0]]++][1];
			}
			DrawGraph(Xline[i[0]] + nowcamera[0], Yline[i[0]] + nowcamera[1], judghimg, TRUE);
		}
		//キャラグラ変換
		for (i[0] = 0; i[0] < 3; i[0]++)while (Ntime >= chamo[i[0]][chamoN[i[0]] + 1][1] &&
			chamo[i[0]][chamoN[i[0]] + 1][1] >= 0) {
			chamoN[i[0]]++;
		}
		G[3] = 0;
		//ghost back
		for (i[0] = 0; i[0] < 3; i[0]++) {
			G[i[0]] = objectN[i[0]] - 1;
			while (note[i[0]][G[i[0]]].object == 8 && G[i[0]] >= 1) {
				G[i[0]]--;
			}
		}
		//get chara position
		charaput = GetCharaPos(Ntime, note[0][G[0]], note[1][G[1]], note[2][G[2]], holdu, holdd,
			holdl, holdr, hitatk[0], hitatk[1]);
		G[4] = Yline[charaput];
		//キャラグラフィックを表示
		if (GetNowCount() - charahit > 250) G[5] = 0;
		else G[5] = pals(250, 0, 0, 50, GetNowCount() - charahit);
		if (charahit > 0) {
			if (carrow[0][carrowN] == 1) {
				DrawGraph(Xline[charaput] + G[5] - 160 + nowcamera[0], G[4] - 75 + nowcamera[1],
					charaimg[maxs(mins((GetNowCount() - charahit) / 125 + 18, 18), 23)], TRUE);
			}
			else {
				DrawTurnGraph(Xline[charaput] - G[5] + 30 + nowcamera[0], G[4] - 75 + nowcamera[1],
					charaimg[maxs(mins((GetNowCount() - charahit) / 125 + 18, 18), 23)], TRUE);
			}
		}
		else {
			if (carrow[0][carrowN] == 1) {
				DrawGraph(Xline[charaput] - 160 + nowcamera[0], G[4] - 75 + nowcamera[1],
					charaimg[Ntime * v_bpm[v_bpmN].BPM / 20000 % 6 + chamo[charaput][chamoN[charaput]][0] * 6],
					TRUE);
			}
			else {
				DrawTurnGraph(Xline[0] + 30 + nowcamera[0], G[4] - 75 + nowcamera[1],
					charaimg[Ntime * v_bpm[v_bpmN].BPM / 20000 % 6 + chamo[charaput][chamoN[charaput]][0] * 6],
					TRUE);
			}
		}
		//キー設定
		if (AutoFlag == 0) {
			if (key[KEY_INPUT_Z] == 0) holda = 0;
			else if (key[KEY_INPUT_Z] == 1) holda++;
			if (key[KEY_INPUT_X] == 0) holdb = 0;
			else if (key[KEY_INPUT_X] == 1) holdb++;
			if (key[KEY_INPUT_C] == 0) holdc = 0;
			else if (key[KEY_INPUT_C] == 1) holdc++;
			if (key[KEY_INPUT_UP] == 0) holdu = 0;
			else if (key[KEY_INPUT_UP] == 1) holdu++;
			if (key[KEY_INPUT_LEFT] == 0) holdl = 0;
			else if (key[KEY_INPUT_LEFT] == 1) holdl++;
			if (key[KEY_INPUT_RIGHT] == 0) holdr = 0;
			else if (key[KEY_INPUT_RIGHT] == 1) holdr++;
			if (key[KEY_INPUT_DOWN] == 0) holdd = 0;
			else if (key[KEY_INPUT_DOWN] == 1) holdd++;
		}
		//オートプレイ用コード
		else if (AutoFlag == 1) {
			if (holda > 0) { holda++; }
			if (holdb > 0) { holdb++; }
			if (holdc > 0) { holdc++; }
			if (holdu > 0) { holdu++; }
			if (holdd > 0) { holdd++; }
			if (holdl > 0) { holdl++; }
			if (holdr > 0) { holdr++; }
			//縦連前ボタン離し
			if (note[0][objectNG[0]].object == 3 && note[0][objectNG[0]].hittime - Ntime <= 40 ||
				note[1][objectNG[1]].object == 3 && note[1][objectNG[1]].hittime - Ntime <= 40 ||
				note[2][objectNG[2]].object == 3 && note[2][objectNG[2]].hittime - Ntime <= 40) {
				holdu = 0;
			}
			if (note[0][objectNG[0]].object == 4 && note[0][objectNG[0]].hittime - Ntime <= 40 ||
				note[1][objectNG[1]].object == 4 && note[1][objectNG[1]].hittime - Ntime <= 40 ||
				note[2][objectNG[2]].object == 4 && note[2][objectNG[2]].hittime - Ntime <= 40) {
				holdd = 0;
			}
			if (note[0][objectNG[0]].object == 5 && note[0][objectNG[0]].hittime - Ntime <= 40 ||
				note[1][objectNG[1]].object == 5 && note[1][objectNG[1]].hittime - Ntime <= 40 ||
				note[2][objectNG[2]].object == 5 && note[2][objectNG[2]].hittime - Ntime <= 40) {
				holdl = 0;
			}
			if (note[0][objectNG[0]].object == 6 && note[0][objectNG[0]].hittime - Ntime <= 40 ||
				note[1][objectNG[1]].object == 6 && note[1][objectNG[1]].hittime - Ntime <= 40 ||
				note[2][objectNG[2]].object == 6 && note[2][objectNG[2]].hittime - Ntime <= 40) {
				holdr = 0;
			}
			//ヒットノーツ処理
			G[0] = 0;
			if (note[0][objectNG[0]].object == 1 && note[0][objectNG[0]].hittime - Ntime <= 8) {
				G[0]++;
			}
			if (note[1][objectNG[1]].object == 1 && note[1][objectNG[1]].hittime - Ntime <= 8) {
				G[0]++;
			}
			if (note[2][objectNG[2]].object == 1 && note[2][objectNG[2]].hittime - Ntime <= 8) {
				G[0]++;
			}
			if (note[0][objectNG[0] + 1].object == 1 && note[0][objectNG[0] + 1].hittime - Ntime <= 8) {
				G[0]++;
			}
			if (note[1][objectNG[1] + 1].object == 1 && note[1][objectNG[1] + 1].hittime - Ntime <= 8) {
				G[0]++;
			}
			if (note[2][objectNG[2] + 1].object == 1 && note[2][objectNG[2] + 1].hittime - Ntime <= 8) {
				G[0]++;
			}
			if (note[0][objectNG[0] + 2].object == 1 && note[0][objectNG[0] + 2].hittime - Ntime <= 8) {
				G[0]++;
			}
			if (note[1][objectNG[1] + 2].object == 1 && note[1][objectNG[1] + 2].hittime - Ntime <= 8) {
				G[0]++;
			}
			if (note[2][objectNG[2] + 2].object == 1 && note[2][objectNG[2] + 2].hittime - Ntime <= 8) {
				G[0]++;
			}
			if (G[0] == 1) {
				if (holdc == 0) {
					holdc = 1;
					holda = 0;
					holdb = 0;
				}
				else {
					holda = 1;
					holdc = 0;
					holdb = 0;
				}
			}
			else if (G[0] == 2) {
				if (holdc == 0) {
					holdc = 1;
					holdb = 1;
					holda = 0;
				}
				else {
					holda = 1;
					holdb = 1;
					holdc = 0;
				}
			}
			else if (G[0] >= 3) {
				holda = 1;
				holdb = 1;
				holdc = 1;
			}
			//左右アローノーツ処理
			G[0] = 0;
			if (note[0][objectNG[0]].object == 5 && note[0][objectNG[0]].hittime - Ntime <= 8 ||
				note[1][objectNG[1]].object == 5 && note[1][objectNG[1]].hittime - Ntime <= 8 ||
				note[2][objectNG[2]].object == 5 && note[2][objectNG[2]].hittime - Ntime <= 8) {
				if (G[0] == 0) {
					holdu = 0;
					holdd = 0;
					holdl = 0;
					holdr = 0;
				}
				G[0] = 1;
				holdl = 1;
			}
			if (note[0][objectNG[0]].object == 6 && note[0][objectNG[0]].hittime - Ntime <= 8 ||
				note[1][objectNG[1]].object == 6 && note[1][objectNG[1]].hittime - Ntime <= 8 ||
				note[2][objectNG[2]].object == 6 && note[2][objectNG[2]].hittime - Ntime <= 8) {
				if (G[0] == 0) {
					holdu = 0;
					holdd = 0;
					holdl = 0;
					holdr = 0;
				}
				G[0] = 1;
				holdr = 1;
			}
			//ボムノーツ処理
			if (note[0][objectNG[0]].object == 7 && note[0][objectNG[0]].hittime - Ntime <= 40) {
				holdu = 0;
			}
			if (note[2][objectNG[2]].object == 7 && note[2][objectNG[2]].hittime - Ntime <= 40) {
				holdd = 0;
			}
			if (note[1][objectNG[1]].object == 7 && note[1][objectNG[1]].hittime - Ntime <= 40) {
				if (note[0][objectNG[0]].object == 7 && note[0][objectNG[0]].hittime - Ntime <= 40 ||
					note[2][objectNG[2]].object == 2 && note[2][objectNG[2]].hittime - Ntime <= 40) {
					holdu = 0;
					holdd++;
				}
				else {
					holdu++;
					holdd = 0;
				}
			}
			//上下アローノーツ処理
			if (note[0][objectNG[0]].object == 3 && note[0][objectNG[0]].hittime - Ntime <= 8 ||
				note[1][objectNG[1]].object == 3 && note[1][objectNG[1]].hittime - Ntime <= 8 ||
				note[2][objectNG[2]].object == 3 && note[2][objectNG[2]].hittime - Ntime <= 8) {
				if (G[0] == 0) {
					holdu = 0;
					holdd = 0;
					holdl = 0;
					holdr = 0;
				}
				G[0] = 1;
				holdu = 1;
				if (note[0][objectNG[0]].object == 7 && note[0][objectNG[0]].hittime - Ntime <= 40) {
					holdd = 1;
				}
			}
			if (note[0][objectNG[0]].object == 4 && note[0][objectNG[0]].hittime - Ntime <= 8 ||
				note[1][objectNG[1]].object == 4 && note[1][objectNG[1]].hittime - Ntime <= 8 ||
				note[2][objectNG[2]].object == 4 && note[2][objectNG[2]].hittime - Ntime <= 8) {
				if (G[0] == 0) {
					holdu = 0;
					holdd = 0;
					holdl = 0;
					holdr = 0;
				}
				G[0] = 1;
				holdd = 1;
				if (note[2][objectNG[2]].hittime == 7 && note[2][objectNG[2]].hittime - Ntime <= 40) {
					holdu = 1;
				}
			}
			//キャッチノーツ処理
			if (note[0][objectNG[0]].object == 2 && note[0][objectNG[0]].hittime - Ntime <= 8) {
				holdu++;
				holdd = 0;
			}
			if (note[1][objectNG[1]].object == 2 && note[1][objectNG[1]].hittime - Ntime <= 8) {
				holdu = 0;
				holdd = 0;
			}
			if (note[2][objectNG[2]].object == 2 && note[2][objectNG[2]].hittime - Ntime <= 8) {
				holdu = 0;
				holdd++;
			}
			if (holdc > 10) { holdc = 0; }
			if (holda > 10) { holda = 0; }
			if (holdb > 10) { holdb = 0; }
			if (holdu > 10 &&
				note[0][objectNG[0]].object != 2 && note[1][objectNG[1]].object != 2 &&
				note[2][objectNG[2]].object != 2 && note[0][objectNG[0]].object != 7 &&
				note[1][objectNG[1]].object != 7 && note[2][objectNG[2]].object != 7) {
				holdu = 0;
			}
			if (holdd > 10 &&
				note[0][objectNG[0]].object != 2 && note[1][objectNG[1]].object != 2 &&
				note[2][objectNG[2]].object != 2 && note[0][objectNG[0]].object != 7 &&
				note[1][objectNG[1]].object != 7 && note[2][objectNG[2]].object != 7) {
				holdd = 0;
			}
			if (holdl > 10) { holdl = 0; }
			if (holdr > 10) { holdr = 0; }
		}
		//キー押しヒット解除
		if (1 == holdu || 1 == holdd || 1 == holdl || 1 == holdr || hitatk[1] + 750 < Ntime) {
			hitatk[1] = -1000;
		}
		if (key[KEY_INPUT_G] == 0) { holdG = 0; }
		else if (key[KEY_INPUT_G] == 1) { holdG++; }
		if (GetWindowUserCloseFlag(TRUE)) { return 5; }
		//キャッチ判定に使う数値を計算
		LaneTrack[charaput] = Ntime;
		if (holdu == 0 && holdd == 0 || holdu > 0 && holdd > 0) { LaneTrack[1] = Ntime; }
		else if (holdu > 0 && holdd == 0) { LaneTrack[0] = Ntime; }
		else if (holdu == 0 && holdd > 0) { LaneTrack[2] = Ntime; }
		if (LaneTrack[0] <= LaneTrack[2]) { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[0]); }
		else { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[2]); }
		//ヒット
		if (holda == 1 || holdb == 1 || holdc == 1) charahit = GetNowCount();
		if (charahit + 750 < GetNowCount()) charahit = 0;
		//コンボ表示
		ShowCombo(combo, ComboFontimg);
		//判定表示
		switch (system[6]) {
		case 0:
			ShowJudge(viewjudge, judgeimg, 270, 100);
			break;
		case 1:
			ShowJudge(viewjudge, judgeimg, 10, 100);
			break;
		case 2:
			ShowJudge(viewjudge, judgeimg, 530, 100);
			break;
		case 3:
			ShowJudge(viewjudge, judgeimg, 270, 260);
			break;
		case 4:
			ShowJudge(viewjudge, judgeimg, Xline[charaput] + nowcamera[0] - 120,
				Yline[charaput] + nowcamera[1] - 100);
			break;
		default:
			break;
		}
		//音符表示
		for (i[0] = 0; i[0] < 2; i[0]++) {
			if (Ntime >= lock[i[0]][1][lockN[i[0]] + 1] && lock[i[0]][1][lockN[i[0]] + 1] >= 0){
				lockN[i[0]]++;
			}
		}
		if (viewT[0][viewTN + 1] <= Ntime && viewT[0][viewTN + 1] >= 0) viewTN++;
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (speedt[i[0]][speedN[i[0]] + 1][0] <= Ntime &&
				speedt[i[0]][speedN[i[0]] + 1][0] >= 0) {
				speedN[i[0]]++;
			}
			G[0] = G[3] = G[4] = G[5] = 0;
			for (i[1] = objectN[i[0]]; note[i[0]][i[1]].hittime > 0; i[1]++) {
				//表示/非表示ナンバーを進める
				if (note[i[0]][i[1]].hittime >= viewT[0][viewTN + G[0] + 1] &&
					viewT[0][viewTN + G[0] + 1] >= 0) {
					G[0]++;
				}
				//非表示スキップ
				if (note[i[0]][i[1]].hittime - Ntime >= viewT[1][viewTN + G[0]]) {
					continue;
				}
				//3秒ブレーク
				if (note[i[0]][i[1]].hittime - Ntime >= 3000 && 3000 >= viewT[1][viewTN + G[0]]) {
					break;
				}
				//ノーツロックナンバーを進める
				if (note[i[0]][i[1]].hittime >= lock[0][1][lockN[0] + G[3] + 1] &&
					lock[0][1][lockN[0] + G[3] + 1] >= 0) {
					G[3]++;
				}
				if (note[i[0]][i[1]].hittime >= lock[1][1][lockN[1] + G[4] + 1] &&
					lock[1][1][lockN[1] + G[4] + 1] >= 0) {
					G[4]++;
				}
				//スピードナンバーを進める
				while (note[i[0]][i[1]].hittime >= speedt[i[0]][speedN[i[0]] + G[5] + 1][0] &&
					speedt[i[0]][speedN[i[0]] + G[5] + 1][0] >= 0) {
					G[5]++;
				}
				//縦位置
				if (lock[1][0][lockN[1] + G[4]] == 1) {
					G[2] = note[i[0]][i[1]].ypos;
				}
				else {
					G[2] = Yline[i[0]];
				}
				//横位置
				G[1] = (speedt[i[0]][speedN[i[0]] + G[5]][1] * 20 *
					(note[i[0]][i[1]].viewtime - 
					(scrool[scroolN].speed * Ntime + scrool[scroolN].basetime))
					+ 5000) / 50;
				G[1] += 50;
				if (lock[0][0][lockN[0] + G[3]] == 1) G[1] += note[i[0]][i[1]].xpos - 150;
				else G[1] += Xline[i[0]] - 150;
				//色
				G[3] = note[i[0]][i[1]].color;
				//カメラ補正
				G[1] += nowcamera[0];
				G[2] += nowcamera[1];
				switch (note[i[0]][i[1]].object) {
				case 1:
					DrawGraph(G[1], G[2], noteimg.notebase, TRUE);
					DrawGraph(G[1], G[2], noteimg.hitcircle[G[3]], TRUE);
					break;
				case 2:
					DrawGraph(G[1], G[2], noteimg.catchi, TRUE);
					break;
				case 3:
					DrawGraph(G[1], G[2], noteimg.notebase, TRUE);
					DrawGraph(G[1], G[2], noteimg.up, TRUE);
					break;
				case 4:
					DrawGraph(G[1], G[2], noteimg.notebase, TRUE);
					DrawGraph(G[1], G[2], noteimg.down, TRUE);
					break;
				case 5:
					DrawGraph(G[1], G[2], noteimg.notebase, TRUE);
					DrawGraph(G[1], G[2], noteimg.left, TRUE);
					break;
				case 6:
					DrawGraph(G[1], G[2], noteimg.notebase, TRUE);
					DrawGraph(G[1], G[2], noteimg.right, TRUE);
					break;
				case 7:
					DrawGraph(G[1], G[2], noteimg.bomb, TRUE);
					break;
				case 8:
					DrawGraph(G[1], G[2], noteimg.goust, TRUE);
					break;
				}
			}
		}
		//判定、ヒットノーツ
		G[0] = 0;//押したボタンの数
		if (holda == 1) { G[0]++; }
		if (holdb == 1) { G[0]++; }
		if (holdc == 1) { G[0]++; }
		for (i[0] = 0; i[0] < G[0]; i[0]++) {
			if (i[0] > 0) {
				hitatk[0] = 1; /* これが誤爆かましてる? */
				hitatk[1] = Ntime;
			}
			G[1] = CheckNearHitNote(note[0][objectN[0]].object, note[1][objectN[1]].object,
				note[2][objectN[2]].object, note[0][objectN[0]].hittime - Ntime,
				note[1][objectN[1]].hittime - Ntime, note[2][objectN[2]].hittime - Ntime);
			if (G[1] == -1) { break; }
			G[2] = note[G[1]][objectN[G[1]]].hittime - Ntime;
			gap[gapa[1] % 30] = G[2];
			gapa[0] += G[2];
			gapa[2] += G[2] * G[2];
			gapa[1]++;
			if (i[0] == 0) {
				hitatk[0] = G[1];
				hitatk[1] = Ntime;
			}
			judge = AddHitJudge(judge, G[2]);
			//just
			if (G[2] <= 40 && G[2] >= -40) {
#if 1
				note_judge_event(NOTE_JUDGE_JUST, NOTE_HIT, &viewjudge[0],
					&judghname[G[1]][0], &combo, &life, &Dscore[0]);
#else
				viewjudge[0] = GetNowCount();
				judghname[G[1]][0] = 1;
				judghname[G[1]][1] = GetNowCount();
				judghname[G[1]][2] = 1;
				combo++;
				life += 2;
				Dscore[0] += 3;
#endif
				seflag = PlayNoteHitSound(note[G[1]][objectN[G[1]]], MelodySnd,
					Sitem, seflag, SE_HIT);
				score.before = pals(500, score.sum, 0, score.before,
					maxs(Ntime - score.time, 500));
				score.time = Ntime;
			}
			//good
			else if (G[2] <= 70 && G[2] >= -70) {
#if 1
				note_judge_event(NOTE_JUDGE_GOOD, NOTE_HIT, &viewjudge[0],
					&judghname[G[1]][0], &combo, &life, &Dscore[0]);
#else
				viewjudge[1] = GetNowCount();
				judghname[G[1]][0] = 2;
				judghname[G[1]][1] = GetNowCount();
				judghname[G[1]][2] = 1;
				combo++;
				life++;
				Dscore[0] += 2;
#endif
				seflag = PlayNoteHitSound(note[G[1]][objectN[G[1]]],
					MelodySnd, Sitem, seflag, SE_HIT);
				score.before = pals(500, score.sum, 0, score.before,
					maxs(Ntime - score.time, 500));
				score.time = Ntime;
			}
			//safe
			else if (G[2] <= 100 && G[2] >= -100) {
#if 1
				note_judge_event(NOTE_JUDGE_SAFE, NOTE_HIT, &viewjudge[0],
					&judghname[G[1]][0], &combo, &life, &Dscore[0]);
#else
				viewjudge[2] = GetNowCount();
				judghname[G[1]][0] = 3;
				judghname[G[1]][1] = GetNowCount();
				judghname[G[1]][2] = 1;
				Dscore[0]++;
#endif
				seflag = PlayNoteHitSound(note[G[1]][objectN[G[1]]],
					MelodySnd, Sitem, seflag, SE_HIT);
				score.before = pals(500, score.sum, 0, score.before,
					maxs(Ntime - score.time, 500));
				score.time = Ntime;
			}
			//fastmiss
			else if (G[2] <= 125) {
#if 1
				note_judge_event(NOTE_JUDGE_MISS, NOTE_HIT, &viewjudge[0],
					&judghname[G[1]][0], &combo, &life, &Dscore[0]);
#else
				viewjudge[3] = GetNowCount();
				judghname[G[1]][0] = 4;
				judghname[G[1]][1] = GetNowCount();
				judghname[G[1]][2] = 1;
				combo = 0;
				life -= 20;
#endif
			}
			objectN[G[1]]++;
		}
		for (i[0] = 0; i[0] < 3; i[0]++) {
			judgh = note[i[0]][objectN[i[0]]].hittime - Ntime;
			//キャッチノーツ(pjustのみ)
			if (LaneTrack[i[0]] + 100 >= note[i[0]][objectN[i[0]]].hittime &&
				note[i[0]][objectN[i[0]]].object == 2) {
				while (note[i[0]][objectN[i[0]]].hittime - Ntime <= 0) {
#if 1
					note_judge_event(NOTE_JUDGE_JUST, NOTE_CATCH, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[0] = GetNowCount();
					judghname[i[0]][0] = 1;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 2;
					combo++;
					life += 2;
					Dscore[0] += 3;
#endif
					seflag = PlayNoteHitSound(note[i[0]][objectN[i[0]]],
						MelodySnd, Sitem, seflag, SE_CATCH);
					score.before = pals(500, score.sum, 0, score.before,
						maxs(Ntime - score.time, 500));
					score.time = Ntime;
					charahit = 0;
					judge.pjust++;
					judge.just++;
					hitatk[1] = -1000;
					objectN[i[0]]++;
				}
			}
			//アローノーツ各種
			else if ((holdu == 1) && note[i[0]][objectN[i[0]]].object == 3 &&
				judgh <= 125 && judgh >= -100) {
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				judge = AddHitJudge(judge, judgh);
				//just
				if (judgh <= 40 && judgh >= -40) {
#if 1
					note_judge_event(NOTE_JUDGE_JUST, NOTE_UP, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[0] = GetNowCount();
					judghname[i[0]][0] = 1;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 3;
					combo++;
					life += 2;
					Dscore[0] += 3;
#endif
					seflag = PlayNoteHitSound(note[i[0]][objectN[i[0]]],
						MelodySnd, Sitem, seflag, SE_ARROW);
					score.before = pals(500, score.sum, 0, score.before,
						maxs(Ntime - score.time, 500));
					score.time = Ntime;
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
#if 1
					note_judge_event(NOTE_JUDGE_GOOD, NOTE_UP, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[1] = GetNowCount();
					judghname[i[0]][0] = 2;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 3;
					combo++;
					life++;
					Dscore[0] += 2;
#endif
					seflag = PlayNoteHitSound(note[i[0]][objectN[i[0]]],
						MelodySnd, Sitem, seflag, SE_ARROW);
					score.before = pals(500, score.sum, 0, score.before,
						maxs(Ntime - score.time, 500));
					score.time = Ntime;
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
#if 1
					note_judge_event(NOTE_JUDGE_SAFE, NOTE_UP, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[2] = GetNowCount();
					judghname[i[0]][0] = 3;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 3;
					Dscore[0]++;
#endif
					seflag = PlayNoteHitSound(note[i[0]][objectN[i[0]]],
						MelodySnd, Sitem, seflag, SE_ARROW);
					score.before = pals(500, score.sum, 0, score.before,
						maxs(Ntime - score.time, 500));
					score.time = Ntime;
				}
				//fastmiss
				else if (judgh <= 125) {
#if 1
					note_judge_event(NOTE_JUDGE_MISS, NOTE_UP, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[3] = GetNowCount();
					judghname[i[0]][0] = 4;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 3;
					combo = 0;
					life -= 20;
#endif
				}
				objectN[i[0]]++;
			}
			else if ((holdd == 1) && note[i[0]][objectN[i[0]]].object == 4 &&
				judgh <= 125 && judgh >= -100) {
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				judge = AddHitJudge(judge, judgh);
				//just
				if (judgh <= 40 && judgh >= -40) {
#if 1
					note_judge_event(NOTE_JUDGE_JUST, NOTE_DOWN, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[0] = GetNowCount();
					judghname[i[0]][0] = 1;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 4;
					combo++;
					life += 2;
					Dscore[0] += 3;
#endif
					seflag = PlayNoteHitSound(note[i[0]][objectN[i[0]]],
						MelodySnd, Sitem, seflag, SE_ARROW);
					score.before = pals(500, score.sum, 0, score.before, maxs(Ntime - score.time, 500));
					score.time = Ntime;
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
#if 1
					note_judge_event(NOTE_JUDGE_GOOD, NOTE_DOWN, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[1] = GetNowCount();
					judghname[i[0]][0] = 2;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 4;
					combo++;
					life++;
					Dscore[0] += 2;
#endif
					seflag = PlayNoteHitSound(note[i[0]][objectN[i[0]]],
						MelodySnd, Sitem, seflag, SE_ARROW);
					score.before = pals(500, score.sum, 0, score.before,
						maxs(Ntime - score.time, 500));
					score.time = Ntime;
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
#if 1
					note_judge_event(NOTE_JUDGE_SAFE, NOTE_DOWN, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[2] = GetNowCount();
					judghname[i[0]][0] = 3;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 4;
					Dscore[0]++;
#endif
					seflag = PlayNoteHitSound(note[i[0]][objectN[i[0]]],
						MelodySnd, Sitem, seflag, SE_ARROW);
					score.before = pals(500, score.sum, 0, score.before,
						maxs(Ntime - score.time, 500));
					score.time = Ntime;
				}
				//fastmiss
				else if (judgh <= 125) {
#if 1
					note_judge_event(NOTE_JUDGE_MISS, NOTE_DOWN, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[3] = GetNowCount();
					judghname[i[0]][0] = 4;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 4;
					combo = 0;
					life -= 20;
#endif
				}
				objectN[i[0]]++;
			}
			else if ((holdl == 1) && note[i[0]][objectN[i[0]]].object == 5 &&
			judgh <= 125 && judgh >= -100) {
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				judge = AddHitJudge(judge, judgh);
				//just
				if (judgh <= 40 && judgh >= -40) {
#if 1
					note_judge_event(NOTE_JUDGE_JUST, NOTE_LEFT, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[0] = GetNowCount();
					judghname[i[0]][0] = 1;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 5;
					combo++;
					life += 2;
					Dscore[0] += 3;
#endif
					seflag = PlayNoteHitSound(note[i[0]][objectN[i[0]]],
						MelodySnd, Sitem, seflag, SE_ARROW);
					score.before = pals(500, score.sum, 0, score.before,
						maxs(Ntime - score.time, 500));
					score.time = Ntime;
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
#if 1
					note_judge_event(NOTE_JUDGE_GOOD, NOTE_LEFT, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[1] = GetNowCount();
					judghname[i[0]][0] = 2;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 5;
					combo++;
					life++;
					Dscore[0] += 2;
#endif
					seflag = PlayNoteHitSound(note[i[0]][objectN[i[0]]],
						MelodySnd, Sitem, seflag, SE_ARROW);
					score.before = pals(500, score.sum, 0, score.before,
						maxs(Ntime - score.time, 500));
					score.time = Ntime;
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
#if 1
					note_judge_event(NOTE_JUDGE_SAFE, NOTE_LEFT, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[2] = GetNowCount();
					judghname[i[0]][0] = 3;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 5;
					Dscore[0]++;
#endif
					seflag = PlayNoteHitSound(note[i[0]][objectN[i[0]]],
						MelodySnd, Sitem, seflag, SE_ARROW);
					score.before = pals(500, score.sum, 0, score.before,
						maxs(Ntime - score.time, 500));
					score.time = Ntime;
				}
				//fastmiss
				else if (judgh <= 125) {
#if 1
					note_judge_event(NOTE_JUDGE_MISS, NOTE_LEFT, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[3] = GetNowCount();
					judghname[i[0]][0] = 4;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 5;
					combo = 0;
					life -= 20;
#endif
				}
				objectN[i[0]]++;
			}
			else if ((holdr == 1) && note[i[0]][objectN[i[0]]].object == 6 &&
			judgh <= 125 && judgh >= -100) {
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				judge = AddHitJudge(judge, judgh);
				//just
				if (judgh <= 40 && judgh >= -40) {
#if 1
					note_judge_event(NOTE_JUDGE_JUST, NOTE_RIGHT, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[0] = GetNowCount();
					judghname[i[0]][0] = 1;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 6;
					combo++;
					life += 2;
					Dscore[0] += 3;
#endif
					seflag = PlayNoteHitSound(note[i[0]][objectN[i[0]]],
						MelodySnd, Sitem, seflag, SE_ARROW);
					score.before = pals(500, score.sum, 0, score.before,
						maxs(Ntime - score.time, 500));
					score.time = Ntime;
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
#if 1
					note_judge_event(NOTE_JUDGE_GOOD, NOTE_RIGHT, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[1] = GetNowCount();
					judghname[i[0]][0] = 2;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 6;
					combo++;
					life++;
					Dscore[0] += 2;
#endif
					seflag = PlayNoteHitSound(note[i[0]][objectN[i[0]]],
						MelodySnd, Sitem, seflag, SE_ARROW);
					score.before = pals(500, score.sum, 0, score.before,
						maxs(Ntime - score.time, 500));
					score.time = Ntime;
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
#if 1
					note_judge_event(NOTE_JUDGE_SAFE, NOTE_RIGHT, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[2] = GetNowCount();
					judghname[i[0]][0] = 3;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 6;
					Dscore[0]++;
#endif
					seflag = PlayNoteHitSound(note[i[0]][objectN[i[0]]],
						MelodySnd, Sitem, seflag, SE_ARROW);
					score.before = pals(500, score.sum, 0, score.before,
						maxs(Ntime - score.time, 500));
					score.time = Ntime;
				}
				//fastmiss
				else if (judgh <= 125) {
#if 1
					note_judge_event(NOTE_JUDGE_MISS, NOTE_RIGHT, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[3] = GetNowCount();
					judghname[i[0]][0] = 4;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 6;
					combo = 0;
					life -= 20;
#endif
				}
				objectN[i[0]]++;
			}
			//ボムノーツ
			else if (i[0] == charaput && note[i[0]][objectN[i[0]]].object == 7 &&
			judgh <= 0 && judgh >= -40) {
#if 1
				note_judge_event(NOTE_JUDGE_MISS, NOTE_BOMB, &viewjudge[0],
					&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
				viewjudge[3] = GetNowCount();
				judghname[i[0]][0] = 4;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 7;
				combo = 0;
				life -= 20;
#endif
				judge.miss++;
				seflag = PlayNoteHitSound(note[i[0]][objectN[i[0]]],
					MelodySnd, Sitem, seflag, SE_BOMB);
				objectN[i[0]]++;
			}
			else if (note[i[0]][objectN[i[0]]].object == 7) {
				while (note[i[0]][objectN[i[0]]].hittime - Ntime < -40) {
#if 1
					note_judge_event(NOTE_JUDGE_JUST, NOTE_BOMB, &viewjudge[0],
						&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
					viewjudge[0] = GetNowCount();
					judghname[i[0]][0] = 1;
					judghname[i[0]][1] = GetNowCount();
					judghname[i[0]][2] = 7;
					combo++;
					life += 2;
					Dscore[0] += 3;
#endif
					objectN[i[0]]++;
					judge.pjust++;
					judge.just++;
					score.before = pals(500, score.sum, 0, score.before,
						maxs(Ntime - score.time, 500));
					score.time = Ntime;
				}
			}
			//ゴーストノーツ
			else if (note[i[0]][objectN[i[0]]].object == 8 && judgh < 16) objectN[i[0]]++;
			//全ノーツslowmiss
			else while (judgh <= -100 && judgh >= -100000 &&
			note[i[0]][objectN[i[0]]].object >= 1 && note[i[0]][objectN[i[0]]].object <= 6) {
#if 1
				note_judge_event(NOTE_JUDGE_MISS, NOTE_HIT, &viewjudge[0],
					&judghname[i[0]][0], &combo, &life, &Dscore[0]);
#else
				viewjudge[3] = GetNowCount();
				judghname[i[0]][0] = 4;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 0;
				combo = 0;
				life -= 20;
#endif
				objectN[i[0]]++;
				judge.miss++;
				judgh = note[i[0]][objectN[i[0]]].hittime - Ntime;
			}
		}
		if ((seflag & SE_HIT) != 0) {
			PlaySoundMem(attack, DX_PLAYTYPE_BACK);
		}
		if ((seflag & SE_CATCH) != 0) {
			PlaySoundMem(catchs, DX_PLAYTYPE_BACK);
		}
		if ((seflag & SE_ARROW) != 0) {
			PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
		}
		if ((seflag & SE_BOMB) != 0) {
			PlaySoundMem(bomb, DX_PLAYTYPE_BACK);
		}
		seflag = 0;
		if (gapa[2] > 2140000000) {
			gapa[0] /= 2;
			gapa[1] /= 2;
			gapa[2] /= 2;
		}
		Mcombo = mins(Mcombo, combo);
		//ヒットエフェクト表示
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (judghname[i[0]][1] + 250 > GetNowCount() && judghname[i[0]][0] >= 1 &&
				judghname[i[0]][0] <= 3 && judghname[i[0]][2] >= 1 && judghname[i[0]][2] <= 6) {
				DrawGraph(Xline[i[0]] - 10 + nowcamera[0], Yline[i[0]] - 10 + nowcamera[1],
					effimg[judghname[i[0]][2] - 1][(GetNowCount() - judghname[i[0]][1] + 250) / 50 % 5],
					TRUE);
			}
			else if (judghname[i[0]][1] + 250 > GetNowCount() && judghname[i[0]][0] == 4 &&
				judghname[i[0]][2] == 7) {
				DrawGraph(Xline[i[0]] - 10 + nowcamera[0], Yline[i[0]] - 10 + nowcamera[1],
					effimg[6][(GetNowCount() - judghname[i[0]][1] + 250) / 50 % 5], TRUE);
			}
			if (judghname[i[0]][1] + 750 < GetNowCount()) {
				judghname[i[0]][0] = 0;
				judghname[i[0]][1] = 0;
				judghname[i[0]][2] = 0;
			}
		}
		//ライフが0未満の時、1毎に減点スコアを20増やす。
		if (life < 0) {
			score.loss = maxs(score.loss - life * 20, score.normal + score.combo);
			life = 0;
		}
		//スコアバー表示
		DrawGraph(0, 0, sbarimg, TRUE);
		//スコア表示
		score = GetScore3(score, judge, notes, Mcombo);
		ShowScore2(score, HighSrore, Ntime);
		//ライフ表示
		life = maxs(life, 500);
		if (life > 100) {
			DrawGraph((291 * life - 57000) / 500, 3, Lbarimg[0], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (-51 * life + 25500) / 80);
			DrawGraph((291 * life - 57000) / 500, 3, Lbarimg[1], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
		else DrawGraph((291 * life - 57000) / 500, 3, Lbarimg[2], TRUE);
		DrawFormatString(440, 10, Cr, L"%3d", life);
		//距離表示
		if (drop == 0) {
			Dscore[1] = Dscore[0];
			G[0] = mins(Ntime - noteoff, 0);
		}
		else if (drop) { G[0] = Dscore[2]; }
		if (G[0] > Etime - noteoff) { //CLEARED
			DrawGraph(155, 38, Tbarimg[1], TRUE);
			GD[0] = (Etime - noteoff) / 100000.0;
			DrawFormatString(180, 45, Crb, L"%.3fkm", GD[0] + Dscore[1] / 1000.0);
		}
		else { //PLAYING or DROPED
			DrawGraph((291 * G[0] - 136 * Etime + 136 * noteoff) / (Etime - noteoff), 38, Tbarimg[0], TRUE);
			GD[0] = G[0] / 100000.0;
			DrawFormatString(180, 45, Cr, L"%.3fkm", GD[0] + Dscore[1] / 1000.0);
		}
		Dscore[3] = (int)(GD[0] * 1000 + Dscore[1]);
		//スコアバー隠し表示
		DrawGraph(0, 0, sbbarimg, TRUE);
		//ランニングステータス表示
		G[0] = GetRemainNotes2(judge, notes);
		G[1] = CalPosScore2(score, G[0], notes, combo, Mcombo);
		RunningStats2(judge, G[1], HighSrore);
		//部分難易度表示 (only auto mode)
		if (holdG >= 1 && AutoFlag == 1) {
			G[0] = ddif[0] * 20 / notzero(ddifG[1]) + 155;
			G[1] = ddif[24] * 20 / notzero(ddifG[1]) + 447;
			for (i[0] = 0; i[0] <= 23; i[0]++)
				DrawLine((G[0] * (24 - i[0]) + G[1] * i[0]) / 24,
					-ddif[i[0]] * 34 / notzero(ddifG[1]) + 72,
					(G[0] * (23 - i[0]) + G[1] * (1 + i[0])) / 24,
					-ddif[i[0] + 1] * 34 / notzero(ddifG[1]) + 72, Cr);
			DrawFormatString(490, 80, Cr, L"mdif:%.2f", difkey[4][3] / 100.0);
			DrawFormatString(490, 100, Cr, L"ldif:%.2f", difkey[6][3] / 100.0);
			DrawFormatString(490, 120, Cr, L"mrat:%.2f", DifRate);
			DrawFormatString(490, 140, Cr, L"ndif:%.2f",
				cal_nowdif_p(ddif, Ntime, noteoff, Etime) / 100.0);
#if 0
			/* エラー表示 */
			if (outpoint[1] != 0) {
				DrawFormatString(20, 120, CrR, L"MAPERROR");
				DrawLine(lins(noteoff, 155, Etime, 446, outpoint[0]), 71,
					lins(noteoff, 175, Etime, 465, outpoint[0]), 38, CrR);
			}
#endif
		}
		//判定ずれバー表示
		DrawGraph(219, 460, gapbarimg, TRUE);
		G[0] = gapa[1] % 30;
		for (i[0] = 0; i[0] < 30; i[0]++) {
			G[0]--;
			if (G[0] < 0) G[0] += 30;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (510 - G[0] * 17) / 2);
			DrawGraph(318 - gap[i[0]], 460, gaplineimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 225);
		}
		//キー押し状況表示(オプション)
		if (system[5]) {
			if (holda == 1) { KeyPushCount[0]++; }
			if (holdb == 1) { KeyPushCount[1]++; }
			if (holdc == 1) { KeyPushCount[2]++; }
			if (holdu == 1) { KeyPushCount[3]++; }
			if (holdd == 1) { KeyPushCount[4]++; }
			if (holdl == 1) { KeyPushCount[5]++; }
			if (holdr == 1) { KeyPushCount[6]++; }
			DrawGraph(5, 445, KeyViewimg[maxs(holda, 1)], TRUE);
			DrawGraph(40, 445, KeyViewimg[maxs(holdb, 1)], TRUE);
			DrawGraph(75, 445, KeyViewimg[maxs(holdc, 1)], TRUE);
			DrawGraph(570, 410, KeyViewimg[maxs(holdu, 1)], TRUE);
			DrawGraph(570, 445, KeyViewimg[maxs(holdd, 1)], TRUE);
			DrawGraph(535, 445, KeyViewimg[maxs(holdl, 1)], TRUE);
			DrawGraph(605, 445, KeyViewimg[maxs(holdr, 1)], TRUE);
			if (KeyPushCount[0] == 0) { DrawString(10, 450, L"Z", Cr); }
			else { DrawFormatString(10, 450, Cr, L"%2d", KeyPushCount[0] % 100); }
			if (KeyPushCount[1] == 0) { DrawString(45, 450, L"X", Cr); }
			else { DrawFormatString(45, 450, Cr, L"%2d", KeyPushCount[1] % 100); }
			if (KeyPushCount[2] == 0) { DrawString(80, 450, L"C", Cr); }
			else { DrawFormatString(80, 450, Cr, L"%2d", KeyPushCount[2] % 100); }
			if (KeyPushCount[3] == 0) { DrawString(575, 415, L"↑", Cr); }
			else { DrawFormatString(575, 415, Cr, L"%2d", KeyPushCount[3] % 100); }
			if (KeyPushCount[4] == 0) { DrawString(575, 450, L"↓", Cr); }
			else { DrawFormatString(575, 450, Cr, L"%2d", KeyPushCount[4] % 100); }
			if (KeyPushCount[5] == 0) { DrawString(540, 450, L"←", Cr); }
			else { DrawFormatString(540, 450, Cr, L"%2d", KeyPushCount[5] % 100); }
			if (KeyPushCount[6] == 0) { DrawString(610, 450, L"→", Cr); }
			else { DrawFormatString(610, 450, Cr, L"%2d", KeyPushCount[6] % 100); }
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		//デバック
		fps[fps[60]++] = Ntime - fps[61];
		if (fps[60] > 59)fps[60] -= 60;
		fps[61] = Ntime;
		G[0] = 0;
		for (i[0] = 0; i[0] <= 59; i[0]++)G[0] += fps[i[0]];
		if (AutoFlag == 1) {
			DrawFormatString(20, 80, Cr, L"FPS: %.1f", 60000.0 / notzero(G[0]));
			DrawFormatString(20, 100, Cr, L"Autoplay");
		}
		RECR_DEBUG(0, 3, objectNG, );
		RECR_DEBUG(3, 3, objectN, );
		//データオーバーフローで警告文表示
		if (0 <= note[0][1999].hittime) {
			DrawFormatString(20, 120, CrR, L"UPPER OVER");
		}
		else if (0 <= note[1][1999].hittime) {
			DrawFormatString(20, 120, CrR, L"MIDDLE OVER");
		}
		else if (0 <= note[2][1999].hittime) {
			DrawFormatString(20, 120, CrR, L"LOWER OVER");
		}
		//ライフが20%以下の時、危険信号(ピクチャ)を出す
		if (life <= 100 && drop == 0) DrawGraph(0, 0, dangerimg, TRUE);
		//ライフがなくなったらDROPED扱い
		if (life <= 0 && drop == 0 && AutoFlag == 0) {
			drop = 1;
			Dscore[1] = Dscore[0];
			Dscore[2] = mins(Ntime - noteoff, 0);
		}
		if (drop) { DrawGraph(0, 0, dropimg, TRUE); }
		//ノーツが全部なくなった瞬間の時間を記録
		if (GetRemainNotes2(judge, notes) == 0 && AllNotesHitTime < 0) {
			AllNotesHitTime = GetNowCount();
		}
		//オートでなく、ノーミス以上を出したら演出
		if (AutoFlag == 0 && AllNotesHitTime + 2000 > GetNowCount()) {
			ShowBonusEff(judge, AllNotesHitTime, BonusSnd, Bonusimg, filterimg, BigLightimg, SmallLightimg, flashimg, B_ringimg);
		}
		//終了時間から5秒以上たって、曲が終了したら抜ける。
		if (Etime + 5000 <= Ntime && (musicmp3 == -1 || CheckSoundMem(musicmp3) == 0)) {
			StopSoundMem(musicmp3);
			DeleteSoundMem(musicmp3);
			break;
		}
		if (AutoFlag == 1) {
			if (CheckHitKey(KEY_INPUT_SPACE)) {
				if (key2 == 0) {
					StopFrag *= -1;
					if (StopFrag == 1) {
						StopSoundMem(musicmp3);
					}
					else {
						SetCurrentPositionSoundMem((int)((double)Ntime / 1000.0 * 44100.0), musicmp3);
						PlaySoundMem(musicmp3, DX_PLAYTYPE_BACK, FALSE);
					}
				}
				key2 = 1;
			}
			else {
				key2 = 0;
			}
		}
		if (StopFrag == 1) {
			if (CheckHitKey(KEY_INPUT_LEFT) == 1) {
				if (key3 == 0) {
					Ntime = mins(Ntime - 10000, 0);
					for (i[0] = 0; i[0] < 3; i[0]++) {
						while (Ntime < note[i[0]][objectN[i[0]]].hittime ||
							note[i[0]][objectN[i[0]]].hittime < 0) {
							objectN[i[0]]--;
						}
					}
					for (i[0] = 0; i[0] < 3; i[0]++) {
						objectNG[i[0]] = objectN[i[0]];
					}
					cameraN = 0;
					scroolN = 0;
					itemN = 0;
					SitemN = 0;
					chamoN[0] = 0;
					chamoN[1] = 0;
					chamoN[2] = 0;
					fallN = 0;
					YmoveN[0] = 0;
					YmoveN[1] = 0;
					YmoveN[2] = 0;
					YmoveN[3] = 0;
					YmoveN[4] = 0;
					YmoveN2[0] = 0;
					YmoveN2[1] = 0;
					YmoveN2[2] = 0;
					XmoveN[0] = 0;
					XmoveN[1] = 0;
					XmoveN[2] = 0;
					lockN[0] = 0;
					lockN[1] = 0;
					carrowN = 0;
					viewTN = 0;
					MovieN = 0;
				}
				key3 = 1;
			}
			else if (CheckHitKey(KEY_INPUT_RIGHT) == 1) {
				if (key3 == 0) {
					Ntime += 10000;
					for (i[0] = 0; i[0] < 3; i[0]++) {
						while (note[i[0]][objectN[i[0]]].hittime < Ntime) {
							objectN[i[0]]++;
						}
					}
					for (i[0] = 0; i[0] < 3; i[0]++) {
						objectNG[i[0]] = objectN[i[0]];
					}
					cameraN = 0;
					scroolN = 0;
					itemN = 0;
					SitemN = 0;
					chamoN[0] = 0;
					chamoN[1] = 0;
					chamoN[2] = 0;
					fallN = 0;
					YmoveN[0] = 0;
					YmoveN[1] = 0;
					YmoveN[2] = 0;
					YmoveN[3] = 0;
					YmoveN[4] = 0;
					YmoveN2[0] = 0;
					YmoveN2[1] = 0;
					YmoveN2[2] = 0;
					XmoveN[0] = 0;
					XmoveN[1] = 0;
					XmoveN[2] = 0;
					lockN[0] = 0;
					lockN[1] = 0;
					carrowN = 0;
					viewTN = 0;
					MovieN = 0;
				}
				key3 = 1;
			}
			else {
				key3 = 0;
			}
		}
		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			StopSoundMem(musicmp3);
			DeleteSoundMem(musicmp3);
			InitGraph();
			return 2;
		}
		WaitTimer(5);
		if (StopFrag == -1) {
			Ntime = GetNowCount() - Stime + system[1] * 5;
		}
		else {
			Stime = GetNowCount() - Ntime + system[1] * 5;
		}
		ScreenFlip();
	}
	InitGraph();
	if (AutoFlag == 1) { return 2; }
	else { return result(p, n, o, Lv, drop, difkey[4][3], songN, DifFN, judge, score.sum, Mcombo, notes, gapa, Dscore[3]); }
}

struct judge_box AddHitJudge(struct judge_box ans, int gup) {
	if (-10 <= gup && gup <= 10) {
		ans.pjust++;
	}
	if (-40 <= gup && gup <= 40) {
		ans.just++;
	}
	else if (-70 <= gup && gup <= 70) {
		ans.good++;
	}
	else if (-100 <= gup && gup <= 100) {
		ans.safe++;
	}
	else if (gup <= 200) {
		ans.miss++;
	}
	return ans;
}

/* (ret / 100) */
void cal_back_x(int *xpos, double Gspeed, double Wspeed, double scrool,
	int cam) {
	xpos[0] -= (int)(100 * Gspeed * scrool);
	while (xpos[0] + 100 * cam / 5 > 0) {
		xpos[0] -= 64000;
	}
	while (xpos[0] + 100 * cam / 5 < -64000) {
		xpos[0] += 64000;
	}
	xpos[1] -= (int)(500 * Wspeed * scrool);
	while (xpos[1] + 100 * cam > 0) {
		xpos[1] -= 64000;
	}
	while (xpos[1] + 100 * cam < -64000) {
		xpos[1] += 64000;
	}
	xpos[2] = xpos[1];
	return;
}

int cal_nowdif_p(int *ddif, int Ntime, int noteoff, int Etime) {
	int ret = 0;
	int sect = 0;
	int stime = 0;
	if (Ntime - noteoff <= 0) {
		ret = ddif[0];
	}
	else if (Ntime - Etime >= 0) {
		ret = ddif[24];
	}
	else {
		sect = (Ntime - noteoff) * 24 / (Etime - noteoff);
		stime = (Ntime - noteoff) % ((Etime - noteoff) / 24);
		ret = lins(0, ddif[sect], (Etime - noteoff) / 24, ddif[sect + 1], stime);
	}
	ret = lins(379 * 50, 100, 34733 * 50, 800, ret);
	return ret;
}

int CheckNearHitNote(int un, int mn, int dn, int ut, int mt, int dt) {
	int ans = -1;
	int mintime = 200;
	if (un == NOTE_HIT && ut < mintime) {
		ans = 0;
		mintime = ut;
	}
	if (mn == NOTE_HIT && mt < mintime) {
		ans = 1;
		mintime = mt;
	}
	if (dn == NOTE_HIT && dt < mintime) {
		ans = 2;
		mintime = dt;
	}
	return ans;
}

int GetCharaPos(int time, struct note_box highnote, struct note_box midnote,
	struct note_box lownote, int keyu, int keyd, int keyl, int keyr, int hitatp,
	int hitatt) {
	struct note_box note[3] = { highnote, midnote, lownote };
	int ans = CHARA_POS_MID;
	// near catch/bomb
	for (int i = 0; i < 3; i++) {
		if ((note[i].object == 2 && note[i].hittime <= time + 40 ||
			note[i].object == 7 && note[i].hittime <= time + 40) &&
			keyu == 0 && keyd == 0) {
			return CHARA_POS_MID;
		}
	}
	// hit note
	if (keyu != 1 && keyd != 1 && keyl != 1 && keyr != 1 && hitatt != -1000) {
		return hitatp;
	}
	// push up
	if (1 <= keyu && 0 == keyd) {
		ans = CHARA_POS_UP;
	}
	// push down
	else if (0 == keyu && 1 <= keyd) {
		ans = CHARA_POS_DOWN;
	}
	// push up and down
	else if (1 <= keyu && 1 <= keyd) {
		ans = CHARA_POS_MID;
	}
	// not hit
	else {
		ans = CHARA_POS_MID;
	}
	return ans;
}

int GetHighScore(wchar_t pas[255], int dif) {
	FILE *fp;
	int a[7] = { 0,0,0,0,0,0,0 };
	int G = _wfopen_s(&fp, pas, L"rb");
	if (G == 0) {
		fread(&a, sizeof(int), 6, fp);
		fclose(fp);
	}
	return a[dif];
}

int GetRemainNotes2(struct judge_box judge, int Notes) {
	return Notes - judge.just - judge.good - judge.safe - judge.miss;
}

struct score_box GetScore3(struct score_box score, struct judge_box judge, const int notes,
	const int MaxCombo) {
	score.normal = (judge.just * 90000 + judge.good * 86667 + judge.safe * 45000) / notes;
	score.combo = MaxCombo * 10000 / notes;
	if (score.normal + score.combo - score.loss == 100000) {
		score.pjust = mins(100 - notes + judge.pjust, 0);
	}
	else {
		score.pjust = 0;
	}
	score.sum = score.normal + score.combo - score.loss + score.pjust;
	return score;
}

void Getxxxpng(wchar_t *str, int num) {
	*str = num / 100 + '0';
	str++;
	*str = num / 10 % 10 + '0';
	str++;
	*str = num % 10 + '0';
	str++;
	*str = '.';
	str++;
	*str = 'p';
	str++;
	*str = 'n';
	str++;
	*str = 'g';
	str++;
	*str = '\0';
	return;
}

void Getxxxwav(wchar_t *str, int num) {
	*str = num / 100 + '0';
	str++;
	*str = num / 10 % 10 + '0';
	str++;
	*str = num % 10 + '0';
	str++;
	*str = '.';
	str++;
	*str = 'w';
	str++;
	*str = 'a';
	str++;
	*str = 'v';
	str++;
	*str = '\0';
	return;
}

void note_judge_event(note_judge judge, note_material note, int *viewjudge,
	int *judgename, int *combo, int *life, int *Dscore){
	const int Ncount = GetNowCount();
	viewjudge[judge] = Ncount;
	judgename[0] = judge + 1;
	judgename[1] = Ncount;
	switch (judge) {
	case NOTE_JUDGE_JUST:
		judgename[2] = note;
		*combo += 1;
		*life += 2;
		*Dscore += 1;
		break;
	case NOTE_JUDGE_GOOD:
		judgename[2] = note;
		*combo += 1;
		*life += 1;
		break;
	case NOTE_JUDGE_SAFE:
		judgename[2] = note;
		break;
	case NOTE_JUDGE_MISS:
		judgename[2] = note == NOTE_BOMB ? NOTE_BOMB : 0;
		*combo = 0;
		*life -= 20;
		break;
	default:
		break;
	}
}

int CalPosScore2(struct score_box score, int RemainNotes, int Notes, int combo, int MaxCombo) {
	int PosCombo = mins(combo + RemainNotes, MaxCombo);
	return score.normal + 90000 * RemainNotes / Notes + 10000 * PosCombo / Notes;
}

void ShowCombo(int combo, int *pic) {
#define ROCATION_X 320
#define ROCATION_Y 150
#define CHARA_WIDTH 50

	if (combo < 10) {
		return;
	}
	int t;
	int xx;
	GetGraphSize(pic[0], &xx, &t);
	t = 0;
	int s;
	for (int i = combo; i > 0; i /= 10) {
		t++;
	}
	for (int i = t - 1; i >= 0; i--) {
		s = combo;
		for (int j = 0; j < i; j++) {
			s /= 10;
		}
		s %= 10;
		DrawGraph(t * CHARA_WIDTH / 2 - CHARA_WIDTH / 2 - i * CHARA_WIDTH - xx / 2 + ROCATION_X, ROCATION_Y, pic[s], TRUE);
	}
}

void ShowBonusEff(struct judge_box judge, int EffStartTime, int *Snd, int *pic, int filter,
	int biglight, int *smalllight, int flash, int ring) {
#define NO_MISS 2
#define FULL_COMBO 1
#define PERFECT 0
#define BIG 8
#define SIZE_X 319
#define SIZE_Y 54
#define PIC_X 320
#define PIC_Y 240
	int Bonus = -1;
	if (judge.miss > 0) {
		return;
	}
	else if (judge.safe > 0) {
		Bonus = NO_MISS;
	}
	else if (judge.good > 0) {
		Bonus = FULL_COMBO;
	}
	else {
		Bonus = PERFECT;
	}
	if (EffStartTime >= GetNowCount()) {
		PlaySoundMem(Snd[Bonus], DX_PLAYTYPE_BACK);
	}
	//BlackCover
	if (GetNowCount() < EffStartTime + 1800) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 127);
		DrawGraph(0, 0, filter, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	if (EffStartTime + 1800 <= GetNowCount() && GetNowCount() < EffStartTime + 2000) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(1800, 127, 2000, 0, GetNowCount() - EffStartTime));
		DrawGraph(0, 0, filter, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	//SmallLight
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 4 - Bonus; k++) {
				int posX = j * 180 + EffStartTime % (57137 + 29 * i + 67 * j + 127 * k) % 180;
				int posY = i * 190 + EffStartTime % (62843 + 37 * i + 67 * j + 157 * k) % 190;
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, mins(pals(posY, 255, posY + 240, 0, lins(100, 720, 1000, -240, GetNowCount() - EffStartTime)), 0));
				DrawGraph(posX, posY, smalllight[Bonus], TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
	}
	//BigLight
	if (100 <= GetNowCount() - EffStartTime && GetNowCount() - EffStartTime <= 1000) {
		for (int i = 0; i < 3 - Bonus; i++) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(500, 255, 1000, 0, mins(GetNowCount() - EffStartTime, 500)));
			DrawRotaGraph(PIC_X, PIC_Y, 1, double(GetNowCount() - EffStartTime) / 200.0 + 3.14 * (180.0 / (3 - Bonus)) * i / 180.0, biglight, TRUE, FALSE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}
	//ring
	if (100 <= GetNowCount() - EffStartTime && GetNowCount() - EffStartTime <= 1000) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, maxs(lins(700, 255, 1000, 0, GetNowCount() - EffStartTime), 255));
		DrawExtendGraph(lins(100, PIC_X - 160, 1000, PIC_X - 240, GetNowCount() - EffStartTime),
			lins(100, PIC_Y - 160, 1000, PIC_Y - 240, GetNowCount() - EffStartTime),
			lins(100, PIC_X + 160, 1000, PIC_X + 240, GetNowCount() - EffStartTime),
			lins(100, PIC_Y + 160, 1000, PIC_Y + 240, GetNowCount() - EffStartTime),
			ring, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	//flash
	if (100 <= GetNowCount() - EffStartTime && GetNowCount() - EffStartTime <= 300) {
		for (int i = 0; i < 3 - Bonus; i++) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(100, 191, 300, 0, GetNowCount() - EffStartTime));
			DrawGraph(0, 0, flash, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}
	//BonusText
	if (GetNowCount() < EffStartTime + 100) {
		DrawExtendGraph(lins(0, PIC_X - SIZE_X * BIG / 2, 100, PIC_X - SIZE_X / 2, GetNowCount() - EffStartTime),
			lins(0, PIC_Y - SIZE_Y * BIG / 2, 100, PIC_Y - SIZE_Y / 2, GetNowCount() - EffStartTime),
			lins(0, PIC_X + SIZE_X * BIG / 2, 100, PIC_X + SIZE_X / 2, GetNowCount() - EffStartTime),
			lins(0, PIC_Y + SIZE_Y * BIG / 2, 100, PIC_Y + SIZE_Y / 2, GetNowCount() - EffStartTime),
			pic[Bonus], TRUE);
	}
	else if (EffStartTime + 100 <= GetNowCount() && GetNowCount() < EffStartTime + 1800) {
		DrawGraph(PIC_X - SIZE_X / 2, PIC_Y - SIZE_Y / 2, pic[Bonus], TRUE);
	}
	else if (EffStartTime + 1800 <= GetNowCount() && GetNowCount() < EffStartTime + 2000) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(1800, 255, 2000, 0, GetNowCount() - EffStartTime));
		DrawExtendGraph(lins(2000, PIC_X - SIZE_X, 1800, PIC_X - SIZE_X / 2, GetNowCount() - EffStartTime),
			lins(2000, PIC_Y - SIZE_Y, 1800, PIC_Y - SIZE_Y / 2, GetNowCount() - EffStartTime),
			lins(2000, PIC_X + SIZE_X, 1800, PIC_X + SIZE_X / 2, GetNowCount() - EffStartTime),
			lins(2000, PIC_Y + SIZE_Y, 1800, PIC_Y + SIZE_Y / 2, GetNowCount() - EffStartTime),
			pic[Bonus], TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
}

void ShowJudge(const int *viewjudge, const int *judgeimg, const int posX, const int posY) {
	for (int i = 0; i < 4; i++) {
		if (GetNowCount() - viewjudge[i] < 750) {
			DrawGraph(posX, posY + pals(250, 0, 0, 25, maxs(GetNowCount() - viewjudge[i], 250)), judgeimg[i], TRUE);
		}
	}
	return;
}

void ShowScore2(struct score_box score, int Hscore, int time) {
	unsigned int Cr = GetColor(255, 255, 255);
	int s_score = score.sum;
	if (time - score.time < 500) {
		s_score = pals(500, score.sum, 0, score.before, time - score.time);
	}
	if (Hscore <= s_score) {
		Cr = GetColor(255, 255, 0);
	}
	DrawFormatString(490, 20, Cr, L"SCORE:%d", s_score);
}

char PlayNoteHitSound(note_box note, int *MelodySnd, int *Sitem, char seflag,
	int notemat) {
	if (note.melody != MELODYSOUND_NONE) {
		PlaySoundMem(MelodySnd[note.melody], DX_PLAYTYPE_BACK);
	}
	else if (note.sound != 0) {
		PlaySoundMem(Sitem[note.sound - 1], DX_PLAYTYPE_BACK);
	}
	else {
		seflag |= notemat;
	}
	return seflag;
}

void RunningStats2(struct judge_box judge, int PosScore, int HighScore) {
#define x1 6
#define y1 6
#define x2 188
#define y2 6
#define x3 150
#define y3 69
#define x4 6
#define y4 69
	unsigned int CrG = GetColor(63, 63, 63);
	unsigned int CrD = GetColor(255, 63, 127);
	unsigned int CrY = GetColor(255, 255, 0);
	unsigned int CrC = GetColor(0, 255, 255);
	if (judge.miss > 0) {
		DrawTriangle(x1, y1, x2, y2, x3, y3, CrG, TRUE);
	}
	else if (judge.safe > 0) {
		DrawTriangle(x1, y1, x2, y2, x3, y3, CrD, TRUE);
	}
	else if (judge.good > 0) {
		DrawTriangle(x1, y1, x2, y2, x3, y3, CrY, TRUE);
	}
	else {
		DrawTriangle(x1, y1, x2, y2, x3, y3, CrC, TRUE);
	}
	if (PosScore < HighScore) {
		DrawTriangle(x1, y1, x3, y3, x4, y4, CrG, TRUE);
	}
	else {
		DrawTriangle(x1, y1, x3, y3, x4, y4, GetColor(lins(HighScore, 255, 100000, 0, PosScore), 255, lins(HighScore, 0, 100000, 255, PosScore)), TRUE);
	}
}
