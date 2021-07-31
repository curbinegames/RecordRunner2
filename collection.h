int collection();
int C_item();
int C_story();
int story(int, int);

int collection(void) {
	int key = 1, command = 0, end = 0;
	int backimg, icon[4], G[2], help, sel;
	unsigned int Cr;
	Cr = GetColor(255, 255, 255);
	backimg = LoadGraph(L"picture/COLLECT back.png");
	icon[0] = LoadGraph(L"picture/CLI.png");
	icon[1] = LoadGraph(L"picture/CLIG.png");
	icon[2] = LoadGraph(L"picture/CLSG.png");
	icon[3] = LoadGraph(L"picture/CLS.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.ogg");
	int	lan[5] = { 0,0,0,2,0 };//使うのは[4,言語]だけ
	FILE *fp;
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&lan, sizeof(lan), 5, fp);
		fclose(fp);
	}
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		DrawGraph(70, 130, icon[0 + command], TRUE);
		DrawGraph(350, 130, icon[2 + command], TRUE);
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"左右キー:選択   Enterキー:決定   BackSpaceキー:戻る", Cr);
		if (lan[4] == 1)DrawString(5, 460, L"←→key:select   Enter key:enter   BackSpace key:back to menu", Cr);
		ScreenFlip();
		//ここからキー判定
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//左が押された
			if (key == 0) {
				command--;
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//右が押された
			if (key == 0) {
				command++;
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN)) {
			//エンターが押された
			if (key == 0) {
				switch (command) {
				case 0:
					end = C_item();
					break;
				case 1:
					end = C_story();
					break;
				}
			}
			if (end == 5) {
				return 5;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK) == 1) {
			//バックスペースが押された
			if (key == 0) {
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			//閉じるボタンが押された
			return 5;
		}
		else {
			//特定のキーが押されていない
			key = 0;
		}
		if (command < 0) {
			command += 2;
		}
		if (command > 1) {
			command -= 2;
		}
	}
	return 1;
}

int C_item(void) {
	//ロード
	int e, i, j, win, cover, backimg, item[11], G[2], help, sel;
	int flag[10] = { 0,0,0,0,0,0,0,0,0,0 };
	double Grate = 0;
	int	play[7];
	double rate[10];
	int	lan[5] = { 0,0,0,2,0 };//使うのは[4,言語]だけ
	FILE *fp;
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&lan, sizeof(lan), 5, fp);
		fclose(fp);
	}
	win = LoadGraph(L"picture/item枠.png");
	cover = LoadGraph(L"picture/itemカバー.png");
	backimg = LoadGraph(L"picture/COLLECT back.png");
	item[0] = LoadGraph(L"picture/item0.png");
	item[1] = LoadGraph(L"picture/item1.png");
	item[2] = LoadGraph(L"picture/item2.png");
	item[3] = LoadGraph(L"picture/item3.png");
	item[4] = LoadGraph(L"picture/item4.png");
	item[5] = LoadGraph(L"picture/item5.png");
	item[6] = LoadGraph(L"picture/item6.png");
	item[7] = LoadGraph(L"picture/item7.png");
	item[8] = LoadGraph(L"picture/item8.png");
	item[9] = LoadGraph(L"picture/item9.png");
	item[10] = LoadGraph(L"picture/item10.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.ogg");
	e = _wfopen_s(&fp, L"save/data.dat", L"rb");
	if (e == 0) {
		fread(&play, sizeof(play), 7, fp);
		fclose(fp);
	}
	e = _wfopen_s(&fp, L"save/rateN.dat", L"rb");
	if (e == 0) {
		fread(&rate, sizeof(rate), 10, fp);
		fclose(fp);
	}
	for (e = 0; e < 10; e++) if (rate[e] >= 0 && rate[e] <= 15) Grate += rate[e] * 1.0;
	//達成フラグ
	if (play[1] >= 1) flag[0] = 1;
	if (play[3] >= 1) flag[1] = 1;
	if (play[4] >= 1) flag[2] = 1;
	if (play[5] >= 1) flag[3] = 1;
	if (play[6] >= 1) flag[4] = 1;
	if (play[0] >= 1) flag[5] = 1;
	if (play[0] >= 10) flag[6] = 1;
	if (Grate >= 25) flag[7] = 1;
	if (Grate >= 55) flag[8] = 1;
	if (Grate >= 90) flag[9] = 1;
	//動作
	int key = 1;
	int	command[2] = { 0,0 };
	unsigned int Cr;
	Cr = GetColor(255, 255, 255);
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		for (i = 0; i < 2; i++) {
			for (j = 0; j < 3; j++) {
				DrawGraph(110 + j * 150, 20 + i * 240, win, TRUE);
				if (flag[j + i * 7]) {
					DrawGraph(113 + j * 150, 23 + i * 240, item[1 + j + i * 7], TRUE);
				}
				else {
					DrawGraph(113 + j * 150, 23 + i * 240, item[0], TRUE);
				}
				if (j + i * 7 != (7 * command[1] + 2 * command[0]) / 2) {
					DrawGraph(110 + j * 150, 20 + i * 240, cover, TRUE);
				}
			}
		}
		for (i = 0; i < 4; i++) {
			DrawGraph(40 + i * 150, 140, win, TRUE);
			if (flag[3 + i]) DrawGraph(43 + i * 150, 143, item[4 + i], TRUE);
			else DrawGraph(43 + i * 150, 143, item[0], TRUE);
			if (3 + i != 3 * command[1] + command[0] || command[1] != 1) DrawGraph(40 + i * 150, 140, cover, TRUE);
		}
		switch (command[1]) {
		case 0:
			switch (command[0]) {
			case 0:
				if (play[1] >= 1 && lan[4] == 0)DrawString(50, 400, L"割れたレコード:\n衝撃に耐えられずに割れてしまったもの。\nもう使えない…。[1回以上DROPED]", Cr);
				else if (play[1] >= 1 && lan[4] == 1)DrawString(50, 400, L"Broken Record Disc:\nThe broken record disc by shock.\nIt cannot be used... [Finish by DROPED]", Cr);
				else if (play[1] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1回以上DROPED]", Cr);
				else if (play[1] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by DROPED]", Cr);
				break;
			case 1:
				if (play[3] >= 1 && lan[4] == 0)DrawString(50, 400, L"サウンドロップ:\n今、人気のお菓子、マップゲーターも持ち歩いている。\n1袋13pop(110円)。[1回以上CLEARED]", Cr);
				else if (play[3] >= 1 && lan[4] == 1)DrawString(50, 400, L"Soundrop:\nThe popular candy now.\nMapgator have it on his bag. [Finish by CLEARED]", Cr);
				else if (play[3] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1回以上CLEARED]", Cr);
				else if (play[3] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by CLEARED]", Cr);
				break;
			case 2:
				if (play[4] >= 1 && lan[4] == 0)DrawString(50, 400, L"ミルク:\n株式会社myumyuが製造しているミルク。さっぱりした味わいだ。\nピッカーの好物。[1回以上NO MISS]", Cr);
				else if (play[4] >= 1 && lan[4] == 1)DrawString(50, 400, L"Milk:\nThe milk made by company \"myumyu\". It have tidy taste.\nPicker like it. [Finish by NO MISS]", Cr);
				else if (play[4] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1回以上NO MISS]", Cr);
				else if (play[4] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by NO MISS]", Cr);
				break;
			}
			break;
		case 1:
			switch (command[0]) {
			case 0:
				if (play[5] >= 1 && lan[4] == 0)DrawString(50, 400, L"ワッフル:\nすべての音を調和したときにできる結晶。\nとてもおいしいが、味の好みが分かれるらしい。[1回以上FULL COMBO]", Cr);
				else if (play[5] >= 1 && lan[4] == 1)DrawString(50, 400, L"Waffle:\nThe crystal produced by harmoning all sound.It's very good \ntaste but some people feel that a bad. [Finish by FULL COMBO]", Cr);
				else if (play[5] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1回以上FULL COMBO]", Cr);
				else if (play[5] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by FULL COMBO]", Cr);
				break;
			case 1:
				if (play[6] >= 1 && lan[4] == 0)DrawString(50, 400, L"パフェ:\n音を完璧に調和したときにできる結晶。\nテイラーの大好物。とってもあまい。[1回以上PERFECT]", Cr);
				else if (play[6] >= 1 && lan[4] == 1)DrawString(50, 400, L"Parfait:\nThe parfait produced by harmoning perfectly the sound.\nTaylor is love it. It's very sweet. [Finish by PERFECT]", Cr);
				else if (play[6] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1回以上PERFECT]", Cr);
				else if (play[6] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by PERFECT]", Cr);
				break;
			case 2:
				if (play[0] >= 1 && lan[4] == 0)DrawString(50, 400, L"ビギナーランナー証明カード:\nランナーであることを証明するもの。\n入ったばかりの初心者さんの証。[1回以上プレイ]", Cr);
				else if (play[0] >= 1 && lan[4] == 1)DrawString(50, 400, L"Beginner Runner Card:\nThe card indicating that you are a Runner.\nAnd the beginner just joined. [Play the game]", Cr);
				else if (play[0] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1回以上プレイ]", Cr);
				else if (play[0] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Play the game]", Cr);
				break;
			case 3:
				if (play[0] >= 10 && lan[4] == 0)DrawString(50, 400, L"ランナー手帳:\nランナーの記録手帳。事後調査や報告もこれで済ませる。\n走りながらメモする上級者もいる。[10回以上プレイ]", Cr);
				else if (play[0] >= 10 && lan[4] == 1)DrawString(50, 400, L"Runner's Note:\nRunner's comprehensive note. Job reports are written on this.\nSome experts write while running. [Play the game 10 times]", Cr);
				else if (play[0] < 10 && lan[4] == 0)DrawString(50, 400, L"???\n[10回以上プレイ]", Cr);
				else if (play[0] < 10 && lan[4] == 1)DrawString(50, 400, L"???\n[Play the game 10 times]", Cr);
				break;
			}
			break;
		case 2:
			switch (command[0]) {
			case 0:
				if (Grate >= 25 && lan[4] == 0)DrawString(50, 400, L"ブロンズランナー証明カード:\nランナーに慣れてきた人に贈られるカード。\nこれでやっとランナーとして認められる。[レート25.00達成]", Cr);
				else if (Grate >= 25 && lan[4] == 1)DrawString(50, 400, L"Bronze Runner Card:\nThe card given to Runner who are used to work.\nYou are admitted as the Runner. [Reach 25.00 rate]", Cr);
				else if (Grate < 25 && lan[4] == 0)DrawString(50, 400, L"???\n[レート25.00達成]", Cr);
				else if (Grate < 25 && lan[4] == 1)DrawString(50, 400, L"???\n[Reach 25.00 rate]", Cr);
				break;
			case 1:
				if (Grate >= 55 && lan[4] == 0)DrawString(50, 400, L"シルバーランナー証明カード:\nランナーとして十分功績を挙げた人に贈られるカード。\n難しい仕事も来るようになる。[レート55.00達成]", Cr);
				else if (Grate >= 55 && lan[4] == 1)DrawString(50, 400, L"Silver Runner Card:\nThe card given to great Runner.\nDifficult job will come to you. [Reach 55.00 rate]", Cr);
				else if (Grate < 55 && lan[4] == 0)DrawString(50, 400, L"???\n[レート55.00達成]", Cr);
				else if (Grate < 55 && lan[4] == 1)DrawString(50, 400, L"???\n[Reach 55.00 rate]", Cr);
				break;
			case 2:
				if (Grate >= 90 && lan[4] == 0)DrawString(50, 400, L"ゴールドランナー証明カード:\nランナーを極めた人に贈られるカード。\n世界各地のかなり難しい仕事も来るようになる。[レート90.00達成]", Cr);
				else if (Grate >= 90 && lan[4] == 1)DrawString(50, 400, L"Gold Runner Card:\nThe card given to EXCELLENT Runner. More difficult \njob all over the world will come to you. [Reach 90.00 rate]", Cr);
				else if (Grate < 90 && lan[4] == 0)DrawString(50, 400, L"???\n[レート90.00達成]", Cr);
				else if (Grate < 90 && lan[4] == 1)DrawString(50, 400, L"???\n[Reach 90.00 rate]", Cr);
				break;
			}
			break;
		}
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"上下左右キー:選択   BackSpaceキー:戻る", Cr);
		else if (lan[4] == 1)DrawString(5, 460, L"←↓↑→key:select   BackSpace key:back", Cr);
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//左が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[0]--;
				if (command[0] < 0) {
					command[0] = 3;
				}
				if (command[0] == 3 && command[1] == 0) {
					command[0] = 2;
				}
				if (command[0] == 3 && command[1] == 2) {
					command[0] = 2;
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//右が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[0]++;
				if (command[0] > 3) {
					command[0] = 0;
				}
				if (command[0] == 3 && command[1] == 0) {
					command[0] = 0;
				}
				if (command[0] == 3 && command[1] == 2) {
					command[0] = 0;
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_UP)) {
			//上が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[1]--;
				if (command[0] == 3 && command[1] == 0) {
					command[0] = 2;
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN)) {
			//下が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[1]++;
				if (command[0] == 3 && command[1] == 2) {
					command[0] = 2;
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK)) {
			//バックスペースが押された
			if (key == 0) {
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			//閉じるボタンが押された
			return 5;
		}
		else {
			//特定のキーが押されていない
			key = 0;
		}
		if (command[1] < 0) {
			command[1] = 2;
		}
		if (command[1] > 2) {
			command[1] = 0;
		}
	}
	return 0;
}

