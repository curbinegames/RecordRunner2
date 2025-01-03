
#include <sancur.h>
#include <system.h>
#include <collect_seclet.h>

static int C_item(void) {
	//ロード
	int i;
	int j;
	int win = LoadGraph(L"picture/item枠.png");
	int cover = LoadGraph(L"picture/itemカバー.png");
	int backimg = LoadGraph(L"picture/COLLECT back.png");
	int item[11] = {
		LoadGraph(L"picture/item0.png"),
		LoadGraph(L"picture/item1.png"),
		LoadGraph(L"picture/item2.png"),
		LoadGraph(L"picture/item3.png"),
		LoadGraph(L"picture/item4.png"),
		LoadGraph(L"picture/item5.png"),
		LoadGraph(L"picture/item6.png"),
		LoadGraph(L"picture/item7.png"),
		LoadGraph(L"picture/item8.png"),
		LoadGraph(L"picture/item9.png"),
		LoadGraph(L"picture/item10.png")
	};
	int sel = LoadSoundMem(L"sound/select.wav");
	int flag[10] = { 0,0,0,0,0,0,0,0,0,0 };
	double Grate = 0;
	int	play[7] = { 0,0,0,0,0,0,0 };
	double rate[10] = { 0,0,0,0,0,0,0,0,0,0 };
	int	lan[6] = { 0,0,0,2,0,0 };//使うのは[4,言語]だけ
	play_rate_t rate2[RATE_NUM];
	rec_helpbar_c help;
	FILE *fp;
	(void)_wfopen_s(&fp, L"save/system.dat", L"rb");
	if (fp != NULL) {
		fread(&lan, sizeof(int), 5, fp);
		fclose(fp);
	}
	(void)_wfopen_s(&fp, L"save/data.dat", L"rb");
	if (fp != NULL) {
		fread(&play, sizeof(int), 7, fp);
		fclose(fp);
	}
	(void)_wfopen_s(&fp, RATE_FILE_NAME, L"rb");
	if (fp != NULL) {
		fread(&rate2[0], sizeof(play_rate_t), RATE_NUM, fp);
		fclose(fp);
	}
	for (int i = 0; i < RATE_NUM; i++) {
		if (0 <= rate2[i].num && rate2[i].num <= 20) { Grate += rate2[i].num; }
	}
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
				if (play[1] >= 1) {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_jp[command[0]].name, item_data_jp[command[0]].detail1, item_data_jp[command[0]].detail2, item_data_jp[command[0]].condition);
					else DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_en[command[0]].name, item_data_en[command[0]].detail1, item_data_en[command[0]].detail2, item_data_en[command[0]].condition);
				}
				else {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_jp[command[0]].condition);
					else DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_en[command[0]].condition);
				}
				break;
			case 1:
				if (play[3] >= 1) {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_jp[command[0]].name, item_data_jp[command[0]].detail1, item_data_jp[command[0]].detail2, item_data_jp[command[0]].condition);
					else DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_en[command[0]].name, item_data_en[command[0]].detail1, item_data_en[command[0]].detail2, item_data_en[command[0]].condition);
				}
				else {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_jp[command[0]].condition);
					else DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_en[command[0]].condition);
				}
				break;
			case 2:
				if (play[4] >= 1) {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_jp[command[0]].name, item_data_jp[command[0]].detail1, item_data_jp[command[0]].detail2, item_data_jp[command[0]].condition);
					else DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_en[command[0]].name, item_data_en[command[0]].detail1, item_data_en[command[0]].detail2, item_data_en[command[0]].condition);
				}
				else {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_jp[command[0]].condition);
					else DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_en[command[0]].condition);
				}
				break;
			}
			break;
		case 1:
			switch (command[0]) {
			case 0:
				if (play[5] >= 1) {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_jp[command[0] + 3].name, item_data_jp[command[0] + 3].detail1, item_data_jp[command[0] + 3].detail2, item_data_jp[command[0] + 3].condition);
					else DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_en[command[0] + 3].name, item_data_en[command[0] + 3].detail1, item_data_en[command[0] + 3].detail2, item_data_en[command[0] + 3].condition);
				}
				else {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_jp[command[0] + 3].condition);
					else DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_en[command[0] + 3].condition);
				}
				break;
			case 1:
				if (play[6] >= 1) {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_jp[command[0] + 3].name, item_data_jp[command[0] + 3].detail1, item_data_jp[command[0] + 3].detail2, item_data_jp[command[0] + 3].condition);
					else DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_en[command[0] + 3].name, item_data_en[command[0] + 3].detail1, item_data_en[command[0] + 3].detail2, item_data_en[command[0] + 3].condition);
				}
				else {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_jp[command[0] + 3].condition);
					else DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_en[command[0] + 3].condition);
				}
				break;
			case 2:
				if (play[0] >= 1) {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_jp[command[0] + 3].name, item_data_jp[command[0] + 3].detail1, item_data_jp[command[0] + 3].detail2, item_data_jp[command[0] + 3].condition);
					else DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_en[command[0] + 3].name, item_data_en[command[0] + 3].detail1, item_data_en[command[0] + 3].detail2, item_data_en[command[0] + 3].condition);
				}
				else {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_jp[command[0] + 3].condition);
					else DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_en[command[0] + 3].condition);
				}
				break;
			case 3:
				if (play[0] >= 10) {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_jp[command[0]].name, item_data_jp[command[0]].detail1, item_data_jp[command[0]].detail2, item_data_jp[command[0]].condition);
					else DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_en[command[0]].name, item_data_en[command[0]].detail1, item_data_en[command[0]].detail2, item_data_en[command[0]].condition);
				}
				else {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_jp[command[0]].condition);
					else DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_en[command[0]].condition);
				}
				break;
			}
			break;
		case 2:
			switch (command[0]) {
			case 0:
				if (Grate >= 25) {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_jp[command[0] + 7].name, item_data_jp[command[0] + 7].detail1, item_data_jp[command[0] + 7].detail2, item_data_jp[command[0] + 7].condition);
					else DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_en[command[0] + 7].name, item_data_en[command[0] + 7].detail1, item_data_en[command[0] + 7].detail2, item_data_en[command[0] + 7].condition);
				}
				else {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_jp[command[0] + 7].condition);
					else DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_en[command[0] + 7].condition);
				}
				break;
			case 1:
				if (Grate >= 55) {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_jp[command[0] + 7].name, item_data_jp[command[0] + 7].detail1, item_data_jp[command[0] + 7].detail2, item_data_jp[command[0] + 7].condition);
					else DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_en[command[0] + 7].name, item_data_en[command[0] + 7].detail1, item_data_en[command[0] + 7].detail2, item_data_en[command[0] + 7].condition);
				}
				else {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_jp[command[0] + 7].condition);
					else DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_en[command[0] + 7].condition);
				}
				break;
			case 2:
				if (Grate >= 90) {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_jp[command[0] + 7].name, item_data_jp[command[0] + 7].detail1, item_data_jp[command[0] + 7].detail2, item_data_jp[command[0] + 7].condition);
					else DrawFormatString(50, 400, Cr, _T("%s:\n%s\n%s[%s]"), item_data_en[command[0] + 7].name, item_data_en[command[0] + 7].detail1, item_data_en[command[0] + 7].detail2, item_data_en[command[0] + 7].condition);
				}
				else {
					if (lan[4] == 0) DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_jp[command[0] + 7].condition);
					else DrawFormatString(50, 400, Cr, _T("???\n[%s]"), item_data_en[command[0] + 7].condition);
				}
				break;
			}
			break;
		}
		help.DrawHelp(HELP_MAT_COLLECTION_ITEM);
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

