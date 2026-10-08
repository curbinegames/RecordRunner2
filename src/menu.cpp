
#include <array>
#include <cmath>
#include <RecSystem.h>

#define MENU_DRAW_LEFT 45
#define MENU_DRAW_HEIGHT 65

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

	void draw(int x = 0, int y = 0) const {
		DxTime_t Ntime = GetNowCount();
		int    drawX = std::sin(Ntime /  1800.0 + this->phase) * 20 + this->base_posx;
		int    drawY = std::sin(Ntime /  2500.0 + this->phase) * 10 + this->base_posy;
		double drawR = std::sin(Ntime /  2200.0 + this->phase) *  5;
		DrawDeformationPic(drawX + x, drawY + y, 1, 1, drawR, this->pic.handle());
	}
};

struct rec_menu_item_st {
	int posUp = 0;
	int posRight = 0;
	bool cutin = false;
	now_scene_t next = SCENE_EXIT;
	dxcur_pic_c backpic;

	std::vector<rec_menu_pic_item_c> itempic;
};

class rec_menu_cmd_c {
private:
	int b_cmd = 0;
	int cmd = 0;
	DxTime_t Stime = 0;

public:
	rec_menu_cmd_c &operator=(int val) {
		set_cmd(val);
		return *this;
	}

	rec_menu_cmd_c operator++(int) {
		set_cmd(LOOP_ADD(cmd, 6));
		return *this;
	}

	rec_menu_cmd_c operator--(int) {
		set_cmd(LOOP_SUB(cmd, 6));
		return *this;
	}

	bool operator==(const int &r) const {
		return this->cmd == r;
	}

	bool operator!=(const int &r) const {
		return !(*this == r);
	}

	void set_cmd(int val) {
		if (cmd == val) { return; }
		b_cmd = cmd;
		cmd = val;
		Stime = GetNowCount();
	}

	int get_bcmd(void) const {
		return b_cmd;
	}

	int get_cmd(void) const {
		return cmd;
	}

	DxTime_t get_Stime(void) const {
		return Stime;
	}
};

class rec_menu_cursor_pic_c {
private:
	bool is_to_white = false;
	DxTime_t Stime = GetNowCount();
	dxcur_window_pic_c curpic{_T("picture/cursorwindow.png")};
	dxcur_window_pic_c curwpic{_T("picture/cursorwindowwhite.png")};

