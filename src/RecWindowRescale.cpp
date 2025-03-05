
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <dxdraw.h>
#include <fontcur.h>
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <system.h>

/* own include */
#include <RecWindowRescale.h>

void RecRescaleDrawLine(int x1, int y1, int x2, int y2, uint cr, int thick) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x1);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y1);
	drawX2 = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x2);
	drawY2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y2);
	DrawLine(drawX, drawY, drawX2, drawY2, cr, thick);
	return;
}

void RecRescaleDrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint cr, int fill) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;
	int drawX3 = 0;
	int drawY3 = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x1);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y1);
	drawX2 = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x2);
	drawY2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y2);
	drawX3 = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x3);
	drawY3 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y3);

	DrawTriangle(drawX, drawY, drawX2, drawY2, drawX3, drawY3, cr, fill);
	return;
}

void RecRescaleDrawGraph(int x, int y, int pic, int anchor) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;
	int sizeX = 0;
	int sizeY = 0;

	GetGraphSize(pic, &sizeX, &sizeY);

	/* Xpos */
	switch (anchor) {
	case REC_RESCALE_TOP_RIGHT:
	case REC_RESCALE_CENTRE_RIGHT:
	case REC_RESCALE_BOTTOM_RIGHT:
		drawX = x - OLD_WINDOW_SIZE_X + WINDOW_SIZE_X;
		drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
		break;
	case REC_RESCALE_TOP_LEFT:
	case REC_RESCALE_CENTRE_LEFT:
	case REC_RESCALE_BOTTOM_LEFT:
		drawX = x;
		drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
		break;
	case REC_RESCALE_STRETCH:
	case REC_RESCALE_TOP_CENTRE:
	case REC_RESCALE_CENTRE:
	case REC_RESCALE_BOTTOM_CENTRE:
		drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
		drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
		break;
	}

	/* Ypos */
	switch (anchor) {
	case REC_RESCALE_TOP_LEFT:
	case REC_RESCALE_TOP_CENTRE:
	case REC_RESCALE_TOP_RIGHT:
		drawY = y;
		drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
		break;
	case REC_RESCALE_BOTTOM_LEFT:
	case REC_RESCALE_BOTTOM_CENTRE:
	case REC_RESCALE_BOTTOM_RIGHT:
		drawY = y - OLD_WINDOW_SIZE_Y + WINDOW_SIZE_Y;
		drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
		break;
	case REC_RESCALE_STRETCH:
	case REC_RESCALE_CENTRE_LEFT:
	case REC_RESCALE_CENTRE:
	case REC_RESCALE_CENTRE_RIGHT:
		drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
		drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
		break;
	}

	/* draw */
	switch (anchor) {
	case REC_RESCALE_TOP_LEFT:
	case REC_RESCALE_TOP_RIGHT:
	case REC_RESCALE_BOTTOM_LEFT:
	case REC_RESCALE_BOTTOM_RIGHT:
		DrawGraph(drawX, drawY, pic, TRUE);
		break;
	case REC_RESCALE_STRETCH:
	case REC_RESCALE_TOP_CENTRE:
	case REC_RESCALE_CENTRE_LEFT:
	case REC_RESCALE_CENTRE:
	case REC_RESCALE_CENTRE_RIGHT:
	case REC_RESCALE_BOTTOM_CENTRE:
		DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic, TRUE);
		break;
	}

	return;
}

