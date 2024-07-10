#include "DxLib.h"
#include "../general/sancur.h"
#include "playbox.h"
#include "../RecWindowRescale.h"

/* define */
#define NO_MISS 2
#define FULL_COMBO 1
#define PERFECT 0
#define BIG 8
#define SIZE_X 319
#define SIZE_Y 54
#define PIC_X 320
#define PIC_Y 240

/* typedef */
typedef struct rec_play_bonus_small_light_picture_s {
	int perfect;
	int fullcombo;
	int nomiss;
} rec_play_bonus_small_light__picture_t;

typedef struct rec_play_bonus_picture_s {
	int perfect;
	int fullcombo;
	int nomiss;
	int BigLight;
	rec_play_bonus_small_light__picture_t SmallLight;
	int flash;
	int ring;
	int filter;
} rec_play_bonus_picture_t;

typedef struct rec_play_bonus_sound_s {
	int perfect;
	int fullcombo;
	int nomiss;
} rec_play_bonus_sound_t;

typedef struct rec_play_bonus_psmat_s {
	rec_play_bonus_picture_t pic;
	rec_play_bonus_sound_t snd;
} rec_play_bonus_psmat_t;

/* global */
static rec_play_bonus_psmat_t g_bonus_psmat;

/* init */
void ReadyBonusPsmat() {
	g_bonus_psmat.pic.perfect = LoadGraph(L"picture/PERFECT.png");
	g_bonus_psmat.pic.fullcombo = LoadGraph(L"picture/FULLCOMBO.png");
	g_bonus_psmat.pic.nomiss = LoadGraph(L"picture/NOMISS.png");
	g_bonus_psmat.pic.BigLight = LoadGraph(L"picture/Bonus-Biglight.png");
	g_bonus_psmat.pic.SmallLight.perfect = LoadGraph(L"picture/Bonus-Smalllight3.png");
	g_bonus_psmat.pic.SmallLight.fullcombo = LoadGraph(L"picture/Bonus-Smalllight2.png");
	g_bonus_psmat.pic.SmallLight.nomiss = LoadGraph(L"picture/Bonus-Smalllight1.png");
	g_bonus_psmat.pic.flash = LoadGraph(L"picture/White.png");
	g_bonus_psmat.pic.ring = LoadGraph(L"picture/Bonus-Ring.png");
	g_bonus_psmat.pic.filter = LoadGraph(L"picture/Black.png");
	g_bonus_psmat.snd.perfect = LoadSoundMem(L"sound/a-perfect.mp3");
	g_bonus_psmat.snd.fullcombo = LoadSoundMem(L"sound/a-fullcombo.mp3");
	g_bonus_psmat.snd.nomiss = LoadSoundMem(L"sound/a-nomiss.mp3");
	return;
}

