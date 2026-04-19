
/* base include */
#include <stdio.h>
#include <DxLib.h>

/* curbine code include */
#include <dxcur.h>
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <RecSystem.h>
#include <helpBar.h>
#include <RecWindowRescale.h>

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
	rec_system_langstr_c title;
	rec_system_langstr_c detail;
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
	rec_system_langstr_c str[3] = {
		rec_system_langstr_c(_T("ピッカー"),       _T("Picker")),
		rec_system_langstr_c(_T("マップゲーター"), _T("MapGator")),
		rec_system_langstr_c(_T("テイラー"),       _T("Taylor"))
	};
	strcopy_2(str[pal].get_str().c_str(), ret, 32);
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
	rec_system_langstr_c str[4] = {
		rec_system_langstr_c(_T("真っ黒"), _T("Black") ),
		rec_system_langstr_c(_T("暗い"),   _T("Dark")  ),
		rec_system_langstr_c(_T("中間"),   _T("Middle")),
		rec_system_langstr_c(_T("明るい"), _T("Bright"))
	};
	strcopy_2(str[pal].get_str().c_str(), ret, 32);
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
	rec_system_langstr_c str[6] = {
		rec_system_langstr_c(_T("中央の上"),   _T("top centre")),
		rec_system_langstr_c(_T("左上"),       _T("top left")  ),
		rec_system_langstr_c(_T("右上"),       _T("top right") ),
		rec_system_langstr_c(_T("中央"),       _T("centre")    ),
		rec_system_langstr_c(_T("キャラの上"), _T("near chara")),
		rec_system_langstr_c(_T("表示しない"), _T("none")      )
	};
	strcopy_2(str[pal].get_str().c_str(), ret, 32);
	return;
}

static void RecOptionBGMVolume(TCHAR *ret, int pal) {
	strnums(ret, pal, 32);
	rec_bgm_system_g.SetVolume(255);
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
		rec_system_langstr_c(_T("キャラクター"), _T("Character")),
		rec_system_langstr_c(_T("使用するキャラクターを変えます。"), _T("Choose the character you use.")),
		0, 2, TRUE, &optiondata.chara
	}, {
		RecOptionOffset,
		rec_system_langstr_c(_T("オフセット"), _T("Offset")),
		rec_system_langstr_c(
			_T("音符が流れてくるタイミングを変えます。\nFASTが多いなら減らして、LATEが多いなら増やしてください。"),
			_T("Change the timing of note. Increase to late, Decrease to early.")
		),
		-2000, 2000, FALSE, &optiondata.offset
	}, {
		RecOptionSE,
		rec_system_langstr_c(_T("効果音"), _T("SE")),
		rec_system_langstr_c(
			_T("音符をたたいた時の効果音を鳴らすかどうかを変えます"),
			_T("Choose whether to sound of hitting notes.")
		),
		0, 1, TRUE, &optiondata.SEenable
	}, {
		RecOptionBackBright,
		rec_system_langstr_c(_T("背景の明るさ"), _T("Background Brightness")),
		rec_system_langstr_c(_T("背景の明るさを変えます。"), _T("Choose brightness of background.")),
		0, 3, TRUE, &optiondata.backbright
	}, {
		RecOptionLang,
		rec_system_langstr_c(_T("言語 Language"), _T("言語 Language")),
		rec_system_langstr_c(
			_T("ゲームで使う言語を変えます。\nChoose the lunguage in this game."),
			_T("ゲームで使う言語を変えます。\nChoose the lunguage in this game.")
		),
		0, 1, TRUE, &optiondata.lang
	}, {
		RecOptionButtonDet,
		rec_system_langstr_c(_T("ボタン表示"), _T("Button Guide")),
		rec_system_langstr_c(
			_T("ボタンの押し状況をプレイ画面に表示します。"),
			_T("Choose whether to display the key states on playing mode.")
		),
		0, 1, TRUE, &optiondata.keydetail
	}, {
		RecOptionComboPos,
		rec_system_langstr_c(_T("判定表示位置"), _T("Judge Position")),
		rec_system_langstr_c(_T("判定の表示場所を決めます。"), _T("Choose judge position.")),
		0, 5, TRUE, &optiondata.combopos
	}, {
		RecOptionBGMVolume,
		rec_system_langstr_c(_T("BGMの音量"), _T("BGM Volume")),
		rec_system_langstr_c(_T("BGMの音量を決めます。"), _T("Choose BGM volume.")),
		0, 10, FALSE, &optiondata.BGMvolume
	}, {
		RecOptionSEVolume,
		rec_system_langstr_c(_T("SEの音量"), _T("SE Volume")),
		rec_system_langstr_c(_T("SEの音量を決めます。"), _T("Choose SE volume.")),
		0, 10, FALSE, &optiondata.SEvolume
	}, {
		RecOptionLaneGuideThick,
		rec_system_langstr_c(_T("レーンガイドの太さ"), _T("Lane Guide Line Thickness")),
		rec_system_langstr_c(_T("レーンガイドの太さを決めます。"), _T("Choose lane guide line thickness.")),
		0, 30, FALSE, &optiondata.lineThick
	}, {
		RecOptionLaneMeasureThick,
		rec_system_langstr_c(_T("小節線の太さ"), _T("Measure Line Thickness")),
		rec_system_langstr_c(_T("小節線の太さを決めます。"), _T("Choose measure line thickness.")),
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
				optionstr[i].title.get_str().c_str(), buf);
		}

		/* 説明 */
		DrawFormatString(det_txposx, det_txposy, COLOR_WHITE, L"%s",
			optionstr[command].detail.get_str().c_str());

		help.DrawHelp(HELP_MAT_OPTION); /* 操作方法 */

		ScreenFlip(); /* 描画エリアここまで */

		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }

		if (exitFg) { break; }
	}

	return SCENE_MENU;
}
