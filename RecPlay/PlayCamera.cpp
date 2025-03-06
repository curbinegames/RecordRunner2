
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <sancur.h>
#include <dxdraw.h>

/* rec system include */
#include <RecScoreFile.h>
#include <RecWindowRescale.h>

/* own include */
#include <PlayCamera.h>

static struct {
	int x = 320;
	int y = 240;
	intx100_t zoom = 100;
} camera_pos;

void RecPlayResetCamera() {
	camera_pos.x = 320;
	camera_pos.y = 240;
	camera_pos.zoom = 100;
	return;
}

void RecPlaySetCamera(rec_camera_set_t *camera, int Ntime) {
	const     uint    num = camera->num;
	const DxTime_t startT = camera->data[num].starttime;
	const DxTime_t   endT = camera->data[num].endtime;
	const      int  moveM = camera->data[num].mode;
	const      int  nposX = camera->data[num].xpos;
	const      int  nposY = camera->data[num].ypos;
	const      int  nZoom = camera->data[num].zoom;
	const      int  bposX = (num == 0) ? 0 : camera->data[num - 1].xpos;
	const      int  bposY = (num == 0) ? 0 : camera->data[num - 1].ypos;
	const      int  bZoom = (num == 0) ? 0 : camera->data[num - 1].zoom;

	if (startT <= Ntime && Ntime <= endT) {
		camera_pos.x    = (int)movecal(moveM, startT, bposX, endT, nposX, Ntime);
		camera_pos.y    = (int)movecal(moveM, startT, bposY, endT, nposY, Ntime);
		camera_pos.zoom = (int)movecal(moveM, startT, bZoom, endT, nZoom, Ntime);
	}
	else {
		camera_pos.x    = bposX;
		camera_pos.y    = bposY;
		camera_pos.zoom = bZoom;
	}
	return;
}

void RecPlayGetCameraPos(int *retX, int *retY) {
	if (retX != NULL) { *retX = camera_pos.x; }
	if (retY != NULL) { *retY = camera_pos.y; }
	return;
}

void DrawStringRecField(int xpos, int ypos, TCHAR *str, DxColor_t cr) {
	int drawX = 0;
	int drawY = 0;

	/* Xpos */
	drawX = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, xpos + camera_pos.x);

	/* Ypos */
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, ypos + camera_pos.y);

	/* draw */
	DrawString(drawX, drawY, str, cr);
	return;
}

/**
 * プレイ中のメイン画面、item、レーン、キャラにあたる場所に描画します。カメラの影響を受けます。
 */
void DrawGraphRecField(int xpos, int ypos, int pic) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;
	int sizeX = 0;
	int sizeY = 0;

	GetGraphSize(pic, &sizeX, &sizeY);

	/* Xpos */
	drawX  = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, xpos + camera_pos.x);
	drawX2 = drawX + sizeX * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;

	/* Ypos */
	drawY  = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, ypos + camera_pos.y);
	drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;

	/* zoom */
	drawX  = (drawX  - WINDOW_SIZE_X) * camera_pos.zoom / 100 + WINDOW_SIZE_X;
	drawX2 = (drawX2 - WINDOW_SIZE_X) * camera_pos.zoom / 100 + WINDOW_SIZE_X;
	drawY  = (drawY  - WINDOW_SIZE_Y) * camera_pos.zoom / 100 + WINDOW_SIZE_Y;
	drawY2 = (drawY2 - WINDOW_SIZE_Y) * camera_pos.zoom / 100 + WINDOW_SIZE_Y;

	/* draw */
	DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic, TRUE);
	return;
}

/**
 * プレイ中のメイン画面、item、レーン、キャラにあたる場所に描画します。カメラの影響を受けます。
 */
