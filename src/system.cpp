
#include <DxLib.h>
#include <sancur.h>
#include <strcur.h>
#include <dxcur.h>
#include <system.h>

int SmallFontData = 0;
int LargeFontData = 0;

int RecPlayDebug[3] = { 0,0,0 };

/**
 * packNoからパックフォルダパスを取得する
 * @param[out] ret パックフォルダパスの格納先
 * @param[in] size retの長さ、配列数で指定
 * @param[in] packNo パックナンバー
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
	} // buf = "<パック名>"
	FileRead_close(fd);

	strcats_2(ret, size, buf);           // ret = "record/<パック名>"
	stradds_2(ret, size, _T('/'));       // ret = "record/<パック名>/"

	return 0;
}

/**
 * packNoとsongNoから曲フォルダパスを取得する
 * @param[out] ret 曲フォルダパスの格納先
 * @param[in] size retの長さ、配列数で指定
 * @param[in] packNo パックナンバー
 * @param[in] songNo 曲ナンバー
 * @return 0:OK, -1:NG
 */
int RecGetMusicFolderPath(TCHAR *ret, size_t size, uint packNo, uint songNo) {
	TCHAR buf[256];
	DxFile_t fd;

	RecGetPackFolderPath(ret, size, packNo); // ret = "record/<パック名>/"

	strcopy_2(ret, buf, 256);            // buf = "record/<パック名>/"
	strcats_2(buf, 256, _T("list.txt")); // buf = "record/<パック名>/list.txt"

	fd = FileRead_open(buf);
	for (int i = 0; i <= songNo; i++) {
		if (FileRead_eof(fd) != 0) { return -1; }
		FileRead_gets(buf, 256, fd);
	} // buf = "<曲名>"
	FileRead_close(fd);

	strcats_2(ret, size, buf);     // ret = "record/<パック名>/<曲名>"
	stradds_2(ret, size, _T('/')); // ret = "record/<パック名>/<曲名>/"
	return 0;
}

/**
 * packNoとsongNoから曲フォルダ名を取得する
 * @param[out] ret 曲フォルダ名の格納先
 * @param[in] size retの長さ、配列数で指定
 * @param[in] packNo パックナンバー
 * @param[in] songNo 曲ナンバー
 * @return 0:OK, -1:NG
 */
int RecGetMusicFolderName(TCHAR *ret, size_t size, uint packNo, uint songNo) {
	TCHAR GT1[256];
	TCHAR GT2[256];
	DxFile_t fd;
	
	RecGetPackFolderPath(GT2, size, packNo); // GT2 = "record/<パック名>/"

	strcats_2(GT2, 256, _T("list.txt")); // GT2 = "record/<パック名>/list.txt"

	fd = FileRead_open(GT2);
	for (int i = 0; i <= songNo; i++) {
		if (FileRead_eof(fd) != 0) { return -1; }
		FileRead_gets(GT1, 256, fd);
	} // GT1 = "<曲名>"
	FileRead_close(fd);

	strcopy_2(GT1, ret, size);

	return 0;
}

/**
 * packNoとsongNoとdifNoからマップファイルパスを取得する
 * @param[out] ret マップファイルパスの格納先
 * @param[in] size retの長さ、配列数で指定
 * @param[in] packNo パックナンバー
 * @param[in] songNo 曲ナンバー
 * @param[in] difNo 難易度ナンバー
 * @return 0:OK, -1:NG
 */
int RecGetMusicMapRrsPath(TCHAR *ret, size_t size, uint packNo, uint songNo, rec_dif_t difNo) {
	if (RecGetMusicFolderPath(ret, size, packNo, songNo) != 0) { return -1; }
	stradds_2(ret, size, (TCHAR)((int)_T('0') + (int)difNo));
	strcats_2(ret, size, _T(".rrs"));
	return 0;
}

/**
 * packNoとsongNoとdifNoからマップテキストファイルパスを取得する
 * @param[out] ret マップテキストファイルパスの格納先
 * @param[in] size retの長さ、配列数で指定
 * @param[in] packNo パックナンバー
 * @param[in] songNo 曲ナンバー
 * @param[in] difNo 難易度ナンバー
 * @return 0:OK, -1:NG
 */
int RecGetMusicMapTxtPath(TCHAR *ret, size_t size, uint packNo, uint songNo, rec_dif_t difNo) {
	if (RecGetMusicFolderPath(ret, size, packNo, songNo) != 0) { return -1; }
	stradds_2(ret, size, (TCHAR)((int)_T('0') + (int)difNo));
	strcats_2(ret, size, _T(".txt"));
	return 0;
}
