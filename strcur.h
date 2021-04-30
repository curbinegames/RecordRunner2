//p1�̐擪��p2�����邩�ǂ����𒲂ׂ�
int strands(wchar_t *p1, wchar_t *p2) {
	int j = 1;
	while (*p2 != L'\0') {
		if (*p1 != *p2) {
			j = 0;
			break;
		}
		p1++;
		p2++;
	}
	return j;
}

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

//p1����擪a����������
void strmods(wchar_t *p1, int a) {
	int b;
	wchar_t *p2;
	p2 = p1;
	for (b = 0; b < a; b++) p1++;
	while (*p1 != L'\0') {
		*p2 = *p1;
		p1++;
		p2++;
	}
	*p2 = L'\0';
	return;
}

//p1�̍Ō��p2��t����
void strcats(wchar_t *p1, wchar_t *p2) {
	while (*p1 != L'\0') p1++;
	while (*p2 != L'\0') {
		*p1 = *p2;
		p1++;
		p2++;
	}
	*p1 = L'\0';
	return;
}

//������𐔎��ɕϊ��ɂ���
//�r���ɐ���,�}�C�i�X�ȊO�̕�������������I���
int strsans(wchar_t *p1) {
	int a = 0, b = 1;
	while (*p1 == L'0' ||
		*p1 == L'1' ||
		*p1 == L'2' ||
		*p1 == L'3' ||
		*p1 == L'4' ||
		*p1 == L'5' ||
		*p1 == L'6' ||
		*p1 == L'7' ||
		*p1 == L'8' ||
		*p1 == L'9' ||
		*p1 == L'-') {
		a *= 10;
		switch (*p1) {
		case L'1':
			a += 1;
			break;
		case L'2':
			a += 2;
			break;
		case L'3':
			a += 3;
			break;
		case L'4':
			a += 4;
			break;
		case L'5':
			a += 5;
			break;
		case L'6':
			a += 6;
			break;
		case L'7':
			a += 7;
			break;
		case L'8':
			a += 8;
			break;
		case L'9':
			a += 9;
			break;
		case L'-':
			b *= -1;
			break;
		}
		p1++;
	}
	return a * b;
}
//�������ɂ�������
double strsans2(wchar_t *p1) {
	short int b = 1, c = 0, d = 99, i;
	double a = 0;
	while (1) {
		if (*p1 >= L'0' && *p1 <= L'9') {
			c++;
			a *= 10;
			a += *p1 - 48;
		}
		else if (*p1 == L'-') b *= -1;
		else if (*p1 == L'.') d = c;
		else {
			for (i = c; i > d; i--) a /= 10.0;
			return b * a;
		}
		p1++;
	}
}

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