
/* base include */
#include <stdio.h>
#include <DxLib.h>

/* curbine code include */
#include <dxcur.h>
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <helpBar.h>
#include <RecSystem.h>
#include <RecWindowRescale.h>
#include <RecSystem.h>

/* own include */
#include <option.h>

#if REC_DEBUG == 1
#include <RecScoreFile.h>
#include <playbox.h>
#include <RecordLoad2.h>
#include <recp_cal_ddif_2.h>
#endif

/**
 * TODO: 追加したいオプション
 * 　文字の大きさ
 * 　PLAYINGバーカスタマイズ
 *   ガイドレーンの色
 */

typedef void (*rec_opt_action_t)(TCHAR *ret, int pal);

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
	int *const val_p;
} rec_opt_text_t;

rec_option_t optiondata;

DxSnd_t s_sel = DXLIB_SND_NULL;

static const int det_txposx = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 20);
static const int det_txposy = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 410);

#if 1 /* option_file */

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

void RecOpenOptionFileSystem() {
	if (RecOpenOptionFile2(&optiondata) != 0) {
		int	data[7] = { 0,0,0,2,0,0,0 };
		if (RecOpenOptionFile(data) == 0) {
			optiondata.chara = data[0];
			optiondata.offset = data[1];
			optiondata.SEenable = data[2];
			optiondata.backbright = data[3];
			optiondata.lang = data[4];
			optiondata.keydetail = data[5];
			optiondata.combopos = data[6];
		}
		else {
			optiondata.chara = 0;
			optiondata.offset = 0;
			optiondata.SEenable = 0;
			optiondata.backbright = 2;
			optiondata.lang = 0;
			optiondata.keydetail = 0;
			optiondata.combopos = 0;
		}
	}
}

#endif /* option_file */

#if 1 /* option_data */

static void RecOptionChar(TCHAR *ret, int pal) {
	const TCHAR jp[3][20] = {
		L"ピッカー", L"マップゲーター", L"テイラー"
	};
	const TCHAR en[3][10] = {
		L"Picker", L"MapGator", L"Taylor"
	};
	strcopy_2(REC_STR_LANG(jp[pal], en[pal]), ret, 32);
	return;
}

static void RecOptionOffset(TCHAR *ret, int pal) {
	strnums(ret, pal * 5, 32);
	return;
}

static void RecOptionSE(TCHAR *ret, int pal) {
	if (pal == 0) { strcopy_2(L"on", ret, 32); }
	else if (pal == 1) { strcopy_2(L"off", ret, 32); }
	return;
}

static void RecOptionBackBright(TCHAR *ret, int pal) {
	wchar_t jp[4][10] = {
		L"真っ黒", L"暗い", L"中間", L"明るい"
	};
	wchar_t en[4][10] = {
		L"Black", L"Dark", L"Middle", L"Bright"
	};
	strcopy_2(REC_STR_LANG(jp[pal], en[pal]), ret, 32);
	return;
}

static void RecOptionLang(TCHAR *ret, int pal) {
	strcopy_2(REC_STR_LANG(_T("日本語"), _T("English")), ret, 32);
	return;
}

static void RecOptionButtonDet(TCHAR *ret, int pal) {
	if (pal == 0) { strcopy_2(L"off", ret, 32); }
	else if (pal == 1) { strcopy_2(L"on", ret, 32); }
	return;
}

static void RecOptionComboPos(TCHAR *ret, int pal) {
	wchar_t jp[6][10] = {
		L"中央の上", L"左上", L"右上", L"中央", L"キャラの上", L"表示しない"
	};
	wchar_t en[6][12] = {
		L"top centre", L"top left", L"top right", L"centre", L"near chara", L"nope"
	};
	strcopy_2(REC_STR_LANG(jp[pal], en[pal]), ret, 32);
	return;
}

static void RecOptionBGMVolume(TCHAR *ret, int pal) {
	strnums(ret, pal, 32);
	RecSysBgmChangeVolume(255);
	return;
}

static void RecOptionSEVolume(TCHAR *ret, int pal) {
	strnums(ret, pal, 32);
	ChangeVolumeSoundMem(optiondata.SEvolume * 255 / 10, s_sel);
	return;
}

static void RecOptionLaneGuideThick(TCHAR *ret, int pal) {
	strnums(ret, pal, 32);
	return;
}

static void RecOptionLaneMeasureThick(TCHAR *ret, int pal) {
	strnums(ret, pal, 32);
	return;
}

