
#include <DxLib.h>
#include <dxcur.h>
#include <RecSave.h>
#include <RecWindowRescale.h>
#include <collect_seclet.h>

static class rec_clct_itemview_c {
private:
	bool openFg[10] = { false,false,false,false,false,false,false,false,false,false };
	DxPic_t win = LoadGraph(L"picture/item˜g.png");
	DxPic_t cover = LoadGraph(L"picture/itemƒJƒo[.png");
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
