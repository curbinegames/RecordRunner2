//p1��p2�ɃR�s�[����Bc��1�������255���܂ŏ���������(����������)
void strcopy(wchar_t *p1, wchar_t *p2, int c) {
	int a = 0;
	while (*p2 != L'\0' && c == 1) {
		*p2 = L'\0';
		p2++;
		a++;
		if (a > 250) break;
	}
	p2 -= a;
	while (*p1 != L'\0') {
		*p2 = *p1;
		p1++;
		p2++;
	}
	*p2 = L'\0';
	return;
}