static rec_opt_text_t optionstr[]{
	{
		RecOptionChar,
		L"キャラクター", L"Character",
		L"使用するキャラクターを変えます。",
		L"Choose the character you use.",
		0, 2, TRUE, &optiondata.chara
	}, {
		RecOptionOffset,
		L"オフセット", L"Offset",
		L"音符が流れてくるタイミングを変えます。\n増やすと遅れて、減らすと早めに流れます。",
		L"Change the timing of note. Increase to late, Decrease to early.",
		-2000, 2000, FALSE, &optiondata.offset
	}, {
		RecOptionSE,
		L"効果音", L"SE",
		L"音符をたたいた時の効果音を鳴らすかどうかを変えます",
		L"Choose whether to sound of hitting notes.",
		0, 1, TRUE, &optiondata.SEenable
	}, {
		RecOptionBackBright,
		L"背景の明るさ", L"Background Brightness",
		L"背景の明るさを変えます。",
		L"Choose brightness of background.",
		0, 3, TRUE, &optiondata.backbright
	}, {
		RecOptionLang,
		L"言語 Language", L"言語 Language",
		L"ゲームで使う言語を変えます。\nChoose the lunguage in this game.",
		L"ゲームで使う言語を変えます。\nChoose the lunguage in this game.",
		0, 1, TRUE, &optiondata.lang
	}, {
		RecOptionButtonDet,
		L"ボタン表示", L"Button Guide",
		L"ボタンの押し状況をプレイ画面に表示します。",
		L"Choose whether to display the key states on playing mode.",
		0, 1, TRUE, &optiondata.keydetail
	}, {
		RecOptionComboPos,
		L"判定表示位置", L"Judge Position",
		L"判定の表示場所を決めます。",
		L"Choose judge position.",
		0, 5, TRUE, &optiondata.combopos
	}, {
		RecOptionBGMVolume,
		L"BGMの音量", L"BGM Volume",
		L"BGMの音量を決めます。",
		L"Choose BGM volume.",
		0, 10, FALSE, &optiondata.BGMvolume
	}, {
		RecOptionSEVolume,
		L"SEの音量", L"SE Volume",
		L"SEの音量を決めます。",
		L"Choose SE volume.",
		0, 10, FALSE, &optiondata.SEvolume
	}, {
		RecOptionLaneGuideThick,
		L"レーンガイドの太さ", L"Lane Guide Line Thickness",
		L"レーンガイドの太さを決めます。",
		L"Choose lane guide line thickness.",
		0, 30, FALSE, &optiondata.lineThick
	}, {
		RecOptionLaneMeasureThick,
		L"小節線の太さ", L"Measure Line Thickness",
		L"小節線の太さを決めます。",
		L"Choose measure line thickness.",
		0, 30, FALSE, &optiondata.barThick
	}
};

static const int optionstr_count = ARRAY_COUNT(optionstr);

#endif /* option_data */

#if REC_DEBUG == 1 /* デバッグ用関数 */

void RecTxtWriteAllDdif() {
	rec_ddif_pal_t ddif[5];
	TCHAR path[255];
	TCHAR songN[5][255];
	FILE *fp;

	_wfopen_s(&fp, L"difout.txt", L"w");
	if (fp == NULL) { return; }

	for (uint iPack = 0; iPack < 10; iPack++) {
		fwprintf(fp, L"\t\tnotes\tarrow\tchord\tchain\ttrill\tmeldy\tactor\ttrick\tmdif\n");
		for (uint iSong = 0; iSong < 20; iSong++) {
			songN[0][0] = L'\0';
			songN[1][0] = L'\0';
			songN[2][0] = L'\0';
			songN[3][0] = L'\0';
			songN[4][0] = L'\0';
			for (uint iDif = 0; iDif < 5; iDif++) {
				RecordLoad2(iPack, iSong, iDif);
				if (RecGetMusicMapRrsPath(path, 255, iPack, iSong, (rec_dif_t)iDif) != 0) { continue; }
				cal_ddif_3(path);
				if (RecScoreReadSongName(songN[iDif], path) != 0) { continue; }
				RecScoreReadDdif(&ddif[iDif], path);
				if (songN[iDif][0] == L'\0') { continue; }

				fwprintf(fp, L"%ls", songN[iDif]);
				switch (iDif) {
				case 0:
					fwprintf(fp, L"[AUTO]\n");
					break;
				case 1:
					fwprintf(fp, L"[EASY]\n");
					break;
				case 2:
					fwprintf(fp, L"[NORMAL]\n");
					break;
				case 3:
					fwprintf(fp, L"[HARD]\n");
					break;
				case 4:
					fwprintf(fp, L"[ANOTHER]\n");
					break;
				}

				fwprintf(fp, L"\t\t%2.2f\t%2.2f\t%2.2f\t%2.2f\t%2.2f\t%2.2f\t%2.2f\t%2.2f\t%2.2f\n",
					ddif[iDif].notes / 100.0,
					ddif[iDif].arrow / 100.0,
					ddif[iDif].chord / 100.0,
					ddif[iDif].chain / 100.0,
					ddif[iDif].trill / 100.0,
					ddif[iDif].meldy / 100.0,
					ddif[iDif].actor / 100.0,
					ddif[iDif].trick / 100.0,
					ddif[iDif].mdif / 100.0);
			}
		}
	}

	fclose(fp);

	return;
}

