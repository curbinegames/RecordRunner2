
#include "DxLib.h"
#include "system.h"
#include "general/sancur.h"
#include "general/strcur.h"
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
#include "RecWindowRescale.h"

#define DX_MAIN_DEF HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow

#define TOOL_NAME L"Record Runner Florting Style" // ツールの名前

static void GameMain() {
	int now = 0, bgm, mnom[7] = { 0,1,0,1,1,0,0 };
	int G[5] = { 0,0,0,0,0 };
	unsigned int Cr = GetColor(255, 255, 255);
	now_scene_t next = SCENE_TITLE;
	FILE *fp;
	bgm = LoadSoundMem(L"song/no.mp3");
	INIT_MAT();
	upgrade_rate_f(); // レートのセーブデータ更新(Ver.1.04 -> Ver.1.05)
	fix10501to10502(); // セーブデータバグのfix(Ver.1.05.1 -> Ver.1.05.2)
	//ゲーム終了(nextが5のとき)までの間、処理を繰り返す
	while (next != 5) {
		INIT_PIC();
		if (mnom[next] != now) {
			StopSoundMem(bgm);
			INIT_SND();
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
			RecRescaleDrawString(200, 200, L"error:001\n予期されない数値", Cr);
			ScreenFlip();
			WaitTimer(100);
			WaitKey();
#endif
			next = SCENE_EXIT;
			break;
		}
	}
}

/* WinMain関数はもう編集する必要なし */
int WINAPI WinMain(DX_MAIN_DEF) {
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nCmdShow;
	ChangeWindowMode(TRUE); // ウィンドウモードにする
	SetGraphMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 32); // ウィンドウサイズの変更
	SetAlwaysRunFlag(TRUE); // 非アクティブでも動くようにする
	SetWindowUserCloseEnableFlag(FALSE); // ×ボタンで勝手に閉じないようにする
	SetMainWindowText(TOOL_NAME); // ウィンドウの名前
	SetWindowSizeChangeEnableFlag(TRUE); // ウィンドウサイズを変えれるようにする
	SetFontSize(lins(OLD_WINDOW_SIZE_Y, 16, 720, 24, WINDOW_SIZE_Y)); //文字の大きさ
	if (DxLib_Init() == -1) { return -1; } // エラーで中断
	SetDrawScreen(DX_SCREEN_BACK); // 作画モード変更
	RecOpenOptionFileSystem(); // レコランのオプション読み込み
	SmallFontData = CreateFontToHandle(NULL, -1, -1);
	LargeFontData = CreateFontToHandle(NULL, lins(OLD_WINDOW_SIZE_Y, 16, 720, 24, WINDOW_SIZE_Y), -1);
	GameMain(); // ゲーム処理
	DxLib_End(); // DxLib終わり
	return 0; // プログラム終わり
}
