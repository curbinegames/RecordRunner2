/**
 * @file strcur.cpp
 * @brief �����񑀍�ŕ֗��Ȃ��̂��W�߂��R�[�h
 * @author curbine
 * @date 2024/7/19
 * @note �l����Ő��삵�����̂Ȃ̂ŁA�i���͕ۏ؂��܂���B���̃R�[�h���g�p�������Ƃɂ��g���u���̐ӔC�͕������˂܂��B
 */

#pragma once

#include <tchar.h>
#include <string.h>
#include "strcur.h"
#include "sancur.h"

/**
 * p1�̐擪��p2�����邩�ǂ����𒲂ׂ�
 * @param[in] p1 �T���ꏊ
 * @param[in] p1size p1�̒����A�z�񐔂Ŏw��
 * @param[in] p2 �T��������
 * @param[in] p2size p2�̒����A�z�񐔂Ŏw��
 * @return int ��������1�A�Ȃ�������0
 */
int strands_2(const TCHAR *p1, size_t p1size, const TCHAR *p2, size_t p2size) {
	for (int i = 0; i < p1size && i < p2size; i++) {
		if (p2[i] == _T('\0')) { break; }
		if (p1[i] != p2[i]) { return false; }
	}
	return true;
}

/**
 * p1����擪a����������
 * @param[out] p1 �Ώۂ̕�����
 * @param[in] size p1�̃T�C�Y
 * @param[in] a ����������
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
 * p1�̍Ō��p2��t����
 * @param[out] p1 ������镶����
 * @param[in] size p1�̃T�C�Y�A�z�񐔂Ŏw��
 * @param[in] p2 ����������
 * @return int 0: ����, 1: ���s
 * @note 1���������ǉ��������Ƃ���stradds_2���g���Ă�������
 * @sa stradds_2
 */
int strcats_2(TCHAR p1[], size_t size, const TCHAR *p2) {
	int offset = -1;

	for (int i = 0; i < size; i++) {
		if (p1[i] == _T('\0')) {
			offset = i;
			break;
		}
	}

	if (offset == -1) { return 1; }

	strcopy_2(p2, &p1[offset], size - offset);

	p1[size - 1] = _T('\0');
	return 0;
}

/**
 * ������s�̒������擾����
 * @param[in] s �ΏۂƂȂ镶����
 * @param[in] size s�̂̃T�C�Y�A�z�񐔂Ŏw��
 * @return int s�̒���
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
 * ������𐔒l�ɕϊ��ɂ���B
 * @param[in] p1 �ϊ����镶����
 * @param[in] size p1�̃T�C�Y�A�z�񐔂Ŏw��
 * @return int �ϊ�����
 * @su strsansD
 * @note �����ȉ��͖�������܂��B�����ȉ����ϊ��������ꍇ��strsansD()���g�p���Ă�������
 * �r���ɐ���,�}�C�i�X�ȊO�̕�������������I���.
 */
int strsans_3(const TCHAR *p1, size_t size) {
	int a = 0, b = 1;
	if (*p1 == _T('R')) { return strrans(p1); } // TODO: rand�͂�����ƍl����

	for (int i = 0; i < 50; i++) {
		if (_T('0') <= *p1 && *p1 <= _T('9')) { a = a * 10 + (*p1 - _T('0')); }
		else if (*p1 == _T('-')) { b *= -1; }
		else { break; }
		p1++;
	}

	return a * b;
}

/**
 * ������𐔒l�ɕϊ��ɂ���B
 * @param[in] p1 �ϊ����镶����
 * @param[in] size p1�̃T�C�Y�A�z�񐔂Ŏw��
 * @return double �ϊ�����
 * @sa strsans_3
 * @note ���̊֐��͏����ȉ����ϊ����܂��B�����݂̂ŗǂ��ꍇ��strsans_3���g�p���Ă��������B
 */
