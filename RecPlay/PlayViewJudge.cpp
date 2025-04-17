
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <sancur.h>

/* rec system include */
#include <option.h>
#include <playbox.h>
#include <PlayCamera.h>
#include <RecWindowRescale.h>

static dxcur_pic_c JudgePic[4];
static int viewjudge[4] = { 0,0,0,0 };

void ReadyJudgePicture() {
	JudgePic[0].reload(L"picture/judge-just.png");
	JudgePic[1].reload(L"picture/judge-good.png");
	JudgePic[2].reload(L"picture/judge-safe.png");
	JudgePic[3].reload(L"picture/judge-miss.png");
	return;
}

void PlaySetJudge(note_judge judge) {
	if (judge == NOTE_JUDGE_PJUST) {
		viewjudge[0] = GetNowCount();
	}
	else {
		viewjudge[judge] = GetNowCount();
	}
	return;
}

void PlayShowJudge(rec_play_lanepos_t *lanePos, int charaput) {
	int BaseDrawX = 0;
	int BaseDrawY = 0;
	int drawY = 0;
	switch (optiondata.combopos) {
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
		BaseDrawX += lanePos->x[charaput] - 120;
		BaseDrawY += lanePos->y[charaput] - 100;
		break;
	default:
		return;
	}
	for (int i = 0; i < 4; i++) {
		if (GetNowCount() - viewjudge[i] < 750) {
			drawY = BaseDrawY + pals(250, 0, 0, 25, mins_2(GetNowCount() - viewjudge[i], 250));
			RecRescaleDrawGraph(BaseDrawX, drawY, JudgePic[i].handle(), TRUE);
		}
	}
	return;
}