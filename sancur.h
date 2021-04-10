//ƒ‹[ƒgc‚ğ•Ô‚µ‚Ü‚·
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