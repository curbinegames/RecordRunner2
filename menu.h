int menu(void) {
	int next = 1; //���̃��[�h
	int command = 1; //�I�𒆂̃��[�h
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
	unsigned int Cr = GetColor(255, 255, 255);
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
		movetime = maxs(GetNowCount() - starttime, 250);
		//�Ǖ\��
		goust = command - LR - 1;
		while (goust < 0) goust += 4;
		while (goust > 3) goust -= 4;
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