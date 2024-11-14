
#include "DxLib.h"
#include "../general/sancur.h"
#include "PlayCamera.h"
#include "../RecWindowRescale.h"

static rec_play_xy_set_t camera_pos;

void RecPlayResetCamera() {
	camera_pos.x = 320;
	camera_pos.y = 240;
	return;
}

void RecPlaySetCamera(struct camera_box camset[], int camN, int Ntime) {
	if (camset[camN].starttime <= Ntime && Ntime <= camset[camN].endtime) {
		camera_pos.x = (int)movecal(camset[camN].mode,
			camset[camN].starttime, camset[camN - 1].xpos,
			camset[camN].endtime, camset[camN].xpos, Ntime);
		camera_pos.y = (int)movecal(camset[camN].mode,
			camset[camN].starttime, camset[camN - 1].ypos,
			camset[camN].endtime, camset[camN].ypos, Ntime);
	}
	else {
		camera_pos.x = camset[camN - 1].xpos;
		camera_pos.y = camset[camN - 1].ypos;
	}
	return;
}

void RecPlayGetCameraPos(int *retX, int *retY) {
	if (retX != NULL) { *retX = camera_pos.x; }
	if (retY != NULL) { *retY = camera_pos.y; }
	return;
}

void DrawGraphRecField(int xpos, int ypos, int pic) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;
	int sizeX = 0;
	int sizeY = 0;

	GetGraphSize(pic, &sizeX, &sizeY);

	/* Xpos */
	drawX = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, xpos + camera_pos.x);
	drawX2 = drawX + sizeX * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;

	/* Ypos */
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, ypos + camera_pos.y);
	drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;

	/* draw */
	DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic, TRUE);
	return;
}

void DrawTurnGraphRecField(int xpos, int ypos, int pic) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;
	int sizeX = 0;
	int sizeY = 0;

	GetGraphSize(pic, &sizeX, &sizeY);

	/* Xpos */
	drawX = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, xpos + camera_pos.x);
	drawX2 = drawX + sizeX * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;

	/* Ypos */
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, ypos + camera_pos.y);
	drawY2 = drawY + sizeY * WINDOW_SIZE_Y / OLD_WINDOW_SIZE_Y;

	/* draw */
	DrawExtendGraph(drawX2, drawY, drawX, drawY2, pic, TRUE);
	return;
}

void DrawLineRecField(int posx1, int posy1, int posx2, int posy2, unsigned int color, int thick) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, posx1 + camera_pos.x);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, posy1 + camera_pos.y);
	drawX2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, posx2 + camera_pos.x);
	drawY2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, posy2 + camera_pos.y);
	DrawLine(drawX, drawY, drawX2, drawY2, color, thick);
	return;
}

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
