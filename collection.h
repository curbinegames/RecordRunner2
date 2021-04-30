int collection();
int C_item();
int C_story();
int story(int, int);

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