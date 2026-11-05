
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <RecSave.h>
#include <RecWindowRescale.h>
#include <RecSystem.h>

/* rec sub include */
#include <collect_seclet.h>

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
			help.DrawHelp(rec_helpbar_type_ec::COLLECT);
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
		rec_system_langstr_c chan[5] = {
			rec_system_langstr_c(_T("ピッカー"),        _T("Picker")),
			rec_system_langstr_c(_T("マップゲーター"),  _T("Mapgator")),
			rec_system_langstr_c(_T("テイラー"),        _T("Taylor")),
			rec_system_langstr_c(_T("サブストーリー1"), _T("Sub Story 1")),
			rec_system_langstr_c(_T("EX ミッション"),   _T("EX mission"))
		};
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
			RecRescaleDrawFormatString(330, 220, Cr, L"%s", chan[command[1]].get_str().c_str());
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

			help.DrawHelp(rec_helpbar_type_ec::COLLECT_STORY);
			ScreenFlip();
		}
		return 0;
	}
};

now_scene_t RecStory(void) {
	rec_clct_storyview_c action;
	switch (action.C_story()) {
	case 0:
		return SCENE_MENU;
	case 5:
		return SCENE_EXIT;
	}
}
