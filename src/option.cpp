
#include <RecWindowRescale.h>
#include <option.h>

typedef enum rec_param_type_e {
	REC_PARAM_TYPE_BOOL,
	REC_PARAM_TYPE_UINT,
	REC_PARAM_TYPE_INT
} rec_param_type_et;

#if 1 /* 項目系 */

/* 継承前提、テンプレートは実装できなかった */
class rec_option_item_base_c {
protected:
	rec_param_type_et param_type = REC_PARAM_TYPE_BOOL;
	bool is_loop_param = false; /* is_bool_paramがtrueだったら無効 */
	int  lower_limit   = 0;     /* is_bool_paramがtrueだったら無効、is_loop_paramがtrueだったら0固定 */
	int  upper_limit   = 0;     /* is_bool_paramがtrueだったら無効 */
	void *option_p     = &optiondata.empty_val;
	dxcur_pic_c base_pic = dxcur_pic_c();

public:
	rec_system_langstr_c item_name{  _T(""), _T("") };
	rec_system_langstr_c item_detail{_T(""), _T("") };

	void CmdUp(void) {
		switch (this->param_type) {
		case REC_PARAM_TYPE_BOOL:
			*(bool *)(this->option_p) = !(*(bool *)(this->option_p));
			break;
		case REC_PARAM_TYPE_UINT:
			if (this->is_loop_param) {
				*(uint *)(this->option_p) = LOOP_ADD(*(uint *)(this->option_p), this->upper_limit + 1);
			}
			else {
				*(uint *)(this->option_p) = min(*(uint *)(this->option_p) + 1, this->upper_limit);
			}
			break;
		case REC_PARAM_TYPE_INT:
			if (this->is_loop_param) {
				*(int *)(this->option_p) = LOOP_ADD(*(int *)(this->option_p), this->upper_limit + 1);
			}
			else {
				*(int *)(this->option_p) = min(*(int *)(this->option_p) + 1, this->upper_limit);
			}
			break;
		}
	}

	void CmdDown(void) {
		switch (this->param_type) {
		case REC_PARAM_TYPE_BOOL:
			*(bool *)(this->option_p) = !(*(bool *)(this->option_p));
			break;
		case REC_PARAM_TYPE_UINT:
			if (this->is_loop_param) {
				*(uint *)(this->option_p) = LOOP_SUB(*(uint *)(this->option_p), this->upper_limit + 1);
			}
			else {
				*(uint *)(this->option_p) = max(this->lower_limit, *(uint *)(this->option_p) - 1);
			}
			break;
		case REC_PARAM_TYPE_INT:
			if (this->is_loop_param) {
				*(int *)(this->option_p) = LOOP_SUB(*(int *)(this->option_p), this->upper_limit + 1);
			}
			else {
				*(int *)(this->option_p) = max(this->lower_limit, *(int *)(this->option_p) - 1);
			}
			break;
		}
	}

	void ChangeBool(void) {
		if (this->param_type == REC_PARAM_TYPE_BOOL) {
			this->CmdUp();
		}
	}

	virtual tstring GetParamName(void) const {
		tstring s = _T("");
		return s;
	}

	virtual tstring GetParamDetail(void) const {
		tstring s = _T("");
		return s;
	}

	virtual DxPic_t GetPicHandle(void) const {
		return this->base_pic.handle();
	}

	virtual void ReloadPic(void) {}
};

class rec_option_item_chara_c : public rec_option_item_base_c {
private:
	dxcur_pic_c pic[5];

	rec_system_langstr_c param_name_str[3] = {
		rec_system_langstr_c(_T("ピッカー"),       _T("Picker")),
		rec_system_langstr_c(_T("マップゲーター"), _T("MapGator")),
		rec_system_langstr_c(_T("テイラー"),       _T("Taylor"))
	};

public:
	rec_option_item_chara_c (void) {
		this->param_type    = REC_PARAM_TYPE_UINT;
		this->is_loop_param = true;
		this->upper_limit   = 2;
		this->option_p      = &optiondata.chara;
		this->item_name     = rec_system_langstr_c(_T("キャラクター"), _T("Character"));
		this->item_detail   = rec_system_langstr_c(
			_T("使用するキャラクターを変えます。"), _T("Choose the character you use.")
		);
	}

