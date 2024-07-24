/**
 * @file strcur.cpp
 * @brief 文字列操作で便利なものを集めたコード
 * @author curbine
 * @date 2024/7/19
 * @note 個人が趣味で製作したものなので、品質は保証しません。このコードを使用したことによるトラブルの責任は負いかねます。
 */

#pragma once

#include <string.h>
#include "strcur.h"
#include "sancur.h"

int strands(const wchar_t *p1, const wchar_t *p2);
void strcopy(const wchar_t*, wchar_t*, int);
void strmods(wchar_t*, int);
void strcats(wchar_t*, const wchar_t*);
int strlens(const wchar_t* s);
int strwlens(const wchar_t* s);
int strrans(wchar_t*);
void strnex(wchar_t*);
void strnex_EX(wchar_t*, wchar_t);
void stradds(wchar_t*, wchar_t);
int strsans(wchar_t*);
double strsans2(wchar_t*);
void get_rec_file(wchar_t *s, int pack, int music, int dif, TXT_OR_RRS torr);

/**
 * p1の先頭にp2があるかどうかを調べる
 * @param[in] p1 探す場所
 * @param[in] p2 探す文字列
 * @return int あったら1、なかったら0
 * @sa strands_2
 */
int strands(const wchar_t *p1, const wchar_t *p2) {
	for (int i = 0; i < 50; i++) {
		if (p2[i] == L'\0') {
			break;
		}
		if (p1[i] != p2[i]) {
			return 0;
		}
	}
	return 1;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrcopy_2を使ってください
 * @param[in] p1 コピーするもの
 * @param[in] p2 コピー先
 * @param[in] c 初期化フラグ
 * @sa strcopy_2
 * @details p1をp2にコピーする。cに1を入れると255桁まで初期化する(初期化推奨)
 */
void strcopy(const wchar_t *p1, wchar_t *p2, int c) {
	int a = 0;
	for (int i = 0; i < 250; i++) {
		p2[i] = L'\0';
	}
	for (int i = 0; i < 250 && p1[i] != L'\0'; i++) {
		p2[i] = p1[i];
		p2[i + 1] = L'\0';
	}
	return;
}

/**
 * p1をp2にコピーする
 * @param[in] p1 コピーするもの
 * @param[in] p2 コピー先
 * @param[in] size p2の長さ、配列数で指定
 */
void strcopy_2(const wchar_t *p1, wchar_t *p2, size_t size) {
	int a = 0;
	for (int i = 0; i < size; i++) {
		p2[i] = L'\0';
	}
	for (int i = 0; i < size - 1 && p1[i] != L'\0'; i++) {
		p2[i] = p1[i];
		p2[i + 1] = L'\0';
	}
	return;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrmods_2を使ってください
 * @param[out] p1 対象の文字列
 * @param[in] a 消す文字数
 * @sa strmods_2
 * @details p1から先頭a文字を消す
 */
void strmods(wchar_t *p1, int a) {
	int i;
	wchar_t *p2 = p1;
	for (i = 0; i < a; i++) {
		if (p1[i] == L'\0') {
			p1[0] = L'\0';
			return;
		}
	}
	for (i = 0; p2[i + a] != L'\0'; i++) {
		p1[i] = p2[i + a];
	}
	p1[i] = L'\0';
	return;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrcats_2を使ってください
 * @param[out] p1 足される文字列
 * @param[in] p2 足す文字列
 * @sa strcats_2
 * @details p1の最後にp2を付ける
 */
void strcats(wchar_t *p1, const wchar_t *p2) {
	while (*p1 != L'\0') p1++;
	for (int i = 0; i < 64 && p2[i] != L'\0'; i++) {
		*p1 = p2[i];
		p1++;
	}
	*p1 = L'\0';
	return;
}

/**
 * p1の最後にp2を付ける
 * @param[out] p1 足される文字列
 * @param[in] size p1のサイズ、配列数で指定
 * @param[in] p2 足す文字列
 * @note 1文字だけ追加したいときはstradds_2を使ってください
 * @sa stradds_2
 */
void strcats_2(TCHAR p1[], size_t size, const TCHAR *p2) {
	int offset = -1;

	for (int i = 0; i < size; i++) {
		if (p1[i] == L'\0') {
			offset = i;
			break;
		}
	}

	if (offset == -1) {
		return;
	}

	strcopy_2(p2, &p1[offset], size - offset);

	p1[size - 1] = L'\0';
	return;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrlens_2を使ってください
 * @param[in] s 対象となる文字列
 * @return int sの長さ
 * @sa strlens_2
 * @details 文字列sの長さを取得する
 */
int strlens(const wchar_t* s) {
	int ret = 0;
	for (ret = 0; ret < 255; ret++) {
		if (s[ret] == L'\0') {
			break;
		}
	}
	return ret;
}

/**
 * 文字列sの長さを取得する
 * @param[in] s 対象となる文字列
 * @return int sの長さ
 * @note この関数いる?
 */
int strwlens(const wchar_t* s) {
	int ret = 0;
	for (int p = 0; p < 255; p++) {
		if (s[p] == L'\0') {
			break;
		}
		else if (L' ' <= s[p] && s[p] <= L'~') {
			ret++;
		}
		else {
			ret += 2;
		}
	}
	return ret;
}

/* TODO: これも多分recにあるべき */
int strrans(wchar_t *p1) {
	int a, b;
	strmods(p1, 2);
	a = strsans(p1);
	strnex_EX(p1, L',');
	b = mins(strsans(p1), a);
	return GetRand(b - a) + a;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrnex_2を使ってください
 * @param[out] p1 対象となる文字列
 * @sa strnex_2
 * @details 次の'/'か':'があるところまで消す
 */
void strnex(wchar_t *p1) {
#if 1
	int i = 1;
	for (i = 0; p1[i] >= L' ' && p1[i] <= L'}' && p1[i] != L'/' && p1[i] != L':' && p1[i] != L'\0'; i++) {
		;
	}
	if (p1[i] == L'/' || p1[i] == L':') {
		i++;
	}
	strmods(p1, i);
	return;
#else
	wchar_t *p2 = p1;
	short int a = 1;
	while (*p1 >= L' ' && *p1 <= L'}' && *p1 != L'/' && *p1 != L':' && *p1 != L'\0') {
		a++;
		p1++;
	}
	strmods(p2, a);
	return;
#endif
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrnex_EX2を使ってください
 * @param[out] p1 対象となる文字列
 * @param[in] p3 指定の文字
 * @sa strnex_EX2
 * @details 指定の文字p3があるところまで消す
 */
void strnex_EX(wchar_t *p1,wchar_t p3) {
	wchar_t *p2 = p1;
	short int a = 1;
	while (*p1 >= L' ' && *p1 <= L'}' && *p1 != p3 && *p1 != L'\0') {
		a++;
		p1++;
	}
	strmods(p2, a);
	return;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstradds_2を使ってください
 * @param[out] p1 足される文字列
 * @param[in] a 足す文字
 * @sa stradds_2
 * @details p1の最後にaの文字を追加する
 */
void stradds(TCHAR *p1, TCHAR a) {
	while (*p1 != L'\0') p1++;
	*p1++ = a;
	*p1 = L'\0';
	return;
}

/**
 * p1の最後にaの文字を追加する
 * @param[out] p1 足される文字列
 * @param[in] size p1のサイズ、配列数で指定
 * @param[in] a 足す文字
 * @note 文字列を追加したいときはstrcats_2を使ってください
 * @sa strcats_2
 */
void stradds_2(TCHAR p1[], size_t size, TCHAR a) {
	int offset = -1;

	for (int i = 0; i < size; i++) {
		if (p1[i] == L'\0') {
			offset = i;
			break;
		}
	}

	if (offset == -1) {
		return;
	}

	p1[offset] = a;
	p1[offset + 1] = L'\0';
	p1[size - 1] = L'\0';
	return;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrsans_3を使ってください
 * 文字列を数字に変換にする。
 * @param[in] p1 変換する文字列
 * @return int 変換結果
 * @sa strsans_3
 * @details 文字列を数字に変換にする。途中に数字,マイナス以外の文字があったら終わり
 */
int strsans(wchar_t *p1) {
	int a = 0, b = 1;
	if (*p1 == L'R') {

	}
	else {
		while (*p1 == L'0' ||
			*p1 == L'1' ||
			*p1 == L'2' ||
			*p1 == L'3' ||
			*p1 == L'4' ||
			*p1 == L'5' ||
			*p1 == L'6' ||
			*p1 == L'7' ||
			*p1 == L'8' ||
			*p1 == L'9' ||
			*p1 == L'-') {
			a *= 10;
			switch (*p1) {
			case L'1':
				a += 1;
				break;
			case L'2':
				a += 2;
				break;
			case L'3':
				a += 3;
				break;
			case L'4':
				a += 4;
				break;
			case L'5':
				a += 5;
				break;
			case L'6':
				a += 6;
				break;
			case L'7':
				a += 7;
				break;
			case L'8':
				a += 8;
				break;
			case L'9':
				a += 9;
				break;
			case L'-':
				b *= -1;
				break;
			}
			p1++;
		}
	}
	return a * b;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrsansDを使ってください
 * @param[in] p1 変換する文字列
 * @return double 変換結果
 * @details strsans()の小数を可にしたもの
 */
double strsans2(wchar_t *p1) {
	short int b = 1, c = 0, d = 99, i;
	double a = 0;
	if (*p1 == L'R') { return strrans(p1); }
	else {
		while (1) {
			if (*p1 >= L'0' && *p1 <= L'9') {
				c++;
				a *= 10;
				a += *p1 - 48;
			}
			else if (*p1 == L'-') b *= -1;
			else if (*p1 == L'.') d = c;
			else {
				for (i = c; i > d; i--) a /= 10.0;
				return b * a;
			}
			p1++;
		}
	}
}

/**
 * 数値を文字列にする。小数点以下は無視される
 * @param[out] ret 変換された文字列の格納場所
 * @param[in] val 変換する数値
 * @param[in] size retの長さ。配列数で指定
 * @note 小数点以下も文字列にしたい場合はstrnumsDを使ってください
 * @sa strnumsD
 */
void strnums(TCHAR ret[], int val, size_t size) {
	int keta = 1;
	int buf = val;
	int minFG = 0;

	if (size < 2) {
		return;
	}

	while (buf != 0) {
		buf /= 10;
		if (buf != 0) {
			keta++;
		}
	}

	if (val < 0) {
		ret[0] = L'-';
		minFG = 1;
	}

	keta = mins_2(size - minFG - 1, keta);

	buf = val;
	if (val < 0) {
		buf *= -1;
	}

	for (int i = 0; i < keta; i++) {
		ret[minFG + keta - i - 1] = (TCHAR)(L'0' + buf % 10);
		buf /= 10;
	}

	ret[minFG + keta] = L'\0';
	ret[size - 1] = L'\0';

	return;
}

/**
 * 数値を文字列にする。小数点以下も変換する
 * @param[out] ret 変換された文字列の格納場所
 * @param[in] val 変換する数値
 * @param[in] size retの長さ。配列数で指定
 * @param[in] under 小数点以下の桁数
 * @note 小数点以下を無視する場合はstrnumsを使ってください
 * @sa strnums
 */
void strnumsD(TCHAR ret[], double val, size_t size, int under) {
	int keta = 1;
	int buf = (int)val;
	double bufD = val;
	int minFG = 0;

	strnums(ret, (int)val, size);

	if (under <= 0) {
		return;
	}

	stradds_2(ret, size, L'.');

	for (int i = 0; i < under; i++) {
		bufD *= 10;
		buf = ((int)bufD) % 10;
		stradds_2(ret, size, (TCHAR)(L'0' + (int)buf));
	}

	ret[size - 1] = L'\0';

	return;
}

/* TODO: get_rec_file()はrecにあるべき */
void get_rec_file(wchar_t *s, int pack, int music, int dif, TXT_OR_RRS torr) {
	wchar_t ret[255] = L"record/";
	wchar_t GT1[255];
	int file;
	file = FileRead_open(L"RecordPack.txt");
	for (int i = 0; i <= pack; i++) {
		if (FileRead_eof(file) != 0) {
			s[0] = L'\0';
			return;
		}
		FileRead_gets(GT1, 256, file); /* GT1 = "pask" */
	}
	FileRead_close(file);
	strcats(ret, GT1); /* ret = "record/pask" */
	strcats(ret, L"/");  /* ret = "record/pask/" */
	strcopy(ret, GT1, 1); /* GT1 = "record/pask/" */
	strcats(GT1, L"list.txt"); /* GT1 = "record/pask/list.txt" */
	file = FileRead_open(GT1);
	for (int i = 0; i <= music; i++) {
		if (FileRead_eof(file) != 0) {
			s[0] = L'\0';
			return;
		}
		FileRead_gets(GT1, 256, file); /* GT1 = "music" */
	}
	FileRead_close(file);
	strcats(ret, GT1); /* ret = "record/pask/music" */
	strcats(ret, L"/");  /* ret = "record/pask/music/" */
	GT1[0] = L'0' + dif;
	GT1[1] = L'\0';
	strcats(ret, GT1); /* ret = "record/pask/music/3" */
	if (torr == FILETYPE_RRS) {
		strcats(ret, L".rrs"); /* ret = "record/pask/music/3.rrs" */
	}
	else {
		strcats(ret, L".txt"); /* ret = "record/pask/music/3.txt" */
	}
	strcopy(ret, s, 1);
	return;
}

/**
 * やってることはvsprintf_sと一緒の為、わざわざこの関数を使う必要なし。vsprintf_sを使ってください。
 * @param[out] ret 変換した文字列を格納する場所
 * @param[in] size retの大きさ。配列数で指定
 * @param[in] s 変換する文字列
 * @param[in] as valistの実体
 */
void vScanPrintfStr(TCHAR *ret, size_t size, const TCHAR s[], va_list as) {
	enum {
		NON,
		PAR,
		ESC,
	} mode = NON;
	int count;
	TCHAR buf[32];
	int unsignval = 0;

	ret[0] = L'\0';

	for (int i = 0; s[i] != L'\0'; i++) {
		switch (mode) {
		case NON:
			if (s[i] == L'%') {
				mode = PAR;
			}
			else if (s[i] == L'\\') {
				mode = ESC;
				stradds_2(ret, size, L'\\');
			}
			else {
				stradds_2(ret, size, s[i]);
			}
			break;
		case PAR:
			switch (s[i]) {
			case L'+':
				stradds_2(ret, size, L'+');
				break;
			case L'd':
			case L'o':
			case L'x':
			case L'e':
			case L'g':
			case L'p':
				strnums(buf, va_arg(as, int), 32);
				strcats_2(ret, size, buf);
				break;
			case L'u':
				if (unsignval)
				strnums(buf, (unsigned int)va_arg(as, int), 32);
				strcats_2(ret, size, buf);
				break;
			case L'f':
				strnumsD(buf, va_arg(as, double), 32, 6);
				strcats_2(ret, size, buf);
				break;
			case L's':
				strcats_2(ret, size, va_arg(as, TCHAR *));
				break;
			case L'c':
				stradds_2(ret, size, va_arg(as, TCHAR));
				break;
			}
			if ((s[i] != L'l') && (s[i] != L'h') && (s[i] != L'+') && (s[i] != L'-') && (s[i] != L'.') &&
				((s[i] < L'0') || (s[i] > L'9')))
			{
				mode = NON;
			}
			break;
		case ESC:
			stradds_2(ret, size, s[i]);
			mode = NON;
			break;
		}
	}

	ret[size - 1] = L'\0';
	return;
}

/**
 * やってることはsprintf_sと一緒の為、わざわざこの関数を使う必要なし。sprintf_sを使ってください。
 * @param[out] ret 変換した文字列を格納する場所
 * @param[in] size retの大きさ。配列数で指定
 * @param[in] s 変換する文字列
 */
void ScanPrintfStr(TCHAR *ret, size_t size, const TCHAR s[], ...) {
	va_list as;
	va_start(as, s);
	vScanPrintfStr(ret, size, s, as);
	va_end(as);
	return;
}
