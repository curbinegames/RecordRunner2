#include "playing.h"
#include "playbox.h"

int IsNoteCode(wchar_t c);

void RecordLoad2(int p, int n, int o) {
	//n: 曲ナンバー
	//o: 難易度ナンバー
	short int i[2];
	short int Lv = 0;
	short int notes = 0;
	int G[10], songT;
	int noteoff = 0; //ノーツのオフセット
	int Etime = 0; //譜面の終わりの時間
	int songdata = 0;
	double GD[5];
	//int item[99]; //アイテムのアドレス、DrawGraphで呼べる。
	//short int itemN = 0; //↑の番号
	int chamo[3][99][2]; //キャラの[0:上,1:中,2:下]の[0:グラフィック,1:切り替え時間]
	chamo[0][0][0] = 0;
	chamo[0][0][1] = 0;
	chamo[1][0][0] = 1;
	chamo[1][0][1] = 0;
	chamo[2][0][0] = 1;
	chamo[2][0][1] = 0;
	short int chamoN[3] = { 1,1,1 }; //↑の番号
	int fall[99][2]; //落ち物背景の[0:番号,1:時間]
	fall[0][0] = 0;
	fall[0][1] = -1;
	short int fallN = 1; //↑の番号
	int Ymove[5][999][4]; //[上, 中, 下, (地面), (水中)]レーン縦移動の[0:開始時間,1:位置,2:終了時間,3:種類]
	Ymove[0][0][0] = 0;
	Ymove[0][0][1] = 300;
	Ymove[0][0][2] = 0;
	Ymove[0][0][3] = 1;
	Ymove[1][0][0] = 0;
	Ymove[1][0][1] = 350;
	Ymove[1][0][2] = 0;
	Ymove[1][0][3] = 1;
	Ymove[2][0][0] = 0;
	Ymove[2][0][1] = 400;
	Ymove[2][0][2] = 0;
	Ymove[2][0][3] = 1;
	Ymove[3][0][0] = 0;
	Ymove[3][0][1] = 350;
	Ymove[3][0][2] = 0;
	Ymove[3][0][3] = 1;
	Ymove[4][0][0] = 0;
	Ymove[4][0][1] = 600;
	Ymove[4][0][2] = 0;
	Ymove[4][0][3] = 1;
	short int YmoveN[5] = { 1,1,1,1,1 }; //↑の番号
	short int YmoveN2[3] = { 0,0,0 };
	int Xmove[3][999][4]; //[上, 中, 下]レーン横移動の[0:開始時間,1:位置,2:終了時間,3:種類]
	Xmove[0][0][0] = 0;
	Xmove[0][0][1] = 150;
	Xmove[0][0][2] = 0;
	Xmove[0][0][3] = 1;
	Xmove[1][0][0] = 0;
	Xmove[1][0][1] = 150;
	Xmove[1][0][2] = 0;
	Xmove[1][0][3] = 1;
	Xmove[2][0][0] = 0;
	Xmove[2][0][1] = 150;
	Xmove[2][0][2] = 0;
	Xmove[2][0][3] = 1;
	short int XmoveN[3] = { 1,1,1 }; //↑の番号
	short int XmoveN2[3] = { 0,0,0 };
	int lock[2][2][99]; //lock = [横,縦]の音符の位置を[(1=固定する,-1以外=固定しない),時間]
	lock[0][0][0] = -1;
	lock[0][1][0] = 0;
	lock[1][0][0] = 1;
	lock[1][1][0] = 0;
	short int lockN[2] = { 1,1 }; //↑の番号
	int carrow[2][99];
	carrow[0][0] = 1;
	carrow[1][0] = 0;
	short int carrowN = 1;
	int viewT[2][99];//[音符表示時間,実行時間,[0]=現ナンバー]
	viewT[0][0] = 0;
	viewT[1][0] = 3000;
	short int viewTN = 1;
	int Movie[14][999];//アイテム表示[アイテム番号,移動形態,開始時間,終了時間,開始x位置,終了x位置,開始y位置,終了y位置,開始サイズ,終了サイズ,開始角度,終了角度,開始透明度,終了透明度]
	short int MovieN = 0;
	struct camera_box camera[255];
	camera[0].starttime = 0;
	camera[0].endtime = 0;
	camera[0].xpos = 0;
	camera[0].ypos = 0;
	camera[0].zoom = 1;
	camera[0].rot = 0;
	camera[0].mode = 0;
	short int cameraN = 1; //↑の番号
	struct custom_note_box customnote[9];
	struct scrool_box scrool[99];
	scrool[0].starttime = 0;
	scrool[0].basetime = 0;
	scrool[0].speed = 1;
	short int scroolN = 1;
	struct note_box note[3][999];//[上,中,下]レーンのノーツ[番号]
	note[0][0].ypos = 300;
	note[1][0].ypos = 350;
	note[2][0].ypos = 400;
	short int objectN[3] = { 0,0,0 }; //↑の番号
	int difkey[50][4];//難易度計算に使う[番号][入力キー,時間,難易度点,[0]個数上限:[1]今の番号:[2]1個前の番号:[3]2個前の番号:[4]最高点:[5]データ個数:[6]最後50個の合計:[7]計算から除外する時間]
	difkey[0][2] = 0;
	difkey[1][2] = 0;
	difkey[1][3] = 0;
	difkey[2][3] = -1;
	difkey[3][3] = -2;
	difkey[4][3] = 0;
	difkey[5][3] = -1;
	int ddif[25] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//各区間の難易度
	int ddifG[2] = { 1,1 };//0=今いる区間番号(1〜25),1=最大値
	double bpm = 120, bpmG = 120;
	double timer[3]; //[上, 中, 下]レーンの時間
	double speedt[5][99][2]; //[上, 中, 下, (地面), (水中)]レーンの[0:切り替え時間,1:速度]
	speedt[0][0][0] = 0;
	speedt[0][0][1] = 1;
	speedt[1][0][0] = 0;
	speedt[1][0][1] = 1;
	speedt[2][0][0] = 0;
	speedt[2][0][1] = 1;
	speedt[3][0][0] = 0;
	speedt[3][0][1] = 1;
	speedt[4][0][0] = 0;
	speedt[4][0][1] = 1;
	short int speedN[5] = { 1,1,1,1,1 }; //↑の番号
	wchar_t songN[255];
	wchar_t songNE[255];
	songN[0] = L'\0';
	wchar_t fileN[255];
	wchar_t dataE[255] = L"record/";//フォルダの名前
	wchar_t RRS[255]; //PC用譜面データの保存場所
	wchar_t mp3FN[255];
	wchar_t skyFN[255] = L"picture/backskynoamal.png";
	wchar_t groundFN[255] = L"picture/groundnaturenormal.png";
	wchar_t waterFN[255] = L"picture/waternormal.png";
	wchar_t DifFN[255] = L"picture/difanother.png";
	wchar_t GT1[255];
	wchar_t GT2[255];
	wchar_t GT24[] = L"picture/";
	wchar_t GT25[6][7] = { L"/0.txt" ,L"/1.txt" ,L"/2.txt" ,L"/3.txt" ,L"/4.txt" ,L"/5.txt" };
	wchar_t GT26[6][7] = { L"/0.rrs" ,L"/1.rrs" ,L"/2.rrs" ,L"/3.rrs" ,L"/4.rrs" ,L"/5.rrs" };
	wchar_t ST1[] = L"record/";
	wchar_t ST2[] = L"list.txt";
	FILE *fp;
	/*
	songT = FileRead_open(L"song.txt");
	for (i[0] = 0; i[0] <= n; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcopy(GT1, fileN, 1);
	strcats(dataE, GT1);
	strcopy(dataE, GT1, 1);
	strcats(GT1, GT25[o]);
	*/

	songT = FileRead_open(L"RecordPack.txt");
	for (i[0] = 0; i[0] <= p; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcats(dataE, GT1); //"record/<パック名>"
	stradds(dataE, L'/'); //"record/<パック名>/"
	strcopy(dataE, GT2, 1);
	strcats(GT2, ST2); //"record/<パック名>/list.txt"
	songT = FileRead_open(GT2);
	for (i[0] = 0; i[0] <= n; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcopy(GT1, fileN, 1); //"<曲名>"
	strcats(dataE, GT1); //"record/<パック名>/<曲名>"
	strcopy(dataE, GT1, 1);
	strcats(GT1, GT25[o]); //"record/<パック名>/<曲名>/<難易度>.txt"

	songdata = FileRead_open(GT1);
	//テキストデータを読む
	while (FileRead_eof(songdata) == 0) {
		FileRead_gets(GT1, 256, songdata);
		//音楽ファイルを読み込む
		if (strands(GT1, L"#MUSIC:")) {
			strmods(GT1, 7);
			strcopy(dataE, mp3FN, 1);
			strcats(mp3FN, L"/");
			strcats(mp3FN, GT1);
		}
		//BPMを読み込む
		else if (strands(GT1, L"#BPM:")) bpmG = bpm = SETbpm(GT1);
		//ノートのオフセットを読み込む
		else if (strands(GT1, L"#NOTEOFFSET:")) timer[0] = timer[1] = timer[2] = noteoff = SEToffset(GT1);
		//空の背景を読み込む
		else if (strands(GT1, L"#SKY:")) {
			strcopy(GT24, skyFN, 1);
			strmods(GT1, 5);
			strcats(skyFN, GT1);
		}
		//地面の画像を読み込む
		else if (strands(GT1, L"#FIELD:")) {
			strcopy(GT24, groundFN, 1);
			strmods(GT1, 7);
			strcats(groundFN, GT1);
		}
		//水中の画像を読み込む
		else if (strands(GT1, L"#WATER:")) {
			strcopy(GT24, waterFN, 1);
			strmods(GT1, 7);
			strcats(waterFN, GT1);
		}
		//難易度バー(another)を読み込む
		else if (strands(GT1, L"#DIFBAR:")) {
			strcopy(dataE, DifFN, 1);
			strmods(GT1, 8);
			stradds(DifFN, L'/');
			strcats(DifFN, GT1);
		}
		//曲名を読み込む
		else if (strands(GT1, L"#TITLE:")) {
			strmods(GT1, 7);
			strcopy(GT1, songN, 1);
		}
		//英語
		else if (strands(GT1, L"#E.TITLE:")) {
			strmods(GT1, 7);
			strcopy(GT1, songNE, 1);
		}
		//レベルを読み込む
		else if (strands(GT1, L"#LEVEL:")) Lv = SETLv(GT1);
		//アイテムを読み込む
		/*else if (strands(GT1, L"#ITEM:")) {
			strmods(GT1, 6);
			strcopy(dataE, GT15, 1);
			stradds(GT15, L'/');
			strcats(GT15, GT1);
			item[itemN++] = LoadGraph(GT15);
		}*/
		//落ち物背景指定
		else if (strands(GT1, L"#FALL:")) {
			strmods(GT1, 6);
			fall[0][0] = strsans(GT1);
			fall[0][1] = 0;
		}
		//譜面を読み込む
		else if (strands(GT1, L"#MAP:")) {
			while (FileRead_eof(songdata) == 0 && strands(GT1, L"#END") == 0) {
				FileRead_gets(GT1, 256, songdata);
				//横ノーツ速度変化
				if (strands(GT1, L"#SPEED")) {
					G[0] = maxs(mins(GT1[6] - 49, 0), 4);
					strmods(GT1, 8);
					speedt[G[0]][speedN[G[0]]][1] = strsans2(GT1);
					strnex(GT1);
					if (GT1[0] >= L'0' && GT1[0] <= L'9' || GT1[0] == L'-') {
						speedt[G[0]][speedN[G[0]]][0] = timer[G[0]] + 240000 * (speedt[G[0]][speedN[G[0]]][1] - 1) / (bpm * 16) - 10;
						speedt[G[0]][speedN[G[0]]][1] = strsans2(GT1);
					}
					else {
						speedt[G[0]][speedN[G[0]]][0] = timer[G[0]] - 10;
					}
					speedN[G[0]]++;
				}
				//BPM変化
				else if (strands(GT1, L"#BPM:")) bpmG = SETbpm(GT1);
				//キャラグラ変化
				else if (strands(GT1, L"#CHARA")) {
					G[0] = GT1[6] - 49;
					strmods(GT1, 8);
					chamo[G[0]][chamoN[G[0]]][0] = maxs(mins(strsans(GT1), 0), 2);
					chamo[G[0]][chamoN[G[0]]][1] = timer[G[0]];
					chamoN[G[0]]++;
				}
				//縦移動
				else if (strands(GT1, L"#MOVE")) {
					if (GT1[8] == L'A') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 0;
					}
					else if (GT1[8] == L'B') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 1;
					}
					else if (GT1[8] == L'C') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 2;
					}
					else if (GT1[8] == L'D') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 3;
					}
					else {
						G[2] = G[0] = maxs(mins(GT1[8] - 49, 0), 4);
						G[3] = 0;
					}
					switch (GT1[5]) {
					case('l'):
						G[1] = 1;
						break;
					case('a'):
						G[1] = 2;
						break;
					case('d'):
						G[1] = 3;
						break;
					case('m'):
						G[1] = 4;
						break;
					case('s'):
						G[1] = 5;
						break;
					case('p'):
						G[1] = 6;
						break;
					case('e'):
						G[1] = 7;
						break;
					}
					strmods(GT1, 10);
					GD[0] = strsans2(GT1);
					strnex(GT1);
					GD[1] = strsans2(GT1);
					strnex(GT1);
					GD[2] = strsans2(GT1);
					for (i[0] = G[0]; i[0] <= G[2]; i[0]++) {
						switch (G[1]) {
						case 1:
						case 2:
						case 3:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3], G[1], GD[2], bpmG, &Ymove[i[0]][YmoveN[i[0]]][0], &Ymove[i[0]][YmoveN[i[0]]][1], &Ymove[i[0]][YmoveN[i[0]]][2], &Ymove[i[0]][YmoveN[i[0]]][3]);
							break;
						case 4:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3], 1, GD[2], bpmG, &Ymove[i[0]][YmoveN[i[0]]][0], &Ymove[i[0]][YmoveN[i[0]]][1], &Ymove[i[0]][YmoveN[i[0]]][2], &Ymove[i[0]][YmoveN[i[0]]][3]);
							Ymove[i[0]][YmoveN[i[0]]][0] -= 5;
							Ymove[i[0]][YmoveN[i[0]]][2] -= 5;
							break;
						case 5:
							SETMove(timer[0], GD[0], (Ymove[i[0]][YmoveN[i[0]] - 1][1] + (GD[1] + G[3] * i[0] - G[3]) * 50 - 100) / 100, 2, (GD[2] + GD[0]) / 2.0, bpmG, &Ymove[i[0]][YmoveN[i[0]]][0], &Ymove[i[0]][YmoveN[i[0]]][1], &Ymove[i[0]][YmoveN[i[0]]][2], &Ymove[i[0]][YmoveN[i[0]]][3]);
							YmoveN[i[0]]++;
							SETMove(timer[0], (GD[2] + GD[0]) / 2.0, GD[1] + G[3] * i[0] - G[3], 3, GD[2], bpmG, &Ymove[i[0]][YmoveN[i[0]]][0], &Ymove[i[0]][YmoveN[i[0]]][1], &Ymove[i[0]][YmoveN[i[0]]][2], &Ymove[i[0]][YmoveN[i[0]]][3]);
							break;
						case 6:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3], 3, (GD[2] + GD[0]) / 2.0, bpmG, &Ymove[i[0]][YmoveN[i[0]]][0], &Ymove[i[0]][YmoveN[i[0]]][1], &Ymove[i[0]][YmoveN[i[0]]][2], &Ymove[i[0]][YmoveN[i[0]]][3]);
							YmoveN[i[0]]++;
							SETMove(timer[0], (GD[2] + GD[0]) / 2.0, (Ymove[i[0]][YmoveN[i[0]] - 2][1] - 100.0) / 50.0, 2, GD[2], bpmG, &Ymove[i[0]][YmoveN[i[0]]][0], &Ymove[i[0]][YmoveN[i[0]]][1], &Ymove[i[0]][YmoveN[i[0]]][2], &Ymove[i[0]][YmoveN[i[0]]][3]);
							break;
						case 7:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3], 2, (GD[2] + GD[0]) / 2.0, bpmG, &Ymove[i[0]][YmoveN[i[0]]][0], &Ymove[i[0]][YmoveN[i[0]]][1], &Ymove[i[0]][YmoveN[i[0]]][2], &Ymove[i[0]][YmoveN[i[0]]][3]);
							YmoveN[i[0]]++;
							SETMove(timer[0], (GD[2] + GD[0]) / 2.0, (Ymove[i[0]][YmoveN[i[0]] - 2][1] - 100.0) / 50.0, 3, GD[2], bpmG, &Ymove[i[0]][YmoveN[i[0]]][0], &Ymove[i[0]][YmoveN[i[0]]][1], &Ymove[i[0]][YmoveN[i[0]]][2], &Ymove[i[0]][YmoveN[i[0]]][3]);
							break;
						}
						YmoveN[i[0]]++;
					}
				}
				//横移動
				else if (strands(GT1, L"#XMOV")) {
					if (GT1[8] == L'A') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 0;
					}
					else if (GT1[8] == L'B') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 1;
					}
					else if (GT1[8] == L'C') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 2;
					}
					else if (GT1[8] == L'D') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 3;
					}
					else {
						G[2] = G[0] = maxs(mins(GT1[8] - 49, 0), 2);
						G[3] = 0;
					}
					switch (GT1[5]) {
					case('l'):
						G[1] = 1;
						break;
					case('a'):
						G[1] = 2;
						break;
					case('d'):
						G[1] = 3;
						break;
					case('m'):
						G[1] = 4;
						break;
					case('s'):
						G[1] = 5;
						break;
					case('p'):
						G[1] = 6;
						break;
					case('e'):
						G[1] = 7;
						break;
					}
					strmods(GT1, 10);
					GD[0] = strsans2(GT1);
					strnex(GT1);
					GD[1] = strsans2(GT1);
					strnex(GT1);
					GD[2] = strsans2(GT1);
					for (i[0] = G[0]; i[0] <= G[2]; i[0]++) {
						switch (G[1]) {
						case 1:
						case 2:
						case 3:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3], G[1], GD[2], bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							break;
						case 4:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3], 1, GD[0], bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							Xmove[i[0]][XmoveN[i[0]]][0] -= 5;
							Xmove[i[0]][XmoveN[i[0]]][2] -= 5;
							break;
						case 5:
							SETMove(timer[0], GD[0], (Xmove[i[0]][XmoveN[i[0]] - 1][1] + GD[1] * 50 - 100) / 100, 2, (GD[2] + GD[0]) / 2.0, bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							XmoveN[i[0]]++;
							SETMove(timer[0], (GD[2] + GD[0]) / 2.0, GD[1] + G[3] * i[0] - G[3], 3, GD[2], bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							break;
						case 6:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3], 3, (GD[2] + GD[0]) / 2.0, bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							XmoveN[i[0]]++;
							SETMove(timer[0], (GD[2] + GD[0]) / 2.0, (Xmove[i[0]][XmoveN[i[0]] - 2][1] - 100.0) / 50.0, 2, GD[2], bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							break;
						case 7:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3], 2, (GD[2] + GD[0]) / 2.0, bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							XmoveN[i[0]]++;
							SETMove(timer[0], (GD[2] + GD[0]) / 2.0, (Xmove[i[0]][XmoveN[i[0]] - 2][1] - 100.0) / 50.0, 3, GD[2], bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							break;
						}
						XmoveN[i[0]]++;
					}
				}
				//振動
				else if (strands(GT1, L"#DIV")) {
					G[0] = betweens(0, GT1[5] - 49, 2);
					G[1] = 0;
					if (GT1[4] == L'Y') { G[1] = 1; }
					strmods(GT1, 7);
					GD[0] = strsans2(GT1);//開始時間
					strnex(GT1);
					GD[1] = strsans2(GT1);//振動位置
					strnex(GT1);
					GD[2] = strsans2(GT1) / 2.0;//往復時間
					strnex(GT1);
					GD[3] = strsans2(GT1);//往復回数
					if (G[1] == 1) {
						for (i[0] = 0; i[0] < GD[3]; i[0]++) {
							SETMove(timer[0], GD[0], GD[1], 1, GD[0] + GD[2], bpmG, &Ymove[G[0]][YmoveN[G[0]]][0], &Ymove[G[0]][YmoveN[G[0]]][1], &Ymove[G[0]][YmoveN[G[0]]][2], &Ymove[G[0]][YmoveN[G[0]]][3]);
							SETMove(timer[0], GD[0] + GD[2], (Ymove[G[0]][YmoveN[G[0]] - 1][1] - 100.0) / 50.0, 1, GD[0] + GD[2] * 2, bpmG, &Ymove[G[0]][YmoveN[G[0]] + 1][0], &Ymove[G[0]][YmoveN[G[0]] + 1][1], &Ymove[G[0]][YmoveN[G[0]] + 1][2], &Ymove[G[0]][YmoveN[G[0]] + 1][3]);
							GD[0] += GD[2] * 2;
							YmoveN[G[0]] += 2;
						}
					}
					else {
						for (i[0] = 0; i[0] < GD[3]; i[0]++) {
							SETMove(timer[0], GD[0], GD[1], 1, GD[0] + GD[2], bpmG, &Xmove[G[0]][XmoveN[G[0]]][0], &Xmove[G[0]][XmoveN[G[0]]][1], &Xmove[G[0]][XmoveN[G[0]]][2], &Xmove[G[0]][XmoveN[G[0]]][3]);
							SETMove(timer[0], GD[0] + GD[2], (Xmove[G[0]][XmoveN[G[0]] - 1][1] - 100.0) / 50.0, 1, GD[0] + GD[2] * 2, bpmG, &Xmove[G[0]][XmoveN[G[0]] + 1][0], &Xmove[G[0]][XmoveN[G[0]] + 1][1], &Xmove[G[0]][XmoveN[G[0]] + 1][2], &Xmove[G[0]][XmoveN[G[0]] + 1][3]);
							GD[0] += GD[2] * 2;
							XmoveN[G[0]] += 2;
						}
					}
				}
				//GMOVE
				else if (strands(GT1, L"#GMOVE")) {
					G[0] = 3;
					switch (GT1[6]) {
					case('l'):
						G[1] = 1;
						break;
					case('a'):
						G[1] = 2;
						break;
					case('d'):
						G[1] = 3;
						break;
					case('m'):
						G[1] = 4;
						break;
					case('s'):
						G[1] = 5;
						break;
					case('p'):
						G[1] = 6;
						break;
					}
					strmods(GT1, 10);
					GD[0] = strsans2(GT1);
					strnex(GT1);
					GD[1] = strsans2(GT1);
					strnex(GT1);
					GD[2] = strsans2(GT1);
					switch (G[1]) {
					case 1:
					case 2:
					case 3:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]]][3] = G[1];
						break;
					case 4:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime(GD[2], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 1;
						break;
					case 5:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = (Ymove[G[0]][YmoveN[G[0]] - 1][1] + GD[1] * 50 + 100) / 2;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 2;
						Ymove[G[0]][YmoveN[G[0]] + 1][0] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]] + 1][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]] + 1][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]] + 1][3] = 3;
						YmoveN[G[0]]++;
						break;
					case 6:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 3;
						Ymove[G[0]][YmoveN[G[0]] + 1][0] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]] + 1][1] = Ymove[G[0]][YmoveN[G[0]] - 1][1];
						Ymove[G[0]][YmoveN[G[0]] + 1][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]] + 1][3] = 2;
						YmoveN[G[0]]++;
						break;
					}
					YmoveN[G[0]]++;
				}
				//横ロック
				else if (strands(GT1, L"#XLOCK")) {
					strmods(GT1, 7);
					lock[0][0][lockN[0]] = lock[0][0][lockN[0] - 1] * -1;
					lock[0][1][lockN[0]] = shifttime(strsans(GT1), bpmG, timer[0]);
					lockN[0]++;
				}
				//縦ロック
				else if (strands(GT1, L"#YLOCK")) {
					strmods(GT1, 7);
					lock[1][0][lockN[1]] = lock[1][0][lockN[1] - 1] * -1;
					lock[1][1][lockN[1]] = shifttime(strsans(GT1), bpmG, timer[0]);
					lockN[1]++;
				}
				//キャラ向き変化
				else if (strands(GT1, L"#CARROW")) {
					strmods(GT1, 8);
					carrow[0][carrowN] = carrow[0][carrowN - 1] * -1;
					carrow[1][carrowN] = shifttime(strsans(GT1), bpmG, timer[0]);
					carrowN++;
				}
				//落ち物背景切り替え
				else if (strands(GT1, L"#FALL")) {
					strmods(GT1, 6);
					fall[fallN][0] = strsans(GT1);
					strnex(GT1);
					fall[fallN][1] = strsans(GT1);
					fallN++;
				}
				//音符表示時間
				else if (strands(GT1, L"#VIEW:")) {
					strmods(GT1, 6);
					viewT[0][viewTN] = shifttime(strsans(GT1), bpmG, timer[0]);
					strnex(GT1);
					viewT[1][viewTN] = strsans(GT1);
					viewTN++;
				}
				//アイテム表示
				else if (strands(GT1, L"#MOVIE:")) {
					strmods(GT1, 7);
					Movie[0][MovieN] = strsans(GT1);
					strnex(GT1);
					switch (GT1[0]) {
					case L'l':
						Movie[1][MovieN] = 1;
						break;
					case L'a':
						Movie[1][MovieN] = 2;
						break;
					case L'd':
						Movie[1][MovieN] = 3;
						break;
					}
					strnex(GT1);
					Movie[2][MovieN] = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					Movie[3][MovieN] = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					Movie[4][MovieN] = strsans2(GT1) * 50 + 115;
					strnex(GT1);
					Movie[5][MovieN] = strsans2(GT1) * 50 + 115;
					strnex(GT1);
					Movie[6][MovieN] = strsans2(GT1) * 50 + 115;
					strnex(GT1);
					Movie[7][MovieN] = strsans2(GT1) * 50 + 115;
					strnex(GT1);
					Movie[8][MovieN] = strsans2(GT1) * 100;
					strnex(GT1);
					Movie[9][MovieN] = strsans2(GT1) * 100;
					strnex(GT1);
					Movie[10][MovieN] = strsans(GT1);
					strnex(GT1);
					Movie[11][MovieN] = strsans(GT1);
					strnex(GT1);
					Movie[12][MovieN] = strsans2(GT1)*255.0;
					strnex(GT1);
					Movie[13][MovieN] = strsans2(GT1)*255.0;
					strnex(GT1);
					MovieN++;
				}
				//カメラ移動+ズーム+角度(未実装)
				else if (strands(GT1, L"#CAMERA:")) {
					strmods(GT1, 8);
					camera[cameraN].starttime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					camera[cameraN].endtime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					camera[cameraN].xpos = strsans2(GT1) * 50;
					strnex(GT1);
					camera[cameraN].ypos = strsans2(GT1) * 50;
					strnex(GT1);
					camera[cameraN].zoom = strsans2(GT1);
					strnex(GT1);
					camera[cameraN].rot = strsans2(GT1);
					strnex(GT1);
					switch (GT1[0]) {
					case L'a':
						camera[cameraN].mode = 2;
						break;
					case L'd':
						camera[cameraN].mode = 3;
						break;
					default:
						camera[cameraN].mode = 1;
						break;
					}
					cameraN++;
				}
				//カメラ移動
				else if (strands(GT1, L"#CAMMOVE:")) {
					strmods(GT1, 9);
					camera[cameraN].starttime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					camera[cameraN].endtime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					camera[cameraN].xpos = strsans2(GT1) * 50;
					strnex(GT1);
					camera[cameraN].ypos = strsans2(GT1) * 50;
					strnex(GT1);
					switch (GT1[0]) {
					case L'a':
						camera[cameraN].mode = 2;
						break;
					case L'd':
						camera[cameraN].mode = 3;
						break;
					default:
						camera[cameraN].mode = 1;
						break;
					}
					cameraN++;
				}
				//スクロール
				else if (strands(GT1, L"#SCROOL:")) {
					strmods(GT1, 8);
					scrool[scroolN].starttime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					scrool[scroolN].speed = strsans2(GT1);
					G[0]= scrool[scroolN - 1].speed * scrool[scroolN].starttime + scrool[scroolN - 1].basetime;
					scrool[scroolN].basetime = G[0] - scrool[scroolN].speed * scrool[scroolN].starttime;
					scroolN++;
				}
				//カスタムノーツセット
				else if (strands(GT1, L"#CUSTOM:")) {
					strmods(GT1, 8);
					G[0] = strsans2(GT1) - 1;
					customnote[G[0]].color = 0;
					customnote[G[0]].melody = MELODYSOUND_NONE;
					customnote[G[0]].note = 0;
					customnote[G[0]].sound = 0;
					strnex(GT1);
					while (GT1[0] != L'\0') {
						if (strands(GT1, L"NOTE=")) {
							strmods(GT1, 5);
							customnote[G[0]].note = GT1[0];
						}
						else if (strands(GT1, L"SOUND=")) {
							strmods(GT1, 6);
							if (GT1[0] == L'L' || GT1[0] == L'H') {
								switch (GT1[1]) {
								case L'F':
									customnote[G[0]].melody = (enum melodysound)(LOW_F + (GT1[0] == L'H' ? 12 : 0) + (GT1[2] == L'#' ? 1 : 0));
									break;
								case L'G':
									customnote[G[0]].melody = (enum melodysound)(LOW_G + (GT1[0] == L'H' ? 12 : 0) + (GT1[2] == L'#' ? 1 : 0));
									break;
								case L'A':
									customnote[G[0]].melody = (enum melodysound)(LOW_A + (GT1[0] == L'H' ? 12 : 0) + (GT1[2] == L'#' ? 1 : 0));
									break;
								case L'B':
									customnote[G[0]].melody = (enum melodysound)(LOW_B + (GT1[0] == L'H' ? 12 : 0));
									break;
								case L'C':
									customnote[G[0]].melody = (enum melodysound)(LOW_C + (GT1[0] == L'H' ? 12 : 0) + (GT1[2] == L'#' ? 1 : 0));
									break;
								case L'D':
									customnote[G[0]].melody = (enum melodysound)(LOW_D + (GT1[0] == L'H' ? 12 : 0) + (GT1[2] == L'#' ? 1 : 0));
									break;
								case L'E':
									customnote[G[0]].melody = (enum melodysound)(LOW_E + (GT1[0] == L'H' ? 12 : 0));
									break;
								default:
									customnote[G[0]].melody = MELODYSOUND_NONE;
									break;
								}
							}
							else {
								customnote[G[0]].sound = strsans2(GT1);
							}
						}
						else if (strands(GT1, L"COLOR=")) {
							strmods(GT1, 6);
							customnote[G[0]].color = strsans2(GT1);
						}
						else {
							break;
						}
						strnex(GT1);
					}
				}
				//終わり
				else if (strands(GT1, L"#END")) { break; }
				//空白
				else if (GT1[0] == L'\0') {}
				//これ以外
				else {
					for (i[0] = 0; i[0] <= 2; i[0]++) {
						G[0] = 0;
						while (GT1[G[0]] != L'\0' && GT1[G[0]] != L',') G[0]++;
						for (i[1] = 0; i[1] < G[0]; i[1]++) {
							if (IsNoteCode(GT1[i[1]]) == 0) {
								continue;
							}
							note[i[0]][objectN[i[0]]].hittime = timer[i[0]] + 240000 * i[1] / (bpmG * G[0]);
							if (L'1' <= GT1[i[1]] && GT1[i[1]] <= L'9') {
								G[2] = (int)(customnote[GT1[i[1]] - L'1'].note);
							}
							else {
								G[2] = (int)(GT1[i[1]]);
							}
							switch ((wchar_t)(G[2])) {
							case L'H':
								note[i[0]][objectN[i[0]]].object = 1;
								break;
							case L'C':
								note[i[0]][objectN[i[0]]].object = 2;
								break;
							case L'U':
								note[i[0]][objectN[i[0]]].object = 3;
								break;
							case L'D':
								note[i[0]][objectN[i[0]]].object = 4;
								break;
							case L'L':
								note[i[0]][objectN[i[0]]].object = 5;
								break;
							case L'R':
								note[i[0]][objectN[i[0]]].object = 6;
								break;
							case L'B':
								note[i[0]][objectN[i[0]]].object = 7;
								break;
							case L'G':
								note[i[0]][objectN[i[0]]].object = 8;
								break;
							case L'?':
								note[i[0]][objectN[i[0]]].object = GetRand(4) + 2;
								if (note[i[0]][objectN[i[0]]].object == 2) {
									note[i[0]][objectN[i[0]]].object = 1;
								}
								break;
							case L'!':
								note[i[0]][objectN[i[0]]].object = GetRand(7) + 1;
								break;
							}
							//viewtimeを計算する
							G[1] = 0;
							while (0 <= scrool[G[1] + 1].starttime &&
								scrool[G[1] + 1].starttime <= note[i[0]][objectN[i[0]]].hittime) {
								G[1]++;
							}
							note[i[0]][objectN[i[0]]].viewtime = note[i[0]][objectN[i[0]]].hittime * scrool[G[1]].speed + scrool[G[1]].basetime;
							note[i[0]][objectN[i[0]]].ypos = 50 * i[0] + 300;
							note[i[0]][objectN[i[0]]].xpos = 150;
							//縦位置を計算する
							while (Ymove[i[0]][YmoveN2[i[0]]][2] <= note[i[0]][objectN[i[0]]].hittime &&
								Ymove[i[0]][YmoveN2[i[0]]][2] >= 0) {
								YmoveN2[i[0]]++;
							}
							if (Ymove[i[0]][YmoveN2[i[0]]][0] >= 0 &&
								Ymove[i[0]][YmoveN2[i[0]]][0] <= note[i[0]][objectN[i[0]]].hittime &&
								Ymove[i[0]][YmoveN2[i[0]]][2] > note[i[0]][objectN[i[0]]].hittime) {
								note[i[0]][objectN[i[0]]].ypos = movecal(Ymove[i[0]][YmoveN2[i[0]]][3],
									Ymove[i[0]][YmoveN2[i[0]]][0], Ymove[i[0]][YmoveN2[i[0]] - 1][1],
									Ymove[i[0]][YmoveN2[i[0]]][2], Ymove[i[0]][YmoveN2[i[0]]][1],
									note[i[0]][objectN[i[0]]].hittime);
							}
							else {
								note[i[0]][objectN[i[0]]].ypos = Ymove[i[0]][YmoveN2[i[0]] - 1][1];
							}
							//横位置を計算する
							while (Xmove[i[0]][XmoveN2[i[0]]][2] <= note[i[0]][objectN[i[0]]].hittime &&
								Xmove[i[0]][XmoveN2[i[0]]][2] >= 0) {
								XmoveN2[i[0]]++;
							}
							if (Xmove[i[0]][XmoveN2[i[0]]][0] >= 0 &&
								Xmove[i[0]][XmoveN2[i[0]]][0] <= note[i[0]][objectN[i[0]]].hittime &&
								Xmove[i[0]][XmoveN2[i[0]]][2] > note[i[0]][objectN[i[0]]].hittime) {
								note[i[0]][objectN[i[0]]].xpos = movecal(Xmove[i[0]][XmoveN2[i[0]]][3],
									Xmove[i[0]][XmoveN2[i[0]]][0], Xmove[i[0]][XmoveN2[i[0]] - 1][1],
									Xmove[i[0]][XmoveN2[i[0]]][2], Xmove[i[0]][XmoveN2[i[0]]][1],
									note[i[0]][objectN[i[0]]].hittime);
							}
							else {
								note[i[0]][objectN[i[0]]].xpos = Xmove[i[0]][XmoveN2[i[0]] - 1][1];
							}
							//効果音を設定する
							if (L'1' <= GT1[i[1]] && GT1[i[1]] <= L'9') {
								note[i[0]][objectN[i[0]]].sound = customnote[GT1[i[1]] - L'1'].sound;
								note[i[0]][objectN[i[0]]].melody = customnote[GT1[i[1]] - L'1'].melody;
							}
							else {
								note[i[0]][objectN[i[0]]].sound = 0;
							}
							//色を設定する
							if (L'1' <= GT1[i[1]] && GT1[i[1]] <= L'9') {
								note[i[0]][objectN[i[0]]].color = customnote[GT1[i[1]] - L'1'].color;
							}
							else {
								note[i[0]][objectN[i[0]]].color = 0;
							}
							if (note[i[0]][objectN[i[0]]].object != 8) {
								notes++;
							}
							objectN[i[0]]++;
						}
						if (i[0] <= 1) FileRead_gets(GT1, 256, songdata);
					}
					timer[0] = timer[1] = timer[2] += 240000.0 / bpmG;
				}
			}
		}
	}
	FileRead_close(songdata);
	//譜面の最後にgoustを置く
	for (i[0] = 0; i[0] <= 2; i[0]++) {
		note[i[0]][objectN[i[0]]].hittime = timer[i[0]];
		note[i[0]][objectN[i[0]] + 1].hittime = -1;
		note[i[0]][objectN[i[0]]].object = 8;
		note[i[0]][objectN[i[0]]].ypos = 1000;
	}
	lock[0][0][lockN[0]] = 1;
	lock[0][1][lockN[0]] = -1;
	lock[1][0][lockN[1]] = -1;
	lock[1][1][lockN[1]] = -1;
	Etime = timer[0];
	/*難易度計算
	(3000000/ひとつ前のノーツとの間隔)を、そのノーツの難易度点数とする。
	叩いたキーを50個記憶し、この組み合わせでできた最高値が曲の難易度。
	1個前と同じキーの時は(対象のキーはarrowのみ)
	┣間隔が150ms以上の時は変動なし
	┣間隔が150ms以下75ms以上の時は得点("間隔"/(-75)+3)倍(1〜2倍)
	┗間隔が75ms以下の時は得点2倍
	間隔が20ms以下のhitノーツは同時押し扱い、1.2倍
	arrowひっかけは1.8倍
	2個前と違うキーの時は得点1.2倍(全キー対象)
	arrowキーは得点1.2倍
	1=hit,2=non,3=up,4=down,5=left,6=right,7=non or down,8=up or down,9=up or non
	*/
	objectN[0] = 0;
	objectN[1] = 0;
	objectN[2] = 0;
	difkey[0][3] = notes;
	if (difkey[0][3] > 49)difkey[0][3] = 49;
	difkey[7][3] = (Etime - noteoff) / 25 * 2;
	if (difkey[7][3] < 10000)difkey[7][3] = 10000;
	//ノーツがなくなるまで繰り返す
	while (note[0][objectN[0]].hittime >= 0 || note[1][objectN[1]].hittime >= 0 ||
		note[2][objectN[2]].hittime >= 0) {
		//GHOSTノーツをスキップ
		for (i[0] = 0; i[0] < 3; i[0]++) {
			while (note[i[0]][objectN[i[0]]].object == 8 && note[i[0]][objectN[i[0]]].hittime >= 0) {
				objectN[i[0]]++;
			}
		}
		G[0] = -1;
		//一番早いノーツを探してG[0]に代入
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (note[i[0]][objectN[i[0]]].hittime >= 0) {
				G[0] = i[0];
				break;
			}
		}
		//無かったらブレーク
		if (G[0] == -1) break;
		//一番早いノーツを探してG[0]に代入
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (G[0] != i[0] && note[G[0]][objectN[G[0]]].hittime > note[i[0]][objectN[i[0]]].hittime &&
				note[i[0]][objectN[i[0]]].hittime >= 0) {
				G[0] = i[0];
			}
			else if (G[0] != i[0] &&
				note[G[0]][objectN[G[0]]].hittime == note[i[0]][objectN[i[0]]].hittime &&
				note[G[0]][objectN[G[0]]].object == 2 && note[i[0]][objectN[i[0]]].object != 2 &&
				note[i[0]][objectN[i[0]]].hittime >= 0) {
				G[0] = i[0];
			}
		}
		//ddifの計算
		while (note[G[0]][objectN[G[0]]].hittime >= (Etime - noteoff) / 25 * ddifG[0] + noteoff) {
			if (difkey[5][3] < 49) {
				for (i[0] = 0; i[0] < difkey[5][3]; i[0]++) {
					if (difkey[i[0]][1] > (Etime - noteoff) / 25 * ddifG[0] - difkey[7][3] + noteoff) {
						ddif[ddifG[0] - 1] += difkey[i[0]][2];
					}
				}
			}
			else {
				for (i[0] = 0; i[0] <= 49; i[0]++) {
					if (difkey[i[0]][1] > (Etime - noteoff) / 25 * ddifG[0] - difkey[7][3] + noteoff) {
						ddif[ddifG[0] - 1] += difkey[i[0]][2];
					}
				}
			}
			ddifG[0]++;
		}
		difkey[difkey[1][3]][0] = note[G[0]][objectN[G[0]]].object;
		difkey[difkey[1][3]][1] = note[G[0]][objectN[G[0]]].hittime;
		//hitノーツ補間
		if (difkey[difkey[1][3]][0] == 1) {
			if (difkey[difkey[2][3]][0] == 1 && difkey[difkey[1][3]][1] - 20 < difkey[difkey[2][3]][1]) {
				difkey[difkey[2][3]][2] *= 1;
				objectN[G[0]]++;
				continue;
			}
		}
		//catchノーツ補間
		else if (difkey[difkey[1][3]][0] == 2) {
			if (G[0] != 1) difkey[difkey[1][3]][0] = G[0] / 2 + 3;
			if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0]) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 3 && (difkey[difkey[2][3]][0] == 8 || difkey[difkey[2][3]][0] == 9)) {
				difkey[difkey[2][3]][0] = 3;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 2 && (difkey[difkey[2][3]][0] == 1 || difkey[difkey[2][3]][0] == 5 || difkey[difkey[2][3]][0] == 6)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 2 && (difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9)) {
				difkey[difkey[2][3]][0] = 2;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 4 && (difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 8)) {
				difkey[difkey[2][3]][0] = 4;
				objectN[G[0]]++;
				continue;
			}
		}
		//leftノーツ補間
		else if (difkey[difkey[1][3]][0] == 5) {
			if (difkey[difkey[2][3]][0] == 2 || difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9) {
				difkey[difkey[2][3]][0] = 5;
				continue;
			}
		}
		//rightノーツ補間
		else if (difkey[difkey[1][3]][0] == 6) {
			if (difkey[difkey[2][3]][0] == 2 || difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9) {
				difkey[difkey[2][3]][0] = 6;
				continue;
			}
		}
		//bombノーツ補間
		else if (difkey[difkey[1][3]][0] == 7) {
			if (G[0] != 0)difkey[difkey[1][3]][0] = G[0] + 7;
			if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0]) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && difkey[difkey[2][3]][0] == 8 || difkey[difkey[1][3]][0] == 8 && difkey[difkey[2][3]][0] == 7) {
				difkey[difkey[2][3]][0] = 4;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && difkey[difkey[2][3]][0] == 9 || difkey[difkey[1][3]][0] == 9 && difkey[difkey[2][3]][0] == 7) {
				difkey[difkey[2][3]][0] = 2;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 8 && difkey[difkey[2][3]][0] == 9 || difkey[difkey[1][3]][0] == 9 && difkey[difkey[2][3]][0] == 8) {
				difkey[difkey[2][3]][0] = 3;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && (difkey[difkey[2][3]][0] != 3)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 8 && (difkey[difkey[2][3]][0] == 3 || difkey[difkey[2][3]][0] == 4)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 9 && (difkey[difkey[2][3]][0] != 4)) {
				objectN[G[0]]++;
				continue;
			}
		}
		if (difkey[2][3] != -1 && difkey[3][3] != -1) {
			G[1] = difkey[difkey[1][3]][1] - difkey[difkey[2][3]][1];
			if (G[1] == 0)G[1] = 1;
			if (G[1] <= 5 && difkey[difkey[1][3]][0] != difkey[difkey[2][3]][0]) difkey[difkey[1][3]][2] = difkey[difkey[2][3]][2] * 1.3;
			else {
				difkey[difkey[1][3]][2] = 3000000 / G[1];
				if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0] && difkey[difkey[1][3]][0] >= 3 && difkey[difkey[1][3]][0] <= 6 && difkey[difkey[1][3]][2] >= 40000) difkey[difkey[1][3]][2] *= 2;
				else if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0] && difkey[difkey[1][3]][0] >= 3 && difkey[difkey[1][3]][0] <= 6 && difkey[difkey[1][3]][2] <= 40000 && difkey[difkey[1][3]][2] >= 20000) difkey[difkey[1][3]][2] = difkey[difkey[1][3]][2] / -75.0 + 3.0;
				if (difkey[difkey[1][3]][0] != difkey[difkey[3][3]][0]) difkey[difkey[1][3]][2] *= 1.2;
				if (difkey[difkey[1][3]][0] >= 3 && difkey[difkey[1][3]][0] <= 6)difkey[difkey[1][3]][2] *= 1.2;
			}
		}
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (note[G[0]][objectN[G[0]]].object >= 3 && note[G[0]][objectN[G[0]]].object <= 6 &&
				G[0] != i[0] && note[G[0]][objectN[G[0]]].object == note[i[0]][objectN[i[0]]].object &&
				note[G[0]][objectN[G[0]]].hittime + 5 >= note[i[0]][objectN[i[0]]].hittime) {
				objectN[i[0]]++;
			}
		}
		objectN[G[0]]++;
		difkey[5][3]++;
		G[0] = 0;
		for (i[0] = 0; i[0] <= difkey[0][3]; i[0]++) {
			if (difkey[i[0]][2] < 0) {
				break;
			}
			if (difkey[i[0]][1] > difkey[difkey[1][3]][1] - difkey[7][3]) {
				G[0] += difkey[i[0]][2];
			}
		}
		if (difkey[4][3] < G[0]) {
			difkey[4][3] = G[0];
		}
		for (i[0] = 1; i[0] < 4; i[0]++) {
			difkey[i[0]][3]++;
			if (difkey[i[0]][3] > difkey[0][3])difkey[i[0]][3] = 0;
		}
	}
	difkey[5][3]++;
	for (i[0] = 0; i[0] < 2; i[0]++)if (difkey[i[0]][2] == 0 && difkey[2][2] > 0)difkey[5][3]--;
	if (difkey[5][3] < 1)difkey[5][3] = 1;
	if (difkey[5][3] > 50)difkey[5][3] = 50;
	ddifG[1] = difkey[4][3];
	if (ddifG[1] <= 0)ddifG[1] = 1;
	difkey[4][3] /= 50;
	ddif[ddifG[0] - 1] = 0;
	for (i[0] = 0; i[0] < difkey[5][3]; i[0]++) {
		if (difkey[i[0]][1] > Etime - difkey[7][3]) {
			ddif[ddifG[0] - 1] += difkey[i[0]][2];
		}
	}
	for (i[0] = ddifG[0] - 1; i[0] <= 24; i[0]++) {
		ddif[i[0]] = ddif[ddifG[0] - 1];
	}
	difkey[6][3] = ddif[ddifG[0] - 1] / 50;
	//NEEDYOULv.1->379, Co-op TaylorLv.9.9->40595, "測定"/31.8+47.1="Lv"*100
	difkey[4][3] = lins(379, 100, 40595, 990, difkey[4][3]);
	difkey[6][3] = lins(379, 100, 40595, 990, difkey[6][3]);
	//ここからPC用譜面データのファイルの作成(セーブ作業)
	strcopy(dataE, RRS, 1);
	strcats(RRS, GT26[o]);
	G[2] = _wfopen_s(&fp, RRS, L"wb");
	fwrite(&mp3FN, 255, 1, fp);//音楽ファイル名
	fwrite(&bpm, sizeof(double), 1, fp);//BPM
	fwrite(&noteoff, sizeof(int), 1, fp);//offset
	fwrite(&skyFN, 255, 1, fp);//空背景名
	fwrite(&groundFN, 255, 1, fp);//地面画像名
	fwrite(&waterFN, 255, 1, fp);//水中画像名
	fwrite(&songN, 255, 1, fp);//曲名
	fwrite(&songNE, 255, 1, fp);//曲名(英語)
	fwrite(&Lv, sizeof(short int), 1, fp);//レベル
	//fwrite(&item, sizeof(int), 99, fp);//アイテム画像データ(動作未確認)
	fwrite(&fall, sizeof(int), 198, fp);//落ち物背景切り替えタイミング
	fwrite(&speedt, sizeof(double), 990, fp);//レーン速度
	fwrite(&chamo, sizeof(int), 594, fp);//キャラグラ変換タイミング
	fwrite(&Ymove, sizeof(int), 19980, fp);//縦位置移動タイミング
	fwrite(&Xmove, sizeof(int), 11988, fp);//横位置移動タイミング
	fwrite(&lock, sizeof(int), 396, fp);//ノーツ固定切り替えタイミング
	fwrite(&carrow, sizeof(int), 198, fp);//キャラ向き切り替えタイミング
	fwrite(&viewT, sizeof(int), 198, fp);//ノーツ表示時間変換タイミング
	fwrite(&note, sizeof(struct note_box), 2997, fp);//ノーツデータ
	fwrite(&notes, sizeof(short int), 1, fp);//ノーツ数
	fwrite(&Etime, sizeof(int), 1, fp);//曲終了時間
	G[0] = difkey[4][3];//最高難易度
	G[1] = difkey[6][3];//最終難易度
	fwrite(&G, sizeof(int), 2, fp);
	fwrite(&ddif, sizeof(int), 25, fp);//各区間難易度データ
	fwrite(&ddifG, sizeof(int), 2, fp);//各区間難易度データ
	fwrite(&DifFN, 255, 1, fp);//難易度バー名
	fwrite(&Movie, sizeof(int), 13986, fp);//動画データ
	fwrite(&camera, sizeof(struct camera_box), 255, fp);//カメラデータ
	fwrite(&scrool, sizeof(struct scrool_box), 99, fp);//スクロールデータ
	fclose(fp);
	return;
}

int IsNoteCode(wchar_t c) {
	if (c == L'H' || c == L'C' || c == L'U' || c == L'D' || c == L'L' || c == L'R' || c == L'B' ||
		c == L'G' || c == L'?' || c == L'!' || (L'1' <= c && c <= L'9')) {
		return 1;
	}
	return 0;
}
