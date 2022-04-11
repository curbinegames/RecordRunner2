#define PackNumLim 8
#define SongNumLim 64
#define SORT_DEFAULT 0
#define SORT_LEVEL 1
#define SORT_SCORE 2

void ChangeSortMode(int *mode);
void SortSong(int *mapping, const int mode, const int dif, const int lv[SongNumLim][6], const int score[SongNumLim][6], const int SongNumCount);
void ViewSortMode(const int mode);

int musicserect2(int *p1) {
	unsigned int Cr[7];
	Cr[0] = Cr[1] = Cr[2] = Cr[4] = Cr[5] = Cr[6] = GetColor(255, 255, 255);
	Cr[3] = GetColor(0, 0, 0);
	int e;
	int G[5];
	int command[2] = { 0,1 };
	int SongNumCount = 0;
	int moveC = 250;
	int startC = -250;
	int XmoveC = 250;
	int XstartC = -250;
	int picsong;
	int UD = 1;
	int LR = 1;
	int ShiftKey = 0;
	int AutoFlag = 0;
	int key = 1;
	int SongPreSTime = 0;
	int SongPrePat = 0;
	int SortMode = SORT_DEFAULT;
	int next = 5;
	int PackFirstNum[PackNumLim];
	int limit[SongNumLim];
	int level[SongNumLim][6];
	int preview[SongNumLim][6][2];
	int Hscore[SongNumLim][6];
	int Hdis[SongNumLim][6];
	int ClearRank[SongNumLim][6];//0=EX, 1=S, 2=A, 3=B, 4=C, 5=D, 6=not play
	int ClearRate[SongNumLim][6];//0=not play, 1=droped, 2=cleared, 3=no miss!, 4=full combo!!, 5=perfect!!!
	int Mapping[SongNumLim];
	int	lan[6] = { 0,0,0,2,0,0 }; //使うのは[4:言語]だけ
	double rate[10];
	double diskr = 0;
	double Hacc[SongNumLim][6];
	wchar_t PackName[PackNumLim][256];
	wchar_t difP[SongNumLim][256];
	wchar_t SongName[SongNumLim][6][256];
	wchar_t artist[SongNumLim][6][256];
	wchar_t SongFileName[SongNumLim][6][256];
	wchar_t jacketP[SongNumLim][6][256];
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
	int back = LoadGraph(L"picture/MSback.png");
	int jacketpic = LoadGraph(L"picture/NULL jucket.png");
	int detail = LoadGraph(L"picture/detail.png");
	int disk = LoadGraph(L"picture/disk.png");
	int help = LoadGraph(L"picture/help.png");
	int bar[7];
	bar[0] = bar[1] = bar[2] = bar[4] = bar[5] = bar[6] = LoadGraph(L"picture/songbarB.png");
	bar[3] = LoadGraph(L"picture/songbarY.png");
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
	int difC[5][2];
	difC[0][0] = LoadGraph(L"picture/dif0S.png");
	difC[0][1] = LoadGraph(L"picture/dif0B.png");
	difC[1][0] = LoadGraph(L"picture/dif1S.png");
	difC[1][1] = LoadGraph(L"picture/dif1B.png");
	difC[2][0] = LoadGraph(L"picture/dif2S.png");
	difC[2][1] = LoadGraph(L"picture/dif2B.png");
	difC[3][0] = LoadGraph(L"picture/dif3S.png");
	difC[3][1] = LoadGraph(L"picture/dif3B.png");
	difC[4][0] = LoadGraph(L"picture/dif4S.png");
	difC[4][1] = LoadGraph(L"picture/dif4B.png");
	int difbar[6];
	difbar[0] = LoadGraph(L"picture/difauto.png");
	difbar[1] = LoadGraph(L"picture/difeasy.png");
	difbar[2] = LoadGraph(L"picture/difnormal.png");
	difbar[3] = LoadGraph(L"picture/difhard.png");
	difbar[4] = difbar[5] = LoadGraph(L"picture/difanother.png");
	int previewM = LoadSoundMem(L"null.mp3");
	int select = LoadSoundMem(L"sound/arrow.ogg");
	FILE *fp;
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&lan, sizeof(int), 6, fp);
		fclose(fp);
	}
	G[0] = _wfopen_s(&fp, L"save/SongSelect2.dat", L"rb");
	if (G[0] == 0) {
		fread(&command, sizeof(int), 2, fp);
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
			limit[G[2]] = 3;
			strcopy(ST3, difP[G[2]], 1);
			for (int j = 0; j < 6; j++) {
				strcopy(ST1, GT2, 1); //"record/"
				strcats(GT2, PackName[i]); //"record/<パック名>"
				stradds(GT2, L'/'); //"record/<パック名>/"
				strcats(GT2, GT1); //"record/<パック名>/<曲名>"
				stradds(GT2, L'/'); //"record/<パック名>/<曲名>/"
				strcopy(GT2, GT3, 1); //GT3にコピー
				strcats(GT2, ST4[j]); //"record/<パック名>/<曲名>/<難易度番号>.txt"
				G[3] = FileRead_open(GT2);
				strcopy(ST3, SongName[G[2]][j], 1);
				strcopy(ST3, artist[G[2]][j], 1);
				strcopy(ST3, SongFileName[G[2]][j], 1);
				strcopy(ST5, jacketP[G[2]][j], 1);
				level[G[2]][j] = -1;
				preview[G[2]][j][0] = 441000;
				preview[G[2]][j][1] = 2646000;
				while (FileRead_eof(G[3]) == 0) {
					FileRead_gets(GT2, 256, G[3]);
					//曲名を読み込む
					if (strands(GT2, ST6) && (lan[4] == 0 || strands(SongName[G[2]][j], ST3))) {
						strmods(GT2, 7);
						strcopy(GT2, SongName[G[2]][j], 1);
					}
					if (strands(GT2, ST6E) && (lan[4] == 1 || strands(SongName[G[2]][j], ST3))) {
						strmods(GT2, 9);
						strcopy(GT2, SongName[G[2]][j], 1);
					}
					//作曲者を読み込む
					if (strands(GT2, ST7) && (lan[4] == 0 || strands(artist[G[2]][j], ST3))) {
						strmods(GT2, 8);
						strcopy(GT2, artist[G[2]][j], 1);
					}
					if (strands(GT2, ST7E) && (lan[4] == 1 || strands(artist[G[2]][j], ST3))) {
						strmods(GT2, 10);
						strcopy(GT2, artist[G[2]][j], 1);
					}
					//曲ファイル名を読み込む
					if (strands(GT2, ST8)) {
						strmods(GT2, 7);
						strcopy(ST9, SongFileName[G[2]][j], 1);
						strcats(SongFileName[G[2]][j], GT2);
					}
					//難易度を読み込む
					if (strands(GT2, ST10)) {
						strmods(GT2, 7);
						level[G[2]][j] = strsans(GT2);
					}
					//プレビュー時間を読み込む
					if (strands(GT2, ST11)) {
						strmods(GT2, 9);
						preview[G[2]][j][0] = double(strsans(GT2)) / 1000 * 44100.0;
						strnex(GT2);
						if (L'0' <= GT2[1] && GT2[1] <= L'9') {
							preview[G[2]][j][1] = double(strsans(GT2)) / 1000 * 44100.0;
						}
					}
					//ジャケット写真を読み込む
					if (strands(GT2, ST12)) {
						strmods(GT2, 8);
						strcopy(GT3, jacketP[G[2]][j], 1);
						strcats(jacketP[G[2]][j], GT2);
					}
					//差し替えAnotherバーを読み込む
					if (strands(GT2, ST13)) {
						strmods(GT2, 8);
						strcopy(GT3, difP[G[2]], 1);
						strcats(difP[G[2]], GT2);
					}
					//マップに入ったら抜ける
					if (strands(GT2, ST14)) { break; }
				}
				FileRead_close(G[3]);
			}
			//ハイスコアを読み込む
			for (int j = 0; j < 6; j++) {
				Hscore[G[2]][j] = 0;
				Hacc[G[2]][j] = 0;
				Hdis[G[2]][j] = 0;
				ClearRank[G[2]][j] = 6;
				ClearRate[G[2]][j] = 0;
			}
			strcopy(ST15, GT2, 1);
			strcats(GT2, GT1);
			strcats(GT2, ST16);
			e = _wfopen_s(&fp, GT2, L"rb");
			if (e == 0) {
				fread(&Hscore[G[2]], sizeof(int), 6, fp);
				fread(&Hacc[G[2]], sizeof(double), 6, fp);
				fread(&Hdis[G[2]], sizeof(int), 6, fp);
				fread(&ClearRank[G[2]], sizeof(int), 6, fp);
				fread(&ClearRate[G[2]], sizeof(int), 6, fp);
				fclose(fp);
			}
			if (strands(SongName[G[2]][4], ST3) == 0 && (strands(SongName[G[2]][5], ST3)
				|| ClearRate[G[2]][5] >= 1)) { limit[G[2]] = 4; }
			Mapping[G[4]] = G[2];
			SongNumCount++;
			G[2]++;
			G[4]++;
		}
	}
	//ここまで、曲情報の読み込み
	e = _wfopen_s(&fp, L"save/rateN.dat", L"rb");
	if (e == 0) {
		fread(&rate, sizeof(rate), 10, fp);
		fclose(fp);
	}
	for (int i = 0; i < 10; i++) { rate[i] = mins(rate[i], 0); }
	G[0] += 0;
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, back, TRUE);
		//曲一覧を作成する
		moveC = mins(-1 * (GetNowCount() - startC) + 250, 0);
		XmoveC = mins(-1 * (GetNowCount() - XstartC) + 250, 0);
		picsong = (command[0] - 3) % SongNumCount;
		if (picsong < 0) { picsong += SongNumCount; }
		for (int i = 0; i < 7; i++) {
			G[0] = (UD * moveC * moveC + 62500 * i) / 3125; //基準横位置
			G[1] = (6 * UD * moveC * moveC + 375000 * i) / 3125; //基準縦位置
			DrawGraph(G[0] - 120, G[1] - 170, bar[i], TRUE); //曲リストバー
			DrawString(G[0] - 30, G[1] - 150, SongName[Mapping[picsong]][command[1]], Cr[i]); //曲名
			DrawString(G[0] - 30, G[1] - 130, artist[Mapping[picsong]][command[1]], Cr[i]); //アーティスト名
			if (1 <= ClearRate[Mapping[picsong]][command[1]]) {
				DrawGraph(G[0] + 155, G[1] - 130, CRatepic[ClearRate[Mapping[picsong]][command[1]] - 1], TRUE);
			}
			if (0 <= ClearRank[Mapping[picsong]][command[1]]
				&& ClearRank[Mapping[picsong]][command[1]] <= 5) {
				DrawGraph(G[0] + 155, G[1] - 130, CRankpic[ClearRank[Mapping[picsong]][command[1]]], TRUE);
			}
			picsong = (picsong + 1) % SongNumCount;
		}
		//ジャケット表示
		if (strands(viewingjacket, jacketP[Mapping[command[0]]][command[1]]) == 0) {
			DeleteGraph(jacketpic);
			strcopy(jacketP[Mapping[command[0]]][command[1]], viewingjacket, 1);
			jacketpic = LoadGraph(viewingjacket);
		}
		DrawExtendGraph(305, 75, 545, 315, jacketpic, TRUE);
		//難易度マーカーを表示する
		G[1] = 0;
		for (int i = 0; i < 4; i++) {
			G[0] = 0;
			if (command[1] == i) G[0] = 1;
			if (command[1] < i) G[1] = 1;
			if (strands(SongFileName[Mapping[command[0]]][i], ST3) == 0
				&& i <= limit[Mapping[command[0]]]) {
				DrawGraph(550 + 11 * G[1] + 16 * i, 290 - 11 * G[0], difC[i][G[0]], TRUE);
			}
		}
		//難易度バーを表示する
		if (strands(viewingDifBar, difP[Mapping[command[0]]]) == 0) {
			DeleteGraph(difbar[4]);
			DeleteGraph(difbar[5]);
			strcopy(difP[Mapping[command[0]]], viewingDifBar, 1);
			difbar[4] = difbar[5] = LoadGraph(viewingDifBar);
			if (difbar[4] == -1) {
				DeleteGraph(difbar[4]);
				DeleteGraph(difbar[5]);
				difbar[4] = difbar[5] = LoadGraph(L"picture/difanother.png");
			}
		}
		if (LR == 1) {
			DrawGraph(460, 320, difbar[command[1]], TRUE);
			DrawGraph(-0.00288*XmoveC*XmoveC + 640, 320, difbar[command[1] + 1], TRUE);
		}
		else if (LR == -1) {
			DrawGraph(460, 320, difbar[command[1] - 1], TRUE);
			DrawGraph(0.00288*XmoveC*XmoveC + 460, 320, difbar[command[1]], TRUE);
		}
		//詳細を表示する
		DrawGraph(316, 370, detail, TRUE);
		DrawFormatString(500, 380, Cr[3], L"LEVEL:%d", level[Mapping[command[0]]][command[1]]);
		DrawFormatString(340, 410, Cr[3], L"HighSCORE:%d", Hscore[Mapping[command[0]]][command[1]]);
		DrawFormatString(500, 410, Cr[3], L"HighACC:%.2f%%", Hacc[Mapping[command[0]]][command[1]]);
		DrawFormatString(350, 430, Cr[3], L"HighDis:%.3fkm", Hdis[Mapping[command[0]]][command[1]] / 1000.0);
		//プレビューを流す
		if (moveC == 0 && XmoveC == 0
			&& strands(SongFileName[Mapping[command[0]]][command[1]], ST3) == 0
			&& strands(playingsong, SongFileName[Mapping[command[0]]][command[1]]) == 0) {
			StopSoundMem(previewM);
			DeleteSoundMem(previewM);
			SongPrePat = 0;
			strcopy(SongFileName[Mapping[command[0]]][command[1]], playingsong, 1);
			previewM = LoadSoundMem(playingsong);
			SetCurrentPositionSoundMem(preview[Mapping[command[0]]][command[1]][0], previewM);
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
			SetCurrentPositionSoundMem(preview[Mapping[command[0]]][command[1]][SongPrePat % 2], previewM);
			ChangeVolumeSoundMem(0, previewM);
			PlaySoundMem(previewM, DX_PLAYTYPE_BACK, FALSE);
			WaitTimer(30);
			SongPreSTime = GetNowCount();
		}
		//ディスクを表示する(レート表示場所)
		DrawRotaGraph(640, 25, 1, diskr, disk, TRUE);
		diskr += 0.02;
		if (diskr > 6.28) diskr -= 6.28;
		DrawFormatString(550, 100, Cr[0], L"RATE:%5.2f",
			rate[0] + rate[1] + rate[2] + rate[3] + rate[4] +
			rate[5] + rate[6] + rate[7] + rate[8] + rate[9]);
		//今のソート内容を表示する
		ViewSortMode(SortMode);
		//操作説明を表示する
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"上下キー:曲選択　左右キー:難易度選択   Enterキー:決定   BackSpaceキー:戻る", Cr[0]);
		else if (lan[4] == 1)DrawString(5, 460, L"↑↓key:music select, ←→key:dif select, Enter key:start, BackSpace key:back to menu", Cr[0]);
		//デバッグ(320,410スタート)
		//DrawFormatString(320, 410, Cr[0], L"%d", SortMode);
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_LSHIFT) == 1 || CheckHitKey(KEY_INPUT_RSHIFT) == 1) { ShiftKey = 1; }
		else { ShiftKey = 0; }
		if (CheckHitKey(KEY_INPUT_P) == 1) { AutoFlag = 1; }
		else { AutoFlag = 0; }
		if (CheckHitKey(KEY_INPUT_UP) == 1) {
			if (key == 0) {
				command[0]--;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				UD = -1;
				startC = GetNowCount();
				//縦コマンド(曲)の端を過ぎたとき、もう片方の端に移動する
				if (command[0] < 0) command[0] = SongNumCount - 1;
				//今選んだ曲の難易度に譜面が無かったら、譜面がある難易度を探す。
				if (strands(SongName[Mapping[command[0]]][command[1]], ST3)) {
					if (strands(SongName[Mapping[command[0]]][0], ST3) != 1) command[1] = 0;
					for (int i = 1; i <= 3; i++) {
						if (strands(SongName[Mapping[command[0]]][i], ST3) != 1) {
							command[1] = i;
							break;
						}
					}
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN) == 1) {
			if (key == 0) {
				command[0]++;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				UD = 1;
				startC = GetNowCount();
				//縦コマンド(曲)の端を過ぎたとき、もう片方の端に移動する
				if (command[0] >= SongNumCount) command[0] = 0;
				//今選んだ曲の難易度に譜面が無かったら、譜面がある難易度を探す。
				if (strands(SongName[Mapping[command[0]]][command[1]], ST3)) {
					if (strands(SongName[Mapping[command[0]]][0], ST3) != 1) command[1] = 0;
					for (int i = 1; i <= 3; i++) {
						if (strands(SongName[Mapping[command[0]]][i], ST3) != 1) {
							command[1] = i;
							break;
						}
					}
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_LEFT) == 1) {
			//左が押された
			if (key == 0) {
				command[1]--;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				LR = 1;
				XstartC = GetNowCount();
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT) == 1) {
			//右が押された
			if (key == 0) {
				command[1]++;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				LR = -1;
				XstartC = GetNowCount();
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_Z) == 1) {
			if (key == 0) {
				ChangeSortMode(&SortMode);
				SortSong(Mapping, SortMode, command[1], level, Hscore, SongNumCount);
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN) == 1) {
			//エンターが押された(曲ファイル名がNULLの場合はスキップ)
			if (key == 0 && strands(SongFileName[Mapping[command[0]]][command[1]], ST3) == 0) {
				//隠し曲用
				if (command[1] == 3 && Hscore[Mapping[command[0]]][3] >= 90000
					&& strands(SongFileName[Mapping[command[0]]][5], ST3) == 0
					&& Hscore[Mapping[command[0]]][5] <= 0) {
					G[0] = 0;
					if (Hscore[Mapping[command[0]]][3] >= 90000
						&& Hscore[Mapping[command[0]]][3] < 92500) {
						G[0] = pals(90000, 0, 92500, 25, Hscore[Mapping[command[0]]][3]);
					}
					else if (Hscore[Mapping[command[0]]][3] >= 92500
						&& Hscore[Mapping[command[0]]][3] < 95000) {
						G[0] = pals(95000, 50, 92500, 25, Hscore[Mapping[command[0]]][3]);
					}
					else if (Hscore[Mapping[command[0]]][3] >= 95000
						&& Hscore[Mapping[command[0]]][3] < 98000) {
						G[0] = pals(95000, 50, 98000, 750, Hscore[Mapping[command[0]]][3]);
					}
					else if (Hscore[Mapping[command[0]]][3] >= 98000
						&& Hscore[Mapping[command[0]]][3] < 99000) {
						G[0] = pals(99000, 1000, 98000, 750, Hscore[Mapping[command[0]]][3]);
					}
					else if (Hscore[Mapping[command[0]]][3] >= 99000) {
						G[0] = 1000;
					}
					if (GetRand(1000) <= G[0]) { command[1] = 5; }
				}
				StopSoundMem(previewM);
				ClearDrawScreen();
				InitSoundMem();
				InitGraph();
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
				next = 6;
				break;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK) == 1) {
			if (key == 0) {
				StopSoundMem(previewM);
				ClearDrawScreen();
				InitSoundMem();
				InitGraph();
				next = 1;
				break;
			}
			key = 1;
		}
		else { key = 0; }
		if (GetWindowUserCloseFlag(TRUE)) {
			next = 5;
			break;
		}
		//横コマンド(難易度)の端を過ぎないようにする
		if (command[1] < 0) {
			command[1] = 0;
			XstartC -= 250;
		}
		else if (command[1] > limit[Mapping[command[0]]]) {
			command[1] = limit[Mapping[command[0]]];
			XstartC -= 250;
		}
		WaitTimer(5);
	}
	//選択曲の保存
	G[0] = _wfopen_s(&fp, L"save/SongSelect2.dat", L"wb");
	fwrite(&Mapping[command[0]], sizeof(int), 1, fp);
	fwrite(&command[1], sizeof(int), 1, fp);
	fclose(fp);
	return next;

	//artist[64]->artist[PackNumLim][SongNumLim]
	//difP[64]->difP[PackNumLim][SongNumLim]
	//GT->ST1
	//GT3->ST4
	//GT5->ST6
	//GT5E->ST6E
	//GT6->ST7
	//GT6E->ST7E
	//GT7->ST8
	//GT8->ST10
	//GT9->ST11
	//GT10->ST12
	//GT12->ST15
	//GT13->ST16
	//GT14->ST3
	//GT15->ST9
	//GT16->ST5
	//GT21->ST1
	//GT20->ST14
	//GT23->ST13
	//jacketP[64]->jacketP[PackNumLim][SongNumLim]
	//level[64]->level[PackNumLim][SongNumLim]
	//mapT->GT2
	//musicT->G[3]
	//preview[64]->preview[PackNumLim][SongNumLim]
	//rimit[64]->limit[PackNumLim][SongNumLim]
	//songM[64]->SongFileName[PackNumLim][SongNumLim]
	//songname[64]->SongName[PackNumLim][SongNumLim]
	//songT->G[0]
}

