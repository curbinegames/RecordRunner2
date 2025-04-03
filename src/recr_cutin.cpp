
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <option.h>
#include <RecWindowRescale.h>

/* own include */
#include <recr_cutin.h>

#define CUT_MES_POSX 75

#define CUT_FRAG_OUT 0
#define CUT_FRAG_IN 1

#define RECR_DEBUG(ofs, data)											\
		RecRescaleDrawFormatString(20, 120 + ofs * 20, 0xffffffff, L#data": %d", data)

static char TipNo = 0;
static int pic_cutin[5];
static int snd_cutin[2];
static int CutInSndLastPlayTime = 0;
static int CutOutSndLastPlayTime = 0;
static wchar_t SongJucketName[256] = { L"NULL" };
static wchar_t CutSongName[256] = { L"NULL" };
static cutin_tips_e CutFg = CUTIN_TIPS_NONE;
static int s_cutIoFg = CUT_FRAG_OUT;
static int s_cutStime = 0;

/* TODO: �ҏW�����(1.5.5�ł�������) */
/* tip�̍ő啶������30����(NULL�I�[����) */
static wchar_t const tip[][31] = {
	L"���y�͖����ɉe����^����", /* ���R�����̐��E�� */
	L"�����i�[�́A���y�ƍ߂�h�~�����ڂ�����",
	L"�s�b�J�[�����̂���n��́A��r�I�������ǂ�",
	L"�S�[���h�����i�[�͑S�̂�5%�������Ȃ�",
	L"�u�������Ă͂������ǁA�����i�[���Č��\�����Ȃ��v", /* �s�b�J�[�̃Z���t */
	L"�u�����A�����C�����ǂ��c�v",
	L"�u�Q�[�^�[����A����������̂����͂�߂āv",
	L"�u�e�C���[������āA�ǂ̂��炢�������񂾂낤�c?�v",
	L"�u��������A�ǂ��ŉ�����Ă�񂾂낤�c�v",
	L"�u�p�t�F���܂ŉ��x�ł��`�������W����!�v", /* �Q�[�^�[�̃Z���t */
	L"�u�񂪂��`�c�A�ڊo�܂������邹���c�v",
	L"�u�s�b�J�[�͐V�l�����A�����F�X�����Ȃ���ȁv",
	L"�u�e�C���[�A���܂ɂ͂��O�������Ă���ˁ[��?�v",
	L"�u��΂�����Ɍ��Ԃ��Ă��񂾁v",
	L"�u���͂����̉��y�D���̃h���S������v", /* �e�C���[�̃Z���t */
	L"�u���ꂢ�Ȃ��̂Ɖ������̂ɖڂ��Ȃ������肷���v",
	L"�u�s�b�J�[�N�͐F�X�Ɗ��҂ł���q�����v",
	L"�u�Q�[�^�[�A�s�b�J�[�N�ɂ��܂�Ӓn�����Ȃ��łˁ�v",
	L"�u�������āA�{���͓��������񂾂�v",
	L"HIT�m�[�c�̓L�����𓮂����Ȃ��ėǂ�", /* �Q�[���̎d�l */
	L"CATCH�m�[�c�̔���͑O��ɂ���",
	L"BOMB�m�[�c�̔���͋ɒ[�ɋ���",
	L"�F�Ⴂ�̃m�[�c�����邯�ǁA����͕ς��Ȃ�",
	L"���[�g�ƃX�R�A�͖��֌W",
	L"SAFE���肾�ƃR���{�������Ȃ�",
	L"MISS�����LIFE������",
	L"LIFE����������MISS���o���ƁA����ɃX�R�A������",
	L"��xLIFE��0�ɂȂ�ƁA���s�����������Ȃ��Ȃ�",
	L"��xLIFE��0�ɂȂ�ƃN���A���s",
	L"�ȒP�ȕ��ʂ�Lv10�Ƃ�����Ă��A���[�g�͏オ��Ȃ���",
	L"�㉺�����������Ă�Ƃ��́A�L�����͐^�񒆂ɗ���",
	L"HIT�m�[�c�́A��������OK�A�Q��OK", /* �Q�[���̃R�c */
	L"�����ZXC�L�[�ɒu���̂���̓�",
	L"������\������L�[�ɒu���̂���̓�",
	L"CATCH�m�[�c�́A�㉺�L�[���K�`���K�`�����Ă�����",
	L"�S�������Ă��ǂ����ǁA�������ݒ��ӂ�",
	L"�I�v�V�����ŁA�v���C����L�����N�^�[��ς�����", /* �I�v�V�������� */
	L"�m�[�c�Ɖ��y�������ĂȂ��Ƃ��́A�I�v�V�����Œ��߂��悤",
	L"�v���C���̌��ʉ��́A�I�v�V�����ŏ������",
	L"�I�v�V�����ŁA�v���C���̔w�i���������",
	L"�I�v�V�����ŁA�v���C���ɃL�[������Ԃ�\���ł����",
	L"�I�v�V�����ŁA�v���C���̔���̈ʒu��ς�����",
	L"Record Runner �̌��l�^�́A�r�u���{��", /* ���R�����̎G�b */
	L"�I�[�g�v���C�́A�܂����S�ł͂Ȃ�",
	L"ARROW��3��ޓ��������͋֎~����Ă���",
	L"Muse Dash? ����c�A�������̃Q�[���m��Ȃ��������c",
	L"�������m�[�c? �ǉ����Ȃ��悻��Ȃ���",
	L"�܂��܂��ǉ��������@�\�͂����",
	L"���ʂ̃��M�����[�V�����͈ꉞ����",
	L"�I�ȉ�ʂ̎��A�}�E�X����������",
	L"�J�[�r���̓P���i�[", /* �J�[�r���̂��� */
	L"�J�[�r����twitter����낵����",
	L"�J�[�r���̉p��\�L�́Acurbine",
	L"��ȁA�C���X�g�A�v���O�����A�S������Ă�̂��J�[�r��",
	L"�N�\���ʂ͂قǂقǂɂ�", /* �J�[�r���̗����� */
	L"�`���E�j�Y���͑���Lv18���o��",
	L"���Q�[�ɃN�\�Q�[�͂Ȃ�",
};
static char const tipNum = ARRAY_COUNT(tip);

#if 1 /* action */

void CutinReadyPic() {
	pic_cutin[0] = LoadGraph(L"picture/cutin/cutinU.png");
	pic_cutin[1] = LoadGraph(L"picture/cutin/cutinD.png");
	pic_cutin[2] = LoadGraph(L"picture/cutin/cutinDisk.png");
	pic_cutin[3] = LoadGraph(SongJucketName);
	pic_cutin[4] = LoadGraph(L"picture/cutin/cutinS.png");
}

void CutinReadySnd() {
	snd_cutin[0] = LoadSoundMem(L"sound/IN.wav");
	snd_cutin[1] = LoadSoundMem(L"sound/OUT.wav");
	ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, snd_cutin[0]);
	ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, snd_cutin[1]);
}

