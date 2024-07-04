#pragma once

typedef enum {
	FILETYPE_TXT,
	FILETYPE_RRS,
} TXT_OR_RRS;

extern int strands(const wchar_t *p1, const wchar_t *p2);
extern void strcopy(const wchar_t*, wchar_t*, int);
extern void strmods(wchar_t*, int);
extern void strcats(wchar_t*, const wchar_t*);
extern int strlens(const wchar_t* s);
extern int strwlens(const wchar_t* s);
extern int strrans(wchar_t*);
extern void strnex(wchar_t*);
extern void strnex_EX(wchar_t*, wchar_t);
extern void stradds(wchar_t*, wchar_t);
extern int strsans(wchar_t*);
extern double strsans2(wchar_t*);
extern void get_rec_file(wchar_t *s, int pack, int music, int dif, TXT_OR_RRS torr);