void RecRescaleDrawExtendGraph(int x1, int y1, int x2, int y2, int pic, int anchor) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;
	int sizeX = 0;
	int sizeY = 0;

	sizeX = x2 - x1;
	sizeY = y2 - y1;

	switch (anchor) {
	case REC_RESCALE_STRETCH:
		drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x1);
		drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y1);
		drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
		drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
		break;
	case REC_RESCALE_TOP_LEFT:
		drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x1);
		drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y1);
		drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
		drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
		break;
	case REC_RESCALE_TOP_CENTRE:
		drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x1);
		drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y1);
		drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
		drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
		break;
	case REC_RESCALE_TOP_RIGHT:
		drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x1);
		drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y1);
		drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
		drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
		break;
	case REC_RESCALE_CENTRE_LEFT:
		drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x1);
		drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y1);
		drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
		drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
		break;
	case REC_RESCALE_CENTRE:
		drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x1);
		drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y1);
		drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
		drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
		break;
	case REC_RESCALE_CENTRE_RIGHT:
		drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x1);
		drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y1);
		drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
		drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
		break;
	case REC_RESCALE_BOTTOM_LEFT:
		drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x1);
		drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y1);
		drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
		drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
		break;
	case REC_RESCALE_BOTTOM_CENTRE:
		drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x1);
		drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y1);
		drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
		drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
		break;
	case REC_RESCALE_BOTTOM_RIGHT:
		drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x1);
		drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y1);
		drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
		drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
		break;
	}

	DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic, TRUE);
}

void RecRescaleDrawRotaGraph(int x, int y, double ExRate, double Angle, int pic, int anchor) {
	int drawX = 0;
	int drawY = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);

	DrawRotaGraph(drawX, drawY, ExRate * 1.5, Angle, pic, TRUE);
	return;
}

void RecRescaleDrawDeformationPic(int x, int y, double sizeX, double sizeY, int rot, int handle) {
	int drawX = 0;
	int drawY = 0;
	double drawSizeX = 0;
	double drawSizeY = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);

	drawSizeX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, sizeX);
	drawSizeY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, sizeY);

	DrawDeformationPic(drawX, drawY, drawSizeX, drawSizeY, rot, handle);
	return;
}

void RecRescaleDrawString(int x, int y, const TCHAR *s, uint cr) {
	int drawX = 0;
	int drawY = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);

	DrawString(drawX, drawY, s, cr);
	return;
}

void RecRescaleAnchorDrawString(int x, int y, const TCHAR *s, uint cr, int anchor) {
	int drawX = 0;
	int drawY = 0;

	/* Xpos */
	switch (anchor) {
	case REC_RESCALE_TOP_RIGHT:
	case REC_RESCALE_CENTRE_RIGHT:
	case REC_RESCALE_BOTTOM_RIGHT:
		drawX = x - OLD_WINDOW_SIZE_X + WINDOW_SIZE_X;
		break;
	case REC_RESCALE_TOP_LEFT:
	case REC_RESCALE_CENTRE_LEFT:
	case REC_RESCALE_BOTTOM_LEFT:
		drawX = x;
		break;
	case REC_RESCALE_STRETCH:
	case REC_RESCALE_TOP_CENTRE:
	case REC_RESCALE_CENTRE:
	case REC_RESCALE_BOTTOM_CENTRE:
		drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
		break;
	}

	/* Ypos */
	switch (anchor) {
	case REC_RESCALE_TOP_LEFT:
	case REC_RESCALE_TOP_CENTRE:
	case REC_RESCALE_TOP_RIGHT:
		drawY = y;
		break;
	case REC_RESCALE_BOTTOM_LEFT:
	case REC_RESCALE_BOTTOM_CENTRE:
	case REC_RESCALE_BOTTOM_RIGHT:
		drawY = y - OLD_WINDOW_SIZE_Y + WINDOW_SIZE_Y;
		break;
	case REC_RESCALE_STRETCH:
	case REC_RESCALE_CENTRE_LEFT:
	case REC_RESCALE_CENTRE:
	case REC_RESCALE_CENTRE_RIGHT:
		drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
		break;
	}

	/* draw */
	switch (anchor) {
	case REC_RESCALE_STRETCH:
		DrawString(drawX, drawY, s, cr);
		break;
	case REC_RESCALE_TOP_RIGHT:
	case REC_RESCALE_CENTRE_RIGHT:
	case REC_RESCALE_BOTTOM_RIGHT:
	case REC_RESCALE_TOP_LEFT:
	case REC_RESCALE_CENTRE_LEFT:
	case REC_RESCALE_BOTTOM_LEFT:
	case REC_RESCALE_TOP_CENTRE:
	case REC_RESCALE_CENTRE:
	case REC_RESCALE_BOTTOM_CENTRE:
		DrawStringToHandle(drawX, drawY, s, cr, SmallFontData);
		break;
	}

	return;
}