int C_story(void) {
	int e, i, j, end = 0, key = 1, backimg, noteimg, help, sel, chac[3] = { 0,0,0 }, sub1[8], command[2] = { 0,0 }, g[7];
	int StoryUpper[5] = { 10,10,10,4,2 };
	wchar_t chan[5][9] = { L"ピッカー",L"マップゲーター",L"テイラー",L"サブストーリー1",L"EX ミッション" };
	wchar_t chanE[5][12] = { L"Picker",L"Mapgator",L"Taylor",L"Sub Story 1",L"EX mission" };
	unsigned int Cr, Crw;
	int	lan[5] = { 0,0,0,2,0 };//使うのは[4,言語]だけ
	FILE *fp;
	e = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (e == 0) {
		fread(&lan, sizeof(lan), 5, fp);
		fclose(fp);
	}
	//キャラプレイ回数読み込み
	e = _wfopen_s(&fp, L"save/chap.dat", L"rb");
	if (e == 0) {
		fread(&chac, sizeof(chac), 3, fp);
		fclose(fp);
	}
	//サブストのスコア読み込み
	for (i = 0; i < 8; i++) {
		sub1[i] = 0;
		switch (i) {
		case 0:
			e = _wfopen_s(&fp, L"score/snow town story.dat", L"rb");
			g[6] = 1;
			break;
		case 1:
			e = _wfopen_s(&fp, L"score/グラデーション・ワールド.dat", L"rb");
			g[6] = 1;
			break;
		case 2:
			e = _wfopen_s(&fp, L"score/What Color Is The Sky？.dat", L"rb");
			g[6] = 1;
			break;
		case 3:
			e = _wfopen_s(&fp, L"score/トリノユメ.dat", L"rb");
			g[6] = 2;
			break;
		case 4:
			e = _wfopen_s(&fp, L"score/トリノユメ.dat", L"rb");
			g[6] = 4;
			break;
		case 5:
			e = _wfopen_s(&fp, L"score/トリノユメ.dat", L"rb");
			g[6] = 5;
			break;
		}
		if (e == 0) {
			fread(&g, sizeof(int), 6, fp);
			fclose(fp);
			for (j = g[6]; j <= 5; j++) sub1[i] = mins(sub1[i], g[j]);
		}
		switch (i) {
		case 0:
			e = _wfopen_s(&fp, L"score/snow town story.dat", L"rb");
			g[6] = 1;
			break;
		case 1:
			e = _wfopen_s(&fp, L"score/Gradation-world.dat", L"rb");
			g[6] = 1;
			break;
		case 2:
			e = _wfopen_s(&fp, L"score/What Color Is The Sky.dat", L"rb");
			g[6] = 1;
			break;
		case 3:
			e = _wfopen_s(&fp, L"score/Torinoyume.dat", L"rb");
			g[6] = 2;
			break;
		case 4:
			e = _wfopen_s(&fp, L"score/Torinoyume.dat", L"rb");
			g[6] = 4;
			break;
		case 5:
			e = _wfopen_s(&fp, L"score/Torinoyume.dat", L"rb");
			g[6] = 5;
			break;
		case 6:
			e = _wfopen_s(&fp, L"score/drop DOWN DOWN.dat", L"rb");
			g[6] = 4;
			break;
		case 7:
			e = _wfopen_s(&fp, L"score/drop DOWN DOWN.dat", L"rb");
			g[6] = 5;
			break;
		}
		if (e == 0) {
			fread(&g, sizeof(int), 6, fp);
			fclose(fp);
			for (j = g[6]; j <= 5; j++) sub1[i] = mins(sub1[i], g[j]);
		}
	}
	Cr = GetColor(0, 0, 0);
	Crw = GetColor(255, 255, 255);
	backimg = LoadGraph(L"picture/COLLECT back.png");
	noteimg = LoadGraph(L"picture/Cnote.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.ogg");
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		DrawGraph(160, 0, noteimg, TRUE);
		DrawFormatString(5, 5, Cr, L"%d/%d", command[0], command[1]);
		if (lan[4] == 0)DrawFormatString(330, 220, Cr, L"%s", chan[command[1]]);
		else if (lan[4] == 1)DrawFormatString(330, 220, Cr, L"%s", chanE[command[1]]);
		if (command[1] < 3) {
			if (command[0] == 0) {
				if (chac[command[1]] >= 1) {
					if (lan[4] == 0)DrawString(300, 245, L"エピソード1-1", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 1-1", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawFormatString(300, 245, Cr, L"%sを使って\n1回プレイしよう。", chan[command[1]]);
					else if (lan[4] == 1)DrawFormatString(300, 245, Cr, L"Play the game\nwith %s.", chanE[command[1]]);
					e = 0;
				}
			}
			else if (1 <= command[0] && command[0] <= 6) {
				if (chac[command[1]] >= command[0] * 2) {
					if (lan[4] == 0)DrawFormatString(300, 245, Cr, L"エピソード%d-%d", command[0] / 5 + 1, command[0] % 5 + 1);
					else if (lan[4] == 1)DrawFormatString(300, 245, Cr, L"Episode %d-%d", command[0] / 5 + 1, command[0] % 5 + 1);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawFormatString(300, 245, Cr, L"%sを使って\n%d回プレイしよう。", chan[command[1]], command[0] * 2);
					else if (lan[4] == 1)DrawFormatString(300, 245, Cr, L"Play the game\n%d times with %s.", command[0] * 2, chanE[command[1]]);
					e = 0;
				}
			}
			else {
				if (chac[command[1]] >= command[0] * 3 - 7) {
					if (lan[4] == 0)DrawFormatString(300, 245, Cr, L"エピソード%d-%d", command[0] / 5 + 1, command[0] % 5 + 1);
					else if (lan[4] == 1)DrawFormatString(300, 245, Cr, L"Episode %d-%d", command[0] / 5 + 1, command[0] % 5 + 1);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawFormatString(300, 245, Cr, L"%sを使って\n%d回プレイしよう。", chan[command[1]], command[0] * 3 - 7);
					else if (lan[4] == 1)DrawFormatString(300, 245, Cr, L"Play the game\n%d times with %s.", command[0] * 3 - 7, chanE[command[1]]);
					e = 0;
				}
			}
		}
		else if (command[1] == 3) {
			switch (command[0]) {
			case 0:
				if (sub1[command[0]] >= 90000) {
					if (lan[4] == 0)DrawString(300, 245, L"エピソード1", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 1", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"snow town story[EASY]以上で\nAランクを取ろう", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath A RANK at\nsnow town story[EASY] or more dif.", Cr);
					e = 0;
				}
				break;
			case 1:
				if (sub1[command[0]] >= 90000) {
					if (lan[4] == 0)DrawString(300, 245, L"エピソード2", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 2", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"グラデーション・ワールド[EASY]以上で\nAランクを取ろう", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath A RANK at\nGradation-world[EASY] or more dif.", Cr);
					e = 0;
				}
				break;
			case 2:
				if (sub1[command[0]] >= 95000) {
					if (lan[4] == 0)DrawString(300, 245, L"エピソード3", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 3", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"What Color Is The Sky?[EASY]以上で\nSランクを取ろう", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath S RANK at\nWhat Color Is The Sky?[EASY] or more dif.", Cr);
					e = 0;
				}
				break;
			case 3:
				if (sub1[command[0]] >= 90000) {
					if (lan[4] == 0)DrawString(300, 245, L"エピソード4", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 4", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"トリノユメ[NOAMAL]以上で\nAランクを取ろう", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath A RANK at\nTorinoyume[NOAMAL] or more dif.", Cr);
					e = 0;
				}
				break;
			}
		}
		else if (command[1] == 4) {
			switch (command[0]) {
			case 0:
				if (sub1[4] >= 98000) {
					DrawString(300, 245, L"appendix-1", Cr);
					e = 1;
				}
				else if (sub1[5] >= 1) {
					if (lan[4] == 0)DrawString(300, 245, L"トリノユメ[HOPENESS]で\nEXランクを取れ", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath EX RANK at\nTorinoyume[HOPENESS].", Cr);
					e = 0;
				}
				else if (sub1[3] >= 90000) {
					if (lan[4] == 0)DrawString(300, 245, L"トリノユx功簾M_^簸]で\nEXランクを取れ", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath EX RANK at\nTorim.v?&1/0)\\$cRD].", Cr);
					e = 0;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"刈ョ切簾M_^簸]\ne\\ny×蘭区奪", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Rns(}1)Op&rrZ\ne\\ny456Ge/t0Z<=00ax(Q).", Cr);
					e = 0;
				}
				break;
			case 1:
				if (sub1[7] >= 98000) {
					DrawString(300, 245, L"appendix-2", Cr);
					e = 1;
				}
				else if (sub1[6] >= 1) {
					if (lan[4] == 0)DrawString(300, 245, L"drop DOWN DOWN[NIGHTMARE]で\nEXランクを取れ", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath EX RANK at\ndrop DOWN DOWN[NIGHTMARE].", Cr);
					e = 0;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"↓↓↓\n↓↓↓↓↓↓", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"↓↓↓\n↓↓↓↓↓↓", Cr);
					e = 0;
				}
				break;
			}
		}
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"上下左右キー:選択   Enterキー:決定   BackSpaceキー:戻る", Crw);
		else if (lan[4] == 1)DrawString(5, 460, L"←↓↑→key:select   Enter key:enter   BackSpace key:back", Crw);
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//左が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[1]--;
				command[0] = 0;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//右が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[1]++;
				command[0] = 0;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_UP)) {
			//上が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[0]--;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN)) {
			//下が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[0]++;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN)) {
			//エンターが押された
			if (key == 0 && e == 1) end = story(command[0], command[1]);
			if (end == 5) return 5;
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK)) {
			//バックスペースが押された
			if (key == 0) {
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
		//閉じるボタンが押された
		else if (GetWindowUserCloseFlag(TRUE)) return 5;
		//特定のキーが押されていない
		else key = 0;
		if (command[0] < 0) command[0] = StoryUpper[command[1]] - 1;
		if (command[0] > StoryUpper[command[1]] - 1) command[0] = 0;
		if (command[1] < 0) command[1] = 4;
		if (command[1] > 4) command[1] = 0;
	}
	return 0;
}

