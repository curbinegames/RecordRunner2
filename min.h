//aをbまで引き上げたものを返す
int mins(int a, int b) {
	if (a < b)a = b;
	return a;
}