#endif /* デバッグ用関数 */

static void RecOptionKeyCtrl(int *cmd, bool *exitFg) {
	InputAllKeyHold();
	switch (GetKeyPushOnce()) {
	case KEY_INPUT_LEFT:
		PlaySoundMem(s_sel, DX_PLAYTYPE_NORMAL);
		(*optionstr[*cmd].val_p)--;
		if (*optionstr[*cmd].val_p < optionstr[*cmd].min) {
			if (optionstr[*cmd].loop == FALSE) {
				*optionstr[*cmd].val_p = optionstr[*cmd].min;
			}
			else {
				*optionstr[*cmd].val_p = optionstr[*cmd].max;
			}
		}
		break;
	case KEY_INPUT_RIGHT:
		PlaySoundMem(s_sel, DX_PLAYTYPE_NORMAL);
		(*optionstr[*cmd].val_p)++;
		if (optionstr[*cmd].max < *optionstr[*cmd].val_p) {
			if (optionstr[*cmd].loop == FALSE) {
				*optionstr[*cmd].val_p = optionstr[*cmd].max;
			}
			else {
				*optionstr[*cmd].val_p = optionstr[*cmd].min;
			}
		}
		break;
	case KEY_INPUT_UP:
		PlaySoundMem(s_sel, DX_PLAYTYPE_NORMAL);
		*cmd = (*cmd + optionstr_count - 1) % optionstr_count;
		break;
	case KEY_INPUT_DOWN:
		PlaySoundMem(s_sel, DX_PLAYTYPE_NORMAL);
		*cmd = (*cmd + 1) % optionstr_count;
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
		*exitFg = true;
		break;
#if REC_DEBUG == 1 /* デバッグ用コード */
	case KEY_INPUT_Q:
		RecTxtWriteAllDdif();
#if 0 /* プレイヤーのレートの詳細を出力 */
		FILE *fp;
		FILE *outfp;
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
	return;
}

now_scene_t option(void) {
	int command = 0;
	struct {
		dxcur_pic_c back   = dxcur_pic_c(_T("picture/OPTION back.png"));
		dxcur_pic_c cursor = dxcur_pic_c(_T("picture/OC.png"));
	} pic;
	bool exitFg = false;
	rec_helpbar_c help;

	if (RecOpenOptionFile2(&optiondata) != 0) {
		int	data[7] = { 0,0,0,2,0,0,0 };
		RecOpenOptionFile(data);
		optiondata.chara = data[0];
		optiondata.offset = data[1];
		optiondata.SEenable = data[2];
		optiondata.backbright = data[3];
		optiondata.lang = data[4];
		optiondata.keydetail = data[5];
		optiondata.combopos = data[6];
	}

	for (int i = 0; i < optionstr_count; i++) {
		if (*optionstr[i].val_p < optionstr[i].min) {
			*optionstr[i].val_p = 0;
		}
		if (optionstr[i].max < *optionstr[i].val_p) {
			*optionstr[i].val_p = 0;
		}
	}

	s_sel = LoadSoundMem(L"sound/select.wav");

	AvoidKeyRush();

	//処理
	while (1) {
		RecOptionKeyCtrl(&command, &exitFg);

		ClearDrawScreen(); /* 描画エリアスタート */

		RecRescaleDrawGraph(0, 0, pic.back.handle(), TRUE); /* 背景 */
		RecRescaleDrawGraph(40, 45 + command * 40, pic.cursor.handle(), TRUE); /* カーソル */

		/* 項目表示 */
		for (int i = 0; i < optionstr_count; i++) {
			static const int title_txposx = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 100);
			static const int title_txposy = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 50);
			static const int title_txgapy = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 40);

			TCHAR buf[32];

			optionstr[i].action(buf, *optionstr[i].val_p);
			DrawFormatString(title_txposx, title_txposy + title_txgapy * i, COLOR_WHITE, L"%s: %s",
				REC_STR_LANG(optionstr[i].title.jp, optionstr[i].title.en), buf);
		}

		/* 説明 */
		DrawFormatString(det_txposx, det_txposy, COLOR_WHITE, L"%s",
			REC_STR_LANG(optionstr[command].detail.jp, optionstr[command].detail.en));

		help.DrawHelp(HELP_MAT_OPTION); /* 操作方法 */

		ScreenFlip(); /* 描画エリアここまで */

		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }

		if (exitFg) { break; }
	}

	return SCENE_MENU;
}
