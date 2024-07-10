
#include "DxLib.h"
#include "../general/sancur.h"
#include "../RecWindowRescale.h"
#include "playbox.h"
#include "PlayCamera.h"

static int JudgePic[4];
static int viewjudge[4] = { 0,0,0,0 };

void ReadyJudgePicture() {
	JudgePic[0] = LoadGraph(L"picture/judge-just.png");
	JudgePic[1] = LoadGraph(L"picture/judge-good.png");
	JudgePic[2] = LoadGraph(L"picture/judge-safe.png");
	JudgePic[3] = LoadGraph(L"picture/judge-miss.png");
	return;
}

void PlaySetJudge(note_judge judge) {
	viewjudge[judge] = GetNowCount();
	return;
}

void PlayShowJudge(int posMode, int charaposX, int charaposY) {
	int BaseDrawX = 0;
	int BaseDrawY = 0;
	int drawY = 0;
	switch (posMode) {
	case 0:
		BaseDrawX = 270;
		BaseDrawY = 100;
		break;
	case 1:
		BaseDrawX = 10;
		BaseDrawY = 100;
		break;
	case 2:
		BaseDrawX = 530;
		BaseDrawY = 100;
		break;
	case 3:
		BaseDrawX = 270;
		BaseDrawY = 260;
		break;
	case 4:
		RecPlayGetCameraPos(&BaseDrawX, &BaseDrawY);
		BaseDrawX += charaposX - 120;
		BaseDrawY += charaposY - 100;
		break;
	default:
		return;
	}
	for (int i = 0; i < 4; i++) {
		if (GetNowCount() - viewjudge[i] < 750) {
			drawY = BaseDrawY + pals(250, 0, 0, 25, maxs(GetNowCount() - viewjudge[i], 250));
			RecRescaleDrawGraph(BaseDrawX, drawY, JudgePic[i], TRUE);
		}
	}
	return;
}