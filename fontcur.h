void DrawCurFont(double dn, short int x, short int y, int sizeY, short int Cr, short int a = 0) {
	int k = 0, n, m, ZeroP = 0, sizeX = sizeY * 58 / 64, width = sizeY * 46 / 64, pointX = sizeY * 17 / 64;
	int font[12];
	switch (Cr) {
	case 0:
		LoadDivGraph(L"picture/NumberMono.png", 12, 5, 3, 58, 64, font);
		break;
	case 1:
		LoadDivGraph(L"picture/NumberRed.png", 12, 5, 3, 58, 64, font);
		break;
	case 2:
		LoadDivGraph(L"picture/NumberYellow.png", 12, 5, 3, 58, 64, font);
		break;
	case 3:
		LoadDivGraph(L"picture/NumberGreen.png", 12, 5, 3, 58, 64, font);
		break;
	case 4:
		LoadDivGraph(L"picture/NumberBlue.png", 12, 5, 3, 58, 64, font);
		break;
	case 5:
		LoadDivGraph(L"picture/NumberPurple.png", 12, 5, 3, 58, 64, font);
		break;
	case 6:
		LoadDivGraph(L"picture/NumberRainbow.png", 12, 5, 3, 58, 64, font);
		break;
	}
	if (dn < 0) {
		ZeroP = 1;
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
		DrawExtendGraph(x + i * width + (a > 0) * pointX + ZeroP * width, y, x + sizeX + i * width + (a > 0) * pointX + ZeroP * width, y + sizeY, font[m], TRUE);
		n /= 10;
		if (a == 1) {
			DrawExtendGraph(x + i * width + ZeroP * width, y, x + sizeX + i * width + ZeroP * width, y + sizeY, font[11], TRUE);
		}
		a--;
	}
	if (ZeroP) { DrawExtendGraph(x, y, x + sizeX, y + sizeY, font[10], TRUE); }
	for (int i = 0; i < 12; i++)DeleteGraph(font[i]);
	return;
}