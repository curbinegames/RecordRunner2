
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <dxcur.h>

/* rec system include */
#include <option.h>
#include <RecWindowRescale.h>
#include <RecSystem.h>

/* own include */
#include <helpBar.h>

#define REC_HELP_DRAW_X 5
#define REC_HELP_DRAW_Y 460

static DxPic_t HelpBar = 0;

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
		RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, REC_STR_LANG(_T("左右キー:選択   Enterキー:決定"), _T("←→key:choose   Enter key:enter")), COLOR_WHITE);
		break;

	case HELP_MAT_MUSIC_SELECT:
		page = (int)(GetNowCount() / 10000);
		switch (page % 3) {
		case 0:
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, REC_STR_LANG(_T("Enterキー: 決定, BackSpaceキー: 戻る"), _T("Enter key: start, BackSpace key: back to menu")), COLOR_WHITE);
			break;
		case 1:
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, REC_STR_LANG(_T("上下キー: 曲選択, 左右キー: 難易度選択"), _T("↑↓key: music select, ←→key: dif select")) , COLOR_WHITE);
			break;
		case 2:
			RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, REC_STR_LANG(_T("Zキー: 楽曲を並び替える, Pキー+Enterキー: オートプレイ"), _T("Z key: Sort Songs, P and Enter key: Auto Play")), COLOR_WHITE);
			break;
		}
		break;

	case HELP_MAT_COLLECTION:
		RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, REC_STR_LANG(_T("左右キー:選択   Enterキー:決定   BackSpaceキー:戻る"), _T("←→key:select   Enter key:enter   BackSpace key:back to menu")), COLOR_WHITE);
		break;

	case HELP_MAT_COLLECTION_ITEM:
		RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, REC_STR_LANG(_T("上下左右キー:選択   BackSpaceキー:戻る"), _T("←↓↑→key:select   BackSpace key:back")), COLOR_WHITE);
		break;

	case HELP_MAT_COLLECTION_STORY:
		RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, REC_STR_LANG(_T("上下左右キー:選択   Enterキー:決定   BackSpaceキー:戻る"), _T("←↓↑→key:select   Enter key:enter   BackSpace key:back")), COLOR_WHITE);
		break;

	case HELP_MAT_COLLECTION_STORY_READ:
		RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, REC_STR_LANG(_T("上下キー:ページ選択   BackSpaceキー:戻る"), _T("↑↓key:select   BackSpace key:back")), COLOR_WHITE);
		break;

	case HELP_MAT_OPTION:
		RecRescaleDrawString(REC_HELP_DRAW_X, REC_HELP_DRAW_Y, REC_STR_LANG(_T("上下左右キー:選択   BackSpaceキー:保存して戻る"), _T("←↓↑→ key:choose   BackSpace key:save the option and return")), COLOR_WHITE);
		break;
	}
	return;
}

void rec_helpbar_c::ReloadMat(void) {
	this->pic = LoadGraph(L"picture/help.png");
}
