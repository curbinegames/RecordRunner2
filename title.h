int title(void) {
	int next = 1, alpha[2] = { 255,-1 }, Title, Push;
	Title = LoadGraph(L"picture/title.png");
	Push = LoadGraph(L"picture/pushkey.png");
	while (1) {
		//�^�C�g����ʂ�\������
		DrawGraph(0, 0, Title, TRUE);
		//������\������
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, betweens_D(0, alpha[0], 255));
		DrawGraph(0, 0, Push, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		alpha[1] == 1 ? alpha[0] += 5 : alpha[0] -= 5;
		if (alpha[0] <= 0 && alpha[1] == -1 || alpha[0] >= 255 && alpha[1] == 1) { alpha[1] *= -1; }
		ScreenFlip();
		if (CheckHitKeyAll()) {
			InitSoundMem();
			InitGraph();
			WaitTimer(500);
			break;
		}
		if (GetWindowUserCloseFlag(TRUE)) {
			next = 5;
			break;
		}
		WaitTimer(5);
	}
	ClearDrawScreen();
	return next;
}