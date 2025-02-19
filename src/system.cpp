
#include <DxLib.h>
#include <sancur.h>
#include <strcur.h>
#include <dxcur.h>
#include <system.h>

int SmallFontData = 0;
int LargeFontData = 0;

int RecPlayDebug[3] = { 0,0,0 };

/**
 * packNo����p�b�N�t�H���_�p�X���擾����
 * @param[out] ret �p�b�N�t�H���_�p�X�̊i�[��
 * @param[in] size ret�̒����A�z�񐔂Ŏw��
 * @param[in] packNo �p�b�N�i���o�[
 * @return 0:OK, -1:NG
 */
int RecGetPackFolderPath(TCHAR *ret, size_t size, uint packNo) {
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

	return 0;
}

/**
 * packNo��songNo����ȃt�H���_�p�X���擾����
 * @param[out] ret �ȃt�H���_�p�X�̊i�[��
 * @param[in] size ret�̒����A�z�񐔂Ŏw��
 * @param[in] packNo �p�b�N�i���o�[
 * @param[in] songNo �ȃi���o�[
 * @return 0:OK, -1:NG
 */
int RecGetMusicFolderPath(TCHAR *ret, size_t size, uint packNo, uint songNo) {
	TCHAR buf[256];
	DxFile_t fd;

	RecGetPackFolderPath(ret, size, packNo); // ret = "record/<�p�b�N��>/"

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

/**
 * packNo��songNo����ȃt�H���_�����擾����
 * @param[out] ret �ȃt�H���_���̊i�[��
 * @param[in] size ret�̒����A�z�񐔂Ŏw��
 * @param[in] packNo �p�b�N�i���o�[
 * @param[in] songNo �ȃi���o�[
 * @return 0:OK, -1:NG
 */
int RecGetMusicFolderName(TCHAR *ret, size_t size, uint packNo, uint songNo) {
	TCHAR GT1[256];
	TCHAR GT2[256];
	DxFile_t fd;
	
	RecGetPackFolderPath(GT2, size, packNo); // GT2 = "record/<�p�b�N��>/"

	strcats_2(GT2, 256, _T("list.txt")); // GT2 = "record/<�p�b�N��>/list.txt"

	fd = FileRead_open(GT2);
	for (int i = 0; i <= songNo; i++) {
		if (FileRead_eof(fd) != 0) { return -1; }
		FileRead_gets(GT1, 256, fd);
	} // GT1 = "<�Ȗ�>"
	FileRead_close(fd);

	strcopy_2(GT1, ret, size);

	return 0;
}

/**
 * packNo��songNo��difNo����}�b�v�t�@�C���p�X���擾����
 * @param[out] ret �}�b�v�t�@�C���p�X�̊i�[��
 * @param[in] size ret�̒����A�z�񐔂Ŏw��
 * @param[in] packNo �p�b�N�i���o�[
 * @param[in] songNo �ȃi���o�[
 * @param[in] difNo ��Փx�i���o�[
 * @return 0:OK, -1:NG
 */
int RecGetMusicMapRrsPath(TCHAR *ret, size_t size, uint packNo, uint songNo, rec_dif_t difNo) {
	if (RecGetMusicFolderPath(ret, size, packNo, songNo) != 0) { return -1; }
	stradds_2(ret, size, (TCHAR)((int)_T('0') + (int)difNo));
	strcats_2(ret, size, _T(".rrs"));
	return 0;
}

/**
 * packNo��songNo��difNo����}�b�v�e�L�X�g�t�@�C���p�X���擾����
 * @param[out] ret �}�b�v�e�L�X�g�t�@�C���p�X�̊i�[��
 * @param[in] size ret�̒����A�z�񐔂Ŏw��
 * @param[in] packNo �p�b�N�i���o�[
 * @param[in] songNo �ȃi���o�[
 * @param[in] difNo ��Փx�i���o�[
 * @return 0:OK, -1:NG
 */
int RecGetMusicMapTxtPath(TCHAR *ret, size_t size, uint packNo, uint songNo, rec_dif_t difNo) {
	if (RecGetMusicFolderPath(ret, size, packNo, songNo) != 0) { return -1; }
	stradds_2(ret, size, (TCHAR)((int)_T('0') + (int)difNo));
	strcats_2(ret, size, _T(".txt"));
	return 0;
}
