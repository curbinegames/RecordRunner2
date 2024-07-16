
#include "fontcur.h"
#include "DxLib.h"
#include "../general/strcur.h"
#include "../general/dxcur.h"
#include "../RecWindowRescale.h"

typedef DxPic_t cur_font_pic_t[12];

static cur_font_pic_t cur_font_pic[7];

void InitCurFont() {
	LoadDivGraph(_TEXT("fontcur/NumberMono.png"),    12, 5, 3, 58, 64, cur_font_pic[CUR_FONT_COLOR_MONO]);
	LoadDivGraph(_TEXT("fontcur/NumberRed.png"),     12, 5, 3, 58, 64, cur_font_pic[CUR_FONT_COLOR_RED]);
	LoadDivGraph(_TEXT("fontcur/NumberYellow.png"),  12, 5, 3, 58, 64, cur_font_pic[CUR_FONT_COLOR_YELLOW]);
	LoadDivGraph(_TEXT("fontcur/NumberGreen.png"),   12, 5, 3, 58, 64, cur_font_pic[CUR_FONT_COLOR_GREEN]);
	LoadDivGraph(_TEXT("fontcur/NumberBlue.png"),    12, 5, 3, 58, 64, cur_font_pic[CUR_FONT_COLOR_BLUE]);
	LoadDivGraph(_TEXT("fontcur/NumberPurple.png"),  12, 5, 3, 58, 64, cur_font_pic[CUR_FONT_COLOR_PURPLE]);
	LoadDivGraph(_TEXT("fontcur/NumberRainbow.png"), 12, 5, 3, 58, 64, cur_font_pic[CUR_FONT_COLOR_RAINBOW]);
	return;
}

/**
 * dn = draw number
 * x = draw X pos
 * y = draw Y pos
 * sizeY = font Y size
 * Cr = font color
 * a = under point count (if dn = 157.423 and a = 2, draw "157.42")
 * zero = draw "0." (if dn = 0.524 and zero = 0, draw ".524")
 */
void DrawCurFont(double dn, short int x, short int y, int sizeY,
	cur_font_cr_t Cr, short int a, int zero) {
	/* int */
	int k = 0;
	int n;
	int m;
	int ZeroP = 0; /* minus flag */
	int sizeX = sizeY * 58 / 64;
	int width = sizeY * 46 / 64;
	int pointX = sizeY * 17 / 64; /* dot X size */

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
		if (a != 1 || zero == 1 || m != 0) {
			RecRescaleDrawExtendGraph(x + i * width + (a > 0) * pointX + ZeroP * width, y,
				x + sizeX + i * width + (a > 0) * pointX + ZeroP * width,
				y + sizeY, cur_font_pic[Cr][m], TRUE);
		}
		n /= 10;
		if (a == 1) {
			RecRescaleDrawExtendGraph(x + i * width + ZeroP * width, y,
				x + sizeX + i * width + ZeroP * width, y + sizeY, cur_font_pic[Cr][CUR_FONT_ID_DOT],
				TRUE);
		}
		a--;
	}
	if (ZeroP) { RecRescaleDrawExtendGraph(x, y, x + sizeX, y + sizeY, cur_font_pic[Cr][CUR_FONT_ID_MINUS], TRUE); }
	return;
}
