
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

#define TOOL_NAME L"Record Runner Florting Style" // �c�[���̖��O

static void GameMain() {
	now_scene_t next = SCENE_TITLE;
	rec_to_play_set_t ps;
	INIT_MAT();
	RecOpenOptionFile2(&optiondata);
	upgrade_rate_f(); // ���[�g�̃Z�[�u�f�[�^�X�V(Ver.1.04 -> Ver.1.05)
	fix10501to10502(); // �Z�[�u�f�[�^�o�O��fix(Ver.1.05.1 -> Ver.1.05.2)
	//�Q�[���I��(next��5�̂Ƃ�)�܂ł̊ԁA�������J��Ԃ�
	while (next != 5) {
		INIT_PIC();
		//�\�������ʂ�I������
		switch (next) {
		case SCENE_TITLE:
			RecSysBgmDelete();
			next = title();
			break;
		case SCENE_MENU:
			RecSysBgmSetMem(_T("song/Let�fs_be_happy.mp3"), sizeof(_T("song/Let�fs_be_happy.mp3")));
			RecSysBgmPlay(false);
			next = menu();
			break;
		case SCENE_SERECT:
			RecSysBgmDelete();
			next = musicserect(&ps);
			break;
		case SCENE_COLLECTION:
			RecSysBgmSetMem(_T("song/Let�fs_be_happy.mp3"), sizeof(_T("song/Let�fs_be_happy.mp3")));
			RecSysBgmPlay(false);
			next = collection();
			break;
		case SCENE_OPTION:
			RecSysBgmSetMem(_T("song/Let�fs_be_happy.mp3"), sizeof(_T("song/Let�fs_be_happy.mp3")));
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

/* WinMain�֐��͂����ҏW����K�v�Ȃ� */
int WINAPI WinMain(DX_MAIN_DEF) {
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nCmdShow;
	ChangeWindowMode(TRUE); // �E�B���h�E���[�h�ɂ���
	SetGraphMode(WINDOW_SIZE_X, WINDOW_SIZE_Y, 32); // �E�B���h�E�T�C�Y�̕ύX
	SetAlwaysRunFlag(TRUE); // ��A�N�e�B�u�ł������悤�ɂ���
	SetWindowUserCloseEnableFlag(FALSE); // �~�{�^���ŏ���ɕ��Ȃ��悤�ɂ���
	SetMainWindowText(TOOL_NAME); // �E�B���h�E�̖��O
	SetWindowSizeChangeEnableFlag(TRUE); // �E�B���h�E�T�C�Y��ς����悤�ɂ���
	SetFontSize(lins(OLD_WINDOW_SIZE_Y, 16, 720, 24, WINDOW_SIZE_Y)); //�����̑傫��
	if (DxLib_Init() == -1) { return -1; } // �G���[�Œ��f
	SetDrawScreen(DX_SCREEN_BACK); // ��惂�[�h�ύX
	RecOpenOptionFileSystem(); // ���R�����̃I�v�V�����ǂݍ���
	SmallFontData = CreateFontToHandle(NULL, -1, -1);
	LargeFontData = CreateFontToHandle(NULL, lins(OLD_WINDOW_SIZE_Y, 16, 720, 24, WINDOW_SIZE_Y), -1);
	GameMain(); // �Q�[������
	DxLib_End(); // DxLib�I���
	return 0; // �v���O�����I���
}
