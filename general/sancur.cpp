/**
 * @file sancur.cpp
 * @brief 数値計算で便利なものを集めたコード
 * @author curbine
 * @date 2024/7/19
 * @note 個人が趣味で製作したものなので、品質は保証しません。このコードを使用したことによるトラブルの責任は負いかねます。
 */

#include <stdbool.h>
#include "sancur.h"

/**
 * 小さいほうを返す
 * @param[in] a 入力1
 * @param[in] b 入力2
 * @return int 小さいほうの数値
 */
int mins_2(int a, int b) {
	if (a > b) { return b; }
	return a;
}

/**
 * 本来のmax関数と用途が逆の為、この関数はもう使っちゃダメ、代わりにmins_2を使ってください
 * @param[in] a 入力1
 * @param[in] b 入力2
 * @return int 小さいほうの数値
 * @details 小さいほうを返す
 */
int maxs(int a, int b) {
	return mins_2(a, b);
}

/**
 * 大きいほうを返す
 * @param[in] a 入力1
 * @param[in] b 入力2
 * @return int 大きいほうの数値
 */
int maxs_2(int a, int b) {
	if (a < b) { return b; }
	return a;
}

/**
 * 本来のmin関数と用途が逆の為、この関数はもう使っちゃダメ、代わりにmaxs_2を使ってください
 * @param[in] a 入力1
 * @param[in] b 入力2
 * @return int 大きいほうの数値
 * @details 大きいほうを返す
 */
int mins(int a, int b) {
	return maxs_2(a, b);
}

/**
 * aをbまで引き上げたものを返す
 * @param[in] a 入力1
 * @param[in] b 入力2
 * @return double 大きいほうの数値
 */
double mins_D(double a, double b) {
	if (a < b) { a = b; }
	return a;
}

/**
 * aをbまで引き下げたものを返す
 * @param[in] a 入力1
 * @param[in] b 入力2
 * @return double 小さいほうの数値
 */
double maxs_D(double a, double b) {
	if (a > b) { a = b; }
	return a;
}

/**
 * bをaまで引き上げ、cまで引き下げたものを返す
 * @param[in] a 下限
 * @param[in] b 検査する数値
 * @param[in] c 上限
 * @return int aで引き上げ、cで引き下げたb
 */
int betweens(int a, int b, int c) {
	if (a > b) { return a; }
	if (b > c) { return c; }
	return b;
}

/**
 * bをaまで引き上げ、cまで引き下げたものを返す
 * @param[in] a 下限
 * @param[in] b 検査する数値
 * @param[in] c 上限
 * @return double aで引き上げ、cで引き下げたb
 */
double betweens_D(double a, double b, double c) {
	if (a > b) { return a; }
	if (b > c) { return c; }
	return b;
}

/**
 * aが0だったら1にする
 * @param[in] a 検査する数値
 * @return int aが0だったら1, それ以外ならa
 */
int notzero(int a) {
	if (a == 0) { a = 1; }
	return a;
}

/**
 * a / b を返す。ただしbが0だった場合はcを返す。
 * @param[in] a 割られる数
 * @param[in] b 割る数
 * @param[in] c 0割り時の返り値
 * @return int a / b, 0割り時はc
 */
#define divzero(a, b, c) (((b) == 0) ? (c) : ((a) / (b)))

/**
 * 点1(x1,y1)、点2(x2,y2)、を通る直線を考えて、xの時の値を返す。
 * @param[in] x1 点1のx座標
 * @param[in] y1 点1のy座標
 * @param[in] x2 点2のx座標
 * @param[in] y2 点2のy座標
 * @param[in] x 評価する値
 * @return double 点1(x1,y1)、点2(x2,y2)、を通る直線を考えた時のxの値
 */
double lins(double x1, double y1, double x2, double y2, double x) {
	if (x1 == x2) { return x1; }
	return (y2 - y1) * (x - x1) / (x2 - x1) + y1;
}

/**
 * 頂点(x1,y1)、通過点(x2,y2)、を通る二次関数を考えて、xの時の値を返す。
 * @param[in] x1 頂点のx座標
 * @param[in] y1 頂点のy座標
 * @param[in] x2 通過点のx座標
 * @param[in] y2 通過点のy座標
 * @param[in] x 評価する値
 * @return double 頂点(x1,y1)、通過点(x2,y2)を通る二次関数を考えた時のxの値
 */
