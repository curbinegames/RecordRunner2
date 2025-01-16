#pragma once

#include <DxLib.h>
#include <system.h>
#include <strcur.h>

static void VerSaveScore(wchar_t const songN[], char dif,
	int score, double acc, int Dscore, short rank, char Clear) {
	int	read[7] = { 0,0,0,0,0,0,0 };
	int	Readdis[7] = { 0,0,0,0,0,0,0 };
	int	ReadRank[7] = { 6,6,6,6,6,6,6 };
	int	ReadClear[7] = { 0,0,0,0,0,0,0 };
	double ReadAcc[7] = { 0,0,0,0,0,0,0 };
	wchar_t save[255] = L"score/";
	strcats(save, songN); // save = score/<�Ȗ�>
	strcats(save, L".dat"); // save = score/<�Ȗ�>.dat
	FILE *fp;
	(void)_wfopen_s(&fp, save, L"rb");
	if (fp != NULL) {
		fread(&read, sizeof(int), 6, fp);
		fread(&ReadAcc, sizeof(double), 6, fp);
		fread(&Readdis, sizeof(int), 6, fp);
		fread(&ReadRank, sizeof(int), 6, fp);
		fread(&ReadClear, sizeof(int), 6, fp);
		fclose(fp);
	}
	if (read[dif] < score) { read[dif] = score; } //�n�C�X�R�A�ۑ�
	if (ReadAcc[dif] < acc) { ReadAcc[dif] = acc; } //ACC�ۑ�
	if (Readdis[dif] < Dscore) { Readdis[dif] = Dscore; } //�Œ����s�����ۑ�
	if (ReadRank[dif] > rank || ReadRank[dif] < 0) {
		ReadRank[dif] = rank; //�����N�ۑ�
	}
	if (ReadClear[dif] < Clear) { ReadClear[dif] = Clear; } //�N���A���[�g�ۑ�
	_wfopen_s(&fp, save, L"wb");
	fwrite(&read, sizeof(int), 6, fp);
	fwrite(&ReadAcc, sizeof(double), 6, fp);
	fwrite(&Readdis, sizeof(int), 6, fp);
	fwrite(&ReadRank, sizeof(int), 6, fp);
	fwrite(&ReadClear, sizeof(int), 6, fp);
	fclose(fp);
	return;
}

void upgrade_rate_f() {
	play_rate_t prate[RATE_NUM];
	wchar_t name[10][255] = {L"\0", L"\0", L"\0", L"\0", L"\0", L"\0", L"\0", L"\0", L"\0", L"\0"};
	double rate[10] = { 0,0,0,0,0,0,0,0,0,0 };
	FILE* fp;
	FILE* log;
	_wfopen_s(&fp, RATE_FILE_NAME, L"rb");
	if (fp != NULL) {
		fclose(fp);
		return;
	}
	_wfopen_s(&fp, OLD_RATE_FILE_NAME_S, L"rb");
	if (fp == NULL) { return; }
	fread(&name, 255, 10, fp);
	fclose(fp);
	_wfopen_s(&fp, OLD_RATE_FILE_NAME_N, L"rb");
	if (fp == NULL) { return; }
	fread(&rate, sizeof(double), 10, fp);
	fclose(fp);
	_wfopen_s(&log, L"reclog.txt", L"w");
	for (int i = 0; i < 10; i++) {
		if (rate[i] == 0) { break; }
		strcopy(name[i], prate[i].name, 1);
		prate[i].num = rate[i];
		if (log != NULL) {
			fwprintf(log, L"%f, %s\n", prate[i].num, prate[i].name);
		}
	}
	_wfopen_s(&fp, RATE_FILE_NAME, L"wb");
	if (fp == NULL) { return; }
	fwrite(&prate, sizeof(play_rate_t), 10, fp);
	fclose(fp);
	if (log != NULL) { fclose(log); }
	return;
}

void fix10501to10502_2(wchar_t const *oldFL, wchar_t const *newFL) {
	int	read[7] = { 0,0,0,0,0,0,0 };
	int	Readdis[7] = { 0,0,0,0,0,0,0 };
	int	ReadRank[7] = { 6,6,6,6,6,6,6 };
	int	ReadClear[7] = { 0,0,0,0,0,0,0 };
	double ReadAcc[7] = { 0,0,0,0,0,0,0 };
	FILE *fp;
	(void)_wfopen_s(&fp, oldFL, L"rb");
	if (fp == NULL) {
		return;
	}
	fread(&read, sizeof(int), 6, fp);
	fread(&ReadAcc, sizeof(double), 6, fp);
	fread(&Readdis, sizeof(int), 6, fp);
	fread(&ReadRank, sizeof(int), 6, fp);
	fread(&ReadClear, sizeof(int), 6, fp);
	fclose(fp);
	for (int i = 0; i < 7; i++) {
		VerSaveScore(newFL, i, read[i], ReadAcc[i], Readdis[i],
			(short)ReadRank[i], (char)ReadClear[i]);
	}
	//_wremove(oldFL);
	return;
}

void fix10501to10502() {
	fix10501to10502_2(L"score/�ؒ�����.dat", L"katyohugetsu");
	fix10501to10502_2(L"score/�O���f�[�V�����E���[���h.dat", L"Gradation-world");
	fix10501to10502_2(L"score/�T�C�N�����O.dat", L"cycling");
	fix10501to10502_2(L"score/�g���m����.dat", L"Torinoyume");
	fix10501to10502_2(L"score/���[�j���O�O���[���[.dat", L"Morning Glory");
	fix10501to10502_2(L"score/����(�^�_)�̂���.dat", L"Tada no Uta");
	fix10501to10502_2(L"score/���邭�����Ă��Ƃ₩��.dat", L"light and graceful");
	return;
}