	tstring GetParamName(void) const override {
		tstring s = _T("");
		switch (*(uint *)(this->option_p)) {
		case REC_CHARA_PICKER:
			s = param_name_str[0].get_str();
			break;
		case REC_CHARA_GATOR:
			s = param_name_str[1].get_str();
			break;
		case REC_CHARA_TAYLOR:
			s = param_name_str[2].get_str();
			break;
		}
		return s;
	}

	DxPic_t GetPicHandle(void) const override {
		return this->pic[*(uint *)(this->option_p)].handle();
	}

	void ReloadPic(void) override {
		this->pic[0].reload(_T(""));
		this->pic[1].reload(_T(""));
		this->pic[2].reload(_T(""));
		this->pic[3].reload(_T(""));
		this->pic[4].reload(_T(""));
	}
};

class rec_option_item_noteoffset_time_c : public rec_option_item_base_c {
public:
	rec_option_item_noteoffset_time_c (void) {
		this->param_type    = REC_PARAM_TYPE_INT;
		this->is_loop_param = false;
		this->lower_limit   = -10000;
		this->upper_limit   =  10000;
		this->option_p      = &optiondata.offset;
		this->item_name     = rec_system_langstr_c(_T("オフセット"), _T("Offset"));
		this->item_detail   = rec_system_langstr_c(
			_T("音符が流れてくるタイミングを変えます。\nFASTが多いなら減らして、LATEが多いなら増やしてください。"),
			_T("Change the timing of note. Increase to late, Decrease to early.")
		);
	}

	tstring GetParamName(void) const override {
		tstring ret = _T("");
		ret += (0 < *(int *)(this->option_p)) ? _T("+") : _T("");
		ret += std::to_wstring(*(int *)(this->option_p));
		ret += _T("ms");
		return ret;
	}

	void ReloadPic(void) override {
		this->base_pic.reload(_T(""));
	}
};

class rec_option_item_seenable_c : public rec_option_item_base_c {
public:
	rec_option_item_seenable_c (void) {
		this->param_type    = REC_PARAM_TYPE_BOOL;
		this->option_p      = &optiondata.SEenable;
		this->item_name     = rec_system_langstr_c(_T("効果音"), _T("SE"));
		this->item_detail   = rec_system_langstr_c(
			_T("音符をたたいた時の効果音を鳴らすかどうかを変えます"),
			_T("Choose whether to sound of hitting notes.")
		);
	}

	tstring GetParamName(void) const override {
		tstring s = _T("");
		switch (*(bool *)(this->option_p)) {
		case false:
			s = _T("OFF");
			break;
		case true:
			s = _T("ON");
			break;
		}
		return s;
	}

	void ReloadPic(void) override {
		this->base_pic.reload(_T(""));
	}
};

class rec_option_item_backbright_c : public rec_option_item_base_c {
private:
	dxcur_pic_c pic[5];

	rec_system_langstr_c param_name_str[4] = {
		rec_system_langstr_c(_T("真っ黒"), _T("Black") ),
		rec_system_langstr_c(_T("暗い"),   _T("Dark")  ),
		rec_system_langstr_c(_T("中間"),   _T("Middle")),
		rec_system_langstr_c(_T("明るい"), _T("Bright"))
	};

public:
	rec_option_item_backbright_c (void) {
		this->param_type    = REC_PARAM_TYPE_UINT;
		this->is_loop_param = true;
		this->upper_limit   = 3;
		this->option_p      = &optiondata.backbright;
		this->item_name     = rec_system_langstr_c(_T("背景の明るさ"), _T("Background Brightness"));
		this->item_detail   = rec_system_langstr_c(
			_T("背景の明るさを変えます。"), _T("Choose brightness of background.")
		);
	}

