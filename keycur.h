#pragma once

#include "DxLib.h"

int keycur(int const* const mat, int num) {
	static int key;
	char push[256];
	for (int i = 0; i < 256; i++) {
		push[i] = 5;
	}
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
