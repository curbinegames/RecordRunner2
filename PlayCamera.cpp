
#include "DxLib.h"
#include "sancur.h"
#include "PlayCamera.h"
#include "RecWindowRescale.h"

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
	RecRescaleDrawGraph(xpos + camera_pos.x, ypos + camera_pos.y, pic, TRUE);
	return;
}

void DrawTurnGraphRecField(int xpos, int ypos, int pic) {
	DrawTurnGraph(xpos + camera_pos.x, ypos + camera_pos.y, pic, TRUE);
	return;
}

void DrawLineRecField(int posx1, int posy1, int posx2, int posy2, unsigned int color, int thick) {
	RecRescaleDrawLine(posx1 + camera_pos.x, posy1 + camera_pos.y,
		posx2 + camera_pos.x, posy2 + camera_pos.y, color, thick);
	return;
}

void DrawGraphRecBackField(int xpos, int ypos, int pic) {
	RecRescaleDrawGraph(xpos + camera_pos.x / 5, ypos + camera_pos.y / 5, pic, TRUE);
	return;
}