void ChangeSortMode(int *mode) {
	*mode = *mode + 1;
	if (*mode >= 3) {
		*mode = 0;
	}
}

void SortSong(int *mapping, const int mode, const int dif, const int lv[SongNumLim][6], const int score[SongNumLim][6], const int SongNumCount) {
	int n = 0;
	int m = SongNumCount;
	int o = 0;
	int p = 1;
	switch (mode) {
	case SORT_DEFAULT:
		for (int i = 0; i < SongNumCount; i++) {
			*mapping = i;
			mapping++;
		}
		break;
	case SORT_LEVEL:
		while (p) {
			p = 0;
			for (int i = 0; i < SongNumCount - 1; i += 2) {
				if (lv[mapping[i]][dif] > lv[mapping[i + 1]][dif]) {
					o = mapping[i];
					mapping[i] = mapping[i + 1];
					mapping[i + 1] = o;
					p = 1;
				}
			}
			for (int i = 1; i < SongNumCount - 1; i += 2) {
				if (lv[mapping[i]][dif] > lv[mapping[i + 1]][dif]) {
					o = mapping[i];
					mapping[i] = mapping[i + 1];
					mapping[i + 1] = o;
					p = 1;
				}
			}
		}
		break;
	case SORT_SCORE:
		while (p) {
			p = 0;
			for (int i = 0; i < SongNumCount - 1; i += 2) {
				if (score[mapping[i]][dif] > score[mapping[i + 1]][dif]) {
					o = mapping[i];
					mapping[i] = mapping[i + 1];
					mapping[i + 1] = o;
					p = 1;
				}
			}
			for (int i = 1; i < SongNumCount - 1; i += 2) {
				if (score[mapping[i]][dif] > score[mapping[i + 1]][dif]) {
					o = mapping[i];
					mapping[i] = mapping[i + 1];
					mapping[i + 1] = o;
					p = 1;
				}
			}
		}
		break;
	}
}

void ViewSortMode(const int mode) {
#define POS_X 260
#define POS_Y 20
	switch (mode) {
	case SORT_DEFAULT:
		DrawString(POS_X, POS_Y, L"デフォルト", GetColor(255, 255, 255));
		break;
	case SORT_LEVEL:
		DrawString(POS_X, POS_Y, L"レベル順", GetColor(255, 255, 255));
		break;
	case SORT_SCORE:
		DrawString(POS_X, POS_Y, L"スコア順", GetColor(255, 255, 255));
		break;
	}
}
