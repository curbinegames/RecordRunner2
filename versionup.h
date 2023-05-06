#pragma once

#include "DxLib.h"
#include "define.h"
#include "strcur.h"

void upgrade_rate_f() {
	play_rate_t prate[RATE_NUM];
	wchar_t name[10][255] = {L"\0", L"\0", L"\0", L"\0", L"\0", L"\0", L"\0", L"\0", L"\0", L"\0"};
	double rate[10] = { 0,0,0,0,0,0,0,0,0,0 };
	FILE* fp;
	FILE* log;
	_wfopen_s(&fp, RATE_FILE_NAME, L"rb");
	if (fp != NULL) { return; }
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
