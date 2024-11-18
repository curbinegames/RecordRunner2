
#include "recr_cutin.h"
#include "RecWindowRescale.h"

#define MENU_NUM sizeof(menu_item) / sizeof(menu_item[0])

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

void DrawBack(int now, int before, int time);
void DrawCard(int num, int LR, int time);

void RecMenuInitPic(void) {
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

now_scene_t menu(void) {
	/* 定数 */
	int command = 0; //選択中のモード
	int before = 0; //前に選んでたモード
	int LR = 1;
	int starttime = -250;
	int help, goust, select;
	int key = 1;
	int	lan[7] = { 0,0,0,2,0,0,0 }; //使うのは[4,言語]だけ

	now_scene_t next = SCENE_MENU; //次のモード
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
	RecMenuInitPic();
	cutin.SetIo(0);

	while (1) {
		ClearDrawScreen(); /* 描画エリアここから */

		DrawBack(command, before, GetNowCount() - starttime);
		DrawCard(command, LR, GetNowCount() - starttime);
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
				before = command;
				command -= 1;
				if (command < 0) command += MENU_NUM;
				LR = -1;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				starttime = GetNowCount();
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//右が押された
			if (key == 0) {
				before = command;
				command += 1;
				if (MENU_NUM <= command) command -= MENU_NUM;
				LR = 1;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				starttime = GetNowCount();
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN)) {
			//エンターが押された
			if (key == 0) {
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
			next = menu_item[command].num;
			break;
		}

		WaitTimer(10);
	}
	return next;
}

void DrawBack(int now, int before, int time) {
	time = betweens(0, time, 250);
	RecRescaleDrawGraph(0, 0, menu_item[before].back, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(0, 0, 250, 255, time));
	RecRescaleDrawGraph(0, 0, menu_item[now].back, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	RecRescaleDrawGraph(0, pals(250, 240, 0, 0, time), menu_item[before].ground, TRUE);
	RecRescaleDrawGraph(0, pals(250, 0, 0, 240, time), menu_item[now].ground, TRUE);
	return;
}

void DrawCard(int num, int LR, int time) {
	time = betweens(0, time, 250);
	num -= 2;
	while (num < 0) {
		num += MENU_NUM;
	}
	for (int i = 0; i < 5; i++) {
		RecRescaleDrawGraph(pals(250, 420 * i - 680, 0, 420 * i + 420 * LR - 680, time),
			pals(250, 100, -250, 0, -time * LR + 250 * i + 250 * LR - 250),
			menu_item[num].card, TRUE);
		num++;
		if (MENU_NUM <= num) {
			num -= MENU_NUM;
		}
	}
	return;
}
