#pragma once

#include <RecScoreFile.h>
#include <playbox.h>

/* main action */
extern void RecPlaySetCamera(
    dxcur_camera_c &camera_pos, const cvec<rec_camera_data_t> &camera, int Ntime
);

/* Draw on Rec Field */
extern void DrawStringRecField(
    const dxcur_camera_c &camera_pos, int xpos, int ypos, const tstring &str, DxColor_t cr
);
extern void DrawLineRecField(
	const dxcur_camera_c &camera_pos, int posx1, int posy1, int posx2, int posy2,
	unsigned int color, int thick
);
extern void DrawLineCurveRecField(
	const dxcur_camera_c &camera_pos, int posx1, int posy1, int posx2, int posy2,
	int mode, DxColor_t color, int thick
);
extern void DrawDeformationPicRecField(
	const dxcur_camera_c &camera_pos, int xpos, int ypos,
	intx100_t size, int rot, int alpha, DxPic_t pic
);
extern void DrawGraphRecBackField(
	const dxcur_camera_c &camera_pos, int xpos, int ypos, DxPic_t pic
);
