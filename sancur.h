//a��b�܂ň����グ�����̂�Ԃ�
int mins(int a, int b) {
	if (a < b)a = b;
	return a;
}
//a��b�܂ň������������̂�Ԃ�
int maxs(int a, int b) {
	if (a > b)a = b;
	return a;
}
//a��b�܂ň����グ�����̂�Ԃ�
double mins2(double a, double b) {
	if (a < b)a = b;
	return a;
}
//a��b�܂ň������������̂�Ԃ�
double maxs2(double a, double b) {
	if (a > b)a = b;
	return a;
}
//a��0��������1�ɂ���
int notzero(int a) {
	if (a == 0)a = 1;
	return a;
}
//�_1(x1,y1)�A�_2(x2,y2)�A��ʂ钼�����l���āAx�̎��̒l��Ԃ��B
int lins(double x1, double y1, double x2, double y2, double x) {
	if (x1 == x2) return x1;
	else return (y2 - y1) / (x2 - x1) * (x - x1) + y1;
}
//���_(x1,y1)�A�_(x2,y2)�A��ʂ�񎟊֐����l���āAx�̎��̒l��Ԃ��B
int pals(double x1, double y1, double x2, double y2, double x) {
	if (x1 == x2) return x1;
	else return (y2 - y1) / (x2 - x1) / (x2 - x1) * (x - x1) * (x - x1) + y1;
}
//���[�gc��Ԃ�(�Ő؂�덷����)
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
//a��b�̐�Βl�����߂�
int abss(int a, int b) {
	a -= b;
	if (a < 0) {
		a *= -1;
	}
	return a;
}
//a��0����b�Ń��[�v������
int NumLoop(int a, int b) {
	while (a < 0) { a += b; }
	while (a > b) { a -= b; }
	return a;
}