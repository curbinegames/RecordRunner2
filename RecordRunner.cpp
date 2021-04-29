#include "DxLib.h"
#include "sancur.h"
#include "strcats.h"
#include "strmods.h"
#include "strsans.h"
#include "strands.h"
#include "strcopy.h"
#include "strcur.h"
#include "abss.h"
#include "max.h"
#include "min.h"
#include "notzero.h"
#include "pals.h"
#include "playing.h"
/* next = 0 = �^�C�g��
	= 1 = ���j���[
	= 2 = �ȑI�����
	= 3 = �v���C���
	= 99 = �I������*/
int title(void);
int menu(void);
int musicserect(int*);
int play2(int, int);
int collection(void);
int C_item(void);
int C_story(void);
int story(int, int);
int option(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	ChangeWindowMode(TRUE);
	SetAlwaysRunFlag(TRUE);
	SetWindowUserCloseEnableFlag(FALSE);
	SetMainWindowText(L"Record Runner");
	SetWindowSizeChangeEnableFlag(TRUE);
	if (DxLib_Init() == -1)return -1;
	SetDrawScreen(DX_SCREEN_BACK);
	int next = 0, now = -1, bgm[2], mnom[7] = { 0,1,0,1,1,0,0 };
	int G[2];
	unsigned int Cr;
	Cr = GetColor(255, 255, 255);
	bgm[0] = LoadSoundMem(L"song/no.mp3");
	bgm[1] = LoadSoundMem(L"song/Let�fs_be_happy.mp3");
	//�Q�[���I��(next��5�̂Ƃ�)�܂ł̊ԁA�������J��Ԃ�
	while (next != 5) {
		InitGraph();
		if (mnom[next] != now) {
			StopSoundMem(bgm[now]);
			InitSoundMem;
			bgm[0] = LoadSoundMem(L"song/no.mp3");
			bgm[1] = LoadSoundMem(L"song/Let�fs_be_happy.mp3");
			PlaySoundMem(bgm[mnom[next]], DX_PLAYTYPE_LOOP);
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
			next = play2(G[0], G[1]);
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

int title(void) {
	int next = 1;
	//�^�C�g����ʂ�\������
	LoadGraphScreen(0, 0, L"picture/title.png", TRUE);
	LoadGraphScreen(0, 0, L"picture/pushkey.png", TRUE);
	ScreenFlip();
	while (1) {
		if (CheckHitKeyAll()) {
			WaitTimer(500);
			break;
		}
		if (GetWindowUserCloseFlag(TRUE)) {
			next = 5;
			break;
		}
	}
	ClearDrawScreen();
	return next;
}

int menu(void) {
	/*
	next = ���̃��[�h
	command = �I�𒆂̃��[�h
	*/
	int next = 1;
	int command = 1;
	int LR = 1;
	int starttime = -250;
	int movetime = 250;
	int menu[4], menuG[4], menuW[4], help, goust, goust2, i, select;
	int key = 1;
	int	lan[5] = { 0,0,0,2,0 };//�g���̂�[4,����]����
	FILE *fp;
	goust = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (goust == 0) {
		fread(&lan, sizeof(lan), 5, fp);
		fclose(fp);
	}
	unsigned int Cr;
	Cr = GetColor(255, 255, 255);
	menu[0] = LoadGraph(L"picture/FREE PLAY.png");
	menu[1] = LoadGraph(L"picture/COLLECTION.png");
	menu[2] = LoadGraph(L"picture/OPTION.png");
	menu[3] = LoadGraph(L"picture/QUIT.png");
	menuG[0] = LoadGraph(L"picture/FREE PLAY G.png");
	menuG[1] = LoadGraph(L"picture/COLLECT G.png");
	menuG[2] = LoadGraph(L"picture/OPTION G.png");
	menuG[3] = LoadGraph(L"picture/QUIT G.png");
	menuW[0] = LoadGraph(L"picture/backskynoamal.png");
	menuW[1] = LoadGraph(L"picture/COLLECT W.png");
	menuW[2] = LoadGraph(L"picture/OPTION W.png");
	menuW[3] = LoadGraph(L"picture/backstar.png");
	help = LoadGraph(L"picture/help.png");
	select = LoadSoundMem(L"sound/arrow.ogg");
	while (next == 1) {
		ClearDrawScreen();
		//���Ԍv�Z
		movetime = GetNowCount() - starttime;
		movetime = maxs(movetime, 250);
		//�Ǖ\��
		goust = command - LR - 1;
		while (goust < 0) goust += 4;
		while (goust > 3) goust -= 4;
		goust2 = (movetime - 250)*(movetime - 250);
		DrawGraph(0, 0, menuW[goust], TRUE);
		goust = command - 1;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 51 * movetime / 50);
		DrawGraph(0, 0, menuW[goust], TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		//�n�ʕ\��
		goust = command - LR - 1;
		while (goust < 0) goust += 4;
		while (goust > 3) goust -= 4;
		goust2 = (movetime - 250)*(movetime - 250);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, -51 * movetime / 50 + 255);
		DrawGraph(0, (-12 * goust2) / 3125 + 240, menuG[goust], TRUE);
		goust = command - 1;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 51 * movetime / 50);
		DrawGraph(0, (12 * goust2) / 3125, menuG[goust], TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		//���̃R�}���h��\������
		goust = command - 3;
		while (goust < 0) goust += 4;
		for (i = 0; i < 5; i++) {
			goust2 = movetime - 250 * i * LR + 500 * LR - 250;
			DrawGraph((21 * LR * movetime * movetime - 10500 * LR * movetime + 1312500 * LR + 1312500 * i - 2125000) / 3125, (-1 * goust2 * goust2 + 250000) / 2500, menu[goust], TRUE);
			goust++;
			if (goust > 3) goust -= 4;
		}
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"���E�L�[:�I��   Enter�L�[:����", Cr);
		else if (lan[4] == 1)DrawString(5, 460, L"����key:choose   Enter key:enter", Cr);
		ScreenFlip();
		//��������L�[����
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//���������ꂽ
			if (key == 0) {
				command -= 1;
				LR = -1;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				starttime = GetNowCount();
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//�E�������ꂽ
			if (key == 0) {
				command += 1;
				LR = 1;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				starttime = GetNowCount();
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN)) {
			//�G���^�[�������ꂽ
			if (key == 0) next = command + 1;
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			//����{�^���������ꂽ
			next = 5;
			break;
		}
		//����̃L�[��������Ă��Ȃ�
		else key = 0;
		//�R�}���h�̒[���߂����Ƃ��A�����Е��̒[�Ɉړ�����
		if (command <= 0) command = 4;
		else if (command >= 5) command = 1;
		WaitTimer(5);
	}
	return next;
}

int musicserect(int *p1) {
	//����64�Ȃ�������Ȃ��̂ŁA�K�v�ɉ����đ��₵�ĂˁB
	unsigned int Cr[7];
	Cr[0] = Cr[1] = Cr[2] = Cr[4] = Cr[5] = Cr[6] = GetColor(255, 255, 255);
	Cr[3] = GetColor(0, 0, 0);
	int songT, musicT, picsong, e, i, n = 0, UD = 1, LR = 1, key = 1, next = 99, startC = -250, XstartC = -250, previewC = 0, moveC = 250, XmoveC = 250, command[2] = { 0,1 }, rimit[64], level[64][6], Hscore[64][6];
	int G[2];
	double diskr = 0, rate[10], preview[64][6][2];
	wchar_t songname[64][6][256], artist[64][6][256], songM[64][6][256], jacketP[64][6][256], mapT[255], GT2[255], GT4[255], GT11[255], GT18[255];
	wchar_t GT[] = { L"record/" };
	wchar_t GT3[6][8] = { L"/0.txt" ,L"/1.txt" ,L"/2.txt" ,L"/3.txt" ,L"/4.txt" ,L"/5.txt" };
	wchar_t GT5[] = { L"#TITLE:" };
	wchar_t GT5E[] = { L"#E.TITLE:" };
	wchar_t GT6[] = { L"#ARTIST:" };
	wchar_t GT6E[] = { L"#E.ARTIST:" };
	wchar_t GT7[] = { L"#MUSIC:" };
	wchar_t GT8[] = { L"#LEVEL:" };
	wchar_t GT9[] = { L"#PREVIEW:" };
	wchar_t GT10[] = { L"#JACKET:" };
	wchar_t GT12[] = { L"score/" };
	wchar_t GT13[] = { L".dat" };
	wchar_t GT14[] = { L"NULL" };
	wchar_t GT15[] = { L"song/" };
	wchar_t GT16[] = { L"picture/NULL jucket.png" };
	wchar_t GT17[] = { L"picture/" };
	wchar_t GT19[] = { L"/" };
	wchar_t GT20[] = { L"#MAP:" };
	wchar_t GT21[255] = L"record/";
	wchar_t playingsong[255] = { L"NULL" };
	wchar_t viewingjacket[255] = { L"NULL" };
	int bar[7], difbar[6], difC[5][2], detail, back, disk, help, select, jacketpic, previewM;
	bar[0] = bar[1] = bar[2] = bar[4] = bar[5] = bar[6] = LoadGraph(L"picture/songbarB.png");
	bar[3] = LoadGraph(L"picture/songbarY.png");
	difbar[0] = LoadGraph(L"picture/difauto.png");
	difbar[1] = LoadGraph(L"picture/difeasy.png");
	difbar[2] = LoadGraph(L"picture/difnormal.png");
	difbar[3] = LoadGraph(L"picture/difhard.png");
	difbar[4] = difbar[5] = LoadGraph(L"picture/difhope.png");
	difC[0][0] = LoadGraph(L"picture/dif0S.png");
	difC[0][1] = LoadGraph(L"picture/dif0B.png");
	difC[1][0] = LoadGraph(L"picture/dif1S.png");
	difC[1][1] = LoadGraph(L"picture/dif1B.png");
	difC[2][0] = LoadGraph(L"picture/dif2S.png");
	difC[2][1] = LoadGraph(L"picture/dif2B.png");
	difC[3][0] = LoadGraph(L"picture/dif3S.png");
	difC[3][1] = LoadGraph(L"picture/dif3B.png");
	difC[4][0] = LoadGraph(L"picture/dif4S.png");
	difC[4][1] = LoadGraph(L"picture/dif4B.png");
	detail = LoadGraph(L"picture/detail.png");
	back = LoadGraph(L"picture/MSback.png");
	disk = LoadGraph(L"picture/disk.png");
	help = LoadGraph(L"picture/help.png");
	jacketpic = LoadGraph(L"picture/NULL jucket.png");
	select = LoadSoundMem(L"sound/arrow.ogg");
	previewM = LoadSoundMem(L"null.mp3");
	PlaySoundMem(previewM, DX_PLAYTYPE_BACK);
	int	lan[5] = { 0,0,0,2,0 };//�g���̂�[4,����]����
	FILE *fp;
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&lan, sizeof(lan), 5, fp);
		fclose(fp);
	}
	songT = FileRead_open(L"song.txt");
	while (FileRead_eof(songT) == 0) {
		FileRead_gets(GT2, 256, songT);
		rimit[n] = 3;
		for (i = 0; i < 6; i++) {
			strcopy(GT, mapT, 1);
			strcats(mapT, GT2);
			strcopy(mapT, GT18, 1);
			strcats(mapT, GT3[i]);
			musicT = FileRead_open(mapT);
			strcopy(GT14, songname[n][i], 1);
			strcopy(GT14, artist[n][i], 1);
			strcopy(GT14, songM[n][i], 1);
			strcopy(GT16, jacketP[n][i], 1);
			level[n][i] = -1;
			preview[n][i][0] = 441000;
			preview[n][i][1] = 2646000;
			while (FileRead_eof(musicT) == 0) {
				FileRead_gets(GT4, 256, musicT);
				//�Ȗ���ǂݍ���
				if (strands(GT4, GT5) && (lan[4] == 0 || strands(songname[n][i], GT14))) {
					strmods(GT4, 7);
					strcopy(GT4, songname[n][i], 1);
				}
				if (strands(GT4, GT5E) && lan[4] == 1) {
					strmods(GT4, 9);
					strcopy(GT4, songname[n][i], 1);
				}
				//��Ȏ҂�ǂݍ���
				if (strands(GT4, GT6) && (lan[4] == 0 || strands(artist[n][i], GT14))) {
					strmods(GT4, 8);
					strcopy(GT4, artist[n][i], 1);
				}
				if (strands(GT4, GT6E) && lan[4] == 1) {
					strmods(GT4, 10);
					strcopy(GT4, artist[n][i], 1);
				}
				//�ȃt�@�C������ǂݍ���
				if (strands(GT4, GT7)) {
					strcopy(GT15, songM[n][i], 1);
					strmods(GT4, 7);
					strcats(songM[n][i], GT4);
				}
				//��Փx��ǂݍ���
				if (strands(GT4, GT8)) {
					strmods(GT4, 7);
					level[n][i] = strsans(GT4);
				}
				//�v���r���[���Ԃ�ǂݍ���
				if (strands(GT4, GT9)) {
					strmods(GT4, 9);
					preview[n][i][0] = strsans(GT4) * 44100.0 / 1000;
					while (GT4[0] >= '0'&&GT4[0] <= '9') strmods(GT4, 1);
					strmods(GT4, 1);
					preview[n][i][1] = strsans(GT4) * 44100.0 / 1000;
				}
				//�W���P�b�g�ʐ^��ǂݍ���
				if (strands(GT4, GT10)) {
					strcopy(GT18, jacketP[n][i], 1);
					strcats(jacketP[n][i], GT19);
					strmods(GT4, 8);
					strcats(jacketP[n][i], GT4);
				}
				//�}�b�v�ɓ������甲����
				if (strands(GT4, GT20)) break;
			}
		}
		//�n�C�X�R�A��ǂݍ���
		strcopy(GT12, GT11, 1);
		strcats(GT11, GT2);
		strcats(GT11, GT13);
		e = _wfopen_s(&fp, GT11, L"rb");
		if (e == 0) {
			fread(&Hscore[n], sizeof(Hscore[n]), 6, fp);
			fclose(fp);
		}
		for (i = 0; i < 6; i++) if (Hscore[n][i] < 0) Hscore[n][i] = 0;
		if (Hscore[n][5] > 0) rimit[n] = 4;
		FileRead_close(musicT);
		n++;
	}
	n--;
	//�����܂ŁA�ȏ��̓ǂݍ���
	e = _wfopen_s(&fp, L"save/rateN.dat", L"rb");
	if (e == 0) {
		fread(&rate, sizeof(rate), 10, fp);
		fclose(fp);
	}
	for (i = 0; i < 10; i++) if (rate[i] < 0) rate[i] = 0;
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, back, TRUE);
		//���ԑ���
		moveC = -1 * (GetNowCount() - startC) + 250;
		XmoveC = -1 * (GetNowCount() - XstartC) + 250;
		if (moveC < 0) moveC = 0;
		if (XmoveC < 0) XmoveC = 0;
		//�W���P�b�g��\������
		if (strands(viewingjacket, jacketP[command[0]][command[1]]) == 0) {
			DeleteGraph(jacketpic);
			strcopy(jacketP[command[0]][command[1]], viewingjacket, 1);
			jacketpic = LoadGraph(viewingjacket);
		}
		DrawGraph(305, 75, jacketpic, TRUE);
		//�Ȗ����̑���������\������
		picsong = command[0] - 3;
		while (picsong < 0) picsong += n + 1;
		for (i = 0; i < 7; i++) {
			DrawGraph((UD*moveC*moveC + 62500 * i - 375000) / 3125, (6 * UD*moveC*moveC + 375000 * i - 531250) / 3125, bar[i], TRUE);
			DrawString((UD*moveC*moveC + 62500 * i - 93750) / 3125, (6 * UD*moveC*moveC + 375000 * i - 468750) / 3125, songname[picsong][command[1]], Cr[i]);
			DrawString((UD*moveC*moveC + 62500 * i - 93750) / 3125, (6 * UD*moveC*moveC + 375000 * i - 406250) / 3125, artist[picsong][command[1]], Cr[i]);
			DrawFormatString((UD*moveC*moveC + 62500 * i + 62500) / 3125, (6 * UD*moveC*moveC + 375000 * i - 312500) / 3125, Cr[i], L"HIGHSCORE:%d", Hscore[picsong][command[1]]);
			picsong++;
			if (picsong > n) picsong -= n + 1;
		}
		//��Փx�}�[�J�[��\������
		G[1] = 0;
		for (i = 0; i <= 4; i++) {
			G[0] = 0;
			if (command[1] == i) G[0] = 1;
			if (command[1] < i) G[1] = 1;
			if (strands(songM[command[0]][i], GT14) == 0 && i <= rimit[command[0]]) DrawGraph(550 + 11 * G[1] + 16 * i, 290 - 11 * G[0], difC[i][G[0]], TRUE);
		}
		//��Փx�̖��O��\������
		if (LR == 1) {
			DrawGraph(460, 320, difbar[command[1]], TRUE);
			DrawGraph(-0.00288*XmoveC*XmoveC + 640, 320, difbar[command[1] + 1], TRUE);
		}
		else if (LR == -1) {
			DrawGraph(460, 320, difbar[command[1] - 1], TRUE);
			DrawGraph(0.00288*XmoveC*XmoveC + 460, 320, difbar[command[1]], TRUE);
		}
		//�ڍׂ�\������
		DrawGraph(316, 370, detail, TRUE);
		DrawFormatString(500, 390, Cr[3], L"LEVEL:%d", level[command[0]][command[1]]);
		//�v���r���[�𗬂�
		if (moveC == 0 && XmoveC == 0 && strands(songM[command[0]][command[1]], GT14) == 0 && strands(playingsong, songM[command[0]][command[1]]) == 0) {
			StopSoundMem(previewM);
			DeleteSoundMem(previewM);
			strcopy(songM[command[0]][command[1]], playingsong, 1);
			previewM = LoadSoundMem(playingsong);
			SetCurrentPositionSoundMem(preview[command[0]][command[1]][0], previewM);
			PlaySoundMem(previewM, DX_PLAYTYPE_BACK, FALSE);
			WaitTimer(30);
		}
		//�f�B�X�N��\������(���[�g�\���ꏊ)
		DrawRotaGraph(640, 25, 1, diskr, disk, TRUE);
		diskr += 0.02;
		if (diskr > 6.28) diskr -= 6.28;
		DrawFormatString(550, 100, Cr[0], L"RATE:%5.2f", rate[0] + rate[1] + rate[2] + rate[3] + rate[4] + rate[5] + rate[6] + rate[7] + rate[8] + rate[9]);
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"�㉺�L�[:�ȑI���@���E�L�[:��Փx�I��   Enter�L�[:����   BackSpace�L�[:�߂�", Cr[0]);
		else if (lan[4] == 1)DrawString(5, 460, L"����key:music select, ����key:dif select, Enter key:start, BackSpace key:back to menu", Cr[0]);
		//�f�o�b�O(320,410�X�^�[�g)
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_UP) == 1) {
			//�オ�����ꂽ
			if (key == 0) {
				command[0]--;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				UD = -1;
				startC = GetNowCount();
				//�c�R�}���h(��)�̒[���߂����Ƃ��A�����Е��̒[�Ɉړ�����
				if (command[0] < 0) command[0] = n;
				else if (command[0] > n) command[0] = 0;
				//���I�񂾋Ȃ̓�Փx�ɕ��ʂ�����������A���ʂ������Փx��T���B
				strands(songname[command[0]][command[1]], GT14);
				if (strands(songname[command[0]][command[1]], GT14)) {
					if (strands(songname[command[0]][0], GT14) != 1) command[1] = 0;
					for (i = 1; i <= 3; i++) {
						if (strands(songname[command[0]][i], GT14) != 1) {
							command[1] = i;
							break;
						}
					}
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN) == 1) {
			//���������ꂽ
			if (key == 0) {
				command[0]++;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				UD = 1;
				startC = GetNowCount();
				//�c�R�}���h(��)�̒[���߂����Ƃ��A�����Е��̒[�Ɉړ�����
				if (command[0] < 0) command[0] = n;
				else if (command[0] > n) command[0] = 0;
				//���I�񂾋Ȃ̓�Փx�ɕ��ʂ�����������A���ʂ������Փx��T���B
				strands(songname[command[0]][command[1]], GT14);
				if (strands(songname[command[0]][command[1]], GT14)) {
					if (strands(songname[command[0]][0], GT14) != 1) command[1] = 0;
					for (i = 1; i <= 3; i++) {
						if (strands(songname[command[0]][i], GT14) != 1) {
							command[1] = i;
							break;
						}
					}
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_LEFT) == 1) {
			//���������ꂽ
			if (key == 0) {
				command[1]--;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				LR = 1;
				XstartC = GetNowCount();
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT) == 1) {
			//�E�������ꂽ
			if (key == 0) {
				command[1]++;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				LR = -1;
				XstartC = GetNowCount();
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN) == 1) {
			//�G���^�[�������ꂽ(�ȃt�@�C������NULL�̏ꍇ�̓X�L�b�v)
			if (key == 0 && strands(songM[command[0]][command[1]], GT14) == 0) {
				//�B���ȗp
				if (command[1] == 3 && Hscore[command[0]][3] >= 90000 && strands(songM[command[0]][5], GT14) == 0 && Hscore[command[0]][5] <= 0) {
					G[0] = 0;
					if (Hscore[command[0]][3] >= 90000 && Hscore[command[0]][3] < 92500)G[0] = Hscore[command[0]][3] / 250000.0 * Hscore[command[0]][3] - Hscore[command[0]][3] * 0.72 + 32400;
					else if (Hscore[command[0]][3] >= 92500 && Hscore[command[0]][3] < 95000)G[0] = -1 * Hscore[command[0]][3] / 250000.0 * Hscore[command[0]][3] + Hscore[command[0]][3] * 0.76 - 36050;
					else if (Hscore[command[0]][3] >= 95000 && Hscore[command[0]][3] < 98000)G[0] = Hscore[command[0]][3] / 12857.14 * Hscore[command[0]][3] - Hscore[command[0]][3] * 14.777777 + 701994;
					else if (Hscore[command[0]][3] >= 98000 && Hscore[command[0]][3] < 99000)G[0] = -1 * Hscore[command[0]][3] / 4000.0 * Hscore[command[0]][3] + Hscore[command[0]][3] * 49.5 - 2449250;
					else if (Hscore[command[0]][3] >= 99000)G[0] = 1000;
					if (GetRand(1000) <= G[0])command[1] = 5;
				}
				StopSoundMem(previewM);
				ClearDrawScreen();
				InitSoundMem();
				InitGraph();
				*p1 = command[0];
				p1++;
				*p1 = command[1];
				next = 6;
				break;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK) == 1) {
			//�o�b�N�X�y�[�X�������ꂽ
			if (key == 0) {
				StopSoundMem(previewM);
				ClearDrawScreen();
				next = 1;
				break;
			}
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			next = 5;
			break;
		}
		else key = 0;
		//���R�}���h(��Փx)�̒[���߂��Ȃ��悤�ɂ���
		if (command[1] < 0) {
			command[1] = 0;
			XstartC -= 250;
		}
		else if (command[1] > rimit[command[0]]) {
			command[1] = rimit[command[0]];
			XstartC -= 250;
		}
		WaitTimer(5);
	}
	return next;
}