void SetCutSong(wchar_t* songName, wchar_t* picName) {
	strcopy_2(songName, CutSongName, 255);
	strcopy_2(picName, SongJucketName, 255);
	pic_cutin[3] = LoadGraph(picName);
}

void SetCutTipFg(cutin_tips_e Fg) {
	CutFg = Fg;
}

void SetTipNo() {
	TipNo = (char)GetRand(tipNum - 1);
}

static void RecCutDrawShut(int EffTime) {
	int PosY = 0;
	if (s_cutIoFg == CUT_FRAG_OUT) {
		PosY = pals(0, 0, 500, 360, EffTime);
	}
	else {
		PosY = pals(500, 0, 0, 360, EffTime);
	}
	RecRescaleDrawGraph(0, 0 - PosY, pic_cutin[0], TRUE);
	RecRescaleDrawGraph(0, 240 + PosY, pic_cutin[1], TRUE);
	return;
}

static void RecCutDrawDisk(int EffTime) {
	int DrawX = 0;
	int Rot = 0;
	int PosY = 0;

	if (s_cutIoFg == CUT_FRAG_OUT) {
		PosY = pals(0, 0, 500, 360, EffTime);
		DrawX = 320 + pals(0, 0, 500, 360, EffTime);
		Rot = pals(0, 0, 500, 300, EffTime);
	}
	else {
		PosY = pals(500, 0, 0, 360, EffTime);
		DrawX = 320 - pals(500, 0, 0, 360, EffTime);
		Rot = pals(500, 0, 0, -300, EffTime);
	}
	RecRescaleDrawRotaGraph(DrawX, 240 - PosY, 1, (double)Rot / 50.0, pic_cutin[2], TRUE);
	return;
}

static void RecCutDrawJacket(int EffTime) {
	int drawX = 0;
	int drawY = 0;
	int drawX2 = 0;
	int drawY2 = 0;
	int PosY = 0;
	int Alpha = 0;

	if (s_cutIoFg == CUT_FRAG_OUT) {
		PosY = pals(0, 0, 500, 360, EffTime);
		Alpha = lins(500, 0, 0, 255, EffTime);
	}
	else {
		PosY = pals(500, 0, 0, 360, EffTime);
		Alpha = lins(500, 255, 0, 0, EffTime);
	}

	drawX = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 200 - PosY);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 120 - PosY);
	drawX2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 440 + PosY);
	drawY2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 360 + PosY);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
	DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic_cutin[3], TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	return;
}

