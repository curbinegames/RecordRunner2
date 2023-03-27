#pragma onse

#include "DxLib.h"

char GetCharNo() {
	int data[6] = { 0,0,0,2,0,0 };
	FILE* fp;
	(void)_wfopen_s(&fp, L"save/system.dat", L"rb");
	if (fp != NULL) {
		fread(&system, sizeof(int), 6, fp);
		fclose(fp);
	}
	return (char)data[0];
}
