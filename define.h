#pragma once

/* don't write. please write system.h */

#define OLD_RATE_NUM 10
#define OLD_RATE_FILE_NAME_S L"save/rateS.dat" /* ���O */
#define OLD_RATE_FILE_NAME_N L"save/rateN.dat" /* ���l */

#define RATE_NUM 20
#define RATE_FILE_NAME L"save/Prate.dat" /* ���O+���l */

typedef struct play_rate_s {
	wchar_t name[255] = L"\0";
	double num = 0;
} play_rate_t;
