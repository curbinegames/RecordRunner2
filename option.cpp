
#include "DxLib.h"
#include "general/dxcur.h"
#include "general/strcur.h"
#include "general/sancur.h"
#include "RecWindowRescale.h"
#include "system.h"
#include "helpBar.h"
#include "option.h"

/**
 * TODO: �ǉ��������I�v�V����
 * �@�����̑傫��
 * �@BGM����
 * �@SE����
 * �@PLAYING�o�[�J�X�^�}�C�Y
 */

typedef void (*rec_opt_action_t)(TCHAR *ret, int pal, int lang);

typedef struct rec_opt_text_s {
	const rec_opt_action_t action;
	const struct {
		wchar_t jp[12];
		wchar_t en[30];
	} title;
	const struct {
		wchar_t jp[64];
		wchar_t en[64];
	} detail;
	const int min;
	const int max;
	const int loop;
	int *val_p;
} rec_opt_text_t;

rec_option_t optiondata;

static const int title_txposx = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 100);
static const int title_txposy = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 50);
static const int title_txgapy = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 50);

static const int det_txposx = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 20);
static const int det_txposy = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 410);

int RecOpenOptionFile(int *data) {
	FILE *fp = NULL;
	_wfopen_s(&fp, L"save/system.dat", L"rb");
	if (fp == NULL) {
		return 1;
	}
	fread(data, sizeof(int), 7, fp);
	fclose(fp);
	return 0;
}

int RecOpenOptionFile2(rec_option_t *data) {
	FILE *fp = NULL;
	_wfopen_s(&fp, L"save/system2.dat", L"rb");
	if (fp == NULL) {
		return 1;
	}
	fread(data, sizeof(rec_option_t), 1, fp);
	fclose(fp);
	return 0;
}

int RecWriteOptineFile(int *data) {
	FILE *fp = NULL;
	_wfopen_s(&fp, L"save/system.dat", L"wb");
	if (fp == NULL) {
		return 1;
	}
	fwrite(data, sizeof(int), 7, fp);
	fclose(fp);
	return 0;
}

int RecWriteOptineFile2(rec_option_t *data) {
	FILE *fp = NULL;
	_wfopen_s(&fp, L"save/system2.dat", L"wb");
	if (fp == NULL) {
		return 1;
	}
	fwrite(data, sizeof(rec_option_t), 1, fp);
	fclose(fp);
	return 0;
}

static void RecOptionChar(TCHAR *ret, int pal, int lang) {
	const TCHAR jp[3][20] = {
		L"�s�b�J�[", L"�}�b�v�Q�[�^�[", L"�e�C���["
	};

	const TCHAR en[3][10] = {
		L"Picker", L"MapGator", L"Taylor"
	};

	if (lang == LANG_JP) { strcopy_2(jp[pal], ret, 32); }
	else if (lang == LANG_EN) { strcopy_2(en[pal], ret, 32); }

	return;
}

static void RecOptionOffset(TCHAR *ret, int pal, int lang) {
	strnums(ret, pal * 5, 32);
	return;
}

static void RecOptionSE(TCHAR *ret, int pal, int lang) {
	if (pal == 0) { strcopy_2(L"on", ret, 32); }
	else if (pal == 1) { strcopy_2(L"off", ret, 32); }

	return;
}

static void RecOptionBackBright(TCHAR *ret, int pal, int lang) {
	wchar_t jp[4][10] = {
		L"�^����", L"�Â�", L"����", L"���邢"
	};

	wchar_t en[4][10] = {
		L"Black", L"Dark", L"Middle", L"Bright"
	};

	if (lang == LANG_JP) { strcopy_2(jp[pal], ret, 32); }
	else if (lang == LANG_EN) { strcopy_2(en[pal], ret, 32); }

	return;
}

static void RecOptionLang(TCHAR *ret, int pal, int lang) {
	if (pal == 0) { strcopy_2(L"���{��", ret, 32); }
	else if (pal == 1) { strcopy_2(L"English", ret, 32); }

	return;
}

static void RecOptionButtonDet(TCHAR *ret, int pal, int lang) {
	if (pal == 0) { strcopy_2(L"off", ret, 32); }
	else if (pal == 1) { strcopy_2(L"on", ret, 32); }

	return;
}

static void RecOptionComboPos(TCHAR *ret, int pal, int lang) {
	wchar_t jp[6][10] = {
		L"�����̏�", L"����", L"�E��", L"����", L"�L�����̏�", L"�\�����Ȃ�"
	};

	wchar_t en[6][12] = {
		L"top centre", L"top left", L"top right", L"centre", L"near chara", L"nope"
	};

	if (lang == LANG_JP) { strcopy_2(jp[pal], ret, 32); }
	else if (lang == LANG_EN) { strcopy_2(en[pal], ret, 32); }

	return;
}

