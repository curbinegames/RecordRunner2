
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <dxcur.h>
#include <dxdraw.h>
#include <sancur.h>

/* rec system include */
#include <option.h>
#include <RecWindowRescale.h>
#include <RecSystem.h>

typedef struct rec_menu_data_s {
	dxcur_pic_c card;
	dxcur_pic_c ground;
	dxcur_pic_c back;
	now_scene_t num;
} rec_menu_data_t;

static class rec_manu_dataset_c {
private:
	int LR = 1;
	int Ncmd = 0;
	int Bcmd = 0;
	DxTime_t stime = -250;
	DxSnd_t select = LoadSoundMem(L"sound/arrow.wav");

	rec_menu_data_t menu_item[4] = {
		{
			dxcur_pic_c(L"picture/menu/FREE PLAY.png"),
			dxcur_pic_c(L"picture/menu/FREE PLAY G.png"),
			dxcur_pic_c(L"picture/backskynoamal.png"),
			SCENE_SERECT
		},
		{
			dxcur_pic_c(L"picture/menu/COLLECTION.png"),
			dxcur_pic_c(L"picture/menu/COLLECT G.png"),
			dxcur_pic_c(L"picture/menu/COLLECT W.png"),
			SCENE_COLLECTION
		},
		{
			dxcur_pic_c(L"picture/menu/OPTION.png"),
			dxcur_pic_c(L"picture/menu/OPTION G.png"),
			dxcur_pic_c(L"picture/menu/OPTION W.png"),
			SCENE_OPTION
		},
		{
			dxcur_pic_c(L"picture/menu/QUIT.png"),
			dxcur_pic_c(L"picture/menu/QUIT G.png"),
			dxcur_pic_c(L"picture/backstar.png"),
			SCENE_EXIT
		}
	};

#define MENU_NUM (sizeof(this->menu_item) / sizeof(this->menu_item[0]))

public:
	rec_manu_dataset_c(void) {
		ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, select);
		return;
	}

	~rec_manu_dataset_c(void) {
		for (int inum = 0; inum < MENU_NUM; inum++) {
			DeleteGraph(this->menu_item[inum].card.handle());
			DeleteGraph(this->menu_item[inum].ground.handle());
			DeleteGraph(this->menu_item[inum].back.handle());
		}
		return;
	}

private:
	void DrawBack(void) const {
		int time = GetNowCount() - this->stime;
		time = betweens(0, time, 250);
		RecRescaleDrawGraph(0, 0, this->menu_item[this->Bcmd].back.handle(), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(0, 0, 250, 255, time));
		RecRescaleDrawGraph(0, 0, this->menu_item[this->Ncmd].back.handle(), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		DrawGraphAnchor(0, pals(250, WINDOW_SIZE_Y / 2, 0, 0, time), this->menu_item[this->Bcmd].ground.handle(), DXDRAW_ANCHOR_BOTTOM_CENTRE);
		DrawGraphAnchor(0, pals(250, 0, 0, WINDOW_SIZE_Y / 2, time), this->menu_item[this->Ncmd].ground.handle(), DXDRAW_ANCHOR_BOTTOM_CENTRE);
		return;
	}

	void DrawCard(void) const {
		int time = GetNowCount() - this->stime;
		int num = this->Ncmd;
		time = betweens(0, time, 250);
		num -= 2;
		while (num < 0) {
			num += MENU_NUM;
		}
		for (int i = 0; i < 5; i++) {
			int drawX = pals(250, 420 * i - 680, 0, 420 * i + 420 * this->LR - 680, time);
			int drawY = pals(250, 100, -250, 0, -time * this->LR + 250 * i + 250 * this->LR - 250);
			RecRescaleDrawGraph(drawX, drawY, this->menu_item[num].card.handle(), TRUE);
			num = (num + 1) % MENU_NUM;
		}
		return;
	}

public:
	void SetLeft(void) {
		this->Bcmd = this->Ncmd;
		this->Ncmd = (this->Ncmd + MENU_NUM - 1) % MENU_NUM;
		this->LR = -1;
		this->stime = GetNowCount();
		PlaySoundMem(this->select, DX_PLAYTYPE_BACK);
		return;
	}
	
	void SetRight(void) {
		this->Bcmd = this->Ncmd;
		this->Ncmd = (this->Ncmd + 1) % MENU_NUM;
		this->LR = 1;
		this->stime = GetNowCount();
		PlaySoundMem(this->select, DX_PLAYTYPE_BACK);
		return;
	}

	now_scene_t GetSerectScene(void) const {
		return this->menu_item[this->Ncmd].num;
	}

	void DrawMenu(void) const {
		this->DrawBack();
		this->DrawCard();
		return;
	}

#undef MENU_NUM

};

now_scene_t menu(void) {
	rec_manu_dataset_c menu_class;
	rec_helpbar_c help;
	rec_cutin_c cutin;

	cutin.SetIo(0);
	AvoidKeyRush();

	while (1) {
		ClearDrawScreen(); /* 描画エリアここから */

		menu_class.DrawMenu();
		help.DrawHelp(HELP_MAT_MENU);
		cutin.DrawCut();

		ScreenFlip(); /* 描画エリアここまで */

		/* キー入力 */
		if (cutin.IsClosing() == 0) {
			InputAllKeyHold();
			switch (GetKeyPushOnce()) {
			case KEY_INPUT_LEFT:
				menu_class.SetLeft();
				break;
			case KEY_INPUT_RIGHT:
				menu_class.SetRight();
				break;
			case KEY_INPUT_RETURN:
				if (menu_class.GetSerectScene() != SCENE_SERECT) { return menu_class.GetSerectScene(); }
				cutin.SetCutTipFg(CUTIN_TIPS_ON);
				cutin.SetTipNo();
				cutin.SetIo(1);
				break;
			default:
				break;
			}
		}

		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }
		if (cutin.IsEndAnim()) { return menu_class.GetSerectScene(); }

		WaitTimer(WAIT_TIME_ON_GAMELOOP);
	}
}