int play2(int n, int o) {
	short int i[2];
	short int Lv = 0;
	short int notes = 0;
	short int bgp[3] = { 0,0,0 }; //[0:sky,1:ground,2:water]�̉��ʒu
	short int bgf[2] = { 0,0 }; //�������w�i��[0:���ʒu,1:�c�ʒu]
	short int charaput = 1; //�L�����̍��̈ʒu[0�ŏ�,1�Œ�,2�ŉ�]
	short int drop = 0;
	int judgh = 0; //�m�[�c�̎��ԋ���
	int charahit = 0; //�L�������m�[�c������������ł��邩�ǂ����B[1�ȏ�Œ@�����A0�Œ@���ĂȂ�]
	int G[10], songT;
	int system[5] = { 0,0,0,2,0 };
	int noteoff = 0; //�m�[�c�̃I�t�Z�b�g
	int Etime = 0; //���ʂ̏I���̎���
	int songdata = 0;
	int Ntime = 0;
	int judghname[3][3]{ 0,0,0,0,0,0,0,0,0 };
	int holda = 0;
	int holdb = 0;
	int holdu = 0;
	int holdd = 0;
	int holdl = 0;
	int holdr = 0;
	int holdG = 0;
	int combo = 0;
	int LaneTrack[3] = { -100,-100,-100 };
	int Mcombo = 0;
	int Dscore[3] = { 0,0,0 }; //�����ɓ����镔��[���_�p,���_�ۑ��p,�����ۑ��p]
	int judghcount[4] = { 0,0,0,0 };
	int life = 500;
	int gap[30] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//gap = ���肸��
	int gapa[3] = { 0,0,0 };//gapa = ���肸��[���v, ��, 2��̍��v]
	int score2[4] = { 0,0,0,0 }; //[�ʏ�X�R�A(90000), �R���{�X�R�A(10000), ���_, ���v]
	int fps[62];//0�`59=1�t���[���Ԋu�̎���,60=���̑����,61=�O��̎���
	for (i[0] = 0; i[0] <= 59; i[0]++)fps[i[0]] = 17;
	fps[60] = 0;
	fps[61] = 0;
	double GD[5];
	int item[99]; //�A�C�e���̃A�h���X�ADrawGraph�ŌĂׂ�B
	short int itemN = 0; //���̔ԍ�
	int chamo[3][99][2]; //�L������[0:��,1:��,2:��]��[0:�O���t�B�b�N,1:�؂�ւ�����]
	chamo[0][0][0] = 0;
	chamo[0][0][1] = 0;
	chamo[1][0][0] = 1;
	chamo[1][0][1] = 0;
	chamo[2][0][0] = 1;
	chamo[2][0][1] = 0;
	short int chamoN[3] = { 1,1,1 }; //���̔ԍ�
	int fall[99][2]; //�������w�i��[0:�ԍ�,1:����]
	fall[0][0] = 0;
	fall[0][1] = -1;
	short int fallN = 1; //���̔ԍ�
	int Ymove[5][999][4]; //[��, ��, ��, (�n��), (����)]���[���c�ړ���[0:�J�n����,1:�ʒu,2:�I������,3:���]
	Ymove[0][0][0] = 0;
	Ymove[0][0][1] = 300;
	Ymove[0][0][2] = 0;
	Ymove[0][0][3] = 1;
	Ymove[1][0][0] = 0;
	Ymove[1][0][1] = 350;
	Ymove[1][0][2] = 0;
	Ymove[1][0][3] = 1;
	Ymove[2][0][0] = 0;
	Ymove[2][0][1] = 400;
	Ymove[2][0][2] = 0;
	Ymove[2][0][3] = 1;
	Ymove[3][0][0] = 0;
	Ymove[3][0][1] = 350;
	Ymove[3][0][2] = 0;
	Ymove[3][0][3] = 1;
	Ymove[4][0][0] = 0;
	Ymove[4][0][1] = 600;
	Ymove[4][0][2] = 0;
	Ymove[4][0][3] = 1;
	short int YmoveN[5] = { 1,1,1,1,1 }; //���̔ԍ�
	short int YmoveN2[3] = { 0,0,0 };
	int Xmove[3][999][4]; //[��, ��, ��]���[�����ړ���[0:�J�n����,1:�ʒu,2:�I������,3:���]
	Xmove[0][0][0] = 0;
	Xmove[0][0][1] = 150;
	Xmove[0][0][2] = 0;
	Xmove[0][0][3] = 1;
	Xmove[1][0][0] = 0;
	Xmove[1][0][1] = 150;
	Xmove[1][0][2] = 0;
	Xmove[1][0][3] = 1;
	Xmove[2][0][0] = 0;
	Xmove[2][0][1] = 150;
	Xmove[2][0][2] = 0;
	Xmove[2][0][3] = 1;
	short int XmoveN[3] = { 1,1,1 }; //���̔ԍ�
	short int XmoveN2[3] = { 0,0,0 };
	int lock[2][2][99]; //lock = [��,�c]�̉����̈ʒu��[(1=�Œ肷��,-1�ȊO=�Œ肵�Ȃ�),����]
	lock[0][0][0] = -1;
	lock[0][1][0] = 0;
	lock[1][0][0] = 1;
	lock[1][1][0] = 0;
	short int lockN[2] = { 1,1 }; //���̔ԍ�
	int carrow[2][99];
	carrow[0][0] = 1;
	carrow[1][0] = 0;
	short int carrowN = 1;
	int viewT[2][99];//[�����\������,���s����,[0]=���i���o�[]
	viewT[0][0] = 0;
	viewT[1][0] = 3000;
	short int viewTN = 1;
	int object[3][5][999]; //[��,��,��]���[���̉�����[����,���,(�g���ĂȂ�),�c�ʒu,���ʒu]
	object[0][3][0] = 300;
	object[1][3][0] = 350;
	object[2][3][0] = 400;
	short int objectN[3] = { 0,0,0 }; //���̔ԍ�
	int difkey[50][4];//��Փx�v�Z�Ɏg��[�ԍ�][���̓L�[,����,��Փx�_,[0]�����:[1]���̔ԍ�:[2]1�O�̔ԍ�:[3]2�O�̔ԍ�:[4]�ō��_:[5]�f�[�^��:[6]�Ō�50�̍��v:[7]�v�Z���珜�O���鎞��]
	difkey[0][2] = 0;
	difkey[1][2] = 0;
	difkey[1][3] = 0;
	difkey[2][3] = -1;
	difkey[3][3] = -2;
	difkey[4][3] = 0;
	difkey[5][3] = -1;
	int ddif[25] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//�e��Ԃ̓�Փx
	int ddifG[2] = { 1,1 };//0=�������Ԕԍ�(1�`25),1=�ő�l
	int Yline[5] = { 300,350,400,350,600 };//[��,��,��,�n��,����]���[���̏c�ʒu
	int Xline[3] = { 150,150,150 };//[��,��,��]���[���̉��ʒu
	double rate, bpm = 120, bpmG = 120;
	double timer[3]; //[��, ��, ��]���[���̎���
	double speedt[5][99][2]; //[��, ��, ��, (�n��), (����)]���[����[0:�؂�ւ�����,1:���x]
	speedt[0][0][0] = 0;
	speedt[0][0][1] = 1;
	speedt[1][0][0] = 0;
	speedt[1][0][1] = 1;
	speedt[2][0][0] = 0;
	speedt[2][0][1] = 1;
	speedt[3][0][0] = 0;
	speedt[3][0][1] = 1;
	speedt[4][0][0] = 0;
	speedt[4][0][1] = 1;
	short int speedN[5] = { 1,1,1,1,1 }; //���̔ԍ�
	char key[256];
	wchar_t songN[255];
	songN[0] = L'\0';
	wchar_t fileN[255];
	wchar_t dataE[255] = L"record/";
	wchar_t mp3FN[255] = L"song/";
	wchar_t skyFN[255] = L"picture/backskynoamal.png";
	wchar_t groundFN[255] = L"picture/groundnaturenormal.png";
	wchar_t waterFN[255] = L"picture/waternormal.png";
	wchar_t GT1[255];
	wchar_t GT15[255];
	wchar_t GT24[] = L"picture/";
	wchar_t GT25[6][8] = { L"/0.txt" ,L"/1.txt" ,L"/2.txt" ,L"/3.txt" ,L"/4.txt" ,L"/5.txt" };
	wchar_t RecordCode[23][13] = { L"#MUSIC:",L"#BPM:",L"#NOTEOFFSET:",L"#SKY:",L"#FIELD:",
		L"#WATER:",L"#TITLE:",L"#LEVEL:",L"#ITEM:",L"#FALL:",
		L"#MAP:",L"#END",L"#SPEED",L"#CHARA",L"#MOVE",
		L"#XMOV",L"#GMOVE",L"#XLOCK",L"#YLOCK",L"#FALL",
		L"#VIEW:",L"#E.TITLE:",L"#CARROW"
	};
	//wchar_t GT2[] = L"#MUSIC:";
	//wchar_t GT3[] = L"#BPM:";
	//wchar_t GT4[] = L"#NOTEOFFSET:";
	//wchar_t GT5[] = L"#SKY:";
	//wchar_t GT6[] = L"#FIELD:";
	//wchar_t GT7[] = L"#WATER:";
	//wchar_t GT8[] = L"#TITLE:";
	//wchar_t GT9[] = L"#LEVEL:";
	//wchar_t GT10[] = L"#ITEM:";
	//wchar_t GT11[] = L"#FALL:";
	//wchar_t GT12[] = L"#MAP:";
	//wchar_t GT13[] = L"#END";
	//wchar_t GT14[] = L"#SPEED";
	//wchar_t GT16[] = L"#CHARA";
	//wchar_t GT17[] = L"#MOVE";
	//wchar_t GT18[] = L"#XMOV";
	//wchar_t GT19[] = L"#GMOVE";
	//wchar_t GT20[] = L"#XLOCK";
	//wchar_t GT21[] = L"#YLOCK";
	//wchar_t GT22[] = L"#FALL";
	//wchar_t GT23[] = L"#VIEW:";
	//wchar_t GT26[] = L"#E.TITLE:";
	//wchar_t GT27[] = L"#CARROW";
	unsigned int Cr, Crb;
	Cr = GetColor(255, 255, 255);
	Crb = GetColor(0, 0, 0);
	FILE *fp;
	//�V�X�e�����[�h
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&system, sizeof(system), 5, fp);
		fclose(fp);
	}
	songT = FileRead_open(L"song.txt");
	for (i[0] = 0; i[0] <= n; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcopy(GT1, fileN, 1);
	strcats(dataE, GT1);
	strcopy(dataE, GT1, 1);
	strcats(GT1, GT25[o]);
	songdata = FileRead_open(GT1);
	//�e�L�X�g�f�[�^��ǂ�
	while (FileRead_eof(songdata) == 0) {
		FileRead_gets(GT1, 256, songdata);
		//���y�t�@�C����ǂݍ���
		if (strands(GT1, RecordCode[0])) {
			strmods(GT1, 7);
			strcats(mp3FN, GT1);
		}
		//BPM��ǂݍ���
		else if (strands(GT1, RecordCode[1])) bpmG = bpm = SETbpm(GT1);
		//�m�[�g�̃I�t�Z�b�g��ǂݍ���
		else if (strands(GT1, RecordCode[2])) timer[0] = timer[1] = timer[2] = noteoff = SEToffset(GT1, system[1]);
		//��̔w�i��ǂݍ���
		else if (strands(GT1, RecordCode[3])) {
			strcopy(GT24, skyFN, 1);
			strmods(GT1, 5);
			strcats(skyFN, GT1);
		}
		//�n�ʂ̉摜��ǂݍ���
		else if (strands(GT1, RecordCode[4])) {
			strcopy(GT24, groundFN, 1);
			strmods(GT1, 7);
			strcats(groundFN, GT1);
		}
		//�����̉摜��ǂݍ���
		else if (strands(GT1, RecordCode[5])) {
			strcopy(GT24, waterFN, 1);
			strmods(GT1, 7);
			strcats(waterFN, GT1);
		}
		//�Ȗ���ǂݍ���
		else if (strands(GT1, RecordCode[6])) {
			strmods(GT1, 7);
			if (system[4] == 0 || songN[0] == L'\0')strcopy(GT1, songN, 1);
		}
		//�p��
		else if (strands(GT1, RecordCode[21])) {
			strmods(GT1, 7);
			if (system[4] == 1 || songN[0] == L'\0')strcopy(GT1, songN, 1);
		}
		//���x����ǂݍ���
		else if (strands(GT1, RecordCode[7])) Lv = SETLv(GT1);
		//�A�C�e����ǂݍ���
		else if (strands(GT1, RecordCode[8])) {
			strmods(GT1, 6);
			strcopy(dataE, GT15, 1);
			stradds(GT15, L'/');
			strcats(GT15, GT1);
			item[itemN++] = LoadGraph(GT15);
		}
		//�������w�i�w��
		else if (strands(GT1, RecordCode[9])) {
			strmods(GT1, 6);
			fall[0][0] = strsans(GT1);
			fall[0][1] = 0;
		}
		//���ʂ�ǂݍ���
		else if (strands(GT1, RecordCode[10])) {
			while (FileRead_eof(songdata) == 0 && strands(GT1, RecordCode[11]) == 0) {
				FileRead_gets(GT1, 256, songdata);
				//���m�[�c���x�ω�
				if (strands(GT1, RecordCode[12])) {
					G[0] = maxs(mins(GT1[6] - 49, 0), 4);
					strmods(GT1, 8);
					speedt[G[0]][speedN[G[0]]][1] = strsans2(GT1);
					strnex(GT1);
					if (GT1[0] >= L'0' && GT1[0] <= L'9' || GT1[0] == L'-') {
						speedt[G[0]][speedN[G[0]]][0] = timer[G[0]] + 240000 * (speedt[G[0]][speedN[G[0]]][1] - 1) / (bpm * 16) - 10;
						speedt[G[0]][speedN[G[0]]][1] = strsans2(GT1);
					}
					else {
						speedt[G[0]][speedN[G[0]]][0] = timer[G[0]] - 10;
					}
					speedN[G[0]]++;
				}
				//BPM�ω�
				else if (strands(GT1, RecordCode[1])) bpmG = SETbpm(GT1);
				//�L�����O���ω�
				else if (strands(GT1, RecordCode[13])) {
					G[0] = GT1[6] - 49;
					strmods(GT1, 8);
					chamo[G[0]][chamoN[G[0]]][0] = maxs(mins(strsans(GT1), 0), 2);
					chamo[G[0]][chamoN[G[0]]][1] = timer[G[0]];
					chamoN[G[0]]++;
				}
				//�c�ړ�
				else if (strands(GT1, RecordCode[14])) {
					G[0] = maxs(mins(GT1[8] - 49, 0), 4);
					switch (GT1[5]) {
					case('l'):
						G[1] = 1;
						break;
					case('a'):
						G[1] = 2;
						break;
					case('d'):
						G[1] = 3;
						break;
					case('m'):
						G[1] = 4;
						break;
					case('s'):
						G[1] = 5;
						break;
					case('p'):
						G[1] = 6;
						break;
					}
					strmods(GT1, 10);
					GD[0] = strsans2(GT1);
					strnex(GT1);
					GD[1] = strsans2(GT1);
					strnex(GT1);
					GD[2] = strsans2(GT1);
					switch (G[1]) {
					case 1:
					case 2:
					case 3:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]]][3] = G[1];
						break;
					case 4:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime(GD[2], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 1;
						break;
					case 5:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = (Ymove[G[0]][YmoveN[G[0]] - 1][1] + GD[1] * 50 + 100) / 2;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 2;
						Ymove[G[0]][YmoveN[G[0]] + 1][0] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]] + 1][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]] + 1][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]] + 1][3] = 3;
						YmoveN[G[0]]++;
						break;
					case 6:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 3;
						Ymove[G[0]][YmoveN[G[0]] + 1][0] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]] + 1][1] = Ymove[G[0]][YmoveN[G[0]] - 1][1];
						Ymove[G[0]][YmoveN[G[0]] + 1][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]] + 1][3] = 2;
						YmoveN[G[0]]++;
						break;
					}
					YmoveN[G[0]]++;
				}
				//���ړ�
				else if (strands(GT1, RecordCode[15])) {
					G[0] = maxs(mins(GT1[8] - 49, 0), 2);
					switch (GT1[5]) {
					case('l'):
						G[1] = 1;
						break;
					case('a'):
						G[1] = 2;
						break;
					case('d'):
						G[1] = 3;
						break;
					case('m'):
						G[1] = 4;
						break;
					case('s'):
						G[1] = 5;
						break;
					case('p'):
						G[1] = 6;
						break;
					}
					strmods(GT1, 10);
					GD[0] = strsans2(GT1);
					strnex(GT1);
					GD[1] = strsans2(GT1);
					strnex(GT1);
					GD[2] = strsans2(GT1);
					switch (G[1]) {
					case 1:
					case 2:
					case 3:
						Xmove[G[0]][XmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Xmove[G[0]][XmoveN[G[0]]][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Xmove[G[0]][XmoveN[G[0]]][3] = G[1];
						break;
					case 4:
						Xmove[G[0]][XmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]) - 5;
						Xmove[G[0]][XmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Xmove[G[0]][XmoveN[G[0]]][2] = shifttime(GD[2], bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]]][3] = 1;
						break;
					case 5:
						Xmove[G[0]][XmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]]][1] = (Xmove[G[0]][XmoveN[G[0]] - 1][1] + GD[1] * 50 + 100) / 2;
						Xmove[G[0]][XmoveN[G[0]]][2] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]]][3] = 2;
						Xmove[G[0]][XmoveN[G[0]] + 1][0] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]] + 1][1] = GD[1] * 50.0 + 100.0;
						Xmove[G[0]][XmoveN[G[0]] + 1][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Xmove[G[0]][XmoveN[G[0]] + 1][3] = 3;
						XmoveN[G[0]]++;
						break;
					case 6:
						Xmove[G[0]][XmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Xmove[G[0]][XmoveN[G[0]]][2] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]]][3] = 3;
						Xmove[G[0]][XmoveN[G[0]] + 1][0] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]] + 1][1] = Xmove[G[0]][XmoveN[G[0]] - 1][1];
						Xmove[G[0]][XmoveN[G[0]] + 1][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Xmove[G[0]][XmoveN[G[0]] + 1][3] = 2;
						XmoveN[G[0]]++;
						break;
					}
					XmoveN[G[0]]++;
				}
				//GMOVE
				else if (strands(GT1, RecordCode[16])) {
					G[0] = 3;
					switch (GT1[6]) {
					case('l'):
						G[1] = 1;
						break;
					case('a'):
						G[1] = 2;
						break;
					case('d'):
						G[1] = 3;
						break;
					case('m'):
						G[1] = 4;
						break;
					case('s'):
						G[1] = 5;
						break;
					case('p'):
						G[1] = 6;
						break;
					}
					strmods(GT1, 10);
					GD[0] = strsans2(GT1);
					strnex(GT1);
					GD[1] = strsans2(GT1);
					strnex(GT1);
					GD[2] = strsans2(GT1);
					switch (G[1]) {
					case 1:
					case 2:
					case 3:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]]][3] = G[1];
						break;
					case 4:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime(GD[2], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 1;
						break;
					case 5:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = (Ymove[G[0]][YmoveN[G[0]] - 1][1] + GD[1] * 50 + 100) / 2;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 2;
						Ymove[G[0]][YmoveN[G[0]] + 1][0] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]] + 1][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]] + 1][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]] + 1][3] = 3;
						YmoveN[G[0]]++;
						break;
					case 6:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 3;
						Ymove[G[0]][YmoveN[G[0]] + 1][0] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]] + 1][1] = Ymove[G[0]][YmoveN[G[0]] - 1][1];
						Ymove[G[0]][YmoveN[G[0]] + 1][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]] + 1][3] = 2;
						YmoveN[G[0]]++;
						break;
					}
					YmoveN[G[0]]++;
				}
				//�����b�N
				else if (strands(GT1, RecordCode[17])) {
					strmods(GT1, 7);
					lock[0][0][lockN[0]] = lock[0][0][lockN[0] - 1] * -1;
					lock[0][1][lockN[0]] = shifttime(strsans(GT1), bpmG, timer[0]);
					lockN[0]++;
				}
				//�c���b�N
				else if (strands(GT1, RecordCode[18])) {
					strmods(GT1, 7);
					lock[1][0][lockN[1]] = lock[1][0][lockN[1] - 1] * -1;
					lock[1][1][lockN[1]] = shifttime(strsans(GT1), bpmG, timer[0]);
					lockN[1]++;
				}
				//�L���������ω�
				else if (strands(GT1, RecordCode[22])) {
					strmods(GT1, 8);
					carrow[0][carrowN] = carrow[0][carrowN - 1] * -1;
					carrow[1][carrowN] = shifttime(strsans(GT1), bpmG, timer[0]);
					carrowN++;
				}
				//�������w�i�؂�ւ�
				else if (strands(GT1, RecordCode[19])) {
					strmods(GT1, 6);
					fall[fallN][0] = strsans(GT1);
					strnex(GT1);
					fall[fallN][1] = strsans(GT1);
					fallN++;
				}
				//�����\������
				else if (strands(GT1, RecordCode[20])) {
					strmods(GT1, 6);
					viewT[0][viewTN] = shifttime(strsans(GT1), bpmG, timer[0]);
					strnex(GT1);
					viewT[1][viewTN] = strsans(GT1);
					viewTN++;
				}
				//�I���
				else if (strands(GT1, RecordCode[11])) {
					break;
				}
				//����ȊO
				else {
					for (i[0] = 0; i[0] <= 2; i[0]++) {
						G[0] = 0;
						while (GT1[G[0]] != L'\0' && GT1[G[0]] != L',') G[0]++;
						for (i[1] = 0; i[1] < G[0]; i[1]++) {
							if (GT1[i[1]] == L'0')continue;
							object[i[0]][0][objectN[i[0]]] = timer[i[0]] + 240000 * i[1] / (bpmG * G[0]);
							switch (GT1[i[1]]) {
							case L'H':
								object[i[0]][1][objectN[i[0]]] = 1;
								break;
							case L'C':
								object[i[0]][1][objectN[i[0]]] = 2;
								break;
							case L'U':
								object[i[0]][1][objectN[i[0]]] = 3;
								break;
							case L'D':
								object[i[0]][1][objectN[i[0]]] = 4;
								break;
							case L'L':
								object[i[0]][1][objectN[i[0]]] = 5;
								break;
							case L'R':
								object[i[0]][1][objectN[i[0]]] = 6;
								break;
							case L'B':
								object[i[0]][1][objectN[i[0]]] = 7;
								break;
							case L'G':
								object[i[0]][1][objectN[i[0]]] = 8;
								break;
							}
							//object[][2][]���g�����������炱���ɓ����(�Q�[�������ԂɂȂ�\��)
							object[i[0]][3][objectN[i[0]]] = 50 * i[0] + 300;
							object[i[0]][4][objectN[i[0]]] = 150;
							//�c�ʒu���v�Z����
							while (Ymove[i[0]][YmoveN2[i[0]]][2] <= object[i[0]][0][objectN[i[0]]] && Ymove[i[0]][YmoveN2[i[0]]][2] >= 0)YmoveN2[i[0]]++;
							if (Ymove[i[0]][YmoveN2[i[0]]][0] >= 0 && Ymove[i[0]][YmoveN2[i[0]]][0] <= object[i[0]][0][objectN[i[0]]] && Ymove[i[0]][YmoveN2[i[0]]][2] > object[i[0]][0][objectN[i[0]]]) {
								switch (Ymove[i[0]][YmoveN2[i[0]]][3]) {
								case 1:
									object[i[0]][3][objectN[i[0]]] = lins(Ymove[i[0]][YmoveN2[i[0]]][0], Ymove[i[0]][YmoveN2[i[0]] - 1][1], Ymove[i[0]][YmoveN2[i[0]]][2], Ymove[i[0]][YmoveN2[i[0]]][1], object[i[0]][0][objectN[i[0]]]);
									break;
								case 2:
									object[i[0]][3][objectN[i[0]]] = pals(Ymove[i[0]][YmoveN2[i[0]]][0], Ymove[i[0]][YmoveN2[i[0]] - 1][1], Ymove[i[0]][YmoveN2[i[0]]][2], Ymove[i[0]][YmoveN2[i[0]]][1], object[i[0]][0][objectN[i[0]]]);
									break;
								case 3:
									object[i[0]][3][objectN[i[0]]] = pals(Ymove[i[0]][YmoveN2[i[0]]][2], Ymove[i[0]][YmoveN2[i[0]]][1], Ymove[i[0]][YmoveN2[i[0]]][0], Ymove[i[0]][YmoveN2[i[0]] - 1][1], object[i[0]][0][objectN[i[0]]]);
									break;
								}
							}
							else object[i[0]][3][objectN[i[0]]] = Ymove[i[0]][YmoveN2[i[0]] - 1][1];
							//���ʒu���v�Z����
							while (Xmove[i[0]][XmoveN2[i[0]]][2] <= object[i[0]][0][objectN[i[0]]] && Xmove[i[0]][XmoveN2[i[0]]][2] >= 0) XmoveN2[i[0]]++;
							if (Xmove[i[0]][XmoveN2[i[0]]][0] >= 0 && Xmove[i[0]][XmoveN2[i[0]]][0] <= object[i[0]][0][objectN[i[0]]] && Xmove[i[0]][XmoveN2[i[0]]][2] > object[i[0]][0][objectN[i[0]]]) {
								switch (Xmove[i[0]][XmoveN2[i[0]]][3]) {
								case 1:
									object[i[0]][4][objectN[i[0]]] = lins(Xmove[i[0]][XmoveN2[i[0]]][0], Xmove[i[0]][XmoveN2[i[0]] - 1][1], Xmove[i[0]][XmoveN2[i[0]]][2], Xmove[i[0]][XmoveN2[i[0]]][1], object[i[0]][0][objectN[i[0]]]);
									break;
								case 2:
									object[i[0]][4][objectN[i[0]]] = pals(Xmove[i[0]][XmoveN2[i[0]]][0], Xmove[i[0]][XmoveN2[i[0]] - 1][1], Xmove[i[0]][XmoveN2[i[0]]][2], Xmove[i[0]][XmoveN2[i[0]]][1], object[i[0]][0][objectN[i[0]]]);
									break;
								case 3:
									object[i[0]][4][objectN[i[0]]] = pals(Xmove[i[0]][XmoveN2[i[0]]][2], Xmove[i[0]][XmoveN2[i[0]]][1], Xmove[i[0]][XmoveN2[i[0]]][0], Xmove[i[0]][XmoveN2[i[0]] - 1][1], object[i[0]][0][objectN[i[0]]]);
									break;
								}
							}
							else object[i[0]][4][objectN[i[0]]] = Xmove[i[0]][XmoveN2[i[0]] - 1][1];
							if (object[i[0]][1][objectN[i[0]]] != 8) notes++;
							objectN[i[0]]++;
						}
						if (i[0] <= 1) FileRead_gets(GT1, 256, songdata);
					}
					timer[0] = timer[1] = timer[2] += 240000.0 / bpmG;
				}
			}
		}
	}
	//���ʂ̍Ō��goust��u��
	for (i[0] = 0; i[0] <= 2; i[0]++) {
		object[i[0]][0][objectN[i[0]]] = timer[i[0]];
		object[i[0]][0][objectN[i[0]] + 1] = -1;
		object[i[0]][1][objectN[i[0]]] = 8;
		object[i[0]][3][objectN[i[0]]] = 1000;
	}
	lock[0][0][lockN[0]] = 1;
	lock[0][1][lockN[0]] = -1;
	lock[1][0][lockN[1]] = -1;
	lock[1][1][lockN[1]] = -1;
	Etime = timer[0];
	/*��Փx�v�Z
	(3000000/�ЂƂO�̃m�[�c�Ƃ̊Ԋu)���A���̃m�[�c�̓�Փx�_���Ƃ���B
	�@�����L�[��50�L�����A���̑g�ݍ��킹�łł����ō��l���Ȃ̓�Փx�B
	1�O�Ɠ����L�[�̎���(�Ώۂ̃L�[��arrow�̂�)
	���Ԋu��150ms�ȏ�̎��͕ϓ��Ȃ�
	���Ԋu��150ms�ȉ�75ms�ȏ�̎��͓��_("�Ԋu"/(-75)+3)�{(1�`2�{)
	���Ԋu��75ms�ȉ��̎��͓��_2�{
	arrow�Ђ�������2.5�{
	2�O�ƈႤ�L�[�̎��͓��_1.2�{(�S�L�[�Ώ�)
	arrow�L�[�͓��_1.2�{
	1=hit,2=non,3=up,4=down,5=left,6=right,7=non or douw,8=up or down,9=up or non
	*/
	objectN[0] = 0;
	objectN[1] = 0;
	objectN[2] = 0;
	difkey[0][3] = notes;
	if (difkey[0][3] > 49)difkey[0][3] = 49;
	difkey[7][3] = (Etime - noteoff) / 25 * 2;
	if (difkey[7][3] < 10000)difkey[7][3] = 10000;
	while (object[0][0][objectN[0]] >= 0 || object[1][0][objectN[1]] >= 0 || object[2][0][objectN[2]] >= 0) {
		for (i[0] = 0; i[0] < 3; i[0]++) while (object[i[0]][1][objectN[i[0]]] == 8 && object[i[0]][0][objectN[i[0]]] >= 0) objectN[i[0]]++;
		G[0] = -1;
		for (i[0] = 0; i[0] < 3; i[0]++) if (object[i[0]][0][objectN[i[0]]] >= 0) {
			G[0] = i[0];
			break;
		}
		if (G[0] == -1) break;
		for (i[0] = 0; i[0] < 3; i[0]++) if (G[0] != i[0] && object[G[0]][0][objectN[G[0]]] > object[i[0]][0][objectN[i[0]]] && object[i[0]][0][objectN[i[0]]] >= 0) G[0] = i[0];
		//ddif�̌v�Z
		//old
		while (object[G[0]][0][objectN[G[0]]] >= (Etime - noteoff) / 25 * ddifG[0] + noteoff) {
			if (difkey[5][3] < 49) {
				for (i[0] = 0; i[0] < difkey[5][3]; i[0]++) if (difkey[i[0]][1] > (Etime - noteoff) / 25 * ddifG[0] - difkey[7][3] + noteoff)ddif[ddifG[0] - 1] += difkey[i[0]][2];
			}
			else {
				for (i[0] = 0; i[0] <= 49; i[0]++)if (difkey[i[0]][1] > (Etime - noteoff) / 25 * ddifG[0] - difkey[7][3] + noteoff) ddif[ddifG[0] - 1] += difkey[i[0]][2];
			}
			ddifG[0]++;
		}
		difkey[difkey[1][3]][0] = object[G[0]][1][objectN[G[0]]];
		if (difkey[difkey[1][3]][0] == 2) {
			if (G[0] != 1) difkey[difkey[1][3]][0] = G[0] / 2 + 3;
			if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0]) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 3 && (difkey[difkey[2][3]][0] == 8 || difkey[difkey[2][3]][0] == 9)) {
				difkey[difkey[2][3]][0] = 3;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 2 && (difkey[difkey[2][3]][0] == 1 || difkey[difkey[2][3]][0] == 5 || difkey[difkey[2][3]][0] == 6)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 2 && (difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9)) {
				difkey[difkey[2][3]][0] = 2;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 4 && (difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 8)) {
				difkey[difkey[2][3]][0] = 4;
				objectN[G[0]]++;
				continue;
			}
		}
		else if (difkey[difkey[1][3]][0] == 7) {
			if (G[0] != 0)difkey[difkey[1][3]][0] = G[0] + 7;
			if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0]) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && difkey[difkey[2][3]][0] == 8 || difkey[difkey[1][3]][0] == 8 && difkey[difkey[2][3]][0] == 7) {
				difkey[difkey[2][3]][0] = 4;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && difkey[difkey[2][3]][0] == 9 || difkey[difkey[1][3]][0] == 9 && difkey[difkey[2][3]][0] == 7) {
				difkey[difkey[2][3]][0] = 2;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 8 && difkey[difkey[2][3]][0] == 9 || difkey[difkey[1][3]][0] == 9 && difkey[difkey[2][3]][0] == 8) {
				difkey[difkey[2][3]][0] = 3;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && (difkey[difkey[2][3]][0] != 3)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 8 && (difkey[difkey[2][3]][0] == 3 || difkey[difkey[2][3]][0] == 4)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 9 && (difkey[difkey[2][3]][0] != 4)) {
				objectN[G[0]]++;
				continue;
			}
		}
		difkey[difkey[1][3]][1] = object[G[0]][0][objectN[G[0]]];
		if (difkey[2][3] != -1 && difkey[3][3] != -1) {
			G[1] = difkey[difkey[1][3]][1] - difkey[difkey[2][3]][1];
			if (G[1] == 0)G[1] = 1;
			if (G[1] <= 5 && difkey[difkey[1][3]][0] != difkey[difkey[2][3]][0]) difkey[difkey[1][3]][2] = difkey[difkey[2][3]][2] * 1.3;
			else {
				difkey[difkey[1][3]][2] = 3000000 / G[1];
				if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0] && difkey[difkey[1][3]][0] >= 3 && difkey[difkey[1][3]][0] <= 6 && difkey[difkey[1][3]][2] >= 40000) difkey[difkey[1][3]][2] *= 2;
				else if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0] && difkey[difkey[1][3]][0] >= 3 && difkey[difkey[1][3]][0] <= 6 && difkey[difkey[1][3]][2] <= 40000 && difkey[difkey[1][3]][2] >= 20000) difkey[difkey[1][3]][2] = difkey[difkey[1][3]][2] / -75.0 + 3.0;
				if (difkey[difkey[1][3]][0] != difkey[difkey[3][3]][0]) difkey[difkey[1][3]][2] *= 1.2;
				if (difkey[difkey[1][3]][0] >= 3 && difkey[difkey[1][3]][0] <= 6)difkey[difkey[1][3]][2] *= 1.2;
			}
		}
		for (i[0] = 0; i[0] < 3; i[0]++) if (object[G[0]][1][objectN[G[0]]] >= 3 && object[G[0]][1][objectN[G[0]]] <= 6 && G[0] != i[0] && object[G[0]][1][objectN[G[0]]] == object[i[0]][1][objectN[i[0]]] && object[G[0]][0][objectN[G[0]]] + 5 >= object[i[0]][0][objectN[i[0]]]) objectN[i[0]]++;
		objectN[G[0]]++;
		difkey[5][3]++;
		G[0] = 0;
		for (i[0] = 0; i[0] <= difkey[0][3]; i[0]++) {
			if (difkey[i[0]][2] < 0) break;
			if (difkey[i[0]][1] > difkey[difkey[1][3]][1] - difkey[7][3])G[0] += difkey[i[0]][2];
		}
		if (difkey[4][3] < G[0])difkey[4][3] = G[0];
		for (i[0] = 1; i[0] < 4; i[0]++) {
			difkey[i[0]][3]++;
			if (difkey[i[0]][3] > difkey[0][3])difkey[i[0]][3] = 0;
		}
	}
	difkey[5][3]++;
	for (i[0] = 0; i[0] < 2; i[0]++)if (difkey[i[0]][2] == 0 && difkey[2][2] > 0)difkey[5][3]--;
	if (difkey[5][3] < 1)difkey[5][3] = 1;
	if (difkey[5][3] > 50)difkey[5][3] = 50;
	ddifG[1] = difkey[4][3];
	if (ddifG[1] <= 0)ddifG[1] = 1;
	difkey[4][3] /= 50;
	ddif[ddifG[0] - 1] = 0;
	for (i[0] = 0; i[0] < difkey[5][3]; i[0]++) if (difkey[i[0]][1] > Etime - difkey[7][3])ddif[ddifG[0] - 1] += difkey[i[0]][2];
	for (i[0] = ddifG[0] - 1; i[0] <= 24; i[0]++)ddif[i[0]] = ddif[ddifG[0] - 1];
	difkey[6][3] = ddif[ddifG[0] - 1] / 50;
	//���[�j���O�O���[���[Lv.1->884, Trick MonitorLv.9->28632, "����"/31.8+47.1="Lv"*100
	difkey[4][3] = difkey[4][3] / 31.8 + 47.1;
	difkey[6][3] = difkey[6][3] / 31.8 + 47.1;
	//�i���o�[���Z�b�g
	objectN[0] = 0;
	objectN[1] = 0;
	objectN[2] = 0;
	YmoveN[0] = 0;
	YmoveN[1] = 0;
	YmoveN[2] = 0;
	YmoveN[3] = 0;
	YmoveN[4] = 0;
	XmoveN[0] = 0;
	XmoveN[1] = 0;
	XmoveN[2] = 0;
	lockN[0] = 0;
	lockN[1] = 0;
	chamoN[0] = 0;
	chamoN[1] = 0;
	chamoN[2] = 0;
	speedN[0] = 0;
	speedN[1] = 0;
	speedN[2] = 0;
	speedN[3] = 0;
	carrowN = 0;
	fallN = 0;
	viewTN = 0;
	//�O���t�B�b�N�ƌ��ʉ��̏���
	/*�O���t�B�b�N�p�ϐ�
	judghimg = ����}�[�N
	hitimg = hit
	hitcimg = hit�̎���̐F
	catchimg = catch
	upimg = up
	downimg = down
	leftimg = left
	rightimg = right
	bombimg = bomb
	goustimg = goust
	backskyimg
	backgroundimg
	dangerimg
	charaimg
	musicmp3 = ���y*/
	int judghimg, hitimg, hitcimg, catchimg, upimg, downimg, leftimg, rightimg, bombimg, goustimg;
	int backskyimg, backgroundimg, backwaterimg, dangerimg, dropimg, resultimg, sbarimg, sbbarimg, filterimg, charaguideimg, gapbarimg, gaplineimg, difberimg;
	int Lbarimg[3];
	int Tbarimg[2];
	int	charaimg[30];
	int rankimg[6];
	int coleimg[5];
	int effimg[7][5];
	int Rchaimg;
	int musicmp3, attack, catchs, arrow;
	judghimg = LoadGraph(L"picture/Marker.png");
	hitimg = LoadGraph(L"picture/hit.png");
	hitcimg = LoadGraph(L"picture/hitc.png");
	catchimg = LoadGraph(L"picture/catch.png");
	upimg = LoadGraph(L"picture/up.png");
	downimg = LoadGraph(L"picture/down.png");
	leftimg = LoadGraph(L"picture/left.png");
	rightimg = LoadGraph(L"picture/right.png");
	bombimg = LoadGraph(L"picture/bomb.png");
	goustimg = LoadGraph(L"picture/goust.png");
	switch (o) {
	case 0:
		difberimg = LoadGraph(L"picture/difauto.png");
		break;
	case 1:
		difberimg = LoadGraph(L"picture/difeasy.png");
		break;
	case 2:
		difberimg = LoadGraph(L"picture/difnormal.png");
		break;
	case 3:
		difberimg = LoadGraph(L"picture/difhard.png");
		break;
	case 4:
	case 5:
		difberimg = LoadGraph(L"picture/difhope.png");
		break;
	}
	backskyimg = LoadGraph(skyFN);
	backgroundimg = LoadGraph(groundFN);
	backwaterimg = LoadGraph(waterFN);
	dangerimg = LoadGraph(L"picture/danger.png");
	dropimg = LoadGraph(L"picture/drop.png");
	resultimg = LoadGraph(L"picture/result.png");
	sbarimg = LoadGraph(L"picture/scoreber.png");
	sbbarimg = LoadGraph(L"picture/scoreber2.png");
	Lbarimg[0] = LoadGraph(L"picture/LIFEbar.png");
	Lbarimg[1] = LoadGraph(L"picture/LIFEbar2.png");
	Lbarimg[2] = LoadGraph(L"picture/LIFEbar3.png");
	Tbarimg[0] = LoadGraph(L"picture/TIMEbar.png");
	Tbarimg[1] = LoadGraph(L"picture/TIMEbar2.png");
	rankimg[0] = LoadGraph(L"picture/rankEX.png");
	rankimg[1] = LoadGraph(L"picture/rankS.png");
	rankimg[2] = LoadGraph(L"picture/rankA.png");
	rankimg[3] = LoadGraph(L"picture/rankB.png");
	rankimg[4] = LoadGraph(L"picture/rankC.png");
	rankimg[5] = LoadGraph(L"picture/rankD.png");
	coleimg[0] = LoadGraph(L"picture/DROPED.png");
	coleimg[1] = LoadGraph(L"picture/CLEARED.png");
	coleimg[2] = LoadGraph(L"picture/NOMISS.png");
	coleimg[3] = LoadGraph(L"picture/FULLCOMBO.png");
	coleimg[4] = LoadGraph(L"picture/PERFECT.png");
	filterimg = LoadGraph(L"picture/Black.png");
	charaguideimg = LoadGraph(L"picture/Cguide.png");
	gapbarimg = LoadGraph(L"picture/GapBer.png");
	gaplineimg = LoadGraph(L"picture/GapBerLine.png");
	LoadDivGraph(L"picture/hiteff.png", 5, 5, 1, 50, 50, effimg[0]);
	LoadDivGraph(L"picture/hiteff.png", 5, 5, 1, 50, 50, effimg[1]);
	LoadDivGraph(L"picture/upeff.png", 5, 5, 1, 50, 50, effimg[2]);
	LoadDivGraph(L"picture/downeff.png", 5, 5, 1, 50, 50, effimg[3]);
	LoadDivGraph(L"picture/lefteff.png", 5, 5, 1, 50, 50, effimg[4]);
	LoadDivGraph(L"picture/righteff.png", 5, 5, 1, 50, 50, effimg[5]);
	LoadDivGraph(L"picture/bombeff.png", 5, 5, 1, 50, 50, effimg[6]);
	switch (system[0]) {
	case 0:
		LoadDivGraph(L"picture/Picker.png", 24, 6, 4, 160, 160, charaimg);
		Rchaimg = LoadGraph(L"picture/RePicker.png");
		break;
	case 1:
		LoadDivGraph(L"picture/Gator.png", 24, 6, 4, 160, 160, charaimg);
		Rchaimg = LoadGraph(L"picture/ReGator.png");
		break;
	case 2:
		LoadDivGraph(L"picture/Taylor.png", 24, 6, 4, 160, 160, charaimg);
		Rchaimg = LoadGraph(L"picture/ReTaylor.png");
		break;
	}
	musicmp3 = LoadSoundMem(mp3FN);
	if (system[2] == 0) {
		attack = LoadSoundMem(L"sound/attack.mp3");
		catchs = LoadSoundMem(L"sound/catch.mp3");
		arrow = LoadSoundMem(L"sound/arrow.ogg");
	}
	else {
		attack = LoadSoundMem(L"sound/non.wav");
		catchs = LoadSoundMem(L"sound/non.wav");
		arrow = LoadSoundMem(L"sound/non.wav");
	}
	//�Q�[���J�n�O�̉�����
	notes = notzero(notes);
	PlaySoundMem(musicmp3, DX_PLAYTYPE_BACK);
	WaitTimer(10);
	int Stime = GetNowCount();
	//�Q�[���J�n
	while (1) {
		ClearDrawScreen();
		GetHitKeyStateAll(key);
		//�w�i��^���Âɂ��Ă���ꍇ�A�w�i��\�����Ȃ��B
		if (system[3] != 0) {
			//�w�i�̉��ʒu�v�Z
			if (speedt[3][speedN[3] + 1][0] < Ntime && speedt[3][speedN[3] + 1][0] >= 0) speedN[3]++;
			bgp[0] -= speedt[3][speedN[3]][1];
			if (bgp[0] < -700) bgp[0] += 640;
			else if (bgp[0] > 0) bgp[0] -= 640;
			bgp[1] -= speedt[3][speedN[3]][1] * 5;
			if (bgp[1] < -700) bgp[1] += 640;
			else if (bgp[1] > 0) bgp[1] -= 640;
			bgp[2] -= speedt[4][speedN[4]][1] * 5;
			if (bgp[2] < -700) bgp[2] += 640;
			else if (bgp[2] > 0) bgp[2] -= 640;
			//�w�i�̏c�ʒu�v�Z
			for (i[0] = 3; i[0] <= 4; i[0]++) {
				if (Ntime >= Ymove[i[0]][YmoveN[i[0]]][0] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0]) {
					switch (Ymove[i[0]][YmoveN[i[0]]][3]) {
					case(1):
						Yline[i[0]] = lins(Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ntime);
						break;
					case(2):
						Yline[i[0]] = pals(Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ntime);
						break;
					case(3):
						Yline[i[0]] = pals(Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ntime);
						break;
					}
				}
				if (Ntime >= Ymove[i[0]][YmoveN[i[0]]][2] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0] || Ymove[i[0]][YmoveN[i[0]]][3] == 4) Yline[i[0]] = Ymove[i[0]][YmoveN[i[0]]++][1];
			}
			//�w�i�\��
			for (i[0] = 0; i[0] < 2; i[0]++) {
				G[0] = bgp[i[0]];
				while (G[0] < 700) {
					switch (i[0]) {
					case(0):
						DrawGraph(G[0], Yline[3] / 5 - 160, backskyimg, TRUE);
						break;
					case(1):
						DrawGraph(G[0], Yline[3] - 400, backgroundimg, TRUE);
						DrawGraph(G[0], Yline[4] - 400, backwaterimg, TRUE);
						break;
					}
					G[0] += 640;
				}
			}
			//�������w�i�\��
			if (Ntime >= fall[fallN + 1][0] && fall[fallN + 1][0] >= 0) fallN++;
			if (fall[fallN][1] >= 0) {
				G[0] = bgf[0];//��
				G[1] = bgf[1] + Yline[3];//�c
				for (i[0] = 0; i[0] < 2; i[0]++) for (i[1] = 0; i[1] < 3; i[1]++) DrawGraph(G[0] + i[0] * 640, G[1] - i[1] * 480, item[fall[fallN][1]], TRUE);
				bgf[0] -= 5;
				bgf[1] += 2;
			}
			if (bgf[0] <= -640)bgf[0] += 640;
			if (bgf[1] >= 640)bgf[1] -= 480;
			//�A�C�e���\��

		}
		//�t�B���^�[�\��
		switch (system[3]) {
		case 1:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 170);
			DrawGraph(0, 0, filterimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			break;
		case 2:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 85);
			DrawGraph(0, 0, filterimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			break;
		}
		//�L�����ʒu�K�C�h�̕\��
		if (carrow[1][carrowN + 1] < Ntime && carrow[1][carrowN + 1] >= 0) carrowN++;
		if (carrow[0][carrowN] == 1) { DrawGraph(Xline[charaput] - 4, Yline[charaput] - 4, charaguideimg, TRUE); }
		else { DrawTurnGraph(Xline[charaput] - 56, Yline[charaput] - 4, charaguideimg, TRUE); }
		//����}�[�J�[�̕\��
		for (i[0] = 0; i[0] < 3; i[0]++) {
			//�c�ړ�
			if (Ntime >= Ymove[i[0]][YmoveN[i[0]]][0] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0]) {
				switch (Ymove[i[0]][YmoveN[i[0]]][3]) {
				case(1):
					Yline[i[0]] = lins(Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ntime);
					break;
				case(2):
					Yline[i[0]] = pals(Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ntime);
					break;
				case(3):
					Yline[i[0]] = pals(Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ntime);
					break;
				}
			}
			if (Ntime >= Ymove[i[0]][YmoveN[i[0]]][2] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0] || Ymove[i[0]][YmoveN[i[0]]][3] == 4) Yline[i[0]] = Ymove[i[0]][YmoveN[i[0]]++][1];
			//���ړ�
			if (Ntime >= Xmove[i[0]][XmoveN[i[0]]][0] && 0 <= Xmove[i[0]][XmoveN[i[0]]][0]) {
				switch (Xmove[i[0]][XmoveN[i[0]]][3]) {
				case(1):
					Xline[i[0]] = lins(Xmove[i[0]][XmoveN[i[0]]][0], Xmove[i[0]][XmoveN[i[0]] - 1][1], Xmove[i[0]][XmoveN[i[0]]][2], Xmove[i[0]][XmoveN[i[0]]][1], Ntime);
					break;
				case(2):
					Xline[i[0]] = pals(Xmove[i[0]][XmoveN[i[0]]][0], Xmove[i[0]][XmoveN[i[0]] - 1][1], Xmove[i[0]][XmoveN[i[0]]][2], Xmove[i[0]][XmoveN[i[0]]][1], Ntime);
					break;
				case(3):
					Xline[i[0]] = pals(Xmove[i[0]][XmoveN[i[0]]][2], Xmove[i[0]][XmoveN[i[0]]][1], Xmove[i[0]][XmoveN[i[0]]][0], Xmove[i[0]][XmoveN[i[0]] - 1][1], Ntime);
					break;
				}
			}
			if (Ntime >= Xmove[i[0]][XmoveN[i[0]]][2] && 0 <= Xmove[i[0]][XmoveN[i[0]]][0] || Xmove[i[0]][XmoveN[i[0]]][3] == 4) Xline[i[0]] = Xmove[i[0]][XmoveN[i[0]]++][1];
			DrawGraph(Xline[i[0]], Yline[i[0]], judghimg, TRUE);
		}
		//�L�����O���ϊ�
		for (i[0] = 0; i[0] < 3; i[0]++)while (Ntime >= chamo[i[0]][chamoN[i[0]] + 1][1] && chamo[i[0]][chamoN[i[0]] + 1][1] >= 0) chamoN[i[0]]++;
		G[3] = 0;
		//�L�����ʒu����
		for (i[0] = 0; i[0] <= 2; i[0]++) if (object[i[0]][0][objectN[i[0]]] <= Ntime + 16 && object[i[0]][1][objectN[i[0]]] == 2) G[3] = 1;
		for (i[0] = 0; i[0] <= 2; i[0]++) {
			G[i[0]] = objectN[i[0]] - 1;
			while (object[i[0]][1][G[i[0]]] == 8 && G[i[0]] >= 1) G[i[0]]--;
		}
		//�ド�C���̃q�b�g�}�[�J�[���������Ă���Ƃ�
		if (charahit > 0 && G[3] == 0 && object[0][0][G[0]] > object[1][0][G[1]] && object[0][0][G[0]] > object[2][0][G[2]] && object[0][0][G[0]] + 750 > Ntime && object[0][1][G[0]] == 1 && judghname[0][0] >= 1 && judghname[0][0] <= 3) {
			G[4] = Yline[0];
			charaput = 0;
		}
		//�����C���̃q�b�g�}�[�J�[���������Ă���Ƃ�
		else if (charahit > 0 && G[3] == 0 && object[1][0][G[1]] > object[0][0][G[0]] && object[1][0][G[1]] > object[2][0][G[2]] && object[1][0][G[1]] + 750 > Ntime && object[1][1][G[1]] == 1 && judghname[1][0] >= 1 && judghname[1][0] <= 3) {
			G[4] = Yline[1];
			charaput = 1;
		}
		//�����C���̃q�b�g�}�[�J�[���������Ă���Ƃ�
		else if (charahit > 0 && G[3] == 0 && object[2][0][G[2]] > object[0][0][G[0]] && object[2][0][G[2]] > object[1][0][G[1]] && object[2][0][G[2]] + 750 > Ntime && object[2][1][G[2]] == 1 && judghname[2][0] >= 1 && judghname[2][0] <= 3) {
			G[4] = Yline[2];
			charaput = 2;
		}
		//�オ������āA���O�̃q�b�g�}�[�J�[���������Ă��Ȃ��Ƃ�
		else if (key[KEY_INPUT_UP] == 1 && key[KEY_INPUT_DOWN] == 0) {
			G[4] = Yline[0];
			charaput = 0;
		}
		//����������āA���O�̃q�b�g�}�[�J�[���������Ă��Ȃ��Ƃ�
		else if (key[KEY_INPUT_UP] == 0 && key[KEY_INPUT_DOWN] == 1) {
			G[4] = Yline[2];
			charaput = 2;
		}
		//����������Ă��Ȃ����A�㉺�����ɉ�����Ă��āA���O�̃q�b�g�}�[�J�[���������Ă��Ȃ��Ƃ�
		else {
			G[4] = Yline[1];
			charaput = 1;
		}
		//�L�����O���t�B�b�N��\��
		if (GetNowCount() - charahit > 250) G[5] = 0;
		else G[5] = pals(250, 0, 0, 50, GetNowCount() - charahit);
		if (charahit > 0) {
			if (carrow[0][carrowN] == 1) DrawGraph(Xline[charaput] + G[5] - 160, G[4] - 75, charaimg[maxs(mins((GetNowCount() - charahit) / 125 + 18, 18), 23)], TRUE);
			else DrawTurnGraph(Xline[charaput] - G[5] + 30, G[4] - 75, charaimg[maxs(mins((GetNowCount() - charahit) / 125 + 18, 18), 23)], TRUE);
		}
		else {
			if (carrow[0][carrowN] == 1) DrawGraph(Xline[charaput] - 160, G[4] - 75, charaimg[Ntime * int(bpm) / 20000 % 6 + chamo[charaput][chamoN[charaput]][0] * 6], TRUE);
			else DrawTurnGraph(Xline[0] + 30, G[4] - 75, charaimg[Ntime * int(bpm) / 20000 % 6 + chamo[charaput][chamoN[charaput]][0] * 6], TRUE);
		}
		if (key[KEY_INPUT_Z] == 0) holda = 0;
		else if (key[KEY_INPUT_Z] == 1) holda++;
		if (key[KEY_INPUT_X] == 0) holdb = 0;
		else if (key[KEY_INPUT_X] == 1) holdb++;
		if (key[KEY_INPUT_UP] == 0) holdu = 0;
		else if (key[KEY_INPUT_UP] == 1) holdu++;
		if (key[KEY_INPUT_LEFT] == 0) holdl = 0;
		else if (key[KEY_INPUT_LEFT] == 1) holdl++;
		if (key[KEY_INPUT_RIGHT] == 0) holdr = 0;
		else if (key[KEY_INPUT_RIGHT] == 1) holdr++;
		if (key[KEY_INPUT_DOWN] == 0) holdd = 0;
		else if (key[KEY_INPUT_DOWN] == 1) holdd++;
		if (key[KEY_INPUT_G] == 0) holdG = 0;
		else if (key[KEY_INPUT_G] == 1) holdG++;
		if (GetWindowUserCloseFlag(TRUE)) return 5;
		//�L���b�`����Ɏg�����l���v�Z
		if (holdu == 0 && holdd == 0 || holdu > 0 && holdd > 0) { LaneTrack[1] = Ntime; }
		else if (holdu > 0 && holdd == 0) { LaneTrack[0] = Ntime; }
		else if (holdu == 0 && holdd > 0) { LaneTrack[2] = Ntime; }
		if (LaneTrack[0] <= LaneTrack[2]) { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[0]); }
		else { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[2]); }
		//�q�b�g
		if (holda == 1 || holdb == 1) charahit = GetNowCount();
		if (charahit + 750 < GetNowCount()) charahit = 0;
		//�����\��
		for (i[0] = 0; i[0] < 2; i[0]++) if (Ntime >= lock[i[0]][1][lockN[i[0]] + 1] && lock[i[0]][1][lockN[i[0]] + 1] >= 0) lockN[i[0]]++;
		if (viewT[0][viewTN + 1] <= Ntime && viewT[0][viewTN + 1] >= 0) viewTN++;
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (speedt[i[0]][speedN[i[0]] + 1][0] <= Ntime && speedt[i[0]][speedN[i[0]] + 1][0] >= 0) speedN[i[0]]++;
			G[0] = G[3] = G[4] = G[5] = 0;
			for (i[1] = objectN[i[0]]; object[i[0]][0][i[1]] > 0; i[1]++) {
				if (object[i[0]][0][i[1]] >= viewT[0][viewTN + G[0] + 1] && viewT[0][viewTN + G[0] + 1] >= 0) G[0]++;
				if (object[i[0]][0][i[1]] - Ntime >= viewT[1][viewTN + G[0]]) continue;
				if (object[i[0]][0][i[1]] - Ntime >= 3000 && 3000 >= viewT[1][viewTN + G[0]]) break;
				if (object[i[0]][0][i[1]] >= lock[0][1][lockN[0] + G[3] + 1] && lock[0][1][lockN[0] + G[3] + 1] >= 0) G[3]++;
				if (object[i[0]][0][i[1]] >= lock[1][1][lockN[1] + G[4] + 1] && lock[1][1][lockN[1] + G[4] + 1] >= 0) G[4]++;
				if (object[i[0]][0][i[1]] >= speedt[i[0]][speedN[i[0]] + G[5] + 1][0] && speedt[i[0]][speedN[i[0]] + G[5] + 1][0] >= 0) G[5]++;
				//�c�ʒu
				if (lock[1][0][lockN[1] + G[4]] == 1) G[2] = object[i[0]][3][i[1]];
				else G[2] = Yline[i[0]];
				//���ʒu
				G[1] = (speedt[i[0]][speedN[i[0]] + G[5]][1] * 20 * (object[i[0]][0][i[1]] - Ntime) + 5000) / 50;
				G[1] += 50;
				if (lock[0][0][lockN[0] + G[3]] == 1) G[1] += object[i[0]][4][i[1]] - 150;
				else G[1] += Xline[i[0]] - 150;
				switch (object[i[0]][1][i[1]]) {
				case 1:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], hitcimg, TRUE);
					break;
				case 2:
					DrawGraph(G[1], G[2], catchimg, TRUE);
					break;
				case 3:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], upimg, TRUE);
					break;
				case 4:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], downimg, TRUE);
					break;
				case 5:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], leftimg, TRUE);
					break;
				case 6:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], rightimg, TRUE);
					break;
				case 7:
					DrawGraph(G[1], G[2], bombimg, TRUE);
					break;
				case 8:
					DrawGraph(G[1], G[2], goustimg, TRUE);
					break;
				}
			}
		}
		//����
		G[1] = 0;
		for (i[0] = 0; i[0] < 3; i[0]++) {
			judgh = object[i[0]][0][objectN[i[0]]] - Ntime;
			//�q�b�g�m�[�c
			if ((holda == 1 || holdb == 1) && G[1] == 0 && object[i[0]][1][objectN[i[0]]] == 1 && judgh <= 125 && judgh >= -100 &&
				(object[i[0]][0][objectN[i[0]]] <= object[0][0][objectN[0]]) &&
				(object[i[0]][0][objectN[i[0]]] <= object[1][0][objectN[1]]) &&
				(object[i[0]][0][objectN[i[0]]] <= object[2][0][objectN[2]])) {
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 1;
				objectN[i[0]]++;
				G[1] = 1;
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				//just
				if (judgh <= 40 && judgh >= -40) {
					judghname[i[0]][0] = 1;
					combo++;
					judghcount[0]++;
					life += 2;
					Dscore[0] += 2;
					PlaySoundMem(attack, DX_PLAYTYPE_BACK);
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
					judghname[i[0]][0] = 2;
					combo++;
					judghcount[1]++;
					life++;
					Dscore[0]++;
					PlaySoundMem(attack, DX_PLAYTYPE_BACK);
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
					judghname[i[0]][0] = 3;
					judghcount[2]++;
					life++;
					PlaySoundMem(attack, DX_PLAYTYPE_BACK);
				}
				//fastmiss
				else if (judgh <= 125) {
					judghname[i[0]][0] = 4;
					combo = 0;
					judghcount[3]++;
					life -= 20;
				}
			}
			//�L���b�`�m�[�c(just�̂�)
			else if (LaneTrack[i[0]] + 100 >= object[i[0]][0][objectN[i[0]]] && object[i[0]][1][objectN[i[0]]] == 2 && judgh <= 0) {
				judghname[i[0]][0] = 1;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 2;
				objectN[i[0]]++;
				combo++;
				charahit = 0;
				judghcount[0]++;
				life += 2;
				Dscore[0] += 2;
				PlaySoundMem(catchs, DX_PLAYTYPE_BACK);
			}
			//�A���[�m�[�c�e��
			else if ((holdu == 1) && object[i[0]][1][objectN[i[0]]] == 3 && judgh <= 125 && judgh >= -100) {
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 3;
				objectN[i[0]]++;
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				//just
				if (judgh <= 40 && judgh >= -40) {
					judghname[i[0]][0] = 1;
					combo++;
					judghcount[0]++;
					life += 2;
					Dscore[0] += 2;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
					judghname[i[0]][0] = 2;
					combo++;
					judghcount[1]++;
					life++;
					Dscore[0]++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
					judghname[i[0]][0] = 3;
					judghcount[2]++;
					life++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//fastmiss
				else if (judgh <= 125) {
					judghname[i[0]][0] = 4;
					combo = 0;
					judghcount[3]++;
					life -= 20;
				}
			}
			else if ((holdd == 1) && object[i[0]][1][objectN[i[0]]] == 4 && judgh <= 125 && judgh >= -100) {
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 4;
				objectN[i[0]]++;
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				//just
				if (judgh <= 40 && judgh >= -40) {
					judghname[i[0]][0] = 1;
					combo++;
					judghcount[0]++;
					life += 2;
					Dscore[0] += 2;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
					judghname[i[0]][0] = 2;
					combo++;
					judghcount[1]++;
					life++;
					Dscore[0]++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
					judghname[i[0]][0] = 3;
					judghcount[2]++;
					life++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//fastmiss
				else if (judgh <= 125) {
					judghname[i[0]][0] = 4;
					combo = 0;
					judghcount[3]++;
					life -= 20;
				}
			}
			else if ((holdl == 1) && object[i[0]][1][objectN[i[0]]] == 5 && judgh <= 125 && judgh >= -100) {
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 5;
				objectN[i[0]]++;
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				//just
				if (judgh <= 40 && judgh >= -40) {
					judghname[i[0]][0] = 1;
					combo++;
					judghcount[0]++;
					life += 2;
					Dscore[0] += 2;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
					judghname[i[0]][0] = 2;
					combo++;
					judghcount[1]++;
					life++;
					Dscore[0]++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
					judghname[i[0]][0] = 3;
					judghcount[2]++;
					life++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//fastmiss
				else if (judgh <= 125) {
					judghname[i[0]][0] = 4;
					combo = 0;
					judghcount[3]++;
					life -= 20;
				}
			}
			else if ((holdr == 1) && object[i[0]][1][objectN[i[0]]] == 6 && judgh <= 125 && judgh >= -100) {
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 6;
				objectN[i[0]]++;
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				//just
				if (judgh <= 40 && judgh >= -40) {
					judghname[i[0]][0] = 1;
					combo++;
					judghcount[0]++;
					life += 2;
					Dscore[0] += 2;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
					judghname[i[0]][0] = 2;
					combo++;
					judghcount[1]++;
					life++;
					Dscore[0]++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
					judghname[i[0]][0] = 3;
					judghcount[2]++;
					life++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//fastmiss
				else if (judgh <= 125) {
					judghname[i[0]][0] = 4;
					combo = 0;
					judghcount[3]++;
					life -= 20;
				}
			}
			//�{���m�[�c
			else if (i[0] == charaput && object[i[0]][1][objectN[i[0]]] == 7 && judgh <= 0 && judgh >= -40) {
				judghname[i[0]][0] = 4;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 7;
				objectN[i[0]]++;
				combo = 0;
				judghcount[3]++;
				life -= 20;
			}
			else if (object[i[0]][1][objectN[i[0]]] == 7 && judgh < -40) {
				judghname[i[0]][0] = 1;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 7;
				objectN[i[0]]++;
				combo++;
				judghcount[0]++;
				life += 2;
				Dscore[0] += 2;
			}
			//�S�[�X�g�m�[�c
			else if (object[i[0]][1][objectN[i[0]]] == 8 && judgh < 16) objectN[i[0]]++;
			//�S�m�[�cslowmiss
			else if (judgh <= -100 && judgh >= -100000 && object[i[0]][1][objectN[i[0]]] >= 1 && object[i[0]][1][objectN[i[0]]] <= 6) {
				judghname[i[0]][0] = 4;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 0;
				combo = 0;
				objectN[i[0]]++;
				judghcount[3]++;
				life -= 20;
			}
		}
		if (gapa[2] > 2140000000) {
			gapa[0] /= 2;
			gapa[1] /= 2;
			gapa[2] /= 2;
		}
		Mcombo = mins(Mcombo, combo);
		//����+�q�b�g�G�t�F�N�g�\��
		for (i[0] = 0; i[0] < 3; i[0]++) {
			switch (judghname[i[0]][0]) {
			case(1):
				DrawString(Xline[i[0]] - 25, Yline[i[0]] - 25, L"JUST", Cr);
				break;
			case(2):
				DrawString(Xline[i[0]] - 25, Yline[i[0]] - 25, L"GOOD", Cr);
				break;
			case(3):
				DrawString(Xline[i[0]] - 25, Yline[i[0]] - 25, L"SAFE", Cr);
				break;
			case(4):
				DrawString(Xline[i[0]] - 25, Yline[i[0]] - 25, L"MISS", Cr);
				break;
			}
			if (judghname[i[0]][1] + 250 > GetNowCount() && judghname[i[0]][0] >= 1 && judghname[i[0]][0] <= 3 && judghname[i[0]][2] >= 1 && judghname[i[0]][2] <= 6)DrawGraph(Xline[i[0]] - 10, Yline[i[0]] - 10, effimg[judghname[i[0]][2] - 1][(GetNowCount() - judghname[i[0]][1] + 250) / 50 % 5], TRUE);
			else if (judghname[i[0]][1] + 250 > GetNowCount() && judghname[i[0]][0] == 4 && judghname[i[0]][2] == 7)DrawGraph(Xline[i[0]] - 10, Yline[i[0]] - 10, effimg[6][(GetNowCount() - judghname[i[0]][1] + 250) / 50 % 5], TRUE);
			if (judghname[i[0]][1] + 750 < GetNowCount()) {
				judghname[i[0]][0] = 0;
				judghname[i[0]][1] = 0;
				judghname[i[0]][2] = 0;
			}
		}
		//���C�t��0�����̎��A1���Ɍ��_�X�R�A��20���₷�B
		if (life < 0) {
			score2[2] = maxs(score2[2] - life * 20, score2[0] + score2[1]);
			life = 0;
		}
		//�X�R�A�o�[�\��
		DrawGraph(0, 0, sbarimg, TRUE);
		//�_���\��
		score2[0] = (judghcount[0] * 90000 + judghcount[1] * 85000 + judghcount[2] * 45000) / notes;
		score2[1] = Mcombo * 10000 / notes;
		DrawFormatString(490, 20, Cr, L"SCORE:%d", score2[0] + score2[1] - score2[2]);
		//���C�t�\��
		life = maxs(life, 500);
		if (life > 100) {
			DrawGraph((291 * life - 57000) / 500, 3, Lbarimg[0], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (-51 * life + 25500) / 80);
			DrawGraph((291 * life - 57000) / 500, 3, Lbarimg[1], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
		else DrawGraph((291 * life - 57000) / 500, 3, Lbarimg[2], TRUE);
		DrawFormatString(440, 10, Cr, L"%3d", life);
		//�����\��
		if (drop == 0) {
			Dscore[1] = Dscore[0];
			G[0] = mins(Ntime - noteoff, 0);
		}
		else if (drop) { G[0] = Dscore[2]; }
		if (G[0] > Etime - noteoff) { //CLEARED
			DrawGraph(155, 38, Tbarimg[1], TRUE);
			GD[0] = (Etime - noteoff) / 100000.0;
			DrawFormatString(180, 45, Crb, L"%.3fkm", GD[0] + Dscore[1] / 1000.0);
		}
		else { //PLAYING or DROPED
			DrawGraph((291 * G[0] - 136 * Etime + 136 * noteoff) / (Etime - noteoff), 38, Tbarimg[0], TRUE);
			GD[0] = G[0] / 100000.0;
			DrawFormatString(180, 45, Cr, L"%.3fkm", GD[0] + Dscore[1] / 1000.0);
		}
		//������Փx�\��
		if (holdG >= 1) {
			G[0] = ddif[0] * 20 / notzero(ddifG[1]) + 155;
			G[1] = ddif[24] * 20 / notzero(ddifG[1]) + 447;
			for (i[0] = 0; i[0] <= 23; i[0]++) DrawLine((G[0] * (24 - i[0]) + G[1] * i[0]) / 24, -ddif[i[0]] * 34 / notzero(ddifG[1]) + 72, (G[0] * (23 - i[0]) + G[1] * (1 + i[0])) / 24, -ddif[i[0] + 1] * 34 / notzero(ddifG[1]) + 72, Cr);
			DrawFormatString(490, 80, Cr, L"mdif:%.2f", difkey[4][3] / 100.0);
			DrawFormatString(490, 100, Cr, L"ldif:%.2f", difkey[6][3] / 100.0);
		}
		//�X�R�A�o�[�B���\��
		DrawGraph(0, 0, sbbarimg, TRUE);
		//�R���{�\��
		if (combo >= 1) DrawFormatString(15, 15, Cr, L"%d combo", combo);
		//���肸��o�[�\��
		DrawGraph(219, 460, gapbarimg, TRUE);
		G[0] = gapa[1] % 30;
		for (i[0] = 0; i[0] < 30; i[0]++) {
			G[0]--;
			if (G[0] < 0) G[0] += 30;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (510 - G[0] * 17) / 2);
			DrawGraph(318 - gap[i[0]], 460, gaplineimg, TRUE);
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		//�f�o�b�N
		fps[fps[60]++] = Ntime - fps[61];
		if (fps[60] > 59)fps[60] -= 60;
		fps[61] = Ntime;
		G[0] = 0;
		for (i[0] = 0; i[0] <= 59; i[0]++)G[0] += fps[i[0]];
		if (Ntime != 0) DrawFormatString(20, 80, Cr, L"FPS: %.0f", 60000.0 / notzero(G[0]));
		//for (i[0] = 0; i[0] < 3; i[0]++) { DrawFormatString(20, 100 + i[0] * 20, Cr, L"LaneTrack%d: %d", i[0], LaneTrack[i[0]]); }
		//���C�t��20%�ȉ��̎��A�댯�M��(�s�N�`��)���o��
		if (life <= 100 && drop == 0) DrawGraph(0, 0, dangerimg, TRUE);
		//���C�t���Ȃ��Ȃ�����DROPED����
		if (life <= 0 && drop == 0) {
			drop = 1;
			Dscore[1] = Dscore[0];
			Dscore[2] = mins(Ntime - noteoff, 0);
		}
		if (drop) { DrawGraph(0, 0, dropimg, TRUE); }
		//�I�����Ԃ���1�b�ȏソ���āA�Ȃ��I�������甲����B
		if (CheckSoundMem(musicmp3) == 0 && Etime <= Ntime + 5000) {
			StopSoundMem(musicmp3);
			DeleteSoundMem(musicmp3);
			break;
		}
		WaitTimer(5);
		Ntime = GetNowCount() - Stime;
		ScreenFlip();
	}
	int	read[7] = { 0,0,0,0,0,0,0 };
	int chap[3] = { 0,0,0 };
	double readR[10];
	wchar_t savec[10][255];
	wchar_t save[255] = L"score/";
	wchar_t save2[255] = L".dat";
	score2[3] = score2[0] + score2[1] - score2[2];
	//�Z�[�u����
	strcats(save, fileN);
	strcats(save, save2);
	//�n�C�X�R�A�ۑ�
	G[0] = _wfopen_s(&fp, save, L"rb");
	if (G[0] == 0) {
		fread(&read, sizeof(read), 6, fp);
		fclose(fp);
	}
	if (read[o] < score2[3]) {
		read[o] = score2[3];
		G[0] = _wfopen_s(&fp, save, L"wb");
		fwrite(&read, sizeof(int), 6, fp);
		fclose(fp);
	}
	//�v���C�񐔕ۑ�
	G[0] = _wfopen_s(&fp, L"save/data.dat", L"rb");
	if (G[0] == 0) {
		fread(&read, sizeof(read), 7, fp);
		fclose(fp);
	}
	read[0]++;
	if (drop == 1) read[1]++;
	else read[3]++;
	if (judghcount[3] == 0)read[4]++;
	if (judghcount[2] == 0 && judghcount[3] == 0)read[5]++;
	if (judghcount[1] == 0 && judghcount[2] == 0 && judghcount[3] == 0)read[6]++;
	G[0] = _wfopen_s(&fp, L"save/data.dat", L"wb");
	fwrite(&read, sizeof(int), 7, fp);
	fclose(fp);
	//�L�����v���C���ۑ�
	G[0] = _wfopen_s(&fp, L"save/chap.dat", L"rb");
	if (G[0] == 0) {
		fread(&chap, sizeof(chap), 3, fp);
		fclose(fp);
	}
	chap[system[0]]++;
	G[0] = _wfopen_s(&fp, L"save/chap.dat", L"wb");
	fwrite(&chap, sizeof(int), 3, fp);
	fclose(fp);
	//���[�g�v�Z(level0�Ȃ�0�Œ�)"���x��" - "miss��" x "���x��" x 0.03(����=0)
	if (Lv == 0) rate = 0;
	else if (judghcount[3] > 0) {
		rate = Lv - judghcount[3] * Lv*0.03;
		rate = mins(rate, 0);
	}
	//NO MISS,"���x��" + 1 - "safe��" x 0.05(����="���x��")
	else if (judghcount[3] == 0 && judghcount[2] > 0) {
		rate = Lv + 1 - judghcount[2] * 0.05;
		rate = mins(rate, Lv);
	}
	//FULL COMBO,"���x��" + 2 - "good��" x 0.01(����="���x��" + 1)
	else if (judghcount[3] == 0 && judghcount[2] == 0 && judghcount[1] > 0) {
		rate = Lv + 2 - judghcount[1] * 0.01;
		rate = mins(rate, Lv + 1);
	}
	//PERFECT, "���x��" + 2
	else if (judghcount[3] == 0 && judghcount[2] == 0 && judghcount[1] == 0) rate = Lv + 2;
	//���[�g�ۑ�
	G[0] = _wfopen_s(&fp, L"save/rateS.dat", L"rb");
	if (G[0] == 0) {
		fread(&savec, 255, 10, fp);
		fclose(fp);
	}
	G[0] = _wfopen_s(&fp, L"save/rateN.dat", L"rb");
	if (G[0] == 0) {
		fread(&readR, sizeof(readR), 10, fp);
		fclose(fp);
	}
	G[0] = -1;
	//�����ȁA�܂��͖����^��T��
	for (i[0] = 0; i[0] < 10; i[0]++)if (strands(fileN, savec[i[0]]) || savec[i[0]] == '\0') {
		G[0] = i[0];
		break;
	}
	//�Ȃ�������A��ԒႢ���[�g��T��
	if (G[0] == -1) {
		G[0] = 0;
		for (i[0] = 1; i[0] < 10; i[0]++)if (readR[G[0]] > readR[i[0]]) G[0] = i[0];
	}
	//���[�g������������X�V����
	if (readR[G[0]] < rate) {
		readR[G[0]] = rate;
		strcopy(fileN, savec[G[0]], 1);
	}
	G[0] = _wfopen_s(&fp, L"save/rateS.dat", L"wb");
	fwrite(&savec, 255, 10, fp);
	fclose(fp);
	G[0] = _wfopen_s(&fp, L"save/rateN.dat", L"wb");
	fwrite(&readR, sizeof(double), 10, fp);
	fclose(fp);
	//���U���g���
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	while (1) {
		double acc;
		ClearDrawScreen();
		acc = (judghcount[0] * 10000 + judghcount[1] * 9500 + judghcount[2] * 5500) / (notes*100.0);
		DrawGraph(0, 0, resultimg, TRUE);
		DrawGraph(460, 20, difberimg, TRUE);
		DrawString(100, 13, songN, Cr);
		DrawFormatString(150, 60, Cr, L"%d", judghcount[0]);
		DrawFormatString(150, 100, Cr, L"%d", judghcount[1]);
		DrawFormatString(150, 140, Cr, L"%d", judghcount[2]);
		DrawFormatString(150, 180, Cr, L"%d", judghcount[3]);
		DrawFormatString(180, 220, Cr, L"%d", Mcombo);
		DrawFormatString(280, 220, Cr, L"%d", notes);
		DrawFormatString(450, 90, Cr, L"%d", score2[3]);
		DrawFormatString(450, 170, Cr, L"%.2f", acc);
		if (gapa[1] == 0) gapa[1] = 1;
		DrawFormatString(510, 210, Cr, L"%2d", gapa[0] / gapa[1]);
		DrawFormatString(510, 235, Cr, L"%2d", gapa[2] / gapa[1] - gapa[0] * gapa[0] / gapa[1] / gapa[1]);
		if (score2[3] >= 98000) DrawGraph(95, 280, rankimg[0], TRUE);
		else if (score2[3] >= 95000) DrawGraph(140, 260, rankimg[1], TRUE);
		else if (score2[3] >= 90000) DrawGraph(140, 260, rankimg[2], TRUE);
		else if (score2[3] >= 85000) DrawGraph(140, 260, rankimg[3], TRUE);
		else if (score2[3] >= 80000) DrawGraph(140, 260, rankimg[4], TRUE);
		else DrawGraph(140, 260, rankimg[5], TRUE);
		if (drop == 1) DrawGraph(5, 420, coleimg[0], TRUE);
		if (drop == 0 && judghcount[3] > 0) DrawGraph(5, 420, coleimg[1], TRUE);
		if (judghcount[3] == 0 && judghcount[2] > 0) DrawGraph(5, 420, coleimg[2], TRUE);
		if (judghcount[3] == 0 && judghcount[2] == 0 && judghcount[1] > 0) DrawGraph(5, 420, coleimg[3], TRUE);
		if (judghcount[3] == 0 && judghcount[2] == 0 && judghcount[1] == 0) DrawGraph(5, 420, coleimg[4], TRUE);
		DrawGraph(336, 252, Rchaimg, TRUE);
		ScreenFlip();
		//�G���^�[�������ꂽ
		if (CheckHitKey(KEY_INPUT_RETURN) == 1) break;
		else if (GetWindowUserCloseFlag(TRUE)) return 5;
	}
	InitGraph();
	return 2;
}

int collection(void) {
	int key = 1, command = 0, end = 0;
	int backimg, icon[4], G[2], help, sel;
	unsigned int Cr;
	Cr = GetColor(255, 255, 255);
	backimg = LoadGraph(L"picture/COLLECT back.png");
	icon[0] = LoadGraph(L"picture/CLI.png");
	icon[1] = LoadGraph(L"picture/CLIG.png");
	icon[2] = LoadGraph(L"picture/CLSG.png");
	icon[3] = LoadGraph(L"picture/CLS.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.ogg");
	int	lan[5] = { 0,0,0,2,0 };//�g���̂�[4,����]����
	FILE *fp;
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&lan, sizeof(lan), 5, fp);
		fclose(fp);
	}
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		DrawGraph(70, 130, icon[0 + command], TRUE);
		DrawGraph(350, 130, icon[2 + command], TRUE);
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"���E�L�[:�I��   Enter�L�[:����   BackSpace�L�[:�߂�", Cr);
		if (lan[4] == 1)DrawString(5, 460, L"����key:select   Enter key:enter   BackSpace key:back to menu", Cr);
		ScreenFlip();
		//��������L�[����
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//���������ꂽ
			if (key == 0) {
				command--;
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//�E�������ꂽ
			if (key == 0) {
				command++;
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN)) {
			//�G���^�[�������ꂽ
			if (key == 0) {
				switch (command) {
				case 0:
					end = C_item();
					break;
				case 1:
					end = C_story();
					break;
				}
			}
			if (end == 5) {
				return 5;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK) == 1) {
			//�o�b�N�X�y�[�X�������ꂽ
			if (key == 0) {
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			//����{�^���������ꂽ
			return 5;
		}
		else {
			//����̃L�[��������Ă��Ȃ�
			key = 0;
		}
		if (command < 0) {
			command += 2;
		}
		if (command > 1) {
			command -= 2;
		}
	}
	return 1;
}

int C_item(void) {
	//���[�h
	int e, i, j, win, cover, backimg, item[11], G[2], help, sel;
	int flag[10] = { 0,0,0,0,0,0,0,0,0,0 };
	double Grate = 0;
	int	play[7];
	double rate[10];
	int	lan[5] = { 0,0,0,2,0 };//�g���̂�[4,����]����
	FILE *fp;
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&lan, sizeof(lan), 5, fp);
		fclose(fp);
	}
	win = LoadGraph(L"picture/item�g.png");
	cover = LoadGraph(L"picture/item�J�o�[.png");
	backimg = LoadGraph(L"picture/COLLECT back.png");
	item[0] = LoadGraph(L"picture/item0.png");
	item[1] = LoadGraph(L"picture/item1.png");
	item[2] = LoadGraph(L"picture/item2.png");
	item[3] = LoadGraph(L"picture/item3.png");
	item[4] = LoadGraph(L"picture/item4.png");
	item[5] = LoadGraph(L"picture/item5.png");
	item[6] = LoadGraph(L"picture/item6.png");
	item[7] = LoadGraph(L"picture/item7.png");
	item[8] = LoadGraph(L"picture/item8.png");
	item[9] = LoadGraph(L"picture/item9.png");
	item[10] = LoadGraph(L"picture/item10.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.ogg");
	e = _wfopen_s(&fp, L"save/data.dat", L"rb");
	if (e == 0) {
		fread(&play, sizeof(play), 7, fp);
		fclose(fp);
	}
	e = _wfopen_s(&fp, L"save/rateN.dat", L"rb");
	if (e == 0) {
		fread(&rate, sizeof(rate), 10, fp);
		fclose(fp);
	}
	for (e = 0; e < 10; e++) if (rate[e] >= 0 && rate[e] <= 15) Grate += rate[e] * 1.0;
	//�B���t���O
	if (play[1] >= 1) flag[0] = 1;
	if (play[3] >= 1) flag[1] = 1;
	if (play[4] >= 1) flag[2] = 1;
	if (play[5] >= 1) flag[3] = 1;
	if (play[6] >= 1) flag[4] = 1;
	if (play[0] >= 1) flag[5] = 1;
	if (play[0] >= 10) flag[6] = 1;
	if (Grate >= 25) flag[7] = 1;
	if (Grate >= 55) flag[8] = 1;
	if (Grate >= 90) flag[9] = 1;
	//����
	int key = 1;
	int	command[2] = { 0,0 };
	unsigned int Cr;
	Cr = GetColor(255, 255, 255);
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		for (i = 0; i < 2; i++) {
			for (j = 0; j < 3; j++) {
				DrawGraph(110 + j * 150, 20 + i * 240, win, TRUE);
				if (flag[j + i * 7]) {
					DrawGraph(113 + j * 150, 23 + i * 240, item[1 + j + i * 7], TRUE);
				}
				else {
					DrawGraph(113 + j * 150, 23 + i * 240, item[0], TRUE);
				}
				if (j + i * 7 != (7 * command[1] + 2 * command[0]) / 2) {
					DrawGraph(110 + j * 150, 20 + i * 240, cover, TRUE);
				}
			}
		}
		for (i = 0; i < 4; i++) {
			DrawGraph(40 + i * 150, 140, win, TRUE);
			if (flag[3 + i]) DrawGraph(43 + i * 150, 143, item[4 + i], TRUE);
			else DrawGraph(43 + i * 150, 143, item[0], TRUE);
			if (3 + i != 3 * command[1] + command[0] || command[1] != 1) DrawGraph(40 + i * 150, 140, cover, TRUE);
		}
		switch (command[1]) {
		case 0:
			switch (command[0]) {
			case 0:
				if (play[1] >= 1 && lan[4] == 0)DrawString(50, 400, L"���ꂽ���R�[�h:\n�Ռ��ɑς���ꂸ�Ɋ���Ă��܂������́B\n�����g���Ȃ��c�B[1��ȏ�DROPED]", Cr);
				else if (play[1] >= 1 && lan[4] == 1)DrawString(50, 400, L"Broken Record Disc:\nThe broken record disc by shock.\nIt cannot be used... [Finish by DROPED]", Cr);
				else if (play[1] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1��ȏ�DROPED]", Cr);
				else if (play[1] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by DROPED]", Cr);
				break;
			case 1:
				if (play[3] >= 1 && lan[4] == 0)DrawString(50, 400, L"�T�E���h���b�v:\n���A�l�C�̂��َq�A�}�b�v�Q�[�^�[�����������Ă���B\n1��13pop(110�~)�B[1��ȏ�CLEARED]", Cr);
				else if (play[3] >= 1 && lan[4] == 1)DrawString(50, 400, L"Soundrop:\nThe popular candy now.\nMapgator have it on his bag. [Finish by CLEARED]", Cr);
				else if (play[3] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1��ȏ�CLEARED]", Cr);
				else if (play[3] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by CLEARED]", Cr);
				break;
			case 2:
				if (play[4] >= 1 && lan[4] == 0)DrawString(50, 400, L"�~���N:\n�������myumyu���������Ă���~���N�B�����ς肵�����킢���B\n�s�b�J�[�̍D���B[1��ȏ�NO MISS]", Cr);
				else if (play[4] >= 1 && lan[4] == 1)DrawString(50, 400, L"Milk:\nThe milk made by company \"myumyu\". It have tidy taste.\nPicker like it. [Finish by NO MISS]", Cr);
				else if (play[4] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1��ȏ�NO MISS]", Cr);
				else if (play[4] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by NO MISS]", Cr);
				break;
			}
			break;
		case 1:
			switch (command[0]) {
			case 0:
				if (play[5] >= 1 && lan[4] == 0)DrawString(50, 400, L"���b�t��:\n���ׂẲ��𒲘a�����Ƃ��ɂł��錋���B\n�ƂĂ������������A���̍D�݂��������炵���B[1��ȏ�FULL COMBO]", Cr);
				else if (play[5] >= 1 && lan[4] == 1)DrawString(50, 400, L"Waffle:\nThe crystal produced by harmoning all sound.It's very good \ntaste but some people feel that a bad. [Finish by FULL COMBO]", Cr);
				else if (play[5] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1��ȏ�FULL COMBO]", Cr);
				else if (play[5] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by FULL COMBO]", Cr);
				break;
			case 1:
				if (play[6] >= 1 && lan[4] == 0)DrawString(50, 400, L"�p�t�F:\n���������ɒ��a�����Ƃ��ɂł��錋���B\n�e�C���[�̑�D���B�Ƃ��Ă����܂��B[1��ȏ�PERFECT]", Cr);
				else if (play[6] >= 1 && lan[4] == 1)DrawString(50, 400, L"Parfait:\nThe parfait produced by harmoning perfectly the sound.\nTaylor is love it. It's very sweet. [Finish by PERFECT]", Cr);
				else if (play[6] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1��ȏ�PERFECT]", Cr);
				else if (play[6] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by PERFECT]", Cr);
				break;
			case 2:
				if (play[0] >= 1 && lan[4] == 0)DrawString(50, 400, L"�r�M�i�[�����i�[�ؖ��J�[�h:\n�����i�[�ł��邱�Ƃ��ؖ�������́B\n�������΂���̏��S�҂���̏؁B[1��ȏ�v���C]", Cr);
				else if (play[0] >= 1 && lan[4] == 1)DrawString(50, 400, L"Beginner Runner Card:\nThe card indicating that you are a Runner.\nAnd the beginner just joined. [Play the game]", Cr);
				else if (play[0] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1��ȏ�v���C]", Cr);
				else if (play[0] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Play the game]", Cr);
				break;
			case 3:
				if (play[0] >= 10 && lan[4] == 0)DrawString(50, 400, L"�����i�[�蒠:\n�����i�[�̋L�^�蒠�B���㒲����񍐂�����ōς܂���B\n����Ȃ��烁������㋉�҂�����B[10��ȏ�v���C]", Cr);
				else if (play[0] >= 10 && lan[4] == 1)DrawString(50, 400, L"Runner's Note:\nRunner's comprehensive note. Job reports are written on this.\nSome experts write while running. [Play the game 10 times]", Cr);
				else if (play[0] < 10 && lan[4] == 0)DrawString(50, 400, L"???\n[10��ȏ�v���C]", Cr);
				else if (play[0] < 10 && lan[4] == 1)DrawString(50, 400, L"???\n[Play the game 10 times]", Cr);
				break;
			}
			break;
		case 2:
			switch (command[0]) {
			case 0:
				if (Grate >= 25 && lan[4] == 0)DrawString(50, 400, L"�u�����Y�����i�[�ؖ��J�[�h:\n�����i�[�Ɋ���Ă����l�ɑ�����J�[�h�B\n����ł���ƃ����i�[�Ƃ��ĔF�߂���B[���[�g25.00�B��]", Cr);
				else if (Grate >= 25 && lan[4] == 1)DrawString(50, 400, L"Bronze Runner Card:\nThe card given to Runner who are used to work.\nYou are admitted as the Runner. [Reach 25.00 rate]", Cr);
				else if (Grate < 25 && lan[4] == 0)DrawString(50, 400, L"???\n[���[�g25.00�B��]", Cr);
				else if (Grate < 25 && lan[4] == 1)DrawString(50, 400, L"???\n[Reach 25.00 rate]", Cr);
				break;
			case 1:
				if (Grate >= 55 && lan[4] == 0)DrawString(50, 400, L"�V���o�[�����i�[�ؖ��J�[�h:\n�����i�[�Ƃ��ď\�����т��������l�ɑ�����J�[�h�B\n����d��������悤�ɂȂ�B[���[�g55.00�B��]", Cr);
				else if (Grate >= 55 && lan[4] == 1)DrawString(50, 400, L"Silver Runner Card:\nThe card given to great Runner.\nDifficult job will come to you. [Reach 55.00 rate]", Cr);
				else if (Grate < 55 && lan[4] == 0)DrawString(50, 400, L"???\n[���[�g55.00�B��]", Cr);
				else if (Grate < 55 && lan[4] == 1)DrawString(50, 400, L"???\n[Reach 55.00 rate]", Cr);
				break;
			case 2:
				if (Grate >= 90 && lan[4] == 0)DrawString(50, 400, L"�S�[���h�����i�[�ؖ��J�[�h:\n�����i�[���ɂ߂��l�ɑ�����J�[�h�B\n���E�e�n�̂��Ȃ����d��������悤�ɂȂ�B[���[�g90.00�B��]", Cr);
				else if (Grate >= 90 && lan[4] == 1)DrawString(50, 400, L"Gold Runner Card:\nThe card given to EXCELLENT Runner. More difficult \njob all over the world will come to you. [Reach 90.00 rate]", Cr);
				else if (Grate < 90 && lan[4] == 0)DrawString(50, 400, L"???\n[���[�g90.00�B��]", Cr);
				else if (Grate < 90 && lan[4] == 1)DrawString(50, 400, L"???\n[Reach 90.00 rate]", Cr);
				break;
			}
			break;
		}
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"�㉺���E�L�[:�I��   BackSpace�L�[:�߂�", Cr);
		else if (lan[4] == 1)DrawString(5, 460, L"��������key:select   BackSpace key:back", Cr);
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//���������ꂽ
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[0]--;
				if (command[0] < 0) {
					command[0] = 3;
				}
				if (command[0] == 3 && command[1] == 0) {
					command[0] = 2;
				}
				if (command[0] == 3 && command[1] == 2) {
					command[0] = 2;
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//�E�������ꂽ
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[0]++;
				if (command[0] > 3) {
					command[0] = 0;
				}
				if (command[0] == 3 && command[1] == 0) {
					command[0] = 0;
				}
				if (command[0] == 3 && command[1] == 2) {
					command[0] = 0;
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_UP)) {
			//�オ�����ꂽ
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[1]--;
				if (command[0] == 3 && command[1] == 0) {
					command[0] = 2;
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN)) {
			//���������ꂽ
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[1]++;
				if (command[0] == 3 && command[1] == 2) {
					command[0] = 2;
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK)) {
			//�o�b�N�X�y�[�X�������ꂽ
			if (key == 0) {
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			//����{�^���������ꂽ
			return 5;
		}
		else {
			//����̃L�[��������Ă��Ȃ�
			key = 0;
		}
		if (command[1] < 0) {
			command[1] = 2;
		}
		if (command[1] > 2) {
			command[1] = 0;
		}
	}
	return 0;
}

