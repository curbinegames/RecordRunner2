//aをbまで引き下げたものを返す
int maxs(int a, int b) {
	if (a > b)a = b;
	return a;
}