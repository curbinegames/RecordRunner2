	//p1‚Ìæ“ª‚Ép2‚ª‚ ‚é‚©‚Ç‚¤‚©‚ğ’²‚×‚é
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