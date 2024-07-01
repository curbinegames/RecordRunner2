
#include "DxLib.h"
#include "dxcur.h"
#include "sancur.h"
#include "RecWindowRescale.h"
#include "system.h"
#include "helpBar.h"
#include "option.h"

#define COLOR_WHITE 0xffffffff

static const int det_txposx = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 20);
static const int det_txposy = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 410);

typedef struct grobal_text_mini_s {
	wchar_t jp[8];
	wchar_t en[20];
} grobal_text_mini_t;

typedef void (*rec_opt_action_t)(int pos, int pal, int lang);

typedef struct rec_opt_text_s {
	const rec_opt_action_t action;
	const struct {
		wchar_t jp[64];
		wchar_t en[64];
	} detail;
	const int min;
	const int max;
	const int loop;
	int *val_p;
} rec_opt_text_t;

int RecOpenOptionFile(int *data) {
	FILE *fp = NULL;
	_wfopen_s(&fp, L"save/system.dat", L"rb");
	if (fp == NULL) {
		return 1;
	}
	fread(&data, sizeof(int), 7, fp);
	fclose(fp);
	return 0;
}

int RecOpenOptionFile2(rec_option_t *data) {
	FILE *fp = NULL;
	_wfopen_s(&fp, L"save/system2.dat", L"rb");
	if (fp == NULL) {
		return 1;
	}
	fread(&data, sizeof(rec_option_t), 1, fp);
	fclose(fp);
	return 0;
}

int RecWriteOptineFile(int *data) {
	FILE *fp = NULL;
	_wfopen_s(&fp, L"save/system.dat", L"wb");
	if (fp == NULL) {
		return 1;
	}
	fwrite(&data, sizeof(int), 7, fp);
	fclose(fp);
	return 0;
}

int RecWriteOptineFile2(rec_option_t *data) {
	FILE *fp = NULL;
	_wfopen_s(&fp, L"save/system2.dat", L"wb");
	if (fp == NULL) {
		return 1;
	}
	fwrite(&data, sizeof(int), 7, fp);
	fclose(fp);
	return 0;
}

static void RecOptionChar(int pos, int pal, int lang) {
	const wchar_t jp[3][20] = {
		L"ピッカー", L"マップゲーター", L"テイラー"
	};

	const wchar_t en[3][10] = {
		L"Picker", L"MapGator", L"Taylor"
	};

	int drawX = 0;
	int drawY = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 100);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 50 * pos + 50);

	if (lang == 0) {
		DrawFormatString(drawX, drawY, COLOR_WHITE, L"キャラクター: %s", jp[pal]);
	}
	else if (lang == 1) {
		DrawFormatString(drawX, drawY, COLOR_WHITE, L"Character: %s", en[pal]);
	}
	return;
}

static void RecOptionOffset(int pos, int pal, int lang) {
	int drawX = 0;
	int drawY = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 100);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 50 * pos + 50);

	if (lang == 0) {
		DrawFormatString(drawX, drawY, COLOR_WHITE, L"オフセット: %dms", pal * 5);
	}
	else if (lang == 1) {
		DrawFormatString(drawX, drawY, COLOR_WHITE, L"Offset: %dms", pal * 5);
	}
	return;
}

static void RecOptionSE(int pos, int pal, int lang) {
	int drawX = 0;
	int drawY = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 100);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 50 * pos + 50);

	if (pal == 0 && lang == 0) {
		DrawString(drawX, drawY, L"効果音: あり", COLOR_WHITE);
	}
	else if (pal == 0 && lang == 1) {
		DrawString(drawX, drawY, L"SE: ON", COLOR_WHITE);
	}
	else if (pal == 1 && lang == 0) {
		DrawString(drawX, drawY, L"効果音: なし", COLOR_WHITE);
	}
	else if (pal == 1 && lang == 1) {
		DrawString(drawX, drawY, L"SE: OFF", COLOR_WHITE);
	}
	return;
}

static void RecOptionBackBright(int pos, int pal, int lang) {
	wchar_t jp[4][10] = {
		L"真っ黒", L"暗い", L"中間", L"明るい"
	};

	wchar_t en[4][10] = {
		L"Black", L"Dark", L"Middle", L"Bright"
	};

	int drawX = 0;
	int drawY = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 100);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 50 * pos + 50);

	if (lang == 0) {
		DrawFormatString(drawX, drawY, COLOR_WHITE, L"背景の明るさ: %s", jp[pal]);
	}
	else if (lang == 1) {
		DrawFormatString(drawX, drawY, COLOR_WHITE, L"BackGroundBrightness: %s", en[pal]);
	}
	return;
}

static void RecOptionLang(int pos, int pal, int lang) {
	int drawX = 0;
	int drawY = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 100);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 50 * pos + 50);

	if (pal == 0) {
		DrawString(drawX, drawY, L"言語 Language: 日本語", COLOR_WHITE);
	}
	else if (pal == 1) {
		DrawString(drawX, drawY, L"言語 Language: English", COLOR_WHITE);
	}
	return;
}