static void RecCutDrawTips(int EffTime, const TCHAR *str) {
	int DrawX = 0;
	int PosY = 0;

	if (s_cutIoFg == CUT_FRAG_OUT) {
		PosY = pals(0, 0, 500, 360, EffTime);
		DrawX = CUT_MES_POSX - PosY * 640 / 360;
	}
	else {
		PosY = pals(500, 0, 0, 360, EffTime);
		DrawX = CUT_MES_POSX + PosY * 640 / 360;
	}

	RecRescaleDrawString(DrawX, 430, str, COLOR_BLACK);
	return;
}

static void RecCutDrawSide(int EffTime) {
	int PosS = 0;
	if (s_cutIoFg == CUT_FRAG_OUT) {
		PosS = pals(150, 0, 500, 100, betweens(0, EffTime, 500));
	}
	else {
		PosS = pals(350, 0, 0, 100, betweens(0, EffTime, 500));
	}
	RecRescaleDrawGraph(0 - PosS, 0, pic_cutin[4], TRUE);
	RecRescaleDrawGraph(590 + PosS, 0, pic_cutin[4], TRUE);
	return;
}

void ViewCutIn(int Stime) {
	int Ntime = GetNowCount();
	int EffTime = mins_2(Ntime - Stime, 500);
	int PosY = pals(500, 0, 0, 360, EffTime);
	int Rot = pals(500, 0, 0, -300, EffTime);
	int Alpha = lins(500, 255, 0, 0, EffTime);
	s_cutIoFg = CUT_FRAG_IN;
	RecRescaleDrawGraph(0, 0 - PosY, pic_cutin[0], TRUE);
	RecRescaleDrawGraph(0, 240 + PosY, pic_cutin[1], TRUE);
	RecRescaleDrawRotaGraph(320 - PosY, 240 - PosY, 1,
		(double)Rot / 50.0, pic_cutin[2], TRUE);
	switch (CutFg) {
	case CUTIN_TIPS_ON:
		RecRescaleDrawString(CUT_MES_POSX + PosY * 640 / 360, 430, tip[TipNo], COLOR_BLACK);
		break;
	case CUTIN_TIPS_SONG:
		{
			int drawX = 0;
			int drawY = 0;
			int drawX2 = 0;
			int drawY2 = 0;
			drawX = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 200 - PosY) + 160;
			drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 120 - PosY);
			drawX2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 440 + PosY) + 160;
			drawY2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 360 + PosY);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
			DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic_cutin[3], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			RecRescaleDrawString(CUT_MES_POSX + PosY * 640 / 360, 430, CutSongName, COLOR_BLACK);
		}
		break;
	default:
		/* none */
		break;
	}
	RecCutDrawSide(EffTime);

#if 0
	if (CutInSndLastPlayTime + 3000 < Ntime) {
		PlaySoundMem(snd_cutin[0], DX_PLAYTYPE_BACK);
		CutInSndLastPlayTime = Ntime;
	}
#endif
	return;
}

void ViewCutOut(int Stime) {
	int Ntime = GetNowCount();
	int EffTime = Ntime - Stime;
	if (500 < EffTime) {
		return;
	}
	int PosY = pals(0, 0, 500, 360, EffTime);
	int Rot = pals(0, 0, 500, 300, EffTime);
	int Alpha = lins(500, 0, 0, 255, EffTime);
	s_cutIoFg = CUT_FRAG_OUT;
	RecRescaleDrawGraph(0, 0 - PosY, pic_cutin[0], TRUE);
	RecRescaleDrawGraph(0, 240 + PosY, pic_cutin[1], TRUE);
	RecRescaleDrawRotaGraph(320 + PosY, 240 - PosY, 1,
		(double)Rot / 50.0, pic_cutin[2], TRUE);
	switch (CutFg) {
	case CUTIN_TIPS_ON:
		RecRescaleDrawString(CUT_MES_POSX - PosY * 640 / 360, 430, tip[TipNo], COLOR_BLACK);
		break;
	case CUTIN_TIPS_SONG:
		{
			int drawX = 0;
			int drawY = 0;
			int drawX2 = 0;
			int drawY2 = 0;
			drawX = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 200 - PosY) + 160;
			drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 120 - PosY);
			drawX2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 440 + PosY) + 160;
			drawY2 = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 360 + PosY);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
			DrawExtendGraph(drawX, drawY, drawX2, drawY2, pic_cutin[3], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			RecRescaleDrawString(CUT_MES_POSX + PosY * 640 / 360, 430, CutSongName, COLOR_BLACK);
		}
		break;
	default:
		/* none */
		break;
	}
	RecCutDrawSide(EffTime);

