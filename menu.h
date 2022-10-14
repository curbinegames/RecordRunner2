#define MENU_NUM 4

typedef struct _menu_item {
	int card;
	int ground;
	int back;
	int num;
} _menu_item;

void DrawBack(_menu_item now, _menu_item before, int time);
void DrawCard(_menu_item *card, int num, int LR, int time);

int menu(void) {
	int next = 1; //次のモード
	int command = 0; //選択中のモード
	int before = 0; //前に選んでたモード
	int LR = 1;
	int starttime = -250;
	int help, goust, select;
	int key = 1;
	int	lan[7] = { 0,0,0,2,0,0,0 };//使うのは[4,言語]だけ
	_menu_item menu_item[MENU_NUM];
	FILE *fp;
	goust = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (goust == 0) {
		fread(&lan, sizeof(lan), 6, fp);
		fclose(fp);
	}
	unsigned int Cr = GetColor(255, 255, 255);
	menu_item[0] = { 
		LoadGraph(L"picture/menu/FREE PLAY.png"),
		LoadGraph(L"picture/menu/FREE PLAY G.png"),
		LoadGraph(L"picture/backskynoamal.png"),
		2
	};
	menu_item[1] = { 
		LoadGraph(L"picture/menu/COLLECTION.png"),
		LoadGraph(L"picture/menu/COLLECT G.png"),
		LoadGraph(L"picture/menu/COLLECT W.png"),
		3
	};
	menu_item[2] = {
		LoadGraph(L"picture/menu/OPTION.png"),
		LoadGraph(L"picture/menu/OPTION G.png"),
		LoadGraph(L"picture/menu/OPTION W.png"),
		4
	};
	menu_item[3] = {
		LoadGraph(L"picture/menu/QUIT.png"),
		LoadGraph(L"picture/menu/QUIT G.png"),
		LoadGraph(L"picture/backstar.png"),
		5
	};
	help = LoadGraph(L"picture/help.png");
	select = LoadSoundMem(L"sound/arrow.ogg");
	while (1) {
		ClearDrawScreen();
		//背景表示
		DrawBack(menu_item[command], menu_item[before], GetNowCount() - starttime);
		DrawCard(menu_item, command, LR, GetNowCount() - starttime);
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"左右キー:選択   Enterキー:決定", Cr);
		else if (lan[4] == 1)DrawString(5, 460, L"←→key:choose   Enter key:enter", Cr);
		ScreenFlip();
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
				next = menu_item[command].num;
				break;
			}
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			//閉じるボタンが押された
			next = 5;
			break;
		}
		//特定のキーが押されていない
		else key = 0;
		WaitTimer(5);
	}
	return next;
}

void DrawBack(_menu_item now, _menu_item before, int time) {
	time = betweens(0, time, 250);
	DrawGraph(0, 0, before.back, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(0, 0, 250, 255, time));
	DrawGraph(0, 0, now.back, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	DrawGraph(0, pals(250, 240, 0, 0, time), before.ground, TRUE);
	DrawGraph(0, pals(250, 0, 0, 240, time), now.ground, TRUE);
	return;
}

void DrawCard(_menu_item *card, int num, int LR, int time) {
	time = betweens(0, time, 250);
	num -= 2;
	while (num < 0) {
		num += MENU_NUM;
	}
	for (int i = 0; i < 5; i++) {
		DrawGraph(pals(250, 420 * i - 680, 0, 420 * i + 420 * LR - 680, time),
			pals(250, 100, -250, 0, -time * LR + 250 * i + 250 * LR - 250),
			card[num].card, TRUE);
		num++;
		if (MENU_NUM <= num) {
			num -= MENU_NUM;
		}
	}
	return;
}