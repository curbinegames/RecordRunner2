
#include "DxLib.h"
#include "sancur.h"
#include "strcur.h"
#include "keycur.h"
#include "title.h"
#include "menu.h"
#include "musicserect.h"
#include "musicserect2.h"
#include "play2.h"
#include "collection.h"
#include "option.h"
#include "editserect.h"
#include "serectbase.h"
#include "edit.h"
#include "recr_cutin.h"

/* next = 0 = タイトル
	= 1 = メニュー
	= 2 = 曲選択画面
	= 3 = コレクション画面
	= 4 = オプション画面
	= 6 = 演奏画面
	= 5 = 終了処理*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	ChangeWindowMode(TRUE);
	SetAlwaysRunFlag(TRUE);
	SetWindowUserCloseEnableFlag(FALSE);
	SetMainWindowText(L"Record Runner");
	SetWindowSizeChangeEnableFlag(TRUE);
	if (DxLib_Init() == -1)return -1;
	SetDrawScreen(DX_SCREEN_BACK);
	int next = 0, now = 0, bgm, mnom[7] = { 0,1,0,1,1,0,0 };
	int G[5];
	unsigned int Cr = GetColor(255, 255, 255);
	bgm = LoadSoundMem(L"song/no.mp3");
	//ゲーム終了(nextが5のとき)までの間、処理を繰り返す
	while (next != 5) {
		InitGraph();
		if (mnom[next] != now) {
			StopSoundMem(bgm);
			InitSoundMem();
			now = mnom[next];
			switch (mnom[next]) {
			case 0:
				bgm = LoadSoundMem(L"song/no.mp3");
				break;
			case 1:
				bgm = LoadSoundMem(L"song/Let’s_be_happy.mp3");
				break;
			}
			PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);
		}
		//表示する画面を選択する
		now = mnom[next];
		switch (next) {
		case 0:
			next = title();
			break;
		case 1:
			next = menu();
			break;
		case 2:
			next = musicserect2(&G[0]);
			break;
		case 3:
			next = collection();
			break;
		case 4:
			next = option();
			break;
		case 6:
			next = play3(G[0], G[1], G[2], G[3], G[4]);
			break;
		case 7:
			next = editserect(&G[0]);
			break;
		case 8:
			next = edit(G[0], G[1], G[2], G[3]);
			break;
		default:
			ClearDrawScreen();
			DrawString(200, 200, L"error:001\n予期されない数値", Cr);
			ScreenFlip();
			next = 5;
			WaitTimer(100);
			WaitKey();
			break;
		}
	}
	DxLib_End();
	return 0;
}