	tstring GetParamName(void) const override {
		return param_name_str[*(uint *)(this->option_p)].get_str();
	}

	DxPic_t GetPicHandle(void) const override {
		return this->pic[*(uint *)(this->option_p)].handle();
	}

	void ReloadPic(void) override {
		this->pic[0].reload(_T(""));
		this->pic[1].reload(_T(""));
		this->pic[2].reload(_T(""));
		this->pic[3].reload(_T(""));
		this->pic[4].reload(_T(""));
	}
};

class rec_option_item_language_c : public rec_option_item_base_c {
private:
	dxcur_pic_c pic[5];

	rec_system_langstr_c param_name_str{_T("日本語"), _T("English")};

public:
	rec_option_item_language_c (void) {
		this->param_type    = REC_PARAM_TYPE_UINT;
		this->is_loop_param = true;
		this->upper_limit   = 1;
		this->option_p      = &optiondata.lang;
		this->item_name     = rec_system_langstr_c(_T("言語 Language"), _T("言語 Language"));
		this->item_detail   = rec_system_langstr_c(
			_T("ゲームで使う言語を変えます。\nChoose the lunguage in this game."),
			_T("ゲームで使う言語を変えます。\nChoose the lunguage in this game.")
		);
	}

	tstring GetParamName(void) const override {
		return param_name_str.get_str();
	}

	DxPic_t GetPicHandle(void) const override {
		return this->pic[*(uint *)(this->option_p)].handle();
	}

	void ReloadPic(void) override {
		this->pic[0].reload(_T(""));
		this->pic[1].reload(_T(""));
		this->pic[2].reload(_T(""));
		this->pic[3].reload(_T(""));
		this->pic[4].reload(_T(""));
	}
};

class rec_option_item_keydetail_c : public rec_option_item_base_c {
public:
	rec_option_item_keydetail_c (void) {
		this->param_type    = REC_PARAM_TYPE_BOOL;
		this->option_p      = &optiondata.keydetail;
		this->item_name     = rec_system_langstr_c(_T("ボタン表示"), _T("Button Guide"));
		this->item_detail   = rec_system_langstr_c(
			_T("ボタンの押し状況をプレイ画面に表示します。"),
			_T("Choose whether to display the key states on playing mode.")
		);
	}

	tstring GetParamName(void) const override {
		tstring s = _T("");
		switch (*(bool *)(this->option_p)) {
		case false:
			s = _T("OFF");
			break;
		case true:
			s = _T("ON");
			break;
		}
		return s;
	}

	void ReloadPic(void) override {
		this->base_pic.reload(_T(""));
	}
};

class rec_option_item_combopos_c : public rec_option_item_base_c {
private:
	dxcur_pic_c pic[6];

	rec_system_langstr_c param_name_str[6] = {
		rec_system_langstr_c(_T("中央の上"),   _T("top centre")),
		rec_system_langstr_c(_T("左上"),       _T("top left")  ),
		rec_system_langstr_c(_T("右上"),       _T("top right") ),
		rec_system_langstr_c(_T("中央"),       _T("centre")    ),
		rec_system_langstr_c(_T("キャラの上"), _T("near chara")),
		rec_system_langstr_c(_T("表示しない"), _T("none")      )
	};

public:
	rec_option_item_combopos_c (void) {
		this->param_type    = REC_PARAM_TYPE_UINT;
		this->is_loop_param = true;
		this->upper_limit   = 5;
		this->option_p      = &optiondata.combopos;
		this->item_name     = rec_system_langstr_c(_T("判定表示位置"), _T("Judge Position"));
		this->item_detail   = rec_system_langstr_c(
			_T("判定の表示場所を決めます。"), _T("Choose judge position.")
		);
	}

