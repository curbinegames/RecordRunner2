
#include "DxLib.h"
#include "sancur.h"

#define RECR_DEBUG(ofs, data)											\
		DrawFormatString(20, 120 + ofs * 20, 0xffffffff, L#data": %d", data)

static int pic_cutin[3];

void CutinReady() {
	pic_cutin[0] = LoadGraph(L"picture/cutin/cutinU.png");
	pic_cutin[1] = LoadGraph(L"picture/cutin/cutinD.png");
	pic_cutin[2] = LoadGraph(L"picture/cutin/cutinDisk.png");
}

void ViewCutIn(int Stime) {
	int EffTime = maxs(GetNowCount() - Stime, 500);
	int PosY = pals(500, 0, 0, 360, EffTime);
	int Rot = pals(500, 0, 0, -300, EffTime);
	DrawGraph(0, 0 - PosY, pic_cutin[0], TRUE);
	DrawGraph(0, 240 + PosY, pic_cutin[1], TRUE);
	DrawRotaGraph(320 - PosY, 240 - PosY, 1,
		(double)Rot / 100.0, pic_cutin[2], TRUE);
	return;
}

void ViewCutOut(int Stime) {
	int EffTime = maxs(GetNowCount() - Stime, 500);
	int PosY = pals(0, 0, 500, 360, EffTime);
	int Rot = pals(0, 0, 500, 300, EffTime);
	DrawGraph(0, 0 - PosY, pic_cutin[0], TRUE);
	DrawGraph(0, 240 + PosY, pic_cutin[1], TRUE);
	DrawRotaGraph(320 + PosY, 240 - PosY, 1,
		(double)Rot / 100.0, pic_cutin[2], TRUE);
	return;
}