#if 0
	if (CutOutSndLastPlayTime + 3000 < Ntime) {
		PlaySoundMem(snd_cutin[1], DX_PLAYTYPE_BACK);
		CutOutSndLastPlayTime = Ntime;
	}
#endif
	return;
}

void RecViewCut2() {
	if (s_cutIoFg == CUT_FRAG_OUT) { ViewCutOut(s_cutStime); }
	if (s_cutIoFg == CUT_FRAG_IN) { ViewCutIn(s_cutStime); }
	return;
}

void RecViewCut3() {
	int EffTime = mins_2(GetNowCount() - s_cutStime, 500);

	if (s_cutIoFg == CUT_FRAG_OUT && 500 <= EffTime) {
		return;
	}

	RecCutDrawShut(EffTime);
	RecCutDrawDisk(EffTime);
	if (CutFg == CUTIN_TIPS_SONG) {
		RecCutDrawJacket(EffTime);
		RecCutDrawTips(EffTime, CutSongName);
	}
	else {
		RecCutDrawTips(EffTime, tip[TipNo]);
	}
	RecCutDrawSide(EffTime);

	return;
}

void RecCutSetIo(int val) {
	s_cutIoFg = val;
	s_cutStime = GetNowCount();
	if (val == CUT_FRAG_IN) {
		PlaySoundMem(snd_cutin[0], DX_PLAYTYPE_BACK);
	}
	else {
		PlaySoundMem(snd_cutin[1], DX_PLAYTYPE_BACK);
	}
	return;
}

int RecCutIsClosing() {
	return s_cutIoFg;
}

int RecCutInEndAnim() {
	return (s_cutIoFg == 1 && s_cutStime + 2000 <= GetNowCount());
}

#endif /* action */

rec_cutin_c::rec_cutin_c() {
	DeleteGraph(pic_cutin[0]);
	DeleteGraph(pic_cutin[1]);
	DeleteGraph(pic_cutin[2]);
	DeleteGraph(pic_cutin[3]);
	DeleteGraph(pic_cutin[4]);
	DeleteGraph(snd_cutin[0]);
	DeleteGraph(snd_cutin[1]);
	pic_cutin[0] = LoadGraph(L"picture/cutin/cutinU.png");
	pic_cutin[1] = LoadGraph(L"picture/cutin/cutinD.png");
	pic_cutin[2] = LoadGraph(L"picture/cutin/cutinDisk.png");
	pic_cutin[3] = LoadGraph(SongJucketName);
	pic_cutin[4] = LoadGraph(L"picture/cutin/cutinS.png");
	snd_cutin[0] = LoadSoundMem(L"sound/IN.wav");
	snd_cutin[1] = LoadSoundMem(L"sound/OUT.wav");
	ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, snd_cutin[0]);
	ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, snd_cutin[1]);
}

rec_cutin_c::~rec_cutin_c() {
	DeleteGraph(pic_cutin[0]);
	DeleteGraph(pic_cutin[1]);
	DeleteGraph(pic_cutin[2]);
	DeleteGraph(pic_cutin[3]);
	DeleteGraph(pic_cutin[4]);
	DeleteGraph(snd_cutin[0]);
	DeleteGraph(snd_cutin[1]);
}

void rec_cutin_c::SetCutSong(TCHAR *songName, TCHAR *picName) {
	strcopy_2(songName, CutSongName, 255);
	strcopy_2(picName, SongJucketName, 255);
	pic_cutin[3] = LoadGraph(picName);
	return;
}

void rec_cutin_c::SetCutTipFg(cutin_tips_e Fg) {
	CutFg = Fg;
	return;
}

void rec_cutin_c::SetTipNo() {
	TipNo = (char)GetRand(tipNum - 1);
	return;
}

void rec_cutin_c::DrawCut() {
	if (s_cutIoFg == CUT_FRAG_OUT) { ViewCutOut(s_cutStime); }
	if (s_cutIoFg == CUT_FRAG_IN) { ViewCutIn(s_cutStime); }
	return;
}

void rec_cutin_c::SetIo(int val) {
	s_cutIoFg = val;
	s_cutStime = GetNowCount();
	if (val == CUT_FRAG_IN) {
		PlaySoundMem(snd_cutin[0], DX_PLAYTYPE_BACK);
	}
	else {
		PlaySoundMem(snd_cutin[1], DX_PLAYTYPE_BACK);
	}
	return;
}

int rec_cutin_c::IsClosing() {
	return s_cutIoFg;
}

int rec_cutin_c::IsEndAnim() {
	return (s_cutIoFg == 1 && s_cutStime + 2000 <= GetNowCount());
}
