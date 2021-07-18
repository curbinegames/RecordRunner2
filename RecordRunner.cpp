#include "DxLib.h"
#include "sancur.h"
#include "strcur.h"
#include "title.h"
#include "menu.h"
#include "musicserect.h"
#include "play.h"
#include "collection.h"
#include "option.h"

/* next = 0 = �^�C�g��
	= 1 = ���j���[
	= 2 = �ȑI�����
	= 3 = �v���C���
	= 99 = �I������*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	ChangeWindowMode(TRUE);
	SetAlwaysRunFlag(TRUE);
	SetWindowUserCloseEnableFlag(FALSE);
	SetMainWindowText(L"Record Runner");
	SetWindowSizeChangeEnableFlag(TRUE);
	if (DxLib_Init() == -1)return -1;
	SetDrawScreen(DX_SCREEN_BACK);
	int next = 0, now = 0, bgm, mnom[7] = { 0,1,0,1,1,0,0 };
	int G[3];
	unsigned int Cr = GetColor(255, 255, 255);
	bgm = LoadSoundMem(L"song/no.mp3");
	//�Q�[���I��(next��5�̂Ƃ�)�܂ł̊ԁA�������J��Ԃ�
	while (next != 5) {
		InitGraph();
		if (mnom[next] != now) {
			StopSoundMem(bgm);
			InitSoundMem;
			now = mnom[next];
			switch (mnom[next]) {
			case 0:
				bgm = LoadSoundMem(L"song/no.mp3");
				break;
			case 1:
				bgm = LoadSoundMem(L"song/Let�fs_be_happy.mp3");
				break;
			}
			PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);
		}
		//�\�������ʂ�I������
		now = mnom[next];
		switch (next) {
		case(0):
			next = title();
			break;
		case(1):
			next = menu();
			break;
		case(2):
			next = musicserect(&G[0]);
			break;
		case(3):
			next = collection();
			break;
		case(4):
			next = option();
			break;
		case(6):
			next = play2(G[0], G[1], G[2]);
			break;
		default:
			ClearDrawScreen();
			DrawString(200, 200, L"error:001\n�\������Ȃ����l", Cr);
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
