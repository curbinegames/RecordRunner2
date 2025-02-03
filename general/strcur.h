/**
 * @file strcur.h
 * @brief 文字列操作で便利なものを集めたコード
 * @author curbine
 * @date 2024/7/19
 * @note 個人が趣味で製作したものなので、品質は保証しません。このコードを使用したことによるトラブルの責任は負いかねます。
 */

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
 * バッファオーバーランを起こす可能性がある為、このマクロはもう使っちゃダメ、代わりにstrcopy_2を使ってください
 * p1をp2にコピーする。cは使用しない
 */
#define strcopy(p1, p2, c) _STRCPY_S(p2, 255, p1)

/**
 * p1をp2にコピーする
 */
#define strcopy_2(p1, p2, size) _STRCPY_S(p2, size, p1)

/* TODO: これもrecにあるべき */
typedef enum {
	FILETYPE_TXT,
	FILETYPE_RRS,
} TXT_OR_RRS;

/**
 * don't use
 * バッファオーバーランを起こす可能性がある為、このマクロはもう使っちゃダメ、代わりにstrands_2を使ってください
 * p1の先頭にp2があるかどうかを調べる
 */
#define strands(p1, p2) strands_2(p1, 50, p2, 50)

extern int strands_2(const TCHAR *p1, size_t p1size, const TCHAR *p2, size_t p2size);
extern void strmods(TCHAR *p1, int a); /* don't use */
extern void strmods_2(TCHAR *p1, size_t size, int a);
extern void strcats(TCHAR *p1, const TCHAR *p2); /* don't use */
extern void strcats_2(TCHAR p1[], size_t size, const TCHAR *p2);
extern int strlens(const TCHAR* s); /* don't use */
extern int strlens_2(const TCHAR* s, size_t size);
extern int strwlens(const TCHAR* s); /* don't use */
extern int strsans(const TCHAR *p1); /* don't use */
extern int strsans_3(const TCHAR *p1, size_t size);
extern double strsans2(const TCHAR *p1); /* don't use */
extern double strsansD(const TCHAR *p1, size_t size);
extern void strnex(TCHAR *p1); /* don't use */
extern void strnex_2(TCHAR *p1, size_t size);
extern void strnex_EX(TCHAR *p1, TCHAR p3); /* don't use */
extern void strnex_EX2(TCHAR *p1, TCHAR p3);
extern void stradds(TCHAR *p1, TCHAR a); /* don't use */
extern void stradds_2(TCHAR p1[], size_t size, TCHAR a);
extern void strnums(TCHAR ret[], int val, size_t size);
extern void strnumsD(TCHAR ret[], double val, size_t size, int under);
extern void vScanPrintfStr(TCHAR *ret, size_t size, const TCHAR s[], va_list as);
extern void ScanPrintfStr(TCHAR *ret, size_t size, const TCHAR s[], ...);
extern int strrans(const TCHAR *p1);
extern void get_rec_file(TCHAR *s, int pack, int music, int dif, TXT_OR_RRS torr);