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