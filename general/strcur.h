#pragma once

#include <string.h>
#include <tchar.h>

#ifdef UNICODE
#define _VSPRINTF_S vswprintf_s
#define _STRCPY_S wcscpy_s

#else
#define _VSPRINTF_S vsprintf_s
#define _STRCPY_S strcpy_s

#endif

/**
 * don't use
 * バッファオーバーランを起こす可能性がある為、このマクロはもう使っちゃダメ、代わりにstrands_2を使ってください
 * p1の先頭にp2があるかどうかを調べる
 */
#define strands(p1, p2) strands_2(p1, 50, p2, 50)

/**
 * don't use
 * バッファオーバーランを起こす可能性がある為、このマクロはもう使っちゃダメ、代わりにstrcopy_2を使ってください
 * p1をp2にコピーする。cに1を入れると255桁まで初期化する(初期化推奨)
 */
#define strcopy(p1, p2, c) _STRCPY_S(p2, 255, p1)

/**
 * p1をp2にコピーする
 */
#define strcopy_2(p1, p2, size) _STRCPY_S(p2, size, p1)

/**
 * don't use
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrmods_2を使ってください
 * p1から先頭a文字を消す
 */
#define strmods(p1, a) strmods_2(p1, 255, a)

/**
 * don't use
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrcats_2を使ってください
 * p1の最後にp2を付ける
 */
#define strcats(p1, p2) strcats_2(p1, 255, p2)

/**
 * don't use
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrlens_2を使ってください
 * 文字列sの長さを取得する
 */
#define strlens(s) strlens_2(s, 255)

/**
 * don't use
 */
#define strwlens(s)

/**
 * don't use
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrsans_3を使ってください
 * 文字列を数字に変換にする。
 */
#define strsans(p1) strsans_3(p1, 50)

/**
 * don't use
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrsansDを使ってください
 * strsans_3()の小数を可にしたもの
 */
#define strsans2(p1) strsansD(p1, 50)

/**
 * don't use
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrnex_2を使ってください
 * 次の'/'か':'があるところまで消す
 */
#define strnex(p1) strnex_2(p1, 255)

/**
 * don't use
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrnex_EX2を使ってください
 * 指定の文字p3があるところまで消す
 */
#define strnex_EX(p1, p3) strnex_EX2(p1, 255, p3)

/**
 * don't use
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstradds_2を使ってください
 * p1の最後にaの文字を追加する
 */
#define stradds(p1, a) stradds_2(p1, 255, a)

/**
 * don't use
 * やってることはvsprintf_sと一緒の為、わざわざこの関数を使う必要なし。vsprintf_sを使ってください。
 */
#define vScanPrintfStr(ret, size, s, as) _VSPRINTF_S(ret, size, s, as)

/* TODO: これはrecにあるべき */
typedef enum {
	FILETYPE_TXT,
	FILETYPE_RRS,
} TXT_OR_RRS;

extern int strands_2(const TCHAR *p1, size_t p1size, const TCHAR *p2, size_t p2size);
extern void strmods_2(TCHAR *p1, size_t size, int a);
extern int strcats_2(TCHAR p1[], size_t size, const TCHAR *p2);
extern int strlens_2(const TCHAR* s, size_t size);
extern int strsans_3(const TCHAR *p1, size_t size);
extern double strsansD(const TCHAR *p1, size_t size);
extern void strnex_2(TCHAR *p1, size_t size);
extern void strnex_EX2(TCHAR *p1, TCHAR p3);
extern int stradds_2(TCHAR p1[], size_t size, TCHAR a);
extern int strnums(TCHAR ret[], int val, size_t size);
extern int strnumsD(TCHAR ret[], double val, size_t size, int under);
extern void ScanPrintfStr(TCHAR *ret, size_t size, const TCHAR s[], ...);
extern int strrans(const TCHAR *p1);
extern void get_rec_file(TCHAR *s, int pack, int music, int dif, TXT_OR_RRS torr);