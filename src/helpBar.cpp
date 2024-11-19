
#include "DxLib.h"
#include "dxcur.h"
#include "helpBar.h"
#include "system.h"
#include "option.h"
#include "RecWindowRescale.h"

#define REC_HELP_DRAW_X 5
#define REC_HELP_DRAW_Y 460

static DxPic_t HelpBar = 0;

void InitHelpBar() {
	HelpBar = LoadGraph(L"picture/help.png");
	return;
}

void DrawHelpBar(help_bar_mat_t mat, char lan) {
	int page = 0;
	RecRescaleDrawGraph(0, 0, HelpBar, TRUE);
	switch (mat) {
	case HELP_MAT_MUSIC_SELECT:
		page = (int)(GetNowCount() / 10000);
		switch (lan) {
		case 0: /* Japanese */
			switch (page % 3) {
			case 0:
				RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"Enterキー: 決定, BackSpaceキー: 戻る", COLOR_WHITE);
				break;
			case 1:
				RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"上下キー: 曲選択, 左右キー: 難易度選択", COLOR_WHITE);
				break;
			case 2:
				RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"Zキー: 楽曲を並び替える, Pキー+Enterキー: オートプレイ", COLOR_WHITE);
				break;
			}
			break;
		case 1: /* English */
			switch (page % 3) {
			case 0:
				RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"Enter key: start, BackSpace key: back to menu", COLOR_WHITE);
				break;
			case 1:
				RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"↑↓key: music select, ←→key: dif select", COLOR_WHITE);
				break;
			case 2:
				RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"Z key: Sort Songs, P and Enter key: Auto Play", COLOR_WHITE);
				break;
			}
			break;
		}
		break;
	case HELP_MAT_OPTION:
		switch (lan) {
		case 0: /* Japanese */
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"上下左右キー:選択   BackSpaceキー:保存して戻る", COLOR_WHITE);
			break;
		case 1: /* English */
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"←↓↑→ key:choose   BackSpace key:save the option and return", COLOR_WHITE);
			break;
		}
		break;
	}
	return;
}

rec_helpbar_c::rec_helpbar_c() {
	this->pic = LoadGraph(L"picture/help.png");
}

rec_helpbar_c::~rec_helpbar_c() {
	DeleteGraph(this->pic);
}

void rec_helpbar_c::DrawHelp(help_bar_mat_t mat) {
	int page = 0;
	RecRescaleDrawGraph(0, 0, this->pic, TRUE);

	switch (mat) {
	case HELP_MAT_MENU:
		switch (optiondata.lang) {
		case 0: /* Japanese */
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"左右キー:選択   Enterキー:決定", COLOR_WHITE);
			break;
		case 1: /* English */
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"←→key:choose   Enter key:enter", COLOR_WHITE);
			break;
		}
		break;

	case HELP_MAT_MUSIC_SELECT:
		page = (int)(GetNowCount() / 10000);
		switch (optiondata.lang) {
		case 0: /* Japanese */
			switch (page % 3) {
			case 0:
				RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"Enterキー: 決定, BackSpaceキー: 戻る", COLOR_WHITE);
				break;
			case 1:
				RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"上下キー: 曲選択, 左右キー: 難易度選択", COLOR_WHITE);
				break;
			case 2:
				RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"Zキー: 楽曲を並び替える, Pキー+Enterキー: オートプレイ", COLOR_WHITE);
				break;
			}
			break;
		case 1: /* English */
			switch (page % 3) {
			case 0:
				RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"Enter key: start, BackSpace key: back to menu", COLOR_WHITE);
				break;
			case 1:
				RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"↑↓key: music select, ←→key: dif select", COLOR_WHITE);
				break;
			case 2:
				RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"Z key: Sort Songs, P and Enter key: Auto Play", COLOR_WHITE);
				break;
			}
			break;
		}
		break;

	case HELP_MAT_COLLECTION:
		switch (optiondata.lang) {
		case 0: /* Japanese */
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"左右キー:選択   Enterキー:決定   BackSpaceキー:戻る", COLOR_WHITE);
			break;
		case 1: /* English */
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"←→key:select   Enter key:enter   BackSpace key:back to menu", COLOR_WHITE);
			break;
		}
		break;

	case HELP_MAT_COLLECTION_ITEM:
		switch (optiondata.lang) {
		case 0: /* Japanese */
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"上下左右キー:選択   BackSpaceキー:戻る", COLOR_WHITE);
			break;
		case 1: /* English */
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"←↓↑→key:select   BackSpace key:back", COLOR_WHITE);
			break;
		}
		break;

	case HELP_MAT_COLLECTION_STORY:
		switch (optiondata.lang) {
		case 0: /* Japanese */
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"上下左右キー:選択   Enterキー:決定   BackSpaceキー:戻る", COLOR_WHITE);
			break;
		case 1: /* English */
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"←↓↑→key:select   Enter key:enter   BackSpace key:back", COLOR_WHITE);
			break;
		}
		break;

	case HELP_MAT_COLLECTION_STORY_READ:
		switch (optiondata.lang) {
		case 0: /* Japanese */
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"上下キー:ページ選択   BackSpaceキー:戻る", COLOR_WHITE);
			break;
		case 1: /* English */
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"↑↓key:select   BackSpace key:back", COLOR_WHITE);
			break;
		}
		break;

	case HELP_MAT_OPTION:
		switch (optiondata.lang) {
		case 0: /* Japanese */
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"上下左右キー:選択   BackSpaceキー:保存して戻る", COLOR_WHITE);
			break;
		case 1: /* English */
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, L"←↓↑→ key:choose   BackSpace key:save the option and return", COLOR_WHITE);
			break;
		}
		break;
	}
	return;
}
