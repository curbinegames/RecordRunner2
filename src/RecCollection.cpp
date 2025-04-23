
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <option.h>
#include <RecSave.h>
#include <RecWindowRescale.h>
#include <RecSystem.h>

/* rec sub include */
#include <collect_seclet.h>

static class rec_clct_itemview_c {
private:
	bool openFg[10] = { false,false,false,false,false,false,false,false,false,false };
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
	DxSnd_t selectSnd = DXLIB_SND_NULL;

private:
	void RecClctGetItemOpenFg(void) {
		const double Grate = RecSaveGetFullRunnerRate();
		rec_user_data_t userData;

		RecSaveReadUserPlay(&userData);

		this->openFg[0] = ( 1 <= userData.dropCount);
		this->openFg[1] = ( 1 <= userData.clearCount);
		this->openFg[2] = ( 1 <= userData.NMCount);
		this->openFg[3] = ( 1 <= userData.FCCount);
		this->openFg[4] = ( 1 <= userData.PFcount);
		this->openFg[5] = ( 1 <= userData.playCount);
		this->openFg[6] = (10 <= userData.playCount);
		this->openFg[7] = (25 <= Grate);
		this->openFg[8] = (55 <= Grate);
		this->openFg[9] = (90 <= Grate);
		return;
	}

public:
	rec_clct_itemview_c(void) {
		this->selectSnd = LoadSoundMem(L"sound/select.wav");
		ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, this->selectSnd);
		this->RecClctGetItemOpenFg();
	}