/* action */
void ShowBonusEff(struct judge_box judge, int EffStartTime) {
	int pic = 0;
	int Snd = 0;
	int smalllight = 0;
	int Bonus = -1;

	if (judge.miss > 0) {
		return;
	}
	else if (judge.safe > 0) {
		Bonus = NO_MISS;
		pic = g_bonus_psmat.pic.nomiss;
		Snd = g_bonus_psmat.snd.nomiss;
		smalllight = g_bonus_psmat.pic.SmallLight.nomiss;
	}
	else if (judge.good > 0) {
		Bonus = FULL_COMBO;
		pic = g_bonus_psmat.pic.fullcombo;
		Snd = g_bonus_psmat.snd.fullcombo;
		smalllight = g_bonus_psmat.pic.SmallLight.fullcombo;
	}
	else {
		Bonus = PERFECT;
		pic = g_bonus_psmat.pic.perfect;
		Snd = g_bonus_psmat.snd.perfect;
		smalllight = g_bonus_psmat.pic.SmallLight.perfect;
	}
	if (EffStartTime >= GetNowCount()) {
		PlaySoundMem(Snd, DX_PLAYTYPE_BACK);
	}

	//BlackCover
	if (GetNowCount() < EffStartTime + 1800) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 127);
		RecRescaleDrawGraph(0, 0, g_bonus_psmat.pic.filter, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	if (EffStartTime + 1800 <= GetNowCount() && GetNowCount() < EffStartTime + 2000) {
		int alpha = lins(1800, 127, 2000, 0, GetNowCount() - EffStartTime);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		RecRescaleDrawGraph(0, 0, g_bonus_psmat.pic.filter, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	//SmallLight
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 4 - Bonus; k++) {
				int posX = j * 180 + EffStartTime % (57137 + 29 * i + 67 * j + 127 * k) % 180;
				int posY = i * 190 + EffStartTime % (62843 + 37 * i + 67 * j + 157 * k) % 190;
				int alpha = lins(100, 720, 1000, -240, GetNowCount() - EffStartTime);
				alpha = mins(pals(posY, 255, posY + 240, 0, alpha), 0);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
				RecRescaleDrawGraph(posX, posY, smalllight, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
	}
	//BigLight
	if (100 <= GetNowCount() - EffStartTime && GetNowCount() - EffStartTime <= 1000) {
		for (int i = 0; i < 3 - Bonus; i++) {
			int alpha = lins(500, 255, 1000, 0, mins(GetNowCount() - EffStartTime, 500));
			double angle = double(GetNowCount() - EffStartTime) / 200.0;
			angle += 3.14 * (180.0 / (3 - Bonus)) * i / 180.0;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			RecRescaleDrawRotaGraph(PIC_X, PIC_Y, 1, angle, g_bonus_psmat.pic.BigLight, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}
	//ring
	if (100 <= GetNowCount() - EffStartTime && GetNowCount() - EffStartTime <= 1000) {
		int LeftPos  = lins(100, PIC_X - 160, 1000, PIC_X - 240, GetNowCount() - EffStartTime);
		int UpPos    = lins(100, PIC_Y - 160, 1000, PIC_Y - 240, GetNowCount() - EffStartTime);
		int RightPos = lins(100, PIC_X + 160, 1000, PIC_X + 240, GetNowCount() - EffStartTime);
		int DownPos  = lins(100, PIC_Y + 160, 1000, PIC_Y + 240, GetNowCount() - EffStartTime);
		int alpha    = maxs(lins(700, 255, 1000, 0, GetNowCount() - EffStartTime), 255);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		RecRescaleDrawExtendGraph(LeftPos, UpPos, RightPos, DownPos, g_bonus_psmat.pic.ring, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	//flash
	if (100 <= GetNowCount() - EffStartTime && GetNowCount() - EffStartTime <= 300) {
		for (int i = 0; i < 3 - Bonus; i++) {
			int alpha = lins(100, 191, 300, 0, GetNowCount() - EffStartTime);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			RecRescaleDrawGraph(0, 0, g_bonus_psmat.pic.flash, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}
	//BonusText
	if (GetNowCount() < EffStartTime + 100) {
		int LeftPos  = lins(0, PIC_X - SIZE_X * BIG / 2, 100, PIC_X - SIZE_X / 2, GetNowCount() - EffStartTime);
		int UpPos    = lins(0, PIC_Y - SIZE_Y * BIG / 2, 100, PIC_Y - SIZE_Y / 2, GetNowCount() - EffStartTime);
		int RightPos = lins(0, PIC_X + SIZE_X * BIG / 2, 100, PIC_X + SIZE_X / 2, GetNowCount() - EffStartTime);
		int DownPos  = lins(0, PIC_Y + SIZE_Y * BIG / 2, 100, PIC_Y + SIZE_Y / 2, GetNowCount() - EffStartTime);
		RecRescaleDrawExtendGraph(LeftPos, UpPos, RightPos, DownPos, pic, TRUE);
	}
	else if (EffStartTime + 100 <= GetNowCount() && GetNowCount() < EffStartTime + 1800) {
		RecRescaleDrawGraph(PIC_X - SIZE_X / 2, PIC_Y - SIZE_Y / 2, pic, TRUE);
	}
	else if (EffStartTime + 1800 <= GetNowCount() && GetNowCount() < EffStartTime + 2000) {
		int LeftPos  = lins(2000, PIC_X - SIZE_X, 1800, PIC_X - SIZE_X / 2, GetNowCount() - EffStartTime);
		int UpPos    = lins(2000, PIC_Y - SIZE_Y, 1800, PIC_Y - SIZE_Y / 2, GetNowCount() - EffStartTime);
		int RightPos = lins(2000, PIC_X + SIZE_X, 1800, PIC_X + SIZE_X / 2, GetNowCount() - EffStartTime);
		int DownPos  = lins(2000, PIC_Y + SIZE_Y, 1800, PIC_Y + SIZE_Y / 2, GetNowCount() - EffStartTime);
		int alpha    = lins(1800, 255, 2000, 0, GetNowCount() - EffStartTime);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		RecRescaleDrawExtendGraph(LeftPos, UpPos, RightPos, DownPos, pic, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	return;
}
