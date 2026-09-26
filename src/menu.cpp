
#include <array>
#include <cmath>
#include <RecSystem.h>

#define MENU_DRAW_LEFT 50
#define MENU_DRAW_HEIGHT 60

struct rec_menu_item_st {
	int posUp = 0;
	int posRight = 0;
	now_scene_t next = SCENE_EXIT;
};

class rec_menu_pic_item_c {
private:
	int base_posx = 0;
	int base_posy = 0;
	dxcur_pic_c pic;
	int phase = GetRand(1000);

public:
	rec_menu_pic_item_c(void) = delete;
	rec_menu_pic_item_c(int x, int y, const tstring &path) :
		base_posx(x), base_posy(y), pic(path) {}

	void draw(void) const {
		DxTime_t Ntime = GetNowCount();
		int    drawX = std::sin(Ntime /  1800.0 + this->phase) * 20 + this->base_posx;
		int    drawY = std::sin(Ntime /  2500.0 + this->phase) * 10 + this->base_posy;
		double drawR = std::sin(Ntime /  2200.0 + this->phase) *  5;
		DrawDeformationPic(drawX, drawY, 1, 1, drawR, this->pic.handle());
	}
};

int RecMenuGetMouseAct(
	int &cmd, const std::array<rec_menu_item_st, 6> &menuitem,
	dxcur_snd_c &s_sel, const rec_cutin_c &cutin
) {
	if (cutin.IsClosing() != 0) { return -1; }

	static int b_mouseX = 0;
	static int b_mouseY = 0;

	bool selecting = false;
	int mouseBtn = 0;
	int mouseX = 0;
	int mouseY = 0;
	int mouseAct = 0;

	GetMousePoint(&mouseX, &mouseY);
	if (mouseX != b_mouseX || mouseY != b_mouseY) {
		b_mouseX = mouseX;
		b_mouseY = mouseY;
		for (int i = 0; i < menuitem.size(); i++) {
			if (IS_BETWEEN(MENU_DRAW_LEFT, mouseX, menuitem[i].posRight) &&
				IS_BETWEEN(menuitem[i].posUp, mouseY, menuitem[i].posUp + MENU_DRAW_HEIGHT))
			{
				if (i != cmd) {
					cmd = i;
					s_sel.PlaySound();
				}
				selecting = true;
				break;
			}
		}
	}

	while (GetMouseInputLog2(&mouseBtn, &mouseX, &mouseY, &mouseAct, true) == 0) {}
	if (selecting && mouseAct == MOUSE_INPUT_LEFT) {
		return KEY_INPUT_RETURN;
	}
	return -1;
}

int RecMenuGetKeyAct(dxcur_key_c &key, const rec_cutin_c &cutin) {
	if (cutin.IsClosing() != 0) { return -1; }
	key.update();
	return key.GetKeyPulseOnce();
}

int RecMenuGetAllAct(
	int &cmd, const std::array<rec_menu_item_st, 6> &menuitem,
	dxcur_key_c &key, dxcur_snd_c &s_sel, const rec_cutin_c &cutin
) {
	int act = -1;
	act = RecMenuGetMouseAct(cmd, menuitem, s_sel, cutin);
	if (act == -1) { act = RecMenuGetKeyAct(key, cutin); }
	return act;
}

now_scene_t RecMenuBase(void) {
	int cmd = 0;
	now_scene_t next = SCENE_EXIT;
	dxcur_key_c key;
	rec_helpbar_c help;
	rec_cutin_c cutin;

	std::array<rec_menu_item_st, 6> menuitem = {
		rec_menu_item_st{230, 290, SCENE_SERECT},
		rec_menu_item_st{292, 450, SCENE_MENU},
		rec_menu_item_st{410, 350, SCENE_COLLECTION},
		rec_menu_item_st{475, 195, SCENE_COLLECTION},
		rec_menu_item_st{538, 230, SCENE_OPTION},
		rec_menu_item_st{598, 175, SCENE_EXIT}
	};

	int draw_win_posU = menuitem[0].posUp;
	int draw_win_posR = menuitem[0].posRight;

	dxcur_pic_c backpic(_T("picture/menu/タイトル原案.png"));
	rec_menu_pic_item_c charpic(920, 330, _T("picture/menu/freerun_picker.png"));
	std::vector<rec_menu_pic_item_c> subpic;
	subpic.push_back(rec_menu_pic_item_c( 650, 250, _T("picture/menu/freerun_hit.png")));
	subpic.push_back(rec_menu_pic_item_c( 600, 550, _T("picture/menu/freerun_catch.png")));
	subpic.push_back(rec_menu_pic_item_c(1200, 300, _T("picture/menu/freerun_arrow.png")));
	dxcur_window_pic_c curpic(_T("picture/cursorwindow.png"));
	dxcur_snd_c s_sel(_T("sound/select.wav"));

	while (GetMouseInputLog2(NULL, NULL, NULL, NULL, true) == 0) {}
	cutin.SetIo(CUT_FRAG_OUT);

	while (true) {
		if (GetWindowUserCloseFlag(TRUE)) {
			next = SCENE_EXIT;
			break;
		}
		if (cutin.IsEndAnim()) { break; }

		switch (RecMenuGetAllAct(cmd, menuitem, key, s_sel, cutin)) {
		case KEY_INPUT_RETURN:
			next = menuitem[cmd].next;
			cutin.SetTipNo();
			cutin.SetCutTipFg(CUTIN_TIPS_ON);
			cutin.SetIo(CUT_FRAG_IN);
			break;
		case KEY_INPUT_UP:
			cmd = LOOP_SUB(cmd, 6);
			s_sel.PlaySound();
			break;
		case KEY_INPUT_DOWN:
			cmd = LOOP_ADD(cmd, 6);
			s_sel.PlaySound();
			break;
		}

		draw_win_posU = (draw_win_posU + menuitem[cmd].posUp) / 2;
		draw_win_posR = (draw_win_posR + menuitem[cmd].posRight) / 2;

		ClearDrawScreen(); /* 描画エリアここから */
		DrawGraph(0, 0, backpic.handle(), TRUE);
		charpic.draw();
		for (size_t i = 0; i < subpic.size(); i++) {
			subpic[i].draw();
		}
		curpic.draw(MENU_DRAW_LEFT, draw_win_posU,
			draw_win_posR, draw_win_posU + MENU_DRAW_HEIGHT);
		help.DrawHelp(rec_helpbar_type_ec::MENU);
		cutin.DrawCut();
		ScreenFlip(); /* 描画エリアここまで */
	}

	return next;
}
