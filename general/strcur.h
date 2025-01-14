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
 * �o�b�t�@�I�[�o�[�������N�����\��������ׁA���̃}�N���͂����g������_���A�����strands_2���g���Ă�������
 * p1�̐擪��p2�����邩�ǂ����𒲂ׂ�
 */
#define strands(p1, p2) strands_2(p1, 50, p2, 50)

/**
 * don't use
 * �o�b�t�@�I�[�o�[�������N�����\��������ׁA���̃}�N���͂����g������_���A�����strcopy_2���g���Ă�������
 * p1��p2�ɃR�s�[����Bc��1�������255���܂ŏ���������(����������)
 */
#define strcopy(p1, p2, c) _STRCPY_S(p2, 255, p1)

/**
 * p1��p2�ɃR�s�[����
 */
#define strcopy_2(p1, p2, size) _STRCPY_S(p2, size, p1)

/**
 * don't use
 * �o�b�t�@�I�[�o�[�������N�����\��������ׁA���̊֐��͂����g������_���A�����strmods_2���g���Ă�������
 * p1����擪a����������
 */
#define strmods(p1, a) strmods_2(p1, 255, a)

/**
 * don't use
 * �o�b�t�@�I�[�o�[�������N�����\��������ׁA���̊֐��͂����g������_���A�����strcats_2���g���Ă�������
 * p1�̍Ō��p2��t����
 */
#define strcats(p1, p2) strcats_2(p1, 255, p2)

/**
 * don't use
 * �o�b�t�@�I�[�o�[�������N�����\��������ׁA���̊֐��͂����g������_���A�����strlens_2���g���Ă�������
 * ������s�̒������擾����
 */
#define strlens(s) strlens_2(s, 255)

/**
 * don't use
 */
#define strwlens(s)

/**
 * don't use
 * �o�b�t�@�I�[�o�[�������N�����\��������ׁA���̊֐��͂����g������_���A�����strsans_3���g���Ă�������
 * ������𐔎��ɕϊ��ɂ���B
 */
#define strsans(p1) strsans_3(p1, 50)

/**
 * don't use
 * �o�b�t�@�I�[�o�[�������N�����\��������ׁA���̊֐��͂����g������_���A�����strsansD���g���Ă�������
 * strsans_3()�̏������ɂ�������
 */
#define strsans2(p1) strsansD(p1, 50)

/**
 * don't use
 * �o�b�t�@�I�[�o�[�������N�����\��������ׁA���̊֐��͂����g������_���A�����strnex_2���g���Ă�������
 * ����'/'��':'������Ƃ���܂ŏ���
 */
#define strnex(p1) strnex_2(p1, 255)

/**
 * don't use
 * �o�b�t�@�I�[�o�[�������N�����\��������ׁA���̊֐��͂����g������_���A�����strnex_EX2���g���Ă�������
 * �w��̕���p3������Ƃ���܂ŏ���
 */
#define strnex_EX(p1, p3) strnex_EX2(p1, 255, p3)

/**
 * don't use
 * �o�b�t�@�I�[�o�[�������N�����\��������ׁA���̊֐��͂����g������_���A�����stradds_2���g���Ă�������
 * p1�̍Ō��a�̕�����ǉ�����
 */
#define stradds(p1, a) stradds_2(p1, 255, a)

/**
 * don't use
 * ����Ă邱�Ƃ�vsprintf_s�ƈꏏ�ׁ̈A�킴�킴���̊֐����g���K�v�Ȃ��Bvsprintf_s���g���Ă��������B
 */
#define vScanPrintfStr(ret, size, s, as) _VSPRINTF_S(ret, size, s, as)

/* TODO: �����rec�ɂ���ׂ� */
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