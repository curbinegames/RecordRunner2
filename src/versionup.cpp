
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <strcur.h>

/* rec system include */
#include <RecSave.h>
#include <RecSystem.h>

/* TODO: バージョンファイルを作る */

void upgrade_rate_f() {
	play_rate_t prate[RATE_NUM];
	wchar_t name[10][255] = {L"\0", L"\0", L"\0", L"\0", L"\0", L"\0", L"\0", L"\0", L"\0", L"\0"};
	double rate[10] = { 0,0,0,0,0,0,0,0,0,0 };
	FILE *fp;
	FILE *log;

	/* ファイルがあったらreturn */
	if (RecSaveReadRunnerRate(prate)) { return; }

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
		strcopy_2(name[i], prate[i].name, 255);
		prate[i].num = rate[i];
		if (log != NULL) {
			fwprintf(log, L"%f, %s\n", prate[i].num, prate[i].name);
		}
	}

	RecSaveWriteRunnerRate(prate);

	if (log != NULL) { fclose(log); }
	return;
}

static void fix10501to10502_2(const TCHAR *oldFL, const TCHAR *newFL) {
	rec_save_score_t buf[6];
	RecSaveReadScoreAllDif(buf, oldFL);
	RecSaveUpdateScoreOneDif(&buf[REC_DIF_AUTO],    newFL, REC_DIF_AUTO);
	RecSaveUpdateScoreOneDif(&buf[REC_DIF_EASY],    newFL, REC_DIF_EASY);
	RecSaveUpdateScoreOneDif(&buf[REC_DIF_NORMAL],  newFL, REC_DIF_NORMAL);
	RecSaveUpdateScoreOneDif(&buf[REC_DIF_HARD],    newFL, REC_DIF_HARD);
	RecSaveUpdateScoreOneDif(&buf[REC_DIF_ANOTHER], newFL, REC_DIF_ANOTHER);
	RecSaveUpdateScoreOneDif(&buf[REC_DIF_SECRET],  newFL, REC_DIF_SECRET);
	return;
}

void fix10501to10502() {
	fix10501to10502_2(L"華調風月", L"katyohugetsu");
	fix10501to10502_2(L"グラデーション・ワールド", L"Gradation-world");
	fix10501to10502_2(L"サイクリング", L"cycling");
	fix10501to10502_2(L"トリノユメ", L"Torinoyume");
	fix10501to10502_2(L"モーニンググローリー", L"Morning Glory");
	fix10501to10502_2(L"無料(タダ)のうた", L"Tada no Uta");
	fix10501to10502_2(L"明るくそしてしとやかに", L"light and graceful");
	return;
}
