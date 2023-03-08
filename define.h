#pragma once

#define RATE_NUM 20
#define OLD_RATE_FILE_NAME_S L"save/rateS.dat"
#define OLD_RATE_FILE_NAME_N L"save/rateN.dat"
#define RATE_FILE_NAME L"save/Prate.dat"

typedef struct play_rate_s {
	wchar_t name[255] = L"\0";
	double num = 0;
} play_rate_t;
