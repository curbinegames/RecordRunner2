//aをbまで引き上げたものを返す
int mins(int a, int b) {
	if (a < b)a = b;
	return a;
}
//aをbまで引き下げたものを返す
int maxs(int a, int b) {
	if (a > b)a = b;
	return a;
}
//aをbまで引き上げたものを返す
double mins2(double a, double b) {
	if (a < b)a = b;
	return a;
}
//aをbまで引き下げたものを返す
double maxs2(double a, double b) {
	if (a > b)a = b;
	return a;
}
//aが0だったら1にする
int notzero(int a) {
	if (a == 0)a = 1;
	return a;
}
//点1(x1,y1)、点2(x2,y2)、を通る直線を考えて、xの時の値を返す。
int lins(double x1, double y1, double x2, double y2, double x) {
	if (x1 == x2) return x1;
	else return (y2 - y1) / (x2 - x1) * (x - x1) + y1;
}
//頂点(x1,y1)、点(x2,y2)、を通る二次関数を考えて、xの時の値を返す。
int pals(double x1, double y1, double x2, double y2, double x) {
	if (x1 == x2) return x1;
	else return (y2 - y1) / (x2 - x1) / (x2 - x1) * (x - x1) * (x - x1) + y1;
}
//ルートcを返す(打切り誤差あり)
double sanrute(double c) {
	short int d = 10;
	double a = 0, b = 1;
	while (b * b < c) b *= 10;
	if (b * b == c) return b;
	b /= 10;
	while (b >= 100000 || d == -10) {
		while (a * a < c) a += b;
		if (a * a == c) return a;
		a -= b;
		b /= d;
		if (b == 1) d *= -1;
	}
	return a;
}
//aとbの絶対値を求める
int abss(int a, int b) {
	a -= b;
	if (a < 0) {
		a *= -1;
	}
	return a;
}
//aを0からbでループさせる
int NumLoop(int a, int b) {
	while (a < 0) { a += b; }
	while (a > b) { a -= b; }
	return a;
}