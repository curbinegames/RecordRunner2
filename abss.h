//aとbの絶対値を求める
int abss(int a,int b) {
	a -= b;
	if (a < 0) {
		a *= -1;
	}
	return a;
}