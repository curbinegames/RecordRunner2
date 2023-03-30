
#include "DxLib.h"
#include "sancur.h"
#include "strcur.h"
#include "recr_cutin.h"

#define RECR_DEBUG(ofs, data)											\
		DrawFormatString(20, 120 + ofs * 20, 0xffffffff, L#data": %d", data)

static char TipNo = 0;
static int pic_cutin[4];
static wchar_t SongJucketName[256] = { L"NULL" };
static wchar_t CutSongName[256] = { L"NULL" };
static cutin_tips_e CutFg = CUTIN_TIPS_NONE;

static wchar_t const tip[][30] = {
	L"���y�͖����ɉe����^����",
	L"HIT�m�[�c�͓�������OK",
	L"CATCH�m�[�c�̔���͑O��ɂ���",
	L"BOMB�m�[�c�̔���͋ɒ[�ɋ���",
	L"���[�g�ƃX�R�A�͖��֌W",
	L"SAFE���肾�ƃR���{�������Ȃ�",
	L"MISS�����LIFE������",
	L"LIFE����������MISS���o���ƁA����ɃX�R�A������",
	L"��xLIFE��0�ɂȂ�ƁA���s�����������Ȃ��Ȃ�",
	L"��xLIFE��0�ɂȂ�ƃN���A���s",
	L"ARROW��3��ޓ��������͋֎~����Ă���",
	L"�N�\���ʂ�������",
	L"�S�[���h�����i�[�͑S�̂�5%�������Ȃ�",
};
static char const tipNum = sizeof(tip) / sizeof(tip[0]);

void CutinReady() {
	pic_cutin[0] = LoadGraph(L"picture/cutin/cutinU.png");
	pic_cutin[1] = LoadGraph(L"picture/cutin/cutinD.png");
	pic_cutin[2] = LoadGraph(L"picture/cutin/cutinDisk.png");
	pic_cutin[3] = LoadGraph(SongJucketName);
}

void SetCutSong(wchar_t* songName, wchar_t* picName) {
	strcopy(songName, CutSongName, 1);
	strcopy(picName, SongJucketName, 1);
	pic_cutin[3] = LoadGraph(picName);
}

void SetCutTipFg(cutin_tips_e Fg) {
	CutFg = Fg;
}

void SetTipNo() {
	TipNo = (char)GetRand(tipNum - 1);
}

void ViewCutIn(int Stime) {
	int EffTime = maxs(GetNowCount() - Stime, 500);
	int PosY = pals(500, 0, 0, 360, EffTime);
	int Rot = pals(500, 0, 0, -300, EffTime);
	int Alpha = lins(500, 255, 0, 0, EffTime);
	DrawGraph(0, 0 - PosY, pic_cutin[0], TRUE);
	DrawGraph(0, 240 + PosY, pic_cutin[1], TRUE);
	DrawRotaGraph(320 - PosY, 240 - PosY, 1,
		(double)Rot / 100.0, pic_cutin[2], TRUE);
	switch (CutFg) {
	case CUTIN_TIPS_ON:
		DrawString(50 + PosY * 640 / 360, 430, tip[TipNo], 0xFF000000);
		break;
	case CUTIN_TIPS_SONG:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
		DrawExtendGraph(200 - PosY, 120 - PosY, 440 + PosY, 360 + PosY, pic_cutin[3], TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		DrawString(50 + PosY * 640 / 360, 430, CutSongName, 0xFF000000);
		break;
	default:
		/* none */
		break;
	}
	return;
}

void ViewCutOut(int Stime) {
	int EffTime = maxs(GetNowCount() - Stime, 500);
	if (500 < EffTime) {
		return;
	}
	int PosY = pals(0, 0, 500, 360, EffTime);
	int Rot = pals(0, 0, 500, 300, EffTime);
	int Alpha = lins(500, 0, 0, 255, EffTime);
	DrawGraph(0, 0 - PosY, pic_cutin[0], TRUE);
	DrawGraph(0, 240 + PosY, pic_cutin[1], TRUE);
	DrawRotaGraph(320 + PosY, 240 - PosY, 1,
		(double)Rot / 100.0, pic_cutin[2], TRUE);
	switch (CutFg) {
	case CUTIN_TIPS_ON:
		DrawString(50 - PosY * 640 / 360, 430, tip[TipNo], 0xFF000000);
		break;
	case CUTIN_TIPS_SONG:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
		DrawExtendGraph(200 - PosY, 120 - PosY, 440 + PosY, 360 + PosY, pic_cutin[3], TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		DrawString(50 - PosY * 640 / 360, 430, CutSongName, 0xFF000000);
		break;
	default:
		/* none */
		break;
	}
	return;
}
