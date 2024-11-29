#pragma once

#include <DxLib.h>

typedef unsigned int uint;
typedef int intx100_t; /** 100”{‚µ‚Ä‚¢‚é‚±‚Æ‚ðŽ¦‚·intŒ^ */

typedef enum cur_move_e {
	CUR_MOVE_LINE = 1,
	CUR_MOVE_ACC,
	CUR_MOVE_DEC
} cur_move_type_t;

extern int mins(int a, int b);
extern int mins_2(int a, int b);
extern int maxs(int a, int b);
extern int maxs_2(int a, int b);
extern double mins_D(double a, double b);
extern double maxs_D(double a, double b);
extern int betweens(int a, int b, int c);
extern double betweens_D(double a, double b, double c);
extern int notzero(int a);
extern double lins(double x1, double y1, double x2, double y2, double x);
extern double pals(double x1, double y1, double x2, double y2, double x);
extern double movecal(int mode, double x1, double y1, double x2, double y2, double x);
extern double sanrute(double c);
extern int abss(int a, int b);
extern int NumLoop(int a, int b);
extern double sinC(int a);
extern double cosC(int a);
extern void rot_xy_pos(int rot, int *x, int *y);
