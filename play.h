#include "playing.h"
#include "RecordLoad.h"
#include "fontcur.h"

int play2(int n, int o, int shift) {
	/*---用語定義-----
	ユーザー用譜面データ: ユーザーが作った譜面データ。ユーザーに分かりやすい。
	PC用譜面データ: ユーザー用譜面データから計算で作られた、PC専用の譜面データ。PCに分かりやすい。
	----------------*/
	//n: 曲ナンバー
	//o: 難易度ナンバー
	short int i[2];
	short int Lv = 0;
	short int notes = 0;
	short int bgp[3] = { 0,0,0 }; //[0:sky,1:ground,2:water]の横位置
	short int bgf[2] = { 0,0 }; //落ち物背景の[0:横位置,1:縦位置]
	short int charaput = 1; //キャラの今の位置[0で上,1で中,2で下]
	short int drop = 0;
	short int rank = 0;
	short int Clear = 0;
	short int KeyPushCount[7] = { 0,0,0,0,0,0,0 };
	int judgh = 0; //ノーツの時間距離
	int charahit = 0; //キャラがノーツをたたいた後であるかどうか。[1以上で叩いた、0で叩いてない]
	int G[20], songT;
	int system[6] = { 0,0,0,2,0,0 };
	int noteoff = 0; //ノーツのオフセット
	int Etime = 0; //譜面の終わりの時間
	int Ntime = 0;
	int judghname[3][3]{ 0,0,0,0,0,0,0,0,0 };
	int holda = 0;
	int holdb = 0;
	int holdc = 0;
	int holdu = 0;
	int holdd = 0;
	int holdl = 0;
	int holdr = 0;
	int holdG = 0;
	int combo = 0;
	int LaneTrack[3] = { -150,-150,-150 };
	int Mcombo = 0;
	int Dscore[4] = { 0,0,0,0 }; //距離に当たる部分[加点用,加点保存用,距離保存用,実点数]
	int judghcount[4] = { 0,0,0,0 };
	int life = 500;
	int gap[30] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//gap = 判定ずれ
	int gapa[3] = { 0,0,0 };//gapa = 判定ずれ[合計, 個数, 2乗の合計]
	int score2[4] = { 0,0,0,0 }; //[通常スコア(90000), コンボスコア(10000), 減点, 合計]
	int fps[62];//0～59=1フレーム間隔の時間,60=次の代入先,61=前回の時間
	for (i[0] = 0; i[0] <= 59; i[0]++)fps[i[0]] = 17;
	fps[60] = 0;
	fps[61] = 0;
	double GD[5];
	int item[99]; //アイテムのアドレス、DrawGraphで呼べる。
	short int itemN = 0; //↑の番号
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
	int Movie[14][99];//アイテム表示[アイテム番号,移動形態,開始時間,終了時間,開始x位置,終了x位置,開始y位置,終了y位置,開始サイズ,終了サイズ,開始角度,終了角度,開始透明度,終了透明度]
	short int MovieN = 0;
	int object[3][5][999]; //[上,中,下]レーンの音符の[時間,種類,(使ってない),縦位置,横位置]
	short int objectN[3] = { 0,0,0 }; //↑の番号
	int difkey[50][4];//難易度計算に使う[番号][入力キー,時間,難易度点,[0]個数上限:[1]今の番号:[2]1個前の番号:[3]2個前の番号:[4]最高点:[5]データ個数:[6]最後50個の合計:[7]計算から除外する時間]
	int ddif[25] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//各区間の難易度
	int ddifG[2] = { 1,25 };//0=今いる区間番号(1～25),1=最大値
	int Yline[5] = { 300,350,400,350,600 };//[上,中,下,地面,水中]レーンの縦位置
	int Xline[3] = { 150,150,150 };//[上,中,下]レーンの横位置
	double rate, SumRate[2] = { 0,0 }, bpm = 120, bpmG = 120;
	double timer[3]; //[上, 中, 下]レーンの時間
	double speedt[5][99][2]; //[上, 中, 下, (地面), (水中)]レーンの[0:切り替え時間,1:速度]
	double DifRate; //譜面定数
	short int speedN[5] = { 0,0,0,0,0 }; //↑の番号
	char key[256];
	wchar_t songN[255];
	wchar_t songNE[255];
	wchar_t fileN[255];
	wchar_t dataE[255] = L"record/";
	wchar_t RRS[255]; //PC用譜面データの保存場所
	wchar_t mp3FN[255] = L"song/";
	wchar_t skyFN[255] = L"picture/backskynoamal.png";
	wchar_t groundFN[255] = L"picture/groundnaturenormal.png";
	wchar_t waterFN[255] = L"picture/waternormal.png";
	wchar_t DifFN[255] = L"picture/difanother.png";
	wchar_t GT1[255];
	wchar_t GT26[6][7] = { L"/0.rrs" ,L"/1.rrs" ,L"/2.rrs" ,L"/3.rrs" ,L"/4.rrs" ,L"/5.rrs" };
	unsigned int Cr, Crb;
	Cr = GetColor(255, 255, 255);
	Crb = GetColor(0, 0, 0);
	FILE *fp;
	//システムロード
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&system, sizeof(int), 6, fp);
		fclose(fp);
	}
	songT = FileRead_open(L"song.txt");
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
		RecordLoad(n, o);//rrsデータが無い、または作成の指示があれば作る
		G[2] = _wfopen_s(&fp, GT1, L"rb");//rrsデータを読み込む
	}
	if (G[2] == 0) {
		fread(&mp3FN, 255, 1, fp);//音楽ファイル名
		fread(&bpm, sizeof(double), 1, fp);//BPM
		fread(&noteoff, sizeof(int), 1, fp);//offset
		fread(&skyFN, 255, 1, fp);//空背景名
		fread(&groundFN, 255, 1, fp);//地面画像名
		fread(&waterFN, 255, 1, fp);//水中画像名
		fread(&songN, 255, 1, fp);//曲名
		fread(&songNE, 255, 1, fp);//曲名(英語)
		fread(&Lv, sizeof(short int), 1, fp);//レベル
		fread(&item, sizeof(int), 99, fp);//アイテム画像データ(動作未確認)
		fread(&fall, sizeof(int), 198, fp);//落ち物背景切り替えタイミング
		fread(&speedt, sizeof(double), 990, fp);//レーン速度
		fread(&chamo, sizeof(int), 594, fp);//キャラグラ変換タイミング
		fread(&Ymove, sizeof(int), 19980, fp);//縦位置移動タイミング
		fread(&Xmove, sizeof(int), 11988, fp);//横位置移動タイミング
		fread(&lock, sizeof(int), 396, fp);//ノーツ固定切り替えタイミング
		fread(&carrow, sizeof(int), 198, fp);//キャラ向き切り替えタイミング
		fread(&viewT, sizeof(int), 198, fp);//ノーツ表示時間変換タイミング
		fread(&object, sizeof(int), 14985, fp);//ノーツデータ
		fread(&notes, sizeof(short int), 1, fp);//ノーツ数
		fread(&Etime, sizeof(int), 1, fp);//曲終了時間
		fread(&G, sizeof(int), 2, fp);
		difkey[4][3] = G[0];//最高難易度
		difkey[6][3] = G[1];//最終難易度
		fread(&ddif, sizeof(int), 25, fp);//各区間難易度データ
		fread(&ddifG, sizeof(int), 2, fp);//各区間難易度データ
		fread(&DifFN, 255, 1, fp);//難易度バー名
		fread(&Movie, sizeof(int), 1386, fp);//動画データ
	}
	fclose(fp);
	//グラフィックと効果音の準備
	/*グラフィック用変数
	judghimg = 判定マーク
	hitimg = hit
	hitcimg = hitの周りの色
	catchimg = catch
	upimg = up
	downimg = down
	leftimg = left
	rightimg = right
	bombimg = bomb
	goustimg = goust
	backskyimg
	backgroundimg
	dangerimg
	charaimg
	musicmp3 = 音楽*/
	int judghimg, hitimg, hitcimg, catchimg, upimg, downimg, leftimg, rightimg, bombimg, goustimg;
	int backskyimg, backgroundimg, backwaterimg, dangerimg, dropimg, resultimg, sbarimg, sbbarimg, filterimg, charaguideimg, gapbarimg, gaplineimg, difberimg;
	int Lbarimg[3];
	int Tbarimg[2];
	int	charaimg[30];
	int rankimg[6];
	int coleimg[5];
	int effimg[7][5];
	int KeyViewimg[2];
	int Rchaimg;
	int musicmp3, attack, catchs, arrow, bomb;
	judghimg = LoadGraph(L"picture/Marker.png");
	hitimg = LoadGraph(L"picture/hit.png");
	hitcimg = LoadGraph(L"picture/hitc.png");
	catchimg = LoadGraph(L"picture/catch.png");
	upimg = LoadGraph(L"picture/up.png");
	downimg = LoadGraph(L"picture/down.png");
	leftimg = LoadGraph(L"picture/left.png");
	rightimg = LoadGraph(L"picture/right.png");
	bombimg = LoadGraph(L"picture/bomb.png");
	goustimg = LoadGraph(L"picture/goust.png");
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
	backskyimg = LoadGraph(skyFN);
	backgroundimg = LoadGraph(groundFN);
	backwaterimg = LoadGraph(waterFN);
	dangerimg = LoadGraph(L"picture/danger.png");
	dropimg = LoadGraph(L"picture/drop.png");
	resultimg = LoadGraph(L"picture/result.png");
	sbarimg = LoadGraph(L"picture/scoreber.png");
	sbbarimg = LoadGraph(L"picture/scoreber2.png");
	Lbarimg[0] = LoadGraph(L"picture/LIFEbar.png");
	Lbarimg[1] = LoadGraph(L"picture/LIFEbar2.png");
	Lbarimg[2] = LoadGraph(L"picture/LIFEbar3.png");
	Tbarimg[0] = LoadGraph(L"picture/TIMEbar.png");
	Tbarimg[1] = LoadGraph(L"picture/TIMEbar2.png");
	rankimg[0] = LoadGraph(L"picture/rankEX.png");
	rankimg[1] = LoadGraph(L"picture/rankS.png");
	rankimg[2] = LoadGraph(L"picture/rankA.png");
	rankimg[3] = LoadGraph(L"picture/rankB.png");
	rankimg[4] = LoadGraph(L"picture/rankC.png");
	rankimg[5] = LoadGraph(L"picture/rankD.png");
	coleimg[0] = LoadGraph(L"picture/DROPED.png");
	coleimg[1] = LoadGraph(L"picture/CLEARED.png");
	coleimg[2] = LoadGraph(L"picture/NOMISS.png");
	coleimg[3] = LoadGraph(L"picture/FULLCOMBO.png");
	coleimg[4] = LoadGraph(L"picture/PERFECT.png");
	filterimg = LoadGraph(L"picture/Black.png");
	charaguideimg = LoadGraph(L"picture/Cguide.png");
	gapbarimg = LoadGraph(L"picture/GapBer.png");
	gaplineimg = LoadGraph(L"picture/GapBerLine.png");
	LoadDivGraph(L"picture/hiteff.png", 5, 5, 1, 50, 50, effimg[0]);
	LoadDivGraph(L"picture/hiteff.png", 5, 5, 1, 50, 50, effimg[1]);
	LoadDivGraph(L"picture/upeff.png", 5, 5, 1, 50, 50, effimg[2]);
	LoadDivGraph(L"picture/downeff.png", 5, 5, 1, 50, 50, effimg[3]);
	LoadDivGraph(L"picture/lefteff.png", 5, 5, 1, 50, 50, effimg[4]);
	LoadDivGraph(L"picture/righteff.png", 5, 5, 1, 50, 50, effimg[5]);
	LoadDivGraph(L"picture/bombeff.png", 5, 5, 1, 50, 50, effimg[6]);
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
		//背景を真っ暗にしている場合、背景と動画を表示しない。
		if (system[3] != 0) {
			//ここから背景
			//背景の横位置計算
			if (speedt[3][speedN[3] + 1][0] < Ntime && speedt[3][speedN[3] + 1][0] >= 0) speedN[3]++;
			bgp[0] -= speedt[3][speedN[3]][1];
			if (bgp[0] < -700) bgp[0] += 640;
			else if (bgp[0] > 0) bgp[0] -= 640;
			bgp[1] -= speedt[3][speedN[3]][1] * 5;
			if (bgp[1] < -700) bgp[1] += 640;
			else if (bgp[1] > 0) bgp[1] -= 640;
			bgp[2] -= speedt[4][speedN[4]][1] * 5;
			if (bgp[2] < -700) bgp[2] += 640;
			else if (bgp[2] > 0) bgp[2] -= 640;
			//背景の縦位置計算
			for (i[0] = 3; i[0] <= 4; i[0]++) {
				if (Ntime >= Ymove[i[0]][YmoveN[i[0]]][0] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0]) {
					switch (Ymove[i[0]][YmoveN[i[0]]][3]) {
					case(1):
						Yline[i[0]] = lins(Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ntime);
						break;
					case(2):
						Yline[i[0]] = pals(Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ntime);
						break;
					case(3):
						Yline[i[0]] = pals(Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ntime);
						break;
					}
				}
				if (Ntime >= Ymove[i[0]][YmoveN[i[0]]][2] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0] || Ymove[i[0]][YmoveN[i[0]]][3] == 4) Yline[i[0]] = Ymove[i[0]][YmoveN[i[0]]++][1];
			}
			//背景表示
			for (i[0] = 0; i[0] < 2; i[0]++) {
				G[0] = bgp[i[0]];
				while (G[0] < 700) {
					switch (i[0]) {
					case(0):
						DrawGraph(G[0], Yline[3] / 5 - 160, backskyimg, TRUE);
						break;
					case(1):
						DrawGraph(G[0], Yline[3] - 400, backgroundimg, TRUE);
						DrawGraph(G[0], Yline[4] - 400, backwaterimg, TRUE);
						break;
					}
					G[0] += 640;
				}
			}
			//落ち物背景表示
			if (Ntime >= fall[fallN + 1][0] && fall[fallN + 1][0] >= 0) fallN++;
			if (fall[fallN][1] >= 0) {
				G[0] = bgf[0];//横
				G[1] = bgf[1] + Yline[3];//縦
				for (i[0] = 0; i[0] < 2; i[0]++) for (i[1] = 0; i[1] < 3; i[1]++) DrawGraph(G[0] + i[0] * 640, G[1] - i[1] * 480, item[fall[fallN][1]], TRUE);
				bgf[0] -= 5;
				bgf[1] += 2;
			}
			if (bgf[0] <= -640)bgf[0] += 640;
			if (bgf[1] >= 640)bgf[1] -= 480;
			//ここまで背景表示
			//ここからアイテム表示
			while (Movie[3][MovieN] < Ntime && Movie[3][MovieN]>-500) { MovieN++; }
			G[0] = 0;
			while (Movie[3][MovieN + G[0]] > -500) {
				if (Movie[2][MovieN + G[0]] > Ntime) {
					G[0]++;
					continue;
				}
				GetGraphSize(item[Movie[0][MovieN + G[0]]], &G[1], &G[2]);
				G[1] /= 2;
				G[2] /= 2;
				switch (Movie[1][MovieN + G[0]]) {
				case 1:
					G[3] = lins(Movie[2][MovieN + G[0]], Movie[4][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[5][MovieN + G[0]], Ntime);
					G[4] = lins(Movie[2][MovieN + G[0]], Movie[6][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[7][MovieN + G[0]], Ntime);
					G[5] = lins(Movie[2][MovieN + G[0]], Movie[8][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[9][MovieN + G[0]], Ntime);
					G[6] = lins(Movie[2][MovieN + G[0]], Movie[10][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[11][MovieN + G[0]], Ntime);
					G[7] = lins(Movie[2][MovieN + G[0]], Movie[12][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[13][MovieN + G[0]], Ntime);
					break;
				case 2:
					G[3] = pals(Movie[2][MovieN + G[0]], Movie[4][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[5][MovieN + G[0]], Ntime);
					G[4] = pals(Movie[2][MovieN + G[0]], Movie[6][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[7][MovieN + G[0]], Ntime);
					G[5] = pals(Movie[2][MovieN + G[0]], Movie[8][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[9][MovieN + G[0]], Ntime);
					G[6] = pals(Movie[2][MovieN + G[0]], Movie[10][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[11][MovieN + G[0]], Ntime);
					G[7] = pals(Movie[2][MovieN + G[0]], Movie[12][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[13][MovieN + G[0]], Ntime);
					break;
				case 3:
					G[3] = pals(Movie[3][MovieN + G[0]], Movie[4][MovieN + G[0]], Movie[2][MovieN + G[0]], Movie[5][MovieN + G[0]], Ntime);
					G[4] = pals(Movie[3][MovieN + G[0]], Movie[6][MovieN + G[0]], Movie[2][MovieN + G[0]], Movie[7][MovieN + G[0]], Ntime);
					G[5] = pals(Movie[3][MovieN + G[0]], Movie[8][MovieN + G[0]], Movie[2][MovieN + G[0]], Movie[9][MovieN + G[0]], Ntime);
					G[6] = pals(Movie[3][MovieN + G[0]], Movie[10][MovieN + G[0]], Movie[2][MovieN + G[0]], Movie[11][MovieN + G[0]], Ntime);
					G[7] = pals(Movie[3][MovieN + G[0]], Movie[12][MovieN + G[0]], Movie[2][MovieN + G[0]], Movie[13][MovieN + G[0]], Ntime);
					break;
				}
				G[7] = betweens(0, G[7], 255);
				G[8] = -G[1]; G[9] = -G[2];
				G[10] = G[1]; G[11] = -G[2];
				G[12] = G[1]; G[13] = G[2];
				G[14] = -G[1]; G[15] = G[2];
				for (i[0] = 8; i[0] < 16; i[0]++) { G[i[0]] = G[i[0]] * G[5] / 100; }
				for (i[0] = 8; i[0] < 16; i[0] += 2) {
					G[16] = G[i[0]];
					G[i[0]] = G[i[0]] * cosC(G[6]) - G[i[0] + 1] * sinC(G[6]) + G[3];
					G[i[0] + 1] = G[16] * sinC(G[6]) + G[i[0] + 1] * cosC(G[6]) + G[4];
				}
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, G[7]);
				DrawModiGraph(G[8], G[9], G[10], G[11], G[12], G[13], G[14], G[15], item[Movie[0][MovieN + G[0]]], TRUE);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				G[0]++;
			}
			//ここまでアイテム表示
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
		//キャラ位置ガイドの表示
		if (carrow[1][carrowN + 1] < Ntime && carrow[1][carrowN + 1] >= 0) carrowN++;
		if (carrow[0][carrowN] == 1) { DrawGraph(Xline[charaput] - 4, Yline[charaput] - 4, charaguideimg, TRUE); }
		else { DrawTurnGraph(Xline[charaput] - 56, Yline[charaput] - 4, charaguideimg, TRUE); }
		//判定マーカーの表示
		for (i[0] = 0; i[0] < 3; i[0]++) {
			//縦移動
			if (Ntime >= Ymove[i[0]][YmoveN[i[0]]][0] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0]) {
				switch (Ymove[i[0]][YmoveN[i[0]]][3]) {
				case(1):
					Yline[i[0]] = lins(Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ntime);
					break;
				case(2):
					Yline[i[0]] = pals(Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ntime);
					break;
				case(3):
					Yline[i[0]] = pals(Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ntime);
					break;
				}
			}
			if (Ntime >= Ymove[i[0]][YmoveN[i[0]]][2] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0] || Ymove[i[0]][YmoveN[i[0]]][3] == 4) Yline[i[0]] = Ymove[i[0]][YmoveN[i[0]]++][1];
			//横移動
			if (Ntime >= Xmove[i[0]][XmoveN[i[0]]][0] && 0 <= Xmove[i[0]][XmoveN[i[0]]][0]) {
				switch (Xmove[i[0]][XmoveN[i[0]]][3]) {
				case(1):
					Xline[i[0]] = lins(Xmove[i[0]][XmoveN[i[0]]][0], Xmove[i[0]][XmoveN[i[0]] - 1][1], Xmove[i[0]][XmoveN[i[0]]][2], Xmove[i[0]][XmoveN[i[0]]][1], Ntime);
					break;
				case(2):
					Xline[i[0]] = pals(Xmove[i[0]][XmoveN[i[0]]][0], Xmove[i[0]][XmoveN[i[0]] - 1][1], Xmove[i[0]][XmoveN[i[0]]][2], Xmove[i[0]][XmoveN[i[0]]][1], Ntime);
					break;
				case(3):
					Xline[i[0]] = pals(Xmove[i[0]][XmoveN[i[0]]][2], Xmove[i[0]][XmoveN[i[0]]][1], Xmove[i[0]][XmoveN[i[0]]][0], Xmove[i[0]][XmoveN[i[0]] - 1][1], Ntime);
					break;
				}
			}
			if (Ntime >= Xmove[i[0]][XmoveN[i[0]]][2] && 0 <= Xmove[i[0]][XmoveN[i[0]]][0] || Xmove[i[0]][XmoveN[i[0]]][3] == 4) Xline[i[0]] = Xmove[i[0]][XmoveN[i[0]]++][1];
			DrawGraph(Xline[i[0]], Yline[i[0]], judghimg, TRUE);
		}
		//キャラグラ変換
		for (i[0] = 0; i[0] < 3; i[0]++)while (Ntime >= chamo[i[0]][chamoN[i[0]] + 1][1] && chamo[i[0]][chamoN[i[0]] + 1][1] >= 0) {
			chamoN[i[0]]++;
		}
		G[3] = 0;
		//キャラ位置決め
		for (i[0] = 0; i[0] <= 2; i[0]++) if (object[i[0]][0][objectN[i[0]]] <= Ntime + 16 && object[i[0]][1][objectN[i[0]]] == 2) G[3] = 1;
		for (i[0] = 0; i[0] <= 2; i[0]++) {
			G[i[0]] = objectN[i[0]] - 1;
			while (object[i[0]][1][G[i[0]]] == 8 && G[i[0]] >= 1) G[i[0]]--;
		}
		//上ラインのヒットマーカーをたたいているとき
		if (charahit > 0 && G[3] == 0 && object[0][0][G[0]] > object[1][0][G[1]] && object[0][0][G[0]] > object[2][0][G[2]] && object[0][0][G[0]] + 750 > Ntime && object[0][1][G[0]] == 1 && judghname[0][0] >= 1 && judghname[0][0] <= 3) {
			G[4] = Yline[0];
			charaput = 0;
		}
		//中ラインのヒットマーカーをたたいているとき
		else if (charahit > 0 && G[3] == 0 && object[1][0][G[1]] > object[0][0][G[0]] && object[1][0][G[1]] > object[2][0][G[2]] && object[1][0][G[1]] + 750 > Ntime && object[1][1][G[1]] == 1 && judghname[1][0] >= 1 && judghname[1][0] <= 3) {
			G[4] = Yline[1];
			charaput = 1;
		}
		//下ラインのヒットマーカーをたたいているとき
		else if (charahit > 0 && G[3] == 0 && object[2][0][G[2]] > object[0][0][G[0]] && object[2][0][G[2]] > object[1][0][G[1]] && object[2][0][G[2]] + 750 > Ntime && object[2][1][G[2]] == 1 && judghname[2][0] >= 1 && judghname[2][0] <= 3) {
			G[4] = Yline[2];
			charaput = 2;
		}
		//上が押されて、直前のヒットマーカーをたたいていないとき
		else if (key[KEY_INPUT_UP] == 1 && key[KEY_INPUT_DOWN] == 0) {
			G[4] = Yline[0];
			charaput = 0;
		}
		//下が押されて、直前のヒットマーカーをたたいていないとき
		else if (key[KEY_INPUT_UP] == 0 && key[KEY_INPUT_DOWN] == 1) {
			G[4] = Yline[2];
			charaput = 2;
		}
		//何も押されていないか、上下同時に押されていて、直前のヒットマーカーをたたいていないとき
		else {
			G[4] = Yline[1];
			charaput = 1;
		}
		//キャラグラフィックを表示
		if (GetNowCount() - charahit > 250) G[5] = 0;
		else G[5] = pals(250, 0, 0, 50, GetNowCount() - charahit);
		if (charahit > 0) {
			if (carrow[0][carrowN] == 1) DrawGraph(Xline[charaput] + G[5] - 160, G[4] - 75, charaimg[maxs(mins((GetNowCount() - charahit) / 125 + 18, 18), 23)], TRUE);
			else DrawTurnGraph(Xline[charaput] - G[5] + 30, G[4] - 75, charaimg[maxs(mins((GetNowCount() - charahit) / 125 + 18, 18), 23)], TRUE);
		}
		else {
			if (carrow[0][carrowN] == 1) DrawGraph(Xline[charaput] - 160, G[4] - 75, charaimg[Ntime * int(bpm) / 20000 % 6 + chamo[charaput][chamoN[charaput]][0] * 6], TRUE);
			else DrawTurnGraph(Xline[0] + 30, G[4] - 75, charaimg[Ntime * int(bpm) / 20000 % 6 + chamo[charaput][chamoN[charaput]][0] * 6], TRUE);
		}
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
		if (key[KEY_INPUT_G] == 0) holdG = 0;
		else if (key[KEY_INPUT_G] == 1) holdG++;
		if (GetWindowUserCloseFlag(TRUE)) return 5;
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
		//音符表示
		for (i[0] = 0; i[0] < 2; i[0]++) if (Ntime >= lock[i[0]][1][lockN[i[0]] + 1] && lock[i[0]][1][lockN[i[0]] + 1] >= 0) lockN[i[0]]++;
		if (viewT[0][viewTN + 1] <= Ntime && viewT[0][viewTN + 1] >= 0) viewTN++;
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (speedt[i[0]][speedN[i[0]] + 1][0] <= Ntime && speedt[i[0]][speedN[i[0]] + 1][0] >= 0) speedN[i[0]]++;
			G[0] = G[3] = G[4] = G[5] = 0;
			for (i[1] = objectN[i[0]]; object[i[0]][0][i[1]] > 0; i[1]++) {
				if (object[i[0]][0][i[1]] >= viewT[0][viewTN + G[0] + 1] && viewT[0][viewTN + G[0] + 1] >= 0) G[0]++;
				if (object[i[0]][0][i[1]] - Ntime >= viewT[1][viewTN + G[0]]) continue;
				if (object[i[0]][0][i[1]] - Ntime >= 3000 && 3000 >= viewT[1][viewTN + G[0]]) break;
				if (object[i[0]][0][i[1]] >= lock[0][1][lockN[0] + G[3] + 1] && lock[0][1][lockN[0] + G[3] + 1] >= 0) G[3]++;
				if (object[i[0]][0][i[1]] >= lock[1][1][lockN[1] + G[4] + 1] && lock[1][1][lockN[1] + G[4] + 1] >= 0) G[4]++;
				while (object[i[0]][0][i[1]] >= speedt[i[0]][speedN[i[0]] + G[5] + 1][0] && speedt[i[0]][speedN[i[0]] + G[5] + 1][0] >= 0) G[5]++;
				//縦位置
				if (lock[1][0][lockN[1] + G[4]] == 1) G[2] = object[i[0]][3][i[1]];
				else G[2] = Yline[i[0]];
				//横位置
				G[1] = (speedt[i[0]][speedN[i[0]] + G[5]][1] * 20 * (object[i[0]][0][i[1]] - Ntime) + 5000) / 50;
				G[1] += 50;
				if (lock[0][0][lockN[0] + G[3]] == 1) G[1] += object[i[0]][4][i[1]] - 150;
				else G[1] += Xline[i[0]] - 150;
				switch (object[i[0]][1][i[1]]) {
				case 1:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], hitcimg, TRUE);
					break;
				case 2:
					DrawGraph(G[1], G[2], catchimg, TRUE);
					break;
				case 3:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], upimg, TRUE);
					break;
				case 4:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], downimg, TRUE);
					break;
				case 5:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], leftimg, TRUE);
					break;
				case 6:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], rightimg, TRUE);
					break;
				case 7:
					DrawGraph(G[1], G[2], bombimg, TRUE);
					break;
				case 8:
					DrawGraph(G[1], G[2], goustimg, TRUE);
					break;
				}
			}
		}
		//判定、ヒットノーツ
		G[0] = 0;//押したボタンの数
		if (holda == 1) { G[0]++; }
		if (holdb == 1) { G[0]++; }
		if (holdc == 1) { G[0]++; }
		while (G[0] > 0) {
			G[1] = -1;//一番近いヒットノーツ[-1:ヒットノーツがない、0:上レーン、1:中レーン、2:下レーン]
			G[2] = 200;//押したタイミングとノーツのタイミングのずれ
			for (i[0] = 0; i[0] < 3; i[0]++) {
				G[2] = object[i[0]][0][objectN[i[0]]] - Ntime;
				if (object[i[0]][1][objectN[i[0]]] == 1 && G[2] <= 125 && G[2] >= -100) {
					G[1] = i[0];
					break;
				}
			}
			if (G[1] == -1) { break; }
			for (i[0] = G[1] + 1; i[0] < 3; i[0]++) {
				G[2] = object[i[0]][0][objectN[i[0]]] - Ntime;
				if (object[i[0]][1][objectN[i[0]]] == 1 && G[2] <= 125 && G[2] >= -100 &&
				object[i[0]][0][objectN[i[0]]] <= object[G[1]][0][objectN[0]]) {
				G[1] = i[0];
				}
			}
			G[2] = object[G[1]][0][objectN[G[1]]] - Ntime;
			judghname[G[1]][1] = GetNowCount();
			judghname[G[1]][2] = 1;
			objectN[G[1]]++;
			gap[gapa[1] % 30] = G[2];
			gapa[0] += G[2];
			gapa[2] += G[2] * G[2];
			gapa[1]++;
			//just
			if (G[2] <= 40 && G[2] >= -40) {
				judghname[G[1]][0] = 1;
				combo++;
				judghcount[0]++;
				life += 2;
				Dscore[0] += 2;
				PlaySoundMem(attack, DX_PLAYTYPE_BACK);
			}
			//good
			else if (G[2] <= 70 && G[2] >= -70) {
				judghname[G[1]][0] = 2;
				combo++;
				judghcount[1]++;
				life++;
				Dscore[0]++;
				PlaySoundMem(attack, DX_PLAYTYPE_BACK);
			}
			//safe
			else if (G[2] <= 100 && G[2] >= -100) {
				judghname[G[1]][0] = 3;
				judghcount[2]++;
				life++;
				PlaySoundMem(attack, DX_PLAYTYPE_BACK);
			}
			//fastmiss
			else if (G[2] <= 125) {
				judghname[G[1]][0] = 4;
				combo = 0;
				judghcount[3]++;
				life -= 20;
			}
			G[0]--;
		}
		for (i[0] = 0; i[0] < 3; i[0]++) {
			judgh = object[i[0]][0][objectN[i[0]]] - Ntime;
			//キャッチノーツ(justのみ)
			if (LaneTrack[i[0]] + 100 >= object[i[0]][0][objectN[i[0]]] && object[i[0]][1][objectN[i[0]]] == 2 && judgh <= 0) {
				judghname[i[0]][0] = 1;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 2;
				objectN[i[0]]++;
				combo++;
				charahit = 0;
				judghcount[0]++;
				life += 2;
				Dscore[0] += 2;
				PlaySoundMem(catchs, DX_PLAYTYPE_BACK);
			}
			//アローノーツ各種
			else if ((holdu == 1) && object[i[0]][1][objectN[i[0]]] == 3 && judgh <= 125 && judgh >= -100) {
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 3;
				objectN[i[0]]++;
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				//just
				if (judgh <= 40 && judgh >= -40) {
					judghname[i[0]][0] = 1;
					combo++;
					judghcount[0]++;
					life += 2;
					Dscore[0] += 2;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
					judghname[i[0]][0] = 2;
					combo++;
					judghcount[1]++;
					life++;
					Dscore[0]++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
					judghname[i[0]][0] = 3;
					judghcount[2]++;
					life++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//fastmiss
				else if (judgh <= 125) {
					judghname[i[0]][0] = 4;
					combo = 0;
					judghcount[3]++;
					life -= 20;
				}
			}
			else if ((holdd == 1) && object[i[0]][1][objectN[i[0]]] == 4 && judgh <= 125 && judgh >= -100) {
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 4;
				objectN[i[0]]++;
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				//just
				if (judgh <= 40 && judgh >= -40) {
					judghname[i[0]][0] = 1;
					combo++;
					judghcount[0]++;
					life += 2;
					Dscore[0] += 2;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
					judghname[i[0]][0] = 2;
					combo++;
					judghcount[1]++;
					life++;
					Dscore[0]++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
					judghname[i[0]][0] = 3;
					judghcount[2]++;
					life++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//fastmiss
				else if (judgh <= 125) {
					judghname[i[0]][0] = 4;
					combo = 0;
					judghcount[3]++;
					life -= 20;
				}
			}
			else if ((holdl == 1) && object[i[0]][1][objectN[i[0]]] == 5 && judgh <= 125 && judgh >= -100) {
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 5;
				objectN[i[0]]++;
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				//just
				if (judgh <= 40 && judgh >= -40) {
					judghname[i[0]][0] = 1;
					combo++;
					judghcount[0]++;
					life += 2;
					Dscore[0] += 2;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
					judghname[i[0]][0] = 2;
					combo++;
					judghcount[1]++;
					life++;
					Dscore[0]++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
					judghname[i[0]][0] = 3;
					judghcount[2]++;
					life++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//fastmiss
				else if (judgh <= 125) {
					judghname[i[0]][0] = 4;
					combo = 0;
					judghcount[3]++;
					life -= 20;
				}
			}
			else if ((holdr == 1) && object[i[0]][1][objectN[i[0]]] == 6 && judgh <= 125 && judgh >= -100) {
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 6;
				objectN[i[0]]++;
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				//just
				if (judgh <= 40 && judgh >= -40) {
					judghname[i[0]][0] = 1;
					combo++;
					judghcount[0]++;
					life += 2;
					Dscore[0] += 2;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
					judghname[i[0]][0] = 2;
					combo++;
					judghcount[1]++;
					life++;
					Dscore[0]++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
					judghname[i[0]][0] = 3;
					judghcount[2]++;
					life++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//fastmiss
				else if (judgh <= 125) {
					judghname[i[0]][0] = 4;
					combo = 0;
					judghcount[3]++;
					life -= 20;
				}
			}
			//ボムノーツ
			else if (i[0] == charaput && object[i[0]][1][objectN[i[0]]] == 7 && judgh <= 0 && judgh >= -40) {
				judghname[i[0]][0] = 4;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 7;
				objectN[i[0]]++;
				combo = 0;
				judghcount[3]++;
				life -= 20;
				PlaySoundMem(bomb, DX_PLAYTYPE_BACK);
			}
			else if (object[i[0]][1][objectN[i[0]]] == 7 && judgh < -40) {
				judghname[i[0]][0] = 1;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 7;
				objectN[i[0]]++;
				combo++;
				judghcount[0]++;
				life += 2;
				Dscore[0] += 2;
			}
			//ゴーストノーツ
			else if (object[i[0]][1][objectN[i[0]]] == 8 && judgh < 16) objectN[i[0]]++;
			//全ノーツslowmiss
			else if (judgh <= -100 && judgh >= -100000 && object[i[0]][1][objectN[i[0]]] >= 1 && object[i[0]][1][objectN[i[0]]] <= 6) {
				judghname[i[0]][0] = 4;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 0;
				combo = 0;
				objectN[i[0]]++;
				judghcount[3]++;
				life -= 20;
			}
		}
		if (gapa[2] > 2140000000) {
			gapa[0] /= 2;
			gapa[1] /= 2;
			gapa[2] /= 2;
		}
		Mcombo = mins(Mcombo, combo);
		//判定+ヒットエフェクト表示
		for (i[0] = 0; i[0] < 3; i[0]++) {
			switch (judghname[i[0]][0]) {
			case(1):
				DrawString(Xline[i[0]] - 25, Yline[i[0]] - 25, L"JUST", Cr);
				break;
			case(2):
				DrawString(Xline[i[0]] - 25, Yline[i[0]] - 25, L"GOOD", Cr);
				break;
			case(3):
				DrawString(Xline[i[0]] - 25, Yline[i[0]] - 25, L"SAFE", Cr);
				break;
			case(4):
				DrawString(Xline[i[0]] - 25, Yline[i[0]] - 25, L"MISS", Cr);
				break;
			}
			if (judghname[i[0]][1] + 250 > GetNowCount() && judghname[i[0]][0] >= 1 && judghname[i[0]][0] <= 3 && judghname[i[0]][2] >= 1 && judghname[i[0]][2] <= 6)DrawGraph(Xline[i[0]] - 10, Yline[i[0]] - 10, effimg[judghname[i[0]][2] - 1][(GetNowCount() - judghname[i[0]][1] + 250) / 50 % 5], TRUE);
			else if (judghname[i[0]][1] + 250 > GetNowCount() && judghname[i[0]][0] == 4 && judghname[i[0]][2] == 7)DrawGraph(Xline[i[0]] - 10, Yline[i[0]] - 10, effimg[6][(GetNowCount() - judghname[i[0]][1] + 250) / 50 % 5], TRUE);
			if (judghname[i[0]][1] + 750 < GetNowCount()) {
				judghname[i[0]][0] = 0;
				judghname[i[0]][1] = 0;
				judghname[i[0]][2] = 0;
			}
		}
		//ライフが0未満の時、1毎に減点スコアを20増やす。
		if (life < 0) {
			score2[2] = maxs(score2[2] - life * 20, score2[0] + score2[1]);
			life = 0;
		}
		//スコアバー表示
		DrawGraph(0, 0, sbarimg, TRUE);
		//点数表示
		score2[0] = (judghcount[0] * 90000 + judghcount[1] * 85000 + judghcount[2] * 45000) / notes;
		score2[1] = Mcombo * 10000 / notes;
		DrawFormatString(490, 20, Cr, L"SCORE:%d", score2[0] + score2[1] - score2[2]);
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
		Dscore[3] = GD[0] * 1000 + Dscore[1];
		//スコアバー隠し表示
		DrawGraph(0, 0, sbbarimg, TRUE);
		//コンボ表示
		if (combo >= 1) DrawFormatString(15, 15, Cr, L"%d combo", combo);
		//部分難易度表示
		if (holdG >= 1) {
			G[0] = ddif[0] * 20 / notzero(ddifG[1]) + 155;
			G[1] = ddif[24] * 20 / notzero(ddifG[1]) + 447;
			for (i[0] = 0; i[0] <= 23; i[0]++) DrawLine((G[0] * (24 - i[0]) + G[1] * i[0]) / 24, -ddif[i[0]] * 34 / notzero(ddifG[1]) + 72, (G[0] * (23 - i[0]) + G[1] * (1 + i[0])) / 24, -ddif[i[0] + 1] * 34 / notzero(ddifG[1]) + 72, Cr);
			DrawFormatString(490, 80, Cr, L"mdif:%.2f", difkey[4][3] / 100.0);
			DrawFormatString(490, 100, Cr, L"ldif:%.2f", difkey[6][3] / 100.0);
			DrawFormatString(490, 120, Cr, L"mrat:%.2f", DifRate);
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
		if (Ntime != 0) DrawFormatString(20, 80, Cr, L"FPS: %.0f", 60000.0 / notzero(G[0]));
		//for (i[0] = 0; i[0] < 3; i[0]++) { DrawFormatString(20, 100 + i[0] * 20, Cr, L"%d,%d", object[i[0]][0][objectN[i[0]]], object[i[0]][1][objectN[i[0]]]); }
		//ライフが20%以下の時、危険信号(ピクチャ)を出す
		if (life <= 100 && drop == 0) DrawGraph(0, 0, dangerimg, TRUE);
		//ライフがなくなったらDROPED扱い
		if (life <= 0 && drop == 0) {
			drop = 1;
			Dscore[1] = Dscore[0];
			Dscore[2] = mins(Ntime - noteoff, 0);
		}
		if (drop) { DrawGraph(0, 0, dropimg, TRUE); }
		//終了時間から1秒以上たって、曲が終了したら抜ける。
		if (CheckSoundMem(musicmp3) == 0 && Etime <= Ntime + 5000) {
			StopSoundMem(musicmp3);
			DeleteSoundMem(musicmp3);
			break;
		}
		WaitTimer(5);
		Ntime = GetNowCount() - Stime + system[1] * 5;
		ScreenFlip();
	}
	int	read[7] = { 0,0,0,0,0,0,0 };
	double ReadAcc[7] = { 0,0,0,0,0,0,0 };
	int	Readdis[7] = { 0,0,0,0,0,0,0 };
	int	ReadRank[7] = { 6,6,6,6,6,6,6 };
	int	ReadClear[7] = { 0,0,0,0,0,0,0 };
	int chap[3] = { 0,0,0 };
	double readR[10] = { 0,0,0,0,0,0,0,0,0,0 };
	double acc = (judghcount[0] * 10000 + judghcount[1] * 9500 + judghcount[2] * 5500) / (notes*100.0);
	wchar_t savec[10][255];
	wchar_t save[255] = L"score/";
	wchar_t save2[255] = L".dat";
	score2[3] = score2[0] + score2[1] - score2[2];
	//ランク判定
	if (score2[3] >= 98000) rank = 0;
	else if (score2[3] >= 95000) rank = 1;
	else if (score2[3] >= 90000) rank = 2;
	else if (score2[3] >= 85000) rank = 3;
	else if (score2[3] >= 80000) rank = 4;
	else rank = 5;
	//クリアレート判定
	if (drop == 1) { Clear = 1; }
	else if (drop == 0 && judghcount[3] > 0) { Clear = 2; }
	else if (drop == 0 && judghcount[3] == 0 && judghcount[2] > 0) { Clear = 3; }
	else if (drop == 0 && judghcount[3] == 0 && judghcount[2] == 0 && judghcount[1] > 0) { Clear = 4; }
	else Clear = 5;
	strcats(save, fileN);
	strcats(save, save2);
	G[0] = _wfopen_s(&fp, save, L"rb"); //記録読み込み
	if (G[0] == 0) {
		fread(&read, sizeof(int), 6, fp);
		fread(&ReadAcc, sizeof(double), 6, fp);
		fread(&Readdis, sizeof(int), 6, fp);
		fread(&ReadRank, sizeof(int), 6, fp);
		fread(&ReadClear, sizeof(int), 6, fp);
		fclose(fp);
	}
	G[9] = 0;
	if (read[o] < score2[3]) { read[o] = score2[3]; } //ハイスコア保存
	if (ReadAcc[o] < acc) { ReadAcc[o] = acc; } //ACC保存
	if (Readdis[o] < Dscore[3]) { Readdis[o] = Dscore[3]; } //最長走行距離保存
	if (ReadRank[o] > rank || ReadRank[o] < 0) { ReadRank[o] = rank; } //ランク保存
	if (ReadClear[o] < Clear) { ReadClear[o] = Clear; } //クリアレート保存
	G[0] = _wfopen_s(&fp, save, L"wb");
	fwrite(&read, sizeof(int), 6, fp);
	fwrite(&ReadAcc, sizeof(double), 6, fp);
	fwrite(&Readdis, sizeof(int), 6, fp);
	fwrite(&ReadRank, sizeof(int), 6, fp);
	fwrite(&ReadClear, sizeof(int), 6, fp);
	fclose(fp);
	//プレイ回数保存
	G[0] = _wfopen_s(&fp, L"save/data.dat", L"rb");
	if (G[0] == 0) {
		fread(&read, sizeof(read), 7, fp);
		fclose(fp);
	}
	read[0]++;
	if (drop == 1) read[1]++;
	else read[3]++;
	if (judghcount[3] == 0)read[4]++;
	if (judghcount[2] == 0 && judghcount[3] == 0)read[5]++;
	if (judghcount[1] == 0 && judghcount[2] == 0 && judghcount[3] == 0)read[6]++;
	G[0] = _wfopen_s(&fp, L"save/data.dat", L"wb");
	fwrite(&read, sizeof(int), 7, fp);
	fclose(fp);
	//キャラプレイ数保存
	G[0] = _wfopen_s(&fp, L"save/chap.dat", L"rb");
	if (G[0] == 0) {
		fread(&chap, sizeof(chap), 3, fp);
		fclose(fp);
	}
	chap[system[0]]++;
	G[0] = _wfopen_s(&fp, L"save/chap.dat", L"wb");
	fwrite(&chap, sizeof(int), 3, fp);
	fclose(fp);
	//レート計算(level0なら0固定)"譜面定数" - "miss数" x "譜面定数" x 0.03(下限=0)
	if (DifRate == 0) rate = 0;
	else if (judghcount[3] > 0) {
		rate = DifRate - judghcount[3] * DifRate*0.03;
		rate = mins_D(rate, 0);
	}
	//NO MISS,"譜面定数" + 1 - "safe数" x 0.05(下限="譜面定数")
	else if (judghcount[3] == 0 && judghcount[2] > 0) {
		rate = DifRate + 1 - judghcount[2] * 0.05;
		rate = mins_D(rate, DifRate);
	}
	//FULL COMBO,"譜面定数" + 2 - "good数" x 0.01(下限="譜面定数" + 1)
	else if (judghcount[3] == 0 && judghcount[2] == 0 && judghcount[1] > 0) {
		rate = DifRate + 2 - judghcount[1] * 0.01;
		rate = mins_D(rate, DifRate + 1);
	}
	//PERFECT, "譜面定数" + 2
	else if (judghcount[3] == 0 && judghcount[2] == 0 && judghcount[1] == 0) rate = DifRate + 2;
	//レート保存
	G[0] = _wfopen_s(&fp, L"save/rateS.dat", L"rb");
	if (G[0] == 0) {
		fread(&savec, 255, 10, fp);
		fclose(fp);
	}
	G[0] = _wfopen_s(&fp, L"save/rateN.dat", L"rb");
	if (G[0] == 0) {
		fread(&readR, sizeof(readR), 10, fp);
		fclose(fp);
	}
	for (i[0] = 0; i[0] < 10; i[0]++) { SumRate[0] += mins_D(readR[i[0]], 0); } //プレイ前のレートを計算
	SumRate[1] = SumRate[0];
	G[0] = -1;
	//同じ曲、または未収録を探す
	for (i[0] = 0; i[0] < 10; i[0]++)if (strands(fileN, savec[i[0]]) || savec[i[0]] == '\0') {
		G[0] = i[0];
		break;
	}
	//なかったら、一番低いレートを探す
	if (G[0] == -1) {
		G[0] = 0;
		for (i[0] = 1; i[0] < 10; i[0]++)if (readR[G[0]] > readR[i[0]]) G[0] = i[0];
	}
	//レートが高かったら更新する
	if (readR[G[0]] < rate) {
		readR[G[0]] = rate;
		SumRate[1] = 0;
		strcopy(fileN, savec[G[0]], 1);
		for (i[0] = 0; i[0] < 10; i[0]++) { SumRate[1] += mins_D(readR[i[0]], 0); } //変化後のレートを計算
		G[0] = _wfopen_s(&fp, L"save/rateS.dat", L"wb");
		fwrite(&savec, 255, 10, fp);
		fclose(fp);
		G[0] = _wfopen_s(&fp, L"save/rateN.dat", L"wb");
		fwrite(&readR, sizeof(double), 10, fp);
		fclose(fp);
	}
	//リザルト画面
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, resultimg, TRUE);
		DrawGraph(460, 20, difberimg, TRUE);
		DrawString(100, 13, songN, Cr);
		DrawCurFont(judghcount[0], 140, 52, 30, 4);
		DrawCurFont(judghcount[1], 140, 93, 30, 2);
		DrawCurFont(judghcount[2], 140, 134, 30, 3);
		DrawCurFont(judghcount[3], 140, 175, 30, 1);
		DrawCurFont(Mcombo, 155, 215, 30, 4);
		DrawCurFont(notes, 265, 215, 30, 5);
		DrawFormatString(10, 320, Cr, L"%.2f", SumRate[1]);
		if (SumRate[1] != SumRate[0]) { DrawFormatString(10, 340, Cr, L"+%.2f", SumRate[1] - SumRate[0]); }
		else { DrawString(10, 340, L"not rise", Cr); }
		switch (rank) {
		case 0:
			DrawCurFont(score2[3], 310, 75, 55, 6);
			DrawCurFont(acc, 430, 150, 30, 6, 2);
			break;
		case 1:
			DrawCurFont(score2[3], 310, 75, 55, 4);
			DrawCurFont(acc, 430, 150, 30, 4, 2);
			break;
		case 2:
			DrawCurFont(score2[3], 310, 75, 55, 2);
			DrawCurFont(acc, 430, 150, 30, 2, 2);
			break;
		case 3:
			DrawCurFont(score2[3], 310, 75, 55, 3);
			DrawCurFont(acc, 430, 150, 30, 3, 2);
			break;
		case 4:
			DrawCurFont(score2[3], 310, 75, 55, 5);
			DrawCurFont(acc, 430, 150, 30, 5, 2);
			break;
		case 5:
			DrawCurFont(score2[3], 310, 75, 55, 1);
			DrawCurFont(acc, 430, 150, 30, 1, 2);
			break;
		}
		if (gapa[1] == 0) gapa[1] = 1;
		DrawCurFont(gapa[0] / gapa[1], 510, 205, 20, 0);
		DrawCurFont(gapa[2] / gapa[1] - gapa[0] * gapa[0] / gapa[1] / gapa[1], 500, 230, 20, 0);
		DrawGraph(140, 260, rankimg[rank], TRUE);
		DrawGraph(5, 420, coleimg[Clear - 1], TRUE);
		DrawGraph(336, 252, Rchaimg, TRUE);
		ScreenFlip();
		//エンターが押された
		if (CheckHitKey(KEY_INPUT_RETURN) == 1) break;
		else if (GetWindowUserCloseFlag(TRUE)) return 5;
	}
	InitGraph();
	return 2;
}