void RecRescaleDrawFormatString(int x, int y, uint cr, const TCHAR *s, ...) {
	va_list as;
	va_start(as, s);

	int drawX = 0;
	int drawY = 0;
	TCHAR buf[256];

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);

	vswprintf_s(buf, 256, s, as);

	DrawString(drawX, drawY, buf, cr);

	va_end(as);
	return;
}

void RecRescaleAnchorDrawFormatString(int x, int y, uint cr, int anchor, const TCHAR *s, ...) {
	va_list as;
	va_start(as, s);

	int drawX = 0;
	int drawY = 0;
	TCHAR buf[64];

	/* Xpos */
	switch (anchor) {
	case REC_RESCALE_TOP_RIGHT:
	case REC_RESCALE_CENTRE_RIGHT:
	case REC_RESCALE_BOTTOM_RIGHT:
		drawX = x - OLD_WINDOW_SIZE_X + WINDOW_SIZE_X;
		break;
	case REC_RESCALE_TOP_LEFT:
	case REC_RESCALE_CENTRE_LEFT:
	case REC_RESCALE_BOTTOM_LEFT:
		drawX = x;
		break;
	case REC_RESCALE_STRETCH:
	case REC_RESCALE_TOP_CENTRE:
	case REC_RESCALE_CENTRE:
	case REC_RESCALE_BOTTOM_CENTRE:
		drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
		break;
	}

	/* Ypos */
	switch (anchor) {
	case REC_RESCALE_TOP_LEFT:
	case REC_RESCALE_TOP_CENTRE:
	case REC_RESCALE_TOP_RIGHT:
		drawY = y;
		break;
	case REC_RESCALE_BOTTOM_LEFT:
	case REC_RESCALE_BOTTOM_CENTRE:
	case REC_RESCALE_BOTTOM_RIGHT:
		drawY = y - OLD_WINDOW_SIZE_Y + WINDOW_SIZE_Y;
		break;
	case REC_RESCALE_STRETCH:
	case REC_RESCALE_CENTRE_LEFT:
	case REC_RESCALE_CENTRE:
	case REC_RESCALE_CENTRE_RIGHT:
		drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
		break;
	}

	/* string */
	vswprintf_s(buf, 64, s, as);

	/* draw */
	switch (anchor) {
	case REC_RESCALE_STRETCH:
		DrawString(drawX, drawY, buf, cr);
		break;
	case REC_RESCALE_TOP_RIGHT:
	case REC_RESCALE_CENTRE_RIGHT:
	case REC_RESCALE_BOTTOM_RIGHT:
	case REC_RESCALE_TOP_LEFT:
	case REC_RESCALE_CENTRE_LEFT:
	case REC_RESCALE_BOTTOM_LEFT:
	case REC_RESCALE_TOP_CENTRE:
	case REC_RESCALE_CENTRE:
	case REC_RESCALE_BOTTOM_CENTRE:
		DrawStringToHandle(drawX, drawY, buf, cr, SmallFontData);
		break;
	}

	va_end(as);
	return;
}

void RecRescaleDrawCurFont(double dn, short int x, short int y, int sizeY,
	cur_font_cr_t Cr, short int a, int zero)
{
	int drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
	int drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
	DrawCurFont(dn, drawX, drawY, sizeY * RESCALE_SIZE_Y, Cr, a, zero);
	return;
}
