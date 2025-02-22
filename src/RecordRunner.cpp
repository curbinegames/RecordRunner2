
/* base include */
#include <DxLib.h>
#include <sancur.h>

/* rec system include */
#include <system.h>
#include <RecSystem.h>

/* rec sub include */
#include <versionup.h>
#include <title.h>
#include <menu.h>
#include <musicserect2.h>
#include <play2.h>
#include <RecCollection.h>
#include <option.h>

#define DX_MAIN_DEF HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow

#define TOOL_NAME L"Record Runner Florting Style" // ツールの名前

static void GameMain() {
	now_scene_t next = SCENE_TITLE;
	rec_to_play_set_t ps;
	INIT_MAT();
	RecOpenOptionFile2(&optiondata);
	upgrade_rate_f(); // レートのセーブデータ更新(Ver.1.04 -> Ver.1.05)
	fix10501to10502(); // セーブデータバグのfix(Ver.1.05.1 -> Ver.1.05.2)
	//ゲーム終了(nextが5のとき)までの間、処理を繰り返す
	while (next != 5) {
		INIT_PIC();
		//表示する画面を選択する
		switch (next) {
		case SCENE_TITLE:
			RecSysBgmDelete();
			next = title();
			break;
		case SCENE_MENU:
			RecSysBgmSetMem(_T("song/Let’s_be_happy.mp3"), sizeof(_T("song/Let’s_be_happy.mp3")));
			RecSysBgmPlay(false);
			next = menu();
			break;
		case SCENE_SERECT:
			RecSysBgmDelete();
			next = musicserect(&ps);
			break;
		case SCENE_COLLECTION:
			RecSysBgmSetMem(_T("song/Let’s_be_happy.mp3"), sizeof(_T("song/Let’s_be_happy.mp3")));
			RecSysBgmPlay(false);
			next = collection();
			break;
		case SCENE_OPTION:
			RecSysBgmSetMem(_T("song/Let’s_be_happy.mp3"), sizeof(_T("song/Let’s_be_happy.mp3")));
			RecSysBgmPlay(false);
			next = option();
			break;
		case SCENE_MUSIC:
			RecSysBgmDelete();
			next = play3(ps.packNo, ps.musicNo, ps.dif, ps.shift, ps.autoFg);
			break;
		default:
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
