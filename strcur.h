//����'/'��':'������Ƃ���܂ŏ���
void strnex(wchar_t *p1) {
	wchar_t *p2 = p1;
	short int a = 1;
	while (*p1 >= L' ' && *p1 <= L'}' && *p1 != L'/' && *p1 != L':' && *p1 != L'\0') {
		a++;
		p1++;
	}
	strmods(p2, a);
	return;
}

//p1�̍Ō��a�̕�����ǉ�����
void stradds(wchar_t *p1, wchar_t a) {
	while (*p1 != L'\0') p1++;
	*p1++ = a;
	*p1 = L'\0';
	return;
}