int story(int a, int b) {
	int key = 1, com = 0, Cx = 220, Cy = 75, backimg, noteimg, pageimg, help, sel;
	unsigned int Cr, Crw;
	Cr = GetColor(0, 0, 0);
	Crw = GetColor(255, 255, 255);
	backimg = LoadGraph(L"picture/COLLECT back.png");
	noteimg = LoadGraph(L"picture/Snote.png");
	pageimg = LoadGraph(L"picture/page.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.ogg");
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		DrawGraph(160, 0, noteimg, TRUE);
		//文字は1行20文字、1ページ20行、3ページまで。
		/*case テンプレ:
		DrawString(Cx, Cy, L"ああ", Cr);
		break;*/
		switch (10 * a + 1000 * b + com) {
			//ピッカー
		case 0:
			DrawString(Cx, Cy, L"「市立松坂音楽高等学校前」\n\n　ボクが乗っているバスは、このバス停の\n前で止まった。ボクは席から降りて、同じ\nバス停で降りる学生に紛れて、運賃箱がある\n前方へ向かった。運賃と整理券を握りしめて\nいる右手が、心なしか小さく震えている\nような気がした。その原因は、緊張なのか、\n期待なのか、よくわからなかった。\n　息苦しいバスから降りて、澄んだ空気が\n充満する外へ出た。そしてボクは、学校へ\n向かう学生とは反対方向へ歩を進めた。\nボクは学校に用があるんじゃない。\n目指すは、この学校の裏にある、「ランナー\n事務所松坂支所」だ。", Cr);
			break;
		case 1:
			DrawString(Cx, Cy, L"　この世界では、音を形に変えて売る商売が\n盛んだ。お菓子にしたり、おもちゃに\nしたり、大きいところだと発電にも使って\nいるんだとか。音を形にする技術を考えた\n人は、世界的な表彰を受けたのだとか。いい\nよなー、頭がいい人は。ボクなんかドッヂ\nボールとピアノくらいしか取り柄がないん\nだもんなー。おっと、ボクのことは\n置いといて…。\n　そんな中、やっぱり悪いことを考える\nやつがいるものだ。音を形にする技術を\n使って、犯罪や強盗、治安が悪いところじゃ\nテロにも使われているらしい。本当なら\n警察が動くんだけど、音楽を相手にできる\n警察はなかなかいないらしい。\n　そこで、ランナーの出番だ。ランナーは\n音楽の専門職業。音楽に関するちょっとした\n困りごとや大規模な事件まで、何でも取り\n締まるのがランナーだ。", Cr);
			break;
		case 2:
			DrawString(Cx, Cy, L"　ボクのお父さんも、ランナーの仕事を\nしていた。結構えらい地位に立っていた\nらしい。そりゃ給料もバカみたいに高かった\nらしい。ランナーを辞めても遊んで暮らせる\nくらいの給料だったらしいけど、お父さんは\nランナーを辞めなかった。\n　そんなお父さんが行方不明になったのは、\n大体３年前のことだった。本当に急だった。\n仕事先で通話が切れてしまい、それっきり\nらしい。この時お父さんは、音を使った\n発電所の暴走を止めに行っていたらしい。\n通話が切れた後、すぐに駆け付けた他の\nランナーによると、遺体どころか、身に\n着けていたものすべてが見つからなかった\nらしい。生死は絶望的だったけど、\nもしかしたら生きているのかも？\n　ランナー試験にも合格して、やっと就職する\nことができた。単純にランナーになってみたい\nというのもあるけど、一番の目的はお父さん\n探し。果てしない目標だけど、がんばるぞ。", Cr);
			break;
		case 10:
			DrawString(Cx, Cy, L"「ランナー事務所松坂支所」\n２階建ての建物の扉の横に、こう書いて\nある。間違いない、ここが事務所のようだ。\nえっと、勝手に入っていいのかな…？\nそれとも、どこかにインターホンがあったり\nして…？そう考えていると…、\n「あっ、どうぞお入りくださいー♪」\nとても軽い声が上にあるスピーカーから\n流れた。よく見たらカメラまである。\n見られてた…。恥ずかしー。\n　とりあえず、扉を押して中に入った。\n中は思ったより狭い。そして人もいない。\n「えっと、新人さんですよね？ちょっと\nそのままでお待ちくださいー♪」\nまたさっきの声だ。今度はどこにカメラが\nあるんだろう…？そう思いつつも待つことに\nした。", Cr);
			break;
		case 11:
			DrawString(Cx, Cy, L"　しばらくして、\n「おー！お前が新人かー！思ったより細い体\nしてんなー」\n近くにあった階段から降りてきたこの黄色い\nワニは、ボクを見るなりいきなりこんな\nことを言ってきた。\n「ほっといてください…」\n対するこのワニは、腕も足も体も太くて、\nスポーツ選手でもやっていけそうな体をして\nいる。正直羨ましい。\n「俺はマップゲーター。長いからゲーターで\nいいぞ。お前は？」\n「ピッカーです」\n「ピッカーな、よし覚えた。そうだ、\nLINEとかやってる？」\n「(交換したく)ないです」\n「そうか、それじゃピッチャー、早速この\n事務所を案内するぜ」\n名前覚えれてないじゃん…。頭は良く\nなさそう。", Cr);
			break;
		case 12:
			DrawString(Cx, Cy, L"　ゲーターに連れられて、ボクは事務所の\n２階にやってきた。１階には受付や待合室が\nあったけど、ここにはランナーの待機部屋や\n会議室、さらにトレーニングルームまで\nあった。\n「ランナーは音楽専門ってよく言うけど、\n体力もいるんだぜ。ここでちったぁ体鍛え\nとけよ、ピッチャー」\n「ピッカーです」\nさて、多分これで事務所は全部見て回った\nかな？そう思ってると。\n「じゃ、次は地下だ。ついて来い」\nおっと、ここって地下にも部屋があるんだ。\n思ったより広いんだなー。\n\nあ、待って、そういえば…。\n\nこの事務所に入ってから、ゲーター以外に\n会っていない…。", Cr);
			break;
		case 20:
			DrawString(Cx, Cy, L"　地下にあったのは、地上の雰囲気とは全く\n違う、機械だらけの部屋だった。部屋の壁に\n沿っていろいろなロボットが並んでいて、\n部屋の片隅には５個ほどモニターが並んで\nいた。\n「ピーター、あれを見てみろ」\n「ピッカーです。なんですか？」\nゲーターが指さした先にあったのは、鎧を\n付けた赤いドラゴンのようなロボット\nだった。\n「近づいてよーく見てみろ。いいから」\n…ゲーターがにやにやしながら言った。\n嫌な予感を感じつつも、言われるままその\nロボットに近づいたみた。すると、\n「うわっ！？」\n突然そのロボットが動き出した。閉じていた\n羽を大きく伸ばして、床に付けていた体を\nゆっくり起こして、\n「やぁ、びっくりしたかい？」\nこう言った。とびらの前で聞いた声だった。", Cr);
			break;
		case 21:
			DrawString(Cx, Cy, L"「さてと、改めまして、ランナー事務所に\nようこそ！ボクがここの管理人のテイラー\nだよ。よろしくね♪」\n「よ、よろしくお願いします」\n予想外の出来事にちょっと驚いてしまった。\n「ゲーターに会って早々連絡先とか聞かれ\nたりしたんじゃない？ごめんね、彼は悪気は\nないのよ♪」\n「あ、まぁ、はい」\n「そうだ、私って生き物なのかロボット\nなのかよく聞かれるんだけど、先に答えて\nおくね♪私はその二つの中間。つまり\nサイボーグなの。まぁ、別に気にしなくても\nいいんだけどね♪」\n「へぇ、そうなんですか」\nこのドラゴンはよくしゃべるなぁ。", Cr);
			break;
		case 22:
			DrawString(Cx, Cy, L"「まぁ、これでここの紹介は終わりだな」\nゲーターが言った。\n「ありがとうございます。ところで、ここ\nって他に人はいないんですか？」\n「いないぜ。俺ら二匹だけだ。もともとこの\n近くは治安がいいからな。俺たちの出番は\nほとんどないのさ」\n「それじゃ、仕事もあまりないの？」\n「そうでもないぜ。最近は音が溢れすぎ\nてて、それを集めてまわるのが俺たちの\n仕事だ」\nそういえば、道に音のかけらが落ちてた\nことがあるなぁ。そっか、ランナーが回収\nしてたのか。\n「それじゃ、早速やってみるか？テイラー、\nサポートできるよな？」\n「できるけど、めんどくさい♪」\n「やっぱダメか。じゃ、俺がサポートして\nやるか」\nこうして、早速仕事が始まった。", Cr);
			break;
		case 30:
			DrawString(Cx, Cy, L"　ゲーターが事務所の中にあった車を運転\nして、誰もいない森の中へ向かった。\nゲーターによると、ポイ捨てが頻繁に\n起きてるらしい。\n「あんなところでこそこそ捨てるなんて\nよぉ、どうせ捨てるなら街中で捨てろよ」\nとゲーターが愚痴をこぼす。いや、それは\nそれで問題だと思うけど。\n「ポイ捨てを見つけたら捕まえるの？」\n「当たり前だろぉ？でもポイ捨てって、\n高くても400pop(約3300円)の罰金で済んじ\nまうんだ。無くならねぇんだよなぁ、ポイ\n捨て。まぁ、ボーナス出るからいいけど」\n\n　そして、だいたい30分後。目的地に\n着いた。", Cr);
			break;
		case 31:
			DrawString(Cx, Cy, L"　ポイ捨てが多い、とは聞いたけど、見た\nところそんな雰囲気はなく、むしろきれい\nだった。鳥の鳴き声も聞こえるし、草木も\n元気そうだった。\n「ねぇ、本当にポイ捨てされてるの？」\nボクは聞いた。\n「きれいに見えるだろ？でもされてるんだ\nなぁ、これが。ポイ捨てっつっても見えない\n場所に捨てるからな。まぁ、ランナーには\nバレバレだけどな」\n「…もう見つけたの？」\n「今から見つけるんだよ」\nゲーターがそういって車から取り出した\nのは、紫色のヘッドホンと、円盤が二枚\n付いたDJキット。DJキットの電源を\n入れると、ブゥゥーーン、という音がして、\nキットが宙に浮いた。\n「さて、今から走るぞ。チーター、ついて\n来いよ！」\n「ピッカーです」", Cr);
			break;
		case 32:
			DrawString(Cx, Cy, L"　ゲーターが走り出したと同時に、DJキット\nから音楽が流れてきた。笛の音を中心とした\n曲、この森にぴったりの曲だった。そして、\nこの曲に合わせるかのように、鳥、リス、\n他の動物たち、さらに草木まで、それぞれの\n音を奏で始めた。走りながら聞いていて、\nボクまで歌ってしまいたくなるほどだった。\n全然知らない曲だけど。\n　そんな時だった。この曲を遮るかのように\n雑音がどこからか聞こえてきた。ゲーターは\nいち早くその場所を特定し、そこに\n向かった。そしてそこにあったのは、誰かが\n捨てたであろう、音のかけらだった。自分も\nなんとか音を出そうとしているようだが、\nうまく出せていなくて、それが雑音になって\n聞こえていたのだ。\n　ゲーターは演奏しながら器用に回収して、\nまた次の場所へ向かう。この曲が終わる\nまで、ずっとこれの繰り返しだった。\nこれが、ランナーの仕事…。", Cr);
			break;
		case 40:
			DrawString(Cx, Cy, L"「終わったぜ、変わりないか？」\n「お疲れさん。こっちは問題ないよ♪」\n演奏が終わって、かなりの音のかけらを\n集めたボクたちは、すぐに事務所に帰った。\n集めた音のかけらは、これから業者に売る\nらしい。\n「これだけ集めれたら、ざっと150pop\n(約1250円)くらいになるかな。大漁だね♪」\n「レートは？」\n「上がんないよ♪」\n「あれでもダメかよぉ、厳しいなぁ…」\n「いつになったら私を抜けるかなー♪」\n「見てろよ、すぐに抜いてやるからな」\n話しているゲーターとテイラーをよそに、\nボクはかけらを見ていた。さっきまで騒がし\nかったかけらが、今は静かにたたずんで\n「あ、もしかしてほしい？一個5pop(約40円)\nで売るよ♪」\n「おいおい、それはぼったくりだろｗ」", Cr);
			break;
		case 41:
			DrawString(Cx, Cy, L"「そういえばキッカー、お前目標とかある\nのか？」\nゲーターが聞いてきた。\n「ピッカーです。目標かぁ、とにかく、\nお父さん探しをしたいんだけど、まずどう\nすればいいんだろう？」\n「お父さんもランナーなのか？」\n「それなら、名前とかがあれば探せるよ♪」\nテイラーが言った。\n「それじゃ、私の部屋に来て。一緒に\n探してあげる♪」\n「え、でも、売りに行くんじゃないの？」\n「そんなのゲーターに任せたらいい\nでしょ♪早速行こうか♪」\nボクはテイラーに連れられて地下に行った。", Cr);
			break;
		case 42:
			DrawString(Cx, Cy, L"「えーと、本当にこの人なの？」\n「うん、そうだけど」\nモニターに映っているのは、ボクのお父さん\nの顔写真と、なんかいろいろと文字が並んで\nいた。多すぎて全部を読むのはめんどくさい。\n要するに、いろいろ功績をあげていて、\nかなり上の地位にいますよ的なことが書いて\nある。\n　そして、事故に遭ってから行方不明だという\nことも書いてある。\n「あまり言いたくないけど、亡くなってるかも\nしれないよ。それでも探すのかい？」\n「うん、そのためにランナーになったから」\n探しに行くことに意味がある。少なくとも、\nボクはそう思ってる。\n\n　さぁ、これから頑張るぞ。", Cr);
			break;
		//EP2
		case 50:
			DrawString(Cx, Cy, L"「小学校の演奏会ですか?」\n「そうだよ、久しぶりに\nランナーらしいお仕事が来たね♪」\n　ランナーのみんなとの夕食中に、\nテイラーが依頼の話を持ち出してきた。\nランナーは音楽の専門という\nこともあり、こういった相談や依頼が\n来ることがあるそうだ。\n「よっしゃ!久々に大暴れできるんだな!」\n　ゲーターがノリノリで言ってると。\n「あ、私たちは小学生に教えるだけだよ♪」\n「はぁ!? なんでだよ? 」\nテイラーに一喝されて机を台パン。やめてよ。\n「主役は小学生だよ。ランナーが出たら\nまずいでしょ♪」\n「はぁ～、つまんねーの」\n今度は椅子を前後にゆらゆら。\nコケないかどうか心配になる。", Cr);
			break;
		case 51:
			DrawString(Cx, Cy, L"　今日のボクの夕食はキノコサラダと\n枝豆ご飯。あとプリン。一度にそんなに\n食べれないから、みんなよりは量は少なめ。\nゲーターはがっつり肉食…かと思いきや、\n肉も野菜も同じくらい食べてる。でも、\n明らかに量が多い。私がいた学校にも\n大食いはいたけど、その倍の量はある。\nそれでも毎回全部食べてるから凄い。\n一方テイラーもきちんと食事はとってる。\n機械だとは聞いてたけど…\n「機械なのは半分だけ。\nご飯は食べないといけないんだ♪」\nだそうだ。意外。", Cr);
			break;
		case 52:
			DrawString(Cx, Cy, L"「でも、ボクうまく教えられるかな?」\n「ピッカー、音楽高校卒業したんだろ?\n大丈夫、自信持てって」\n「そうそう、難しいこと考えないの♪」\n「さて、俺は部屋に戻って機材の調整して\nくるぜ、ごちそうさま」\nゲーターはそう言って部屋に戻っていった。\n「ゲーター、相当張り切ってるね♪」\n「え、そうなんですか?」\nテイラーが急に言い出した。\nそんな風には見えなかったけどなぁ。\n「いつもなら夕食は飽きるまで喋ってる\nでしょ。ゲーター今頃部屋で作戦立ててるよ。\n私にも相談してくれればいいのに、ひとりで\nやるとか意地張っちゃってね」\nテイラーはその後、大きなあくびを一つ。\n「ふぁー、ちょっと早いけど今日は\nもう寝ようかな。お休みピッカー♪」\n「あ、はい。おやすみなさい」", Cr);
			break;
		case 60:
			DrawString(Cx, Cy, L"　6月28日、ボクたちは私立東松坂小学校に\n来ていた。\n「今日はランナーの人たちと一緒に練習\nしますよ。それじゃランナーさん、あとは\nよろしくお願いします」\n　先生はそう言って体育館を出て行った…、\nえ、出て行っちゃうの!?水曜日にやった\n打ち合わせと違う。ゲーターは慌ててるし、\nテイラーもちょっと困ってるみたい。\n「そ、それじゃ、まずはそれぞれ自主練習ね。\n私たちは順番に見ていくからね♪」\n　テイラーがうまい具合に気を利かせて\nくれたおかげで、何とかこの場は切り抜けた。\n「予定がかなり変わっちゃっね。私は左の\n子から教えるから、ゲーターは右の子から\nお願いね。ピッカーはどっちからやる? 」\n　こういう切り替えの早さがテイラーの\n凄いところだよね。\n「ボクも左からやります」", Cr);
			break;
		case 61:
			DrawString(Cx, Cy, L"　周っていると、テイラーが鉄琴を\n担当している一人に声をかけた。\n「ねぇねぇ、演奏は悪くないんだけど、\n慎重になりすぎてるね」\n「え? そんなことないよ」\n「そう? でも、レガートに演奏出来たら\nもっと良くなるよ♪」\n「……レガート? 」\n　えっと、言いたいことは分かるけど…、\n「テイラーさん、\"レガート\"は小学校じゃ\n習いません」\n「あれ、そうだっけ!?」\n「えっと、\"レガート\"は\"滑らかに\"という\n意味なんだよ」\n「え、鉄琴を\"滑らかに\"演奏…? 」\n　明らかに困ったような表情をしている…。\n「テイラーさん、小学生には難しいのでは? 」\n「うーん、そっかぁ。ごめんね♪」", Cr);
			break;
		case 62:
			DrawString(Cx, Cy, L"　途中でゲーターと合流して、後半は\nゲーターと一緒に見て回ることになった。\n「あれ、キミ、まだそこのフレーズの\n練習してたんだ」\n　ゲーターが声をかけたのは、ティンパニを\n演奏している子だった。\n「うん、なかなかうまくいかなくて」\n　そう言って、またフレーズを叩き出した。\nで、すぐに失敗した。\n「まぁ、何度もやってたらそのうち\nできるようになるさ、がんばれ」\n　ちょっと、雑すぎでしょ!?\n「あー、まずはフレーズを\"ゆっくり\"、\n\"確実\"に叩いてみて。これを何度も\nやってたら、元の速さでも叩けるように\nなるよ」\n「そっか、ありがとう、耳のお姉さん!」\n　この子はそう言って、また練習に戻った。\n…だいぶゆっくりになったね。まあいっか。", Cr);
			break;
		case 70:
			DrawString(Cx, Cy, L"　大体3時間の練習が終わり、ボクたちも\n小学生も解散することになった。\n「いやぁ、流石に長かったですね…」\n「確かに、途中で小学生たち遊んでたからな」\n「眠そうにしてたり、疲れてたりしてる人\nもいましたよね」\n「先生、ずっと帰ってこなかったしな」\n　珍しくゲーターと話が合ってる。\n「ランナーさん、今日はお忙しい中\nありがとうございました」\n　先生が来てお礼を言いに来た。\n「いえいえ、こちらこそ音楽で繋がる機会を\n与えてくださりありがとうございます♪」\n　テイラーが丁寧に返した。多分本音だろう。\n「いやー、ランナーってやっぱり変人の\n集まりなんですね」\n　…やっぱり先生に問題があるね。", Cr);
			break;
		case 71:
			DrawString(Cx, Cy, L"「あの、すみません」\n突然、ひとりの児童がボクたちのところに\nやってきた。手にはサックスを持っている。\n「原因がよく分からないんですけど、\nなんかうまく吹けないんです」\n「原因が分からない?」\nゲーターが怪訝そうに聞いた。\n「はい、なんか違和感というか、…なんだろ」\n「譜面は覚えてるのか?」\n「はい、譜面自体はほぼ完璧に覚えてます」\n「そうだねぇ、一回吹いてみてよ♪」\n　テイラーがそう言うと、その子は\nサックスを吹き始めた。聞いてみた感じだと、\nどこも間違えてないし、リズムにも乗れてる。\nだけど、この子の言う通り、何かが足りない\nような気がする。", Cr);
			break;
		case 72:
			DrawString(Cx, Cy, L"「どうでしょうか」\n　演奏が終わって、児童が聞いてきた。\n「よく吹けてたじゃねーか、違和感も何も\nなかったぞ」\n「はぁ」\n　ゲーターがそう言うと、児童が軽くため息\nをついた。本人が納得してないんだから\n当然の反応だよ。でも、ボクもはっきりと\nした答えが出ていない。\n「うーん…、何か足りないような気はしたよ」\n「………」\n　一応ボクも思ったことは言ってみたけど、\n答えになってない気がする。\nふとテイラーの方を見てみると、何か\n考えている様子だった。\n「うん、心当たりが一つだけあるね」", Cr);
			break;
		case 80:
			DrawString(Cx, Cy, L"「もしかして、うまくやらなきゃとか、\nミスしないようにしなきゃとか考えてない?」\n　テイラーが答えた。そういえば、この子は\n演奏しているときに真剣になりすぎている\nような気がした。\n「はぁ…、考えてますけど。でもそれ普通\nですよね?」\n「うーん、確かにミスしないように演奏する\nのは良いことなんだけど、それを意識する\nこと自体がミスなんだ♪」\n　また難しい話してる。\n「じゃあランナーさんはどうやって演奏\nしてるんですか?」\n「ん? 私の演奏聞きたい?」\n「はい」\n「うん、いいよ。曲はさっきと同じで\nいいよね♪」", Cr);
			break;
		case 81:
			DrawString(Cx, Cy, L"　ということで、テイラーが実演すること\nになった。楽器を持って演奏するのかと\n思ったら、なんと自分の声でソプラノ\nサックスの音を出し始めた。\n「ゲーター、これ、テイラーの声だよね?」\n「まぁ、機械だからな、機械音くらい\n出せるぜ」\n　テイラーの演奏を聞いてると、なんだか\nノリノリになれるというか、楽しい感じが\nする。さっきの児童の演奏とはまた違った\n感じだった。でも、曲はさっきと同じなのに、\nどうしてこんなに違いが出てくるんだろう?\n\n　大体3分の演奏が終わった。\n「さて、どうかな?」\n「…うまいです。どうしたらそんなにうまく\n出来るんですか?」", Cr);
			break;
		case 82:
			DrawString(Cx, Cy, L"「\"楽しく\"演奏するんだ♪」\n「\"楽しく\"…?」\n　テイラーから出た答えは、とてもシンプル\nなものだった。\n「そう、本来演奏は楽しくやるものなんだ。\nそしたら聞き手も楽しくなるんだ。\"音楽\"\nっていう漢字だって、\"楽しい音\"って書く\nでしょ。特にリード系とか、息を使う楽器は\n奏者の心も出てくるからさ♪」\n　へ、へぇ…そうだったんだ。確かに、\nボクがピアノを弾くとき、いつもと同じ\nように弾いてるつもりなのに、何か違う\nような気がするときがあった。もしかして\nこれかな…?\n「確かに技術も必要だけど、無理に力を\n入れると音にも出てくるからさ。力を抜いて\n気楽に演奏しようよ♪」", Cr);
			break;
		case 90:
			DrawString(Cx, Cy, L"　演奏会の当日、ボクたちは観客として\n小学校に来ていた。年に一度のイベント\nというのもあって、たくさんの人が来ていた。\n中には保護者もいるんだろうな。\n　…実を言うと、ボクはこういうざわざわ\nした音が苦手なんだ。音楽とか曲とかは好き\nなんだけど、人混みとかだとうるさく感じる。\n困ったなぁ…。「あ、ランナーさん、いらしてたんですね」\n　練習の時にボクらを置いてどこか行ってた\n先生が話しかけてきた。\n「いやー、ランナーさんも物好きですねー、\nこんなちんけな演奏会に来るなんて」\n　こんな人が先生だなんて信じられない。\nもっとまともな人はいないの?\n「もしかしてヒマなんですかー?」\n　まぁ、そうだけど、暇じゃなくても多分\n来てますよ。", Cr);
			break;
		case 91:
			DrawString(Cx, Cy, L"『みなさん、お待たせしました。これより、\n小学生たちによる演奏会を始めます』\n　時間が来て、演奏会が始まった。\nボクたちは小学生を緊張させないように、\n隅のほうで見守ることにした。\n　最初は学校の校歌を歌う。…内容は\nふざけてるけど、曲としてはまぁ聞ける。\n次は楽器演奏。練習の時に聞きなれている\nのもあって、聞きやすかった。\n　ふと隣を見ると、ゲーターが音楽に\n合わせて体を揺らしていた。よく見たら手も。\n「ゲーター、ノリノリだね」\n「え? あ、気づかなかったぜw」\nゲーターは恥ずかしそうに言った。\n「前のソプラノサックスの子、硬さがとれて\nうまく演奏できてるね。ちょっとミスってる\nけど気にならないくらいだね♪」\n　テイラーさんもたまに辛口なコメント\n言うよね…。", Cr);
			break;
		case 92:
			DrawString(Cx, Cy, L"「いやー、楽しかったな!」\n　演奏会が終わって、会場にいた人たちが\nそれぞれの帰路についていた。\n「ゲーターったら、結局最後まで\nノリノリだったね」\n「まぁな、音楽が俺の生きがいだからな」\n「テイラーさんも結構楽しんでましたよね」\n　そう言って話しかけたけど、さっきまで\nいたはずのテイラーさんがいない…。\n「あれ、テイラーさんは?」\n「あーまたか。たまーにどこか行くんだよ。\nまぁ、ほっときゃ帰ってくるし、俺たちも\nもう帰ろうぜ」\n「えぇ、校門のあたりで待ってましょうよー」\n「わりーけど、俺はこの後知り合いと\n音ゲーの約束してんだ。それに、いつ帰って\nくるかわかんねーぞ」", Cr);
			break;
			//ゲーター
		case 1000:
			DrawString(Cx, Cy, L"　ランナー事務所の中。俺は自然に目を\n覚ました。ゆっくりと体を起こして、伸びを\nした。ベッドから出て、部屋のカーテンを\n開ける。外はまだ暗いが、すぐに明るくなる\nだろう。空にはちらほらと雲が見える。奥の\n方に見える町は、まだぐっすり眠っている\nようだ。\n　にしても、この時間はマジで静かだ。\nSilentroomのGAMEBOYの前奏くらいに静かだ。\n昼と夕方あたりで盛り上がって、夜に\nなったらまた静かになっていくんだろうな。\nこうやってのんびり考え事ができるのは、\nここが平和だからだろうな。のんきなもん\nだぜ、ここの人たちは。\n\n　日が昇ってきた。良い朝だ。地下で寝てる\nあいつもそろそろ起きるだろう。今日も\n元気に走るぜ。", Cr);
			break;
		case 1001:
			DrawString(Cx, Cy, L"「おはよーさん、ゲーター♪。もしかして\nもう起きてた？」\n１階でテイラーと話している。こいつは半分\n機械だから、時間にほとんど正確だ。正直\n羨ましい。ランナーはタイミングも大事な\n仕事だからな。演奏にブレがないのはかなり\n有利だ。でもまぁ、俺はそこまでして機械に\nなりたくはないけどな。\n「おう、今日は早起きしたぜ。で、今日の\n仕事は何だ？」\n「今日もポイ捨て回収だよ♪。今回は海\nだよ♪海好きでしょ？」\n「まぁ、好きだけど、またゴミ回収かよ…」\n「ゴミって言わないの♪」\nこの町は音楽犯罪がない代わりに、ポイ\n捨てが多い。楽でいいが、せっかくシルバー\nランナーになったんだから、もっと派手な\nことがしたいな。", Cr);
			break;
		case 1002:
			DrawString(Cx, Cy, L"「そうだ、明日ここに新人くんが来るよ♪\nゲーター後輩が欲しいって言ってたよね♪」\n「お、まじか！嬉しいぜ、どんなやつ？」\n新人はマジでありがてぇ、ここは俺と\nテイラー二匹しかいなかったから、派遣の\n仕事ができなかったんだ。事務所に最低\n二匹のランナーがいなければいけないって\nいう決まりがあるからな。\n「とっても耳がいいウサギさんだよ♪」\n「おお、演奏得意そうなやつだな」\n「すぐそこにある音楽学校出身だってさ♪」\n「あそこからかぁ、結構優等生？」\n「そうらしいね♪」\nあそこを卒業したやつは、だいたい都会の\nランナーとか、バンドとか作曲家とかに行く\nもんだ。新人は地元で働きたいっていうこと\nかな？楽しみだ。", Cr);
			break;
		case 1010:
			DrawString(Cx, Cy, L"　次の日、今日は新人が来るらしい。\nめっちゃ楽しみ。でも、今日の仕事もゴミ\nひr…ポイ捨て回収だ。今回は森だ。新人に\n仕事を見せるために、俺は新人が来るまで\n待機だ。早く来ないかな…。新しく話し\n相手ができるのはマジでうれしい。あ、\nいや、テイラーと話すのが飽きたってわけ\nじゃなくて、人は多いほうがいいだろ？\n俺ってば人に教えるのが好きだから。\nテイラーからは教えてもらってばかり\nだから。\n俺はさっきから部屋の窓から事務所の\n入り口を見つめている。多分テイラーも\n部屋のモニターから見つめているんだろう\nな。とか思っていたら、来たようだ。", Cr);
			break;
		case 1011:
			DrawString(Cx, Cy, L"　よし、会いに行こう！早速部屋から出て\n迎え入れようとしたら、\n　ピッコン、ピッコン。\n　電話が鳴った。タイミング悪すぎだろ…。\nしゃーねー、電話優先だ。俺は受話器を\n取った。\n「やっほ♪。起きてる？」\nテイラーだった…。\n「起きてるよ、なんだ？」\n「新人さん来たよ♪」\n…………………………。\n「うん、知ってるって」\n「さっすがダイナー♪。じゃ、３分後に\n１階に来て♪」\nなぜ３分？\n「じゃ、あとはよろしくねー♪」\n意味が分からないまま電話が切れた。\nまぁいいや、３分後に行こう。", Cr);
			break;
		case 1012:
			DrawString(Cx, Cy, L"　３分後。俺は１階に降りた。そこには\nさっき見た新人がいた。改めて見ると、長い\n耳を持った、緑色のウサギだった。思った\nより体が細くて、\n「おー！お前が新人かー！思ったより細い体\nしてんなー」\nついうっかりこんなことを言ってた。\n「ほっといてください…」\nおっと、まずは自己紹介だっけ？\n「俺はマップゲーター。長いからゲーターで\nいいぞ。お前は？」\n「ピッカーです」\n「ピッカーな、よし覚えた。そうだ、\nLINEとかやってる？」\n一応連絡先も聞いておこう。あとでいると\n思うから。\n「…ないです」\nないんかい。じゃー…、次は案内だな。\n「それじゃピッチャー、早速この事務所を\n案内するぜ」", Cr);
			break;
		case 1020:
			DrawString(Cx, Cy, L"　俺はこいつを連れて、２階にやってきた。\n一応、端の部屋から１個ずつ紹介して\nいった。とはいっても、そんなに部屋の数は\n多くないから、すぐに終わった。もともと\n人数が少ないからなぁ、この事務所。今、\n俺らがいるトレーニングルームも、俺が来る\nまではまったく使われてなかったらしい。\n今は俺がバリバリ使ってて、嫌がる\nテイラーを連れてたまに一緒にやってる。\n「ランナーは音楽専門ってよく言うけど、\n体力もいるんだぜ。ここでちったぁ体鍛え\nとけよ、ピッチャー」\n一応ピッチャーにも勧めておく。\n「ピッカーです」\nあ、名前間違えてた。\n「じゃ、次は地下だ。ついて来い」\nそういや、テイラーは今何やってんだ？", Cr);
			break;
		case 1021:
			DrawString(Cx, Cy, L"　とりあえず地下に連れてきた。俺もあまり\n入ったことはないが、相変わらずテイラーの\nコレクションが部屋に並んでる。テイラーは\nいつもモニターの前にいるんだが、今は\nいない。ああ、あいつやる気だな。俺は\nあたりを探す。\n「ピーター、あれを見てみろ」\n俺は、赤いドラゴンのロボットを指さして\n言った。\n「ピッカーです。なんですか？」\n「近づいてよーく見てみろ。いいから」\n俺も前にやられたやつだ。あいつはこれの\n準備をしてたんだな。相変わらず性格\n悪いな。ピッカーがロボットに近づくと、\n「うわっ！？」\n突然そのロボットが動き出した。羽を大きく\n伸ばして、大げさな起動の仕方をして、\n「やぁ、びっくりしたかい？」\nそのロボット(テイラー)こう言った。初見の\n俺はすげーびっくりした。", Cr);
			break;
		case 1022:
			DrawString(Cx, Cy, L"「まぁ、これでここの紹介は終わりだな」\n俺はこう言った。\n「ありがとうございます。ところで、ここ\nって他に人はいないんですか？」\nまぁ、そりゃ気づくよな。\n「いないぜ。俺ら二匹だけだ。もともとこの\n近くは治安がいいからな。俺たちの出番は\nほとんどないのさ」\n「それじゃ、仕事もあまりないの？」\n「そうでもないぜ。最近は音が溢れすぎ\nてて、それを集めてまわるのが俺たちの\n仕事だ」\nまぁ、これが今日の仕事なんだがな。\n「それじゃ、早速やってみるか？テイラー、\nサポートできるよな？」\nテイラーは教えるのがうまいし、手本に\nなるだろうと思ったが…、\n「できるけど、めんどくさい♪」\nやっぱダメか、このめんどくさがりめ。\n「じゃ、俺がサポートしてやるか」", Cr);
			break;
		case 1030:
			DrawString(Cx, Cy, L"「サッカー、車の運転免許は？」\n「ピッカーです。持ってません」\nということで、俺の運転で今日の仕事場に\n向かう。とりあえず、最近のポイ捨ての\n現状について話しといた。\n最近は、音楽犯罪が全く起こらない代わり\nに、ポイ捨てとかその他もろもろの軽犯罪が\n起こってる。あ、一応犯罪なのか。\n「あんなところでこそこそ捨てるなんて\nよぉ、どうせ捨てるなら街中で捨てろよ」\nついつい愚痴を言っちゃった。\n「ポイ捨てを見つけたら捕まえるの？」\n「当たり前だろぉ？でもポイ捨てって、\n高くても400pop(約3300円)の罰金で済んじ\nまうんだ。無くならねぇんだよなぁ、ポイ\n捨て。まぁ、ボーナス出るからいいけど」\nボーナス出るのはまぁ嬉しい。本音だ。\n\nまぁ、目的地には30分くらいで着いた。\nさて、仕事だ。", Cr);
			break;
		case 1031:
			DrawString(Cx, Cy, L"　さて、見た感じはきれいだが、前に来た時\nから２週間は経ってる。そろそろ溜まって\nきてるだろ。\n「ねぇ、本当にポイ捨てされてるの？」\n「きれいに見えるだろ？でもされてるんだ\nなぁ、これが。ポイ捨てっつっても見えない\n場所に捨てるからな。まぁ、ランナーには\nバレバレだけどな」\n「…もう見つけたの？」\n「今から見つけるんだよ」\n俺はそういって、車に積んでた愛用のヘッド\nホンとDJキットを取り出した。俺用に改造\nした特性のキットだ。聞こえはいいし、\n音色もばっちりだ。早速電源を付けた。\n聞きなれた起動音が鳴る。今日も好調のよう\nだな。\n「さて、今から走るぞ。チーター、ついて\n来いよ！」\n「ピッカーです」", Cr);
			break;
		case 1032:
			DrawString(Cx, Cy, L"　曲を流す準備をして、俺は走った。走ると\n同時に音楽がかかる。ここは森だ。フルート\nとサックスが中心の曲を選んだ。猫叉master\nのcaring danceでもよかったが、著作権的に\n危なそうだから別の曲にした。俺が流してる\n曲に合わせて鳥たちが鳴いてくれるのは\nいいが、俺の目的はゴミあつm…じゃない、\nかけら集めだ。さーて、どこだ？\n　っと、早速聞こえたぞ。雑音が。自分も\n曲に合わせて音を出そうとしてるが、うまく\n出せなくて雑音になってるぞ、かけらくん。\n俺は聞こえる場所に走った。そこには思った\nより多いかけらが捨てられてた。誰がか\nめっちゃ捨ててったな。こんにゃろー。俺は\n走りながらかけらを回収していった。さて、\n早く次に行くぞ。この曲は２分半しかない\nんだ。", Cr);
			break;
		case 1040:
			DrawString(Cx, Cy, L"　正直に言うと、後ろからついてくる\nピーターのことをすっかり忘れてた。でも\nちゃんとついてきてたようだ。\n「終わったぜ、変わりないか？」\n「お疲れさん。こっちは問題ないよ♪」\n俺は一通り仕事を終わらして、いつもより\n多いかけらを集めて帰った。\n「これだけ集めれたら、ざっと150pop\n(約1250円)くらいになるかな。大漁だね♪」\n「レートは？」\n「上がんないよ♪」\nあれでもダメかよ。もっとむずい曲にすれば\nよかったか？\n「いつになったら私を抜けるかなー♪」\n「見てろよ、すぐに抜いてやるからな」\nそもそも、テイラーと同じゴールドランナー\nになるの自体むずんだなぁ。はぁ、きつい。", Cr);
			break;
		case 1041:
			DrawString(Cx, Cy, L"「そういえばキッカー、お前目標とかある\nのか？」\nわざわざこういう特殊な仕事に来るもん\nだから、何かあるんだろうなって思った。\n「ピッカーです。目標かぁ、とにかく、\nお父さん探しをしたいんだけど、まずどう\nすればいいんだろう？」\n「お父さんもランナーなのか？」\n「それなら、名前とかがあれば探せるよ♪」\nテイラーが会話に入ってきた。\n「それじゃ、私の部屋に来て。一緒に\n探してあげる♪」\n「え、でも、売りに行くんじゃないの？」\n「そんなのゲーターに任せたらいい\nでしょ♪早速行こうか♪」\nテイラーはそう言って、ピッカーを地下に\n連れ去って行った。\n\n「テイラー、作業したくないだけだろ…」\n俺は小さくつぶやいた。", Cr);
			break;
		case 1042:
			DrawString(Cx, Cy, L"「親父かぁ…」\n俺は、生まれてすぐに親父に捨てられた。\nおかんと一緒に捨てられた。むりやり交尾\nさせて、妊娠したと分かればすぐに捨て\nやがった。旅行先で二人分の荷物を盗んで\n飛行機で帰ったらしい。金もチケットも\nないおかんは困って、警察とかに相談して、\nなんとか帰れたらしいが、家のものも持ち\n去られてたらしい。それからはもう本当に\n大変で、本当に苦労した。\n　俺は、親父がでっかい音楽犯罪をしている\nって聞いて、ランナーになりたいって思った。\n世界中で起こってる犯罪を無くしたいってな。\nでも、俺は音楽が好きだが、演奏の才能が\nないから、７年たった今でもシルバー\nランナー止まりだ。でっかい派遣の仕事が\n来るのはゴールドランナーからだ。\n　…今ここにあるかけらも、捨てられたくて\n作られたわけじゃないんだよな。あぁ、ゴミ\nって言っちゃだめだな。", Cr);
			break;
		//EP2
		case 1050:
			DrawString(Cx, Cy, L"　さーてと、今日の仕事終わりっ!\n今日も誰かが海に捨てやがったゴミ…\nじゃない、かけらを回収していた。\nやめろよなー、せっかくのきれいな海が\n汚されるんだぜ。音の不協も他よりはっきり\n出てくるんだから、バレバレだっつの。\n　もっとポイ捨ての監視強くならねーかな。\nここまで多いと環境に影響が出始めるのも\n時間の問題だぜ。それかゴミが出ないような\n音楽製品を作るとか、捨てられても自然に\n安全に還る製品を作るとか。再利用が\nできる…いや、再利用はいろんなスーパーで\n音楽製品の回収作業やってるな。\n　ていうか、ポイ捨てがなくなったら俺たち\nの仕事の大半がなくなるな。ポイ捨てする\nなら程よくやってくれ。\n「ゲーター、そろそろ夕食だよ♪」\n「あ、おう! 今行く」", Cr);
			break;
		case 1051:
			DrawString(Cx, Cy, L"「小学校の演奏会の依頼が来たんだ♪」\n「おお!?本当かっ!」\n　夕飯の時にテイラーが新しい仕事の話を\n持ち出してきた。最近はかけら集めと\n窓口相談の仕事しかなかったからマジで\n退屈だったんだ。\n「よっしゃ!久々に大暴れできるんだな!」\n「あ、私たちは小学生に教えるだけだよ♪」\n「はぁ!?なんでだよ?」\n　バァン!!\n　あ、やべっ、台パンしちまった。\n「主役は小学生だよ。ランナーが出たら\nまずいでしょ♪」\n「はぁ～、つまんねーの」\n　うーん、まぁ今までの仕事に比べたら\nいくらかはやりがいありそうだな。", Cr);
			break;
		case 1052:
			DrawString(Cx, Cy, L"「さて、俺は部屋に戻って機材の調整して\nくるぜ、ごちそうさま」\n　それから俺は自分の部屋に戻り、頭の中で\nシミュレーションをする。楽譜の読み方と、\n楽器の扱い方を教えておけば後はうまく\nやってくれるだろう。\n　テイラーはこういうのめんどくさがる\nだろうし、ピッカーは新人だ。こういうのは\n一人でやることになる。頼れる人がいれば\nいいんだが、なかなかそう甘くない。自分で\nできることは自分でやる。こうするしかない。\n　…そういえば、演奏会じゃ何の楽器\n使うんだ? 流石にギターはやらんか。\nそうだなぁ、俺の学校じゃピアノと\nリコーダーと木琴鉄琴をやったっけ。\nここら辺の楽器かな?", Cr);
			break;
		case 1060:
			DrawString(Cx, Cy, L"　金曜の昼、俺たちは小学校の体育館にいる。\n小学生が祭壇に並んで、それぞれ楽器を\n手に持っていた。水曜にやった打ち合わせ\nでは、まずは一回通しで演奏をして、\nテイラーが全体としてのアドバイスをする。\nそのあとは自主練をしながら一人ずつ\n見ていく予定だ。\n「今日はランナーの人たちと一緒に練習\nしますよ。それじゃランナーさん、あとは\nよろしくお願いします」\n　…え、は? 演奏は!?\nおいおい、先生どっか行くなよ。俺たちも\n小学生もこれからどうすりゃいいんだ。\n「そ、それじゃ、まずはそれぞれ自主練習ね。\n私たちは順番に見ていくからね♪」\n　テイラー、ナイスアドリブ。", Cr);
			break;
		case 1061:
			DrawString(Cx, Cy, L"　とりあえず、演奏してる小学生のそば\nまで行って様子を見ることにした。すると、\n明らかに変な音を出してるやつがいた。\n複数いる鍵盤ハーモニカ奏者の一人だった。\n注意深く聞いてると…、\n「あ、ねぇ、そこは黒鍵盤じゃねぇか? 」\n　白鍵盤だけで弾いてた。\n「ほらぁー、やっぱそうじゃねーかー」\n「じゃあお前弾けるんかよー」\n　周りにいた小学生たちが騒ぎ出した。\n「こういうのは暗記だよバーカ」\n「できるわけねーじゃん12個もあるのにー」\n　あ、もしかしてこいつら…、\n「使ってる鍵盤、5個だけだぞ」\n　一応教えてやった。\n「……まじで!?」\n　やっぱわかってなかったか。", Cr);
			break;
		case 1062:
			DrawString(Cx, Cy, L"　途中からは、ピッカーもつれて見て周る\nことになった。\n「あれ、キミ、まだそこのフレーズの\n練習してたんだ」\n　さっきからずっと同じところの練習してる\nティンパニ奏者に声をかけてみた。\n「うん、なかなかうまくいかなくて」\n「まぁ、何度もやってたらそのうち\nできるようになるさ、がんばれ」\n　こればっかりは練習あるのみだからな。\n「あー、まずはフレーズを\"ゆっくり\"、\n\"確実\"に叩いてみて。これを何度も\nやってたら、元の速さでも叩けるように\nなるよ」\n　突然ピッカーが口をはさんできた。いや、\nそれは今言ったから大丈夫だって。\n「そっか、ありがとう、耳のお姉さん!」\n　……俺も同じこと言ったのにこの反応の\n違いは何なんだ。", Cr);
			break;
		case 1070:
			DrawString(Cx, Cy, L"　小学生との練習は3時間も続いた、\nっていうか、先生がずっと帰ってこなくて\n終われなかった。\n「打ち合わせの時もこんな感じだったか?」\n「先生も一緒に見て周る予定だったよ、\n打ち合わせではね♪」\n「途中で総演奏も入れたほうが良かったか?」\n「そうかもねぇ…」\n　早速テイラーと反省会をしてた。\n「ランナーさん、今日はお忙しい中\nありがとうございました」\n　先生が来てお礼を言いに来た。いろいろと\n文句を言いたかったが。辞めておいた。\n「いえいえ、こちらこそ音楽で繋がる機会を\n与えてくださりありがとうございます♪」\n　テイラーが丁寧に返した。まぁ仕事だしな。\n「いやー、ランナーってやっぱり変人の\n集まりなんですね」\n　練習見に来てないくせによく言えたな。", Cr);
			break;
		case 1071:
			DrawString(Cx, Cy, L"「あの、すみません」\n　帰ろうとしたとき、ひとりの小学生に声を\nかけられた。ソプラノサックスを持ってる\nから、この子も奏者だな。\n「原因がよく分からないんですけど、\nなんかうまく吹けないんです」\n「原因が分からない?」\n「はい、なんか違和感というか、…なんだろ」\n「譜面は覚えてるのか?」\n「はい、譜面自体はほぼ完璧に覚えてます」\n　えー、なんだろなぁ。あとに考えられる\nのは、サックスの調子が悪いとか?\n「そうだねぇ、一回吹いてみてよ♪」\n　テイラーがそう言って、その子は演奏し\n始めた。うーん? 特に間違ってないし、\n変な感じもしないけどなぁ…。", Cr);
			break;
		case 1072:
			DrawString(Cx, Cy, L"「どうでしょうか」\n　一通り吹き終わったその子が聞いてきた。\n「よく吹けてたじゃねーか、違和感も何も\nなかったぞ」\n　安心させるために褒めてやったが、\n「はぁ」\n　その子はため息をついた。褒めたのに\nなんでそんな反応するんだよ…?\n「うーん…、何か足りないような気はしたよ」\n　ピッカーが横から言ってきた。\n「何かって何だよ?」\n「さぁ、詳しくは分からないけど、\nそんな感じがした」\n　へぇ…? そうなのか…。\n「うん、心当たりが一つだけあるね」\n　今度はテイラーが言ってきた。", Cr);
			break;
		case 1080:
			DrawString(Cx, Cy, L"「もしかして、うまくやらなきゃとか、\nミスしないようにしなきゃとか考えてない?」\n　んん? それって普通だろ? 演奏するん\nだから失敗しちゃダメだろ。\n「はぁ…、考えてますけど。でもそれ普通\nですよね?」\n「うーん、確かにミスしないように演奏する\nのは良いことなんだけど、それを意識する\nこと自体がミスなんだ♪」\n　どういうことだよそれ…。普段から訳\n分らんこと言ってるけど、今のは特に\nひどいぞ。\n「じゃあランナーさんはどうやって演奏\nしてるんですか?」\n「ん? 私の演奏聞きたい?」\n「はい」\n「うん、いいよ。曲はさっきと同じで\nいいよね♪」\n　ん? テイラー珍しく乗り気だな。\nまぁいいけど。", Cr);
			break;
		case 1081:
			DrawString(Cx, Cy, L"　さて、テイラーの演奏だが、いつもの\nように自分の口から音を出して演奏している。\n…本当に口かどうかは怪しいが。\n「ゲーター、これ、テイラーの声だよね?」\n「まぁ、機械だからな、機械音くらい\n出せるぜ」\n　しっかし、テイラーの演奏はうまいよな。\n流石ゴールドランナーって言うか、普段から\n怠けてるとは思えないほどの美声(?)だな。\nまったく、このくらいできるんだったら俺に\n仕事を押し付けるなよなー。ピッカーも\n被害者だし。\n「さて、どうかな?」\n　テイラーの演奏が終わった。\n「…うまいです。どうしたらそんなにうまく\n出来るんですか?」", Cr);
			break;
		case 1082:
			DrawString(Cx, Cy, L"「\"楽しく\"演奏するんだ♪」\n「\"楽しく\"…?」\n　ああ、それか。テイラー\"楽しく\"ってよく\n言ってるな。\n「そう、本来演奏は楽しくやるものなんだ。\nそしたら聞き手も楽しくなるんだ。\"音楽\"\nっていう漢字だって、\"楽しい音\"って書く\nでしょ。特にリード系とか、息を使う楽器は\n奏者の心も出てくるからさ♪」\n　理屈は分かるが、そこまで変わるもの\nなのか?\n「そうそう、ゲーターにも言ってるからね♪」\n「は、俺!?」\n「ゲーターも上手くやろうとして焦ってる\nときがあるからね」\n「俺はミスらないように\n楽しくやってるっつーの!」", Cr);
			break;
		case 1090:
			DrawString(Cx, Cy, L"　土曜日、俺たちは演奏会の様子を見に\n来ていた。結構の人が来ていて賑わっていた。\nまぁ、ほとんどが小学生の保護者なんだろう\nけど。\n「あ、ランナーさん、いらしてたんですね」\n　あ、丸投げ先生、久しぶり!\n…とは言わずに心の中で思った。\n「いやー、ランナーさんも物好きですねー、\nこんなちんけな演奏会に来るなんて」\n　ちょっと待てや、この先生、言動からして\nヤバいにおいがプンプンするぞ。いや、\n練習の時もヤバかったけど、今日のこの人は\n特にヤバい。\n「もしかしてヒマなんですかー?」\n　…一発ぶんなぐってやりたくなったが、\nいろいろ考えてやめた。あとで事務所の\nサンドバッグをこの人だと思って殴ろ。", Cr);
			break;
		case 1091:
			DrawString(Cx, Cy, L"『みなさん、お待たせしました。これより、\n小学生たちによる演奏会を始めます』\n　アナウンスが流れて、観客達が静かになる。\n小学生たちの緊張も伝わってくる。\n　まずは学校の校歌から始まった。この\n学校の校歌は初めて聞くが、…なんだこれ。\n音楽覚えたての小学生が作ったみたいな\n歌詞だな。メロディーはなかなか良いのに。\n　さて、次が本題の演奏だ。練習の時に\n各パートのフレーズを聞いたおかげで、\n音楽がクリアに聞こえてくる。\n「ゲーター、ノリノリだね」\n　ピッカーに言われて気づいたが、演奏に\nつられて体が動いてた。\n「前のソプラノサックスの子、硬さがとれて\nうまく演奏できてるね。ちょっとミスってる\nけど気にならないくらいだね♪」\n　一言余計だっての。ってかミスってるか ? ", Cr);
			break;
		case 1092:
			DrawString(Cx, Cy, L"　演奏会は大成功だった。特にトラブルも\nなく、小学生も最後まで演奏しきった。\n「ゲーターったら、結局最後まで\nノリノリだったね」\n「まぁな、音楽が俺の生きがいだからな」\n　…だが、一つだけ気がかりなことがあった。\n演奏会の時、ずっと誰かの視線を感じてた。\n気づかれないように辺りを見渡しても、\n人混みのせいでよく分からなかった。\n演奏会が終わった後も少し探してみたが、\n結局わからなかった。\n　まぁ、俺たちはランナーだし、有名には\nなってる。俺たちを見て話しかけてくる\n人もいた。特に俺はガタイが良いし、俺の\nことを見て目が離せなかったやつがいたん\nだろう。\n\n　…多分なw", Cr);
			break;
			//テイラー
		case 2000:
			DrawString(Cx, Cy, L"　朝だ。地下のこの部屋からじゃ分からない\nけど、時間的には日の出の時間だ。まずは、\nパソコンの電源を入れよう。起動するまでの\n間に、部屋の電気を付けて、私の体の点検を\nする。特に大きい仕事はないけど、点検は\nしないとガタがきちゃうから、これだけは\nさぼれない。\n　関節、メモリ、バッテリー。異常なし。\n　メインと予備のバッテリー、充電済み。\n　羽の動作確認。問題なく飛べます。\n　軽い運動。いざというときに動くため。\n　一通り終わって、やっとパソコンが\n起動した。といっても、５個あるモニターの\nうち３個はカメラだけど。メインで使ってる\nモニターを見ると、いつも通りメールが\n届いていた、ほとんどは仕事のメール\nだから、軽く読み飛ばす。\nおっと、一つだけ違うメールが来てた。\n\n『明日、そこの事務所に新人を入れる』", Cr);
			break;
		case 2001:
			DrawString(Cx, Cy, L"「お、まじか！嬉しいぜ、どんなやつ？」\n私は１階にあがって、もう一人のメンバー、\nゲーターに新人がくるってことを教えた。\n新人はイングリッシュロップ(ウサギ)。\nすぐそこの松坂音楽学校を卒業したばかり。\n成績は、音楽と工作が良くて、数学と英語が\nちょっとダメ。在学中にランナー試験に合格\nしてる。あと、ピアノの小さい演奏会に\n何回か出てる。\n「新人かぁ、楽しみだな」\n「ここって私とゲーターの二人だけだった\nからね♪」\n「なんで今まで来なかったんだろうな？」\n「求人は出してたけど、志願者がいなかった\nんだよね♪」\nランナーは、基礎知識を見る試験に合格\nしないとなれないって言うのもあるけど、\nそもそも敷居が高いっていうのもあるのよ\nね。そんなに難しく考えなくていいのに。", Cr);
			break;
		case 2002:
			DrawString(Cx, Cy, L"「そうだテイラー、たまにはランナーとして\n走ってみないか？」\n「事務所の管理があるからヤダ♪」\n「そんなの俺に任せろって」\n「まだ最後に走ってからたったの３日しか\n経ってないよ♪」\n正直に言うと、走るのがめんどくさい。\n走った後はメンテが大変だし、簡単な仕事の\nためにわざわざ体を動かしたくない。\n「いざっていうときに動けないぞ」\n「私はサイボーグだもん♪だいじょーぶ♪」\nサイボーグっていうのは、体の一部を機械に\n変える技術のこと。私の場合は半分以上が\n機械だから、もうロボットって言われても\nおかしくないし、私も特に気にしてない。\nなんでロボットになったかっていうと、\n音楽事故で瀕死だったところを博士に助けて\nもらったからなんだ。でも、みんなには\n自分が好きでこうしてもらったって言って\nる。あまり知られたくないからね。", Cr);
			break;
		case 2010:
			DrawString(Cx, Cy, L"　次の日、新人くんが来る日だ。ここに\n新人を迎えるのは久しぶりだな。４年前に\nゲーターがここに異動して以来だ。新しい\nランナー仲間が増えるのはうれしいし\n助かる。私の代わりに働いてくれる人が\n増えるからね♪。\nそれに、派遣の仕事ができるようになる\nのもうれしいな。事務所には最低二人は\nいないとだめで、元々二人しかいなかった\nここは派遣の仕事が絶対に来なかったから、\nゴールドランナーの私は退屈で仕方なかった\nんだ。\n\n　とか考えてたら、見えてきた。モニター\n越しだけど、はっきりと見える。思った\nよりは背は低くて、かわいらしいイング\nリッシュロップ(ウサギ)だ。", Cr);
			break;
		case 2011:
			DrawString(Cx, Cy, L"　新人くんが事務所の扉の前でおどおど\nしてるのがモニター越しに見えた。\nかわいい。私はマイクを手に取って、\n「あっ、どうぞお入りくださいー♪」\n新人くんにこう言った。そのあとは、\nカメラに気づいたみたいで、恥ずかしそうな\n顔をしてた。新人くんが入った後は、\n「えっと、新人さんですよね？ちょっと\nそのままでお待ちくださいー♪」\nそう言って、新人くんを待たせた。あとは、\n今頃２階でグースカ寝てるだろうダイナーを\n起こして、あとはダイナーに任せよう。\n　ダイナーの希望で、ダイナーの部屋に\nカメラは置いてない。めんどくさいときは\n電話でやり取りするのが習慣だ。私は\n受話器を取って、ダイナーに電話をかける。", Cr);
			break;
		case 2012:
			DrawString(Cx, Cy, L"　思ったよりすぐに電話がつながった。\n「やっほ♪。起きてる？」\n「起きてるよ、なんだ？」\nあら、ダイナーが私より早起きするなんて\n珍しいなぁ。\n「新人さん来たよ♪」\n「うん、知ってるって」\nそっか、ダイナーも新人くんが来るのを\n楽しみにしてたんだね。\n「さっすがダイナー♪。じゃ、３分後に\n１階に来て♪。あとはよろしくねー♪」\n３分の間を開けるのは、新人くんを落ち\n着かせるため。ちょっと緊張してるみたい\nだったからね。あとはダイナーがやって\nくれるでしょ。\n　あ、そうだ。せっかくだし、私が\nダイナーにやったサプライズを新人くんにも\nやってあげよう。えっと、名前は確か\nピッカーだったよね。よし、大丈夫だ。", Cr);
			break;
		case 2020:
			DrawString(Cx, Cy, L"　まぁ、これはある種の初見殺しだよね♪\n私は今、部屋のコレクションの中に隠れて\nいる。やっぱり、ロボットが隠れるには\nロボットの中だよね。近くを通ったところで\n動き出して、ビックリさせる作戦だ。多分、\n今頃ピッカーはゲーターと一緒に事務所を\n見て回ってるんだろうね。…ゲーター、\nピッカーに失礼なこと言ってないかな？\nいきなり連絡先とか聞いてるかもなー。\nあとで私が代わりに謝っとこ。\n\n　しばらくして、階段から二匹分の足音が\n聞こえてきた。来たね。まずはゲーター、\n次にピッカーが出てきた。へぇ、ピッカーて\n思ったより小さいんだね。", Cr);
			break;
		case 2021:
			DrawString(Cx, Cy, L"　ピッカーがきょろきょろとあたりを\n見渡している。まぁ、珍しいものばかり\nだし、当然かな。あ、モニターつけっぱなし\nだった。別にいいや。デスクトップとカメラ\nしか映ってないし、勝手にいじることも\nしないでしょ。ゲーターじゃあるまいし。\n「ピーター、あれを見てみろ」\n「ピッカーです。なんですか？」\n名前間違ってるよ、ゲーター。ゲーターは\n私を指さして言った。\n「近づいてよーく見てみろ。いいから」\nフォローしてくれるのはありがたいん\nだけど、イタズラ心が顔に出てる。\nピッカーが私にある程度近づいたところで、\n「うわっ！？」\n私は動き出した。止まってたものが突然動き\nだすんだから、そりゃビックリするよね。\nでも私はこれだけじゃ物足りなくて、羽を\n大きく広げてみたり、大げさな登場をして\nみた。", Cr);
			break;
		case 2022:
			DrawString(Cx, Cy, L"「さてと、改めまして、ランナー事務所に\nようこそ！ボクがここの管理人のテイラー\nだよ。よろしくね♪」\n「よ、よろしくお願いします」\n「ゲーターに会って早々連絡先とか聞かれ\nたりしたんじゃない？ごめんね、彼は悪気は\nないのよ♪」\n「あ、まぁ、はい」\n私はこの後も、おしゃべりがしたくて話し\n続けてた。そもそもこの事務所から出てない\nから、ゲーター以外の人と話すのって久し\nぶりで、嬉しかったんだ。ついでに、私が\nサイボーグだってこともピッカーに話して\nおいた。\n「テイラー、もういいんじゃないか？」\n私のほぼ一方的な会話は、ゲーターのこの\n言葉で強制終了となった。私の体内時計\nだと、約３分くらいだった。", Cr);
			break;
		case 2030:
			DrawString(Cx, Cy, L"　ピッカーの事務所案内もこれで終わった\nみたいで、次はさっそく仕事の話になって\nいた。\n「テイラー、サポートできるよな？」\nゲーターにこう聞かれたけど、\n「できるけど、めんどくさい♪」\n断った♪\n「じゃ、俺がサポートしてやるか。\nピッカー、ついて来い」\nそういって、ゲーターとピッカーは仕事に\n向かっていった。\nさて、部屋には私しかいない。部屋に\nカギもかけた。カギは念のため、３時間後に\n自動で外されるけど、それでも十分だ。私は\n私だけの秘密の作業に入る。", Cr);
			break;
		case 2031:
			DrawString(Cx, Cy, L"　私はゴールドランナー。ランナーの中でも\nトップクラスの実力を持つランナーだ。\nランナーには４種類いて、ランナーになった\nばかりのビギナーランナー。仕事ができる\nようになるブロンズランナーが約95％。\n難しい仕事も来るようになるシルバー\nランナーが約25％。世界中の大きな仕事を\n任せられるゴールドランナーが約１％だ。\n他にもあるらしいけど、私が知っているのは\nこれだけ。\n私が見ている画面は、ゴールドランナーの\n中の情報サイト。ほかの人に見られては\nいけない情報まである。カギをかけている\nのはこれのため。ゲーターには、部屋を\n閉めているときは寝ているって言っている。\n起こしたらハグルマミキサーの刑って\n言ってる(冗談)♪。", Cr);
			break;
		case 2032:
			DrawString(Cx, Cy, L"　普段から情報だけは集めてた私。ピッカー\nが入ったから、私にも派遣の仕事が来るん\nだろうな。簡単な仕事は全くやる気がないん\nだけど、難しい仕事となると、なんだか\nワクワクする。これがドラゴンの本能なの\nかな？いや、よくわかんないけど。\n　私は特にランナーになりたかったわけじゃ\nなくて、街で音ゲーやってたらスカウト\nされたから入っただけ。普段家から出ずに、\nおうち音ゲーばっかりやってた私をどう\nやって見つけたのかは不明。ランナー試験は\nスカウトされてから受けた。それから12年が\n経って、今まではゴールドランナーとして\nというより、ネットで情報提供やアドバイス\nをしてきた。派遣の仕事がないから、世界\n中で何が起こっているのかは、写真と文字で\nしか知ることができなかった。\n　…やっぱり、嬉しいんだろうな、私。まだ\n仕事がくるって決まったわけじゃないけど、\n来たら頑張ろ♪", Cr);
			break;
		case 2040:
			DrawString(Cx, Cy, L"「終わったぜ、変わりないか？」\n「お疲れさん。こっちは問題ないよ♪」\nゲーターとピッカーが仕事を終えて帰って\nきた。今回の仕事は森に捨てられたかけら\n集め、別名、ポイ捨て回収。平和なこの地域\nじゃ、ランナーの仕事はほとんどこれ。\n「これだけ集めれたら、ざっと150pop\n(約1250円)くらいになるかな。大漁だね♪」\n「レートは？」\n「上がんないよ♪」\nレートっていうのは、ランナーの腕前を示す\n数値。ランナーの基準は全部これで決まって\nいる。私はゴールドの中でも上のほう。\n「いつになったら私を抜けるかなー♪」\n「見てろよ、すぐに抜いてやるからな」\nゲーターは、シルバーランナーになるのに\n５年もかかってる。成長が遅いけど、音楽\n好きだし努力家だから、あと８年したら\nゴールドになれるんじゃないかな。頑張れ♪", Cr);
			break;
		case 2041:
			DrawString(Cx, Cy, L"「そういえばキッカー、お前目標とかある\nのか？」\n名前間違えてるよ。\n「ピッカーです。目標かぁ、とにかく、\nお父さん探しをしたいんだけど、まずどう\nすればいいんだろう？」\n「お父さんもランナーなのか？」\nお、これはチャンスだ♪\n「それなら、名前とかがあれば探せるよ♪。\n私の部屋に来て。一緒に探してあげる♪」\n「え、でも、売りに行くんじゃないの？」\n「そんなのゲーターに任せたらいい\nでしょ♪早速行こうか♪」\n情報探しを口実に、めんどくさいかけらの\n集計をゲーターに投げつける。というか、\nピッカーとお話がしたい。ゲーターが何が\n愚痴った気がするけど、私には聞こえ\nなーい♪", Cr);
			break;
		case 2042:
			DrawString(Cx, Cy, L"「えーと、本当にこの人なの？」\nピッカーのお父さんの名前と、出身地とか\nその他もろもろを入れて、出てきた検索\n結果は、とんでもないものだった。\n『ブrikppぽエ』っていう、名前の発音が\nすごく難しい国にある音響発電所で起きた\n大規模爆発。彼はその当事者だったのだ。\n生きているかもわからない絶望的な状況。\n多分、ピッカーも知ってるんだろうけど、\n一応聞いた。\n「あまり言いたくないけど、亡くなってる\nかもしれないよ。それでも探すのかい？」\n「うん、そのためにランナーになったから」\n「分かった。私もできるだけ協力するよ」\n以前まではネットでしか知れなかった情報。\n派遣という手段も増えた今、私にしかでき\nないこともあるだろう。ついでに、って\n言うと悪いかもしれないけど、情報くらいは\n集めてこようかな。", Cr);
			break;
		//EP2
		case 2050:
			DrawString(Cx, Cy, L"  午後17時56分、私はネット上でランナーと\n情報交換をしていた。どうも最近、\n音楽犯罪を中心に活動している団体が\n世界各地で暴れているようだ。私がいる\n地域ではそのような事件はまだ\n起こってないが、注意しておこう。\n  ゲーターにも言っておいた方が良いかな?\n最近平和だから、いざというときに\n動けないかも。あ、でも変なこと\n考えないかな? 「早く来いや」とか。\nうーん、どうしよう。\n  …あ、ああ! かわいい～! 周りで\n鳴っている音楽に合わせて踊ってくれる\nぬいぐるみ、新しくトラとクモとニワトリが\n出たんだ! …クモはちょっと気味悪いけど、\nトラかわいいーーー。いくらかな? 320pop\n(約2640円)かぁ、今度買ってみようかな♪\n  …あれ、メールが来てる。地元の人からだ、\n珍しいなぁ。どれどれ…。", Cr);
			break;
		case 2051:
			DrawString(Cx, Cy, L"ランナー事務所松坂支所\n総合担当者 御中\n\n  拝啓、貴社ますますご清祥のことと\nお慶び申し上げます。私立東松坂小学校\n教頭のマグニスです。\n  さて、私共は7月6日(土)に演奏会を\n開催する予定で、児童とともに日々練習を\n重ねております。しかしながら、私共の\n力不足もあり、練習がなかなかはかどって\nいない現状です。\n  そこで、音楽の専門であるランナーの\n皆様方にお力添えをいただきたく、連絡\nいたしました。つきましては、6月28日(金)\nの午後12時に、本校までお越し頂けない\nでしょうか。ご連絡、お待ちしております。\n\n私立東松坂小学校\n教頭 マグニス", Cr);
			break;
		case 2052:
			DrawString(Cx, Cy, L"「おお!? 本当かっ!」\n「久しぶりにランナーらしいお仕事が\n来たね♪」\n  ちょうど夕食の時間が近かったから、\nそのときにゲーターとピッカーに演奏会の\n依頼の話をしてみた。\n「よっしゃ! 久々に大暴れできるんだな! 」\n「あ、私たちは小学生に教えるだけだよ♪」\n「はぁ!?なんでだよ? 」\n  バァン!!\n  あ、台パン。気持ちは分かるけどやめて。\n「主役は小学生だよ。ランナーが出たら\nまずいでしょ♪」\n「はぁ～、つまんねーの」\n  とか言ってるけど、声の調子からして\n結構ノリノリだね。\n  あ、そうそう、学校にはしっかりお金は\n請求したよ。あくまでも仕事だからね♪", Cr);
			break;
		case 2060:
			DrawString(Cx, Cy, L"　金曜日の6月28日、私たちは小学校の\n体育館にいた。先生とは事前に打ち合わせを\nして、この後は一回演奏をしてもらうことに\nなっている。\n「今日はランナーの人たちと一緒に練習\nしますよ。それじゃランナーさん、あとは\nよろしくお願いします」\n　…あれ? 演奏は? 先生はそのまま体育館を\n出て行っちゃった。うーん、困ったなぁ。\n「そ、それじゃ、まずはそれぞれ自主練習ね。\n私たちは順番に見ていくからね♪」\n　後はよろしくって言われたから、もう\n好きにやっちゃおう。\n「予定がかなり変わっちゃっね。私は左の\n子から教えるから、ゲーターは右の子から\nお願いね。ピッカーはどっちからやる?」\n「ボクも左からやります」", Cr);
			break;
		case 2061:
			DrawString(Cx, Cy, L"　ピッカーと一緒に演奏を聞いてると、\n動きがぎこちない鉄琴奏者がいた。\n「ねぇねぇ、演奏は悪くないんだけど、\n慎重になりすぎてるね」\n「え? そんなことないよ」\n「そう? でも、レガートに演奏出来たら\nもっと良くなるよ♪」\n「……レガート?」\n「テイラーさん、\"レガート\"は小学校じゃ\n習いません」\n　あれ? そうだったっけ…?\n「えっと、\"レガート\"は\"滑らかに\"という\n意味なんだよ」\n「え、鉄琴を\"滑らかに\"演奏…?」\n「テイラーさん、小学生には難しいのでは?」\n　えー、私ってば要求しすぎ?\n「うーん、そっかぁ。ごめんね♪」", Cr);
			break;
		case 2062:
			DrawString(Cx, Cy, L"　途中からピッカーはゲーターと一緒に見て\n周ることになった。今は私一人だ。\n「あ、ランナーさん。ちょっと来てください」\n　誰かに呼ばれて振り返ると、ひとりの\nリコーダー奏者がこっちを見ていた。\n「低い\"レ\"の音が出ないんです」\n「え、音が出ない?」\n「高い\"レ\"になるんですよ」\n　そう言ってリコーダーを吹いた。\n　ピィーー♪\n「あ、多分それ、強く吹きすぎだね」\n「え?」\n「もっとレジェロ…じゃない、\n軽く吹いてみて♪」\n　プゥー♪\n「あ、できた!面白い!」\n　ピィープゥーピィープゥーピィープゥー♪\n\"レ\"のオクターブのトリルを吹き始めた。\n面白いのは分かるけど演奏の練習してね。", Cr);
			break;
		case 2070:
			DrawString(Cx, Cy, L"　練習が始まって3.13時間後、やっと先生が\n体育館に帰ってきた。呼びに行こうって\nずっと思ってたけど、教えるのが楽しくて\n結局呼びに行かなかった。\n「ランナーさん、今日はお忙しい中\nありがとうございました」\n「いえいえ、こちらこそ音楽で繋がる機会を\n与えてくださりありがとうございます♪」\n　正直なところ、先生も一緒にいて\nほしかった。長すぎて途中で小学生たちが\n飽きてたからね。ピッカーは疲れてるし、\nゲーターは愚痴ってる。聞こえてるよ。\n「いやー、ランナーってやっぱり変人の\n集まりなんですね」\n　うーん、一回この人の頭にネジ突っ込んで\nおこうか。足りてないだろうから。", Cr);
			break;
		case 2071:
			DrawString(Cx, Cy, L"　お仕事も終わって、帰ろうとしたとき、\nひとりの小学生がこっちに歩いてきた。\n「あの、すみません、ちょっといいですか?」\n「うん、どうしたの?」\n「原因がよく分からないんですけど、\nなんかうまく吹けないんです」\n「原因が分からない?」\n　ピッカーが不思議そうに聞いた。\n「はい、なんか違和感というか、…なんだろ」\n「そうだねぇ、一回吹いてみてよ♪」\n　こういう時は実際に聞くのが一番良い。\n\"百聞は一見に如かず\"っていうし、…あれ、\n今回は音楽だから、\"百聞は一聞に如かず\"\nかな? いや、なんか違うね。…もういいや♪\n　さて、肝心の演奏だけど、譜面は完璧に\n覚えてるみたい。大きなミスもしてない。\nけど…。", Cr);
			break;
		case 2072:
			DrawString(Cx, Cy, L"「どうでしょうか」\n　一通り演奏して、小学生が質問してきた。\n「よく吹けてたじゃねーか、違和感も何も\nなかったぞ」\n　えーと、ゲーター、まだまだ聞く力が\n足りないね。\n「はぁ…」\n　小学生は納得いってない様子。\n「うーん…、何か足りないような気はしたよ」\n　ピッカーも何かひっかかっている様子。\n　この小学生が吹いていたのはソプラノ\nサックス。リード系の楽器は奏者の\nコンディションも大きく現れる。この子の\n演奏には、失敗はないけどどこか不安定。\n余裕がないというか、音が硬いというか…。\nそうだなぁ、これは…。\n「うん、心当たりが一つだけあるね」", Cr);
			break;
		case 2080:
			DrawString(Cx, Cy, L"「もしかして、うまくやらなきゃとか、\nミスしないようにしなきゃとか考えてない?」\n「はぁ…、考えてますけど。でもそれ普通\nですよね?」\n「うーん、確かにミスしないように演奏する\nのは良いことなんだけど、それを意識する\nこと自体がミスなんだ♪」\n　……この説明で伝わるかな? また\nピッカーから難しいって怒られないかな?\n「じゃあランナーさんはどうやって演奏\nしてるんですか?」\n「ん? 私の演奏聞きたい?」\n「はい」\n　そっか、説明が難しいなら実演すれば\nいいんだ。\"百聞は一聞に如かず\"だね♪\n※違います。\n「うん、いいよ。曲はさっきと同じで\nいいよね♪」", Cr);
			break;
		case 2081:
			DrawString(Cx, Cy, L"　体育館の中、私たちと小学生しかいない\n空間で、私のソロコンサートが始まった♪\n「ゲーター、これ、テイラーの声だよね?」\n　そういえば、ピッカーに私の演奏を見せる\nのは初めてだったね。驚かせちゃったかな?\n「まぁ、機械だからな、機械音くらい\n出せるぜ」\n　機械音じゃなくてソプラノサックス!\nいつも言ってるのにゲーターはちっとも\nわかってくれないねっ!\n　私は半分機械のサイボーグ。あらゆる\n楽器の音を出せるように作られてる。ピアノ\nとかストリングスとかドラムとか。一番\n気に入ってるのはスクエアウェーブだね。\n\n　締めの音を出して演奏終了♪\n「さて、どうかな?」\n「…うまいです。どうしたらそんなにうまく\n出来るんですか?」", Cr);
			break;
		case 2082:
			DrawString(Cx, Cy, L"「\"楽しく\"演奏するんだ♪」\n「\"楽しく\"…?」\n「そう、本来演奏は楽しくやるものなんだ。\nそしたら聞き手も楽しくなるんだ。\"音楽\"\nっていう漢字だって、\"楽しい音\"って書く\nでしょ。特にリード系とか、息を使う楽器は\n奏者の心も出てくるからさ♪」\n　正直、音楽は楽しむものだということを\n教わらずに小学校を卒業していく子がとても\n多い気がする。音楽社会とはいえ、実際、\n学校を卒業してからも楽器に触れる人は5\%も\nいないらしい。せっかく滅多に触れない\nソプラノサックスを演奏できるんだから、\nここは楽しんでほしい。\n「確かに技術も必要だけど、無理に力を\n入れると音にも出てくるからさ。力を抜いて\n気楽に演奏しようよ♪」", Cr);
			break;
		case 2090:
			DrawString(Cx, Cy, L"　演奏会当日の朝8時36.45分、私たちは\n演奏会の様子を見に、再び東松坂小学校に\n来ていた。大勢の人が鑑賞しに来ていた。\n出店はやってないみたい。残念♪\n「あ、ランナーさん、いらしてたんですね」\n　あ、ネジが外れてる先生来た。REC。\n「いやー、ランナーさんも物好きですねー、\nこんなちんけな演奏会に来るなんて」\n　…今、音楽をバカにした? 頑張って\n練習してきた小学生たちをバカにした??\n「もしかしてヒマなんですかー?」\n　…でもまぁいいや。どうせこの人も\nもう終わりだし。演奏会終わった後が\n楽しみだなー♪\n　あ、そろそろ本番が始まる時間だ。\n急いで体育館に行かなきゃ♪", Cr);
			break;
		case 2091:
			DrawString(Cx, Cy, L"『みなさん、お待たせしました。これより、\n小学生たちによる演奏会を始めます』\n　なんとか間に合ったね。小学生も観客も、\nこの日を楽しみにしてたんだろうね。\n私たちもだよ♪\n　小学生が校歌を歌って演奏会が始まった。\nなかなか面白い歌詞だね。観客の誰かも少し\n笑ってる。この曲を歌うためにはまず\n笑わないようにすることから始まるのか。\n過酷だなぁ。w\n　面白い校歌が終わって、次は楽器演奏だ。\n流石に生身の人たちが演奏するだけあって、\nリズムや強さにばらつきがあるけど、これも\nまた面白い。\n「前のソプラノサックスの子、硬さがとれて\nうまく演奏できてるね。ちょっとミスってる\nけど気にならないくらいだね♪」\n　私の実演もあるだろうけど、あの子の\n技術力もあっての演奏だよ♪", Cr);
			break;
		case 2092:
			DrawString(Cx, Cy, L"「なるほど、こんなことが…」\n　演奏が終わった後、私は校長室に来ていた。\nこの部屋にいるのは、私と教頭先生と校長\n先生だ。\n　実は、練習の様子と、今日のあの先生の\n様子を全部録画していた。私の鼻先には\n小さなカメラが付いていて、いつでも記録\nすることができる。容量は1TBと大容量♪\n1.5カ月撮り続けることができるよ。\n「小学生たちにも同じようなことしている\nのかと思うと、正直怖くて…。なんとか\nなりませんか?」\n「うーん、とはいえ、これだけじゃあ\nなんとも言えないなぁ」\n　教頭先生が渋い顔をしている。こりゃ\nダメかな?\n「わかった、しばらくはあの先生の動向を\nモニターしてみよう。情報ありがとう」\n　おっ、校長先生ナイス。やっぱり言って\nみるものだね♪", Cr);
			break;
			//nature prhysm コラボストーリー(ただしnature prhysm作者(Take_Maさん)は関わっていません)
		case 3000:
			DrawString(Cx, Cy, L"「お前がテイラーか?\n見るからにメカメカしいな」\n「はい、少しの間ですが、\nよろしくお願いします♪」\nまさか派遣先でもゲーターみたいな人に\n会うとは思わなかったな。\n　私は今、ハーリッツという場所にある、\n自然管理センターのハーリッツ支所にいる。\n今私と話しているゴリラのボリスさんは、\nここで一人で働いているのだそうだ。\n「人手が足りないとのことで派遣された\nのですが、やはり一人だと大変ですか?」\n「勘違いするなよ。上司からの命令で\n呼んだんだ。俺はまだまだやれるさ」\nまぁ、まだ若そうなムキムキのゴリラさん\nだし、わざわざ私の助けはいらないかな。\n…あれ、それじゃどうして私は\n呼ばれたんだろう?", Cr);
			break;
		case 3001:
			DrawString(Cx, Cy, L"　あたりを見渡すと、額縁に入った一枚の\n写真が壁にかかっているのが見えた。\n「あれは、センターの社長の自画像さ」\nボリスさんが教えてくれた。青い髪を\n持った人間の顔が写ってた、けど、\n「画素数低くないですか?\nまるでドット絵のようですよ♪」\n「ドット絵なんだよ(怒)」\nおっと、怒らせちゃったかな…? 私って\n人を怒らせやすいからね、気をつけなきゃ。\n「社長もランナーなんですか?」\n「俺達はランナーとは違う。様々な異常\n気象を予測し、報道、必要なら調査を行う\nのが仕事だ。ランナーみたいに隙あらば\n突入しに行くような仕事じゃないんだ」\nランナーの仕事については75%合ってる。\nでもそれだけじゃないんだぞ。\nってことを言うと怒らせそうだから\n言わないでおいた。", Cr);
			break;
		case 3002:
			DrawString(Cx, Cy, L"「ところで、私は何をすれば良いですか?」\n「そうだな…、これからセンターの裏にある\n気象レーダーのメンテナンスをしに\n行くんだが、一緒に来るか?」\n「あ、是非行きます♪」\n「いや、待て、そういえば、あと3日で\nまとめて出さないといかん気象観測\nデータが残ってるんだった。\nこっちにするか」\n「あ、大丈夫ですっ。私機械いじりは\n得意なんですっ♪」\n「え、でも、ムズいぞ?\n結構精密な機械だぞ?」\n「私だって精密な機械ですからっ!\n早くメンテ行きましょっ♪」\nデータ整理なんてたまったものじゃない。\n機械いじりしてる方が92.4倍マシよ。", Cr);
			break;
		case 3010:
			DrawString(Cx, Cy, L"　翌日、あまり仕事がないらしく、結局\nデータ整理をやらされることになった。\nっていうかどうしてデジタル情報を\nわざわざ紙に書いて郵便で送らないと\nいけないの(怒)? せめてプリンターくらい\n設置しなさいよ。\n　私の頭の中ではもう計算は終わって\nいて、シャーペンを持っている手を動かす\n以外にすることがなくて退屈。もし今私が\n生身のドラゴンのままだったらとっくに\n寝てるわよ。\n　そうだ、派遣から帰った後のことを\n考えよう。とりあえず、帰ったら一曲作って\nみようかな。\"Digital of Analog\"って曲で、\nアナログの環境に拉致られたデジタルくんを\n想像してみよう。最初からBPMは215\nあたりで飛ばしていこう。アナログ(悪役)と\n戦うデジタルくんを想像するんだ。\n「お前、歌ってないでさっさとやれ」\n気がついたら歌ってた♪", Cr);
			break;
		case 3011:
			DrawString(Cx, Cy, L"「多いっ! こんなに書く必要あるんですか?」\n「ある。今後の気象予測には欠かせん」\n「とはいえ1週間分のデータを30分毎に\n手書きするなんて酷ですよー。\n推定8.2時間かかりますよ」\n「いや、そんなに早く終わらないだろ。\n計算もあるんだぞ」\n「私の機械の頭にかかれば計算なんて\n一瞬なのである♪」\n「すごいなお前」\n　自分からすすんで機械になったわけじゃ\nないけど、こういう時はよく助けて\nもらってる。バッテリーさえあれば\nちゃんと動くし♪\n「俺はちょっと食い物買いに行って来る。\nデータ整理頼むぞ」\n「はーい♪」\nボリスさんがセンターを出ていった。\n多分45分は帰ってこないかな。\nよし、ちょっとサボろ♪", Cr);
			break;
		case 3012:
			DrawString(Cx, Cy, L"\n　　ピガシャン!!\n\n　私は驚いて窓の外の景色を見る。\n大雨。しかも雷も一緒。35.6分前まで晴れて\nいたのに、こんなことがあるの!?\n　私の体がブルッと震える。大粒の雨が\n地面を突く音と、雨雲の中で電気が流れる\n音、そして雷がハーリッツの街を襲う音。\nこれらに混ざって、人々が騒ぎながら街を\n掛ける音がかすかに聞こえる。\n　私はこんな時でも、これらの音を\"曲\"\nとして聞いてしまう性癖がある。なんて\n迫力がある曲なんだ。私もこの曲の一員に\nなりたい。\n\n　気がついたら、私はセンターから\n飛び出していた。", Cr);
			break;
		case 3020:
			DrawString(Cx, Cy, L"　雨がピアノを叩き、人の声がバイオリンを\n弾き、時折雷がシンバルを打つ曲が流れる街。\n私は私の音を鳴らしながら走っていると、\nボリスさんに会った。\n「何やってるんだよお前…」\n「ごめんなさい、ついうっかり\n一曲奏でてました♪」\n「さすがランナーだな…」\nボリスさんの手には、何かが入っている\nビニール袋があった。多分お弁当かな?\n「でも、急に降り始めましたね。\nびっくりしましたよ」私が言った。\n「俺もだ、こんなの予測すら\nできてなかったぞ」\n再び雷がシンバルを打った。\n「テイラー、演奏の準備はできてるよな?」\n「あ、はい、バッチリ♪」\n「一緒に演奏するぞ。今回ばかりは\n俺一人じゃ厳しい」", Cr);
			break;
		case 3021:
			DrawString(Cx, Cy, L"　こうして私達の協奏が始まった。私は\nいろんな楽器の音を口から出して演奏した。\nボリスさんは、ランナーとは違う方法で演奏\nしていた。よく聞くと、3種類の音しか\n出していないことに気づいた。必要な音\nだけを鳴らして演奏する方法は、私が\n知ってる中だとクラシックに近いものだった。\n(最近、クラシックは古いとかショボいとか\n言われてるけど、クラシックなめないでよ♪\nクラシックは基礎がしっかり構築されてる曲\nなんだからね♪)\n「無駄がないすっきりした音色ですね♪」\nボリスさんはちょっと驚いた様子でこっちを\n見た。\n「お ? これの良さがわかるのか?」\n「はい、私は音と友達なので♪」\nボリスさんが少し笑ったような気がした。\n「よし、調査を続けるぞ。この気象の\nパターンを見つけるんだ」", Cr);
			break;
		case 3022:
			DrawString(Cx, Cy, L"　私たちは雨の中それぞれの方法で協奏\nしていた。私は大きい雷の音に負けない\nように、力強いエレキギターの音を出した。\n(シンセの音がどうしても混ざっちゃうけど)\nもはや私が雷に合わせるんじゃなくて、雷が\n私に合わせるんだ。\n　ほかの人はとっくに建物の中に\n入っちゃった。雨の中演奏しながら外を\n走ってる人って、よく考えたら変だよねw\n近所迷惑で怒られたら謝っとこ♪\n\n　体内時計じゃ1.24時間。私たちはずっと\n協奏してた。振っていた雨はだんだん弱く\nなって、雷はもう止んでいた。私たちは\nすっかりびしょ濡れになっていて、ボリス\nさんは一つくしゃみをした。私は体内に\nヒーターがあるから平気♪\n「一回センターに戻るぞ…」\nボリスさんが言った。\n「はい、お疲れ様です♪」", Cr);
			break;
		case 3030:
			DrawString(Cx, Cy, L"「えっと、それ本当ですか…?」\nあの楽しかった演奏会を終えた後、ボリス\nさんはまずシャワーを浴びに行った。私は\n今、私の体のメンテナンスをするために、\n身体のパーツを分解して、細かく見て\n回ってる。そんなにガタはないけど、一応\n全部見ておく。\n　そしてボリスさんは今、受話器の前で\nあんなことを言っていた。\n「ちょっと、それこっちじゃ予測できな\nかったんですが…、どういうことですか?」\nあのボリスさんが丁寧な言い方をしている。\nお偉いさんかな?\n「ああ、データならちゃんと取れましたよ。\nテイラーもよく働いてくれました」\n突然名前を呼ばれてびっくりした。私は\n結構好き勝手にやってたんだけどな♪", Cr);
			break;
		case 3031:
			DrawString(Cx, Cy, L"　ボリスさんの通話が終わったようで、私の\n方へ歩いてきた。私を見ると、少し驚いた\n様子だった。\n「すごいな、本当にロボットなんだな」\n「ああ、ちょうどメンテナンスしてたところ\nなんです♪」\nボリスさんはメンテ中の私を興味深そうに\n見ていた。\n「それで、何のお話だったんですか?」\n「え? ああ、あれか。さっきの大嵐だが、\n他のセンターでは予測できていたらしいんだ」\n「あら? それじゃ、ここのが壊れてたの\nでしょうか? 」\n「いや、ここのはデータが古いって言われた」\n「冗談でしょ!?頻繁に更新して\nなかったの!?」\nつい普段の口調になってしまった。\n「アナログなんだようちは…」\n\n　最悪。", Cr);
			break;
		case 3032:
			DrawString(Cx, Cy, L"「まぁ、俺がめんどくさがって、やって\nなかっただけだからな。叱られちまった」\nボリスさんが言った。ちょっといじめて\n差し上げようかな。\n「めんどくさがりの気持ちわかりますよ～、\n私がそうですから。でも、\nめんどくさいことには案外大切な\nものもあるんですよ～♪」\n「データ整理とかな」\n「あっ…」\nそういえばまだ仕事残ってたんだった。\n「あーもうアナログなんか大っ嫌いっ!」\n「そう言うな、俺も手伝うから」\n「せめてプリンターくらい置いてください…」\n「まぁ、そのうちな」", Cr);
			break;
			//EXミッション
		case 4000:
			DrawString(Cx, Cy, L"\n\n\n　　　　　　　　　　　　　3018年3月13日\n　　　　　　　オリヴァー・ハンビューヘン\n\n　　　　　音楽災害調査報告書\n\n1.発生日時: 3018年3月10日　AM0:50ごろ\n\n2.発生場所: Irreversible地方", Cr);
			break;
		case 4001:
			DrawString(Cx, Cy, L"\n3.災害状況:\n　特に何の前触れもないにも関わらず、\n時空間の歪みが各地に発生。あるところでは\n昼であり、他のある所では夜であった。また、\nある所では1秒が1分になっており、他のある\n所では1分が1秒になっていた。空の様子は\n滅茶苦茶であり、もはやこの場所には空と\nいうものは存在していないかのように思えた。\n　この場所が奏でていた音楽は、BPMは200と、\n音楽災害にしては遅めのペースに見えたが、\n32分の音が当たり前のように鳴っていたため、\n事実上は400であった。", Cr);
			break;
		case 4002:
			DrawString(Cx, Cy, L"\n4.処理状況:\n　現在、この地域は特別指定区域として、\n一般人及びゴールド以下のランナーの立ち\n入りを禁止している。時空間の歪みは日が\n経つとともに収まっているが、不測の事態に\n備えて、私、ハンビューヘンと、助手の\nマヒューズを配属している。常に監視し、\n状況をレポートしていく。\n\n　<追記>\n　現在は音楽社会であり、音楽製造物の扱い\nには十分気をつけなければならない。一人\nひとりがその自覚を持ち、安全に、快適に\n音楽を扱う必要がある。", Cr);
			break;
		case 4010:
			DrawString(Cx, Cy, L"………………………………………………。\n\nまたやってしまった。\n\nボクの×××××は、\nあたりに歪みを発生させる。\nその歪みに社員を巻き込んでしまった。\n\nもうこれで何回目だろう。\n流石にもう嫌になっているだろう。\n\n嫌われたかもしれない。\n\nもういいや、ボクはすべて失ってしまった。\nまた一人ぼっちだ。\nこれからは孤独に生きていこう。", Cr);
			break;
		case 4011:
			DrawString(Cx, Cy, L"　コンコン…\n\n「失礼します」\n「…ああ、×××××か。どうした?」\n「あの、あれから体調はどうですか?」\n「そんなに変わってないよ」\n「でも、落ち着いてはいるでしょ?」\n「まぁね」\n\n「もう出ていきなよ」\n「え?」\n「もう嫌になったでしょ。こんなボクに\n付き合うの。迷惑でしょ。もう好きに\nしなよ。離れて行ったってボクは文句\n言わないから。ボクは一人でなんとか\nするから」", Cr);
			break;
		case 4012:
			DrawString(Cx, Cy, L"「バカ言ってないで、さっさと休んで\n元気になってくださいよ。私たちはどこにも\n行きませんよ」\n\nボクは一人だ。\n…とは何度も言ってきたが、なんだかんだ\n言っていつもそばに誰かがいた。\n「もうっ、社長ったら、私がいないと\nだらしないんだから」\n\n今はまともに考えることができそうにない、\n彼女の言うとおり、今日はもう寝て\n回復させよう。\n\n「社長の鬱なんて、×××××事件と\n比べたらかわいいものですよ」\n「……比較でそれ持ってくる?」", Cr);
			break;
		}
		DrawGraph(0, 405, pageimg, TRUE);
		DrawFormatString(15, 420, Cr, L"%d / 3", com + 1);
		DrawGraph(0, 0, help, TRUE);
		DrawString(5, 460, L"上下キー:ページ選択   BackSpaceキー:戻る", Crw);
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_UP)) {
			//左が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				com--;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN)) {
			//右が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				com++;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK)) {
			//バックスペースが押された
			if (key == 0) {
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
		//閉じるボタンが押された
		else if (GetWindowUserCloseFlag(TRUE)) return 5;
		//特定のキーが押されていない
		else key = 0;
		com = mins(maxs(com, 2), 0);
	}
	return 0;
}