int C_story(void) {
	int e, i, j, end = 0, key = 1, backimg, noteimg, help, sel, chac[3] = { 0,0,0 }, sub1[6], command[2] = { 0,0 }, g[7];
	wchar_t chan[4][40] = { L"�s�b�J�[",L"�}�b�v�Q�[�^�[",L"�e�C���[",L"�T�u�X�g�[���[1" };
	wchar_t chanE[4][40] = { L"Picker",L"Mapgator",L"Taylor",L"Sub Story 1" };
	unsigned int Cr, Crw;
	int	lan[5] = { 0,0,0,2,0 };//�g���̂�[4,����]����
	FILE *fp;
	e = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (e == 0) {
		fread(&lan, sizeof(lan), 5, fp);
		fclose(fp);
	}
	//�L�����v���C�񐔓ǂݍ���
	e = _wfopen_s(&fp, L"save/chap.dat", L"rb");
	if (e == 0) {
		fread(&chac, sizeof(chac), 3, fp);
		fclose(fp);
	}
	//�T�u�X�g1�̃X�R�A�ǂݍ���
	for (i = 0; i < 6; i++) {
		switch (i) {
		case 0:
			e = _wfopen_s(&fp, L"score/snow town story.dat", L"rb");
			g[6] = 1;
			break;
		case 1:
			e = _wfopen_s(&fp, L"score/�O���f�[�V�����E���[���h.dat", L"rb");
			g[6] = 1;
			break;
		case 2:
			e = _wfopen_s(&fp, L"score/What Color Is The Sky�H.dat", L"rb");
			g[6] = 1;
			break;
		case 3:
			e = _wfopen_s(&fp, L"score/�g���m����.dat", L"rb");
			g[6] = 2;
			break;
		case 4:
			e = _wfopen_s(&fp, L"score/�g���m����.dat", L"rb");
			g[6] = 4;
			break;
		case 5:
			e = _wfopen_s(&fp, L"score/�g���m����.dat", L"rb");
			g[6] = 5;
			break;
		}
		sub1[i] = 0;
		if (e == 0) {
			fread(&g, sizeof(g), 6, fp);
			fclose(fp);
			for (j = g[6]; j <= 5; j++) sub1[i] = mins(sub1[i], g[j]);
		}
		switch (i) {
		case 0:
			e = _wfopen_s(&fp, L"score/snow town story.dat", L"rb");
			g[6] = 1;
			break;
		case 1:
			e = _wfopen_s(&fp, L"score/Gradation-world.dat", L"rb");
			g[6] = 1;
			break;
		case 2:
			e = _wfopen_s(&fp, L"score/What Color Is The Sky.dat", L"rb");
			g[6] = 1;
			break;
		case 3:
			e = _wfopen_s(&fp, L"score/Torinoyume.dat", L"rb");
			g[6] = 2;
			break;
		case 4:
			e = _wfopen_s(&fp, L"score/Torinoyume.dat", L"rb");
			g[6] = 4;
			break;
		case 5:
			e = _wfopen_s(&fp, L"score/Torinoyume.dat", L"rb");
			g[6] = 5;
			break;
		}
		if (e == 0) {
			fread(&g, sizeof(g), 6, fp);
			fclose(fp);
			for (j = g[6]; j <= 5; j++) sub1[i] = mins(sub1[i], g[j]);
		}
	}
	Cr = GetColor(0, 0, 0);
	Crw = GetColor(255, 255, 255);
	backimg = LoadGraph(L"picture/COLLECT back.png");
	noteimg = LoadGraph(L"picture/Cnote.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.ogg");
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		DrawGraph(160, 0, noteimg, TRUE);
		if (lan[4] == 0)DrawFormatString(330, 220, Cr, L"%s", chan[command[1]]);
		else if (lan[4] == 1)DrawFormatString(330, 220, Cr, L"%s", chanE[command[1]]);
		if (command[1] < 3) {
			if (command[0] == 0) {
				if (chac[command[1]] >= 1) {
					if (lan[4] == 0)DrawString(300, 245, L"�G�s�\�[�h1", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 1", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawFormatString(300, 245, Cr, L"%s���g����\n1��v���C���悤�B", chan[command[1]]);
					else if (lan[4] == 1)DrawFormatString(300, 245, Cr, L"Play the game\nwith %s.", chanE[command[1]]);
					e = 0;
				}
			}
			else if (command[0] == 1) {
				if (chac[command[1]] >= 2) {
					if (lan[4] == 0)DrawString(300, 245, L"�G�s�\�[�h2", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 2", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawFormatString(300, 245, Cr, L"%s���g����\n2��v���C���悤�B", chan[command[1]]);
					else if (lan[4] == 1)DrawFormatString(300, 245, Cr, L"Play the game\n2 times with %s.", chanE[command[1]]);
					e = 0;
				}
			}
			else {
				if (chac[command[1]] >= command[0] * 3 - 2) {
					if (lan[4] == 0)DrawFormatString(300, 245, Cr, L"�G�s�\�[�h%d", command[0] + 1);
					else if (lan[4] == 1)DrawFormatString(300, 245, Cr, L"Episode %d", command[0] + 1);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawFormatString(300, 245, Cr, L"%s���g����\n%d��v���C���悤�B", chan[command[1]], command[0] * 3 - 2);
					else if (lan[4] == 1)DrawFormatString(300, 245, Cr, L"Play the game\n%d times with %s.", command[0] * 3 - 2, chanE[command[1]]);
					e = 0;
				}
			}
		}
		else if (command[1] == 3) {
			switch (command[0]) {
			case 0:
				if (sub1[command[0]] >= 90000) {
					if (lan[4] == 0)DrawString(300, 245, L"�G�s�\�[�h1", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 1", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"snow town story[EASY]�ȏ��\nA�����N����낤", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath A RANK at\nsnow town story[EASY] or more dif.", Cr);
					e = 0;
				}
				break;
			case 1:
				if (sub1[command[0]] >= 90000) {
					if (lan[4] == 0)DrawString(300, 245, L"�G�s�\�[�h2", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 2", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"�O���f�[�V�����E���[���h[EASY]�ȏ��\nA�����N����낤", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath A RANK at\nGradation-world[EASY] or more dif.", Cr);
					e = 0;
				}
				break;
			case 2:
				if (sub1[command[0]] >= 95000) {
					if (lan[4] == 0)DrawString(300, 245, L"�G�s�\�[�h3", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 3", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"What Color Is The Sky?[EASY]�ȏ��\nS�����N����낤", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath S RANK at\nWhat Color Is The Sky?[EASY] or more dif.", Cr);
					e = 0;
				}
				break;
			case 3:
				if (sub1[command[0]] >= 90000) {
					if (lan[4] == 0)DrawString(300, 245, L"�G�s�\�[�h4", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 4", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"�g���m����[NOAMAL]�ȏ��\nA�����N����낤", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath A RANK at\nTorinoyume[NOAMAL] or more dif.", Cr);
					e = 0;
				}
				break;
			case 4:
				if (sub1[command[0]] >= 98000) {
					DrawString(300, 245, L"appendix-1", Cr);
					e = 1;
				}
				else if (sub1[5] >= 1) {
					if (lan[4] == 0)DrawString(300, 245, L"�g���m����[HOPENESS]��\nEX�����N�����", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath EX RANK at\nTorinoyume[HOPENESS].", Cr);
					e = 0;
				}
				else if (sub1[3] >= 90000) {
					if (lan[4] == 0)DrawString(300, 245, L"�g���m��x����M_^��]��\nEX�����N�����", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath EX RANK at\nTorim.v?&1/0)\\$cRD].", Cr);
					e = 0;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"�����ؗ�M_^��]\ne\\ny�~����D", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Rns(}1)Op&rrZ\ne\\ny456Ge/t0Z<=00ax(Q).", Cr);
					e = 0;
				}
				break;
			}
		}
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"�㉺���E�L�[:�I��   Enter�L�[:����   BackSpace�L�[:�߂�", Crw);
		else if (lan[4] == 1)DrawString(5, 460, L"��������key:select   Enter key:enter   BackSpace key:back", Crw);
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//���������ꂽ
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[1]--;
				command[0] = 0;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//�E�������ꂽ
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[1]++;
				command[0] = 0;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_UP)) {
			//�オ�����ꂽ
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[0]--;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN)) {
			//���������ꂽ
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[0]++;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN)) {
			//�G���^�[�������ꂽ
			if (key == 0 && e == 1) end = story(command[0], command[1]);
			if (end == 5) return 5;
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK)) {
			//�o�b�N�X�y�[�X�������ꂽ
			if (key == 0) {
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
		//����{�^���������ꂽ
		else if (GetWindowUserCloseFlag(TRUE)) return 5;
		//����̃L�[��������Ă��Ȃ�
		else key = 0;
		if (command[0] < 0) command[0] = 4;
		if (command[0] > 4) command[0] = 0;
		if (command[1] < 0) command[1] = 3;
		if (command[1] > 3) command[1] = 0;
	}
	return 0;
}

