
#include "DxLib.h"
#include "sancur.h"
#include "system.h"
#include "RecWindowRescale.h"

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

	switch (anchor) {
		case REC_RESCALE_STRETCH:
			drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
			drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
			drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
			drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
			break;
		case REC_RESCALE_TOP_LEFT:
			drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
			drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
			drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
			drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
			break;
		case REC_RESCALE_TOP_CENTRE:
			drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
			drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
			drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
			drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
			break;
		case REC_RESCALE_TOP_RIGHT:
			drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
			drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
			drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
			drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
			break;
		case REC_RESCALE_CENTRE_LEFT:
			drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
			drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
			drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
			drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
			break;
		case REC_RESCALE_CENTRE:
			drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
			drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
			drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
			drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
			break;
		case REC_RESCALE_CENTRE_RIGHT:
			drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
			drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
			drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
			drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
			break;
		case REC_RESCALE_BOTTOM_LEFT:
			drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
			drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
			drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
			drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
			break;
		case REC_RESCALE_BOTTOM_CENTRE:
			drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
			drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
			drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
			drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
			break;
		case REC_RESCALE_BOTTOM_RIGHT:
			drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
			drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);
			drawX2 = drawX + sizeX * WINDOW_SIZE_X / OLD_WINDOW_SIZE_X;
			drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;
			break;
	}

	DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic, TRUE);
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

void RecRescaleDrawString(int x, int y, const TCHAR *s, uint cr) {
	int drawX = 0;
	int drawY = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);

	DrawString(drawX, drawY, s, cr);
	return;
}

void RecRescaleDrawFormatString(int x, int y, uint cr, const TCHAR *s, ...) {
	va_list as;
	va_start(as, s);

	int drawX = 0;
	int drawY = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, x);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, y);

	DrawFormatString(drawX, drawY, cr, s, as);

	va_end(as);
	return;
}
