void RecordLoad(int n, int o) {
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
	int item[99]; //アイテムのアドレス、DrawGraphで呼べる。
	short int itemN = 0; //↑の番号
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
	int Movie[14][99];//アイテム表示[アイテム番号,移動形態,開始時間,終了時間,開始x位置,終了x位置,開始y位置,終了y位置,開始サイズ,終了サイズ,開始角度,終了角度,開始透明度,終了透明度]
	short int MovieN = 0;
	int object[3][5][999]; //[上,中,下]レーンの音符の[時間,種類,(使ってない),縦位置,横位置]
	object[0][3][0] = 300;
	object[1][3][0] = 350;
	object[2][3][0] = 400;
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
	int ddifG[2] = { 1,1 };//0=今いる区間番号(1～25),1=最大値
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
	char key[256];
	wchar_t songN[255];
	wchar_t songNE[255];
	songN[0] = L'\0';
	wchar_t fileN[255];
	wchar_t dataE[255] = L"record/";//フォルダの名前
	wchar_t RRS[255]; //PC用譜面データの保存場所
	wchar_t mp3FN[255] = L"song/";
	wchar_t skyFN[255] = L"picture/backskynoamal.png";
	wchar_t groundFN[255] = L"picture/groundnaturenormal.png";
	wchar_t waterFN[255] = L"picture/waternormal.png";
	wchar_t DifFN[255] = L"picture/difanother.png";
	wchar_t GT1[255];
	wchar_t GT15[255];
	wchar_t GT24[] = L"picture/";
	wchar_t GT25[6][7] = { L"/0.txt" ,L"/1.txt" ,L"/2.txt" ,L"/3.txt" ,L"/4.txt" ,L"/5.txt" };
	wchar_t GT26[6][7] = { L"/0.rrs" ,L"/1.rrs" ,L"/2.rrs" ,L"/3.rrs" ,L"/4.rrs" ,L"/5.rrs" };
	wchar_t RecordCode[27][13] = { L"#MUSIC:",L"#BPM:",L"#NOTEOFFSET:",L"#SKY:",L"#FIELD:",
		L"#WATER:",L"#TITLE:",L"#LEVEL:",L"#ITEM:",L"#FALL:",
		L"#MAP:",L"#END",L"#SPEED",L"#CHARA",L"#MOVE",
		L"#XMOV",L"#GMOVE",L"#XLOCK",L"#YLOCK",L"#FALL",
		L"#VIEW:",L"#E.TITLE:",L"#CARROW",L"#DIFBAR:",L"#DIV",
		L"#ROT",L"#MOVIE:"
	};
	FILE *fp;
	songT = FileRead_open(L"song.txt");
	for (i[0] = 0; i[0] <= n; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcopy(GT1, fileN, 1);
	strcats(dataE, GT1);
	strcopy(dataE, GT1, 1);
	strcats(GT1, GT25[o]);
	songdata = FileRead_open(GT1);
	//テキストデータを読む
	while (FileRead_eof(songdata) == 0) {
		FileRead_gets(GT1, 256, songdata);
		//音楽ファイルを読み込む
		if (strands(GT1, RecordCode[0])) {
			strmods(GT1, 7);
			strcats(mp3FN, GT1);
		}
		//BPMを読み込む
		else if (strands(GT1, RecordCode[1])) bpmG = bpm = SETbpm(GT1);
		//ノートのオフセットを読み込む
		else if (strands(GT1, RecordCode[2])) timer[0] = timer[1] = timer[2] = noteoff = SEToffset(GT1);
		//空の背景を読み込む
		else if (strands(GT1, RecordCode[3])) {
			strcopy(GT24, skyFN, 1);
			strmods(GT1, 5);
			strcats(skyFN, GT1);
		}
		//地面の画像を読み込む
		else if (strands(GT1, RecordCode[4])) {
			strcopy(GT24, groundFN, 1);
			strmods(GT1, 7);
			strcats(groundFN, GT1);
		}
		//水中の画像を読み込む
		else if (strands(GT1, RecordCode[5])) {
			strcopy(GT24, waterFN, 1);
			strmods(GT1, 7);
			strcats(waterFN, GT1);
		}
		//難易度バー(another)を読み込む
		else if (strands(GT1, RecordCode[23])) {
			strcopy(dataE, DifFN, 1);
			strmods(GT1, 8);
			stradds(DifFN, L'/');
			strcats(DifFN, GT1);
		}
		//曲名を読み込む
		else if (strands(GT1, RecordCode[6])) {
			strmods(GT1, 7);
			strcopy(GT1, songN, 1);
		}
		//英語
		else if (strands(GT1, RecordCode[21])) {
			strmods(GT1, 7);
			strcopy(GT1, songNE, 1);
		}
		//レベルを読み込む
		else if (strands(GT1, RecordCode[7])) Lv = SETLv(GT1);
		//アイテムを読み込む
		else if (strands(GT1, RecordCode[8])) {
			strmods(GT1, 6);
			strcopy(dataE, GT15, 1);
			stradds(GT15, L'/');
			strcats(GT15, GT1);
			item[itemN++] = LoadGraph(GT15);
		}
		//落ち物背景指定
		else if (strands(GT1, RecordCode[9])) {
			strmods(GT1, 6);
			fall[0][0] = strsans(GT1);
			fall[0][1] = 0;
		}
		//譜面を読み込む
		else if (strands(GT1, RecordCode[10])) {
			while (FileRead_eof(songdata) == 0 && strands(GT1, RecordCode[11]) == 0) {
				FileRead_gets(GT1, 256, songdata);
				//横ノーツ速度変化
				if (strands(GT1, RecordCode[12])) {
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
				else if (strands(GT1, RecordCode[1])) bpmG = SETbpm(GT1);
				//キャラグラ変化
				else if (strands(GT1, RecordCode[13])) {
					G[0] = GT1[6] - 49;
					strmods(GT1, 8);
					chamo[G[0]][chamoN[G[0]]][0] = maxs(mins(strsans(GT1), 0), 2);
					chamo[G[0]][chamoN[G[0]]][1] = timer[G[0]];
					chamoN[G[0]]++;
				}
				//縦移動
				else if (strands(GT1, RecordCode[14])) {
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
							SETMove(timer[0], GD[0], (Ymove[i[0]][YmoveN[i[0]] - 1][1] + GD[1] * 50 - 100) / 100, 2, (GD[2] + GD[0]) / 2.0, bpmG, &Ymove[i[0]][YmoveN[i[0]]][0], &Ymove[i[0]][YmoveN[i[0]]][1], &Ymove[i[0]][YmoveN[i[0]]][2], &Ymove[i[0]][YmoveN[i[0]]][3]);
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
				else if (strands(GT1, RecordCode[15])) {
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
				else if (strands(GT1, RecordCode[24])) {
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
				else if (strands(GT1, RecordCode[16])) {
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
				else if (strands(GT1, RecordCode[17])) {
					strmods(GT1, 7);
					lock[0][0][lockN[0]] = lock[0][0][lockN[0] - 1] * -1;
					lock[0][1][lockN[0]] = shifttime(strsans(GT1), bpmG, timer[0]);
					lockN[0]++;
				}
				//縦ロック
				else if (strands(GT1, RecordCode[18])) {
					strmods(GT1, 7);
					lock[1][0][lockN[1]] = lock[1][0][lockN[1] - 1] * -1;
					lock[1][1][lockN[1]] = shifttime(strsans(GT1), bpmG, timer[0]);
					lockN[1]++;
				}
				//キャラ向き変化
				else if (strands(GT1, RecordCode[22])) {
					strmods(GT1, 8);
					carrow[0][carrowN] = carrow[0][carrowN - 1] * -1;
					carrow[1][carrowN] = shifttime(strsans(GT1), bpmG, timer[0]);
					carrowN++;
				}
				//落ち物背景切り替え
				else if (strands(GT1, RecordCode[19])) {
					strmods(GT1, 6);
					fall[fallN][0] = strsans(GT1);
					strnex(GT1);
					fall[fallN][1] = strsans(GT1);
					fallN++;
				}
				//音符表示時間
				else if (strands(GT1, RecordCode[20])) {
					strmods(GT1, 6);
					viewT[0][viewTN] = shifttime(strsans(GT1), bpmG, timer[0]);
					strnex(GT1);
					viewT[1][viewTN] = strsans(GT1);
					viewTN++;
				}
				//アイテム表示
				else if (strands(GT1, RecordCode[26])) {
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
					Movie[4][MovieN] = strsans2(GT1) * 50 + 100;
					strnex(GT1);
					Movie[5][MovieN] = strsans2(GT1) * 50 + 100;
					strnex(GT1);
					Movie[6][MovieN] = strsans2(GT1) * 50 + 100;
					strnex(GT1);
					Movie[7][MovieN] = strsans2(GT1) * 50 + 100;
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
				//終わり
				else if (strands(GT1, RecordCode[11])) { break; }
				//空白
				else if (GT1[0] == L'\0') {}
				//これ以外
				else {
					for (i[0] = 0; i[0] <= 2; i[0]++) {
						G[0] = 0;
						while (GT1[G[0]] != L'\0' && GT1[G[0]] != L',') G[0]++;
						for (i[1] = 0; i[1] < G[0]; i[1]++) {
							if (GT1[i[1]] != L'H' && GT1[i[1]] != L'C' && GT1[i[1]] != L'U' && GT1[i[1]] != L'D' && GT1[i[1]] != L'L' && GT1[i[1]] != L'R' && GT1[i[1]] != L'B' && GT1[i[1]] != L'G' && GT1[i[1]] != L'?' && GT1[i[1]] != L'!')continue;
							object[i[0]][0][objectN[i[0]]] = timer[i[0]] + 240000 * i[1] / (bpmG * G[0]);
							switch (GT1[i[1]]) {
							case L'H':
								object[i[0]][1][objectN[i[0]]] = 1;
								break;
							case L'C':
								object[i[0]][1][objectN[i[0]]] = 2;
								break;
							case L'U':
								object[i[0]][1][objectN[i[0]]] = 3;
								break;
							case L'D':
								object[i[0]][1][objectN[i[0]]] = 4;
								break;
							case L'L':
								object[i[0]][1][objectN[i[0]]] = 5;
								break;
							case L'R':
								object[i[0]][1][objectN[i[0]]] = 6;
								break;
							case L'B':
								object[i[0]][1][objectN[i[0]]] = 7;
								break;
							case L'G':
								object[i[0]][1][objectN[i[0]]] = 8;
								break;
							case L'?':
								object[i[0]][1][objectN[i[0]]] = GetRand(4)+2;
								if (object[i[0]][1][objectN[i[0]]] == 2) { object[i[0]][1][objectN[i[0]]] = 1; }
								break;
							case L'!':
								object[i[0]][1][objectN[i[0]]] = GetRand(7)+1;
								break;
							}
							//object[][2][]を使いたかったらここに入れる(ゲーム内時間になる予定)
							object[i[0]][3][objectN[i[0]]] = 50 * i[0] + 300;
							object[i[0]][4][objectN[i[0]]] = 150;
							//縦位置を計算する
							while (Ymove[i[0]][YmoveN2[i[0]]][2] <= object[i[0]][0][objectN[i[0]]] && Ymove[i[0]][YmoveN2[i[0]]][2] >= 0)YmoveN2[i[0]]++;
							if (Ymove[i[0]][YmoveN2[i[0]]][0] >= 0 && Ymove[i[0]][YmoveN2[i[0]]][0] <= object[i[0]][0][objectN[i[0]]] && Ymove[i[0]][YmoveN2[i[0]]][2] > object[i[0]][0][objectN[i[0]]]) {
								switch (Ymove[i[0]][YmoveN2[i[0]]][3]) {
								case 1:
									object[i[0]][3][objectN[i[0]]] = lins(Ymove[i[0]][YmoveN2[i[0]]][0], Ymove[i[0]][YmoveN2[i[0]] - 1][1], Ymove[i[0]][YmoveN2[i[0]]][2], Ymove[i[0]][YmoveN2[i[0]]][1], object[i[0]][0][objectN[i[0]]]);
									break;
								case 2:
									object[i[0]][3][objectN[i[0]]] = pals(Ymove[i[0]][YmoveN2[i[0]]][0], Ymove[i[0]][YmoveN2[i[0]] - 1][1], Ymove[i[0]][YmoveN2[i[0]]][2], Ymove[i[0]][YmoveN2[i[0]]][1], object[i[0]][0][objectN[i[0]]]);
									break;
								case 3:
									object[i[0]][3][objectN[i[0]]] = pals(Ymove[i[0]][YmoveN2[i[0]]][2], Ymove[i[0]][YmoveN2[i[0]]][1], Ymove[i[0]][YmoveN2[i[0]]][0], Ymove[i[0]][YmoveN2[i[0]] - 1][1], object[i[0]][0][objectN[i[0]]]);
									break;
								}
							}
							else object[i[0]][3][objectN[i[0]]] = Ymove[i[0]][YmoveN2[i[0]] - 1][1];
							//横位置を計算する
							while (Xmove[i[0]][XmoveN2[i[0]]][2] <= object[i[0]][0][objectN[i[0]]] && Xmove[i[0]][XmoveN2[i[0]]][2] >= 0) XmoveN2[i[0]]++;
							if (Xmove[i[0]][XmoveN2[i[0]]][0] >= 0 && Xmove[i[0]][XmoveN2[i[0]]][0] <= object[i[0]][0][objectN[i[0]]] && Xmove[i[0]][XmoveN2[i[0]]][2] > object[i[0]][0][objectN[i[0]]]) {
								switch (Xmove[i[0]][XmoveN2[i[0]]][3]) {
								case 1:
									object[i[0]][4][objectN[i[0]]] = lins(Xmove[i[0]][XmoveN2[i[0]]][0], Xmove[i[0]][XmoveN2[i[0]] - 1][1], Xmove[i[0]][XmoveN2[i[0]]][2], Xmove[i[0]][XmoveN2[i[0]]][1], object[i[0]][0][objectN[i[0]]]);
									break;
								case 2:
									object[i[0]][4][objectN[i[0]]] = pals(Xmove[i[0]][XmoveN2[i[0]]][0], Xmove[i[0]][XmoveN2[i[0]] - 1][1], Xmove[i[0]][XmoveN2[i[0]]][2], Xmove[i[0]][XmoveN2[i[0]]][1], object[i[0]][0][objectN[i[0]]]);
									break;
								case 3:
									object[i[0]][4][objectN[i[0]]] = pals(Xmove[i[0]][XmoveN2[i[0]]][2], Xmove[i[0]][XmoveN2[i[0]]][1], Xmove[i[0]][XmoveN2[i[0]]][0], Xmove[i[0]][XmoveN2[i[0]] - 1][1], object[i[0]][0][objectN[i[0]]]);
									break;
								}
							}
							else object[i[0]][4][objectN[i[0]]] = Xmove[i[0]][XmoveN2[i[0]] - 1][1];
							if (object[i[0]][1][objectN[i[0]]] != 8) notes++;
							objectN[i[0]]++;
						}
						if (i[0] <= 1) FileRead_gets(GT1, 256, songdata);
					}
					timer[0] = timer[1] = timer[2] += 240000.0 / bpmG;
				}
			}
		}
	}
	//譜面の最後にgoustを置く
	for (i[0] = 0; i[0] <= 2; i[0]++) {
		object[i[0]][0][objectN[i[0]]] = timer[i[0]];
		object[i[0]][0][objectN[i[0]] + 1] = -1;
		object[i[0]][1][objectN[i[0]]] = 8;
		object[i[0]][3][objectN[i[0]]] = 1000;
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
	┣間隔が150ms以下75ms以上の時は得点("間隔"/(-75)+3)倍(1～2倍)
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
	while (object[0][0][objectN[0]] >= 0 || object[1][0][objectN[1]] >= 0 || object[2][0][objectN[2]] >= 0) {
		//GHOSTノーツをスキップ
		for (i[0] = 0; i[0] < 3; i[0]++) while (object[i[0]][1][objectN[i[0]]] == 8 && object[i[0]][0][objectN[i[0]]] >= 0) objectN[i[0]]++;
		G[0] = -1;
		//一番早いノーツを探してG[0]に代入
		for (i[0] = 0; i[0] < 3; i[0]++) if (object[i[0]][0][objectN[i[0]]] >= 0) {
			G[0] = i[0];
			break;
		}
		//無かったらブレーク
		if (G[0] == -1) break;
		//一番早いノーツを探してG[0]に代入
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (G[0] != i[0] && object[G[0]][0][objectN[G[0]]] > object[i[0]][0][objectN[i[0]]] && object[i[0]][0][objectN[i[0]]] >= 0) { G[0] = i[0]; }
			else if (G[0] != i[0] && object[G[0]][0][objectN[G[0]]] == object[i[0]][0][objectN[i[0]]] && object[G[0]][1][objectN[G[0]]] == 2 && object[i[0]][1][objectN[i[0]]] != 2 && object[i[0]][0][objectN[i[0]]] >= 0) { G[0] = i[0]; }
		}
		//ddifの計算
		while (object[G[0]][0][objectN[G[0]]] >= (Etime - noteoff) / 25 * ddifG[0] + noteoff) {
			if (difkey[5][3] < 49) {
				for (i[0] = 0; i[0] < difkey[5][3]; i[0]++) if (difkey[i[0]][1] > (Etime - noteoff) / 25 * ddifG[0] - difkey[7][3] + noteoff)ddif[ddifG[0] - 1] += difkey[i[0]][2];
			}
			else {
				for (i[0] = 0; i[0] <= 49; i[0]++)if (difkey[i[0]][1] > (Etime - noteoff) / 25 * ddifG[0] - difkey[7][3] + noteoff) ddif[ddifG[0] - 1] += difkey[i[0]][2];
			}
			ddifG[0]++;
		}
		difkey[difkey[1][3]][0] = object[G[0]][1][objectN[G[0]]];
		difkey[difkey[1][3]][1] = object[G[0]][0][objectN[G[0]]];
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
		for (i[0] = 0; i[0] < 3; i[0]++) if (object[G[0]][1][objectN[G[0]]] >= 3 && object[G[0]][1][objectN[G[0]]] <= 6 && G[0] != i[0] && object[G[0]][1][objectN[G[0]]] == object[i[0]][1][objectN[i[0]]] && object[G[0]][0][objectN[G[0]]] + 5 >= object[i[0]][0][objectN[i[0]]]) objectN[i[0]]++;
		objectN[G[0]]++;
		difkey[5][3]++;
		G[0] = 0;
		for (i[0] = 0; i[0] <= difkey[0][3]; i[0]++) {
			if (difkey[i[0]][2] < 0) break;
			if (difkey[i[0]][1] > difkey[difkey[1][3]][1] - difkey[7][3])G[0] += difkey[i[0]][2];
		}
		if (difkey[4][3] < G[0])difkey[4][3] = G[0];
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
	for (i[0] = 0; i[0] < difkey[5][3]; i[0]++) if (difkey[i[0]][1] > Etime - difkey[7][3])ddif[ddifG[0] - 1] += difkey[i[0]][2];
	for (i[0] = ddifG[0] - 1; i[0] <= 24; i[0]++)ddif[i[0]] = ddif[ddifG[0] - 1];
	difkey[6][3] = ddif[ddifG[0] - 1] / 50;
	//モーニンググローリーLv.1->884, Trick MonitorLv.9->28632, "測定"/31.8+47.1="Lv"*100
	difkey[4][3] = difkey[4][3] / 31.8 + 47.1;
	difkey[6][3] = difkey[6][3] / 31.8 + 47.1;
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
	fwrite(&item, sizeof(int), 99, fp);//アイテム画像データ(動作未確認)
	fwrite(&fall, sizeof(int), 198, fp);//落ち物背景切り替えタイミング
	fwrite(&speedt, sizeof(double), 990, fp);//レーン速度
	fwrite(&chamo, sizeof(int), 594, fp);//キャラグラ変換タイミング
	fwrite(&Ymove, sizeof(int), 19980, fp);//縦位置移動タイミング
	fwrite(&Xmove, sizeof(int), 11988, fp);//横位置移動タイミング
	fwrite(&lock, sizeof(int), 396, fp);//ノーツ固定切り替えタイミング
	fwrite(&carrow, sizeof(int), 198, fp);//キャラ向き切り替えタイミング
	fwrite(&viewT, sizeof(int), 198, fp);//ノーツ表示時間変換タイミング
	fwrite(&object, sizeof(int), 14985, fp);//ノーツデータ
	fwrite(&notes, sizeof(short int), 1, fp);//ノーツ数
	fwrite(&Etime, sizeof(int), 1, fp);//曲終了時間
	G[0] = difkey[4][3];//最高難易度
	G[1] = difkey[6][3];//最終難易度
	fwrite(&G, sizeof(int), 2, fp);
	fwrite(&ddif, sizeof(int), 25, fp);//各区間難易度データ
	fwrite(&ddifG, sizeof(int), 2, fp);//各区間難易度データ
	fwrite(&DifFN, 255, 1, fp);//難易度バー名
	fwrite(&Movie, sizeof(int), 1386, fp);//動画データ
	fclose(fp);
	return;
}