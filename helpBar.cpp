
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
	int page = 0;
	RecRescaleDrawGraph(0, 0, HelpBar, TRUE);
	switch (mat) {
	case HELP_MAT_MUSIC_SELECT:
		page = (int)(GetNowCount() / 10000);
		switch (lan) {
		case 0: /* Japanese */
			switch (page % 3) {
			case 0:
				RecRescaleDrawString(5, 460, L"Enter�L�[: ����, BackSpace�L�[: �߂�", COLOR_WHITE);
				break;
			case 1:
				RecRescaleDrawString(5, 460, L"�㉺�L�[: �ȑI��, ���E�L�[: ��Փx�I��", COLOR_WHITE);
				break;
			case 2:
				RecRescaleDrawString(5, 460, L"Z�L�[: �y�Ȃ���ёւ���, P�L�[+Enter�L�[: �I�[�g�v���C", COLOR_WHITE);
				break;
			}
			break;
		case 1: /* English */
			switch (page % 3) {
			case 0:
				RecRescaleDrawString(5, 460, L"Enter key: start, BackSpace key: back to menu", COLOR_WHITE);
				break;
			case 1:
				RecRescaleDrawString(5, 460, L"����key: music select, ����key: dif select", COLOR_WHITE);
				break;
			case 2:
				RecRescaleDrawString(5, 460, L"Z key: Sort Songs, P and Enter key: Auto Play", COLOR_WHITE);
				break;
			}
			break;
		}
		break;
	case HELP_MAT_OPTION:
		switch (lan) {
		case 0: /* Japanese */
			RecRescaleDrawString(5, 460, L"�㉺���E�L�[:�I��   BackSpace�L�[:�ۑ����Ė߂�", COLOR_WHITE);
			break;
		case 1: /* English */
			RecRescaleDrawString(5, 460, L"�������� key:choose   BackSpace key:save the option and return", COLOR_WHITE);
			break;
		}
		break;
	}
	return;
}