void DrawTurnGraphRecField(int xpos, int ypos, int pic) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;
	int sizeX = 0;
	int sizeY = 0;

	GetGraphSize(pic, &sizeX, &sizeY);

	/* Xpos */
	drawX  = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, xpos + camera_pos.x);
	drawX2 = drawX + sizeX * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;

	/* Ypos */
	drawY  = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, ypos + camera_pos.y);
	drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;

	/* zoom */
	drawX  = (drawX  - WINDOW_SIZE_X) * camera_pos.zoom / 100 + WINDOW_SIZE_X;
	drawX2 = (drawX2 - WINDOW_SIZE_X) * camera_pos.zoom / 100 + WINDOW_SIZE_X;
	drawY  = (drawY  - WINDOW_SIZE_Y) * camera_pos.zoom / 100 + WINDOW_SIZE_Y;
	drawY2 = (drawY2 - WINDOW_SIZE_Y) * camera_pos.zoom / 100 + WINDOW_SIZE_Y;

	/* draw */
	DrawExtendGraph(drawX2, drawY, drawX, drawY2, pic, TRUE);
	return;
}

/**
 * プレイ中のメイン画面、item、レーン、キャラにあたる場所に描画します。カメラの影響を受けます。
 */
void DrawLineRecField(int posx1, int posy1, int posx2, int posy2, unsigned int color, int thick) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;

	/* Xpos */
	drawX  = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, posx1 + camera_pos.x);
	drawX2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, posx2 + camera_pos.x);

	/* Ypos */
	drawY  = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, posy1 + camera_pos.y);
	drawY2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, posy2 + camera_pos.y);

	/* zoom */
	drawX  = (drawX  - WINDOW_SIZE_X) * camera_pos.zoom / 100 + WINDOW_SIZE_X;
	drawX2 = (drawX2 - WINDOW_SIZE_X) * camera_pos.zoom / 100 + WINDOW_SIZE_X;
	drawY  = (drawY  - WINDOW_SIZE_Y) * camera_pos.zoom / 100 + WINDOW_SIZE_Y;
	drawY2 = (drawY2 - WINDOW_SIZE_Y) * camera_pos.zoom / 100 + WINDOW_SIZE_Y;

	DrawLine(drawX, drawY, drawX2, drawY2, color, thick);
	return;
}

void DrawLineCurveRecField(int posx1, int posy1, int posx2, int posy2, int mode, unsigned int color, int thick) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;

	drawX  = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, posx1 + camera_pos.x);
	drawY  = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, posy1 + camera_pos.y);
	drawX2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, posx2 + camera_pos.x);
	drawY2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, posy2 + camera_pos.y);

	DrawLineCurve(drawX, drawY, drawX2, drawY2, mode, color, thick);
	return;
}

/**
 * プレイ中のメイン画面、item、レーン、キャラにあたる場所に描画します。カメラの影響を受けます。
 */
void DrawDeformationPicRecField(int xpos, int ypos, intx100_t size, int rot, int alpha, DxPic_t pic) {
	int drawX = 0;
	int drawY = 0;
	int drawS = 0;

	//rescale
	drawX = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, xpos);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, ypos);
	drawS = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, size);

	/* zoom */
	drawX = (drawX - WINDOW_SIZE_X) * camera_pos.zoom / 100 + WINDOW_SIZE_X;
	drawY = (drawY - WINDOW_SIZE_Y) * camera_pos.zoom / 100 + WINDOW_SIZE_Y;
	drawS = drawS * camera_pos.zoom / 100;

	//drawing
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawDeformationPic(drawX, drawY, drawS / 100.0, drawS / 100.0, rot, pic);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	return;
}

/**
 * プレイ中の背景、back skyにあたる場所に描画します。カメラの影響を受けます。
 */
void DrawGraphRecBackField(int xpos, int ypos, int pic) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;
	int sizeX = 0;
	int sizeY = 0;

	GetGraphSize(pic, &sizeX, &sizeY);

	/* Xpos */
	drawX = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, xpos + camera_pos.x / 5);
	drawX2 = drawX + sizeX * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;

	/* Ypos */
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, ypos + camera_pos.y / 5);
	drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;

	/* draw */
	DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic, TRUE);
	return;
}
