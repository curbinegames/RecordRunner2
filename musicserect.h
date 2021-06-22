int musicserect(int *p1) {
	//現在64曲しか入らないので、必要に応じて増やしてね。
	unsigned int Cr[7];
	Cr[0] = Cr[1] = Cr[2] = Cr[4] = Cr[5] = Cr[6] = GetColor(255, 255, 255);
	Cr[3] = GetColor(0, 0, 0);
	int songT, musicT, picsong, e, i, n = 0, UD = 1, LR = 1, key = 1, ShiftKey = 0, next = 99, startC = -250, XstartC = -250, previewC = 0, moveC = 250, XmoveC = 250, command[2] = { 0,1 }, rimit[64], level[64][6], Hscore[64][6], Hdis[64][6];
	int ClearRank[64][6];//0=EX, 1=S, 2=A, 3=B, 4=C, 5=D, 6=not play
	int ClearRate[64][6];//0=not play, 1=droped, 2=cleared, 3=no miss!, 4=full combo!!, 5=perfect!!!
	int G[2];
	double diskr = 0, rate[10], Hacc[64][6];
	int preview[64][6][2];
	wchar_t songname[64][6][256], artist[64][6][256], songM[64][6][256], jacketP[64][6][256], mapT[255], GT2[255], GT4[255], GT11[255], GT18[255];
	wchar_t GT[] = { L"record/" };
	wchar_t GT3[6][7] = { L"/0.txt" ,L"/1.txt" ,L"/2.txt" ,L"/3.txt" ,L"/4.txt" ,L"/5.txt" };
	wchar_t GT5[] = { L"#TITLE:" };
	wchar_t GT5E[] = { L"#E.TITLE:" };
	wchar_t GT6[] = { L"#ARTIST:" };
	wchar_t GT6E[] = { L"#E.ARTIST:" };
	wchar_t GT7[] = { L"#MUSIC:" };
	wchar_t GT8[] = { L"#LEVEL:" };
	wchar_t GT9[] = { L"#PREVIEW:" };
	wchar_t GT10[] = { L"#JACKET:" };
	wchar_t GT12[] = { L"score/" };
	wchar_t GT13[] = { L".dat" };
	wchar_t GT14[] = { L"NULL" };
	wchar_t GT15[] = { L"song/" };
	wchar_t GT16[] = { L"picture/NULL jucket.png" };
	wchar_t GT17[] = { L"picture/" };
	wchar_t GT19[] = { L"/" };
	wchar_t GT20[] = { L"#MAP:" };
	wchar_t GT21[255] = L"record/";
	wchar_t GT22[6][7] = { L"/0.rrs" ,L"/1.rrs" ,L"/2.rrs" ,L"/3.rrs" ,L"/4.rrs" ,L"/5.rrs" };
	wchar_t playingsong[255] = { L"NULL" };
	wchar_t viewingjacket[255] = { L"NULL" };
	int bar[7], difbar[6], difC[5][2], CRatepic[5], CRankpic[6], detail, back, disk, help, select, jacketpic, previewM;
	bar[0] = bar[1] = bar[2] = bar[4] = bar[5] = bar[6] = LoadGraph(L"picture/songbarB.png");
	bar[3] = LoadGraph(L"picture/songbarY.png");
	difbar[0] = LoadGraph(L"picture/difauto.png");
	difbar[1] = LoadGraph(L"picture/difeasy.png");
	difbar[2] = LoadGraph(L"picture/difnormal.png");
	difbar[3] = LoadGraph(L"picture/difhard.png");
	difbar[4] = difbar[5] = LoadGraph(L"picture/difhope.png");
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
	CRatepic[0] = LoadGraph(L"picture/MarkD.png");
	CRatepic[1] = LoadGraph(L"picture/MarkC.png");
	CRatepic[2] = LoadGraph(L"picture/MarkNM.png");
	CRatepic[3] = LoadGraph(L"picture/MarkFC.png");
	CRatepic[4] = LoadGraph(L"picture/MarkP.png");
	CRankpic[0] = LoadGraph(L"picture/MiniEX.png");
	CRankpic[1] = LoadGraph(L"picture/MiniS.png");
	CRankpic[2] = LoadGraph(L"picture/MiniA.png");
	CRankpic[3] = LoadGraph(L"picture/MiniB.png");
	CRankpic[4] = LoadGraph(L"picture/MiniC.png");
	CRankpic[5] = LoadGraph(L"picture/MiniD.png");
	detail = LoadGraph(L"picture/detail.png");
	back = LoadGraph(L"picture/MSback.png");
	disk = LoadGraph(L"picture/disk.png");
	help = LoadGraph(L"picture/help.png");
	jacketpic = LoadGraph(L"picture/NULL jucket.png");
	select = LoadSoundMem(L"sound/arrow.ogg");
	previewM = LoadSoundMem(L"null.mp3");
	PlaySoundMem(previewM, DX_PLAYTYPE_BACK);
	int	lan[5] = { 0,0,0,2,0 };//使うのは[4,言語]だけ
	FILE *fp;
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&lan, sizeof(lan), 5, fp);
		fclose(fp);
	}
	songT = FileRead_open(L"song.txt");
	while (FileRead_eof(songT) == 0) {
		FileRead_gets(GT2, 256, songT);
		rimit[n] = 3;
		for (i = 0; i < 6; i++) {
			strcopy(GT, mapT, 1);
			strcats(mapT, GT2);
			strcopy(mapT, GT18, 1);
			strcats(mapT, GT3[i]);
			musicT = FileRead_open(mapT);
			strcopy(GT14, songname[n][i], 1);
			strcopy(GT14, artist[n][i], 1);
			strcopy(GT14, songM[n][i], 1);
			strcopy(GT16, jacketP[n][i], 1);
			level[n][i] = -1;
			preview[n][i][0] = 441000;
			preview[n][i][1] = 2646000;
			while (FileRead_eof(musicT) == 0) {
				FileRead_gets(GT4, 256, musicT);
				//曲名を読み込む
				if (strands(GT4, GT5) && (lan[4] == 0 || strands(songname[n][i], GT14))) {
					strmods(GT4, 7);
					strcopy(GT4, songname[n][i], 1);
				}
				if (strands(GT4, GT5E) && lan[4] == 1) {
					strmods(GT4, 9);
					strcopy(GT4, songname[n][i], 1);
				}
				//作曲者を読み込む
				if (strands(GT4, GT6) && (lan[4] == 0 || strands(artist[n][i], GT14))) {
					strmods(GT4, 8);
					strcopy(GT4, artist[n][i], 1);
				}
				if (strands(GT4, GT6E) && lan[4] == 1) {
					strmods(GT4, 10);
					strcopy(GT4, artist[n][i], 1);
				}
				//曲ファイル名を読み込む
				if (strands(GT4, GT7)) {
					strcopy(GT15, songM[n][i], 1);
					strmods(GT4, 7);
					strcats(songM[n][i], GT4);
				}
				//難易度を読み込む
				if (strands(GT4, GT8)) {
					strmods(GT4, 7);
					level[n][i] = strsans(GT4);
				}
				//プレビュー時間を読み込む
				if (strands(GT4, GT9)) {
					strmods(GT4, 9);
					preview[n][i][0] = double(strsans(GT4)) / 1000 * 44100.0;
					while (GT4[0] >= '0'&&GT4[0] <= '9') strmods(GT4, 1);
					strmods(GT4, 1);
					preview[n][i][1] = double(strsans(GT4)) / 1000 * 44100.0;
				}
				//ジャケット写真を読み込む
				if (strands(GT4, GT10)) {
					strcopy(GT18, jacketP[n][i], 1);
					strcats(jacketP[n][i], GT19);
					strmods(GT4, 8);
					strcats(jacketP[n][i], GT4);
				}
				//マップに入ったら抜ける
				if (strands(GT4, GT20)) break;
			}
		}
		//ハイスコアを読み込む
		strcopy(GT12, GT11, 1);
		strcats(GT11, GT2);
		strcats(GT11, GT13);
		e = _wfopen_s(&fp, GT11, L"rb");
		for (i = 0; i < 6; i++) {
			Hscore[n][i] = 0;
			Hacc[n][i] = 0;
			Hdis[n][i] = 0;
			ClearRank[n][i] = 6;
			ClearRate[n][i] = 0;
		}
		if (e == 0) {
			fread(&Hscore[n], sizeof(int), 6, fp);
			fread(&Hacc[n], sizeof(double), 6, fp);
			fread(&Hdis[n], sizeof(int), 6, fp);
			fread(&ClearRank[n], sizeof(int), 6, fp);
			fread(&ClearRate[n], sizeof(int), 6, fp);
			fclose(fp);
		}
		if (Hscore[n][5] > 0) rimit[n] = 4;
		FileRead_close(musicT);
		n++;
	}
	n--;
	//ここまで、曲情報の読み込み
	e = _wfopen_s(&fp, L"save/rateN.dat", L"rb");
	if (e == 0) {
		fread(&rate, sizeof(rate), 10, fp);
		fclose(fp);
	}
	for (i = 0; i < 10; i++) if (rate[i] < 0) rate[i] = 0;
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, back, TRUE);
		//時間操作
		moveC = -1 * (GetNowCount() - startC) + 250;
		XmoveC = -1 * (GetNowCount() - XstartC) + 250;
		if (moveC < 0) moveC = 0;
		if (XmoveC < 0) XmoveC = 0;
		//ジャケットを表示する
		if (strands(viewingjacket, jacketP[command[0]][command[1]]) == 0) {
			DeleteGraph(jacketpic);
			strcopy(jacketP[command[0]][command[1]], viewingjacket, 1);
			jacketpic = LoadGraph(viewingjacket);
		}
		DrawGraph(305, 75, jacketpic, TRUE);
		//曲名その他もろもろを表示する
		picsong = command[0] - 3;
		while (picsong < 0) picsong += n + 1;
		for (i = 0; i < 7; i++) {
			DrawGraph((UD*moveC*moveC + 62500 * i - 375000) / 3125, (6 * UD*moveC*moveC + 375000 * i - 531250) / 3125, bar[i], TRUE);
			DrawString((UD*moveC*moveC + 62500 * i - 93750) / 3125, (6 * UD*moveC*moveC + 375000 * i - 468750) / 3125, songname[picsong][command[1]], Cr[i]);
			DrawString((UD*moveC*moveC + 62500 * i - 93750) / 3125, (6 * UD*moveC*moveC + 375000 * i - 406250) / 3125, artist[picsong][command[1]], Cr[i]);
			if (1 <= ClearRate[picsong][command[1]]) {
				DrawGraph((UD*moveC*moveC + 62500 * i - 375000) / 3125 + 275, (6 * UD*moveC*moveC + 375000 * i - 531250) / 3125 + 40, CRatepic[ClearRate[picsong][command[1]] - 1], TRUE);
			}
			if (0 <= ClearRank[picsong][command[1]] && ClearRank[picsong][command[1]] <= 5) {
				DrawGraph((UD*moveC*moveC + 62500 * i - 375000) / 3125 + 275, (6 * UD*moveC*moveC + 375000 * i - 531250) / 3125 + 40, CRankpic[ClearRank[picsong][command[1]]], TRUE);
			}
			DrawFormatString((UD*moveC*moveC + 62500 * i + 62500) / 3125, (6 * UD*moveC*moveC + 375000 * i - 312500) / 3125, Cr[i], L"ClearRank:%d", ClearRank[picsong][command[1]]);
			DrawFormatString((UD*moveC*moveC + 62500 * i + 62500) / 3125, (6 * UD*moveC*moveC + 375000 * i - 312500) / 3125 + 20, Cr[i], L"ClearRate:%d", ClearRate[picsong][command[1]]);
			picsong++;
			if (picsong > n) picsong -= n + 1;
		}
		//難易度マーカーを表示する
		G[1] = 0;
		for (i = 0; i <= 4; i++) {
			G[0] = 0;
			if (command[1] == i) G[0] = 1;
			if (command[1] < i) G[1] = 1;
			if (strands(songM[command[0]][i], GT14) == 0 && i <= rimit[command[0]]) DrawGraph(550 + 11 * G[1] + 16 * i, 290 - 11 * G[0], difC[i][G[0]], TRUE);
		}
		//難易度の名前を表示する
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
		DrawFormatString(500, 380, Cr[3], L"LEVEL:%d", level[command[0]][command[1]]);
		DrawFormatString(340, 410, Cr[3], L"HighSCORE:%d", Hscore[command[0]][command[1]]);
		DrawFormatString(500, 410, Cr[3], L"HighACC:%.2f%%", Hacc[command[0]][command[1]]);
		DrawFormatString(350, 430, Cr[3], L"HighDis:%.3fkm", Hdis[command[0]][command[1]] / 1000.0);
		//プレビューを流す
		if (moveC == 0 && XmoveC == 0 && strands(songM[command[0]][command[1]], GT14) == 0 && strands(playingsong, songM[command[0]][command[1]]) == 0) {
			StopSoundMem(previewM);
			DeleteSoundMem(previewM);
			strcopy(songM[command[0]][command[1]], playingsong, 1);
			previewM = LoadSoundMem(playingsong);
			SetCurrentPositionSoundMem(preview[command[0]][command[1]][0], previewM);
			PlaySoundMem(previewM, DX_PLAYTYPE_BACK, FALSE);
			WaitTimer(30);
		}
		//ディスクを表示する(レート表示場所)
		DrawRotaGraph(640, 25, 1, diskr, disk, TRUE);
		diskr += 0.02;
		if (diskr > 6.28) diskr -= 6.28;
		DrawFormatString(550, 100, Cr[0], L"RATE:%5.2f", rate[0] + rate[1] + rate[2] + rate[3] + rate[4] + rate[5] + rate[6] + rate[7] + rate[8] + rate[9]);
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"上下キー:曲選択　左右キー:難易度選択   Enterキー:決定   BackSpaceキー:戻る", Cr[0]);
		else if (lan[4] == 1)DrawString(5, 460, L"↑↓key:music select, ←→key:dif select, Enter key:start, BackSpace key:back to menu", Cr[0]);
		//デバッグ(320,410スタート)
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_LSHIFT) == 1 || CheckHitKey(KEY_INPUT_RSHIFT) == 1) { ShiftKey = 1; }
		else { ShiftKey = 0; }
		if (CheckHitKey(KEY_INPUT_UP) == 1) {
			//上が押された
			if (key == 0) {
				command[0]--;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				UD = -1;
				startC = GetNowCount();
				//縦コマンド(曲)の端を過ぎたとき、もう片方の端に移動する
				if (command[0] < 0) command[0] = n;
				else if (command[0] > n) command[0] = 0;
				//今選んだ曲の難易度に譜面が無かったら、譜面がある難易度を探す。
				strands(songname[command[0]][command[1]], GT14);
				if (strands(songname[command[0]][command[1]], GT14)) {
					if (strands(songname[command[0]][0], GT14) != 1) command[1] = 0;
					for (i = 1; i <= 3; i++) {
						if (strands(songname[command[0]][i], GT14) != 1) {
							command[1] = i;
							break;
						}
					}
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN) == 1) {
			//下が押された
			if (key == 0) {
				command[0]++;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				UD = 1;
				startC = GetNowCount();
				//縦コマンド(曲)の端を過ぎたとき、もう片方の端に移動する
				if (command[0] < 0) command[0] = n;
				else if (command[0] > n) command[0] = 0;
				//今選んだ曲の難易度に譜面が無かったら、譜面がある難易度を探す。
				strands(songname[command[0]][command[1]], GT14);
				if (strands(songname[command[0]][command[1]], GT14)) {
					if (strands(songname[command[0]][0], GT14) != 1) command[1] = 0;
					for (i = 1; i <= 3; i++) {
						if (strands(songname[command[0]][i], GT14) != 1) {
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
		else if (CheckHitKey(KEY_INPUT_RETURN) == 1) {
			//エンターが押された(曲ファイル名がNULLの場合はスキップ)
			if (key == 0 && strands(songM[command[0]][command[1]], GT14) == 0) {
				//隠し曲用
				if (command[1] == 3 && Hscore[command[0]][3] >= 90000 && strands(songM[command[0]][5], GT14) == 0 && Hscore[command[0]][5] <= 0) {
					G[0] = 0;
					if (Hscore[command[0]][3] >= 90000 && Hscore[command[0]][3] < 92500)G[0] = Hscore[command[0]][3] / 250000.0 * Hscore[command[0]][3] - Hscore[command[0]][3] * 0.72 + 32400;
					else if (Hscore[command[0]][3] >= 92500 && Hscore[command[0]][3] < 95000)G[0] = -1 * Hscore[command[0]][3] / 250000.0 * Hscore[command[0]][3] + Hscore[command[0]][3] * 0.76 - 36050;
					else if (Hscore[command[0]][3] >= 95000 && Hscore[command[0]][3] < 98000)G[0] = Hscore[command[0]][3] / 12857.14 * Hscore[command[0]][3] - Hscore[command[0]][3] * 14.777777 + 701994;
					else if (Hscore[command[0]][3] >= 98000 && Hscore[command[0]][3] < 99000)G[0] = -1 * Hscore[command[0]][3] / 4000.0 * Hscore[command[0]][3] + Hscore[command[0]][3] * 49.5 - 2449250;
					else if (Hscore[command[0]][3] >= 99000)G[0] = 1000;
					if (GetRand(1000) <= G[0])command[1] = 5;
				}
				StopSoundMem(previewM);
				ClearDrawScreen();
				InitSoundMem();
				InitGraph();
				*p1 = command[0];
				p1++;
				*p1 = command[1];
				p1++;
				*p1 = ShiftKey;
				next = 6;
				break;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK) == 1) {
			//バックスペースが押された
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
		else if (GetWindowUserCloseFlag(TRUE)) {
			next = 5;
			break;
		}
		else key = 0;
		//横コマンド(難易度)の端を過ぎないようにする
		if (command[1] < 0) {
			command[1] = 0;
			XstartC -= 250;
		}
		else if (command[1] > rimit[command[0]]) {
			command[1] = rimit[command[0]];
			XstartC -= 250;
		}
		WaitTimer(5);
	}
	return next;
}