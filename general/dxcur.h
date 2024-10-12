#pragma once

#include "sancur.h"

#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0xff000000

typedef int DxPic_t; /* DXライブラリのグラフィックのハンドルであることを示す型。int型 */
typedef int DxSnd_t; /* DXライブラリのサウンドのハンドルであることを示す型。int型 */
typedef int DxTime_t; /* DXライブラリのGetNowCount()等で取得した時間であることを示す型。int型 */
typedef unsigned int DxColor_t; /* DXライブラリの色として使える型。unsigned int型 */

/**
 * FPSを計測するクラス
 */
class dxcur_fps_c {
private:
	DxTime_t tick[120];
	uint head = 0;
	uint tail = 0;
	uint num = 0;
public:
	void SetTick();
	uint GetFPS();
};

/**
 * ダブリングを回避したサウンド再生クラス
 */
class dxcur_snd_c {
private:
	DxTime_t Btime = 0; // 前回再生した時間
	DxTime_t Gtime = 2; // 回避する時間
	DxSnd_t mat;
public:
	dxcur_snd_c(TCHAR *path);
	~dxcur_snd_c();
	void PlaySound();
};

extern int GetMouseKeyCur(int* key, int* posX, int* posY, int* PR);
extern void AvoidKeyRush();
extern int GetKeyHold(int n);
extern int GetKeyPushOnce();
extern void InputAllKeyHold();
extern void AvoidKeyBug();
extern int keycur(int const* const mat, int num);