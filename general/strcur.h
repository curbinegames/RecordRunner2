#pragma once

typedef enum {
	FILETYPE_TXT,
	FILETYPE_RRS,
} TXT_OR_RRS;

extern int strands(const wchar_t *p1, const wchar_t *p2);
extern void strcopy(const wchar_t*, wchar_t*, int);
extern void strcopy_2(const wchar_t *p1, wchar_t *p2, size_t size);
extern void strmods(wchar_t*, int);
extern void strcats(wchar_t*, const wchar_t*);
extern void strcats_2(TCHAR p1[], size_t size, const TCHAR *p2);
extern int strlens(const wchar_t* s);
extern int strwlens(const wchar_t* s);
extern int strrans(wchar_t*);
extern void strnex(wchar_t*);
extern void strnex_EX(wchar_t*, wchar_t);
extern void stradds(TCHAR *p1, TCHAR a);
extern void stradds_2(TCHAR p1[], size_t size, TCHAR a);
extern int strsans(wchar_t*);
extern double strsans2(wchar_t*);
extern void strnums(TCHAR ret[], int val, size_t size);
extern void strnumsD(TCHAR ret[], double val, size_t size, int under);
extern void get_rec_file(wchar_t *s, int pack, int music, int dif, TXT_OR_RRS torr);
extern void vScanPrintfStr(TCHAR *ret, size_t size, const TCHAR s[], va_list as);
extern void ScanPrintfStr(TCHAR *ret, size_t size, const TCHAR s[], ...);
