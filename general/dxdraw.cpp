
#include "DxLib.h"
#include "dxdraw.h"
#include "../system.h"

/**
 * DrawGraphに基準点を追加したもの
 * @param[in] x x位置
 * @param[in] y y位置
 * @param[in] pic 画像ハンドル
 * @param[in] anchor 基準点
 * @details DrawGraphAnchor(xxx, xxx, xxx, DXDRAW_ANCHOR_TOP_LEFT)はDrawGraph(xxx,xxx,xxx,TRUE)と同じ。
 */
void DrawGraphAnchor(int x, int y, int pic, dxdraw_anchor_t anchor) {
	int drawX = 0;
	int drawY = 0;
	int sizeX = 0;
	int sizeY = 0;

	if (anchor == DXDRAW_ANCHOR_TOP_LEFT) {
		DrawGraph(x, y, pic, TRUE);
		return;
	}

	GetGraphSize(pic, &sizeX, &sizeY);

	switch (anchor) {
	case DXDRAW_ANCHOR_CENTRE_LEFT:
	case DXDRAW_ANCHOR_BOTTOM_LEFT:
		drawX = x;
		break;
	case DXDRAW_ANCHOR_TOP_CENTRE:
	case DXDRAW_ANCHOR_CENTRE:
	case DXDRAW_ANCHOR_BOTTOM_CENTRE:
		drawX = (WINDOW_SIZE_X - sizeX + 2 * x) / 2;
		break;
	case DXDRAW_ANCHOR_TOP_RIGHT:
	case DXDRAW_ANCHOR_CENTRE_RIGHT:
	case DXDRAW_ANCHOR_BOTTOM_RIGHT:
		drawX = WINDOW_SIZE_X - sizeX + x;
		break;
	}

	switch (anchor) {
	case DXDRAW_ANCHOR_TOP_CENTRE:
	case DXDRAW_ANCHOR_TOP_RIGHT:
		drawY = y;
		break;
	case DXDRAW_ANCHOR_CENTRE_LEFT:
	case DXDRAW_ANCHOR_CENTRE:
	case DXDRAW_ANCHOR_CENTRE_RIGHT:
		drawX = (WINDOW_SIZE_Y - sizeY + 2 * y) / 2;
		break;
	case DXDRAW_ANCHOR_BOTTOM_LEFT:
	case DXDRAW_ANCHOR_BOTTOM_CENTRE:
	case DXDRAW_ANCHOR_BOTTOM_RIGHT:
		drawY = WINDOW_SIZE_Y - sizeY + y;
		break;
	}

	DrawGraph(drawX, drawY, pic, TRUE);

	return;
}

/**
 * DrawGraphAnchorに拡大量を追加したもの
 * @param[in] x x位置
 * @param[in] y y位置
 * @param[in] size 拡大量、100で等倍
 * @param[in] pic 画像ハンドル
 * @param[in] anchor 基準点
 * @details sizeに100が入った場合、DrawGraphAnchorを実行する
 */
void DrawZoomGraphAnchor(int x, int y, intx100 size, int pic, dxdraw_anchor_t anchor) {
	int drawLeft = 0;
	int drawUp = 0;
	int drawRight = 0;
	int drawDown = 0;
	int sizeX = 0;
	int sizeY = 0;

	if (size == 100) {
		DrawGraphAnchor(x, y, pic, anchor);
		return;
	}

	GetGraphSize(pic, &sizeX, &sizeY);
	sizeX = sizeX * size / 100;
	sizeY = sizeY * size / 100;

	switch (anchor) {
	case DXDRAW_ANCHOR_CENTRE_LEFT:
	case DXDRAW_ANCHOR_BOTTOM_LEFT:
		drawLeft = x;
		break;
	case DXDRAW_ANCHOR_TOP_CENTRE:
	case DXDRAW_ANCHOR_CENTRE:
	case DXDRAW_ANCHOR_BOTTOM_CENTRE:
		drawLeft = (WINDOW_SIZE_X - sizeX + 2 * x) / 2;
		break;
	case DXDRAW_ANCHOR_TOP_RIGHT:
	case DXDRAW_ANCHOR_CENTRE_RIGHT:
	case DXDRAW_ANCHOR_BOTTOM_RIGHT:
		drawLeft = WINDOW_SIZE_X - sizeX + x;
		break;
	}
	drawRight = x + sizeX;

	switch (anchor) {
	case DXDRAW_ANCHOR_TOP_CENTRE:
	case DXDRAW_ANCHOR_TOP_RIGHT:
		drawUp = y;
		break;
	case DXDRAW_ANCHOR_CENTRE_LEFT:
	case DXDRAW_ANCHOR_CENTRE:
	case DXDRAW_ANCHOR_CENTRE_RIGHT:
		drawUp = (WINDOW_SIZE_Y - sizeY + 2 * y) / 2;
		break;
	case DXDRAW_ANCHOR_BOTTOM_LEFT:
	case DXDRAW_ANCHOR_BOTTOM_CENTRE:
	case DXDRAW_ANCHOR_BOTTOM_RIGHT:
		drawUp = WINDOW_SIZE_Y - sizeY + y;
		break;
	}
	drawDown = x + sizeX;

	DrawExtendGraph(drawLeft, drawUp, drawRight, drawDown, pic, TRUE);

	return;
}