	tstring GetParamName(void) const override {
		return param_name_str[*(uint *)(this->option_p)].get_str();
	}

	DxPic_t GetPicHandle(void) const override {
		return this->pic[*(uint *)(this->option_p)].handle();
	}

	void ReloadPic(void) override {
		this->pic[0].reload(_T(""));
		this->pic[1].reload(_T(""));
		this->pic[2].reload(_T(""));
		this->pic[3].reload(_T(""));
		this->pic[4].reload(_T(""));
	}
};

class rec_option_item_bgmvol_c : public rec_option_item_base_c {
public:
	rec_option_item_bgmvol_c (void) {
		this->param_type    = REC_PARAM_TYPE_INT;
		this->is_loop_param = false;
		this->lower_limit   = 0;
		this->upper_limit   = 10;
		this->option_p      = &optiondata.BGMvolume;
		this->item_name     = rec_system_langstr_c(_T("BGMの音量"), _T("BGM Volume"));
		this->item_detail   = rec_system_langstr_c(_T("BGMの音量を決めます。"), _T("Choose BGM volume."));
	}

	tstring GetParamName(void) const override {
		tstring ret = _T("");
		int val = *(int *)(this->option_p) * 10;
		ret += (0 < val) ? _T("+") : _T("");
		ret += std::to_wstring(val);
		ret += _T("%");
		return ret;
	}

	void ReloadPic(void) override {
		this->base_pic.reload(_T(""));
	}
};

class rec_option_item_sevol_c : public rec_option_item_base_c {
public:
	rec_option_item_sevol_c (void) {
		this->param_type    = REC_PARAM_TYPE_INT;
		this->is_loop_param = false;
		this->lower_limit   = 0;
		this->upper_limit   = 10;
		this->option_p      = &optiondata.SEvolume;
		this->item_name     = rec_system_langstr_c(_T("SEの音量"), _T("SE Volume"));
		this->item_detail   = rec_system_langstr_c(_T("SEの音量を決めます。"), _T("Choose SE volume."));
	}

	tstring GetParamName(void) const override {
		tstring ret = _T("");
		int val = *(int *)(this->option_p) * 10;
		ret += (0 < val) ? _T("+") : _T("");
		ret += std::to_wstring(val);
		ret += _T("%");
		return ret;
	}

	void ReloadPic(void) override {
		this->base_pic.reload(_T(""));
	}
};

class rec_option_item_lineThick_c : public rec_option_item_base_c {
public:
	rec_option_item_lineThick_c (void) {
		this->param_type    = REC_PARAM_TYPE_INT;
		this->is_loop_param = false;
		this->lower_limit   = 0;
		this->upper_limit   = 30;
		this->option_p      = &optiondata.lineThick;
		this->item_name     = rec_system_langstr_c(
			_T("レーンガイドの太さ"), _T("Lane Guide Line Thickness")
		);
		this->item_detail   = rec_system_langstr_c(
			_T("レーンガイドの太さを決めます。"), _T("Choose lane guide line thickness.")
		);
	}

	tstring GetParamName(void) const override {
		tstring ret = _T("");
		int val = *(int *)(this->option_p);
		ret += std::to_wstring(val);
		ret += _T("px");
		return ret;
	}

	void ReloadPic(void) override {
		this->base_pic.reload(_T(""));
	}
};

class rec_option_item_barThick_c : public rec_option_item_base_c {
public:
	rec_option_item_barThick_c (void) {
		this->param_type    = REC_PARAM_TYPE_INT;
		this->is_loop_param = false;
		this->lower_limit   = 0;
		this->upper_limit   = 30;
		this->option_p      = &optiondata.barThick;
		this->item_name     = rec_system_langstr_c(_T("小節線の太さ"), _T("Measure Line Thickness"));
		this->item_detail   = rec_system_langstr_c(
			_T("小節線の太さを決めます。"), _T("Choose measure line thickness.")
		);
	}

