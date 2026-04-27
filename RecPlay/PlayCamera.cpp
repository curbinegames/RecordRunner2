
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

void RecPlaySetCamera(dxcur_camera_c &camera_pos, const cvec<rec_camera_data_t> &camera, int Ntime) {
	const DxTime_t startT = camera.nowData().starttime;
	const DxTime_t   endT = camera.nowData().endtime;
	const      int  moveM = camera.nowData().mode;
	const      int  nposX = camera.nowData().xpos;
	const      int  nposY = camera.nowData().ypos;
	const   double  nZoom = camera.nowData().zoom;
	const   double  nRot  = camera.nowData().rot;
	const      int  bposX = camera.offsetData(-1).xpos;
	const      int  bposY = camera.offsetData(-1).ypos;
	const   double  bZoom = camera.offsetData(-1).zoom;
	const   double  bRot  = camera.offsetData(-1).rot;

	int    setX    = 0;
	int    setY    = 0;
	double setZoom = 1.0;
	double setRot  = 0.0;

	/* ãåâÊñ î‰ó¶ï‚ê≥ */
	setX    -= (WINDOW_SIZE_Y - OLD_WINDOW_SIZE_Y);
	setY    -= (WINDOW_SIZE_Y - OLD_WINDOW_SIZE_Y) / 2;
	setZoom *=  WINDOW_SIZE_Y / (double)OLD_WINDOW_SIZE_Y;

	if (startT <= Ntime && Ntime <= endT) {
		setX    -= movecal(moveM, startT, bposX, endT, nposX, Ntime);
		setY    -= movecal(moveM, startT, bposY, endT, nposY, Ntime);
		setZoom *= movecal(moveM, startT, bZoom, endT, nZoom, Ntime);
		setRot  += movecal(moveM, startT, bRot , endT, nRot , Ntime);
	}
	else {
		setX    -= nposX;
		setY    -= nposY;
		setZoom *= nZoom;
		setRot  += nRot;
	}

	camera_pos.setX(       setX   );
	camera_pos.setY(       setY   );
	camera_pos.setZoom(    setZoom);
	camera_pos.setAngleDeg(setRot );
}

void DrawStringRecField(
	const dxcur_camera_c &camera_pos, int xpos, int ypos, const tstring &str, DxColor_t cr
) {
	double drawX = xpos;
	double drawY = ypos;
	camera_pos.WorldToScreen(drawX, drawY);
	DrawString(drawX, drawY, str.c_str(), cr);
}

void DrawLineRecField(
	const dxcur_camera_c &camera_pos, int posx1, int posy1, int posx2, int posy2,
	unsigned int color, int thick
) {
	double drawX  = posx1;
	double drawY  = posy1;
	double drawX2 = posx2;
	double drawY2 = posy2;
	camera_pos.WorldToScreen(drawX , drawY );
	camera_pos.WorldToScreen(drawX2, drawY2);
	DrawLine(drawX, drawY, drawX2, drawY2, color, thick);
}

void DrawLineCurveRecField(
	const dxcur_camera_c &camera_pos, int posx1, int posy1, int posx2, int posy2,
	int mode, DxColor_t color, int thick
) {
	double drawX  = posx1;
	double drawY  = posy1;
	double drawX2 = posx2;
	double drawY2 = posy2;
	camera_pos.WorldToScreen(drawX , drawY );
	camera_pos.WorldToScreen(drawX2, drawY2);
	DrawLineCurve(drawX, drawY, drawX2, drawY2, mode, color, thick);
}

void DrawDeformationPicRecField(
	const dxcur_camera_c &camera_pos, int xpos, int ypos,
	intx100_t size, int rot, int alpha, DxPic_t pic
) {
	double drawX = xpos;
	double drawY = ypos;
	double drawS = size * camera_pos.getZoom() / 100.0;
	double drawR = rot  - camera_pos.getAngleDeg();
	camera_pos.WorldToScreen(drawX, drawY);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawDeformationPic(drawX, drawY, drawS / 100.0, drawS / 100.0, rot, pic);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
}

void DrawGraphRecBackField(const dxcur_camera_c &camera_pos, int xpos, int ypos, DxPic_t pic) {
	double drawX = xpos + camera_pos.getX() / 5;
	double drawY = ypos + camera_pos.getY() / 5;
	camera_pos.drawpic(drawX, drawY, pic);
}
