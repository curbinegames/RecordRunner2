
#include <sancur.h>
#include <strcur.h>
#include <RecWindowRescale.h>
#include <system.h>
#include <collect_seclet.h>
#include <option.h>

#if 1 /* C_item */

static void RecClctGetItemOpenFg(bool openFg[]) {
	double Grate = 0;
	int	play[7] = { 0,0,0,0,0,0,0 };
	play_rate_t rate2[RATE_NUM];
	FILE *fp;

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
		if (0 <= rate2[i].num && rate2[i].num <= 20) {
			Grate += rate2[i].num;
		}
	}

	openFg[0] = ( 1 <= play[1]);
	openFg[1] = ( 1 <= play[3]);
	openFg[2] = ( 1 <= play[4]);
	openFg[3] = ( 1 <= play[5]);
	openFg[4] = ( 1 <= play[6]);
	openFg[5] = ( 1 <= play[0]);
	openFg[6] = (10 <= play[0]);
	openFg[7] = (25 <= Grate);
	openFg[8] = (55 <= Grate);
	openFg[9] = (90 <= Grate);
	return;
}

static void RecClctDrawItemPictureLine(uint DrawX, int DrawY, uint StartNum, uint count,
	uint cmdX, uint cmdY, const bool openFg[], DxPic_t item[], DxPic_t win, DxPic_t cover)
{
	uint cmd_item_num = 0;
	uint DrawP = 0;
	switch (cmdY) {
	case 0:
		cmd_item_num = cmdX + 0;
		break;
	case 1:
		cmd_item_num = cmdX + 3;
		break;
	case 2:
		cmd_item_num = cmdX + 7;
		break;
	}
	for (int i = 0; i < count; i++) {
		DrawP = (openFg[i + StartNum]) ? (i + 1 + StartNum) : 0;
		RecRescaleDrawGraph(DrawX + i * 150, DrawY, win, TRUE);
		RecRescaleDrawGraph(DrawX + i * 150 + 3, DrawY + 3, item[DrawP], TRUE);
		if (i + StartNum != cmd_item_num) {
			RecRescaleDrawGraph(DrawX + i * 150, DrawY, cover, TRUE);
		}
	}
	return;
}

static void RecClctDrawItemDetail(uint cmdX, int cmdY, const bool openFg[]) {
	const rec_collect_item_set_t *const p_item_data = (optiondata.lang == 0) ? item_data_jp : item_data_en;
	uint itemNo = 0;
	switch (cmdY) {
	case 0:
		itemNo = cmdX + 0;
		break;
	case 1:
		itemNo = cmdX + 3;
		break;
	case 2:
		itemNo = cmdX + 7;
		break;
	}
	if (openFg[itemNo]) {
		RecRescaleDrawFormatString(50, 400, COLOR_WHITE, _T("%s:\n%s\n%s[%s]"),
			p_item_data[itemNo].name,
			p_item_data[itemNo].detail1,
			p_item_data[itemNo].detail2,
			p_item_data[itemNo].condition);
	}
	else {
		RecRescaleDrawFormatString(50, 400, COLOR_WHITE, _T("???\n[%s]"),
			p_item_data[itemNo].condition);
	}
	return;
}

