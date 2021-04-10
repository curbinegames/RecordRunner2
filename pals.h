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