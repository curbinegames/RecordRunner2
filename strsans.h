//文字列を数字に変換にする
//途中に数字,マイナス以外の文字があったら終わり
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

//小数を可にしたもの
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