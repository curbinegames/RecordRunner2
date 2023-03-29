#include "serectbase.h"
#include "recr_cutin.h"

now_scene_t musicserect2(int *p1) {
	FILE *fp;
	char closeFg = 0;
	unsigned int Cr[7];
	Cr[0] = Cr[1] = Cr[2] = Cr[4] = Cr[5] = Cr[6] = GetColor(255, 255, 255);
	Cr[3] = GetColor(0, 0, 0);
	int e;
	int G[10];
	int command[2] = { 0,1 };
	int SongNumCount = 0;
	int moveC = 250;
	int startC = -250;
	int XmoveC = 250;
	int XstartC = -250;
	int UD = 1;
	int LR = 1;
	int ShiftKey = 0;
	int AutoFlag = 0;
	int key = 1;
	int SongPreSTime = 0;
	int SongPrePat = 0;
	int SortMode = SORT_DEFAULT;
	int PackFirstNum[PackNumLim];
	int Mapping[SongNumLim];
	int	lan[6] = { 0,0,0,2,0,0 }; //使うのは[0:キャラ, 4:言語]だけ
	int chap[3] = { 0,0,0 };
	int backpos = 0;
	int rate = GetRate();
	int CutTime = 0;
	const int keyCB[7] = {
		KEY_INPUT_RETURN, KEY_INPUT_BACK, KEY_INPUT_UP,
		KEY_INPUT_DOWN, KEY_INPUT_LEFT, KEY_INPUT_RIGHT, KEY_INPUT_Z
	};
	double diskr = 0;
	//wchar_t変数定義
	wchar_t PackName[PackNumLim][256];
	wchar_t viewingjacket[255] = { L"picture/NULL jucket.png" };
	wchar_t viewingDifBar[255] = { L"NULL" };
	wchar_t playingsong[255] = { L"NULL" };
	wchar_t GT1[256];
	wchar_t GT2[256];
	wchar_t GT3[256];
	wchar_t ST1[] = L"record/";
	wchar_t ST2[] = L"/list.txt";
	wchar_t ST3[] = { L"NULL" };
	wchar_t ST4[6][7] = { L"0.txt" ,L"1.txt" ,L"2.txt" ,L"3.txt" ,L"4.txt" ,L"5.txt" };
	wchar_t ST5[] = { L"picture/NULL jucket.png" };
	wchar_t ST6[] = { L"#TITLE:" };
	wchar_t ST6E[] = { L"#E.TITLE:" };
	wchar_t ST7[] = { L"#ARTIST:" };
	wchar_t ST7E[] = { L"#E.ARTIST:" };
	wchar_t ST8[] = { L"#MUSIC:" };
	wchar_t ST9[] = { L"song/" };
	wchar_t ST10[] = { L"#LEVEL:" };
	wchar_t ST11[] = { L"#PREVIEW:" };
	wchar_t ST12[] = { L"#JACKET:" };
	wchar_t ST13[] = { L"#DIFBAR:" };
	wchar_t ST14[] = { L"#MAP:" };
	wchar_t ST15[] = { L"score/" };
	wchar_t ST16[] = { L".dat" };
	now_scene_t next = SCENE_EXIT;
	MUSIC_BOX songdata[SongNumLim];
	int back = LoadGraph(L"picture/MSback.png");
	int jacketpic = LoadGraph(L"picture/NULL jucket.png");
	int detail = LoadGraph(L"picture/detail.png");
	int disk = LoadGraph(L"picture/disk.png");
	int help = LoadGraph(L"picture/help.png");
	int bar[2];
	bar[0] = LoadGraph(L"picture/songbarB.png");
	bar[1] = LoadGraph(L"picture/songbarY.png");
	int CRatepic[5];
	CRatepic[0] = LoadGraph(L"picture/MarkD.png");
	CRatepic[1] = LoadGraph(L"picture/MarkC.png");
	CRatepic[2] = LoadGraph(L"picture/MarkNM.png");
	CRatepic[3] = LoadGraph(L"picture/MarkFC.png");
	CRatepic[4] = LoadGraph(L"picture/MarkP.png");
	int CRankpic[6];
	CRankpic[0] = LoadGraph(L"picture/MiniEX.png");
	CRankpic[1] = LoadGraph(L"picture/MiniS.png");
	CRankpic[2] = LoadGraph(L"picture/MiniA.png");
	CRankpic[3] = LoadGraph(L"picture/MiniB.png");
	CRankpic[4] = LoadGraph(L"picture/MiniC.png");
	CRankpic[5] = LoadGraph(L"picture/MiniD.png");
	int difC[10];
	difC[0] = LoadGraph(L"picture/Dif0S.png");
	difC[1] = LoadGraph(L"picture/Dif0B.png");
	difC[2] = LoadGraph(L"picture/Dif1S.png");
	difC[3] = LoadGraph(L"picture/Dif1B.png");
	difC[4] = LoadGraph(L"picture/Dif2S.png");
	difC[5] = LoadGraph(L"picture/Dif2B.png");
	difC[6] = LoadGraph(L"picture/Dif3S.png");
	difC[7] = LoadGraph(L"picture/Dif3B.png");
	difC[8] = LoadGraph(L"picture/Dif4S.png");
	difC[9] = LoadGraph(L"picture/Dif4B.png");
	int difbar[6];
	difbar[0] = LoadGraph(L"picture/difauto.png");
	difbar[1] = LoadGraph(L"picture/difeasy.png");
	difbar[2] = LoadGraph(L"picture/difnormal.png");
	difbar[3] = LoadGraph(L"picture/difhard.png");
	difbar[4] = difbar[5] = LoadGraph(L"picture/difanother.png");
	int charaimg = 0;
	int ratebarimg = GetRateBarPic(rate);
	int previewM = LoadSoundMem(L"null.mp3");
	int select = LoadSoundMem(L"sound/arrow.wav");
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&lan, sizeof(int), 6, fp);
		fclose(fp);
	}
	G[0] = _wfopen_s(&fp, L"save/chap.dat", L"rb");
	if (G[0] == 0) {
		fread(&chap, sizeof(chap), 3, fp);
		fclose(fp);
	}
	G[0] = _wfopen_s(&fp, L"save/SongSelect2.dat", L"rb");
	if (G[0] == 0) {
		fread(&command, sizeof(int), 2, fp);
		fread(&SortMode, sizeof(int), 1, fp);
		fclose(fp);
	}
	G[0] = FileRead_open(L"RecordPack.txt");
	G[1] = 0;
	while (FileRead_eof(G[0]) == 0) {
		FileRead_gets(GT1, 256, G[0]);
		strcopy(GT1, PackName[G[1]], 1);
		G[1]++;
	}
	G[4] = 0;
	G[2] = 0;
	for (int i = 0; i < G[1]; i++) {
		//"record/<パック名>/list.txt"を探す
		strcopy(ST1, GT1, 1); //"record/"
		strcats(GT1, PackName[i]); //"record/<パック名>"
		strcats(GT1, ST2); //"record/<パック名>/list.txt"
		G[0] = FileRead_open(GT1);
		PackFirstNum[i] = G[2];
		while (FileRead_eof(G[0]) == 0) {
			FileRead_gets(GT1, 256, G[0]); //GT1に曲名が入る
			songdata[G[2]].limit = 3;
			strcopy(ST3, songdata[G[2]].difP, 1);
			for (int j = 0; j < 6; j++) {
				strcopy(ST1, GT2, 1); //"record/"
				strcats(GT2, PackName[i]); //"record/<パック名>"
				stradds(GT2, L'/'); //"record/<パック名>/"
				strcats(GT2, GT1); //"record/<パック名>/<曲名>"
				stradds(GT2, L'/'); //"record/<パック名>/<曲名>/"
				strcopy(GT2, GT3, 1); //GT3にコピー
				strcats(GT2, ST4[j]); //"record/<パック名>/<曲名>/<難易度番号>.txt"
				//初期値定義(ファイルがなくても代入する)
				strcopy(ST3, songdata[G[2]].SongName[j], 1);
				strcopy(ST3, songdata[G[2]].artist[j], 1);
				strcopy(ST3, songdata[G[2]].SongFileName[j], 1);
				strcopy(ST5, songdata[G[2]].jacketP[j], 1);
				strcopy(PackName[i], songdata[G[2]].packName, 1);
				songdata[G[2]].level[j] = -1;
				songdata[G[2]].preview[j][0] = 441000;
				songdata[G[2]].preview[j][1] = 2646000;
				G[3] = FileRead_open(GT2);
				if (G[3] == 0) {
					continue;
				}
				while (FileRead_eof(G[3]) == 0) {
					FileRead_gets(GT2, 256, G[3]);
					//曲名を読み込む
					if (strands(GT2, ST6) && (lan[4] == 0 || strands(songdata[G[2]].SongName[j], ST3))) {
						strmods(GT2, 7);
						strcopy(GT2, songdata[G[2]].SongName[j], 1);
					}
					if (strands(GT2, ST6E) && (lan[4] == 1 || strands(songdata[G[2]].SongName[j], ST3))) {
						strmods(GT2, 9);
						strcopy(GT2, songdata[G[2]].SongName[j], 1);
					}
					//作曲者を読み込む
					if (strands(GT2, ST7) && (lan[4] == 0 || strands(songdata[G[2]].artist[j], ST3))) {
						strmods(GT2, 8);
						strcopy(GT2, songdata[G[2]].artist[j], 1);
					}
					if (strands(GT2, ST7E) && (lan[4] == 1 || strands(songdata[G[2]].artist[j], ST3))) {
						strmods(GT2, 10);
						strcopy(GT2, songdata[G[2]].artist[j], 1);
					}
					//曲ファイル名を読み込む
					if (strands(GT2, ST8)) {
						strmods(GT2, 7);
						strcopy(GT3, songdata[G[2]].SongFileName[j], 1);
						strcats(songdata[G[2]].SongFileName[j], GT2);
					}
					//難易度を読み込む
					if (strands(GT2, ST10)) {
						strmods(GT2, 7);
						songdata[G[2]].level[j] = strsans(GT2);
					}
					//プレビュー時間を読み込む
					if (strands(GT2, ST11)) {
						strmods(GT2, 9);
						songdata[G[2]].preview[j][0] = (int)((double)strsans(GT2) / 1000.0 * 44100.0);
						strnex(GT2);
						if (L'0' <= GT2[1] && GT2[1] <= L'9') {
							songdata[G[2]].preview[j][1] = (int)((double)strsans(GT2) / 1000.0 * 44100.0);
						}
					}
					//ジャケット写真を読み込む
					if (strands(GT2, ST12)) {
						strmods(GT2, 8);
						strcopy(GT3, songdata[G[2]].jacketP[j], 1);
						strcats(songdata[G[2]].jacketP[j], GT2);
					}
					//差し替えAnotherバーを読み込む
					if (strands(GT2, ST13)) {
						strmods(GT2, 8);
						strcopy(GT3, songdata[G[2]].difP, 1);
						strcats(songdata[G[2]].difP, GT2);
					}
					//マップに入ったら抜ける
					if (strands(GT2, ST14)) { break; }
				}
				FileRead_close(G[3]);
			}
			//ハイスコアを読み込む
			for (int j = 0; j < 6; j++) {
				songdata[G[2]].Hscore[j] = 0;
				songdata[G[2]].Hacc[j] = 0;
				songdata[G[2]].Hdis[j] = 0;
				songdata[G[2]].ClearRank[j] = 6;
				songdata[G[2]].ClearRate[j] = 0;
			}
			strcopy(ST15, GT2, 1);
			strcats(GT2, GT1);
			strcats(GT2, ST16);
			e = _wfopen_s(&fp, GT2, L"rb");
			if (e == 0) {
				fread(&songdata[G[2]].Hscore, sizeof(int), 6, fp);
				fread(&songdata[G[2]].Hacc, sizeof(double), 6, fp);
				fread(&songdata[G[2]].Hdis, sizeof(int), 6, fp);
				fread(&songdata[G[2]].ClearRank, sizeof(int), 6, fp);
				fread(&songdata[G[2]].ClearRate, sizeof(int), 6, fp);
				fclose(fp);
			}
			if (strands(songdata[G[2]].SongName[4], L"NULL") == 0 &&
				(strands(songdata[G[2]].SongName[5], L"NULL") ||
				songdata[G[2]].Hscore[5] >= 1)) {
				songdata[G[2]].limit = 4;
			}
			if (songdata[G[2]].limit == 3) {
				strcopy(L"NULL", songdata[G[2]].SongName[4], 1);
				strcopy(L"NULL", songdata[G[2]].artist[4], 1);
			}
			Mapping[G[4]] = G[2];
			SongNumCount++;
			G[2]++;
			G[4]++;
		}
	}
	switch (lan[0]) {
		case 0: charaimg = LoadGraph(L"picture/Mpicker.png"); break;
		case 1: charaimg = LoadGraph(L"picture/Mgator.png"); break;
		case 2: charaimg = LoadGraph(L"picture/Mtaylor.png"); break;
		default: charaimg = 0; break;
	}
	//ここまで、曲情報の読み込み
	//曲のソート
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
	CutinReady();
	CutTime = GetNowCount();
	while (1) {
		ClearDrawScreen();
		//背景の表示
		DrawGraph(backpos, 0, back, TRUE);
		DrawGraph(backpos + 640, 0, back, TRUE);
		backpos = (backpos - 2) % 640;
		//時間設定
		moveC = mins(-1 * (GetNowCount() - startC) + 250, 0);
		XmoveC = mins(-1 * (GetNowCount() - XstartC) + 250, 0);
		//曲一覧を作成する
		DrawSongBar(command[0], command[1], SongNumCount, UD, moveC, bar,
			songdata, Mapping, CRatepic, CRankpic);
		//ジャケット表示
		if (strands(viewingjacket, songdata[Mapping[command[0]]].jacketP[command[1]]) == 0) {
			DeleteGraph(jacketpic);
			strcopy(songdata[Mapping[command[0]]].jacketP[command[1]], viewingjacket, 1);
			jacketpic = LoadGraph(viewingjacket);
		}
		DrawExtendGraph(305, 75, 545, 315, jacketpic, TRUE);
		//難易度マーカーを表示する
		DrawDifMaker(songdata[Mapping[command[0]]], command[1], command[0], difC);
		//難易度バーを表示する
		if (strands(viewingDifBar, songdata[Mapping[command[0]]].difP) == 0) {
			DeleteGraph(difbar[4]);
			DeleteGraph(difbar[5]);
			strcopy(songdata[Mapping[command[0]]].difP, viewingDifBar, 1);
			difbar[4] = difbar[5] = LoadGraph(viewingDifBar);
			if (difbar[4] == -1) {
				DeleteGraph(difbar[4]);
				DeleteGraph(difbar[5]);
				difbar[4] = difbar[5] = LoadGraph(L"picture/difanother.png");
			}
		}
		if (LR == 1) {
			DrawGraph(460, 320, difbar[command[1]], TRUE);
			DrawGraph(pals(0, 640, 250, 460, XmoveC), 320, difbar[command[1] + 1], TRUE);
		}
		else if (LR == -1) {
			DrawGraph(460, 320, difbar[command[1] - 1], TRUE);
			DrawGraph(pals(0, 460, 250, 640, XmoveC), 320, difbar[command[1]], TRUE);
		}
		//詳細を表示する
		DrawGraph(316, 370, detail, TRUE);
		DrawFormatString(330, 380, Cr[3], L"%s", songdata[Mapping[command[0]]].packName);
		DrawFormatString(335, 405, Cr[3], L"Lv.%2d", songdata[Mapping[command[0]]].level[command[1]]);
		for (int i = 0; i < 15; i++) {
			if (10 <= i && songdata[Mapping[command[0]]].level[command[1]] <= i) {
				break;
			}
			if (i < songdata[Mapping[command[0]]].level[command[1]]) {
				DrawString(16 * i + 390, 405, L"★", Cr[3]);
			}
			else {
				DrawString(16 * i + 390, 405, L"☆", Cr[3]);
			}
		}
		DrawFormatString(340, 430, Cr[3], L"HighSCORE:%6d/%6.2f%%/%5.3fkm",
			songdata[Mapping[command[0]]].Hscore[command[1]],
			songdata[Mapping[command[0]]].Hacc[command[1]],
			songdata[Mapping[command[0]]].Hdis[command[1]] / 1000.0);
		//プレビューを流す
		if (moveC == 0 && XmoveC == 0
			&& strands(songdata[Mapping[command[0]]].SongFileName[command[1]], ST3) == 0
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
			SongPreSTime = GetNowCount();
		}
		if (GetNowCount() - SongPreSTime < 500) {
			ChangeVolumeSoundMem(lins(0, 0, 500, 255, GetNowCount() - SongPreSTime), previewM);
		}
		else if (500 <= GetNowCount() - SongPreSTime && GetNowCount() - SongPreSTime < 14500) {
			ChangeVolumeSoundMem(255, previewM);
		}
		else if (14500 <= GetNowCount() - SongPreSTime && GetNowCount() - SongPreSTime < 15000) {
			ChangeVolumeSoundMem(lins(14500, 255, 15000, 0, GetNowCount() - SongPreSTime), previewM);
		}
		else if (15000 <= GetNowCount() - SongPreSTime) {
			StopSoundMem(previewM);
			SongPrePat++;
			SetCurrentPositionSoundMem(songdata[Mapping[command[0]]].preview[command[1]][SongPrePat % 2], previewM);
			ChangeVolumeSoundMem(0, previewM);
			PlaySoundMem(previewM, DX_PLAYTYPE_BACK, FALSE);
			WaitTimer(30);
			SongPreSTime = GetNowCount();
		}
		//レートを表示する
		DrawRate(rate, ratebarimg, chap[lan[0]], charaimg);
		//ディスクを表示する(ソート表示場所)
		if (UD == 1) { diskr += pals(0, 2, 250, -48, moveC) / 100.0; }
		else { diskr += pals(0, 2, 250, 50, moveC) / 100.0; }
		if (diskr > 6.28) { diskr -= 6.28; }
		else if (diskr < 0) { diskr += 6.28; }
		DrawRotaGraph(610, 25, 1, diskr, disk, TRUE);
		//今のソート内容を表示する
		ViewSortMode(SortMode, lan[4]);
		//操作説明を表示する
		ShowHelpBar(Cr[0], help, lan[4]);
		//デバッグ(320,410スタート)
		//DrawFormatString(320, 410, Cr[0], L"%d", SortMode);
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
					&& strands(songdata[Mapping[command[0]]].SongFileName[5], ST3) == 0
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
				closeFg = 1;
				CutTime = GetNowCount();
			}
		}
		else if (G[0] == 2) { /* 戻る */
			next = SCENE_MENU;
			closeFg = 1;
			CutTime = GetNowCount();
		}
		else {
			switch (G[0]) {
			case 3: /* 曲選択上 */
				command[0]--;
				//縦コマンド(曲)の端を過ぎたとき、もう片方の端に移動する
				if (command[0] < 0) command[0] = SongNumCount - 1;
				if (command[1] > songdata[Mapping[command[0]]].limit) {
					command[1] = songdata[Mapping[command[0]]].limit;
					XstartC -= 250;
					SortSong(songdata, Mapping, SortMode, command[1], SongNumCount);
				}
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				UD = -1;
				startC = GetNowCount();
				//デフォルトソートで、今選んだ曲の難易度に譜面が無かったら、譜面がある難易度を探す。
				if (SortMode == SORT_DEFAULT && strands(songdata[Mapping[command[0]]].SongName[command[1]], ST3)) {
					if (strands(songdata[Mapping[command[0]]].SongName[0], ST3) != 1) command[1] = 0;
					for (int i = 1; i <= 3; i++) {
						if (strands(songdata[Mapping[command[0]]].SongName[i], ST3) != 1) {
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
					XstartC -= 250;
					SortSong(songdata, Mapping, SortMode, command[1], SongNumCount);
				}
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				UD = 1;
				startC = GetNowCount();
				//デフォルトソートで、今選んだ曲の難易度に譜面が無かったら、譜面がある難易度を探す。
				if (SortMode == SORT_DEFAULT && strands(songdata[Mapping[command[0]]].SongName[command[1]], ST3)) {
					if (strands(songdata[Mapping[command[0]]].SongName[0], ST3) != 1) command[1] = 0;
					for (int i = 1; i <= 3; i++) {
						if (strands(songdata[Mapping[command[0]]].SongName[i], ST3) != 1) {
							command[1] = i;
							break;
						}
					}
				}
				break;
			case 5: /* 難易度下降 */
				command[1]--;
				XstartC = GetNowCount();
				if (command[1] < 0) {
					command[1] = 0;
					XstartC -= 250;
				}
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				LR = 1;
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
				XstartC = GetNowCount();
				if (command[1] > songdata[Mapping[command[0]]].limit) {
					command[1] = songdata[Mapping[command[0]]].limit;
					XstartC -= 250;
				}
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				LR = -1;
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
		if (closeFg == 1 && CutTime + 2000 <= GetNowCount()) {
			StopSoundMem(previewM);
			ClearDrawScreen();
			InitSoundMem();
			InitGraph();
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