static void RecOptionButtonDet(int pos, int pal, int lang) {
	int drawX = 0;
	int drawY = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 100);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 50 * pos + 50);

	if (pal == 0 && lang == 0) {
		DrawString(drawX, drawY, L"ボタン表示: off", COLOR_WHITE);
	}
	else if (pal == 0 && lang == 1) {
		DrawString(drawX, drawY, L"ButtonGuide: off", COLOR_WHITE);
	}
	else if (pal == 1 && lang == 0) {
		DrawString(drawX, drawY, L"ボタン表示: on", COLOR_WHITE);
	}
	else if (pal == 1 && lang == 1) {
		DrawString(drawX, drawY, L"ButtonGuide: on", COLOR_WHITE);
	}

	return;
}

static void RecOptionComboPos(int pos, int pal, int lang) {
	wchar_t jp[6][10] = {
		L"中央の上", L"左上", L"右上", L"中央", L"キャラの上", L"表示しない"
	};

	wchar_t en[6][12] = {
		L"top centre", L"top left", L"top right", L"centre", L"near chara", L"nope"
	};

	int drawX = 0;
	int drawY = 0;

	drawX = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 100);
	drawY = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 50 * pos + 50);

	if (lang == 0) {
		DrawFormatString(drawX, drawY, COLOR_WHITE, L"判定表示位置: %s", jp[pal]);
	}
	else if (lang == 1) {
		DrawFormatString(drawX, drawY, COLOR_WHITE, L"Judge Position: %s", en[pal]);
	}
	return;
}

static rec_opt_text_t optionstr[] {
	{
		RecOptionChar,
		L"使用するキャラクターを変えます。",
		L"Choose the character you use.",
		0, 2, 1
	}, {
		RecOptionOffset,
		L"音符が流れてくるタイミングを変えます。\n増やすと遅れて、減らすと早めに流れます。",
		L"Change the timing of note. Increase to late, Decrease to early.",
		-2000, 2000, 0
	}, {
		RecOptionSE,
		L"音符をたたいた時の効果音を鳴らすかどうかを変えます",
		L"Choose whether to sound of hitting notes.",
		0, 1, 1
	}, {
		RecOptionBackBright,
		L"背景の明るさを変えます。",
		L"Choose brightness of background.",
		0, 3, 1
	}, {
		RecOptionLang,
		L"ゲームで使う言語を変えます。\nChoose the lunguage in this game.",
		L"ゲームで使う言語を変えます。\nChoose the lunguage in this game.",
		0, 1, 1
	}, {
		RecOptionButtonDet,
		L"ボタンの押し状況をプレイ画面に表示します。",
		L"Choose whether to display the key states on playing mode.",
		0, 1, 1
	}, {
		RecOptionComboPos,
		L"判定の表示場所を決めます。",
		L"Choose judge position.",
		0, 5, 1
	}
};

static const int optionstr_count = sizeof(optionstr) / sizeof(rec_opt_text_t);

now_scene_t option(void) {
	int command = 0;
	rec_option_t optiondata;
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

	pic.back   = LoadGraph(L"picture/OPTION back.png");
	pic.cursor = LoadGraph(L"picture/OC.png");
	sel        = LoadSoundMem(L"sound/select.wav");

	AvoidKeyRush();

	//処理
	while (1) {		
		InputAllKeyHold();
		switch (GetKeyPushOnce()) {
		case KEY_INPUT_LEFT:
			PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			(*optionstr[command].val_p)--;
			if (*optionstr[command].val_p < optionstr[command].min) {
				if (optionstr[command].loop == 0) {
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
				if (optionstr[command].loop == 0) {
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
			exitFg = 1;
			break;
#if 0 /* デバッグ用コード */
		case KEY_INPUT_Q:
#if 0 /* 全譜面定数を出力 */
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
						G[0] = _wfopen_s(&qfp, file, L"rb");//rrsデータを読み込む
						if (G[0] == 0) {
							fread(&allnum, sizeof(playnum_box), 1, qfp);//各データの個数

							fseek(qfp, 255 * 4 + sizeof(int) + sizeof(double), SEEK_CUR);

							fread(&songN[dif], 255, 1, qfp);//曲名

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
#elif 0 /* プレイヤーのレートの詳細を出力 */
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

		ClearDrawScreen(); /* 描画エリアスタート */

		RecRescaleDrawGraph(0, 0, pic.back, TRUE); /* 背景 */
		RecRescaleDrawGraph(40, 45 + command * 50, pic.cursor, TRUE); /* カーソル */

		/* 項目表示 */
		for (int i = 0; i < 7; i++) {
			optionstr[i].action(i, *optionstr[i].val_p, optiondata.lang);
		}

		/* 説明 */
		if (optiondata.lang == 0) {
			DrawFormatString(det_txposx, det_txposy, COLOR_WHITE, L"%s", optionstr[command].detail.jp);
		}
		else if (optiondata.lang == 1) {
			DrawFormatString(det_txposx, det_txposy, COLOR_WHITE, L"%s", optionstr[command].detail.en);
		}

		DrawHelpBar(HELP_MAT_OPTION, optiondata.lang); /* 操作方法 */

		ScreenFlip(); /* 描画エリアここまで */

		if (GetWindowUserCloseFlag(TRUE)) {
			return SCENE_EXIT;
		}

		if (exitFg == 1) {
			break;
		}
	}
	return SCENE_MENU;
}