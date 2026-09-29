
#include <array>
#include <cmath>
#include <RecSystem.h>

#define MENU_DRAW_LEFT 45
#define MENU_DRAW_HEIGHT 65

struct rec_menu_item_st {
	int posUp = 0;
	int posRight = 0;
	bool cutin = false;
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

class rec_menu_cursor_pic_c {
private:
	bool is_to_white = false;
	DxTime_t Stime = GetNowCount();
	dxcur_window_pic_c curpic{_T("picture/cursorwindow.png")};
	dxcur_window_pic_c curwpic{_T("picture/cursorwindowwhite.png")};

public:
	void update(void) {
		DxTime_t Ntime = GetNowCount();
		if (this->Stime + 500 < Ntime) {
			this->Stime = Ntime;
			this->is_to_white = !this->is_to_white;
		}
	}

	void draw(int left, int up, int right, int down) const {
		DxTime_t Ntime = GetNowCount() - this->Stime;
		int alpha = 255;
		if (this->is_to_white) {
			alpha = lins_scale(0, 0, 500, 255, Ntime);
		}
		else {
			alpha = lins_scale(0, 255, 500, 0, Ntime);
		}
		curpic.draw(left, up, right, down);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		curwpic.draw(left, up, right, down);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	}
};

/**
 * @brief メニュー画面のマウス操作を取得する
 * @param[out] cmd 選択中のメニュー番号
 * @param[in] menuitem メニュー項目の情報
 * @param[in] s_sel 選択音の再生用
 * @param[in] cutin カットインの状態
 * @return 選択された操作のキーコード、選択されなかった場合は-1
 * @details マウス操作がないときは更新しない
 * キーボード操作があった後にマウスのクリックを押した場合、マウスカーソルの位置にあるメニュー項目が選択される
 */
int RecMenuGetMouseAct(
	int &cmd, const std::array<rec_menu_item_st, 6> &menuitem,
	dxcur_snd_c &s_sel, const rec_cutin_c &cutin
) {
	if (cutin.IsClosing() != 0) { return -1; }

	static int b_mouseX = 0;
	static int b_mouseY = 0;

	bool selecting = false;
	bool moving = false;
	bool clicked = false;
	int mouseBtn = 0;
	int mouseX = 0;
	int mouseY = 0;
	int mouseAct = 0;

	while (GetMouseInputLog2(&mouseBtn, &mouseX, &mouseY, &mouseAct, true) == 0) {}
	if (mouseAct == MOUSE_INPUT_LEFT) { clicked = true; }

	GetMousePoint(&mouseX, &mouseY);
	if (mouseX != b_mouseX || mouseY != b_mouseY) {
		moving = true;
		b_mouseX = mouseX;
		b_mouseY = mouseY;
	}

	for (int i = 0; i < menuitem.size(); i++) {
		if (IS_BETWEEN(MENU_DRAW_LEFT, mouseX, menuitem[i].posRight) &&
			IS_BETWEEN(menuitem[i].posUp, mouseY, menuitem[i].posUp + MENU_DRAW_HEIGHT))
		{
			if (i != cmd) {
				if (clicked) {
					cmd = i;
				}
				else if (moving) {
					cmd = i;
					s_sel.PlaySound();
				}
			}
			selecting = true;
			break;
		}
	}

	if (selecting && clicked) { return KEY_INPUT_RETURN; }
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
	bool exit_flag = false;
	int cmd = 0;
	now_scene_t next = SCENE_EXIT;
	dxcur_key_c key;
	rec_helpbar_c help;
	rec_cutin_c cutin;

	std::array<rec_menu_item_st, 6> menuitem = {
		rec_menu_item_st{220, 295,  true, SCENE_SERECT},
		rec_menu_item_st{285, 450, false, SCENE_MENU},
		rec_menu_item_st{408, 340, false, SCENE_COLLECTION},
		rec_menu_item_st{472, 208, false, SCENE_COLLECTION},
		rec_menu_item_st{536, 222, false, SCENE_OPTION},
		rec_menu_item_st{598, 170, false, SCENE_EXIT}
	};

	int draw_win_posU = menuitem[0].posUp;
	int draw_win_posR = menuitem[0].posRight;

	dxcur_pic_c backpic(_T("picture/menu/freerun_back.png"));
	dxcur_pic_c basepic(_T("picture/menu/menu_base.png"));
	rec_menu_pic_item_c charpic(920, 330, _T("picture/menu/freerun_picker.png"));
	std::vector<rec_menu_pic_item_c> subpic;
	subpic.push_back(rec_menu_pic_item_c( 650, 250, _T("picture/menu/freerun_hit.png")));
	subpic.push_back(rec_menu_pic_item_c( 600, 550, _T("picture/menu/freerun_catch.png")));
	subpic.push_back(rec_menu_pic_item_c(1200, 300, _T("picture/menu/freerun_arrow.png")));

	rec_menu_cursor_pic_c curpic;

	dxcur_snd_c s_sel(_T("sound/select.wav"));

	while (GetMouseInputLog2(NULL, NULL, NULL, NULL, true) == 0) {}
	cutin.SetIo(CUT_FRAG_OUT);

	while (true) {
		if (GetWindowUserCloseFlag(TRUE)) {
			next = SCENE_EXIT;
			break;
		}
		if (cutin.IsEndAnim()) { break; }
		if (exit_flag) { break; }

		switch (RecMenuGetAllAct(cmd, menuitem, key, s_sel, cutin)) {
		case KEY_INPUT_RETURN:
			next = menuitem[cmd].next;
			if (menuitem[cmd].cutin) {
				cutin.SetTipNo();
				cutin.SetCutTipFg(CUTIN_TIPS_ON);
				cutin.SetIo(CUT_FRAG_IN);
			}
			else {
				exit_flag = true;
			}
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
		curpic.update();

		ClearDrawScreen(); /* 描画エリアここから */
		DrawGraph(0, 0, backpic.handle(), TRUE);
		charpic.draw();
		for (size_t i = 0; i < subpic.size(); i++) {
			subpic[i].draw();
		}
		curpic.draw(MENU_DRAW_LEFT, draw_win_posU,
			draw_win_posR, draw_win_posU + MENU_DRAW_HEIGHT);
		DrawGraph(0, 0, basepic.handle(), TRUE);
		help.DrawHelp(rec_helpbar_type_ec::MENU);
		cutin.DrawCut();
		ScreenFlip(); /* 描画エリアここまで */
	}

	return next;
}
