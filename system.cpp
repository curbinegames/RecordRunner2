
#include <DxLib.h>
#include "general/sancur.h"
#include "general/strcur.h"
#include "general/dxcur.h"

int SmallFontData = 0;
int LargeFontData = 0;

/**
 * packNoとsongNoからファイルパスを取得する
 * @param[out] ret ファイルパスの格納先
 * @param[in] size retの長さ、配列数で指定
 * @param[in] packNo パックナンバー
 * @param[in] songNo 曲ナンバー
 */
void RecGetMusicPath(TCHAR *ret, size_t size, uint packNo, uint songNo) {
	TCHAR buf[256];

	DxFile_t fd;

	strcopy_2(_T("record/"), ret, size); // ret = "record/"

	fd = FileRead_open(_T("RecordPack.txt"));
	for (int i = 0; i <= packNo; i++) { FileRead_gets(buf, 256, fd); } // buf = "<パック名>"
	FileRead_close(fd);

	strcats_2(ret, size, buf);           // ret = "record/<パック名>"
	stradds_2(ret, size, _T('/'));       // ret = "record/<パック名>/"
	strcopy_2(ret, buf, 256);            // buf = "record/<パック名>/"
	strcats_2(buf, 256, _T("list.txt")); // buf = "record/<パック名>/list.txt"

	fd = FileRead_open(buf);
	for (int i = 0; i <= songNo; i++) { FileRead_gets(buf, 256, fd); } // buf = "<曲名>"
	FileRead_close(fd);

	strcats_2(ret, size, buf);     // ret = "record/<パック名>/<曲名>"
	stradds_2(ret, size, _T('/')); // ret = "record/<パック名>/<曲名>/"
	return;
}