	void drawBase(int left, int up, int right, int down) const {
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

public:
	void update(void) {
		DxTime_t Ntime = GetNowCount();
		if (this->Stime + 500 < Ntime) {
			this->Stime = Ntime;
			this->is_to_white = !this->is_to_white;
		}
	}

	void draw(
		const rec_menu_cmd_c &cmd, const std::array<rec_menu_item_st, 6> &menuitem
	) {
		int drawUp    = 0;
		int drawRight = 0;
		DxTime_t Ntime = GetNowCount() - cmd.get_Stime();
		drawUp    = pals_scale(300, menuitem[cmd.get_cmd()].posUp,    0, menuitem[cmd.get_bcmd()].posUp,    Ntime);
		drawRight = pals_scale(300, menuitem[cmd.get_cmd()].posRight, 0, menuitem[cmd.get_bcmd()].posRight, Ntime);
		this->drawBase(MENU_DRAW_LEFT, drawUp, drawRight, drawUp + MENU_DRAW_HEIGHT);
	}
};

/**
 * @brief マウスで何か項目をクリックしたかを確認する。
 * @param[in] menuitem メニュー項目の情報
 * @return -1以外=対応する番号の項目が押された, -1=何も押していない
 */
static int RecMenuMouseClickCheck(const std::array<rec_menu_item_st, 6> &menuitem) {
	int mouseBtn = 0;
	int mouseX = 0;
	int mouseY = 0;
	int mouseAct = 0;

	while (GetMouseInputLog2(&mouseBtn, &mouseX, &mouseY, &mouseAct, true) == 0) {}
	if (mouseAct != MOUSE_INPUT_LEFT) { return -1; }

	for (int i = 0; i < menuitem.size(); i++) {
		if (IS_BETWEEN(MENU_DRAW_LEFT, mouseX, menuitem[i].posRight) &&
			IS_BETWEEN(menuitem[i].posUp, mouseY, menuitem[i].posUp + MENU_DRAW_HEIGHT))
		{
			return i;
		}
	}
	return -1;
}

/**
 * @brief マウスで何か項目を選択したかを確認する。
 * @param[in] menuitem メニュー項目の情報
 * @return -1以外=対応する番号の項目が選択された, -1=何も押していない
 */
static int RecMenuMouseMoveCheck(const std::array<rec_menu_item_st, 6> &menuitem) {
	static int b_mouseX = 0;
	static int b_mouseY = 0;

	int mouseX = 0;
	int mouseY = 0;

	GetMousePoint(&mouseX, &mouseY);
	if (mouseX == b_mouseX && mouseY == b_mouseY) { return -1; }
	b_mouseX = mouseX;
	b_mouseY = mouseY;

	for (int i = 0; i < menuitem.size(); i++) {
		if (IS_BETWEEN(MENU_DRAW_LEFT, mouseX, menuitem[i].posRight) &&
			IS_BETWEEN(menuitem[i].posUp, mouseY, menuitem[i].posUp + MENU_DRAW_HEIGHT))
		{
			return i;
		}
	}
	return -1;
}

/**
 * @brief メニュー画面のマウス操作を取得する
 * @param[out] cmd 選択中のメニュー番号
 * @param[in] menuitem メニュー項目の情報
 * @param[in] s_sel 選択音の再生用
 * @return 選択された操作のキーコード、選択されなかった場合は-1
 * @details マウス操作がないときは更新しない
 * キーボード操作があった後にマウスのクリックを押した場合、マウスカーソルの位置にあるメニュー項目が選択される
 */
static int RecMenuGetMouseAct(
	rec_menu_cmd_c &cmd, const std::array<rec_menu_item_st, 6> &menuitem,
	dxcur_snd_c &s_sel
) {
	int itemNo = -1;

	itemNo = RecMenuMouseClickCheck(menuitem);
	if (itemNo != -1) {
		cmd = itemNo;
		return KEY_INPUT_RETURN;
	}

	itemNo = RecMenuMouseMoveCheck(menuitem);
	if (itemNo != -1 && cmd != itemNo) {
		cmd = itemNo;
		s_sel.PlaySound();
	}

	return -1;
}

static int RecMenuGetKeyAct(dxcur_key_c &key) {
	key.update();
	return key.GetKeyPulseOnce();
}

static int RecMenuGetAllAct(
	rec_menu_cmd_c &cmd, const std::array<rec_menu_item_st, 6> &menuitem,
	dxcur_key_c &key, dxcur_snd_c &s_sel, const rec_cutin_c &cutin
) {
	if (cutin.IsClosing() != 0) { return -1; }

	int act = -1;
	act = RecMenuGetMouseAct(cmd, menuitem, s_sel);
	if (act == -1) { act = RecMenuGetKeyAct(key); }
	return act;
}

static void RecMenuDrawBack(
	const rec_menu_cmd_c &cmd, const std::array<rec_menu_item_st, 6> &menuitem
) {
	int drawA = 0;
	DxTime_t Ntime = GetNowCount() - cmd.get_Stime();
	drawA = lins_scale(0, 0, 300, 255, Ntime);
	/* 前に選んでいた項目 */
	if (Ntime < 300) {
		DrawGraph(0, 0, menuitem[cmd.get_bcmd()].backpic.handle(), TRUE);
	}
	/* 今選んでる項目 */
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, drawA);
	DrawGraph(0, 0, menuitem[cmd.get_cmd()].backpic.handle(), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
}

static void RecMenuDrawPicItem(
	const rec_menu_cmd_c &cmd, const std::array<rec_menu_item_st, 6> &menuitem
) {
	int drawX = 0;
	int drawY = 0;
	int drawA = 0;
	DxTime_t Ntime = GetNowCount() - cmd.get_Stime();
	drawX = pals_scale(300, 200, 0, 0, Ntime);
	drawY = pals_scale(300, 0, 0, 100, Ntime);
	drawA = pals_scale(300, 0, 0, 255, Ntime);
	/* 前に選んでいた項目 */
	if (Ntime < 300) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, drawA);
		for (size_t i = 0; i < menuitem[cmd.get_bcmd()].itempic.size(); i++) {
			menuitem[cmd.get_bcmd()].itempic[i].draw(drawX, 0);
		}
	}
	/* 今選んでる項目 */
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - drawA);
	for (size_t i = 0; i < menuitem[cmd.get_cmd()].itempic.size(); i++) {
		menuitem[cmd.get_cmd()].itempic[i].draw(0, drawY);
	}
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
}

