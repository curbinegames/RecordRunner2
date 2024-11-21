
#include <stdio.h>

typedef unsigned char uchar;
typedef unsigned short ushort;

void MakeRlock(const char *out, const char *in) {
	uchar buf[2] = { 0, 0 };
	uchar wbyte = 0;
	FILE *fin;
	FILE *fout;
	fopen_s(&fin, in, "rb");
	if (fin != NULL) {
		fopen_s(&fout, out, "wb");
		if (fout != NULL) {

			while (fread(&buf[1], sizeof(buf[1]), 1, fin) != 0) {
				if (buf[0] <= buf[1]) {
					wbyte = buf[1] - buf[0];
				}
				else {
					wbyte = (uchar)(0x100 - (ushort)buf[0]) + buf[1];
				}
				fwrite(&wbyte, sizeof(wbyte), 1, fout);
				buf[0] = buf[1];
			}

			fclose(fout);
		}
		fclose(fin);
	}
	return;
}

void OpenRlock(const char *out, const char *in) {
	uchar buf[2] = { 0, 0 };
	uchar wbyte = 0;
	FILE *fin;
	FILE *fout;
	fopen_s(&fin, in, "rb");
	if (fin != NULL) {
		fopen_s(&fout, out, "wb");
		if (fout != NULL) {

			while (fread(&buf[1], sizeof(buf[1]), 1, fin) != 0) {
				wbyte = (((ushort)buf[0] + buf[1]) % 0x100);
				fwrite(&wbyte, sizeof(wbyte), 1, fout);
				buf[0] = wbyte;
			}

			fclose(fout);
		}
		fclose(fin);
	}
	return;
}