int story(int a, int b) {
	int key = 1, com = 0, Cx = 220, Cy = 75, backimg, noteimg, pageimg, help, sel;
	unsigned int Cr, Crw;
	Cr = GetColor(0, 0, 0);
	Crw = GetColor(255, 255, 255);
	backimg = LoadGraph(L"picture/COLLECT back.png");
	noteimg = LoadGraph(L"picture/Snote.png");
	pageimg = LoadGraph(L"picture/page.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.ogg");
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		DrawGraph(160, 0, noteimg, TRUE);
		//������1�s20�����A1�y�[�W20�s�A3�y�[�W�܂ŁB
		switch (10 * a + 1000 * b + com) {
			//�s�b�J�[
		case 0:
			DrawString(Cx, Cy, L"�u�s�����≹�y�����w�Z�O�v\n\n�@�{�N������Ă���o�X�́A���̃o�X���\n�O�Ŏ~�܂����B�{�N�͐Ȃ���~��āA����\n�o�X��ō~���w���ɕ���āA�^����������\n�O���֌��������B�^���Ɛ����������肵�߂�\n����E�肪�A�S�Ȃ����������k���Ă���\n�悤�ȋC�������B���̌����́A�ْ��Ȃ̂��A\n���҂Ȃ̂��A�悭�킩��Ȃ������B\n�@���ꂵ���o�X����~��āA���񂾋�C��\n�[������O�֏o���B�����ă{�N�́A�w�Z��\n�������w���Ƃ͔��Ε����֕���i�߂��B\n�{�N�͊w�Z�ɗp������񂶂�Ȃ��B\n�ڎw���́A���̊w�Z�̗��ɂ���A�u�����i�[\n����������x���v���B", Cr);
			break;
		case 1:
			DrawString(Cx, Cy, L"�@���̐��E�ł́A�����`�ɕς��Ĕ��鏤����\n���񂾁B���َq�ɂ�����A���������\n������A�傫���Ƃ��낾�Ɣ��d�ɂ��g����\n����񂾂Ƃ��B�����`�ɂ���Z�p���l����\n�l�́A���E�I�ȕ\�����󂯂��̂��Ƃ��B����\n��ȁ[�A���������l�́B�{�N�Ȃ񂩃h�b�a\n�{�[���ƃs�A�m���炢������蕿���Ȃ���\n������ȁ[�B�����ƁA�{�N�̂��Ƃ�\n�u���Ƃ��āc�B\n�@����Ȓ��A����ς舫�����Ƃ��l����\n���������̂��B�����`�ɂ���Z�p��\n�g���āA�ƍ߂⋭���A�����������Ƃ��낶��\n�e���ɂ��g���Ă���炵���B�{���Ȃ�\n�x�@�������񂾂��ǁA���y�𑊎�ɂł���\n�x�@�͂Ȃ��Ȃ����Ȃ��炵���B\n�@�����ŁA�����i�[�̏o�Ԃ��B�����i�[��\n���y�̐��E�ƁB���y�Ɋւ��邿����Ƃ���\n���育�Ƃ��K�͂Ȏ����܂ŁA���ł����\n���܂�̂������i�[���B", Cr);
			break;
		case 2:
			DrawString(Cx, Cy, L"�@�{�N�̂���������A�����i�[�̎d����\n���Ă����B���\���炢�n�ʂɗ����Ă���\n�炵���B����ዋ�����o�J�݂����ɍ�������\n�炵���B�����i�[�����߂Ă��V��ŕ�点��\n���炢�̋����������炵�����ǁA���������\n�����i�[�����߂Ȃ������B\n�@����Ȃ������񂪍s���s���ɂȂ����̂́A\n��̂R�N�O�̂��Ƃ������B�{���ɋ}�������B\n�d����Œʘb���؂�Ă��܂��A���������\n�炵���B���̎���������́A�����g����\n���d���̖\�����~�߂ɍs���Ă����炵���B\n�ʘb���؂ꂽ��A�����ɋ삯�t��������\n�����i�[�ɂ��ƁA��̂ǂ��납�A�g��\n�����Ă������̂��ׂĂ�������Ȃ�����\n�炵���B�����͐�]�I���������ǁA\n�����������琶���Ă���̂����H\n�@�����i�[�����ɂ����i���āA����ƏA�E����\n���Ƃ��ł����B�P���Ƀ����i�[�ɂȂ��Ă݂���\n�Ƃ����̂����邯�ǁA��Ԃ̖ړI�͂�������\n�T���B�ʂĂ��Ȃ��ڕW�����ǁA����΂邼�B", Cr);
			break;
		case 10:
			DrawString(Cx, Cy, L"�u�����i�[����������x���v\n�Q�K���Ă̌����̔��̉��ɁA����������\n����B�ԈႢ�Ȃ��A�������������̂悤���B\n�����ƁA����ɓ����Ă����̂��ȁc�H\n����Ƃ��A�ǂ����ɃC���^�[�z������������\n���āc�H�����l���Ă���Ɓc�A\n�u�����A�ǂ��������肭�������[��v\n�ƂĂ��y��������ɂ���X�s�[�J�[����\n���ꂽ�B�悭������J�����܂ł���B\n�����Ă��c�B�p�������[�B\n�@�Ƃ肠�����A���������Ē��ɓ������B\n���͎v������苷���B�����Đl�����Ȃ��B\n�u�����ƁA�V�l����ł���ˁH�������\n���̂܂܂ł��҂����������[��v\n�܂��������̐����B���x�͂ǂ��ɃJ������\n����񂾂낤�c�H�����v�����҂��Ƃ�\n�����B", Cr);
			break;
		case 11:
			DrawString(Cx, Cy, L"�@���΂炭���āA\n�u���[�I���O���V�l���[�I�v�������ׂ���\n���Ă�ȁ[�v\n�߂��ɂ������K�i����~��Ă������̉��F��\n���j�́A�{�N������Ȃ肢���Ȃ肱���\n���Ƃ������Ă����B\n�u�ق��Ƃ��Ă��������c�v\n�΂��邱�̃��j�́A�r�������̂������āA\n�X�|�[�c�I��ł�����Ă��������ȑ̂�����\n����B�����A�܂����B\n�u���̓}�b�v�Q�[�^�[�B��������Q�[�^�[��\n�������B���O�́H�v\n�u�s�b�J�[�ł��v\n�u�s�b�J�[�ȁA�悵�o�����B�������A\nLINE�Ƃ�����Ă�H�v\n�u(����������)�Ȃ��ł��v\n�u�������A���ꂶ��s�b�`���[�A��������\n���������ē����邺�v\n���O�o����ĂȂ������c�B���͗ǂ�\n�Ȃ������B", Cr);
			break;
		case 12:
			DrawString(Cx, Cy, L"�@�Q�[�^�[�ɘA����āA�{�N�͎�������\n�Q�K�ɂ���Ă����B�P�K�ɂ͎�t��ҍ�����\n���������ǁA�����ɂ̓����i�[�̑ҋ@������\n��c���A����Ƀg���[�j���O���[���܂�\n�������B\n�u�����i�[�͉��y�����Ă悭�������ǁA\n�̗͂�����񂾂��B�����ł��������̒b��\n�Ƃ���A�s�b�`���[�v\n�u�s�b�J�[�ł��v\n���āA��������Ŏ������͑S�����ĉ����\n���ȁH�����v���Ă�ƁB\n�u����A���͒n�����B���ė����v\n�����ƁA�������Ēn���ɂ�����������񂾁B\n�v�������L���񂾂ȁ[�B\n\n���A�҂��āA���������΁c�B\n\n���̎������ɓ����Ă���A�Q�[�^�[�ȊO��\n����Ă��Ȃ��c�B", Cr);
			break;
		case 20:
			DrawString(Cx, Cy, L"�@�n���ɂ������̂́A�n��̕��͋C�Ƃ͑S��\n�Ⴄ�A�@�B���炯�̕����������B�����̕ǂ�\n�����Ă��낢��ȃ��{�b�g������ł��āA\n�����̕Ћ��ɂ͂T�قǃ��j�^�[�������\n�����B\n�u�s�[�^�[�A��������Ă݂�v\n�u�s�b�J�[�ł��B�Ȃ�ł����H�v\n�Q�[�^�[���w��������ɂ������̂́A�Z��\n�t�����Ԃ��h���S���̂悤�ȃ��{�b�g\n�������B\n�u�߂Â��Ă�[�����Ă݂�B��������v\n�c�Q�[�^�[���ɂ�ɂ₵�Ȃ��猾�����B\n���ȗ\�����������A������܂܂���\n���{�b�g�ɋ߂Â����݂��B����ƁA\n�u������I�H�v\n�ˑR���̃��{�b�g�������o�����B���Ă���\n�H��傫���L�΂��āA���ɕt���Ă����̂�\n�������N�����āA\n�u�₟�A�т����肵�������H�v\n�����������B�Ƃт�̑O�ŕ��������������B", Cr);
			break;
		case 21:
			DrawString(Cx, Cy, L"�u���ĂƁA���߂܂��āA�����i�[��������\n�悤�����I�{�N�������̊Ǘ��l�̃e�C���[\n����B��낵���ˁ�v\n�u��A��낵�����肢���܂��v\n�\�z�O�̏o�����ɂ�����Ƌ����Ă��܂����B\n�u�Q�[�^�[�ɉ���đ��X�A����Ƃ�������\n���肵���񂶂�Ȃ��H���߂�ˁA�ނ͈��C��\n�Ȃ��̂��v\n�u���A�܂��A�͂��v\n�u�������A�����Đ������Ȃ̂����{�b�g\n�Ȃ̂��悭�������񂾂��ǁA��ɓ�����\n�����ˁ􎄂͂��̓�̒��ԁB�܂�\n�T�C�{�[�O�Ȃ́B�܂��A�ʂɋC�ɂ��Ȃ��Ă�\n�����񂾂��ǂˁ�v\n�u�ւ��A�����Ȃ�ł����v\n���̃h���S���͂悭����ׂ�Ȃ��B", Cr);
			break;
		case 22:
			DrawString(Cx, Cy, L"�u�܂��A����ł����̏Љ�͏I��肾�ȁv\n�Q�[�^�[���������B\n�u���肪�Ƃ��������܂��B�Ƃ���ŁA����\n���đ��ɐl�͂��Ȃ���ł����H�v\n�u���Ȃ����B�����C�������B���Ƃ��Ƃ���\n�߂��͎�������������ȁB�������̏o�Ԃ�\n�قƂ�ǂȂ��̂��v\n�u���ꂶ��A�d�������܂�Ȃ��́H�v\n�u�����ł��Ȃ����B�ŋ߂͉�����ꂷ��\n�ĂāA������W�߂Ă܂��̂���������\n�d�����v\n���������΁A���ɉ��̂����炪�����Ă�\n���Ƃ�����Ȃ��B�������A�����i�[�����\n���Ă��̂��B\n�u���ꂶ��A��������Ă݂邩�H�e�C���[�A\n�T�|�[�g�ł����ȁH�v\n�u�ł��邯�ǁA�߂�ǂ�������v\n�u����σ_�����B����A�����T�|�[�g����\n��邩�v\n�������āA�����d�����n�܂����B", Cr);
			break;
		case 30:
			DrawString(Cx, Cy, L"�@�Q�[�^�[���������̒��ɂ������Ԃ��^�]\n���āA�N�����Ȃ��X�̒��֌��������B\n�Q�[�^�[�ɂ��ƁA�|�C�̂Ă��p�ɂ�\n�N���Ă�炵���B\n�u����ȂƂ���ł��������̂Ă�Ȃ��\n�悧�A�ǂ����̂Ă�Ȃ�X���Ŏ̂Ă��v\n�ƃQ�[�^�[����s�����ڂ��B����A�����\n����Ŗ�肾�Ǝv�����ǁB\n�u�|�C�̂Ă���������߂܂���́H�v\n�u������O���낧�H�ł��|�C�̂Ă��āA\n�����Ă�400pop(��3300�~)�̔����ōς�\n�܂��񂾁B�����Ȃ�˂��񂾂�Ȃ��A�|�C\n�̂āB�܂��A�{�[�i�X�o�邩�炢�����ǁv\n\n�@�����āA��������30����B�ړI�n��\n�������B", Cr);
			break;
		case 31:
			DrawString(Cx, Cy, L"�@�|�C�̂Ă������A�Ƃ͕��������ǁA����\n�Ƃ��낻��ȕ��͋C�͂Ȃ��A�ނ��낫�ꂢ\n�������B���̖������������邵�A���؂�\n���C�����������B\n�u�˂��A�{���Ƀ|�C�̂Ă���Ă�́H�v\n�{�N�͕������B\n�u���ꂢ�Ɍ����邾��H�ł�����Ă��\n�Ȃ��A���ꂪ�B�|�C�̂Ă����Ă������Ȃ�\n�ꏊ�Ɏ̂Ă邩��ȁB�܂��A�����i�[�ɂ�\n�o���o�������ǂȁv\n�u�c�����������́H�v\n�u�����猩����񂾂�v\n�Q�[�^�[�����������ĎԂ�����o����\n�̂́A���F�̃w�b�h�z���ƁA�~�Ղ���\n�t����DJ�L�b�g�BDJ�L�b�g�̓d����\n�����ƁA�u�D�D�[�[���A�Ƃ����������āA\n�L�b�g�����ɕ������B\n�u���āA�����瑖�邼�B�`�[�^�[�A����\n������I�v\n�u�s�b�J�[�ł��v", Cr);
			break;
		case 32:
			DrawString(Cx, Cy, L"�@�Q�[�^�[������o�����Ɠ����ɁADJ�L�b�g\n���特�y������Ă����B�J�̉��𒆐S�Ƃ���\n�ȁA���̐X�ɂ҂�����̋Ȃ������B�����āA\n���̋Ȃɍ��킹�邩�̂悤�ɁA���A���X�A\n���̓��������A����ɑ��؂܂ŁA���ꂼ���\n����t�Ŏn�߂��B����Ȃ��畷���Ă��āA\n�{�N�܂ŉ̂��Ă��܂������Ȃ�قǂ������B\n�S�R�m��Ȃ��Ȃ����ǁB\n�@����Ȏ��������B���̋Ȃ��Ղ邩�̂悤��\n�G�����ǂ����炩�������Ă����B�Q�[�^�[��\n�����������̏ꏊ����肵�A������\n���������B�����Ă����ɂ������̂́A�N����\n�̂Ă��ł��낤�A���̂����炾�����B������\n�Ȃ�Ƃ������o�����Ƃ��Ă���悤�����A\n���܂��o���Ă��Ȃ��āA���ꂪ�G���ɂȂ���\n�������Ă����̂��B\n�@�Q�[�^�[�͉��t���Ȃ����p�ɉ�����āA\n�܂����̏ꏊ�֌������B���̋Ȃ��I���\n�܂ŁA�����Ƃ���̌J��Ԃ��������B\n���ꂪ�A�����i�[�̎d���c�B", Cr);
			break;
		case 40:
			DrawString(Cx, Cy, L"�u�I��������A�ς��Ȃ����H�v\n�u����ꂳ��B�������͖��Ȃ����v\n���t���I����āA���Ȃ�̉��̂������\n�W�߂��{�N�����́A�����Ɏ������ɋA�����B\n�W�߂����̂�����́A���ꂩ��Ǝ҂ɔ���\n�炵���B\n�u���ꂾ���W�߂ꂽ��A������150pop\n(��1250�~)���炢�ɂȂ邩�ȁB�務���ˁ�v\n�u���[�g�́H�v\n�u�オ��Ȃ����v\n�u����ł��_�����悧�A�������Ȃ��c�v\n�u���ɂȂ����玄�𔲂��邩�ȁ[��v\n�u���Ă��A�����ɔ����Ă�邩��ȁv\n�b���Ă���Q�[�^�[�ƃe�C���[���悻�ɁA\n�{�N�͂���������Ă����B�������܂ő�����\n�����������炪�A���͐Â��ɂ��������\n�u���A���������Ăق����H���5pop(��40�~)\n�Ŕ�����v\n�u���������A����͂ڂ������肾�낗�v", Cr);
			break;
		case 41:
			DrawString(Cx, Cy, L"�u���������΃L�b�J�[�A���O�ڕW�Ƃ�����\n�̂��H�v\n�Q�[�^�[�������Ă����B\n�u�s�b�J�[�ł��B�ڕW�����A�Ƃɂ����A\n��������T�����������񂾂��ǁA�܂��ǂ�\n����΂����񂾂낤�H�v\n�u��������������i�[�Ȃ̂��H�v\n�u����Ȃ�A���O�Ƃ�������ΒT������v\n�e�C���[���������B\n�u���ꂶ��A���̕����ɗ��āB�ꏏ��\n�T���Ă������v\n�u���A�ł��A����ɍs���񂶂�Ȃ��́H�v\n�u����Ȃ̃Q�[�^�[�ɔC�����炢��\n�ł���������s��������v\n�{�N�̓e�C���[�ɘA����Ēn���ɍs�����B", Cr);
			break;
		case 42:
			DrawString(Cx, Cy, L"�u���[�ƁA�{���ɂ��̐l�Ȃ́H�v\n�u����A���������ǁv\n���j�^�[�ɉf���Ă���̂́A�{�N�̂�������\n�̊�ʐ^�ƁA�Ȃ񂩂��낢��ƕ����������\n�����B�������đS����ǂނ̂͂߂�ǂ������B\n�v����ɁA���낢����т������Ă��āA\n���Ȃ��̒n�ʂɂ��܂���I�Ȃ��Ƃ�������\n����B\n�@�����āA���̂ɑ����Ă���s���s�����Ƃ���\n���Ƃ������Ă���B\n�u���܂茾�������Ȃ����ǁA�S���Ȃ��Ă邩��\n����Ȃ���B����ł��T���̂����H�v\n�u����A���̂��߂Ƀ����i�[�ɂȂ�������v\n�T���ɍs�����ƂɈӖ�������B���Ȃ��Ƃ��A\n�{�N�͂����v���Ă�B\n\n�@�����A���ꂩ��撣�邼�B", Cr);
			break;
			//�Q�[�^�[
		case 1000:
			DrawString(Cx, Cy, L"�@�����i�[�������̒��B���͎��R�ɖڂ�\n�o�܂����B�������Ƒ̂��N�����āA�L�т�\n�����B�x�b�h����o�āA�����̃J�[�e����\n�J����B�O�͂܂��Â����A�����ɖ��邭�Ȃ�\n���낤�B��ɂ͂���ق�Ɖ_��������B����\n���Ɍ����钬�́A�܂��������薰���Ă���\n�悤���B\n�@�ɂ��Ă��A���̎��Ԃ̓}�W�ŐÂ����B\nSilentroom��GAMEBOY�̑O�t���炢�ɐÂ����B\n���Ɨ[��������Ő���オ���āA���\n�Ȃ�����܂��Â��ɂȂ��Ă����񂾂낤�ȁB\n��������Ă̂�т�l�������ł���̂́A\n���������a�����炾�낤�ȁB�̂񂫂Ȃ���\n�����A�����̐l�����́B\n\n�@���������Ă����B�ǂ������B�n���ŐQ�Ă�\n���������낻��N���邾�낤�B������\n���C�ɑ��邺�B", Cr);
			break;
		case 1001:
			DrawString(Cx, Cy, L"�u���͂�[����A�Q�[�^�[��B����������\n�����N���Ă��H�v\n�P�K�Ńe�C���[�Ƙb���Ă���B�����͔���\n�@�B������A���ԂɂقƂ�ǐ��m���B����\n�A�܂����B�����i�[�̓^�C�~���O���厖��\n�d��������ȁB���t�Ƀu�����Ȃ��̂͂��Ȃ�\n�L�����B�ł��܂��A���͂����܂ł��ċ@�B��\n�Ȃ肽���͂Ȃ����ǂȁB\n�u�����A�����͑��N���������B�ŁA������\n�d���͉����H�v\n�u�������|�C�̂ĉ�������B����͊C\n�����C�D���ł���H�v\n�u�܂��A�D�������ǁA�܂��S�~�������c�v\n�u�S�~���Č���Ȃ��́�v\n���̒��͉��y�ƍ߂��Ȃ�����ɁA�|�C\n�̂Ă������B�y�ł������A���������V���o�[\n�����i�[�ɂȂ����񂾂���A�����Ɣh���\n���Ƃ��������ȁB", Cr);
			break;
		case 1002:
			DrawString(Cx, Cy, L"�u�������A���������ɐV�l���񂪗�����\n�Q�[�^�[��y���~�������Č����Ă���ˁ�v\n�u���A�܂����I���������A�ǂ�Ȃ�H�v\n�V�l�̓}�W�ł��肪�Ă��A�����͉���\n�e�C���[��C�������Ȃ���������A�h����\n�d�����ł��Ȃ������񂾁B�������ɍŒ�\n��C�̃����i�[�����Ȃ���΂����Ȃ�����\n�������܂肪���邩��ȁB\n�u�Ƃ��Ă����������E�T�M���񂾂��v\n�u�����A���t���ӂ����Ȃ���ȁv\n�u���������ɂ��鉹�y�w�Z�o�g�����Ă���v\n�u���������炩���A���\�D�����H�v\n�u�����炵���ˁ�v\n�������𑲋Ƃ�����́A���������s���\n�����i�[�Ƃ��A�o���h�Ƃ���ȉƂƂ��ɍs��\n���񂾁B�V�l�͒n���œ����������Ă�������\n���ȁH�y���݂��B", Cr);
			break;
		case 1010:
			DrawString(Cx, Cy, L"�@���̓��A�����͐V�l������炵���B\n�߂�����y���݁B�ł��A�����̎d�����S�~\n��r�c�|�C�̂ĉ�����B����͐X���B�V�l��\n�d���������邽�߂ɁA���͐V�l������܂�\n�ҋ@���B�������Ȃ����ȁc�B�V�����b��\n���肪�ł���̂̓}�W�ł��ꂵ���B���A\n����A�e�C���[�Ƙb���̂��O�������Ă킯\n����Ȃ��āA�l�͑����ق�����������H\n�����Ăΐl�ɋ�����̂��D��������B\n�e�C���[����͋����Ă�����Ă΂���\n������B\n���͂��������畔���̑����玖������\n����������߂Ă���B�����e�C���[��\n�����̃��j�^�[���猩�߂Ă���񂾂낤\n�ȁB�Ƃ��v���Ă�����A�����悤���B", Cr);
			break;
		case 1011:
			DrawString(Cx, Cy, L"�@�悵�A��ɍs�����I������������o��\n�}������悤�Ƃ�����A\n�@�s�b�R���A�s�b�R���B\n�@�d�b�������B�^�C�~���O����������c�B\n����[�ˁ[�A�d�b�D�悾�B���͎�b���\n������B\n�u����ف�B�N���Ă�H�v\n�e�C���[�������c�B\n�u�N���Ă��A�Ȃ񂾁H�v\n�u�V�l���񗈂����v\n�c�c�c�c�c�c�c�c�c�c�B\n�u����A�m���Ă���āv\n�u���������_�C�i�[��B����A�R�����\n�P�K�ɗ��ā�v\n�Ȃ��R���H\n�u����A���Ƃ͂�낵���ˁ[��v\n�Ӗ���������Ȃ��܂ܓd�b���؂ꂽ�B\n�܂�������A�R����ɍs�����B", Cr);
			break;
		case 1012:
			DrawString(Cx, Cy, L"�@�R����B���͂P�K�ɍ~�肽�B�����ɂ�\n�����������V�l�������B���߂Č���ƁA����\n�����������A�ΐF�̃E�T�M�������B�v����\n���̂��ׂ��āA\n�u���[�I���O���V�l���[�I�v�������ׂ���\n���Ă�ȁ[�v\n���������肱��Ȃ��Ƃ������Ă��B\n�u�ق��Ƃ��Ă��������c�v\n�����ƁA�܂��͎��ȏЉ�����H\n�u���̓}�b�v�Q�[�^�[�B��������Q�[�^�[��\n�������B���O�́H�v\n�u�s�b�J�[�ł��v\n�u�s�b�J�[�ȁA�悵�o�����B�������A\nLINE�Ƃ�����Ă�H�v\n�ꉞ�A����������Ă������B���Ƃł����\n�v������B\n�u�c�Ȃ��ł��v\n�Ȃ��񂩂��B����[�c�A���͈ē����ȁB\n�u���ꂶ��s�b�`���[�A�������̎�������\n�ē����邺�v", Cr);
			break;
		case 1020:
			DrawString(Cx, Cy, L"�@���͂�����A��āA�Q�K�ɂ���Ă����B\n�ꉞ�A�[�̕�������P���Љ��\n�������B�Ƃ͂����Ă��A����Ȃɕ����̐���\n�����Ȃ�����A�����ɏI������B���Ƃ���\n�l�������Ȃ�����Ȃ��A���̎������B���A\n���炪����g���[�j���O���[�����A��������\n�܂ł͂܂������g���ĂȂ������炵���B\n���͉����o���o���g���ĂāA������\n�e�C���[��A��Ă��܂Ɉꏏ�ɂ���Ă�B\n�u�����i�[�͉��y�����Ă悭�������ǁA\n�̗͂�����񂾂��B�����ł��������̒b��\n�Ƃ���A�s�b�`���[�v\n�ꉞ�s�b�`���[�ɂ����߂Ă����B\n�u�s�b�J�[�ł��v\n���A���O�ԈႦ�Ă��B\n�u����A���͒n�����B���ė����v\n��������A�e�C���[�͍�������Ă񂾁H", Cr);
			break;
		case 1021:
			DrawString(Cx, Cy, L"�@�Ƃ肠�����n���ɘA��Ă����B�������܂�\n���������Ƃ͂Ȃ����A���ς�炸�e�C���[��\n�R���N�V�����������ɕ���ł�B�e�C���[��\n�������j�^�[�̑O�ɂ���񂾂��A����\n���Ȃ��B�����A�������C���ȁB����\n�������T���B\n�u�s�[�^�[�A��������Ă݂�v\n���́A�Ԃ��h���S���̃��{�b�g���w������\n�������B\n�u�s�b�J�[�ł��B�Ȃ�ł����H�v\n�u�߂Â��Ă�[�����Ă݂�B��������v\n�����O�ɂ��ꂽ����B�����͂����\n���������Ă��񂾂ȁB���ς�炸���i\n�����ȁB�s�b�J�[�����{�b�g�ɋ߂Â��ƁA\n�u������I�H�v\n�ˑR���̃��{�b�g�������o�����B�H��傫��\n�L�΂��āA�傰���ȋN���̎d�������āA\n�u�₟�A�т����肵�������H�v\n���̃��{�b�g(�e�C���[)�����������B������\n���͂����[�т����肵���B", Cr);
			break;
		case 1022:
			DrawString(Cx, Cy, L"�u�܂��A����ł����̏Љ�͏I��肾�ȁv\n���͂����������B\n�u���肪�Ƃ��������܂��B�Ƃ���ŁA����\n���đ��ɐl�͂��Ȃ���ł����H�v\n�܂��A�����C�Â���ȁB\n�u���Ȃ����B�����C�������B���Ƃ��Ƃ���\n�߂��͎�������������ȁB�������̏o�Ԃ�\n�قƂ�ǂȂ��̂��v\n�u���ꂶ��A�d�������܂�Ȃ��́H�v\n�u�����ł��Ȃ����B�ŋ߂͉�����ꂷ��\n�ĂāA������W�߂Ă܂��̂���������\n�d�����v\n�܂��A���ꂪ�����̎d���Ȃ񂾂��ȁB\n�u���ꂶ��A��������Ă݂邩�H�e�C���[�A\n�T�|�[�g�ł����ȁH�v\n�e�C���[�͋�����̂����܂����A��{��\n�Ȃ邾�낤�Ǝv�������c�A\n�u�ł��邯�ǁA�߂�ǂ�������v\n����σ_�����A���̂߂�ǂ�������߁B\n�u����A�����T�|�[�g���Ă�邩�v", Cr);
			break;
		case 1030:
			DrawString(Cx, Cy, L"�u�T�b�J�[�A�Ԃ̉^�]�Ƌ��́H�v\n�u�s�b�J�[�ł��B�����Ă܂���v\n�Ƃ������ƂŁA���̉^�]�ō����̎d�����\n�������B�Ƃ肠�����A�ŋ߂̃|�C�̂Ă�\n����ɂ��Ęb���Ƃ����B\n�ŋ߂́A���y�ƍ߂��S���N����Ȃ�����\n�ɁA�|�C�̂ĂƂ����̑��������̌y�ƍ߂�\n�N�����Ă�B���A�ꉞ�ƍ߂Ȃ̂��B\n�u����ȂƂ���ł��������̂Ă�Ȃ��\n�悧�A�ǂ����̂Ă�Ȃ�X���Ŏ̂Ă��v\n������s��������������B\n�u�|�C�̂Ă���������߂܂���́H�v\n�u������O���낧�H�ł��|�C�̂Ă��āA\n�����Ă�400pop(��3300�~)�̔����ōς�\n�܂��񂾁B�����Ȃ�˂��񂾂�Ȃ��A�|�C\n�̂āB�܂��A�{�[�i�X�o�邩�炢�����ǁv\n�{�[�i�X�o��̂͂܂��������B�{�����B\n\n�܂��A�ړI�n�ɂ�30�����炢�Œ������B\n���āA�d�����B", Cr);
			break;
		case 1031:
			DrawString(Cx, Cy, L"�@���āA���������͂��ꂢ�����A�O�ɗ�����\n����Q�T�Ԃ͌o���Ă�B���낻�뗭�܂���\n���Ă邾��B\n�u�˂��A�{���Ƀ|�C�̂Ă���Ă�́H�v\n�u���ꂢ�Ɍ����邾��H�ł�����Ă��\n�Ȃ��A���ꂪ�B�|�C�̂Ă����Ă������Ȃ�\n�ꏊ�Ɏ̂Ă邩��ȁB�܂��A�����i�[�ɂ�\n�o���o�������ǂȁv\n�u�c�����������́H�v\n�u�����猩����񂾂�v\n���͂��������āA�Ԃɐς�ł����p�̃w�b�h\n�z����DJ�L�b�g�����o�����B���p�ɉ���\n���������̃L�b�g���B�������͂������A\n���F���΂����肾�B�����d����t�����B\n�����Ȃꂽ�N��������B�������D���̂悤\n���ȁB\n�u���āA�����瑖�邼�B�`�[�^�[�A����\n������I�v\n�u�s�b�J�[�ł��v", Cr);
			break;
		case 1032:
			DrawString(Cx, Cy, L"�@�Ȃ𗬂����������āA���͑������B�����\n�����ɉ��y��������B�����͐X���B�t���[�g\n�ƃT�b�N�X�����S�̋Ȃ�I�񂾁B�L��master\n��caring dance�ł��悩�������A���쌠�I��\n��Ȃ���������ʂ̋Ȃɂ����B���������Ă�\n�Ȃɍ��킹�Ē����������Ă����̂�\n�������A���̖ړI�̓S�~����m�c����Ȃ��A\n������W�߂��B���[�āA�ǂ����H\n�@���ƁA���������������B�G�����B������\n�Ȃɍ��킹�ĉ����o�����Ƃ��Ă邪�A���܂�\n�o���Ȃ��ĎG���ɂȂ��Ă邼�A�����炭��B\n���͕�������ꏊ�ɑ������B�����ɂ͎v����\n��葽�������炪�̂Ă��Ă��B�N����\n�߂�����̂ĂĂ����ȁB����ɂ��[�B����\n����Ȃ��炩�����������Ă������B���āA\n�������ɍs�����B���̋Ȃ͂Q���������Ȃ�\n�񂾁B", Cr);
			break;
		case 1040:
			DrawString(Cx, Cy, L"�@�����Ɍ����ƁA��납����Ă���\n�s�[�^�[�̂��Ƃ���������Y��Ă��B�ł�\n�����Ƃ��Ă��Ă��悤���B\n�u�I��������A�ς��Ȃ����H�v\n�u����ꂳ��B�������͖��Ȃ����v\n���͈�ʂ�d�����I��炵�āA�������\n������������W�߂ċA�����B\n�u���ꂾ���W�߂ꂽ��A������150pop\n(��1250�~)���炢�ɂȂ邩�ȁB�務���ˁ�v\n�u���[�g�́H�v\n�u�オ��Ȃ����v\n����ł��_������B�����Ƃނ����Ȃɂ����\n�悩�������H\n�u���ɂȂ����玄�𔲂��邩�ȁ[��v\n�u���Ă��A�����ɔ����Ă�邩��ȁv\n���������A�e�C���[�Ɠ����S�[���h�����i�[\n�ɂȂ�̎��̂ނ��񂾂Ȃ��B�͂��A�����B", Cr);
			break;
		case 1041:
			DrawString(Cx, Cy, L"�u���������΃L�b�J�[�A���O�ڕW�Ƃ�����\n�̂��H�v\n�킴�킴������������Ȏd���ɗ������\n������A��������񂾂낤�Ȃ��Ďv�����B\n�u�s�b�J�[�ł��B�ڕW�����A�Ƃɂ����A\n��������T�����������񂾂��ǁA�܂��ǂ�\n����΂����񂾂낤�H�v\n�u��������������i�[�Ȃ̂��H�v\n�u����Ȃ�A���O�Ƃ�������ΒT������v\n�e�C���[����b�ɓ����Ă����B\n�u���ꂶ��A���̕����ɗ��āB�ꏏ��\n�T���Ă������v\n�u���A�ł��A����ɍs���񂶂�Ȃ��́H�v\n�u����Ȃ̃Q�[�^�[�ɔC�����炢��\n�ł���������s��������v\n�e�C���[�͂��������āA�s�b�J�[��n����\n�A�ꋎ���čs�����B\n\n�u�e�C���[�A��Ƃ������Ȃ���������c�v\n���͏������Ԃ₢���B", Cr);
			break;
		case 1042:
			DrawString(Cx, Cy, L"�u�e�������c�v\n���́A���܂�Ă����ɐe���Ɏ̂Ă�ꂽ�B\n������ƈꏏ�Ɏ̂Ă�ꂽ�B�ނ�����\n�����āA�D�P�����ƕ�����΂����Ɏ̂�\n�₪�����B���s��œ�l���̉ו��𓐂��\n��s�@�ŋA�����炵���B�����`�P�b�g��\n�Ȃ�������͍����āA�x�@�Ƃ��ɑ��k���āA\n�Ȃ�Ƃ��A�ꂽ�炵�����A�Ƃ̂��̂�����\n�����Ă��炵���B���ꂩ��͂����{����\n��ςŁA�{���ɋ�J�����B\n�@���́A�e�����ł��������y�ƍ߂����Ă���\n���ĕ����āA�����i�[�ɂȂ肽�����Ďv�����B\n���E���ŋN�����Ă�ƍ߂𖳂����������ĂȁB\n�ł��A���͉��y���D�������A���t�̍˔\��\n�Ȃ�����A�V�N���������ł��V���o�[\n�����i�[�~�܂肾�B�ł������h���̎d����\n����̂̓S�[���h�����i�[���炾�B\n�@�c�������ɂ��邩������A�̂Ă�ꂽ����\n���ꂽ�킯����Ȃ��񂾂�ȁB�����A�S�~\n���Č������Ⴞ�߂��ȁB", Cr);
			break;
			//�e�C���[
		case 2000:
			DrawString(Cx, Cy, L"�@�����B�n���̂��̕������炶�ᕪ����Ȃ�\n���ǁA���ԓI�ɂ͓��̏o�̎��Ԃ��B�܂��́A\n�p�\�R���̓d�������悤�B�N������܂ł�\n�ԂɁA�����̓d�C��t���āA���̑̂̓_����\n����B���ɑ傫���d���͂Ȃ����ǁA�_����\n���Ȃ��ƃK�^�������Ⴄ����A���ꂾ����\n���ڂ�Ȃ��B\n�@�֐߁A�������A�o�b�e���[�B�ُ�Ȃ��B\n�@���C���Ɨ\���̃o�b�e���[�A�[�d�ς݁B\n�@�H�̓���m�F�B���Ȃ���ׂ܂��B\n�@�y���^���B�����Ƃ����Ƃ��ɓ������߁B\n�@��ʂ�I����āA����ƃp�\�R����\n�N�������B�Ƃ����Ă��A�T���郂�j�^�[��\n�����R�̓J���������ǁB���C���Ŏg���Ă�\n���j�^�[������ƁA�����ʂ胁�[����\n�͂��Ă����A�قƂ�ǂ͎d���̃��[��\n������A�y���ǂݔ�΂��B\n�����ƁA������Ⴄ���[�������Ă��B\n\n�w�����A�����̎������ɐV�l������x", Cr);
			break;
		case 2001:
			DrawString(Cx, Cy, L"�u���A�܂����I���������A�ǂ�Ȃ�H�v\n���͂P�K�ɂ������āA������l�̃����o�[�A\n�Q�[�^�[�ɐV�l��������Ă��Ƃ��������B\n�V�l�̓C���O���b�V�����b�v(�E�T�M)�B\n���������̏��≹�y�w�Z�𑲋Ƃ����΂���B\n���т́A���y�ƍH�삪�ǂ��āA���w�Ɖp�ꂪ\n������ƃ_���B�݊w���Ƀ����i�[�����ɍ��i\n���Ă�B���ƁA�s�A�m�̏��������t���\n���񂩏o�Ă�B\n�u�V�l�����A�y���݂��ȁv\n�u�������Ď��ƃQ�[�^�[�̓�l����������\n����ˁ�v\n�u�Ȃ�ō��܂ŗ��Ȃ������񂾂낤�ȁH�v\n�u���l�͏o���Ă����ǁA�u��҂����Ȃ�����\n�񂾂�ˁ�v\n�����i�[�́A��b�m�������鎎���ɍ��i\n���Ȃ��ƂȂ�Ȃ����Č����̂����邯�ǁA\n���������~�����������Ă����̂�����̂�\n�ˁB����Ȃɓ���l���Ȃ��Ă����̂ɁB", Cr);
			break;
		case 2002:
			DrawString(Cx, Cy, L"�u�������e�C���[�A���܂ɂ̓����i�[�Ƃ���\n�����Ă݂Ȃ����H�v\n�u�������̊Ǘ������邩�烄�_��v\n�u����Ȃ̉��ɔC������āv\n�u�܂��Ō�ɑ����Ă��炽�����̂R������\n�o���ĂȂ����v\n�����Ɍ����ƁA����̂��߂�ǂ������B\n��������̓����e����ς����A�ȒP�Ȏd����\n���߂ɂ킴�킴�̂𓮂��������Ȃ��B\n�u�������Ă����Ƃ��ɓ����Ȃ����v\n�u���̓T�C�{�[�O������􂾂�����[�ԁ�v\n�T�C�{�[�O���Ă����̂́A�̂̈ꕔ���@�B��\n�ς���Z�p�̂��ƁB���̏ꍇ�͔����ȏオ\n�@�B������A�������{�b�g���Č����Ă�\n���������Ȃ����A�������ɋC�ɂ��ĂȂ��B\n�Ȃ�Ń��{�b�g�ɂȂ��������Ă����ƁA\n���y���̂ŕm���������Ƃ���𔎎m�ɏ�����\n�����������Ȃ񂾁B�ł��A�݂�Ȃɂ�\n�������D���ł������Ă���������Č�����\n��B���܂�m��ꂽ���Ȃ�����ˁB", Cr);
			break;
		case 2010:
			DrawString(Cx, Cy, L"�@���̓��A�V�l���񂪗�������B������\n�V�l���}����̂͋v���Ԃ肾�ȁB�S�N�O��\n�Q�[�^�[�������Ɉٓ����Ĉȗ����B�V����\n�����i�[���Ԃ�������̂͂��ꂵ����\n������B���̑���ɓ����Ă����l��\n�����邩��ˁ�B\n����ɁA�h���̎d�����ł���悤�ɂȂ�\n�̂����ꂵ���ȁB�������ɂ͍Œ��l��\n���Ȃ��Ƃ��߂ŁA���X��l�������Ȃ�����\n�����͔h���̎d������΂ɗ��Ȃ���������A\n�S�[���h�����i�[�̎��͑ދ��Ŏd���Ȃ�����\n�񂾁B\n\n�@�Ƃ��l���Ă���A�����Ă����B���j�^�[\n�z�������ǁA�͂�����ƌ�����B�v����\n���͔w�͒Ⴍ�āA���킢�炵���C���O\n���b�V�����b�v(�E�T�M)���B", Cr);
			break;
		case 2011:
			DrawString(Cx, Cy, L"�@�V�l���񂪎������̔��̑O�ł��ǂ���\n���Ă�̂����j�^�[�z���Ɍ������B\n���킢���B���̓}�C�N����Ɏ���āA\n�u�����A�ǂ��������肭�������[��v\n�V�l����ɂ����������B���̂��Ƃ́A\n�J�����ɋC�Â����݂����ŁA�p������������\n������Ă��B�V�l���񂪓�������́A\n�u�����ƁA�V�l����ł���ˁH�������\n���̂܂܂ł��҂����������[��v\n���������āA�V�l�����҂������B���Ƃ́A\n�����Q�K�ŃO�[�X�J�Q�Ă邾�낤�_�C�i�[��\n�N�����āA���Ƃ̓_�C�i�[�ɔC���悤�B\n�@�_�C�i�[�̊�]�ŁA�_�C�i�[�̕�����\n�J�����͒u���ĂȂ��B�߂�ǂ������Ƃ���\n�d�b�ł���肷��̂��K�����B����\n��b�������āA�_�C�i�[�ɓd�b��������B", Cr);
			break;
		case 2012:
			DrawString(Cx, Cy, L"�@�v������肷���ɓd�b���Ȃ������B\n�u����ف�B�N���Ă�H�v\n�u�N���Ă��A�Ȃ񂾁H�v\n����A�_�C�i�[������葁�N������Ȃ��\n�������Ȃ��B\n�u�V�l���񗈂����v\n�u����A�m���Ă���āv\n�������A�_�C�i�[���V�l���񂪗���̂�\n�y���݂ɂ��Ă��񂾂ˁB\n�u���������_�C�i�[��B����A�R�����\n�P�K�ɗ��ā�B���Ƃ͂�낵���ˁ[��v\n�R���̊Ԃ��J����̂́A�V�l����𗎂�\n�������邽�߁B������Ƌْ����Ă�݂���\n����������ˁB���Ƃ̓_�C�i�[�������\n�����ł���B\n�@���A�������B�������������A����\n�_�C�i�[�ɂ�����T�v���C�Y��V�l����ɂ�\n����Ă����悤�B�����ƁA���O�͊m��\n�s�b�J�[��������ˁB�悵�A���v���B", Cr);
			break;
		case 2020:
			DrawString(Cx, Cy, L"�@�܂��A����͂����̏����E������ˁ�\n���͍��A�����̃R���N�V�����̒��ɉB���\n����B����ς�A���{�b�g���B���ɂ�\n���{�b�g�̒�����ˁB�߂���ʂ����Ƃ����\n�����o���āA�r�b�N���������킾�B�����A\n�����s�b�J�[�̓Q�[�^�[�ƈꏏ�Ɏ�������\n���ĉ���Ă�񂾂낤�ˁB�c�Q�[�^�[�A\n�s�b�J�[�Ɏ���Ȃ��ƌ����ĂȂ����ȁH\n�����Ȃ�A����Ƃ������Ă邩���ȁ[�B\n���ƂŎ�������Ɏӂ��Ƃ��B\n\n�@���΂炭���āA�K�i�����C���̑�����\n�������Ă����B�����ˁB�܂��̓Q�[�^�[�A\n���Ƀs�b�J�[���o�Ă����B�ւ��A�s�b�J�[��\n�v������菬�����񂾂ˁB", Cr);
			break;
		case 2021:
			DrawString(Cx, Cy, L"�@�s�b�J�[������낫���Ƃ������\n���n���Ă���B�܂��A���������̂΂���\n�����A���R���ȁB���A���j�^�[�����ςȂ�\n�������B�ʂɂ�����B�f�X�N�g�b�v�ƃJ����\n�����f���ĂȂ����A����ɂ����邱�Ƃ�\n���Ȃ��ł���B�Q�[�^�[���Ⴀ��܂����B\n�u�s�[�^�[�A��������Ă݂�v\n�u�s�b�J�[�ł��B�Ȃ�ł����H�v\n���O�Ԉ���Ă��A�Q�[�^�[�B�Q�[�^�[��\n�����w�����Č������B\n�u�߂Â��Ă�[�����Ă݂�B��������v\n�t�H���[���Ă����̂͂��肪������\n�����ǁA�C�^�Y���S����ɏo�Ă�B\n�s�b�J�[�����ɂ�����x�߂Â����Ƃ���ŁA\n�u������I�H�v\n���͓����o�����B�~�܂��Ă����̂��ˑR����\n�����񂾂���A�����r�b�N�������ˁB\n�ł����͂��ꂾ�����ᕨ����Ȃ��āA�H��\n�傫���L���Ă݂���A�傰���ȓo�������\n�݂��B", Cr);
			break;
		case 2022:
			DrawString(Cx, Cy, L"�u���ĂƁA���߂܂��āA�����i�[��������\n�悤�����I�{�N�������̊Ǘ��l�̃e�C���[\n����B��낵���ˁ�v\n�u��A��낵�����肢���܂��v\n�u�Q�[�^�[�ɉ���đ��X�A����Ƃ�������\n���肵���񂶂�Ȃ��H���߂�ˁA�ނ͈��C��\n�Ȃ��̂��v\n�u���A�܂��A�͂��v\n���͂��̌���A������ׂ肪�������Ęb��\n�����Ă��B�����������̎���������o�ĂȂ�\n����A�Q�[�^�[�ȊO�̐l�Ƙb���̂��ċv��\n�Ԃ�ŁA�����������񂾁B���łɁA����\n�T�C�{�[�O�����Ă��Ƃ��s�b�J�[�ɘb����\n�������B\n�u�e�C���[�A���������񂶂�Ȃ����H�v\n���̂قڈ���I�ȉ�b�́A�Q�[�^�[�̂���\n���t�ŋ����I���ƂȂ����B���̑̓����v\n���ƁA��R�����炢�������B", Cr);
			break;
		case 2030:
			DrawString(Cx, Cy, L"�@�s�b�J�[�̎������ē�������ŏI�����\n�݂����ŁA���͂��������d���̘b�ɂȂ���\n�����B\n�u�e�C���[�A�T�|�[�g�ł����ȁH�v\n�Q�[�^�[�ɂ��������ꂽ���ǁA\n�u�ł��邯�ǁA�߂�ǂ�������v\n�f������\n�u����A�����T�|�[�g���Ă�邩�B\n�s�b�J�[�A���ė����v\n���������āA�Q�[�^�[�ƃs�b�J�[�͎d����\n�������Ă������B\n���āA�����ɂ͎��������Ȃ��B������\n�J�M���������B�J�M�͔O�̂��߁A�R���Ԍ��\n�����ŊO����邯�ǁA����ł��\�����B����\n�������̔閧�̍�Ƃɓ���B", Cr);
			break;
		case 2031:
			DrawString(Cx, Cy, L"�@���̓S�[���h�����i�[�B�����i�[�̒��ł�\n�g�b�v�N���X�̎��͂��������i�[���B\n�����i�[�ɂ͂S��ނ��āA�����i�[�ɂȂ���\n�΂���̃r�M�i�[�����i�[�B�d�����ł���\n�悤�ɂȂ�u�����Y�����i�[����95���B\n����d��������悤�ɂȂ�V���o�[\n�����i�[����25���B���E���̑傫�Ȏd����\n�C������S�[���h�����i�[����P�����B\n���ɂ�����炵�����ǁA�����m���Ă���̂�\n���ꂾ���B\n�������Ă����ʂ́A�S�[���h�����i�[��\n���̏��T�C�g�B�ق��̐l�Ɍ����Ă�\n�����Ȃ����܂ł���B�J�M�������Ă���\n�̂͂���̂��߁B�Q�[�^�[�ɂ́A������\n�߂Ă���Ƃ��͐Q�Ă�����Č����Ă���B\n�N��������n�O���}�~�L�T�[�̌Y����\n�����Ă�(��k)��B", Cr);
			break;
		case 2032:
			DrawString(Cx, Cy, L"�@���i�����񂾂��͏W�߂Ă����B�s�b�J�[\n������������A���ɂ��h���̎d���������\n���낤�ȁB�ȒP�Ȏd���͑S�����C���Ȃ���\n�����ǁA����d���ƂȂ�ƁA�Ȃ񂾂�\n���N���N����B���ꂪ�h���S���̖{�\�Ȃ�\n���ȁH����A�悭�킩��Ȃ����ǁB\n�@���͓��Ƀ����i�[�ɂȂ肽�������킯����\n�Ȃ��āA�X�ŉ��Q�[����Ă���X�J�E�g\n���ꂽ��������������B���i�Ƃ���o���ɁA\n���������Q�[�΂��������Ă������ǂ�\n����Č������̂��͕s���B�����i�[������\n�X�J�E�g����Ă���󂯂��B���ꂩ��12�N��\n�o���āA���܂ł̓S�[���h�����i�[�Ƃ���\n�Ƃ������A�l�b�g�ŏ��񋟂�A�h�o�C�X\n�����Ă����B�h���̎d�����Ȃ�����A���E\n���ŉ����N�����Ă���̂��́A�ʐ^�ƕ�����\n�����m�邱�Ƃ��ł��Ȃ������B\n�@�c����ς�A�������񂾂낤�ȁA���B�܂�\n�d����������Č��܂����킯����Ȃ����ǁA\n������撣���", Cr);
			break;
		case 2040:
			DrawString(Cx, Cy, L"�u�I��������A�ς��Ȃ����H�v\n�u����ꂳ��B�������͖��Ȃ����v\n�Q�[�^�[�ƃs�b�J�[���d�����I���ċA����\n�����B����̎d���͐X�Ɏ̂Ă�ꂽ������\n�W�߁A�ʖ��A�|�C�̂ĉ���B���a�Ȃ��̒n��\n����A�����i�[�̎d���͂قƂ�ǂ���B\n�u���ꂾ���W�߂ꂽ��A������150pop\n(��1250�~)���炢�ɂȂ邩�ȁB�務���ˁ�v\n�u���[�g�́H�v\n�u�オ��Ȃ����v\n���[�g���Ă����̂́A�����i�[�̘r�O������\n���l�B�����i�[�̊�͑S������Ō��܂���\n����B���̓S�[���h�̒��ł���̂ق��B\n�u���ɂȂ����玄�𔲂��邩�ȁ[��v\n�u���Ă��A�����ɔ����Ă�邩��ȁv\n�Q�[�^�[�́A�V���o�[�����i�[�ɂȂ�̂�\n�T�N���������Ă�B�������x�����ǁA���y\n�D�������w�͉Ƃ�����A���ƂW�N������\n�S�[���h�ɂȂ��񂶂�Ȃ����ȁB�撣���", Cr);
			break;
		case 2041:
			DrawString(Cx, Cy, L"�u���������΃L�b�J�[�A���O�ڕW�Ƃ�����\n�̂��H�v\n���O�ԈႦ�Ă��B\n�u�s�b�J�[�ł��B�ڕW�����A�Ƃɂ����A\n��������T�����������񂾂��ǁA�܂��ǂ�\n����΂����񂾂낤�H�v\n�u��������������i�[�Ȃ̂��H�v\n���A����̓`�����X����\n�u����Ȃ�A���O�Ƃ�������ΒT������B\n���̕����ɗ��āB�ꏏ�ɒT���Ă������v\n�u���A�ł��A����ɍs���񂶂�Ȃ��́H�v\n�u����Ȃ̃Q�[�^�[�ɔC�����炢��\n�ł���������s��������v\n���T���������ɁA�߂�ǂ������������\n�W�v���Q�[�^�[�ɓ�������B�Ƃ������A\n�s�b�J�[�Ƃ��b���������B�Q�[�^�[������\n��s�����C�����邯�ǁA���ɂ͕�����\n�ȁ[����", Cr);
			break;
		case 2042:
			DrawString(Cx, Cy, L"�u���[�ƁA�{���ɂ��̐l�Ȃ́H�v\n�s�b�J�[�̂�������̖��O�ƁA�o�g�n�Ƃ�\n���̑������������āA�o�Ă�������\n���ʂ́A�Ƃ�ł��Ȃ����̂������B\n�w�urikpp�ۃG�x���Ă����A���O�̔�����\n������������ɂ��鉹�����d���ŋN����\n��K�͔����B�ނ͂��̓����҂������̂��B\n�����Ă��邩���킩��Ȃ���]�I�ȏ󋵁B\n�����A�s�b�J�[���m���Ă�񂾂낤���ǁA\n�ꉞ�������B\n�u���܂茾�������Ȃ����ǁA�S���Ȃ��Ă�\n��������Ȃ���B����ł��T���̂����H�v\n�u����A���̂��߂Ƀ����i�[�ɂȂ�������v\n�u���������B�����ł��邾�����͂����v\n�ȑO�܂ł̓l�b�g�ł����m��Ȃ��������B\n�h���Ƃ�����i�����������A���ɂ����ł�\n�Ȃ����Ƃ����邾�낤�B���łɁA����\n�����ƈ�����������Ȃ����ǁA��񂭂炢��\n�W�߂Ă��悤���ȁB", Cr);
			break;
			//nature prhysm �R���{�X�g�[���[(������nature prhysm���(Take_Ma����)�͊ւ���Ă��܂���)
		case 3000:
			DrawString(Cx, Cy, L"�u���O���e�C���[��?\n���邩��Ƀ��J���J�����ȁv\n�u�͂��A�����̊Ԃł����A\n��낵�����肢���܂���v\n�܂����h����ł��Q�[�^�[�݂����Ȑl��\n��Ƃ͎v��Ȃ������ȁB\n�@���͍��A�n�[���b�c�Ƃ����ꏊ�ɂ���A\n���R�Ǘ��Z���^�[�̃n�[���b�c�x���ɂ���B\n�����Ƙb���Ă���S�����̃{���X����́A\n�����ň�l�œ����Ă���̂��������B\n�u�l�肪����Ȃ��Ƃ̂��ƂŔh�����ꂽ\n�̂ł����A��͂��l���Ƒ�ςł���?�v\n�u���Ⴂ����Ȃ�B��i����̖��߂�\n�Ă񂾂񂾁B���͂܂��܂����邳�v\n�܂��A�܂��Ⴛ���ȃ��L���L�̃S��������\n�����A�킴�킴���̏����͂���Ȃ����ȁB\n�c����A���ꂶ��ǂ����Ď���\n�Ă΂ꂽ�񂾂낤?", Cr);
			break;
		case 3001:
			DrawString(Cx, Cy, L"�@����������n���ƁA�z���ɓ������ꖇ��\n�ʐ^���ǂɂ������Ă���̂��������B\n�u����́A�Z���^�[�̎В��̎��摜���v\n�{���X���񂪋����Ă��ꂽ�B������\n�������l�Ԃ̊炪�ʂ��Ă��A���ǁA\n�u��f���Ⴍ�Ȃ��ł���?\n�܂�Ńh�b�g�G�̂悤�ł����v\n�u�h�b�g�G�Ȃ񂾂�(�{)�v\n�����ƁA�{�点����������ȁc? ������\n�l��{�点�₷������ˁA�C�����Ȃ���B\n�u�В��������i�[�Ȃ�ł���?�v\n�u���B�̓����i�[�Ƃ͈Ⴄ�B�l�X�Ȉُ�\n�C�ۂ�\�����A�񓹁A�K�v�Ȃ璲�����s��\n�̂��d�����B�����i�[�݂����Ɍ������\n�˓����ɍs���悤�Ȏd������Ȃ��񂾁v\n�����i�[�̎d���ɂ��Ă�75%�����Ă�B\n�ł����ꂾ������Ȃ��񂾂��B\n���Ă��Ƃ������Ɠ{�点����������\n����Ȃ��ł������B", Cr);
			break;
		case 3002:
			DrawString(Cx, Cy, L"�u�Ƃ���ŁA���͉�������Ηǂ��ł���?�v\n�u�������ȁc�A���ꂩ��Z���^�[�̗��ɂ���\n�C�ۃ��[�_�[�̃����e�i���X������\n�s���񂾂��A�ꏏ�ɗ��邩?�v\n�u���A����s���܂���v\n�u����A�҂āA���������΁A����3����\n�܂Ƃ߂ďo���Ȃ��Ƃ�����C�ۊϑ�\n�f�[�^���c���Ă�񂾂����B\n�������ɂ��邩�v\n�u���A���v�ł����B���@�B�������\n���ӂȂ�ł�����v\n�u���A�ł��A���Y����?\n���\�����ȋ@�B����?�v\n�u�������Đ����ȋ@�B�ł������!\n���������e�s���܂������v\n�f�[�^�����Ȃ�Ă��܂������̂���Ȃ��B\n�@�B�����肵�Ă����92.4�{�}�V��B", Cr);
			break;
		case 3010:
			DrawString(Cx, Cy, L"�@�����A���܂�d�����Ȃ��炵���A����\n�f�[�^��������炳��邱�ƂɂȂ����B\n���Ă������ǂ����ăf�W�^������\n�킴�킴���ɏ����ėX�ւő���Ȃ���\n�����Ȃ���(�{)? ���߂ăv�����^�[���炢\n�ݒu���Ȃ�����B\n�@���̓��̒��ł͂����v�Z�͏I�����\n���āA�V���[�y���������Ă����𓮂���\n�ȊO�ɂ��邱�Ƃ��Ȃ��đދ��B����������\n���g�̃h���S���̂܂܂�������Ƃ�����\n�Q�Ă���B\n�@�������A�h������A������̂��Ƃ�\n�l���悤�B�Ƃ肠�����A�A�������ȍ����\n�݂悤���ȁB\"Digital of Analog\"���ċȂŁA\n�A�i���O�̊��ɝf�v��ꂽ�f�W�^�������\n�z�����Ă݂悤�B�ŏ�����BPM��215\n������Ŕ�΂��Ă������B�A�i���O(����)��\n�키�f�W�^�������z������񂾁B\n�u���O�A�̂��ĂȂ��ł������Ƃ��v\n�C��������̂��Ă���", Cr);
			break;
		case 3011:
			DrawString(Cx, Cy, L"�u������! ����Ȃɏ����K�v�����ł���?�v\n�u����B����̋C�ۗ\���ɂ͌�������v\n�u�Ƃ͂���1�T�ԕ��̃f�[�^��30������\n�菑������Ȃ�č��ł���[�B\n����8.2���Ԃ�����܂���v\n�u����A����Ȃɑ����I���Ȃ�����B\n�v�Z������񂾂��v\n�u���̋@�B�̓��ɂ�����Όv�Z�Ȃ��\n��u�Ȃ̂ł����v\n�u�������Ȃ��O�v\n�@�������炷����ŋ@�B�ɂȂ����킯����\n�Ȃ����ǁA�����������͂悭������\n������Ă�B�o�b�e���[���������\n�����Ɠ�������\n�u���͂�����ƐH���������ɍs���ė���B\n�f�[�^�������ނ��v\n�u�́[����v\n�{���X���񂪃Z���^�[���o�Ă������B\n����45���͋A���Ă��Ȃ����ȁB\n�悵�A������ƃT�{���", Cr);
			break;
		case 3012:
			DrawString(Cx, Cy, L"\n�@�@�s�K�V����!!\n\n�@���͋����đ��̊O�̌i�F������B\n��J�B�����������ꏏ�B35.6���O�܂Ő����\n�����̂ɁA����Ȃ��Ƃ������!?\n�@���̑̂��u���b�Ɛk����B�嗱�̉J��\n�n�ʂ�˂����ƁA�J�_�̒��œd�C�������\n���A�����ė����n�[���b�c�̊X���P�����B\n�����ɍ������āA�l�X�������Ȃ���X��\n�|���鉹���������ɕ�������B\n�@���͂���Ȏ��ł��A�����̉���\"��\"\n�Ƃ��ĕ����Ă��܂����Ȃ�����B�Ȃ��\n���͂�����ȂȂ񂾁B�������̋Ȃ̈����\n�Ȃ肽���B\n\n�@�C��������A���̓Z���^�[����\n��яo���Ă����B", Cr);
			break;
		case 3020:
			DrawString(Cx, Cy, L"�@�J���s�A�m��@���A�l�̐����o�C�I������\n�e���A���ܗ����V���o����łȂ������X�B\n���͎��̉���炵�Ȃ��瑖���Ă���ƁA\n�{���X����ɉ�����B\n�u������Ă�񂾂您�O�c�v\n�u���߂�Ȃ����A����������\n��ȑt�łĂ܂�����v\n�u�����������i�[���ȁc�v\n�{���X����̎�ɂ́A�����������Ă���\n�r�j�[���܂��������B�������ٓ�����?\n�u�ł��A�}�ɍ~��n�߂܂����ˁB\n�т����肵�܂�����v�����������B\n�u�������A����Ȃ̗\������\n�ł��ĂȂ��������v\n�Ăї����V���o����ł����B\n�u�e�C���[�A���t�̏����͂ł��Ă���?�v\n�u���A�͂��A�o�b�`����v\n�u�ꏏ�ɉ��t���邼�B����΂����\n����l���ጵ�����v", Cr);
			break;
		case 3021:
			DrawString(Cx, Cy, L"�@�������Ď��B�̋��t���n�܂����B����\n�����Ȋy��̉���������o���ĉ��t�����B\n�{���X����́A�����i�[�Ƃ͈Ⴄ���@�ŉ��t\n���Ă����B�悭�����ƁA3��ނ̉�����\n�o���Ă��Ȃ����ƂɋC�Â����B�K�v�ȉ�\n������炵�ĉ��t������@�́A����\n�m���Ă钆���ƃN���V�b�N�ɋ߂����̂������B\n(�ŋ߁A�N���V�b�N�͌Â��Ƃ��V���{���Ƃ�\n�����Ă邯�ǁA�N���V�b�N�Ȃ߂Ȃ��ł��\n�N���V�b�N�͊�b����������\�z����Ă��\n�Ȃ񂾂���ˁ�)\n�u���ʂ��Ȃ��������肵�����F�ł��ˁ�v\n�{���X����͂�����Ƌ������l�q�ł�������\n�����B\n�u�� ? ����̗ǂ����킩��̂�?�v\n�u�͂��A���͉��ƗF�B�Ȃ̂Ł�v\n�{���X���񂪏����΂����悤�ȋC�������B\n�u�悵�A�����𑱂��邼�B���̋C�ۂ�\n�p�^�[����������񂾁v", Cr);
			break;
		case 3022:
			DrawString(Cx, Cy, L"�@�������͉J�̒����ꂼ��̕��@�ŋ��t\n���Ă����B���͑傫�����̉��ɕ����Ȃ�\n�悤�ɁA�͋����G���L�M�^�[�̉����o�����B\n(�V���Z�̉����ǂ����Ă����������Ⴄ����)\n���͂⎄�����ɍ��킹��񂶂�Ȃ��āA����\n���ɍ��킹��񂾁B\n�@�ق��̐l�͂Ƃ����Ɍ����̒���\n������������B�J�̒����t���Ȃ���O��\n�����Ă�l���āA�悭�l������ς����w\n�ߏ����f�œ{��ꂽ��ӂ��Ƃ���\n\n�@�̓����v����1.24���ԁB�������͂�����\n���t���Ă��B�U���Ă����J�͂��񂾂�キ\n�Ȃ��āA���͂����~��ł����B��������\n��������т���G��ɂȂ��Ă��āA�{���X\n����͈������݂������B���͑̓���\n�q�[�^�[�����邩�畽�C��\n�u���Z���^�[�ɖ߂邼�c�v\n�{���X���񂪌������B\n�u�͂��A�����l�ł���v", Cr);
			break;
		case 3030:
			DrawString(Cx, Cy, L"�u�����ƁA����{���ł����c?�v\n���̊y�����������t����I������A�{���X\n����͂܂��V�����[�𗁂тɍs�����B����\n���A���̑̂̃����e�i���X�����邽�߂ɁA\n�g�̂̃p�[�c�𕪉����āA�ׂ�������\n����Ă�B����ȂɃK�^�͂Ȃ����ǁA�ꉞ\n�S�����Ă����B\n�@�����ă{���X����͍��A��b��̑O��\n����Ȃ��Ƃ������Ă����B\n�u������ƁA���ꂱ��������\���ł���\n��������ł����c�A�ǂ��������Ƃł���?�v\n���̃{���X���񂪒��J�Ȍ����������Ă���B\n���̂����񂩂�?\n�u�����A�f�[�^�Ȃ炿���Ǝ��܂�����B\n�e�C���[���悭�����Ă���܂����v\n�ˑR���O���Ă΂�Ăт����肵���B����\n���\�D������ɂ���Ă��񂾂��ǂȁ�", Cr);
			break;
		case 3031:
			DrawString(Cx, Cy, L"�@�{���X����̒ʘb���I������悤�ŁA����\n���֕����Ă����B��������ƁA����������\n�l�q�������B\n�u�������ȁA�{���Ƀ��{�b�g�Ȃ񂾂ȁv\n�u�����A���傤�ǃ����e�i���X���Ă��Ƃ���\n�Ȃ�ł���v\n�{���X����̓����e���̎��������[������\n���Ă����B\n�u����ŁA���̂��b��������ł���?�v\n�u��? �����A���ꂩ�B�������̑嗒�����A\n���̃Z���^�[�ł͗\���ł��Ă����炵���񂾁v\n�u����? ���ꂶ��A�����̂����Ă���\n�ł��傤��? �v\n�u����A�����̂̓f�[�^���Â����Č���ꂽ�v\n�u��k�ł���!?�p�ɂɍX�V����\n�Ȃ�������!?�v\n�����i�̌����ɂȂ��Ă��܂����B\n�u�A�i���O�Ȃ񂾂悤���́c�v\n\n�@�ň��B", Cr);
			break;
		case 3032:
			DrawString(Cx, Cy, L"�u�܂��A�����߂�ǂ��������āA�����\n�Ȃ���������������ȁB����ꂿ�܂����v\n�{���X���񂪌������B������Ƃ����߂�\n�����グ�悤���ȁB\n�u�߂�ǂ�������̋C�����킩��܂���`�A\n���������ł�����B�ł��A\n�߂�ǂ��������Ƃɂ͈ĊO��؂�\n���̂������ł���`��v\n�u�f�[�^�����Ƃ��ȁv\n�u�����c�v\n���������΂܂��d���c���Ă��񂾂����B\n�u���[�����A�i���O�Ȃ񂩑��������!�v\n�u���������ȁA������`������v\n�u���߂ăv�����^�[���炢�u���Ă��������c�v\n�u�܂��A���̂����ȁv", Cr);
			break;
		case 3040:
			DrawString(Cx, Cy, L"\n\n\n�@�@�@�@�@�@�@�@�@�@�@�@�@3018�N3��13��\n�@�@�@�@�@�@�@�I�����@�[�E�n���r���[�w��\n\n�@�@�@�@�@���y�ЊQ�����񍐏�\n\n1.��������: 3018�N3��10���@AM0:50����\n\n2.�����ꏊ: Irreversible�n��", Cr);
			break;
		case 3041:
			DrawString(Cx, Cy, L"\n3.�ЊQ��:\n�@���ɉ��̑O�G����Ȃ��ɂ��ւ�炸�A\n����Ԃ̘c�݂��e�n�ɔ����B����Ƃ���ł�\n���ł���A���̂��鏊�ł͖�ł������B�܂��A\n���鏊�ł�1�b��1���ɂȂ��Ă���A���̂���\n���ł�1����1�b�ɂȂ��Ă����B��̗l�q��\n�Œ��ꒃ�ł���A���͂₱�̏ꏊ�ɂ͋��\n�������̂͑��݂��Ă��Ȃ����̂悤�Ɏv�����B\n�@���̏ꏊ���t�łĂ������y�́ABPM��200�ƁA\n���y�ЊQ�ɂ��Ă͒x�߂̃y�[�X�Ɍ��������A\n32���̉���������O�̂悤�ɖ��Ă������߁A\n�������400�ł������B", Cr);
			break;
		case 3042:
			DrawString(Cx, Cy, L"\n4.������:\n�@���݁A���̒n��͓��ʎw����Ƃ��āA\n��ʐl�y�уS�[���h�ȉ��̃����i�[�̗���\n������֎~���Ă���B����Ԃ̘c�݂͓���\n�o�ƂƂ��Ɏ��܂��Ă��邪�A�s���̎��Ԃ�\n�����āA���A�n���r���[�w���ƁA�����\n�}�q���[�Y��z�����Ă���B��ɊĎ����A\n�󋵂����|�[�g���Ă����B\n\n�@<�ǋL>\n�@���݂͉��y�Љ�ł���A���y�������̈���\n�ɂ͏\���C�����Ȃ���΂Ȃ�Ȃ��B��l\n�ЂƂ肪���̎��o�������A���S�ɁA���K��\n���y�������K�v������B", Cr);
			break;
		}
		DrawGraph(0, 405, pageimg, TRUE);
		DrawFormatString(15, 420, Cr, L"%d / 3", com + 1);
		DrawGraph(0, 0, help, TRUE);
		DrawString(5, 460, L"�㉺�L�[:�y�[�W�I��   BackSpace�L�[:�߂�", Crw);
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_UP)) {
			//���������ꂽ
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				com--;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN)) {
			//�E�������ꂽ
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				com++;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK)) {
			//�o�b�N�X�y�[�X�������ꂽ
			if (key == 0) {
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
		//����{�^���������ꂽ
		else if (GetWindowUserCloseFlag(TRUE)) return 5;
		//����̃L�[��������Ă��Ȃ�
		else key = 0;
		com = mins(maxs(com, 2), 0);
	}
	return 0;
}