now_scene_t RecMenuBase(void) {
	bool exit_flag = false;
	rec_menu_cmd_c cmd;
	now_scene_t next = SCENE_EXIT;
	dxcur_key_c key;
	rec_helpbar_c help;
	rec_cutin_c cutin;

	std::array<rec_menu_item_st, 6> menuitem = {
		rec_menu_item_st{220, 295,  true, SCENE_SELECT,     _T("picture/menu/freerun_back.png")},
		rec_menu_item_st{285, 450,  true, SCENE_CHALLENGE,  _T("picture/menu/freerun_back.png")},
		rec_menu_item_st{408, 340, false, SCENE_ACHIVEMENT  },
		rec_menu_item_st{472, 208, false, SCENE_STORY       },
		rec_menu_item_st{536, 222, false, SCENE_OPTION      },
		rec_menu_item_st{598, 170, false, SCENE_EXIT        }
	};
	menuitem[0].itempic.push_back(rec_menu_pic_item_c( 920, 330, _T("picture/menu/freerun_picker.png")));
	menuitem[0].itempic.push_back(rec_menu_pic_item_c( 650, 250, _T("picture/menu/freerun_hit.png"   )));
	menuitem[0].itempic.push_back(rec_menu_pic_item_c( 600, 550, _T("picture/menu/freerun_catch.png" )));
	menuitem[0].itempic.push_back(rec_menu_pic_item_c(1200, 300, _T("picture/menu/freerun_arrow.png" )));
	menuitem[1].itempic.push_back(rec_menu_pic_item_c( 650, 450, _T("picture/menu/chall_gator.png"   )));
	menuitem[1].itempic.push_back(rec_menu_pic_item_c(1000, 200, _T("picture/menu/chall_taylor.png"  )));

	rec_menu_cursor_pic_c curpic;
	dxcur_pic_c basepic(_T("picture/menu/menu_base.png"));
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
			next = menuitem[cmd.get_cmd()].next;
			if (menuitem[cmd.get_cmd()].cutin) {
				cutin.SetTipNo();
				cutin.SetCutTipFg(CUTIN_TIPS_ON);
				cutin.SetIo(CUT_FRAG_IN);
			}
			else {
				exit_flag = true;
			}
			break;
		case KEY_INPUT_UP:
			cmd--;
			s_sel.PlaySound();
			break;
		case KEY_INPUT_DOWN:
			cmd++;
			s_sel.PlaySound();
			break;
		}

		curpic.update();

		ClearDrawScreen(); /* 描画エリアここから */
		RecMenuDrawBack(cmd, menuitem);
		RecMenuDrawPicItem(cmd, menuitem);
		curpic.draw(cmd, menuitem);
		DrawGraph(0, 0, basepic.handle(), TRUE);
		help.DrawHelp(rec_helpbar_type_ec::MENU);
		cutin.DrawCut();
		ScreenFlip(); /* 描画エリアここまで */
	}

	return next;
}
