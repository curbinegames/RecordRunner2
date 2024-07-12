#pragma once

#define COLOR_WHITE 0xffffffff

typedef int DxPic_t; /* DXライブラリのグラフィックのハンドルであることを示す型。int型 */
typedef int DxSnd_t; /* DXライブラリのサウンドのハンドルであることを示す型。int型 */

extern int GetMouseKeyCur(int* key, int* posX, int* posY, int* PR);
extern void AvoidKeyRush();
extern int GetKeyHold(int n);
extern int GetKeyPushOnce();
extern void InputAllKeyHold();
extern void AvoidKeyBug();
extern int keycur(int const* const mat, int num);
