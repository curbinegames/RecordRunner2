
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <dxcur.h>
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <RecSystem.h>

int SmallFontData = 0;
int LargeFontData = 0;

int RecPlayDebug[3] = { 0,0,0 };

/* TODO: �e�Ăь��ł̖߂�l�`�F�b�N */
/**
 * packNo����p�b�N�t�H���_�p�X���擾����
 * @param[out] ret �p�b�N�t�H���_�p�X�̊i�[��
 * @param[in] size ret�̒����A�z�񐔂Ŏw��
 * @param[in] packNo �p�b�N�i���o�[
 * @return rec_error_t
 */
rec_error_t RecGetPackFolderPath(TCHAR *ret, size_t size, uint packNo) {
	TCHAR buf[256];
	DxFile_t fd;

	strcopy_2(_T("record/"), ret, size); // ret = "record/"

	fd = FileRead_open(_T("RecordPack.txt"));
	if (fd == DXLIB_FILE_HAND_DEFAULT) { return REC_ERROR_FILE_EXIST; }

	for (int i = 0; i <= packNo; i++) {
		if (FileRead_eof(fd) != 0) { return REC_ERROR_FILE_NUM; }
		FileRead_gets(buf, 256, fd);
	} // buf = "<�p�b�N��>"
	FileRead_close(fd);

	strcats_2(ret, size, buf);           // ret = "record/<�p�b�N��>"
	stradds_2(ret, size, _T('/'));       // ret = "record/<�p�b�N��>/"

	return REC_ERROR_NONE;
}

/**
 * packNo��songNo����ȃt�H���_�p�X���擾����
 * @param[out] ret �ȃt�H���_�p�X�̊i�[��
 * @param[in] size ret�̒����A�z�񐔂Ŏw��
 * @param[in] packNo �p�b�N�i���o�[
 * @param[in] songNo �ȃi���o�[
 * @return rec_error_t
 */
rec_error_t RecGetMusicFolderPath(TCHAR *ret, size_t size, uint packNo, uint songNo) {
	rec_error_t status = REC_ERROR_NONE;
	TCHAR buf[256];
	DxFile_t fd;

	status = RecGetPackFolderPath(ret, size, packNo); // ret = "record/<�p�b�N��>/"
	if (status != REC_ERROR_NONE) { return status; }

	strcopy_2(ret, buf, 256);            // buf = "record/<�p�b�N��>/"
	strcats_2(buf, 256, _T("list.txt")); // buf = "record/<�p�b�N��>/list.txt"

	fd = FileRead_open(buf);
	if (fd == DXLIB_FILE_HAND_DEFAULT) { return REC_ERROR_FILE_EXIST; }

	for (int i = 0; i <= songNo; i++) {
		if (FileRead_eof(fd) != 0) { return REC_ERROR_FILE_NUM; }
		FileRead_gets(buf, 256, fd);
	} // buf = "<�Ȗ�>"
	FileRead_close(fd);

	strcats_2(ret, size, buf);     // ret = "record/<�p�b�N��>/<�Ȗ�>"
	stradds_2(ret, size, _T('/')); // ret = "record/<�p�b�N��>/<�Ȗ�>/"

	return REC_ERROR_NONE;
}

/**
 * packNo��songNo����ȃt�H���_�����擾����
 * @param[out] ret �ȃt�H���_���̊i�[��
 * @param[in] size ret�̒����A�z�񐔂Ŏw��
 * @param[in] packNo �p�b�N�i���o�[
 * @param[in] songNo �ȃi���o�[
 * @return rec_error_t
 */
rec_error_t RecGetMusicFolderName(TCHAR *ret, size_t size, uint packNo, uint songNo) {
	rec_error_t status = REC_ERROR_NONE;
	TCHAR GT1[256];
	TCHAR GT2[256];
	DxFile_t fd;
	
	status = RecGetPackFolderPath(GT2, size, packNo); // GT2 = "record/<�p�b�N��>/"
	if (status != REC_ERROR_NONE) { return status; }

	strcats_2(GT2, 256, _T("list.txt")); // GT2 = "record/<�p�b�N��>/list.txt"

	fd = FileRead_open(GT2);
	if (fd == DXLIB_FILE_HAND_DEFAULT) { return REC_ERROR_FILE_EXIST; }

	for (int i = 0; i <= songNo; i++) {
		if (FileRead_eof(fd) != 0) { return REC_ERROR_FILE_NUM; }
		FileRead_gets(GT1, 256, fd);
	} // GT1 = "<�Ȗ�>"
	FileRead_close(fd);

	strcopy_2(GT1, ret, size);

	return REC_ERROR_NONE;
}

/**
 * packNo��songNo��difNo����}�b�v�t�@�C���p�X���擾����
 * @param[out] ret �}�b�v�t�@�C���p�X�̊i�[��
 * @param[in] size ret�̒����A�z�񐔂Ŏw��
 * @param[in] packNo �p�b�N�i���o�[
 * @param[in] songNo �ȃi���o�[
 * @param[in] difNo ��Փx�i���o�[
 * @return rec_error_t
 */
rec_error_t RecGetMusicMapRrsPath(TCHAR *ret, size_t size, uint packNo, uint songNo, rec_dif_t difNo) {
	rec_error_t status = REC_ERROR_NONE;
	status = RecGetMusicFolderPath(ret, size, packNo, songNo);
	if (status != REC_ERROR_NONE) { return status; }
	stradds_2(ret, size, (TCHAR)((int)_T('0') + (int)difNo));
	strcats_2(ret, size, _T(".rrs"));
	return REC_ERROR_NONE;
}

/**
 * packNo��songNo��difNo����}�b�v�e�L�X�g�t�@�C���p�X���擾����
 * @param[out] ret �}�b�v�e�L�X�g�t�@�C���p�X�̊i�[��
 * @param[in] size ret�̒����A�z�񐔂Ŏw��
 * @param[in] packNo �p�b�N�i���o�[
 * @param[in] songNo �ȃi���o�[
 * @param[in] difNo ��Փx�i���o�[
 * @return rec_error_t
 */
rec_error_t RecGetMusicMapTxtPath(TCHAR *ret, size_t size, uint packNo, uint songNo, rec_dif_t difNo) {
	rec_error_t status = REC_ERROR_NONE;
	status = RecGetMusicFolderPath(ret, size, packNo, songNo);
	if (status != REC_ERROR_NONE) { return status; }
	stradds_2(ret, size, (TCHAR)((int)_T('0') + (int)difNo));
	strcats_2(ret, size, _T(".txt"));
	return REC_ERROR_NONE;
}
