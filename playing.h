double SETbpm(wchar_t *p1) {
	strmods(p1, 5);
	return strsans2(p1);
}

int SEToffset(wchar_t *p1) {
	strmods(p1, 12);
	return strsans(p1);
}

int SETLv(wchar_t *p1) {
	strmods(p1, 7);
	return strsans(p1);
}

int SETitem(wchar_t *p1, wchar_t *p2) {
	int a;
	wchar_t GT1[255] = L"record/";
	wchar_t GT2[] = L"/";
	strmods(p1, 6);
	strcats(GT1, p2);
	strcats(GT1, GT2);
	strcats(GT1, p1);
	WaitTimer(100);
	return LoadGraph(GT1);
}

int shifttime(double n, double bpm, int time) {
	return time + 240000.0 * (n - 1.0) / (bpm * 16.0);
}

int read(int a) {
	return LoadGraph(L"picture/backskynoamal.png");
}

void SETMove(double NowTime, double StartTime, double MovePoint, double MoveType, double EndTime, double bpm,int *StaetTimeBuff,int *MovePointBuff,int *EndTimeBuff,int *MoveTypeBuff) {
	*StaetTimeBuff = shifttime(StartTime, bpm, NowTime);
	*MovePointBuff = MovePoint * 50.0 + 100.0;
	*EndTimeBuff = shifttime(EndTime, bpm, NowTime) - 5;
	*MoveTypeBuff = MoveType;
}