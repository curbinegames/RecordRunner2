int strands(const wchar_t *p1, const wchar_t *p2);
void strcopy(const wchar_t*, wchar_t*, int);
void strmods(wchar_t*, int);
void strcats(wchar_t*, wchar_t*);
int strrans(wchar_t*);
void strnex(wchar_t*);
void strnex_EX(wchar_t*, wchar_t);
void stradds(wchar_t*, wchar_t);
int strsans(wchar_t*);
double strsans2(wchar_t*);

//p1の先頭にp2があるかどうかを調べる
int strands(const wchar_t *p1, const wchar_t *p2) {
	for (int i = 0; i < 50; i++) {
		if (p2[i] == L'\0') {
			break;
		}
		if (p1[i] != p2[i]) {
			return 0;
		}
	}
	return 1;
}

//p1をp2にコピーする。cに1を入れると255桁まで初期化する(初期化推奨)
void strcopy(const wchar_t *p1, wchar_t *p2, int c) {
	int a = 0;
	for (int i = 0; i < 250; i++) {
		p2[i] = L'\0';
	}
	for (int i = 0; i < 250 && p1[i] != L'\0'; i++) {
		p2[i] = p1[i];
		p2[i + 1] = L'\0';
	}
	return;
}

//p1から先頭a文字を消す
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

//p1の最後にp2を付ける
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

int strrans(wchar_t *p1) {
	int a, b;
	strmods(p1, 2);
	a = strsans(p1);
	strnex_EX(p1, L',');
	b = mins(strsans(p1), a);
	return GetRand(b - a) + a;
}

//次の'/'か':'があるところまで消す
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

//指定の文字p3があるところまで消す
void strnex_EX(wchar_t *p1,wchar_t p3) {
	wchar_t *p2 = p1;
	short int a = 1;
	while (*p1 >= L' ' && *p1 <= L'}' && *p1 != p3 && *p1 != L'\0') {
		a++;
		p1++;
	}
	strmods(p2, a);
	return;
}

//p1の最後にaの文字を追加する
void stradds(wchar_t *p1, wchar_t a) {
	while (*p1 != L'\0') p1++;
	*p1++ = a;
	*p1 = L'\0';
	return;
}

//文字列を数字に変換にする
//途中に数字,マイナス以外の文字があったら終わり
int strsans(wchar_t *p1) {
	int a = 0, b = 1;
	if (*p1 == L'R') {

	}
	else {
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
	}
	return a * b;
}
//小数を可にしたもの
double strsans2(wchar_t *p1) {
	short int b = 1, c = 0, d = 99, i;
	double a = 0;
	if (*p1 == L'R') { return strrans(p1); }
	else {
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
}