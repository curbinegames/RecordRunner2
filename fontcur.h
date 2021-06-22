void DrawCurFont(double dn, short int x, short int y, int sizeY, short int Cr, short int a = 0) {
	int k = 0, n, m, sizeX = sizeY * 58 / 64, width = sizeY * 46 / 64, pointX = sizeY * 17 / 64;
	int font[10];
	int Gpoint[7];
	LoadDivGraph(L"picture/NumberPoint.png", 7, 3, 3, 58, 64, Gpoint);
	switch (Cr) {
	case 0:
		LoadDivGraph(L"picture/NumberMono.png", 10, 5, 2, 58, 64, font);
		break;
	case 1:
		LoadDivGraph(L"picture/NumberRed.png", 10, 5, 2, 58, 64, font);
		break;
	case 2:
		LoadDivGraph(L"picture/NumberYellow.png", 10, 5, 2, 58, 64, font);
		break;
	case 3:
		LoadDivGraph(L"picture/NumberGreen.png", 10, 5, 2, 58, 64, font);
		break;
	case 4:
		LoadDivGraph(L"picture/NumberBlue.png", 10, 5, 2, 58, 64, font);
		break;
	case 5:
		LoadDivGraph(L"picture/NumberPurple.png", 10, 5, 2, 58, 64, font);
		break;
	case 6:
		LoadDivGraph(L"picture/NumberRainbow.png", 10, 5, 2, 58, 64, font);
		break;
	}
	if (dn < 0) {
		dn *= -1;
	}
	for (int i = a; i > 0; i--) {
		dn *= 10;
	}
	n = dn;
	if (n != 0) {
		for (int i = 10; i <= n; i *= 10) {
			k++;
		}
	}
	for (int i = k; i >= 0; i--) {
		m = n % 10;
		DrawExtendGraph(x + i * width + (a > 0) * pointX, y, x + sizeX + i * width + (a > 0) * pointX, y + sizeY, font[m], TRUE);
		n /= 10;
		if (a == 1) {
			DrawExtendGraph(x + i * width, y, x + sizeX + i * width, y + sizeY, Gpoint[Cr], TRUE);
		}
		a--;
	}
	for (int i = 0; i < 10; i++)DeleteGraph(font[i]);
	for (int i = 0; i < 7; i++)DeleteGraph(Gpoint[i]);
	return;
}