int option(void) {
	//���[�h
	//data = [�L����,�I�t�Z�b�g,SE,�w�i���邳,����]
	int e, sel;
	int	data[5] = { 0,0,0,2,0 };
	FILE *fp;
	e = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (e == 0) {
		fread(&data, sizeof(data), 5, fp);
		fclose(fp);
	}
	//����
	int key;
	int command = 0;
	unsigned int Cr;
	int backimg, cimg, help;
	backimg = LoadGraph(L"picture/OPTION back.png");
	cimg = LoadGraph(L"picture/OC.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.ogg");
	Cr = GetColor(255, 255, 255);
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		DrawGraph(40, 45 + command * 50, cimg, TRUE);
		switch (data[0]) {
		case 0:
			if (data[4] == 0)DrawString(100, 50, L"�L�����N�^�[: �s�b�J�[", Cr);
			else if (data[4] == 1)DrawString(100, 50, L"Character: Picker", Cr);
			break;
		case 1:
			if (data[4] == 0)DrawString(100, 50, L"�L�����N�^�[: �}�b�v�Q�[�^�[", Cr);
			else if (data[4] == 1)DrawString(100, 50, L"Character: MapGator", Cr);
			break;
		case 2:
			if (data[4] == 0)DrawString(100, 50, L"�L�����N�^�[: �e�C���[", Cr);
			else if (data[4] == 1)DrawString(100, 50, L"Character: Taylor", Cr);
			break;
		}
		if (data[4] == 0)DrawFormatString(100, 100, Cr, L"�I�t�Z�b�g: %dms", data[1] * 5);
		else if (data[4] == 1)DrawFormatString(100, 100, Cr, L"Offset: %dms", data[1] * 5);
		if (data[2] == 0 && data[4] == 0) DrawString(100, 150, L"���ʉ�: ����", Cr);
		else if (data[2] == 0 && data[4] == 1) DrawString(100, 150, L"SE: ON", Cr);
		else if (data[2] == 1 && data[4] == 0)DrawString(100, 150, L"���ʉ�: �Ȃ�", Cr);
		else if (data[2] == 1 && data[4] == 1)DrawString(100, 150, L"SE: OFF", Cr);
		switch (data[3]) {
		case 0:
			if (data[4] == 0)DrawString(100, 200, L"�w�i�̖��邳: �^����", Cr);
			else if (data[4] == 1)DrawString(100, 200, L"BackGroundBrightness: Black", Cr);
			break;
		case 1:
			if (data[4] == 0)DrawString(100, 200, L"�w�i�̖��邳: �Â�", Cr);
			else if (data[4] == 1)DrawString(100, 200, L"BackGroundBrightness: Dark", Cr);
			break;
		case 2:
			if (data[4] == 0)DrawString(100, 200, L"�w�i�̖��邳: ����", Cr);
			else if (data[4] == 1)DrawString(100, 200, L"BackGroundBrightness: Middle", Cr);
			break;
		case 3:
			if (data[4] == 0)DrawString(100, 200, L"�w�i�̖��邳: ���邢", Cr);
			else if (data[4] == 1)DrawString(100, 200, L"BackGroundBrightness: Bright", Cr);
			break;
		}
		switch (data[4]) {
		case 0:
			DrawString(100, 250, L"���� Language: ���{��", Cr);
			break;
		case 1:
			DrawString(100, 250, L"���� Language: English", Cr);
			break;
		}
		switch (command) {
		case 0:
			if (data[4] == 0)DrawString(20, 420, L"�g�p����L�����N�^�[��ς��܂��B", Cr);
			else if (data[4] == 1)DrawString(20, 420, L"Choose the character you use.", Cr);
			break;
		case 1:
			if (data[4] == 0)DrawString(20, 420, L"����������Ă���^�C�~���O��ς��܂��B\n���₷�ƒx��āA���炷�Ƒ��߂ɗ���܂��B", Cr);
			else if (data[4] == 1)DrawString(20, 420, L"Change the timing of note. Increase to late, Decrease to early.", Cr);
			break;
		case 2:
			if (data[4] == 0)DrawString(20, 420, L"�����������������̌��ʉ���炷���ǂ�����ς��܂�", Cr);
			else if (data[4] == 1)DrawString(20, 420, L"Choose whether to play the sound of hitting the note", Cr);
			break;
		case 3:
			if (data[4] == 0)DrawString(20, 420, L"�w�i�̖��邳��ς��܂��B", Cr);
			else if (data[4] == 1)DrawString(20, 420, L"Choose brightness of background.", Cr);
			break;
		case 4:
			DrawString(20, 420, L"�Q�[���Ŏg�������ς��܂��B\nChoose the lunguage in this game.", Cr);
			break;
		}
		DrawGraph(0, 0, help, TRUE);
		if (data[4] == 0)DrawString(5, 460, L"�㉺���E�L�[:�I��   BackSpace�L�[:�ۑ����Ė߂�", Cr);
		else if (data[4] == 1)DrawString(5, 460, L"�������� key:choose   BackSpace key:save the option and return", Cr);
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//���������ꂽ
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				data[command]--;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//�E�������ꂽ
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				data[command]++;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_UP)) {
			//�オ�����ꂽ
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command--;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN)) {
			//���������ꂽ
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command++;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK) == 1) {
			//�o�b�N�X�y�[�X�������ꂽ
			if (key == 0) {
				e = _wfopen_s(&fp, L"save/system.dat", L"wb");
				fwrite(&data, sizeof(int), 5, fp);
				fclose(fp);
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			//����{�^���������ꂽ
			return 5;
		}
		else {
			//����̃L�[��������Ă��Ȃ�
			key = 0;
		}
		if (command < 0) {
			command = 4;
		}
		if (command > 4) {
			command = 0;
		}
		if (data[0] == -1) {
			data[0] = 2;
		}
		if (data[0] == 3) {
			data[0] = 0;
		}
		if (data[1] < -10000) {
			data[1] = -10000;
		}
		if (data[1] > 10000) {
			data[1] = 10000;
		}
		if (data[2] == -1) {
			data[2] = 1;
		}
		if (data[2] == 2) {
			data[2] = 0;
		}
		if (data[3] == -1) {
			data[3] = 3;
		}
		if (data[3] == 4) {
			data[3] = 0;
		}
		if (data[4] == -1) {
			data[4] = 1;
		}
		if (data[4] == 2) {
			data[4] = 0;
		}
	}
	return 1;
}