private:
	void RecClctDrawItemPictureLine(uint DrawX, int DrawY, uint StartNum, uint count,
		uint cmdX, uint cmdY)
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
			DrawP = (this->openFg[i + StartNum]) ? (i + 1 + StartNum) : 0;
			RecRescaleDrawGraph(DrawX + i * 150, DrawY, this->win, TRUE);
			RecRescaleDrawGraph(DrawX + i * 150 + 3, DrawY + 3, this->item[DrawP], TRUE);
			if (i + StartNum != cmd_item_num) {
				RecRescaleDrawGraph(DrawX + i * 150, DrawY, this->cover, TRUE);
			}
		}
		return;
	}

	void RecClctDrawItemDetail(uint cmdX, int cmdY) {
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
		if (this->openFg[itemNo]) {
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

public:
	int C_item(void) {
		int	command[2] = { 0,0 };
		rec_helpbar_c help;

		while (1) {
			InputAllKeyHold();
			switch (GetKeyPushOnce()) {
			case KEY_INPUT_LEFT:
				PlaySoundMem(this->selectSnd, DX_PLAYTYPE_NORMAL);
				command[0] = (command[0] + 3) % 4;
				if (command[0] == 3 && (command[1] == 0 || command[1] == 2)) { command[0] = 2; }
				break;
			case KEY_INPUT_RIGHT:
				PlaySoundMem(this->selectSnd, DX_PLAYTYPE_NORMAL);
				command[0] = (command[0] + 1) % 4;
				if (command[0] == 3 && (command[1] == 0 || command[1] == 2)) { command[0] = 0; }
				break;
			case KEY_INPUT_UP:
				PlaySoundMem(this->selectSnd, DX_PLAYTYPE_NORMAL);
				command[1] = (command[1] + 2) % 3;
				if (command[0] == 3 && command[1] == 0) { command[0] = 2; }
				break;
			case KEY_INPUT_DOWN:
				PlaySoundMem(this->selectSnd, DX_PLAYTYPE_NORMAL);
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
			RecRescaleDrawGraph(0, 0, this->backimg, TRUE);

			this->RecClctDrawItemPictureLine(110,  20, 0, 3, command[0], command[1]);
			this->RecClctDrawItemPictureLine( 40, 140, 3, 4, command[0], command[1]);
			this->RecClctDrawItemPictureLine(110, 260, 7, 3, command[0], command[1]);
			this->RecClctDrawItemDetail(command[0], command[1]);

			help.DrawHelp(HELP_MAT_COLLECTION_ITEM);
			ScreenFlip();

			WaitTimer(WAIT_TIME_ON_GAMELOOP);
		}
		return 0;
	}
};

#if 1 /* C_story */

static class rec_clct_storyview_c {
private:
	void RecClctGetCharaPlayCount(int charaPlayCount[]) {
		rec_save_charaplay_t buf;
		RecSaveReadCharaPlay(&buf);
		charaPlayCount[0] = buf.picker;
		charaPlayCount[1] = buf.mapgator;
		charaPlayCount[2] = buf.taylor;
		return;
	}

	void RecClctGetSubStoryScore(int subScore[]) {
		uint minDif = 1;

		for (int i = 0; i < 4; i++) {
			rec_save_score_t databuf[6];
			subScore[i] = 0;
			switch (i) {
			case 0:
				RecSaveReadScoreAllDif(databuf, L"snow town story");
				minDif = 1;
				break;
			case 1:
				RecSaveReadScoreAllDif(databuf, L"Gradation-world");
				minDif = 1;
				break;
			case 2:
				RecSaveReadScoreAllDif(databuf, L"What Color Is The Sky");
				minDif = 1;
				break;
			case 3:
				RecSaveReadScoreAllDif(databuf, L"Torinoyume");
				minDif = 2;
				break;
			}
			for (uint j = minDif; j <= 5; j++) {
				subScore[i] = maxs_2(subScore[i], databuf[j].score);
			}
		}
		return;
	}

	void RecClctGetExStoryScore(int exScore[]) {
		rec_save_score_t databuf[6];

		for (int i = 0; i < 2; i++) {
			switch (i) {
			case 0:
				RecSaveReadScoreAllDif(databuf, L"Torinoyume");
				break;
			case 1:
				RecSaveReadScoreAllDif(databuf, L"drop DOWN DOWN");
				break;
			}

			for (uint j = 0; j < 3; j++) {
				exScore[i * 3 + j] = databuf[j + 3].score;
			}
		}
		return;
	}

	bool RecClctCheckOpenMainStory(int cmdX, int cmdY, int charaPlayCount[]) {
		uint for_open_count = 0;
		if      (cmdX == 0)              { for_open_count = 1; }
		else if (1 <= cmdX && cmdX <= 6) { for_open_count = cmdX * 2; }
		else                             { for_open_count = cmdX * 3 - 7; }
		if (for_open_count <= charaPlayCount[cmdY]) { return true; }
		return false;
	}

	void RecClctDrawMainStoryTitle(int cmdX, int cmdY, bool openFg) {
		TCHAR charaName[16];
		TCHAR GT1[128];

		switch (cmdY) {
		case 0:
			strcopy_2(REC_STR_LANG(_T("ピッカー"), _T("Picker")), charaName, 16);
			break;
		case 1:
			strcopy_2(REC_STR_LANG(_T("マップゲーター"), _T("Mapgator")), charaName, 16);
			break;
		case 2:
			strcopy_2(REC_STR_LANG(_T("テイラー"), _T("Taylor")), charaName, 16);
			break;
		}

		if (openFg == true) {
			swprintf_s(GT1, sizeof(GT1) / sizeof(GT1[0]), REC_STR_LANG(_T("エピソード%d-%d"), _T("Episode %d-%d")), cmdX / 5 + 1, cmdX % 5 + 1);
		}
		else {
			uint for_open_count = 0;
			if      (cmdX == 0)              { for_open_count = 1; }
			else if (1 <= cmdX && cmdX <= 6) { for_open_count = cmdX * 2; }
			else                             { for_open_count = cmdX * 3 - 7; }
			if (optiondata.lang == 0) { /* 単純にREC_STR_LANG()は使えなかった */
				swprintf_s(GT1, sizeof(GT1) / sizeof(GT1[0]), _T("%sを使って\n%d回プレイしよう。"), charaName, for_open_count);
			}
			else {
				swprintf_s(GT1, sizeof(GT1) / sizeof(GT1[0]), _T("Play the game\n%d times with %s."), for_open_count, charaName);
			}
		}
		RecRescaleDrawString(300, 245, GT1, COLOR_BLACK);
		return;
	}

	bool RecClctCheckOpenSubStory(int cmdX, int score[]) {
		uint target_score;
		switch (cmdX) {
		case 0:
		case 1:
			target_score = 90000;
			break;
		case 2:
		case 3:
			target_score = 95000;
			break;
		}

		if (score[cmdX] >= target_score) { return true; }
		return false;
	}

	void RecClctDrawSubStoryTitle(int cmdX, bool openFg) {
		TCHAR song_name[32];
		TCHAR dif_name[32];
		TCHAR target_str;
		TCHAR GT1[64];

		switch (cmdX) {
		case 0:
			strcopy_2(_T("snow town story"), song_name, 32);
			strcopy_2(_T("EASY"), dif_name, 32);
			target_str = _T('A');
			break;
		case 1:
			strcopy_2(REC_STR_LANG(_T("グラデーション・ワールド"), _T("Gradation-world")), song_name, 32);
			strcopy_2(_T("EASY"), dif_name, 32);
			target_str = _T('A');
			break;
		case 2:
			strcopy_2(_T("What Color Is The Sky?"), song_name, 32);
			strcopy_2(_T("EASY"), dif_name, 32);
			target_str = _T('S');
			break;
		case 3:
			strcopy_2(REC_STR_LANG(_T("トリノユメ"), _T("Torinoyume")), song_name, 32);
			strcopy_2(_T("NORMAL"), dif_name, 32);
			target_str = _T('S');
			break;
		}

		if (openFg == 1) {
			swprintf_s(GT1, sizeof(GT1) / sizeof(GT1[0]), REC_STR_LANG(_T("エピソード%d"), _T("Episode %d")), cmdX + 1);
		}
		else {
			if (optiondata.lang == 0) { /* 単純にREC_STR_LANG()は使えなかった */
				swprintf_s(GT1, sizeof(GT1) / sizeof(GT1[0]), _T("%s[%s]以上で\n%cランクを取ろう"), song_name, dif_name, target_str);
			}
			else {
				swprintf_s(GT1, sizeof(GT1) / sizeof(GT1[0]), _T("Reath %c RANK at\n%s[%s] or more dif."), target_str, song_name, dif_name);
			}
		}
		RecRescaleDrawString(300, 245, GT1, COLOR_BLACK);
		return;
	}

	int RecClctCheckOpenWayExStory(int cmdX, int score[]) {
		if (score[cmdX * 3 + 1] >= 98000) {
			return 3;
		}
		if (score[cmdX * 3 + 2] >= 1) {
			return 2;
		}
		if (score[cmdX * 3] >= 90000) {
			return 1;
		}
		return 0;
	}

	void RecClctDrawExStoryTitle(int cmdX, int openWay) {
		TCHAR GT1[64];
		switch (cmdX) {
		case 0:
			if (openWay == 3) {
				swprintf_s(GT1, sizeof(GT1) / sizeof(GT1[0]), _T("appendix-%d"), cmdX + 1);
			}
			else if (openWay == 2) {
				strcopy_2(REC_STR_LANG(_T("トリノユメ[HOPENESS]で\nEXランクを取れ"), _T("Reath EX RANK at\nTorinoyume[HOPENESS].")), GT1, 64);
			}
			else if (openWay == 1) {
				strcopy_2(REC_STR_LANG(_T("トリノユx功簾M_^簸]で\nEXランクを取れ"), _T("Reath EX RANK at\nTorim.v?&1/0)\\$cRD].")), GT1, 64);
			}
			else {
				strcopy_2(REC_STR_LANG(_T("刈ョ切簾M_^簸]\ne\\ny×蘭区奪"), _T("Rns(}1)Op&rrZ\ne\\ny456Ge/t0Z<=00ax(Q).")), GT1, 64);
			}
			break;
		case 1:
			if (openWay == 3) {
				swprintf_s(GT1, sizeof(GT1) / sizeof(GT1[0]), _T("appendix-%d"), cmdX + 1);
			}
			else if (openWay == 2) {
				strcopy_2(REC_STR_LANG(_T("drop DOWN DOWN[NIGHTMARE]で\nEXランクを取れ"), _T("Reath EX RANK at\ndrop DOWN DOWN[NIGHTMARE].")), GT1, 64);
			}
			else if (openWay == 1) {
				strcopy_2(REC_STR_LANG(_T("drop DOWN DO↓[↓↓↓↓\n↓↓↓ｹ↓↓↓↓↓"), _T("Rea↓↓↓ ↓↓at\n↓↓↓WN DOW↓↓↓↓↓↓↓].")), GT1, 64);
			}
			else {
				strcopy_2(_T("↓↓↓\n↓↓↓↓↓↓"), GT1, 64); /* jp,en 一緒 */
			}
			break;
		}
		RecRescaleDrawString(300, 245, GT1, COLOR_BLACK);
		return;
	}

	int story(int a, int b) {
		int key = 1, com = 0, Cx = 220, Cy = 75, backimg, noteimg, pageimg, sel;
		unsigned int Cr, Crw;
		rec_helpbar_c help;

		Cr = GetColor(0, 0, 0);
		Crw = GetColor(255, 255, 255);
		backimg = LoadGraph(L"picture/COLLECT back.png");
		noteimg = LoadGraph(L"picture/Snote.png");
		pageimg = LoadGraph(L"picture/page.png");
		sel = LoadSoundMem(L"sound/select.wav");
		ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, sel);
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
			com = betweens(0, com, 2);
		}
		return 0;
	}

public:
	int C_story(void) {
		bool openFg = false;
		int chac[3] = { 0,0,0 }, command[2] = { 0,0 };
		DxPic_t backimg, noteimg;
		DxSnd_t sel;
		int sub1[4];
		int sub2[6];
		const int StoryUpper[5] = { 10,10,10,4,2 };
		const wchar_t chan[5][9] = { L"ピッカー",L"マップゲーター",L"テイラー",L"サブストーリー1",L"EX ミッション" };
		const wchar_t chanE[5][12] = { L"Picker",L"Mapgator",L"Taylor",L"Sub Story 1",L"EX mission" };
		TCHAR GT1[256] = _T("");
		unsigned int Cr;
		rec_helpbar_c help;

		// ストーリー解禁に関わるパラメータの読み込み
		RecClctGetCharaPlayCount(chac);
		RecClctGetSubStoryScore(sub1);
		RecClctGetExStoryScore(sub2);

		Cr = GetColor(0, 0, 0);
		backimg = LoadGraph(L"picture/COLLECT back.png");
		noteimg = LoadGraph(L"picture/Cnote.png");
		sel = LoadSoundMem(L"sound/select.wav");
		ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, sel);
		AvoidKeyRush();
		while (1) {
			InputAllKeyHold();
			switch (GetKeyPushOnce()) {
			case KEY_INPUT_LEFT:
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[1] = (command[1] + 4) % 5;
				command[0] = 0;
				break;
			case KEY_INPUT_RIGHT:
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[1] = (command[1] + 1) % 5;
				command[0] = 0;
				break;
			case KEY_INPUT_UP:
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[0] = (command[0] + StoryUpper[command[1]] - 1) % StoryUpper[command[1]];
				break;
			case KEY_INPUT_DOWN:
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[0] = (command[0] + 1) % StoryUpper[command[1]];
				break;
			case KEY_INPUT_RETURN:
				if (openFg) {
					if (story(command[0], command[1]) == 5) { return 5; }
					AvoidKeyRush();
				}
				break;
			case KEY_INPUT_BACK:
				return 0;
			default:
				break;
			}
			if (GetWindowUserCloseFlag(TRUE)) { return 5; }

			ClearDrawScreen();
			RecRescaleDrawGraph(0, 0, backimg, TRUE);
			RecRescaleDrawGraph(160, 0, noteimg, TRUE);

			// ストーリーのタイトル/解放条件を表示
			RecRescaleDrawFormatString(5, 5, Cr, L"%d/%d", command[0], command[1]);
			RecRescaleDrawFormatString(330, 220, Cr, L"%s", REC_STR_LANG(chan[command[1]], chanE[command[1]]));
			if (command[1] <= 2) {
				openFg = RecClctCheckOpenMainStory(command[0], command[1], chac);
				RecClctDrawMainStoryTitle(command[0], command[1], openFg);
			}
			else if (command[1] == 3) {
				openFg = RecClctCheckOpenSubStory(command[0], sub1);
				RecClctDrawSubStoryTitle(command[0], openFg);
			}
			else if (command[1] == 4) {
				uint openWay = RecClctCheckOpenWayExStory(command[0], sub2);
				openFg = (openWay == 3);
				RecClctDrawExStoryTitle(command[0], openWay);
			}

			help.DrawHelp(HELP_MAT_COLLECTION_STORY);
			ScreenFlip();
		}
		return 0;
	}
};