double pals(double x1, double y1, double x2, double y2, double x) {
	double s1 = 0;
	double s2 = 0;
	if (x1 == x2) { return x1; }
	s1 = x - x1;
	s2 = x2 - x1;
	return (y2 - y1) * s1 * s1 / (s2 * s2) + y1;
}

/**
 * mode(1:lin,2:acc,3:dec)に合わせた計算をする。
 * @param[in] mode 線の種類
 * @param[in] x1 始点のx座標
 * @param[in] y1 始点のy座標
 * @param[in] x2 終点のx座標
 * @param[in] y2 終点のy座標
 * @param[in] x 評価する値
 * @return double 始点(x1,y1)、終点(x2,y2)をmode法で通る線を考えた時のxの値
 */
double movecal(int mode, double startx, double starty, double endx, double endy, double nowx) {
	switch (mode) {
	case 1:
		return lins(startx, starty, endx, endy, nowx);
	case 2:
		return pals(startx, starty, endx, endy, nowx);
	case 3:
		return pals(endx, endy, startx, starty, nowx);
	default:
		return 0;
	}
}

/**
 * ルートcを返す
 * @param[in] c 評価する値
 * @return double ルート(c)
 */
double sanrute(double c) {
	short int d = 10;
	double a = 0, b = 1;
	while (b * b < c) { b *= 10; }
	if (b * b == c) { return b; }
	b /= 10;
	while (b >= 100000 || d == -10) {
		while (a * a < c) { a += b; }
		if (a * a == c) { return a; }
		a -= b;
		b /= d;
		if (b == 1) { d *= -1; }
	}
	return a;
}

/**
 * aとbの差の絶対値を求める
 * @param[in] a 入力1
 * @param[in] b 入力2
 * @return int |a-b|
 */
int abss(int a, int b) {
	a -= b;
	if (a < 0) { a *= -1; }
	return a;
}

/**
 * aを0からbでループさせる
 * @param[in] a 評価する値
 * @param[in] b 最大値
 * @return int a % (b + 1)
 * @note この関数は使う必要なし。演算子で頑張ってください。
 */
int NumLoop(int a, int b) {
	while (a < 0) { a += b; }
	while (a > b) { a -= b; }
	return a;
}

/**
 * sin(a)を出力する。単位はdeg
 * @param[in] a 入力
 * @return double sin(a)
 */
double sinC(int a) {
	bool minFg = false;
	double ret = 0;
	double sin[91] = {
		0.0000,0.0175,0.0349,0.0523,0.0698,
		0.0872,0.1045,0.1219,0.1392,0.1564,
		0.1736,0.1908,0.2079,0.2250,0.2419,
		0.2588,0.2756,0.2924,0.3090,0.3256,
		0.3420,0.3584,0.3746,0.3907,0.4067,
		0.4226,0.4384,0.4540,0.4695,0.4848,
		0.5000,0.5150,0.5299,0.5446,0.5592,
		0.5736,0.5878,0.6018,0.6157,0.6293,
		0.6428,0.6561,0.6691,0.6820,0.6947,
		0.7071,0.7193,0.7314,0.7431,0.7547,
		0.7660,0.7771,0.7880,0.7986,0.8090,
		0.8192,0.8290,0.8387,0.8480,0.8572,
		0.8660,0.8746,0.8829,0.8910,0.8988,
		0.9063,0.9135,0.9205,0.9272,0.9336,
		0.9397,0.9455,0.9511,0.9563,0.9613,
		0.9659,0.9703,0.9744,0.9781,0.9816,
		0.9848,0.9877,0.9903,0.9925,0.9945,
		0.9962,0.9976,0.9986,0.9994,0.9998,1.0000
	};

	/* a -> -359~359 */
	a %= 360;
	/* a -> 0~359 */
	if (a < 0) {
		a += 360;
	}
	/* a -> 0~179 */
	if (180 <= a) {
		a -= 180;
		minFg = true;
	}
	/* a -> 0~90 */
	if (90 < a) {
		a = 180 - a;
	}

	return (minFg ? -1 : 1) * sin[a];
}

/**
 * cos(a)を出力する。単位はdeg
 * @param[in] a 入力
 * @return double cos(a)
 */
double cosC(int a) {
	return sinC(a + 90);
}

/**
 * 点(x, y)を　原点(0, 0)を中心にrot度右回転させる
 * @param[in] rot 角度
 * @param[out] x x座標
 * @param[out] y y座標
 */
void rot_xy_pos(int rot, int *x, int *y) {
	int G = *x;
	*x = *x * cosC(rot) - *y * sinC(rot);
	*y = G * sinC(rot) + *y * cosC(rot);
	return;
}