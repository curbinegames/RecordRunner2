
#ifndef PLAY_BOX

#define PLAY_BOX 1

struct judge_box {
	int pjust = 0;
	int just = 0;
	int good = 0;
	int safe = 0;
	int miss = 0;
};
struct note_img {
	int notebase = LoadGraph(L"picture/hit.png");
	int hitcircle = LoadGraph(L"picture/hitc.png");
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

#endif // PLAY_BOX
