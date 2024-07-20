#pragma once

#include "DxLib.h"

extern int mins(int a, int b);
extern int mins_2(int a, int b);
extern int maxs(int a, int b);
extern int maxs_2(int a, int b);
extern double mins_D(double a, double b);
extern double maxs_D(double a, double b);
extern int betweens(int a, int b, int c);
extern double betweens_D(double a, double b, double c);
extern int notzero(int a);
extern int lins(double x1, double y1, double x2, double y2, double x);
extern int pals(double x1, double y1, double x2, double y2, double x);
extern double movecal(int mode, double x1, double y1, double x2, double y2, double x);
extern double sanrute(double c);
extern int abss(int a, int b);
extern int NumLoop(int a, int b);
extern double sinC(int a);
extern double cosC(int a);
extern void rot_xy_pos(int rot, int *x, int *y);
