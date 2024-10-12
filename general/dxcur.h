#pragma once

#include <DxLib.h>

#define COLOR_WHITE GetColor(255, 255, 255)
#define COLOR_BLACK GetColor(0, 0, 0)
#define COLOR_RED GetColor(255, 0, 0)
#define COLOR_GREEN GetColor(0, 255, 0)
#define COLOR_BLUE GetColor(0, 0, 255)

#define DXLIB_PIC_HAND_DEFAULT -1

typedef unsigned int DxColor_t; /* DXライブラリの色情報であることを示す型。unsigned int型 */
typedef int DxPic_t; /* DXライブラリのグラフィックのハンドルであることを示す型。int型 */
typedef int DxSnd_t; /* DXライブラリのサウンドのハンドルであることを示す型。int型 */
typedef int DxFile_t; /* DXライブラリのファイルのハンドルであることを示す型。int型 */

extern int GetMouseKeyCur(int* key, int* posX, int* posY, int* PR);
extern void AvoidKeyRush();
extern int GetKeyHold(int n);
extern int GetKeyPushOnce();
extern void InputAllKeyHold();
extern void AvoidKeyBug();
extern int keycur(int const* const mat, int num);
