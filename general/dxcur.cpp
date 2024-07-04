#pragma once

#include "DxLib.h"
#include "sancur.h"

static int keyhold[256];

int GetMouseKeyCur(int* key, int* posX, int* posY, int* PR) {
	while (GetMouseInputLog2(key, posX, posY, PR, true) == 0) {}
	return 0;
}

void AvoidKeyRush() {
	for (int i = 0; i < 256; i++) {
		keyhold[i] = 2;
	}
	return;
}

int GetKeyHold(int n) {
	return keyhold[n];
}

int GetKeyPushOnce() {
	for (int i = 0; i < 256; i++) {
		if (keyhold[i] == 1) {
			return i;
		}
	}
	return -1;
}

void InputAllKeyHold() {
	char recv[256];
	GetHitKeyStateAll(recv);
	for (int i = 0; i < 256; i++) {
		if (recv[i] == 1) {
			keyhold[i]++;
		}
		else {
			keyhold[i] = 0;
		}
	}
	return;
}

void AvoidKeyBug() {
	/* nop */
}

int keycur(int const* const mat, int num) {
	return GetKeyPushOnce();
}
