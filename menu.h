
#include "recr_cutin.h"
#include "RecWindowRescale.h"

#define MENU_NUM (sizeof(menu_item) / sizeof(menu_item[0]))

typedef struct rec_menu_data_s {
	DxPic_t card;
	DxPic_t ground;
	DxPic_t back;
	now_scene_t num;
} rec_menu_data_t;

static rec_menu_data_t menu_item[] = {
	{
		LoadGraph(L"picture/menu/FREE PLAY.png"),
		LoadGraph(L"picture/menu/FREE PLAY G.png"),
		LoadGraph(L"picture/backskynoamal.png"),
		SCENE_SERECT
	},
	{
		LoadGraph(L"picture/menu/COLLECTION.png"),
		LoadGraph(L"picture/menu/COLLECT G.png"),
		LoadGraph(L"picture/menu/COLLECT W.png"),
		SCENE_COLLECTION
	},
	{
		LoadGraph(L"picture/menu/OPTION.png"),
		LoadGraph(L"picture/menu/OPTION G.png"),
		LoadGraph(L"picture/menu/OPTION W.png"),
		SCENE_OPTION
	},
	{
		LoadGraph(L"picture/menu/QUIT.png"),
		LoadGraph(L"picture/menu/QUIT G.png"),
		LoadGraph(L"picture/backstar.png"),
		SCENE_EXIT
	}
};

static class rec_manu_dataset_c {
private:
	int LR = 1;
	int Ncmd = 0;
	int Bcmd = 0;
	DxTime_t stime = -250;

	void DrawBack(void) const {
		int time = GetNowCount() - this->stime;
		time = betweens(0, time, 250);
		RecRescaleDrawGraph(0, 0, menu_item[this->Bcmd].back, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(0, 0, 250, 255, time));
		RecRescaleDrawGraph(0, 0, menu_item[this->Ncmd].back, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		RecRescaleDrawGraph(0, pals(250, 240, 0, 0, time), menu_item[this->Bcmd].ground, TRUE);
		RecRescaleDrawGraph(0, pals(250, 0, 0, 240, time), menu_item[this->Ncmd].ground, TRUE);
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
			RecRescaleDrawGraph(drawX, drawY, menu_item[num].card, TRUE);
			num = (num + 1) % MENU_NUM;
		}
		return;
	}

public:
	rec_manu_dataset_c(void) {
		menu_item[0].card = LoadGraph(L"picture/menu/FREE PLAY.png");
		menu_item[0].ground = LoadGraph(L"picture/menu/FREE PLAY G.png");
		menu_item[0].back = LoadGraph(L"picture/backskynoamal.png");
		menu_item[1].card = LoadGraph(L"picture/menu/COLLECTION.png");
		menu_item[1].ground = LoadGraph(L"picture/menu/COLLECT G.png");
		menu_item[1].back = LoadGraph(L"picture/menu/COLLECT W.png");
		menu_item[2].card = LoadGraph(L"picture/menu/OPTION.png");
		menu_item[2].ground = LoadGraph(L"picture/menu/OPTION G.png");
		menu_item[2].back = LoadGraph(L"picture/menu/OPTION W.png");
		menu_item[3].card = LoadGraph(L"picture/menu/QUIT.png");
		menu_item[3].ground = LoadGraph(L"picture/menu/QUIT G.png");
		menu_item[3].back = LoadGraph(L"picture/backstar.png");
		return;
	}

	~rec_manu_dataset_c(void) {
		for (int inum = 0; inum < MENU_NUM; inum++) {
			DeleteGraph(menu_item[inum].card);
			DeleteGraph(menu_item[inum].ground);
			DeleteGraph(menu_item[inum].back);
		}
		return;
	}

	void SetLeft(void) {
		this->Bcmd = this->Ncmd;
		this->Ncmd--;
		if (this->Ncmd < 0) { this->Ncmd += MENU_NUM; }
		this->LR = -1;
		this->stime = GetNowCount();
		return;
	}
	
	void SetRight(void) {
		this->Bcmd = this->Ncmd;
		this->Ncmd++;
		if (MENU_NUM <= this->Ncmd) { this->Ncmd -= MENU_NUM; }
		this->LR = 1;
		this->stime = GetNowCount();
		return;
	}

	int GetCmd(void) const {
		return this->Ncmd;
	}

	void DrawMenu(void) const {
		this->DrawBack();
		this->DrawCard();
		return;
	}
};

now_scene_t menu(void) {
	/* 定数 */
	int command = 0; //選択したモード
	int help, goust, select;
	int key = 1;
	int	lan[7] = { 0,0,0,2,0,0,0 }; //使うのは[4,言語]だけ

	now_scene_t next = SCENE_MENU; //次のモード
	rec_manu_dataset_c menu_class;
	rec_cutin_c cutin;

	FILE *fp;
	goust = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (goust == 0) {
		fread(&lan, sizeof(lan), 6, fp);
		fclose(fp);
	}
	unsigned int Cr = GetColor(255, 255, 255);
	help = LoadGraph(L"picture/help.png");
	select = LoadSoundMem(L"sound/arrow.wav");
	cutin.SetIo(0);

	while (1) {
		ClearDrawScreen(); /* 描画エリアここから */

		menu_class.DrawMenu();

		RecRescaleDrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0) {
			RecRescaleDrawString(5, 460, L"左右キー:選択   Enterキー:決定", Cr);
		}
		else if (lan[4] == 1) {
			RecRescaleDrawString(5, 460, L"←→key:choose   Enter key:enter", Cr);
		}

		cutin.DrawCut();

		ScreenFlip(); /* 描画エリアここまで */

		//ここからキー判定
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//左が押された
			if (key == 0) {
				menu_class.SetLeft();
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//右が押された
			if (key == 0) {
				menu_class.SetRight();
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN)) {
			//エンターが押された
			if (key == 0) {
				command = menu_class.GetCmd();
				if (menu_item[command].num != SCENE_SERECT) {
					next = menu_item[command].num;
					break;
				}
				cutin.SetCutTipFg(CUTIN_TIPS_ON);
				cutin.SetTipNo();
				cutin.SetIo(1);
			}
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			//閉じるボタンが押された
			next = SCENE_EXIT;
			break;
		}
		else {
			//特定のキーが押されていない
			key = 0;
		}

		if (cutin.IsEndAnim()) {
			command = menu_class.GetCmd();
			next = menu_item[command].num;
			break;
		}

		WaitTimer(10);
	}
	return next;
}
