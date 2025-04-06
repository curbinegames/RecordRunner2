/* TODO: ddifが更新されない!! (ver1.5.3では更新できてる、おそらくmasterブランチの不具合) */

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

void RecSysBgmPlay(bool force, bool loop, bool top_position) {
	if (!(force) && CheckSoundMem(s_bgm) == 1) { return; }
	if (force) { StopSoundMem(s_bgm); }
	ChangeVolumeSoundMem(s_totalVolume, s_bgm);
	if (loop) {
		PlaySoundMem(s_bgm, DX_PLAYTYPE_LOOP, top_position);
	}
	else {
		PlaySoundMem(s_bgm, DX_PLAYTYPE_BACK, top_position);
	}
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
