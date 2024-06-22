#include "recr_cutin.h"
#include "RecWindowRescale.h"

#define MENU_NUM 4

typedef struct _menu_item {
	int card;
	int ground;
	int back;
	now_scene_t num;
} _menu_item;

void DrawBack(_menu_item now, _menu_item before, int time);
void DrawCard(_menu_item *card, int num, int LR, int time);

now_scene_t menu(void) {
	char closeFg = 0;
	int command = 0; //�I�𒆂̃��[�h
	int before = 0; //�O�ɑI��ł����[�h
	int LR = 1;
	int starttime = -250;
	int help, goust, select;
	int key = 1;
	int	lan[7] = { 0,0,0,2,0,0,0 }; //�g���̂�[4,����]����
	int CutTime = 0;
	now_scene_t next = SCENE_MENU; //���̃��[�h
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
		SCENE_SERECT
	};
	menu_item[1] = { 
		LoadGraph(L"picture/menu/COLLECTION.png"),
		LoadGraph(L"picture/menu/COLLECT G.png"),
		LoadGraph(L"picture/menu/COLLECT W.png"),
		SCENE_COLLECTION
	};
	menu_item[2] = {
		LoadGraph(L"picture/menu/OPTION.png"),
		LoadGraph(L"picture/menu/OPTION G.png"),
		LoadGraph(L"picture/menu/OPTION W.png"),
		SCENE_OPTION
	};
	menu_item[3] = {
		LoadGraph(L"picture/menu/QUIT.png"),
		LoadGraph(L"picture/menu/QUIT G.png"),
		LoadGraph(L"picture/backstar.png"),
		SCENE_EXIT
	};
	help = LoadGraph(L"picture/help.png");
	select = LoadSoundMem(L"sound/arrow.wav");
	CutTime = GetNowCount();
	while (1) {
		ClearDrawScreen();
		//�w�i�\��
		DrawBack(menu_item[command], menu_item[before], GetNowCount() - starttime);
		DrawCard(menu_item, command, LR, GetNowCount() - starttime);
		RecRescaleDrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0) {
			RecRescaleDrawString(5, 460, L"���E�L�[:�I��   Enter�L�[:����", Cr);
		}
		else if (lan[4] == 1) {
			RecRescaleDrawString(5, 460, L"����key:choose   Enter key:enter", Cr);
		}
		if (closeFg == 0) {
			ViewCutOut(CutTime);
		}
		if (closeFg == 1) {
			ViewCutIn(CutTime);
		}
		ScreenFlip();
		//��������L�[����
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//���������ꂽ
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
			//�E�������ꂽ
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
			//�G���^�[�������ꂽ
			if (key == 0) {
				if (menu_item[command].num != SCENE_SERECT) {
					next = menu_item[command].num;
					break;
				}
				SetCutTipFg(CUTIN_TIPS_ON);
				SetTipNo();
				closeFg = 1;
				CutTime = GetNowCount();
			}
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			//����{�^���������ꂽ
			next = SCENE_EXIT;
			break;
		}
		//����̃L�[��������Ă��Ȃ�
		else key = 0;
		if (closeFg == 1 && CutTime + 2000 <= GetNowCount()) {
			next = menu_item[command].num;
			break;
		}
		WaitTimer(10);
	}
	return next;
}

void DrawBack(_menu_item now, _menu_item before, int time) {
	time = betweens(0, time, 250);
	RecRescaleDrawGraph(0, 0, before.back, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(0, 0, 250, 255, time));
	RecRescaleDrawGraph(0, 0, now.back, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	RecRescaleDrawGraph(0, pals(250, 240, 0, 0, time), before.ground, TRUE);
	RecRescaleDrawGraph(0, pals(250, 0, 0, 240, time), now.ground, TRUE);
	return;
}

void DrawCard(_menu_item *card, int num, int LR, int time) {
	time = betweens(0, time, 250);
	num -= 2;
	while (num < 0) {
		num += MENU_NUM;
	}
	for (int i = 0; i < 5; i++) {
		RecRescaleDrawGraph(pals(250, 420 * i - 680, 0, 420 * i + 420 * LR - 680, time),
			pals(250, 100, -250, 0, -time * LR + 250 * i + 250 * LR - 250),
			card[num].card, TRUE);
		num++;
		if (MENU_NUM <= num) {
			num -= MENU_NUM;
		}
	}
	return;
}