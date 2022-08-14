
#ifndef PLAY_BOX

#define PLAY_BOX 1

struct camera_box {
	int starttime = -1;
	int endtime = -1;
	int xpos = -1;
	int ypos = -1;
	double zoom = -1;
	int rot = -1;
	int mode = -1;
};
struct custom_note_box {
	wchar_t note = L'\0';
	int color = 0;/*(only hit note)0=green, 1=red, 2=blue, 3=yellow, 4=black, 5=white*/
	int sound = 0;
};
struct judge_box {
	int pjust = 0;
	int just = 0;
	int good = 0;
	int safe = 0;
	int miss = 0;
};
struct note_img {
	int notebase = LoadGraph(L"picture/hit.png");
	int hitcircle[6] = {
		LoadGraph(L"picture/hitc-G.png"),
		LoadGraph(L"picture/hitc-R.png"),
		LoadGraph(L"picture/hitc-B.png"),
		LoadGraph(L"picture/hitc-Y.png"),
		LoadGraph(L"picture/hitc-X.png"),
		LoadGraph(L"picture/hitc-W.png"),
	};
	int catchi = LoadGraph(L"picture/catch.png");
	int up = LoadGraph(L"picture/up.png");
	int down = LoadGraph(L"picture/down.png");
	int left = LoadGraph(L"picture/left.png");
	int right = LoadGraph(L"picture/right.png");
	int bomb = LoadGraph(L"picture/bomb.png");
	int goust = LoadGraph(L"picture/goust.png");
};
struct note_box {
	int hittime = -1;
	int viewtime = -1;
	int object = -1;
	int xpos = -1;
	int ypos = -1;
	int sound = 0;
	int color = 0;
};
struct score_box {
	int normal = 0;
	int combo = 0;
	int loss = 0;
	int sum = 0;
	int pjust = 0;
	int before = 0;
	int time = 0;
};
struct scrool_box {
	int starttime = -1;
	double basetime = -1;
	double speed = -1;
};

#endif // PLAY_BOX
