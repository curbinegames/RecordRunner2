/**
 * @file strcur.cpp
 * @brief 文字列操作で便利なものを集めたコード
 * @author curbine
 * @date 2024/7/19
 * @note 個人が趣味で製作したものなので、品質は保証しません。このコードを使用したことによるトラブルの責任は負いかねます。
 */

#pragma once

#include <tchar.h>
#include <string.h>
#include "strcur.h"
#include "sancur.h"

/**
 * p1の先頭にp2があるかどうかを調べる
 * @param[in] p1 探す場所
 * @param[in] p1size p1の長さ、配列数で指定
 * @param[in] p2 探す文字列
 * @param[in] p2size p2の長さ、配列数で指定
 * @return int あったらtrue、なかったらfalse
 */
int strands_2(const TCHAR *p1, size_t p1size, const TCHAR *p2, size_t p2size) {
	for (int i = 0; i < p1size && i < p2size; i++) {
		if (p2[i] == _T('\0')) { break; }
		if (p1[i] != p2[i]) { return false; }
	}
	return true;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrmods_2を使ってください
 * @param[out] p1 対象の文字列
 * @param[in] a 消す文字数
 * @sa strmods_2
 * @details p1から先頭a文字を消す
 */
void strmods(TCHAR *p1, int a) {
	int i;
	TCHAR *p2 = p1;
	for (i = 0; i < a; i++) {
		if (p1[i] == _T('\0')) {
			p1[0] = _T('\0');
			return;
		}
	}
	for (i = 0; p2[i + a] != _T('\0'); i++) {
		p1[i] = p2[i + a];
	}
	p1[i] = _T('\0');
	return;
}

/**
 * p1から先頭a文字を消す
 * @param[out] p1 対象の文字列
 * @param[in] size p1のサイズ
 * @param[in] a 消す文字数
 */
void strmods_2(TCHAR *p1, size_t size, int a) {
	int i;
	TCHAR *p2 = p1;
	for (i = 0; i < a; i++) {
		if (p1[i] == _T('\0')) {
			p1[0] = _T('\0');
			return;
		}
	}
	for (i = 0; p2[i + a] != _T('\0'); i++) {
		p1[i] = p2[i + a];
	}
	p1[i] = _T('\0');
	return;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrcats_2を使ってください
 * @param[out] p1 足される文字列
 * @param[in] p2 足す文字列
 * @sa strcats_2
 * @details p1の最後にp2を付ける
 */
void strcats(TCHAR *p1, const TCHAR *p2) {
	while (*p1 != _T('\0')) p1++;
	for (int i = 0; i < 64 && p2[i] != _T('\0'); i++) {
		*p1 = p2[i];
		p1++;
	}
	*p1 = _T('\0');
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
		if (p1[i] == _T('\0')) {
			offset = i;
			break;
		}
	}

	if (offset == -1) { return; }

	strcopy_2(p2, &p1[offset], size - offset);

	p1[size - 1] = _T('\0');
	return;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrlens_2を使ってください
 * @param[in] s 対象となる文字列
 * @return int sの長さ
 * @sa strlens_2
 * @details 文字列sの長さを取得する
 */
int strlens(const TCHAR* s) {
	int ret = 0;
	for (ret = 0; ret < 255; ret++) {
		if (s[ret] == _T('\0')) {
			break;
		}
	}
	return ret;
}

/**
 * 文字列sの長さを取得する
 * @param[in] s 対象となる文字列
 * @param[in] size sののサイズ、配列数で指定
 * @return int sの長さ
 */
int strlens_2(const TCHAR* s, size_t size) {
	int ret = 0;
	for (ret = 0; ret < size; ret++) {
		if (s[ret] == _T('\0')) {
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
int strwlens(const TCHAR* s) {
	return 0;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrsans_3を使ってください
 * 文字列を数字に変換にする。
 * @param[in] p1 変換する文字列
 * @return int 変換結果
 * @sa strsans_3
 * @details 文字列を数字に変換にする。途中に数字,マイナス以外の文字があったら終わり
 */
int strsans(const TCHAR *p1) {
	int a = 0, b = 1;
	if (*p1 == _T('R')) { return strrans(p1); }

	for (int i = 0; i < 50; i++) {
		if (_T('0') <= *p1 && *p1 <= _T('9')) { a = a * 10 + (*p1 - _T('0')); }
		else if (*p1 == _T('-')) { b *= -1; }
		else { break; }
		p1++;
	}

	return a * b;
}

/**
 * 文字列を数値に変換にする。
 * @param[in] p1 変換する文字列
 * @param[in] size p1のサイズ、配列数で指定
 * @return int 変換結果
 * @su strsansD
 * @note 少数以下は無視されます。少数以下も変換したい場合はstrsansD()を使用してください
 * 途中に数字,マイナス以外の文字があったら終わり.
 */
int strsans_3(const TCHAR *p1, size_t size) {
	int a = 0, b = 1;
	if (*p1 == _T('R')) { return strrans(p1); }

	for (int i = 0; i < 50; i++) {
		if (_T('0') <= *p1 && *p1 <= _T('9')) { a = a * 10 + (*p1 - _T('0')); }
		else if (*p1 == _T('-')) { b *= -1; }
		else { break; }
		p1++;
	}

	return a * b;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrsansDを使ってください
 * @param[in] p1 変換する文字列
 * @return double 変換結果
 * @details strsans()の小数を可にしたもの
 */
double strsans2(const TCHAR *p1) {
	short int b = 1, c = 0, d = 99, i;
	double a = 0;
	if (*p1 == _T('R')) { return strrans(p1); }

	for (int ip = 0; ip < 50; ip++) {
		if (_T('0') <= *p1 && *p1 <= _T('9')) {
			c++;
			a = a * 10 + (*p1 - _T('0'));
		}
		else if (*p1 == _T('-')) { b *= -1; }
		else if (*p1 == _T('.')) { d = c; }
		else {
			for (i = c; i > d; i--) { a /= 10.0; }
			break;
		}
		p1++;
	}

	return b * a;
}

/**
 * 文字列を数値に変換にする。
 * @param[in] p1 変換する文字列
 * @param[in] size p1のサイズ、配列数で指定
 * @return double 変換結果
 * @sa strsans_3
 * @note この関数は少数以下も変換します。整数のみで良い場合はstrsans_3を使用してください。
 */
double strsansD(const TCHAR *p1, size_t size) {
	short int b = 1, c = 0, d = 99, i;
	double a = 0;
	if (*p1 == _T('R')) { return strrans(p1); }

	for (int ip = 0; ip < 50; ip++) {
		if (_T('0') <= *p1 && *p1 <= _T('9')) {
			c++;
			a = a * 10 + (*p1 - _T('0'));
		}
		else if (*p1 == _T('-')) { b *= -1; }
		else if (*p1 == _T('.')) { d = c; }
		else {
			for (i = c; i > d; i--) { a /= 10.0; }
			break;
		}
		p1++;
	}

	return b * a;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrnex_2を使ってください
 * @param[out] p1 対象となる文字列
 * @sa strnex_2
 * @details 次の'/'か':'があるところまで消す
 */
void strnex(TCHAR *p1) {
	int i = 1;
	for (i = 0; p1[i] >= _T(' ') && p1[i] <= _T('}') && p1[i] != _T('/') && p1[i] != _T(':') && p1[i] != _T('\0'); i++) { ; }
	if (p1[i] == _T('/') || p1[i] == _T(':')) { i++; }
	strmods(p1, i);
	return;
}

/**
 * 次の'/'か':'があるところまで消す
 * @param[out] p1 対象となる文字列
 * @param[in] size p1のサイズ、配列数で指定
 */
void strnex_2(TCHAR *p1, size_t size) {
	int i = 1;
	for (i = 0; i < size && p1[i] >= _T(' ') && p1[i] <= _T('}') && p1[i] != _T('/') && p1[i] != _T(':') && p1[i] != _T('\0'); i++) { ; }
	if (p1[i] == _T('/') || p1[i] == _T(':')) { i++; }
	strmods(p1, i);
	return;
}

/**
 * バッファオーバーランを起こす可能性がある為、この関数はもう使っちゃダメ、代わりにstrnex_EX2を使ってください
 * @param[out] p1 対象となる文字列
 * @param[in] p3 指定の文字
 * @sa strnex_EX2
 * @details 指定の文字p3があるところまで消す
 */
void strnex_EX(TCHAR *p1, TCHAR p3) {
	TCHAR *p2 = p1;
	short int a = 1;
	while (*p1 >= _T(' ') && *p1 <= _T('}') && *p1 != p3 && *p1 != _T('\0')) {
		a++;
		p1++;
	}
	strmods(p2, a);
	return;
}

/**
 * 指定の文字p3があるところまで消す
 * @param[out] p1 対象となる文字列
 * @param[in] p3 指定の文字
 */
void strnex_EX2(TCHAR *p1, TCHAR p3) {
	TCHAR *p2 = p1;
	short int a = 1;
	while (*p1 >= _T(' ') && *p1 <= _T('}') && *p1 != p3 && *p1 != _T('\0')) {
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
	while (*p1 != _T('\0')) p1++;
	*p1++ = a;
	*p1 = _T('\0');
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
		if (p1[i] == _T('\0')) {
			offset = i;
			break;
		}
	}

	if (offset == -1) { return; }

	p1[offset] = a;
	p1[offset + 1] = _T('\0');
	p1[size - 1] = _T('\0');
	return;
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

	if (size < 2) { return; }

	while (buf != 0) {
		buf /= 10;
		if (buf != 0) { keta++; }
	}

	if (val < 0) {
		ret[0] = _T('-');
		minFG = 1;
	}

	keta = mins_2(size - minFG - 1, keta);

	buf = val;
	if (val < 0) { buf *= -1; }

	for (int i = 0; i < keta; i++) {
		ret[minFG + keta - i - 1] = (TCHAR)(_T('0') + buf % 10);
		buf /= 10;
	}

	ret[minFG + keta] = _T('\0');
	ret[size - 1] = _T('\0');

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

	if (under <= 0) { return; }

	stradds_2(ret, size, _T('.'));

	for (int i = 0; i < under; i++) {
		bufD *= 10;
		buf = ((int)bufD) % 10;
		stradds_2(ret, size, (TCHAR)(_T('0') + (int)buf));
	}

	ret[size - 1] = _T('\0');

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
	_VSPRINTF_S(ret, size, s, as);
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
	_VSPRINTF_S(ret, size, s, as);
	va_end(as);
	return;
}

/* TODO: これも多分recにあるべき */
int strrans(const TCHAR *p1) {
	int a, b;
	TCHAR buf[16];
	strcopy_2(p1, buf, 16);
	strmods(buf, 2);
	a = strsans(buf);
	strnex_EX(buf, _T(','));
	b = mins(strsans(buf), a);
	return GetRand(b - a) + a;
}

/* TODO: get_rec_file()はrecにあるべき */
void get_rec_file(TCHAR *s, int pack, int music, int dif, TXT_OR_RRS torr) {
	TCHAR ret[255] = _T("record/");
	TCHAR GT1[255];
	int file;
	file = FileRead_open(_T("RecordPack.txt"));
	for (int i = 0; i <= pack; i++) {
		if (FileRead_eof(file) != 0) {
			s[0] = _T('\0');
			return;
		}
		FileRead_gets(GT1, 256, file); /* GT1 = "pask" */
	}
	FileRead_close(file);
	strcats(ret, GT1); /* ret = "record/pask" */
	strcats(ret, _T("/"));  /* ret = "record/pask/" */
	strcopy(ret, GT1, 1); /* GT1 = "record/pask/" */
	strcats(GT1, _T("list.txt")); /* GT1 = "record/pask/list.txt" */
	file = FileRead_open(GT1);
	for (int i = 0; i <= music; i++) {
		if (FileRead_eof(file) != 0) {
			s[0] = _T('\0');
			return;
		}
		FileRead_gets(GT1, 256, file); /* GT1 = "music" */
	}
	FileRead_close(file);
	strcats(ret, GT1); /* ret = "record/pask/music" */
	strcats(ret, _T("/"));  /* ret = "record/pask/music/" */
	GT1[0] = _T('0') + dif;
	GT1[1] = _T('\0');
	strcats(ret, GT1); /* ret = "record/pask/music/3" */
	if (torr == FILETYPE_RRS) {
		strcats(ret, _T(".rrs")); /* ret = "record/pask/music/3.rrs" */
	}
	else {
		strcats(ret, _T(".txt")); /* ret = "record/pask/music/3.txt" */
	}
	strcopy(ret, s, 1);
	return;
}
