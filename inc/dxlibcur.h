#pragma once

#if 0
#include "sancur.h"
#include "DxLib.h"

static int key = 1;

int GetMouseKeyCur(int* key, int* posX, int* posY, int* PR) {
	while (GetMouseInputLog2(key, posX, posY, PR, true) == 0) {}
	return 0;
}

void AvoidKeyBug() {
	key = 1;
}

int keycur(int const* const mat, int num) {
	char push[256];
	for (int i = 0; i < 256; i++) { push[i] = 5; }
	GetHitKeyStateAll(&push[0]);
	for (int i = 0; i < num; i++) {
		if (push[mat[i]] == 1) {
			if (key == 1) {
				key = 1;
				return 0;
			}
			key = 1;
			return mat[i];
		}
	}
	key = 0;
	return 0;
}
#endif