	tstring GetParamName(void) const override {
		tstring ret = _T("");
		int val = *(int *)(this->option_p);
		ret += std::to_wstring(val);
		ret += _T("px");
		return ret;
	}

	void ReloadPic(void) override {
		this->base_pic.reload(_T(""));
	}
};

#endif /* 項目系 */

static std::vector<rec_option_item_base_c *> s_op_list_system = {
	new rec_option_item_language_c(),
	new rec_option_item_chara_c(),
	new rec_option_item_bgmvol_c(),
	new rec_option_item_sevol_c()
};

static std::vector<rec_option_item_base_c *> s_op_list_play = {
	new rec_option_item_noteoffset_time_c(),
	new rec_option_item_backbright_c(),
	new rec_option_item_keydetail_c(),
	new rec_option_item_combopos_c(),
	new rec_option_item_lineThick_c(),
	new rec_option_item_barThick_c()
};

rec_option_t optiondata;

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

void RecOptionDrawAll(
	int cmd,
	const rec_helpbar_c &help,
	const dxcur_pic_c &cursor_pic,
	const dxcur_pic_c &back_pic
) {
	static const int det_txposx = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 20);
	static const int det_txposy = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 410);

	RecRescaleDrawGraph(0, 0, back_pic.handle(), TRUE); /* 背景 */
	RecRescaleDrawGraph(40, 45 + cmd * 40, cursor_pic.handle(), TRUE); /* カーソル */

	/* 項目表示 */
	for (int i = 0; i < s_op_list_play.size(); i++) {
		static const int title_txposx = lins(0, 0, OLD_WINDOW_SIZE_X, WINDOW_SIZE_X, 100);
		static const int title_txposy = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 50);
		static const int title_txgapy = lins(0, 0, OLD_WINDOW_SIZE_Y, WINDOW_SIZE_Y, 40);

		tstring buf = s_op_list_play[i]->GetParamName();

		DrawFormatString(title_txposx, title_txposy + title_txgapy * i, COLOR_WHITE, L"%s: %s",
			s_op_list_play[i]->item_name.get_str().c_str(), buf.c_str());
	}

	DrawFormatString(det_txposx, det_txposy, COLOR_WHITE, L"%s",
		s_op_list_play[cmd]->item_detail.get_str().c_str());

	help.DrawHelp(rec_helpbar_type_ec::OPTION);
}

static void RecOptionKeyCtrl(int *cmd, bool *exitFg, dxcur_snd_c &s_sel) {
	InputAllKeyHold();
	switch (GetKeyPushOnce()) {
	case KEY_INPUT_UP:   /* 項目選択 */
		*cmd = LOOP_SUB(*cmd, s_op_list_play.size());
		s_sel.PlaySound();
		break;
	case KEY_INPUT_DOWN: /* 項目選択 */
		*cmd = LOOP_ADD(*cmd, s_op_list_play.size());
		s_sel.PlaySound();
		break;
	case KEY_INPUT_LEFT: /* 項目操作 */
		s_op_list_play.at(*cmd)->CmdDown();
		s_sel.PlaySound();
		break;
	case KEY_INPUT_RIGHT: /* 項目操作 */
		s_op_list_play.at(*cmd)->CmdUp();
		s_sel.PlaySound();
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
	dxcur_snd_c s_sel(_T("sound/select.wav"));

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

	AvoidKeyRush();

	//処理
	while (1) {
		RecOptionKeyCtrl(&command, &exitFg, s_sel);
		ClearDrawScreen(); /* 描画エリアスタート */
		RecOptionDrawAll(command, help, pic.cursor, pic.back);
		ScreenFlip(); /* 描画エリアここまで */
		if (GetWindowUserCloseFlag(TRUE)) { return SCENE_EXIT; }
		if (exitFg) { break; }
	}

	return SCENE_MENU;
}
