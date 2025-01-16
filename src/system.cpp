
#include <DxLib.h>
#include <sancur.h>
#include <strcur.h>
#include <dxcur.h>

int SmallFontData = 0;
int LargeFontData = 0;

int RecPlayDebug[3] = { 0,0,0 };

/**
 * packNo��songNo����t�@�C���p�X���擾����
 * @param[out] ret �t�@�C���p�X�̊i�[��
 * @param[in] size ret�̒����A�z�񐔂Ŏw��
 * @param[in] packNo �p�b�N�i���o�[
 * @param[in] songNo �ȃi���o�[
 * @return 0:OK, -1:NG
 */
int RecGetMusicPath(TCHAR *ret, size_t size, uint packNo, uint songNo) {
	TCHAR buf[256];

	DxFile_t fd;

	strcopy_2(_T("record/"), ret, size); // ret = "record/"

	fd = FileRead_open(_T("RecordPack.txt"));
	for (int i = 0; i <= packNo; i++) {
		if (FileRead_eof(fd) != 0) { return -1; }
		FileRead_gets(buf, 256, fd);
	} // buf = "<�p�b�N��>"
	FileRead_close(fd);

	strcats_2(ret, size, buf);           // ret = "record/<�p�b�N��>"
	stradds_2(ret, size, _T('/'));       // ret = "record/<�p�b�N��>/"
	strcopy_2(ret, buf, 256);            // buf = "record/<�p�b�N��>/"
	strcats_2(buf, 256, _T("list.txt")); // buf = "record/<�p�b�N��>/list.txt"

	fd = FileRead_open(buf);
	for (int i = 0; i <= songNo; i++) {
		if (FileRead_eof(fd) != 0) { return -1; }
		FileRead_gets(buf, 256, fd);
	} // buf = "<�Ȗ�>"
	FileRead_close(fd);

	strcats_2(ret, size, buf);     // ret = "record/<�p�b�N��>/<�Ȗ�>"
	stradds_2(ret, size, _T('/')); // ret = "record/<�p�b�N��>/<�Ȗ�>/"
	return 0;
}
