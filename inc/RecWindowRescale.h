#pragma once

#include <sancur.h>
#include <RecSystem.h>
#include <fontcur.h>

#define RESCALE_SIZE_X ((double)WINDOW_SIZE_X / (double)OLD_WINDOW_SIZE_X)
#define RESCALE_SIZE_Y ((double)WINDOW_SIZE_Y / (double)OLD_WINDOW_SIZE_Y)

enum {
	REC_RESCALE_STRETCH = 1,
	REC_RESCALE_TOP_LEFT,
	REC_RESCALE_TOP_CENTRE,
	REC_RESCALE_TOP_RIGHT,
	REC_RESCALE_CENTRE_LEFT,
	REC_RESCALE_CENTRE,
	REC_RESCALE_CENTRE_RIGHT,
	REC_RESCALE_BOTTOM_LEFT,
	REC_RESCALE_BOTTOM_CENTRE,
	REC_RESCALE_BOTTOM_RIGHT,
};

extern void RecRescaleDrawLine(int x1, int y1, int x2, int y2, uint cr, int thick = 1);
extern void RecRescaleDrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint cr, int fill);
extern void RecRescaleDrawGraph(int x, int y, int pic, int anchor);
extern void RecRescaleDrawExtendGraph(int x1, int y1, int x2, int y2, int pic, int anchor);
extern void RecRescaleDrawRotaGraph(int x, int y, double ExRate, double Angle, int pic, int anchor);
extern void RecRescaleDrawDeformationPic(int x, int y, double sizeX, double sizeY, int rot, int handle);
extern void RecRescaleDrawString(int x, int y, const TCHAR *s, uint cr);
extern void RecRescaleAnchorDrawString(int x, int y, const TCHAR *s, uint cr, int anchor);
extern void RecRescaleDrawFormatString(int x, int y, uint cr, const TCHAR *s, ...);
extern void RecRescaleAnchorDrawFormatString(int x, int y, uint cr, int anchor, const TCHAR *s, ...);
extern void RecRescaleDrawCurFont(double dn, short int x, short int y, int sizeY,
	cur_font_cr_t Cr, short int a = 0, int zero = 1);
