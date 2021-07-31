int option(void) {
	//���[�h
	//data = [�L����,�I�t�Z�b�g,SE,�w�i���邳,����]
	int e, sel;
	int	data[6] = { 0,0,0,2,0,0 };
	FILE *fp;
	e = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (e == 0) {
		fread(&data, sizeof(int), 6, fp);
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
		switch (data[5]) {
		case 0:
			DrawString(100, 300, L"�{�^���\��: off", Cr);
			break;
		case 1:
			DrawString(100, 300, L"�{�^���\��: on", Cr);
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
			else if (data[4] == 1)DrawString(20, 420, L"Choose whether to display the key states on playing mode", Cr);
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
				fwrite(&data, sizeof(int), 6, fp);
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
			command = 5;
		}
		if (command > 5) {
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
	}
	return 1;
}