double strsansD(const TCHAR *p1, size_t size) {
	short int b = 1, c = 0, d = 99, i;
	double a = 0;
	if (*p1 == _T('R')) { return strrans(p1); } // TODO: rand�͂�����ƍl����

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
 * ����'/'��':'������Ƃ���܂ŏ���
 * @param[out] p1 �ΏۂƂȂ镶����
 * @param[in] size p1�̃T�C�Y�A�z�񐔂Ŏw��
 */
void strnex_2(TCHAR *p1, size_t size) {
	int i = 0;
	for (i = 0; i < size && p1[i] >= _T(' ') && p1[i] <= _T('}') && p1[i] != _T('/') && p1[i] != _T(':') && p1[i] != _T('\0'); i++) { ; }
	if (p1[i] == _T('/') || p1[i] == _T(':')) { i++; }
	strmods_2(p1, size, i);
	return;
}

/**
 * �w��̕���p3������Ƃ���܂ŏ���
 * @param[out] p1 �ΏۂƂȂ镶����
 * @param[in] p3 �w��̕���
 */
void strnex_EX2(TCHAR *p1, size_t size, TCHAR p3) {
	TCHAR *p2 = p1;
	short int a = 1;
	while ((*p1 >= _T(' ')) && (*p1 <= _T('}')) && (*p1 != p3) && (*p1 != _T('\0')) && (a < size)) {
		a++;
		p1++;
	}
	strmods_2(p2, size, a);
	return;
}

/**
 * p1�̍Ō��a�̕�����ǉ�����
 * @param[out] p1 ������镶����
 * @param[in] size p1�̃T�C�Y�A�z�񐔂Ŏw��
 * @param[in] a ��������
 * @return int 0:����, 1:���s
 * @note �������ǉ��������Ƃ���strcats_2���g���Ă�������
 * @sa strcats_2
 */
int stradds_2(TCHAR p1[], size_t size, TCHAR a) {
	int offset = -1;

	for (int i = 0; i < size; i++) {
		if (p1[i] == _T('\0')) {
			offset = i;
			break;
		}
	}

	if (offset == -1) { return 1; }

	p1[offset] = a;
	p1[offset + 1] = _T('\0');
	p1[size - 1] = _T('\0');
	return 0;
}

/**
 * ���l�𕶎���ɂ���B�����_�ȉ��͖��������
 * @param[out] ret �ϊ����ꂽ������̊i�[�ꏊ
 * @param[in] val �ϊ����鐔�l
 * @param[in] size ret�̒����B�z�񐔂Ŏw��
 * @return int 0:����, 1:���s
 * @note �����_�ȉ���������ɂ������ꍇ��strnumsD���g���Ă�������
 * @sa strnumsD
 */
int strnums(TCHAR ret[], int val, size_t size) {
	int keta = 1;
	int buf = val;
	int minFG = 0;

	if (size < 2) { return 1; }

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

	return 0;
}

/**
 * ���l�𕶎���ɂ���B�����_�ȉ����ϊ�����
 * @param[out] ret �ϊ����ꂽ������̊i�[�ꏊ
 * @param[in] val �ϊ����鐔�l
 * @param[in] size ret�̒����B�z�񐔂Ŏw��
 * @param[in] under �����_�ȉ��̌���
 * @return int 0:����, 1:���s
 * @note �����_�ȉ��𖳎�����ꍇ��strnums���g���Ă�������
 * @sa strnums
 */
int strnumsD(TCHAR ret[], double val, size_t size, int under) {
	int keta = 1;
	int buf = (int)val;
	double bufD = val;
	int minFG = 0;

	strnums(ret, (int)val, size);

	if (under <= 0) { return 0; }

	stradds_2(ret, size, _T('.'));

	for (int i = 0; i < under; i++) {
		bufD *= 10;
		buf = ((int)bufD) % 10;
		stradds_2(ret, size, (TCHAR)(_T('0') + (int)buf));
	}

	ret[size - 1] = _T('\0');

	return 0;
}

/**
 * ����Ă邱�Ƃ�sprintf_s�ƈꏏ�ׁ̈A�킴�킴���̊֐����g���K�v�Ȃ��Bsprintf_s���g���Ă��������B
 * @param[out] ret �ϊ�������������i�[����ꏊ
 * @param[in] size ret�̑傫���B�z�񐔂Ŏw��
 * @param[in] s �ϊ����镶����
 */
void ScanPrintfStr(TCHAR *ret, size_t size, const TCHAR s[], ...) {
	va_list as;
	va_start(as, s);
	_VSPRINTF_S(ret, size, s, as);
	va_end(as);
	return;
}

/* TODO: ���������rec�ɂ���ׂ� */
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

/* TODO: get_rec_file()��rec�ɂ���ׂ� */
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
