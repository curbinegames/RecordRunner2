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