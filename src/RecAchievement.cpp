
#include <DxLib.h>
#include <dxcur.h>
#include <RecSave.h>
#include <RecWindowRescale.h>
#include <collect_seclet.h>

static class rec_cursor_pic_c {
private:
	bool is_to_white = false;
	DxTime_t Stime = GetNowCount();
	dxcur_window_pic_c curpic{_T("picture/cursorwindow.png")};
	dxcur_window_pic_c curwpic{_T("picture/cursorwindowwhite.png")};

public:
	void update(void) {
		DxTime_t Ntime = GetNowCount();
		if (this->Stime + 500 < Ntime) {
			this->Stime = Ntime;
			this->is_to_white = !this->is_to_white;
		}
	}

	void draw(int left, int up, int right, int down) const {
		DxTime_t Ntime = GetNowCount() - this->Stime;
		int alpha = 255;
		if (this->is_to_white) {
			alpha = lins_scale(0, 0, 500, 255, Ntime);
		}
		else {
			alpha = lins_scale(0, 255, 500, 0, Ntime);
		}
		curpic.draw(left, up, right, down);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		curwpic.draw(left, up, right, down);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	}
};

static class rec_achivement_c {
private:
	static constexpr uint list_gap = 40;

	bool openFg[10] = { false,false,false,false,false,false,false,false,false,false };
	rec_cursor_pic_c cursor;
	dxcur_pic_c win{_T("picture/achievement/item˜g.png")};
	dxcur_pic_c backimg{_T("picture/COLLECT back.png")};
	dxcur_pic_c null_item{_T("picture/achievement/item0.png")};
	dxcur_window_pic_c winpic{_T("picture/achievement/window.png")};
	dxcur_pic_c item[10] = {
		dxcur_pic_c{_T("picture/achievement/item1.png")},
		dxcur_pic_c{_T("picture/achievement/item2.png")},
		dxcur_pic_c{_T("picture/achievement/item3.png")},
		dxcur_pic_c{_T("picture/achievement/item4.png")},
		dxcur_pic_c{_T("picture/achievement/item5.png")},
		dxcur_pic_c{_T("picture/achievement/item6.png")},
		dxcur_pic_c{_T("picture/achievement/item7.png")},
		dxcur_pic_c{_T("picture/achievement/item8.png")},
		dxcur_pic_c{_T("picture/achievement/item9.png")},
		dxcur_pic_c{_T("picture/achievement/item10.png")}
	};
	rec_se_system_c selectSnd{_T("sound/select.wav")};

private:
	void GetItemOpenFg(void) {
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

	void DrawItemName(uint left, uint up, uint down) {
		const rec_collect_item_set_t *const p_item_data = (optiondata.lang == 0) ? item_data_jp : item_data_en;
		for (uint i = 0; i < 10; i++) {
			if (up + i * this->list_gap > down) { break; }
			if (this->openFg[i]) {
				DrawFormatString(left, up + i * this->list_gap, COLOR_WHITE, _T("%s"), p_item_data[i].name.c_str());
			}
			else {
				DrawFormatString(left, up + i * this->list_gap, COLOR_WHITE, _T("???"));
			}
		}
		return;
	}

	void DrawItemPic(uint DrawX, int DrawY, uint cmd) {
		int size = 300;
		DrawExtendGraph(DrawX, DrawY, DrawX + size, DrawY + size, this->win.handle(), TRUE);
		if (this->openFg[cmd]) {
			DrawExtendGraph(DrawX + 3, DrawY + 3, DrawX + size - 3, DrawY + size - 3, this->item[cmd].handle(), TRUE);
		}
		else {
			DrawExtendGraph(DrawX + 3, DrawY + 3, DrawX + size - 3, DrawY + size - 3, this->null_item.handle(), TRUE);
		}
		return;
	}

	void DrawItemDetail(int x, int y, uint cmd) {
		const rec_collect_item_set_t *const p_item_data = (optiondata.lang == 0) ? item_data_jp : item_data_en;
		uint itemNo = cmd;
		if (this->openFg[itemNo]) {
			DrawFormatString(x, y, COLOR_WHITE, _T("%s\n[%s]"),
				AddNewLineString(p_item_data[itemNo].detail, 21).c_str(),
				p_item_data[itemNo].condition.c_str());
		}
		else {
			DrawFormatString(x, y, COLOR_WHITE, _T("[%s]"),
				p_item_data[itemNo].condition.c_str());
		}
		return;
	}

	void DrawLeftWindow(int left, int up, int right, int down, uint cmd) {
		winpic.draw(left, up, right, down);
		this->DrawItemName(left + 30, up + 30, down - 30);
		this->cursor.draw(left + 20, up + 25 + cmd * this->list_gap, right - 20, up + 60 + cmd * this->list_gap);
	}

	void DrawDetailWindow(int left, int up, int right, int down, uint cmd) {
		winpic.draw(left, up, right, down);
		this->DrawItemDetail(left + 10, up + 20, cmd);
	}

public:
	int action(void) {
		int	cmd = 0;
		rec_helpbar_c help;

		int win_sizeX = 0;
		int win_sizeY = 0;
		GetScreenState(&win_sizeX, &win_sizeY, NULL);

		this->GetItemOpenFg();

		while (1) {
			InputAllKeyHold();
			switch (GetKeyPushOnce()) {
			case KEY_INPUT_UP:
				this->selectSnd.reserveSound();
				cmd = LOOP_SUB(cmd, 10);
				break;
			case KEY_INPUT_DOWN:
				this->selectSnd.reserveSound();
				cmd = LOOP_ADD(cmd, 10);
				break;
			case KEY_INPUT_BACK:
			case KEY_INPUT_ESCAPE:
				return 0;
			default:
				break;
			}

			if (GetWindowUserCloseFlag(TRUE)) { return 5; }

			this->cursor.update();

			ClearDrawScreen();
			RecRescaleDrawGraph(0, 0, this->backimg.handle(), TRUE);

			this->DrawLeftWindow(50, 50, win_sizeX / 2 - 25, win_sizeY - 80, cmd);
			this->DrawItemPic(790, 50, cmd);
			this->DrawDetailWindow(win_sizeX / 2 + 25, win_sizeY / 2, win_sizeX - 50, win_sizeY - 80, cmd);

			help.DrawHelp(rec_helpbar_type_ec::COLLECT_ITEM);
			ScreenFlip();

			this->selectSnd.playSound();

			WaitTimer(WAIT_TIME_ON_GAMELOOP);
		}
		return 0;
	}
};

now_scene_t Achievement(void) {
	rec_achivement_c action;
	switch (action.action()) {
	case 0:
		return SCENE_MENU;
	case 5:
		return SCENE_EXIT;
	}
}
