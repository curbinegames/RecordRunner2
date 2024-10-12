#include <DxLib.h>
#include "dxcur.h"
#include "sancur.h"

/* dxcur_fps_c */
void dxcur_fps_c::SetTick() {
	DxTime_t Ntime = GetNowCount();

	this->tick[this->head] = Ntime;
	this->head = (this->head + 1) % 120;
	this->num++;
	while (120 < this->num || 0 < this->num && this->tick[this->tail] + 1000 < Ntime) {
		this->tail = (this->tail + 1) % 120;
		this->num--;
	}
}

uint dxcur_fps_c::GetFPS() {
	return this->num;
}

/* dxcur_snd_c */
dxcur_snd_c::dxcur_snd_c(TCHAR *path) {
	this->mat = LoadSoundMem(path);
}

dxcur_snd_c::~dxcur_snd_c() {
	DeleteSoundMem(this->mat);
}

void dxcur_snd_c::PlaySound() {
	DxTime_t Ntime = GetNowCount();
	if ((this->Btime + this->Gtime) < Ntime) {
		PlaySoundMem(this->mat, DX_PLAYTYPE_BACK);
		this->Btime = Ntime;
	}
}

/*+++*/

static int keyhold[256];

/**
 * マウスのキー入力の情報を返す
 * @param[out] key 反応したボタンの種類
 * @param[out] posX 反応したX位置
 * @param[out] posY 反応したy位置
 * @param[out] key 反応の種類, 0: 押した, 1: 離した
 * @return 0のみ
 */
int GetMouseKeyCur(int* key, int* posX, int* posY, int* PR) {
	while (GetMouseInputLog2(key, posX, posY, PR, true) == 0) {}
	return 0;
}

/**
 * キー入力のバグを回避する関数
 * @return なし
 */
void AvoidKeyRush() {
	for (int i = 0; i < 256; i++) {
		keyhold[i] = 2;
	}
	return;
}

/**
 * 指定されたキーの状態を返す
 * @return 0: 押していない, 1: たった今押された, 2以上: 長押ししている
 */
int GetKeyHold(int n) {
	return keyhold[n];
}

/**
 * たった今押したキーを1個だけ返す。同時押しには対応していない
 * @return たった今押したキー
 * @note この関数を使用する前に、InputAllKeyHoldでキー情報を更新してください
 * @note InputAllKeyHoldを使用しないと、前回と同じ値が返ります
 */
int GetKeyPushOnce() {
	for (int i = 0; i < 256; i++) {
		if (keyhold[i] == 1) {
			return i;
		}
	}
	return -1;
}

/**
 * キー情報を更新する
 * @return なし
 */
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

/**
 * 今はもう何もしない関数。使用している場所があるため残している
 * @return なし
 */
void AvoidKeyBug() {
	/* nop */
}

/**
 * この関数はもう古いので、代わりにInputAllKeyHold(),GetKeyPushOnce()を使ってください。
 * @param[in] mat 未使用
 * @param[in] num 未使用
 * @return たった今押したキー
 * @details たった今押したキーを1個だけ返す。同時押しには対応していない
 */
int keycur(int const* const mat, int num) {
	InputAllKeyHold();
	return GetKeyPushOnce();
}