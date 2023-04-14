#include "recr_cutin.h"

#define EFF_TIME_1 750
#define EFF_TIME_2 EFF_TIME_1 + 200

void ShowTitle1(const int *pic, const int time);
void ShowTitle2(const int back, const int string, const int white, const int time);

now_scene_t title(void) {
	now_scene_t next = SCENE_MENU;
	char closeFg = 0;
	int alpha[2] = { 255,-1 };
	int StartTime = -1;
	int TitleChar[12] = {
		LoadGraph(L"picture/Title-1.png"),
		LoadGraph(L"picture/Title-2.png"),
		LoadGraph(L"picture/Title-3.png"),
		LoadGraph(L"picture/Title-4.png"),
		LoadGraph(L"picture/Title-5.png"),
		LoadGraph(L"picture/Title-6.png"),
		LoadGraph(L"picture/Title-7.png"),
		LoadGraph(L"picture/Title-8.png"),
		LoadGraph(L"picture/Title-9.png"),
		LoadGraph(L"picture/Title-10.png"),
		LoadGraph(L"picture/Title-11.png"),
		LoadGraph(L"picture/Title-12.png")
	};
	int Title = LoadGraph(L"picture/TitleMain.png");
	int Push = LoadGraph(L"picture/pushkey.png");
	int white = LoadGraph(L"picture/White.png");
	int CutTime = 0;
	StartTime = GetNowCount();
	CutTime = StartTime;
	while (1) {
		ClearDrawScreen();
		//タイトル画面の始まりを表示する
		if (GetNowCount() - StartTime < EFF_TIME_1) {
			ShowTitle1(TitleChar, GetNowCount() - StartTime);
		}
		else if (EFF_TIME_1 <= GetNowCount() - StartTime) {
			ShowTitle2(Title, Push, white, GetNowCount() - StartTime);
		}
		if (closeFg == 1) {
			ViewCutIn(CutTime);
		}
		ScreenFlip();
		if (CheckHitKeyAll() && 1000 <= GetNowCount() - StartTime) {
			SetCutTipFg(CUTIN_TIPS_NONE);
			closeFg = 1;
			CutTime = GetNowCount();
		}
		if (closeFg == 1 && CutTime + 2000 <= GetNowCount()) {
			InitSoundMem();
			InitPic();
			break;
		}
		if (GetWindowUserCloseFlag(TRUE)) {
			next = SCENE_EXIT;
			break;
		}
		WaitTimer(10);
	}
	ClearDrawScreen();
	return next;
}

void ShowTitle1(const int *pic, const int time) {
	int posX[12] = { 29,133,204,277,352,422,130,224,288,353,422,480 };
	int posY[2] = { 37,120 };
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 6; j++) {
			DrawGraph(
				pals(EFF_TIME_1 * (i * 6 + j + 3) / 14,
					posX[i * 6 + j],
					EFF_TIME_1 * (i * 6 + j) / 14,
					posX[i * 6 + j] + 640, maxs(time, EFF_TIME_1 * (i * 6 + j + 3) / 14)),
				posY[i], pic[i * 6 + j], TRUE);
		}
	}
	return;
}

void ShowTitle2(const int back, const int string, const int white, const int time) {
	DrawGraph(0, 0, back, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(-1, 0, 1, 255, sinC((GetNowCount() / 4) % 360)));
	DrawGraph(0, 0, string, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	if (EFF_TIME_1 <= time && time < EFF_TIME_2) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(EFF_TIME_1, 255, EFF_TIME_2, 0, time));
		DrawGraph(0, 0, white, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	return;
}