static rec_opt_text_t optionstr[] {
	{
		RecOptionChar,
		L"�L�����N�^�[", L"Character",
		L"�g�p����L�����N�^�[��ς��܂��B",
		L"Choose the character you use.",
		0, 2, TRUE
	}, {
		RecOptionOffset,
		L"�I�t�Z�b�g", L"Offset",
		L"����������Ă���^�C�~���O��ς��܂��B\n���₷�ƒx��āA���炷�Ƒ��߂ɗ���܂��B",
		L"Change the timing of note. Increase to late, Decrease to early.",
		-2000, 2000, FALSE
	}, {
		RecOptionSE,
		L"���ʉ�", L"SE",
		L"�����������������̌��ʉ���炷���ǂ�����ς��܂�",
		L"Choose whether to sound of hitting notes.",
		0, 1, TRUE
	}, {
		RecOptionBackBright,
		L"�w�i�̖��邳", L"Background Brightness",
		L"�w�i�̖��邳��ς��܂��B",
		L"Choose brightness of background.",
		0, 3, TRUE
	}, {
		RecOptionLang,
		L"���� Language", L"���� Language",
		L"�Q�[���Ŏg�������ς��܂��B\nChoose the lunguage in this game.",
		L"�Q�[���Ŏg�������ς��܂��B\nChoose the lunguage in this game.",
		0, 1, TRUE
	}, {
		RecOptionButtonDet,
		L"�{�^���\��", L"Button Guide",
		L"�{�^���̉����󋵂��v���C��ʂɕ\�����܂��B",
		L"Choose whether to display the key states on playing mode.",
		0, 1, TRUE
	}, {
		RecOptionComboPos,
		L"����\���ʒu", L"Judge Position",
		L"����̕\���ꏊ�����߂܂��B",
		L"Choose judge position.",
		0, 5, TRUE
	}
};

static const int optionstr_count = sizeof(optionstr) / sizeof(rec_opt_text_t);

