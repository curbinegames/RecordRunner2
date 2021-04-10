//p1‚©‚çæ“ªa•¶š‚ğÁ‚·
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