static int story(int a, int b) {
	int key = 1, com = 0, Cx = 220, Cy = 75, backimg, noteimg, pageimg, sel;
	unsigned int Cr, Crw;
	rec_helpbar_c help;

	Cr = GetColor(0, 0, 0);
	Crw = GetColor(255, 255, 255);
	backimg = LoadGraph(L"picture/COLLECT back.png");
	noteimg = LoadGraph(L"picture/Snote.png");
	pageimg = LoadGraph(L"picture/page.png");
	sel = LoadSoundMem(L"sound/select.wav");
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		DrawGraph(160, 0, noteimg, TRUE);
		RecCollectDrawStory(10 * a + 1000 * b + com); /* secret function */
		DrawGraph(0, 405, pageimg, TRUE);
		DrawFormatString(15, 420, Cr, L"%d / 3", com + 1);
		help.DrawHelp(HELP_MAT_COLLECTION);
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

static int C_story(void) {
	int e, i, j, end = 0, key = 1, backimg, noteimg, sel, chac[3] = { 0,0,0 }, sub1[8], command[2] = { 0,0 }, g[7];
	int StoryUpper[5] = { 10,10,10,4,2 };
	wchar_t chan[5][9] = { L"ピッカー",L"マップゲーター",L"テイラー",L"サブストーリー1",L"EX ミッション" };
	wchar_t chanE[5][12] = { L"Picker",L"Mapgator",L"Taylor",L"Sub Story 1",L"EX mission" };
	unsigned int Cr, Crw;
	int	lan[6] = { 0,0,0,2,0,0 };//使うのは[4,言語]だけ
	rec_helpbar_c help;
	FILE *fp;
	e = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (fp != NULL) {
		fread(&lan, sizeof(int), 6, fp);
		fclose(fp);
	}
	//キャラプレイ回数読み込み
	e = _wfopen_s(&fp, L"save/chap.dat", L"rb");
	if (fp != NULL) {
		fread(&chac, sizeof(int), 3, fp);
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
		if (fp != NULL) {
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
		if (fp != NULL) {
			fread(&g, sizeof(int), 6, fp);
			fclose(fp);
			for (j = g[6]; j <= 5; j++) sub1[i] = mins(sub1[i], g[j]);
		}
	}
	Cr = GetColor(0, 0, 0);
	Crw = GetColor(255, 255, 255);
	backimg = LoadGraph(L"picture/COLLECT back.png");
	noteimg = LoadGraph(L"picture/Cnote.png");
	sel = LoadSoundMem(L"sound/select.wav");
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
		help.DrawHelp(HELP_MAT_COLLECTION_STORY);
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

now_scene_t collection(void) {
	int command = 0;
	int end = 0;
	int backimg = LoadGraph(L"picture/COLLECT back.png");
	int icon[4] = {
		LoadGraph(L"picture/CLI.png"),
		LoadGraph(L"picture/CLIG.png"),
		LoadGraph(L"picture/CLSG.png"),
		LoadGraph(L"picture/CLS.png")
	};
	int sel = LoadSoundMem(L"sound/select.wav");
	const int keyCB[4] = {
		KEY_INPUT_RETURN, KEY_INPUT_BACK, KEY_INPUT_LEFT, KEY_INPUT_RIGHT
	};
	unsigned int Cr = GetColor(255, 255, 255);
	rec_helpbar_c help;

	AvoidKeyRush();
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		DrawGraph(70, 130, icon[0 + command], TRUE);
		DrawGraph(350, 130, icon[2 + command], TRUE);
		help.DrawHelp(HELP_MAT_COLLECTION);
		ScreenFlip();

		/* キー入力 */
		{
			bool breakFg = false;
			InputAllKeyHold();
			switch (GetKeyPushOnce()) {
			case KEY_INPUT_RETURN:
				switch (command) {
				case 0:
					end = C_item();
					AvoidKeyRush();
					break;
				case 1:
					end = C_story();
					AvoidKeyRush();
					break;
				}
				if (end == 5) { return SCENE_EXIT; }
				break;
			case KEY_INPUT_BACK:
				breakFg = true;
				break;
			case KEY_INPUT_LEFT:
				command = (command + 1) % 2;
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				break;
			case KEY_INPUT_RIGHT:
				command = (command + 3) % 2;
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				break;
			default:
				break;
			}
			if (breakFg) { break; }
		}

		//×ボタンが押された
		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }
		WaitTimer(10);
	}
	return SCENE_MENU;
}