#endif /* C_story */

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
	ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, sel);
	rec_helpbar_c help;

	// AvoidKeyRush();
	while (1) {
		ClearDrawScreen();
		RecRescaleDrawGraph(0, 0, backimg, TRUE);
		RecRescaleDrawGraph(70, 130, icon[0 + command], TRUE);
		RecRescaleDrawGraph(350, 130, icon[2 + command], TRUE);
		help.DrawHelp(HELP_MAT_COLLECTION);
		ScreenFlip();

		/* キー入力 */
		InputAllKeyHold();
		switch (GetKeyPushOnce()) {
		case KEY_INPUT_RETURN:
			switch (command) {
			case 0:
				{
					rec_clct_itemview_c action;
					end = action.C_item();
				}
				break;
			case 1:
				{
					rec_clct_storyview_c action;
					end = action.C_story();
				}
				break;
			}
			// AvoidKeyRush();
			if (end == 5) { return SCENE_EXIT; }
			break;
		case KEY_INPUT_BACK:
			return SCENE_MENU;
		case KEY_INPUT_LEFT:
		case KEY_INPUT_RIGHT:
			command = (command + 1) % 2;
			PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			break;
		default:
			break;
		}

		//×ボタンが押された
		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }
		WaitTimer(WAIT_TIME_ON_GAMELOOP);
	}
	return SCENE_MENU;
}
