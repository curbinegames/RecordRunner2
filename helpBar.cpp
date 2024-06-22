
#include "DxLib.h"
#include "helpBar.h"
#include "system.h"
#include "RecWindowRescale.h"

static int HelpBar = 0;

void InitHelpBar() {
	HelpBar = LoadGraph(L"picture/help.png");
	return;
}

void DrawHelpBar(help_bar_mat_t mat, char lan) {
	RecRescaleDrawGraph(0, 0, HelpBar, TRUE);
	switch (mat) {
	case HELP_MAT_MUSIC_SELECT:
		switch (lan) {
		case 0: /* Japanese */
			switch (GetNowCount() / 10000 % 3) {
			case 0:
				RecRescaleDrawString(5, 460, L"Enterキー: 決定, BackSpaceキー: 戻る", COLOR_WHITE);
				break;
			case 1:
				RecRescaleDrawString(5, 460, L"上下キー: 曲選択, 左右キー: 難易度選択", COLOR_WHITE);
				break;
			case 2:
				RecRescaleDrawString(5, 460, L"Zキー: 楽曲を並び替える, Pキー+Enterキー: オートプレイ", COLOR_WHITE);
				break;
			}
			break;
		case 1: /* English */
			switch (GetNowCount() / 10000 % 3) {
			case 0:
				RecRescaleDrawString(5, 460, L"Enter key: start, BackSpace key: back to menu", COLOR_WHITE);
				break;
			case 1:
				RecRescaleDrawString(5, 460, L"↑↓key: music select, ←→key: dif select", COLOR_WHITE);
				break;
			case 2:
				RecRescaleDrawString(5, 460, L"Z key: Sort Songs, P and Enter key: Auto Play", COLOR_WHITE);
				break;
			}
			break;
		}
		break;
	}
	return;
}
