
/* base include */
#include <DxLib.h>

/* curbine code include */
#include <sancur.h>

/* rec system include */
#include <RecWindowRescale.h>
#include <RecSystem.h>

#define EFF_TIME_1 750
#define EFF_TIME_2 200

class rec_title_cutin_c {
private:
	int StartTime = 0;
	dxcur_pic_c TitleChar[12] = {
		dxcur_pic_c(L"picture/Title-1.png"),
		dxcur_pic_c(L"picture/Title-2.png"),
		dxcur_pic_c(L"picture/Title-3.png"),
		dxcur_pic_c(L"picture/Title-4.png"),
		dxcur_pic_c(L"picture/Title-5.png"),
		dxcur_pic_c(L"picture/Title-6.png"),
		dxcur_pic_c(L"picture/Title-7.png"),
		dxcur_pic_c(L"picture/Title-8.png"),
		dxcur_pic_c(L"picture/Title-9.png"),
		dxcur_pic_c(L"picture/Title-10.png"),
		dxcur_pic_c(L"picture/Title-11.png"),
		dxcur_pic_c(L"picture/Title-12.png")
	};

public:
	~rec_title_cutin_c() {
		for (uint inum = 0; inum < 12; inum++) {
			DeleteGraph(TitleChar[inum].handle());
		}
	}

	bool IsEndAnim(void) const {
		return (EFF_TIME_1 <= (GetNowCount() - StartTime));
	}

	void DrawAll(void) const {
		const int Ntime = GetNowCount() - this->StartTime;
		int posX[12] = { 29,133,204,277,352,422,130,224,288,353,422,480 };
		int posY[2] = { 37,120 };
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 6; j++) {
				int drawX = pals(EFF_TIME_1 * (i * 6 + j + 3) / 14,
					posX[i * 6 + j],
					EFF_TIME_1 * (i * 6 + j) / 14,
					posX[i * 6 + j] + 640,
					mins_2(Ntime, EFF_TIME_1 * (i * 6 + j + 3) / 14));
				RecRescaleDrawGraph(drawX, posY[i], this->TitleChar[i * 6 + j].handle(), TRUE);
			}
		}
		return;
	}

	void SetStartTime(void) {
		this->StartTime = GetNowCount();
	}
};

class rec_title_main_c {
private:
	int StartTime = 0;
	dxcur_pic_c Title = dxcur_pic_c(L"picture/TitleMain.png");
	dxcur_pic_c Push  = dxcur_pic_c(L"picture/pushkey.png");
	dxcur_pic_c white = dxcur_pic_c(L"picture/White.png");

public:
	void DrawAll(void) const {
		RecRescaleDrawGraph(0, 0, this->Title.handle(), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(-1, 0, 1, 255, sinC((GetNowCount() / 4) % 360)));
		RecRescaleDrawGraph(0, 0, this->Push.handle(), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		if ((GetNowCount() - this->StartTime) < EFF_TIME_2) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(0, 255, EFF_TIME_2, 0, GetNowCount() - this->StartTime));
			RecRescaleDrawGraph(0, 0, this->white.handle(), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		return;
	}

	void SetStartTime(void) {
		this->StartTime = GetNowCount();
	}
};

class rec_title_obnect_c {
private:
	rec_title_cutin_c anim1;
	rec_title_main_c  anim2;

	int state = 0;

public:
	void DrawObject(void) const {
		switch (this->state) {
		case 0:
			this->anim1.DrawAll();
			break;
		case 1:
			this->anim2.DrawAll();
			break;
		}
	}

	void Start(void) {
		this->anim1.SetStartTime();
	}

	void Update(void) {
		if ((this->state == 0) && (this->anim1.IsEndAnim())) {
			this->state = 1;
			this->anim2.SetStartTime();
		}
	}

	int GetState(void) const {
		return this->state;
	}
};

now_scene_t title(void) {
	rec_cutin_c cutin;
	rec_title_obnect_c TitleObjectClass;

	TitleObjectClass.Start();
	while (1) {
		ClearDrawScreen(); /* •`‰æƒGƒŠƒA‚±‚±‚©‚ç */

		TitleObjectClass.Update();
		TitleObjectClass.DrawObject();

		cutin.DrawCut();

		ScreenFlip(); /* •`‰æƒGƒŠƒA‚±‚±‚Ü‚Å */

		if (CheckHitKeyAll() && (TitleObjectClass.GetState() == 1) && (cutin.IsClosing() == 0)) {
			cutin.SetCutTipFg(CUTIN_TIPS_NONE);
			cutin.SetIo(CUT_FRAG_IN);
		}

		if (cutin.IsEndAnim()) {
			INIT_SND();
			break;
		}

		if (GetWindowUserCloseFlag(TRUE)) {
			return SCENE_EXIT;
		}
		WaitTimer(WAIT_TIME_ON_GAMELOOP);
	}
	ClearDrawScreen();
	return SCENE_MENU;
}