now_scene_t option(void) {
	int command = 0;
	struct {
		int back = -1;
		int cursor = -1;
	} pic;
	int sel = -1;
	int exitFg = 0;

	if (RecOpenOptionFile2(&optiondata) != 0) {
		int	data[7] = { 0,0,0,2,0,0,0 };
		RecOpenOptionFile(data);
		optiondata.chara      = data[0];
		optiondata.offset     = data[1];
		optiondata.SEenable   = data[2];
		optiondata.backbright = data[3];
		optiondata.lang       = data[4];
		optiondata.keydetail  = data[5];
		optiondata.combopos   = data[6];
	}

	optionstr[0].val_p = &optiondata.chara;
	optionstr[1].val_p = &optiondata.offset;
	optionstr[2].val_p = &optiondata.SEenable;
	optionstr[3].val_p = &optiondata.backbright;
	optionstr[4].val_p = &optiondata.lang;
	optionstr[5].val_p = &optiondata.keydetail;
	optionstr[6].val_p = &optiondata.combopos;

	for (int i = 0; i < 7; i++) {
		if (*optionstr[i].val_p < optionstr[i].min) {
			*optionstr[i].val_p = 0;
		}
		if (optionstr[i].max < *optionstr[i].val_p) {
			*optionstr[i].val_p = 0;
		}
	}

	pic.back   = LoadGraph(L"picture/OPTION back.png");
	pic.cursor = LoadGraph(L"picture/OC.png");
	sel        = LoadSoundMem(L"sound/select.wav");

	AvoidKeyRush();

	//����
	while (1) {		
		InputAllKeyHold();
		switch (GetKeyPushOnce()) {
		case KEY_INPUT_LEFT:
			PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			(*optionstr[command].val_p)--;
			if (*optionstr[command].val_p < optionstr[command].min) {
				if (optionstr[command].loop == FALSE) {
					*optionstr[command].val_p = optionstr[command].min;
				}
				else {
					*optionstr[command].val_p = optionstr[command].max;
				}
			}
			break;
		case KEY_INPUT_RIGHT:
			PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			(*optionstr[command].val_p)++;
			if (optionstr[command].max < *optionstr[command].val_p) {
				if (optionstr[command].loop == FALSE) {
					*optionstr[command].val_p = optionstr[command].max;
				}
				else {
					*optionstr[command].val_p = optionstr[command].min;
				}
			}
			break;
		case KEY_INPUT_UP:
			PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			command = (command + optionstr_count - 1) % optionstr_count;
			break;
		case KEY_INPUT_DOWN:
			PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			command = (command + 1) % optionstr_count;
			break;
		case KEY_INPUT_BACK:
			RecWriteOptineFile2(&optiondata);
			{
				int buf[7] = {
					optiondata.chara,
					optiondata.offset,
					optiondata.SEenable,
					optiondata.backbright,
					optiondata.lang,
					optiondata.keydetail,
					optiondata.combopos
				};
				RecWriteOptineFile(buf);
			}
			exitFg = 1;
			break;
#if 0 /* �f�o�b�O�p�R�[�h */
		case KEY_INPUT_Q:
#if 0 /* �S���ʒ萔���o�� */
			/* output dif */
			int G[10];
			int difl[4] = { 0,0,0,0 };
			wchar_t file[255];
			wchar_t songN[4][255];
			songN[0][0] = L'\0';
			songN[1][0] = L'\0';
			songN[2][0] = L'\0';
			songN[3][0] = L'\0';
			playnum_box allnum;
			FILE *qfp;
			FILE *ofp;
			G[1] = _wfopen_s(&ofp, L"difout.txt", L"w");
			fwprintf(ofp, L"\t\t\tEASY\tNORMAL\tHARD\tANOTHER\n");
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 20; j++) {
					difl[0] = 0;
					difl[1] = 0;
					difl[2] = 0;
					difl[3] = 0;
					songN[0][0] = L'\0';
					songN[1][0] = L'\0';
					songN[2][0] = L'\0';
					songN[3][0] = L'\0';
					for (int dif = 0; dif < 4; dif++) {
						RecordLoad2(i, j, dif + 1);
						get_rec_file(file, i, j, dif + 1, FILETYPE_RRS);
						G[0] = _wfopen_s(&qfp, file, L"rb");//rrs�f�[�^��ǂݍ���
						if (G[0] == 0) {
							fread(&allnum, sizeof(playnum_box), 1, qfp);//�e�f�[�^�̌�

							fseek(qfp, 255 * 4 + sizeof(int) + sizeof(double), SEEK_CUR);

							fread(&songN[dif], 255, 1, qfp);//�Ȗ�

							fseek(qfp, 255 + sizeof(short int) * 2 +
								sizeof(int) * 1585 + sizeof(double) * 990 +
								sizeof(int) * (allnum.Ymovenum[0] * 4 + allnum.Ymovenum[1] * 4 +
									allnum.Ymovenum[2] * 4 + allnum.Ymovenum[3] * 4 +
									allnum.Ymovenum[4] * 4 + allnum.Xmovenum[0] * 4 +
									allnum.Xmovenum[1] * 4 + allnum.Xmovenum[2] * 4) +
								sizeof(struct note_box) * (allnum.notenum[0] + allnum.notenum[1] +
									allnum.notenum[2]), SEEK_CUR);

							fread(&difl[dif], sizeof(int), 1, qfp);
							fclose(qfp);
						}
					}
					for (G[0] = 0; G[0] < 4; G[0]++) {
						if (songN[G[0]][0] != L'\0') {
							break;
						}
					}
					if (G[0] >= 4) {
						break;
					}
					songN[G[0]][23] = L'\0';
					fwprintf(ofp, L"%s", songN[G[0]]);
					for (int i = 24 - strlens(songN[G[0]]); i > 0; i -= 8) {
						fwprintf(ofp, L"\t");
					}
					for (int dif = 0; dif < 4; dif++) {
						if (difl[dif] == 0) {
							fwprintf(ofp, L"-");
						}
						else {
							fwprintf(ofp, L"%2.2f", difl[dif] / 100.0);
						}
						if (dif < 3) {
							fwprintf(ofp, L"\t");
						}
					}
					fwprintf(ofp, L"\n");
				}
			}
			fclose(ofp);
#elif 0 /* �v���C���[�̃��[�g�̏ڍׂ��o�� */
			FILE* fp;
			FILE* outfp;
			play_rate_t temp;
			wchar_t temp2[255];
			(void)_wfopen_s(&fp, RATE_FILE_NAME, L"rb");
			(void)_wfopen_s(&outfp, L"RateDetail.txt", L"w");
			if (fp != NULL && outfp != NULL) {
				for (int i = 0; i < RATE_NUM; i++) {
					fread(&temp, sizeof(play_rate_t), 1, fp);
					fwprintf(outfp, L"%2d: %5.2f: %s\n", i + 1, temp.num, temp.name);
				}
			}
			fclose(fp);
			fclose(outfp);
#endif
			break;
#endif
		}

		ClearDrawScreen(); /* �`��G���A�X�^�[�g */

		RecRescaleDrawGraph(0, 0, pic.back, TRUE); /* �w�i */
		RecRescaleDrawGraph(40, 45 + command * 50, pic.cursor, TRUE); /* �J�[�\�� */

		/* ���ڕ\�� */
		for (int i = 0; i < 7; i++) {
			TCHAR buf[32];

			optionstr[i].action(buf, *optionstr[i].val_p, optiondata.lang);

			if (optiondata.lang == LANG_JP) {
				DrawFormatString(title_txposx, title_txposy + title_txgapy * i, COLOR_WHITE, L"%s: %s", optionstr[i].title.jp, buf);
			}
			else if (optiondata.lang == LANG_EN) {
				DrawFormatString(title_txposx, title_txposy + title_txgapy * i, COLOR_WHITE, L"%s: %s", optionstr[i].title.en, buf);
			}
		}

		/* ���� */
		if (optiondata.lang == LANG_JP) {
			DrawFormatString(det_txposx, det_txposy, COLOR_WHITE, L"%s", optionstr[command].detail.jp);
		}
		else if (optiondata.lang == LANG_EN) {
			DrawFormatString(det_txposx, det_txposy, COLOR_WHITE, L"%s", optionstr[command].detail.en);
		}

		DrawHelpBar(HELP_MAT_OPTION, optiondata.lang); /* ������@ */

		ScreenFlip(); /* �`��G���A�����܂� */

		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }

		if (exitFg == 1) { break; }
	}

	return SCENE_MENU;
}
