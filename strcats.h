//p1�̍Ō��p2��t����
void strcats(wchar_t *p1, wchar_t *p2) {
	while (*p1 != L'\0') p1++;
	while (*p2 !=L'\0') {
		*p1 = *p2;
		p1++;
		p2++;
	}
	*p1 = L'\0';
	return;
}