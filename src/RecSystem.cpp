
/* base include */
#include <stdio.h>

/* curbine code include */
#include <dxcur.h>
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <option.h>

/* own include */
#include <RecSystem.h>

int recSystenLoad(rec_system_t *sys) {
	int ret = 0;
	FILE *fp;
	ret = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (fp != NULL) {
		fread(&sys->chara, sizeof(int), 1, fp);
		fread(&sys->offset, sizeof(int), 1, fp);
		fread(&sys->soundEn, sizeof(int), 1, fp);
		fread(&sys->backLight, sizeof(int), 1, fp);
		fread(&sys->language, sizeof(int), 1, fp);
		fread(&sys->keyViewEn, sizeof(int), 1, fp);
		fread(&sys->judgePos, sizeof(int), 1, fp);
		fclose(fp);
	}
	else {
		return 1;
	}
	return 0;
}

static DxSnd_t s_bgm = DXLIB_SND_NULL;
static TCHAR s_bgmName[256] = _T("");
static int s_totalVolume = 255;

void RecSysBgmChangeVolume(int val) {
	s_totalVolume = val * optiondata.BGMvolume / 10;
	ChangeVolumeSoundMem(s_totalVolume, s_bgm);
	return;
}

void RecSysBgmSetMem(const TCHAR *sndPath, size_t size) {
	if (strands_2(s_bgmName, size, sndPath, ARRAY_COUNT(s_bgmName))) { return; }

	strcopy_2(sndPath, s_bgmName, ARRAY_COUNT(s_bgmName));
	StopSoundMem(s_bgm);
	DeleteSoundMem(s_bgm);
	s_bgm = LoadSoundMem(sndPath);
	RecSysBgmChangeVolume(255);

	return;
}

bool RecSysBgmCheckSoundMem(void) {
	return CheckSoundMem(s_bgm);
}

void RecSysBgmPlay(bool force, bool top_position) {
	if (!(force) && CheckSoundMem(s_bgm) == 1) { return; }
	if (force) { StopSoundMem(s_bgm); }
	ChangeVolumeSoundMem(s_totalVolume, s_bgm);
	PlaySoundMem(s_bgm, DX_PLAYTYPE_BACK, top_position);
	return;
}

void RecSysBgmDelete(void) {
	StopSoundMem(s_bgm);
	DeleteSoundMem(s_bgm);
	s_bgmName[0] = _T('\0');
	return;
}

void RecSysBgmStop(void) {
	StopSoundMem(s_bgm);
	return;
}

void RecSysBgmSetCurrentPosition(int val) {
	SetCurrentPositionSoundMem(val, s_bgm);
	return;
}
