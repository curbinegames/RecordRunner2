#include <stdio.h>
#include "RecSystem.h"

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
