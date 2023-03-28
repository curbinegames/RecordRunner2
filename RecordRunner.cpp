
#include "DxLib.h"
#include "system.h"
#include "sancur.h"
#include "strcur.h"
#include "keycur.h"
#include "title.h"
#include "menu.h"
#include "musicserect2.h"
#include "play2.h"
#include "collection.h"
#include "option.h"
#include "editserect.h"
#include "serectbase.h"
#include "edit.h"
#include "recr_cutin.h"
#include "versionup.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow) {
	now_scene_t next = SCENE_TITLE;
	int now = 0, bgm, mnom[7] = { 0,1,0,1,1,0,0 };
	int G[5] = { 0,0,0,0,0 };
	unsigned int Cr = GetColor(255, 255, 255);
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nCmdShow;
	ChangeWindowMode(TRUE);
	SetAlwaysRunFlag(TRUE);
	SetWindowUserCloseEnableFlag(FALSE);
	SetMainWindowText(L"Record Runner");
	SetWindowSizeChangeEnableFlag(TRUE);
	if (DxLib_Init() == -1) { return -1; }
	SetDrawScreen(DX_SCREEN_BACK);
	bgm = LoadSoundMem(L"song/no.mp3");
	upgrade_rate_f(); // レートのセーブデータ更新(Ver.1.04 -> Ver.1.05)
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
		case SCENE_TITLE:
			next = title();
			break;
		case SCENE_MENU:
			next = menu();
			break;
		case SCENE_SERECT:
			next = musicserect2(&G[0]);
			break;
		case SCENE_COLLECTION:
			next = collection();
			break;
		case SCENE_OPTION:
			next = option();
			break;
		case SCENE_MUSIC:
			next = play3(G[0], G[1], G[2], G[3], G[4]);
			break;
#if 0
		case 7:
			next = editserect(&G[0]);
			break;
		case 8:
			next = edit(G[0], G[1], G[2], G[3]);
			break;
#endif
		default:
#if 0
			ClearDrawScreen();
			DrawString(200, 200, L"error:001\n予期されない数値", Cr);
			ScreenFlip();
			WaitTimer(100);
			WaitKey();
#endif
			next = SCENE_EXIT;
			break;
		}
	}
	DxLib_End();
	return 0;
}
