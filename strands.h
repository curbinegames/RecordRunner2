	//p1の先頭にp2があるかどうかを調べる
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