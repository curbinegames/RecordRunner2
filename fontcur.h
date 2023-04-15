
typedef enum cur_font_id_e {
	CUR_FONT_ID_0 = 0,
	CUR_FONT_ID_1,
	CUR_FONT_ID_2,
	CUR_FONT_ID_3,
	CUR_FONT_ID_4,
	CUR_FONT_ID_5,
	CUR_FONT_ID_6,
	CUR_FONT_ID_7,
	CUR_FONT_ID_8,
	CUR_FONT_ID_9,
	CUR_FONT_ID_MINUS,
	CUR_FONT_ID_DOT,
} cur_font_id_t;

typedef enum cur_font_cr_e {
	CUR_FONT_COLOR_MONO = 0,
	CUR_FONT_COLOR_RED,
	CUR_FONT_COLOR_YELLOW,
	CUR_FONT_COLOR_GREEN,
	CUR_FONT_COLOR_BLUE,
	CUR_FONT_COLOR_PURPLE,
	CUR_FONT_COLOR_RAINBOW,
} cur_font_cr_t;

/*
 * dn = draw number
 * x = draw X pos
 * y = draw Y pos
 * sizeY = font Y size
 * Cr = font color
 * a = under point count (if dn = 157.423 and a = 2, draw "157.42")
 */
void DrawCurFont(double dn, short int x, short int y, int sizeY,
	cur_font_cr_t Cr, short int a = 0) {
	/* int */
	int k = 0;
	int n;
	int m;
	int ZeroP = 0; /* minus flag */
	int sizeX = sizeY * 58 / 64;
	int width = sizeY * 46 / 64;
	int pointX = sizeY * 17 / 64; /* dot X size */
	wchar_t fontN[32] = L"picture/Number";
	switch (Cr) {
	case CUR_FONT_COLOR_MONO:
		strcats(fontN, L"Mono.png");
		break;
	case CUR_FONT_COLOR_RED:
		strcats(fontN, L"Red.png");
		break;
	case CUR_FONT_COLOR_YELLOW:
		strcats(fontN, L"Yellow.png");
		break;
	case CUR_FONT_COLOR_GREEN:
		strcats(fontN, L"Green.png");
		break;
	case CUR_FONT_COLOR_BLUE:
		strcats(fontN, L"Blue.png");
		break;
	case CUR_FONT_COLOR_PURPLE:
		strcats(fontN, L"Purple.png");
		break;
	case CUR_FONT_COLOR_RAINBOW:
		strcats(fontN, L"Rainbow.png");
		break;
	}
	/* pic */
	int font[12];
	LoadDivGraph(fontN, 12, 5, 3, 58, 64, font);
	/* cal */
	if (dn < 0) {
		ZeroP = 1;
		dn *= -1;
	}
	for (int i = a; i > 0; i--) { dn *= 10; }
	n = (int)dn;
	if (n != 0) { for (int i = 10; i <= n; i *= 10) { k++; } }
	/* draw */
	for (int i = k; i >= 0; i--) {
		m = n % 10;
		DrawExtendGraph(x + i * width + (a > 0) * pointX + ZeroP * width, y,
			x + sizeX + i * width + (a > 0) * pointX + ZeroP * width,
			y + sizeY, font[m], TRUE);
		n /= 10;
		if (a == 1) {
			DrawExtendGraph(x + i * width + ZeroP * width, y,
				x + sizeX + i * width + ZeroP * width, y + sizeY, font[11],
				TRUE);
		}
		a--;
	}
	if (ZeroP) { DrawExtendGraph(x, y, x + sizeX, y + sizeY, font[10], TRUE); }
	for (int i = 0; i < 12; i++) { DeleteGraph(font[i]); }
	return;
}