#pragma once

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
	CUR_FONT_ID_MINUS, /* 10 */
	CUR_FONT_ID_DOT,   /* 11 */
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

extern void InitCurFont();
extern void DrawCurFont(double dn, short x, short y,
	int sizeY, cur_font_cr_t Cr, short a = 0, int zero = 1);
