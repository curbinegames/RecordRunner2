#pragma once

#include <RecScoreFile.h>
#include <playbox.h>

/* main action */
extern void RecPlayResetCamera();
extern void RecPlaySetCamera(rec_camera_set_t *camera, int Ntime);
extern void RecPlayGetCameraPos(int *retX, int *retY);

/* Draw on Rec Field */
extern void DrawGraphRecField(int xpos, int ypos, int pic);
extern void DrawTurnGraphRecField(int xpos, int ypos, int pic);
extern void DrawLineRecField(int posx1, int posy1, int posx2, int posy2, unsigned int color, int thick);
extern void DrawGraphRecBackField(int xpos, int ypos, int pic);
