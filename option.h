now_scene_t option(void) {
	//���[�h
	//data = [�L����,�I�t�Z�b�g,SE,�w�i���邳,����]
	int e, sel;
	int	data[7] = { 0,0,0,2,0,0,0 };
	FILE *fp;
	e = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (e == 0) {
		fread(&data, sizeof(int), 7, fp);
		fclose(fp);
	}
	//����
	int key = 1;
	int command = 0;
	unsigned int Cr;
	int backimg, cimg, help;
	backimg = LoadGraph(L"picture/OPTION back.png");
	cimg = LoadGraph(L"picture/OC.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.wav");
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
		switch (data[5]) {
		case 0:
			DrawString(100, 300, L"�{�^���\��: off", Cr);
			break;
		case 1:
			DrawString(100, 300, L"�{�^���\��: on", Cr);
			break;
		}
		switch (data[6]) {
		case 0:
			DrawString(100, 350, L"����\���ʒu: �����̏�", Cr);
			break;
		case 1:
			DrawString(100, 350, L"����\���ʒu: ����", Cr);
			break;
		case 2:
			DrawString(100, 350, L"����\���ʒu: �E��", Cr);
			break;
		case 3:
			DrawString(100, 350, L"����\���ʒu: ����", Cr);
			break;
		case 4:
			DrawString(100, 350, L"����\���ʒu: �L�����̏�", Cr);
			break;
		case 5:
			DrawString(100, 350, L"����\���ʒu: �\�����Ȃ�", Cr);
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
			else if (data[4] == 1)DrawString(20, 420, L"Choose whether to sound of hitting the note.", Cr);
			break;
		case 3:
			if (data[4] == 0)DrawString(20, 420, L"�w�i�̖��邳��ς��܂��B", Cr);
			else if (data[4] == 1)DrawString(20, 420, L"Choose brightness of background.", Cr);
			break;
		case 4:
			DrawString(20, 420, L"�Q�[���Ŏg�������ς��܂��B\nChoose the lunguage in this game.", Cr);
			break;
		case 5:
			if (data[4] == 0)DrawString(20, 420, L"�{�^���̉����󋵂��v���C��ʂɕ\�����܂��B", Cr);
			else if (data[4] == 1)DrawString(20, 420, L"Choose whether to display the key states on playing mode.", Cr);
			break;
		case 6:
			if (data[4] == 0)DrawString(20, 420, L"����̕\���ꏊ�����߂܂��B", Cr);
			else if (data[4] == 1)DrawString(20, 420, L"Choose judge position.", Cr);
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
				fwrite(&data, sizeof(int), 7, fp);
				fclose(fp);
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
#if 0
		else if (CheckHitKey(KEY_INPUT_Q) == 1) {
			/* output dif */
			if (key == 0) {
				int G[10];
				int difl[4] = { 0,0,0,0 };
				wchar_t file[255];
				wchar_t songN[4][255];
				songN[0][0] = L'\0';
				songN[1][0] = L'\0';
				songN[2][0] = L'\0';
				songN[3][0] = L'\0';
				playnum_box allnum;
				FILE *qfp;
				FILE *ofp;
				G[1] = _wfopen_s(&ofp, L"difout.txt", L"w");
				fwprintf(ofp, L"\t\t\tEASY\tNORMAL\tHARD\tANOTHER\n");
				for (int i = 0; i < 10; i++) {
					for (int j = 0; j < 20; j++) {
						difl[0] = 0;
						difl[1] = 0;
						difl[2] = 0;
						difl[3] = 0;
						songN[0][0] = L'\0';
						songN[1][0] = L'\0';
						songN[2][0] = L'\0';
						songN[3][0] = L'\0';
						for (int dif = 0; dif < 4; dif++) {
							RecordLoad2(i, j, dif + 1);
							get_rec_file(file, i, j, dif + 1, FILETYPE_RRS);
							G[0] = _wfopen_s(&qfp, file, L"rb");//rrs�f�[�^��ǂݍ���
							if (G[0] == 0) {
								fread(&allnum, sizeof(playnum_box), 1, qfp);//�e�f�[�^�̌�

								fseek(qfp, 255 * 4 + sizeof(int) + sizeof(double), SEEK_CUR);

								fread(&songN[dif], 255, 1, qfp);//�Ȗ�

								fseek(qfp, 255 + sizeof(short int) * 2 +
									sizeof(int) * 1585 + sizeof(double) * 990 +
									sizeof(int) * (allnum.Ymovenum[0] * 4 + allnum.Ymovenum[1] * 4 +
										allnum.Ymovenum[2] * 4 + allnum.Ymovenum[3] * 4 +
										allnum.Ymovenum[4] * 4 + allnum.Xmovenum[0] * 4 +
										allnum.Xmovenum[1] * 4 + allnum.Xmovenum[2] * 4) +
									sizeof(struct note_box) * (allnum.notenum[0] + allnum.notenum[1] +
										allnum.notenum[2]), SEEK_CUR);

								fread(&difl[dif], sizeof(int), 1, qfp);
								fclose(qfp);
							}
						}
						for (G[0] = 0; G[0] < 4; G[0]++) {
							if (songN[G[0]][0] != L'\0') {
								break;
							}
						}
						if (G[0] >= 4) {
							break;
						}
						songN[G[0]][23] = L'\0';
						fwprintf(ofp, L"%s", songN[G[0]]);
						for (int i = 24 - strlens(songN[G[0]]); i > 0; i -= 8) {
							fwprintf(ofp, L"\t");
						}
						for (int dif = 0; dif < 4; dif++) {
							if (difl[dif] == 0) {
								fwprintf(ofp, L"-");
							}
							else {
								fwprintf(ofp, L"%2.2f", difl[dif] / 100.0);
							}
							if (dif < 3) {
								fwprintf(ofp, L"\t");
							}
						}
						fwprintf(ofp, L"\n");
					}
				}
				fclose(ofp);
			}
			key = 1;
		}
#endif
		else if (GetWindowUserCloseFlag(TRUE)) {
			//����{�^���������ꂽ
			return SCENE_EXIT;
		}
		else {
			//����̃L�[��������Ă��Ȃ�
			key = 0;
		}
		if (command < 0) {
			command = 6;
		}
		if (command > 6) {
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
		if (data[5] == -1) {
			data[5] = 1;
		}
		if (data[5] == 2) {
			data[5] = 0;
		}
		if (data[6] == -1) {
			data[6] = 5;
		}
		if (data[6] == 6) {
			data[6] = 0;
		}
	}
	return SCENE_MENU;
}