static int C_item(void) {
	bool openFg[10] = { false,false,false,false,false,false,false,false,false,false };
	int	command[2] = { 0,0 };
	DxPic_t win = LoadGraph(L"picture/item枠.png");
	DxPic_t cover = LoadGraph(L"picture/itemカバー.png");
	DxPic_t backimg = LoadGraph(L"picture/COLLECT back.png");
	DxPic_t item[11] = {
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
	DxSnd_t sel = LoadSoundMem(L"sound/select.wav");
	rec_helpbar_c help;

	RecClctGetItemOpenFg(openFg);

	while (1) {
		InputAllKeyHold();
		switch (GetKeyPushOnce()) {
		case KEY_INPUT_LEFT:
			PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			command[0] = (command[0] + 3) % 4;
			if (command[0] == 3 && (command[1] == 0 || command[1] == 2)) { command[0] = 2; }
			break;
		case KEY_INPUT_RIGHT:
			PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			command[0] = (command[0] + 1) % 4;
			if (command[0] == 3 && (command[1] == 0 || command[1] == 2)) { command[0] = 0; }
			break;
		case KEY_INPUT_UP:
			PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			command[1] = (command[1] + 2) % 3;
			if (command[0] == 3 && command[1] == 0) { command[0] = 2; }
			break;
		case KEY_INPUT_DOWN:
			PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			command[1] = (command[1] + 1) % 3;
			if (command[0] == 3 && command[1] == 2) { command[0] = 2; }
			break;
		case KEY_INPUT_BACK:
			return 0;
		default:
			break;
		}

		if (GetWindowUserCloseFlag(TRUE)) { return 5; }

		ClearDrawScreen();
		RecRescaleDrawGraph(0, 0, backimg, TRUE);

		RecClctDrawItemPictureLine(110,  20, 0, 3, command[0], command[1], openFg, item, win, cover);
		RecClctDrawItemPictureLine( 40, 140, 3, 4, command[0], command[1], openFg, item, win, cover);
		RecClctDrawItemPictureLine(110, 260, 7, 3, command[0], command[1], openFg, item, win, cover);
		RecClctDrawItemDetail(command[0], command[1], openFg);

		help.DrawHelp(HELP_MAT_COLLECTION_ITEM);
		ScreenFlip();

		WaitTimer(10);
	}
	return 0;
}

#endif /* C_item */

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
		RecRescaleDrawGraph(0, 0, backimg, TRUE);
		RecRescaleDrawGraph(160, 0, noteimg, TRUE);
		RecCollectDrawStory(10 * a + 1000 * b + com); /* secret function */
		RecRescaleDrawGraph(0, 405, pageimg, TRUE);
		RecRescaleDrawFormatString(15, 420, Cr, L"%d / 3", com + 1);
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
		RecRescaleDrawGraph(0, 0, backimg, TRUE);
		RecRescaleDrawGraph(160, 0, noteimg, TRUE);
		RecRescaleDrawFormatString(5, 5, Cr, L"%d/%d", command[0], command[1]);
		if (lan[4] == 0)RecRescaleDrawFormatString(330, 220, Cr, L"%s", chan[command[1]]);
		else if (lan[4] == 1)RecRescaleDrawFormatString(330, 220, Cr, L"%s", chanE[command[1]]);
		if (command[1] < 3) {
			if (command[0] == 0) {
				if (chac[command[1]] >= 1) {
					if (lan[4] == 0)RecRescaleDrawString(300, 245, L"エピソード1-1", Cr);
					else if (lan[4] == 1)RecRescaleDrawString(300, 245, L"Episode 1-1", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)RecRescaleDrawFormatString(300, 245, Cr, L"%sを使って\n1回プレイしよう。", chan[command[1]]);
					else if (lan[4] == 1)RecRescaleDrawFormatString(300, 245, Cr, L"Play the game\nwith %s.", chanE[command[1]]);
					e = 0;
				}
			}
			else if (1 <= command[0] && command[0] <= 6) {
				if (chac[command[1]] >= command[0] * 2) {
					if (lan[4] == 0)RecRescaleDrawFormatString(300, 245, Cr, L"エピソード%d-%d", command[0] / 5 + 1, command[0] % 5 + 1);
					else if (lan[4] == 1)RecRescaleDrawFormatString(300, 245, Cr, L"Episode %d-%d", command[0] / 5 + 1, command[0] % 5 + 1);
					e = 1;
				}
				else {
					if (lan[4] == 0)RecRescaleDrawFormatString(300, 245, Cr, L"%sを使って\n%d回プレイしよう。", chan[command[1]], command[0] * 2);
					else if (lan[4] == 1)RecRescaleDrawFormatString(300, 245, Cr, L"Play the game\n%d times with %s.", command[0] * 2, chanE[command[1]]);
					e = 0;
				}
			}
			else {
				if (chac[command[1]] >= command[0] * 3 - 7) {
					if (lan[4] == 0)RecRescaleDrawFormatString(300, 245, Cr, L"エピソード%d-%d", command[0] / 5 + 1, command[0] % 5 + 1);
					else if (lan[4] == 1)RecRescaleDrawFormatString(300, 245, Cr, L"Episode %d-%d", command[0] / 5 + 1, command[0] % 5 + 1);
					e = 1;
				}
				else {
					if (lan[4] == 0)RecRescaleDrawFormatString(300, 245, Cr, L"%sを使って\n%d回プレイしよう。", chan[command[1]], command[0] * 3 - 7);
					else if (lan[4] == 1)RecRescaleDrawFormatString(300, 245, Cr, L"Play the game\n%d times with %s.", command[0] * 3 - 7, chanE[command[1]]);
					e = 0;
				}
			}
		}
		else if (command[1] == 3) {
			switch (command[0]) {
			case 0:
				if (sub1[command[0]] >= 90000) {
					if (lan[4] == 0)RecRescaleDrawString(300, 245, L"エピソード1", Cr);
					else if (lan[4] == 1)RecRescaleDrawString(300, 245, L"Episode 1", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)RecRescaleDrawString(300, 245, L"snow town story[EASY]以上で\nAランクを取ろう", Cr);
					else if (lan[4] == 1)RecRescaleDrawString(300, 245, L"Reath A RANK at\nsnow town story[EASY] or more dif.", Cr);
					e = 0;
				}
				break;
			case 1:
				if (sub1[command[0]] >= 90000) {
					if (lan[4] == 0)RecRescaleDrawString(300, 245, L"エピソード2", Cr);
					else if (lan[4] == 1)RecRescaleDrawString(300, 245, L"Episode 2", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)RecRescaleDrawString(300, 245, L"グラデーション・ワールド[EASY]以上で\nAランクを取ろう", Cr);
					else if (lan[4] == 1)RecRescaleDrawString(300, 245, L"Reath A RANK at\nGradation-world[EASY] or more dif.", Cr);
					e = 0;
				}
				break;
			case 2:
				if (sub1[command[0]] >= 95000) {
					if (lan[4] == 0)RecRescaleDrawString(300, 245, L"エピソード3", Cr);
					else if (lan[4] == 1)RecRescaleDrawString(300, 245, L"Episode 3", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)RecRescaleDrawString(300, 245, L"What Color Is The Sky?[EASY]以上で\nSランクを取ろう", Cr);
					else if (lan[4] == 1)RecRescaleDrawString(300, 245, L"Reath S RANK at\nWhat Color Is The Sky?[EASY] or more dif.", Cr);
					e = 0;
				}
				break;
			case 3:
				if (sub1[command[0]] >= 90000) {
					if (lan[4] == 0)RecRescaleDrawString(300, 245, L"エピソード4", Cr);
					else if (lan[4] == 1)RecRescaleDrawString(300, 245, L"Episode 4", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)RecRescaleDrawString(300, 245, L"トリノユメ[NOAMAL]以上で\nAランクを取ろう", Cr);
					else if (lan[4] == 1)RecRescaleDrawString(300, 245, L"Reath A RANK at\nTorinoyume[NOAMAL] or more dif.", Cr);
					e = 0;
				}
				break;
			}
		}
		else if (command[1] == 4) {
			switch (command[0]) {
			case 0:
				if (sub1[4] >= 98000) {
					RecRescaleDrawString(300, 245, L"appendix-1", Cr);
					e = 1;
				}
				else if (sub1[5] >= 1) {
					if (lan[4] == 0)RecRescaleDrawString(300, 245, L"トリノユメ[HOPENESS]で\nEXランクを取れ", Cr);
					else if (lan[4] == 1)RecRescaleDrawString(300, 245, L"Reath EX RANK at\nTorinoyume[HOPENESS].", Cr);
					e = 0;
				}
				else if (sub1[3] >= 90000) {
					if (lan[4] == 0)RecRescaleDrawString(300, 245, L"トリノユx功簾M_^簸]で\nEXランクを取れ", Cr);
					else if (lan[4] == 1)RecRescaleDrawString(300, 245, L"Reath EX RANK at\nTorim.v?&1/0)\\$cRD].", Cr);
					e = 0;
				}
				else {
					if (lan[4] == 0)RecRescaleDrawString(300, 245, L"刈ョ切簾M_^簸]\ne\\ny×蘭区奪", Cr);
					else if (lan[4] == 1)RecRescaleDrawString(300, 245, L"Rns(}1)Op&rrZ\ne\\ny456Ge/t0Z<=00ax(Q).", Cr);
					e = 0;
				}
				break;
			case 1:
				if (sub1[7] >= 98000) {
					RecRescaleDrawString(300, 245, L"appendix-2", Cr);
					e = 1;
				}
				else if (sub1[6] >= 1) {
					if (lan[4] == 0)RecRescaleDrawString(300, 245, L"drop DOWN DOWN[NIGHTMARE]で\nEXランクを取れ", Cr);
					else if (lan[4] == 1)RecRescaleDrawString(300, 245, L"Reath EX RANK at\ndrop DOWN DOWN[NIGHTMARE].", Cr);
					e = 0;
				}
				else {
					if (lan[4] == 0)RecRescaleDrawString(300, 245, L"↓↓↓\n↓↓↓↓↓↓", Cr);
					else if (lan[4] == 1)RecRescaleDrawString(300, 245, L"↓↓↓\n↓↓↓↓↓↓", Cr);
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
		RecRescaleDrawGraph(0, 0, backimg, TRUE);
		RecRescaleDrawGraph(70, 130, icon[0 + command], TRUE);
		RecRescaleDrawGraph(350, 130, icon[2 + command], TRUE);
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
