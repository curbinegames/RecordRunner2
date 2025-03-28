
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

/* TODO: 各呼び元での戻り値チェック */
/**
 * packNoからパックフォルダパスを取得する
 * @param[out] ret パックフォルダパスの格納先
 * @param[in] size retの長さ、配列数で指定
 * @param[in] packNo パックナンバー
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
	} // buf = "<パック名>"
	FileRead_close(fd);

	strcats_2(ret, size, buf);           // ret = "record/<パック名>"
	stradds_2(ret, size, _T('/'));       // ret = "record/<パック名>/"

	return REC_ERROR_NONE;
}

/**
 * packNoとsongNoから曲フォルダパスを取得する
 * @param[out] ret 曲フォルダパスの格納先
 * @param[in] size retの長さ、配列数で指定
 * @param[in] packNo パックナンバー
 * @param[in] songNo 曲ナンバー
 * @return rec_error_t
 */
rec_error_t RecGetMusicFolderPath(TCHAR *ret, size_t size, uint packNo, uint songNo) {
	rec_error_t status = REC_ERROR_NONE;
	TCHAR buf[256];
	DxFile_t fd;

	status = RecGetPackFolderPath(ret, size, packNo); // ret = "record/<パック名>/"
	if (status != REC_ERROR_NONE) { return status; }

	strcopy_2(ret, buf, 256);            // buf = "record/<パック名>/"
	strcats_2(buf, 256, _T("list.txt")); // buf = "record/<パック名>/list.txt"

	fd = FileRead_open(buf);
	if (fd == DXLIB_FILE_HAND_DEFAULT) { return REC_ERROR_FILE_EXIST; }

	for (int i = 0; i <= songNo; i++) {
		if (FileRead_eof(fd) != 0) { return REC_ERROR_FILE_NUM; }
		FileRead_gets(buf, 256, fd);
	} // buf = "<曲名>"
	FileRead_close(fd);

	strcats_2(ret, size, buf);     // ret = "record/<パック名>/<曲名>"
	stradds_2(ret, size, _T('/')); // ret = "record/<パック名>/<曲名>/"

	return REC_ERROR_NONE;
}

/**
 * packNoとsongNoから曲フォルダ名を取得する
 * @param[out] ret 曲フォルダ名の格納先
 * @param[in] size retの長さ、配列数で指定
 * @param[in] packNo パックナンバー
 * @param[in] songNo 曲ナンバー
 * @return rec_error_t
 */
rec_error_t RecGetMusicFolderName(TCHAR *ret, size_t size, uint packNo, uint songNo) {
	rec_error_t status = REC_ERROR_NONE;
	TCHAR GT1[256];
	TCHAR GT2[256];
	DxFile_t fd;
	
	status = RecGetPackFolderPath(GT2, size, packNo); // GT2 = "record/<パック名>/"
	if (status != REC_ERROR_NONE) { return status; }

	strcats_2(GT2, 256, _T("list.txt")); // GT2 = "record/<パック名>/list.txt"

	fd = FileRead_open(GT2);
	if (fd == DXLIB_FILE_HAND_DEFAULT) { return REC_ERROR_FILE_EXIST; }

	for (int i = 0; i <= songNo; i++) {
		if (FileRead_eof(fd) != 0) { return REC_ERROR_FILE_NUM; }
		FileRead_gets(GT1, 256, fd);
	} // GT1 = "<曲名>"
	FileRead_close(fd);

	strcopy_2(GT1, ret, size);

	return REC_ERROR_NONE;
}

/**
 * packNoとsongNoとdifNoからマップファイルパスを取得する
 * @param[out] ret マップファイルパスの格納先
 * @param[in] size retの長さ、配列数で指定
 * @param[in] packNo パックナンバー
 * @param[in] songNo 曲ナンバー
 * @param[in] difNo 難易度ナンバー
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
 * packNoとsongNoとdifNoからマップテキストファイルパスを取得する
 * @param[out] ret マップテキストファイルパスの格納先
 * @param[in] size retの長さ、配列数で指定
 * @param[in] packNo パックナンバー
 * @param[in] songNo 曲ナンバー
 * @param[in] difNo 難易度ナンバー
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
