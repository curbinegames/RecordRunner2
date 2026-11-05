
#include <DxLib.h>
#include <dxcur.h>
#include <RecSave.h>
#include <RecWindowRescale.h>
#include <collect_seclet.h>

static class rec_clct_itemview_c {
private:
	bool openFg[10] = { false,false,false,false,false,false,false,false,false,false };
	dxcur_pic_c win{_T("picture/item˜g.png")};
	dxcur_pic_c cover{_T("picture/itemƒJƒo[.png")};
	dxcur_pic_c backimg{_T("picture/COLLECT back.png")};
	dxcur_pic_c null_item{_T("picture/item0.png")};
	dxcur_pic_c item[10] = {
		dxcur_pic_c{_T("picture/item1.png")},
		dxcur_pic_c{_T("picture/item2.png")},
		dxcur_pic_c{_T("picture/item3.png")},
		dxcur_pic_c{_T("picture/item4.png")},
		dxcur_pic_c{_T("picture/item5.png")},
		dxcur_pic_c{_T("picture/item6.png")},
		dxcur_pic_c{_T("picture/item7.png")},
		dxcur_pic_c{_T("picture/item8.png")},
		dxcur_pic_c{_T("picture/item9.png")},
		dxcur_pic_c{_T("picture/item10.png")}
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
	void DrawItemPic(uint DrawX, int DrawY, uint cmd) {
		RecRescaleDrawGraph(DrawX, DrawY, this->win.handle(), TRUE);
		if (this->openFg[cmd]) {
			RecRescaleDrawGraph(DrawX + 3, DrawY + 3, this->item[cmd].handle(), TRUE);
		}
		else {
			RecRescaleDrawGraph(DrawX + 3, DrawY + 3, this->null_item.handle(), TRUE);
		}
		return;
	}

	void RecClctDrawItemDetail(uint cmd) {
		const rec_collect_item_set_t *const p_item_data = (optiondata.lang == 0) ? item_data_jp : item_data_en;
		uint itemNo = cmd;
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
		int	cmd = 0;
		rec_helpbar_c help;
		dxcur_window_pic_c winpic{_T("picture/achievement/window.png")};

		while (1) {
			InputAllKeyHold();
			switch (GetKeyPushOnce()) {
			case KEY_INPUT_UP:
				PlaySoundMem(this->selectSnd, DX_PLAYTYPE_NORMAL);
				cmd = LOOP_SUB(cmd, 10);
				break;
			case KEY_INPUT_DOWN:
				PlaySoundMem(this->selectSnd, DX_PLAYTYPE_NORMAL);
				cmd = LOOP_ADD(cmd, 10);
				break;
			case KEY_INPUT_BACK:
				return 0;
			default:
				break;
			}

			if (GetWindowUserCloseFlag(TRUE)) { return 5; }

			ClearDrawScreen();
			RecRescaleDrawGraph(0, 0, this->backimg.handle(), TRUE);
			winpic.draw(640, 50, 1230, 640);

			this->DrawItemPic(100, 100, cmd);
			this->RecClctDrawItemDetail(cmd);

			help.DrawHelp(rec_helpbar_type_ec::COLLECT_ITEM);
			ScreenFlip();

			WaitTimer(WAIT_TIME_ON_GAMELOOP);
		}
		return 0;
	}
};

now_scene_t Achievement(void) {
	rec_clct_itemview_c action;
	switch (action.C_item()) {
	case 0:
		return SCENE_MENU;
	case 5:
		return SCENE_EXIT;
	}
}
