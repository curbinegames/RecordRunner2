int GetHighScore(wchar_t pas[255], int dif);
int GetRemainNotes(int *judghcount, int Notes);
int CalPosScore(int *score, int RemainNotes, int Notes, int combo, int MaxCombo);
void ShowCombo(int combo, int *pic);
void ShowBonusEff(int *judghcount, int EffStartTime, int *Snd, int *pic);
void RunningStats(int *judghcount, int Score, int HighScore);

int play3(int p, int n, int o, int shift, int AutoFlag) {
	/*---�p���`-----
	���[�U�[�p���ʃf�[�^: ���[�U�[����������ʃf�[�^�B���[�U�[�ɕ�����₷���B
	PC�p���ʃf�[�^: ���[�U�[�p���ʃf�[�^����v�Z�ō��ꂽ�APC��p�̕��ʃf�[�^�BPC�ɕ�����₷���B
	----------------*/
	//p: �p�b�N�i���o�[
	//n: �ȃi���o�[
	//o: ��Փx�i���o�[
	short int i[3];
	short int Lv = 0;
	short int notes = 0;
	short int bgp[3] = { 0,0,0 }; //[0:sky,1:ground,2:water]�̉��ʒu
	short int bgf[2] = { 0,0 }; //�������w�i��[0:���ʒu,1:�c�ʒu]
	short int charaput = 1; //�L�����̍��̈ʒu[0�ŏ�,1�Œ�,2�ŉ�]
	short int drop = 0;
	short int rank = 0;
	short int Clear = 0;
	short int KeyPushCount[7] = { 0,0,0,0,0,0,0 };
	int judgh = 0; //�m�[�c�̎��ԋ���
	int charahit = 0; //�L�������m�[�c������������ł��邩�ǂ����B[1�ȏ�Œ@�����A0�Œ@���ĂȂ�]
	int G[20], songT;
	int system[6] = { 0,0,0,2,0,0 };
	int noteoff = 0; //�m�[�c�̃I�t�Z�b�g
	int Etime = 0; //���ʂ̏I���̎���
	int Ntime = 0;
	int judghname[3][3]{ 0,0,0,0,0,0,0,0,0 };
	int holda = 0;
	int holdb = 0;
	int holdc = 0;
	int holdu = 0;
	int holdd = 0;
	int holdl = 0;
	int holdr = 0;
	int holdG = 0;
	int combo = 0;
	int AllNotesHitTime = -1;
	int LaneTrack[3] = { -150,-150,-150 };
	int Mcombo = 0;
	int Dscore[4] = { 0,0,0,0 }; //�����ɓ����镔��[���_�p,���_�ۑ��p,�����ۑ��p,���_��]
	int judghcount[4] = { 0,0,0,0 };
	int life = 500;
	int gap[30] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//gap = ���肸��
	int gapa[3] = { 0,0,0 };//gapa = ���肸��[���v, ��, 2��̍��v]
	int score2[4] = { 0,0,0,0 }; //[�ʏ�X�R�A(90000), �R���{�X�R�A(10000), ���_, ���v]
	int HighSrore; //�n�C�X�R�A
	int fps[62];//0�`59=1�t���[���Ԋu�̎���,60=���̑����,61=�O��̎���
	for (i[0] = 0; i[0] <= 59; i[0]++)fps[i[0]] = 17;
	fps[60] = 0;
	fps[61] = 0;
	double GD[5];
	int item[99]; //�A�C�e���̃A�h���X�ADrawGraph�ŌĂׂ�B
	short int itemN = 0; //���̔ԍ�
	int chamo[3][99][2]; //�L������[0:��,1:��,2:��]��[0:�O���t�B�b�N,1:�؂�ւ�����]
	short int chamoN[3] = { 0,0,0 }; //���̔ԍ�
	int fall[99][2]; //�������w�i��[0:�ԍ�,1:����]
	short int fallN = 0; //���̔ԍ�
	int Ymove[5][999][4]; //[��, ��, ��, (�n��), (����)]���[���c�ړ���[0:�J�n����,1:�ʒu,2:�I������,3:���]
	short int YmoveN[5] = { 0,0,0,0,0 }; //���̔ԍ�
	short int YmoveN2[3] = { 0,0,0 };
	int Xmove[3][999][4]; //[��, ��, ��]���[�����ړ���[0:�J�n����,1:�ʒu,2:�I������,3:���]
	short int XmoveN[3] = { 0,0,0 }; //���̔ԍ�
	int lock[2][2][99]; //lock = [��,�c]�̉����̈ʒu��[(1=�Œ肷��,-1�ȊO=�Œ肵�Ȃ�),����]
	short int lockN[2] = { 0,0 }; //���̔ԍ�
	int carrow[2][99];
	short int carrowN = 0;
	int viewT[2][99];//[�����\������,���s����,[0]=���i���o�[]
	short int viewTN = 0;
	int Movie[14][999];//�A�C�e���\��[�A�C�e���ԍ�,�ړ��`��,�J�n����,�I������,�J�nx�ʒu,�I��x�ʒu,�J�ny�ʒu,�I��y�ʒu,�J�n�T�C�Y,�I���T�C�Y,�J�n�p�x,�I���p�x,�J�n�����x,�I�������x]
	short int MovieN = 0;
	int object[3][5][999]; //[��,��,��]���[���̉�����[����,���,(�g���ĂȂ�),�c�ʒu,���ʒu]
	short int objectN[3] = { 0,0,0 }; //���̔ԍ�
	int difkey[50][4];//��Փx�v�Z�Ɏg��[�ԍ�][���̓L�[,����,��Փx�_,[0]�����:[1]���̔ԍ�:[2]1�O�̔ԍ�:[3]2�O�̔ԍ�:[4]�ō��_:[5]�f�[�^��:[6]�Ō�50�̍��v:[7]�v�Z���珜�O���鎞��]
	int ddif[25] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//�e��Ԃ̓�Փx
	int ddifG[2] = { 1,25 };//0=�������Ԕԍ�(1�`25),1=�ő�l
	int Yline[5] = { 300,350,400,350,600 };//[��,��,��,�n��,����]���[���̏c�ʒu
	int Xline[3] = { 150,150,150 };//[��,��,��]���[���̉��ʒu
	double rate, SumRate[2] = { 0,0 }, bpm = 120, bpmG = 120;
	double timer[3]; //[��, ��, ��]���[���̎���
	double speedt[5][99][2]; //[��, ��, ��, (�n��), (����)]���[����[0:�؂�ւ�����,1:���x]
	double DifRate; //���ʒ萔
	short int speedN[5] = { 0,0,0,0,0 }; //���̔ԍ�
	char key[256];
	wchar_t songN[255];
	wchar_t songNE[255];
	wchar_t fileN[255];
	wchar_t dataE[255] = L"record/";
	wchar_t RRS[255]; //PC�p���ʃf�[�^�̕ۑ��ꏊ
	wchar_t mp3FN[255] = L"song/";
	wchar_t DataFN[255] = L"score/";
	wchar_t skyFN[255] = L"picture/backskynoamal.png";
	wchar_t groundFN[255] = L"picture/groundnaturenormal.png";
	wchar_t waterFN[255] = L"picture/waternormal.png";
	wchar_t DifFN[255] = L"picture/difanother.png";
	wchar_t GT1[255];
	wchar_t GT2[255];
	wchar_t GT3[] = L".png";
	wchar_t GT4[255];
	wchar_t ST1[] = L"record/";
	wchar_t ST2[] = L"/list.txt";
	wchar_t ST3[] = L".dat";
	wchar_t GT26[6][7] = { L"/0.rrs" ,L"/1.rrs" ,L"/2.rrs" ,L"/3.rrs" ,L"/4.rrs" ,L"/5.rrs" };
	unsigned int Cr, Crb;
	Cr = GetColor(255, 255, 255);
	Crb = GetColor(0, 0, 0);
	FILE *fp;
	//�V�X�e�����[�h
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&system, sizeof(int), 6, fp);
		fclose(fp);
	}
	songT = FileRead_open(L"RecordPack.txt");
	for (i[0] = 0; i[0] <= p; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcats(dataE, GT1);
	stradds(dataE, L'/');
	strcopy(ST1, GT2, 1);
	strcats(GT2, GT1);
	strcats(GT2, ST2);
	songT = FileRead_open(GT2);
	for (i[0] = 0; i[0] <= n; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcopy(GT1, fileN, 1);
	strcats(dataE, GT1);
	strcopy(dataE, GT1, 1);
	strcats(GT1, GT26[o]);
	G[2] = -1;
	if (shift == 0) {
		G[2] = _wfopen_s(&fp, GT1, L"rb");//rrs�f�[�^��ǂݍ���
	}
	if (G[2] != 0) {
		RecordLoad2(p, n, o);//rrs�f�[�^�������A�܂��͍쐬�̎w��������΍��
		G[2] = _wfopen_s(&fp, GT1, L"rb");//rrs�f�[�^��ǂݍ���
	}
	if (G[2] == 0) {
		fread(&mp3FN, 255, 1, fp);//���y�t�@�C����
		fread(&bpm, sizeof(double), 1, fp);//BPM
		fread(&noteoff, sizeof(int), 1, fp);//offset
		fread(&skyFN, 255, 1, fp);//��w�i��
		fread(&groundFN, 255, 1, fp);//�n�ʉ摜��
		fread(&waterFN, 255, 1, fp);//�����摜��
		fread(&songN, 255, 1, fp);//�Ȗ�
		fread(&songNE, 255, 1, fp);//�Ȗ�(�p��)
		fread(&Lv, sizeof(short int), 1, fp);//���x��
		//fread(&item, sizeof(int), 99, fp);//�A�C�e���摜�f�[�^(���얢�m�F)
		fread(&fall, sizeof(int), 198, fp);//�������w�i�؂�ւ��^�C�~���O
		fread(&speedt, sizeof(double), 990, fp);//���[�����x
		fread(&chamo, sizeof(int), 594, fp);//�L�����O���ϊ��^�C�~���O
		fread(&Ymove, sizeof(int), 19980, fp);//�c�ʒu�ړ��^�C�~���O
		fread(&Xmove, sizeof(int), 11988, fp);//���ʒu�ړ��^�C�~���O
		fread(&lock, sizeof(int), 396, fp);//�m�[�c�Œ�؂�ւ��^�C�~���O
		fread(&carrow, sizeof(int), 198, fp);//�L���������؂�ւ��^�C�~���O
		fread(&viewT, sizeof(int), 198, fp);//�m�[�c�\�����ԕϊ��^�C�~���O
		fread(&object, sizeof(int), 14985, fp);//�m�[�c�f�[�^
		fread(&notes, sizeof(short int), 1, fp);//�m�[�c��
		fread(&Etime, sizeof(int), 1, fp);//�ȏI������
		fread(&G, sizeof(int), 2, fp);
		difkey[4][3] = G[0];//�ō���Փx
		difkey[6][3] = G[1];//�ŏI��Փx
		fread(&ddif, sizeof(int), 25, fp);//�e��ԓ�Փx�f�[�^
		fread(&ddifG, sizeof(int), 2, fp);//�e��ԓ�Փx�f�[�^
		fread(&DifFN, 255, 1, fp);//��Փx�o�[��
		fread(&Movie, sizeof(int), 13986, fp);//����f�[�^
	}
	fclose(fp);
	strcats(DataFN, fileN);
	strcats(DataFN, ST3);
	//�n�C�X�R�A�擾
	HighSrore = GetHighScore(DataFN, o);
	//�O���t�B�b�N�ƌ��ʉ��̏���
	/*�O���t�B�b�N�p�ϐ�
	judghimg = ����}�[�N
	hitimg = hit
	hitcimg = hit�̎���̐F
	catchimg = catch
	upimg = up
	downimg = down
	leftimg = left
	rightimg = right
	bombimg = bomb
	goustimg = goust
	backskyimg
	backgroundimg
	dangerimg
	charaimg
	musicmp3 = ���y*/
	int judghimg, hitimg, hitcimg, catchimg, upimg, downimg, leftimg, rightimg, bombimg, goustimg;
	int backskyimg, backgroundimg, backwaterimg, dangerimg, dropimg, resultimg, sbarimg, sbbarimg, filterimg, charaguideimg, gapbarimg, gaplineimg, difberimg;
	int Lbarimg[3];
	int Tbarimg[2];
	int	charaimg[30];
	int rankimg[6];
	int coleimg[5];
	int effimg[7][5];
	int KeyViewimg[2];
	int Bonusimg[3] = { LoadGraph(L"picture/PERFECT.png"),
		LoadGraph(L"picture/FULLCOMBO.png"),
		LoadGraph(L"picture/NOMISS.png") };
	int Rchaimg;
	int ComboFontimg[10];
	int musicmp3, attack, catchs, arrow, bomb;
	int BonusSnd[3] = { LoadSoundMem(L"sound/a-perfect.mp3"),
		LoadSoundMem(L"sound/a-fullcombo.mp3"),
		LoadSoundMem(L"sound/a-nomiss.mp3") };
	judghimg = LoadGraph(L"picture/Marker.png");
	hitimg = LoadGraph(L"picture/hit.png");
	hitcimg = LoadGraph(L"picture/hitc.png");
	catchimg = LoadGraph(L"picture/catch.png");
	upimg = LoadGraph(L"picture/up.png");
	downimg = LoadGraph(L"picture/down.png");
	leftimg = LoadGraph(L"picture/left.png");
	rightimg = LoadGraph(L"picture/right.png");
	bombimg = LoadGraph(L"picture/bomb.png");
	goustimg = LoadGraph(L"picture/goust.png");
	switch (o) {
	case 0:
		difberimg = LoadGraph(L"picture/difauto.png");
		break;
	case 1:
		difberimg = LoadGraph(L"picture/difeasy.png");
		break;
	case 2:
		difberimg = LoadGraph(L"picture/difnormal.png");
		break;
	case 3:
		difberimg = LoadGraph(L"picture/difhard.png");
		break;
	case 4:
	case 5:
		difberimg = LoadGraph(DifFN);
		break;
	}
	backskyimg = LoadGraph(skyFN);
	backgroundimg = LoadGraph(groundFN);
	backwaterimg = LoadGraph(waterFN);
	dangerimg = LoadGraph(L"picture/danger.png");
	dropimg = LoadGraph(L"picture/drop.png");
	resultimg = LoadGraph(L"picture/result.png");
	sbarimg = LoadGraph(L"picture/scoreber.png");
	sbbarimg = LoadGraph(L"picture/scoreber2.png");
	Lbarimg[0] = LoadGraph(L"picture/LIFEbar.png");
	Lbarimg[1] = LoadGraph(L"picture/LIFEbar2.png");
	Lbarimg[2] = LoadGraph(L"picture/LIFEbar3.png");
	Tbarimg[0] = LoadGraph(L"picture/TIMEbar.png");
	Tbarimg[1] = LoadGraph(L"picture/TIMEbar2.png");
	rankimg[0] = LoadGraph(L"picture/rankEX.png");
	rankimg[1] = LoadGraph(L"picture/rankS.png");
	rankimg[2] = LoadGraph(L"picture/rankA.png");
	rankimg[3] = LoadGraph(L"picture/rankB.png");
	rankimg[4] = LoadGraph(L"picture/rankC.png");
	rankimg[5] = LoadGraph(L"picture/rankD.png");
	coleimg[0] = LoadGraph(L"picture/DROPED.png");
	coleimg[1] = LoadGraph(L"picture/CLEARED.png");
	coleimg[2] = LoadGraph(L"picture/NOMISS.png");
	coleimg[3] = LoadGraph(L"picture/FULLCOMBO.png");
	coleimg[4] = LoadGraph(L"picture/PERFECT.png");
	filterimg = LoadGraph(L"picture/Black.png");
	charaguideimg = LoadGraph(L"picture/Cguide.png");
	gapbarimg = LoadGraph(L"picture/GapBer.png");
	gaplineimg = LoadGraph(L"picture/GapBerLine.png");
	LoadDivGraph(L"picture/hiteff.png", 5, 5, 1, 50, 50, effimg[0]);
	LoadDivGraph(L"picture/hiteff.png", 5, 5, 1, 50, 50, effimg[1]);
	LoadDivGraph(L"picture/upeff.png", 5, 5, 1, 50, 50, effimg[2]);
	LoadDivGraph(L"picture/downeff.png", 5, 5, 1, 50, 50, effimg[3]);
	LoadDivGraph(L"picture/lefteff.png", 5, 5, 1, 50, 50, effimg[4]);
	LoadDivGraph(L"picture/righteff.png", 5, 5, 1, 50, 50, effimg[5]);
	LoadDivGraph(L"picture/bombeff.png", 5, 5, 1, 50, 50, effimg[6]);
	LoadDivGraph(L"picture/NumberComboBlue.png", 10, 5, 2, 80, 100, ComboFontimg);
	for (i[0] = L'0'; i[0] <= L'9'; i[0]++) {
		for (i[1] = L'0'; i[1] <= L'9'; i[1]++) {
			for (i[2] = L'0'; i[2] <= L'9'; i[2]++) {
				strcopy(dataE, GT2, 1);
				stradds(GT2, L'/');
				stradds(GT2, i[0]);
				stradds(GT2, i[1]);
				stradds(GT2, i[2]);
				strcats(GT2, GT3);
				item[itemN] = LoadGraph(GT2);
				if (item[itemN] == -1) { break; }
				itemN++;
			}
			if (item[itemN] == -1) { break; }
		}
		if (item[itemN] == -1) { break; }
	}
	if (system[5]) {
		KeyViewimg[0] = LoadGraph(L"picture/KeyViewOff.png");
		KeyViewimg[1] = LoadGraph(L"picture/KeyViewOn.png");
	}
	switch (system[0]) {
	case 0:
		LoadDivGraph(L"picture/Picker.png", 24, 6, 4, 160, 160, charaimg);
		Rchaimg = LoadGraph(L"picture/RePicker.png");
		break;
	case 1:
		LoadDivGraph(L"picture/Gator.png", 24, 6, 4, 160, 160, charaimg);
		Rchaimg = LoadGraph(L"picture/ReGator.png");
		break;
	case 2:
		LoadDivGraph(L"picture/Taylor.png", 24, 6, 4, 160, 160, charaimg);
		Rchaimg = LoadGraph(L"picture/ReTaylor.png");
		break;
	}
	musicmp3 = LoadSoundMem(mp3FN);
	if (system[2] == 0) {
		attack = LoadSoundMem(L"sound/attack.mp3");
		catchs = LoadSoundMem(L"sound/catch.mp3");
		arrow = LoadSoundMem(L"sound/arrow.ogg");
		bomb = LoadSoundMem(L"sound/bomb.wav");
	}
	else {
		attack = LoadSoundMem(L"sound/non.wav");
		catchs = LoadSoundMem(L"sound/non.wav");
		arrow = LoadSoundMem(L"sound/non.wav");
		bomb = LoadSoundMem(L"sound/non.wav");
	}
	//�Q�[���J�n�O�̉�����
	notes = notzero(notes);
	GD[0] = difkey[4][3] / 100.0 - Lv;//mdif�Ɠ�Փx�\�L�̍�
	if (Lv == 0) { DifRate = 0; }
	else if (2 <= GD[0]) { DifRate = Lv + 0.9; }
	else if (0 <= GD[0] && GD[0] < 2) { DifRate = Lv + 0.45 * GD[0]; }
	else { DifRate = difkey[4][3] / 100.0; }
	PlaySoundMem(musicmp3, DX_PLAYTYPE_BACK);
	WaitTimer(10);
	int Stime = GetNowCount();
	//�Q�[���J�n
	while (1) {
		ClearDrawScreen();
		GetHitKeyStateAll(key);
		//�w�i�\��
		if (system[3] != 0) {
			//�w�i�̉��ʒu�v�Z
			if (speedt[3][speedN[3] + 1][0] < Ntime && speedt[3][speedN[3] + 1][0] >= 0) speedN[3]++;
			bgp[0] -= speedt[3][speedN[3]][1];
			if (bgp[0] < -700) bgp[0] += 640;
			else if (bgp[0] > 0) bgp[0] -= 640;
			bgp[1] -= speedt[3][speedN[3]][1] * 5;
			if (bgp[1] < -700) bgp[1] += 640;
			else if (bgp[1] > 0) bgp[1] -= 640;
			bgp[2] -= speedt[4][speedN[4]][1] * 5;
			if (bgp[2] < -700) bgp[2] += 640;
			else if (bgp[2] > 0) bgp[2] -= 640;
			//�w�i�̏c�ʒu�v�Z
			for (i[0] = 3; i[0] <= 4; i[0]++) {
				if (Ntime >= Ymove[i[0]][YmoveN[i[0]]][0] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0]) {
					switch (Ymove[i[0]][YmoveN[i[0]]][3]) {
					case(1):
						Yline[i[0]] = lins(Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ntime);
						break;
					case(2):
						Yline[i[0]] = pals(Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ntime);
						break;
					case(3):
						Yline[i[0]] = pals(Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ntime);
						break;
					}
				}
				if (Ntime >= Ymove[i[0]][YmoveN[i[0]]][2] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0] || Ymove[i[0]][YmoveN[i[0]]][3] == 4) Yline[i[0]] = Ymove[i[0]][YmoveN[i[0]]++][1];
			}
			//�w�i�\��
			for (i[0] = 0; i[0] < 2; i[0]++) {
				G[0] = bgp[i[0]];
				while (G[0] < 700) {
					switch (i[0]) {
					case(0):
						DrawGraph(G[0], Yline[3] / 5 - 160, backskyimg, TRUE);
						break;
					case(1):
						DrawGraph(G[0], Yline[3] - 400, backgroundimg, TRUE);
						DrawGraph(G[0], Yline[4] - 400, backwaterimg, TRUE);
						break;
					}
					G[0] += 640;
				}
			}
			//�������w�i�\��
			if (Ntime >= fall[fallN + 1][0] && fall[fallN + 1][0] >= 0) fallN++;
			if (fall[fallN][1] >= 0) {
				G[0] = bgf[0];//��
				G[1] = bgf[1] + Yline[3];//�c
				for (i[0] = 0; i[0] < 2; i[0]++) for (i[1] = 0; i[1] < 3; i[1]++) DrawGraph(G[0] + i[0] * 640, G[1] - i[1] * 480, item[fall[fallN][1]], TRUE);
				bgf[0] -= 5;
				bgf[1] += 2;
			}
			if (bgf[0] <= -640)bgf[0] += 640;
			if (bgf[1] >= 640)bgf[1] -= 480;
		}
		//�t�B���^�[�\��
		switch (system[3]) {
		case 1:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 170);
			DrawGraph(0, 0, filterimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			break;
		case 2:
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 85);
			DrawGraph(0, 0, filterimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			break;
		}
		//����\��
		if (system[3] != 0) {
			while (Movie[3][MovieN] < Ntime && Movie[3][MovieN]>-500) { MovieN++; }
			G[0] = 0;
			while (Movie[3][MovieN + G[0]] > -500) {
				if (Movie[2][MovieN + G[0]] > Ntime || Movie[3][MovieN + G[0]] < Ntime) {
					G[0]++;
					continue;
				}
				GetGraphSize(item[Movie[0][MovieN + G[0]]], &G[1], &G[2]);
				G[1] /= 2;
				G[2] /= 2;
				switch (Movie[1][MovieN + G[0]]) {
				case 1:
					G[3] = lins(Movie[2][MovieN + G[0]], Movie[4][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[5][MovieN + G[0]], Ntime);
					G[4] = lins(Movie[2][MovieN + G[0]], Movie[6][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[7][MovieN + G[0]], Ntime);
					G[5] = lins(Movie[2][MovieN + G[0]], Movie[8][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[9][MovieN + G[0]], Ntime);
					G[6] = lins(Movie[2][MovieN + G[0]], Movie[10][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[11][MovieN + G[0]], Ntime);
					G[7] = lins(Movie[2][MovieN + G[0]], Movie[12][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[13][MovieN + G[0]], Ntime);
					break;
				case 2:
					G[3] = pals(Movie[2][MovieN + G[0]], Movie[4][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[5][MovieN + G[0]], Ntime);
					G[4] = pals(Movie[2][MovieN + G[0]], Movie[6][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[7][MovieN + G[0]], Ntime);
					G[5] = pals(Movie[2][MovieN + G[0]], Movie[8][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[9][MovieN + G[0]], Ntime);
					G[6] = pals(Movie[2][MovieN + G[0]], Movie[10][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[11][MovieN + G[0]], Ntime);
					G[7] = pals(Movie[2][MovieN + G[0]], Movie[12][MovieN + G[0]], Movie[3][MovieN + G[0]], Movie[13][MovieN + G[0]], Ntime);
					break;
				case 3:
					G[3] = pals(Movie[3][MovieN + G[0]], Movie[5][MovieN + G[0]], Movie[2][MovieN + G[0]], Movie[4][MovieN + G[0]], Ntime);
					G[4] = pals(Movie[3][MovieN + G[0]], Movie[7][MovieN + G[0]], Movie[2][MovieN + G[0]], Movie[6][MovieN + G[0]], Ntime);
					G[5] = pals(Movie[3][MovieN + G[0]], Movie[9][MovieN + G[0]], Movie[2][MovieN + G[0]], Movie[8][MovieN + G[0]], Ntime);
					G[6] = pals(Movie[3][MovieN + G[0]], Movie[11][MovieN + G[0]], Movie[2][MovieN + G[0]], Movie[10][MovieN + G[0]], Ntime);
					G[7] = pals(Movie[3][MovieN + G[0]], Movie[13][MovieN + G[0]], Movie[2][MovieN + G[0]], Movie[12][MovieN + G[0]], Ntime);
					break;
				}
				G[7] = betweens(0, G[7], 255);
				G[8] = -G[1]; G[9] = -G[2];
				G[10] = G[1]; G[11] = -G[2];
				G[12] = G[1]; G[13] = G[2];
				G[14] = -G[1]; G[15] = G[2];
				for (i[0] = 8; i[0] < 16; i[0]++) { G[i[0]] = G[i[0]] * G[5] / 100; }
				for (i[0] = 8; i[0] < 16; i[0] += 2) {
					G[16] = G[i[0]];
					G[i[0]] = G[i[0]] * cosC(G[6]) - G[i[0] + 1] * sinC(G[6]) + G[3];
					G[i[0] + 1] = G[16] * sinC(G[6]) + G[i[0] + 1] * cosC(G[6]) + G[4];
				}
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, G[7]);
				DrawModiGraph(G[8], G[9], G[10], G[11], G[12], G[13], G[14], G[15], item[Movie[0][MovieN + G[0]]], TRUE);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				G[0]++;
			}
		}
		//�L�����ʒu�K�C�h�̕\��
		if (carrow[1][carrowN + 1] < Ntime && carrow[1][carrowN + 1] >= 0) carrowN++;
		if (carrow[0][carrowN] == 1) { DrawGraph(Xline[charaput] - 4, Yline[charaput] - 4, charaguideimg, TRUE); }
		else { DrawTurnGraph(Xline[charaput] - 56, Yline[charaput] - 4, charaguideimg, TRUE); }
		//����}�[�J�[�̕\��
		for (i[0] = 0; i[0] < 3; i[0]++) {
			//�c�ړ�
			if (Ntime >= Ymove[i[0]][YmoveN[i[0]]][0] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0]) {
				switch (Ymove[i[0]][YmoveN[i[0]]][3]) {
				case(1):
					Yline[i[0]] = lins(Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ntime);
					break;
				case(2):
					Yline[i[0]] = pals(Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ntime);
					break;
				case(3):
					Yline[i[0]] = pals(Ymove[i[0]][YmoveN[i[0]]][2], Ymove[i[0]][YmoveN[i[0]]][1], Ymove[i[0]][YmoveN[i[0]]][0], Ymove[i[0]][YmoveN[i[0]] - 1][1], Ntime);
					break;
				}
			}
			if (Ntime >= Ymove[i[0]][YmoveN[i[0]]][2] && 0 <= Ymove[i[0]][YmoveN[i[0]]][0] || Ymove[i[0]][YmoveN[i[0]]][3] == 4) Yline[i[0]] = Ymove[i[0]][YmoveN[i[0]]++][1];
			//���ړ�
			if (Ntime >= Xmove[i[0]][XmoveN[i[0]]][0] && 0 <= Xmove[i[0]][XmoveN[i[0]]][0]) {
				switch (Xmove[i[0]][XmoveN[i[0]]][3]) {
				case(1):
					Xline[i[0]] = lins(Xmove[i[0]][XmoveN[i[0]]][0], Xmove[i[0]][XmoveN[i[0]] - 1][1], Xmove[i[0]][XmoveN[i[0]]][2], Xmove[i[0]][XmoveN[i[0]]][1], Ntime);
					break;
				case(2):
					Xline[i[0]] = pals(Xmove[i[0]][XmoveN[i[0]]][0], Xmove[i[0]][XmoveN[i[0]] - 1][1], Xmove[i[0]][XmoveN[i[0]]][2], Xmove[i[0]][XmoveN[i[0]]][1], Ntime);
					break;
				case(3):
					Xline[i[0]] = pals(Xmove[i[0]][XmoveN[i[0]]][2], Xmove[i[0]][XmoveN[i[0]]][1], Xmove[i[0]][XmoveN[i[0]]][0], Xmove[i[0]][XmoveN[i[0]] - 1][1], Ntime);
					break;
				}
			}
			if (Ntime >= Xmove[i[0]][XmoveN[i[0]]][2] && 0 <= Xmove[i[0]][XmoveN[i[0]]][0] || Xmove[i[0]][XmoveN[i[0]]][3] == 4) Xline[i[0]] = Xmove[i[0]][XmoveN[i[0]]++][1];
			DrawGraph(Xline[i[0]], Yline[i[0]], judghimg, TRUE);
		}
		//�L�����O���ϊ�
		for (i[0] = 0; i[0] < 3; i[0]++)while (Ntime >= chamo[i[0]][chamoN[i[0]] + 1][1] && chamo[i[0]][chamoN[i[0]] + 1][1] >= 0) {
			chamoN[i[0]]++;
		}
		G[3] = 0;
		//�L�����ʒu����
		for (i[0] = 0; i[0] <= 2; i[0]++) if (object[i[0]][0][objectN[i[0]]] <= Ntime + 16 && object[i[0]][1][objectN[i[0]]] == 2) G[3] = 1;
		for (i[0] = 0; i[0] <= 2; i[0]++) {
			G[i[0]] = objectN[i[0]] - 1;
			while (object[i[0]][1][G[i[0]]] == 8 && G[i[0]] >= 1) G[i[0]]--;
		}
		//�ド�C���̃q�b�g�}�[�J�[���������Ă���Ƃ�
		if (charahit > 0 && G[3] == 0 && object[0][0][G[0]] > object[1][0][G[1]] && object[0][0][G[0]] > object[2][0][G[2]] && object[0][0][G[0]] + 750 > Ntime && object[0][1][G[0]] == 1 && judghname[0][0] >= 1 && judghname[0][0] <= 3) {
			G[4] = Yline[0];
			charaput = 0;
		}
		//�����C���̃q�b�g�}�[�J�[���������Ă���Ƃ�
		else if (charahit > 0 && G[3] == 0 && object[1][0][G[1]] > object[0][0][G[0]] && object[1][0][G[1]] > object[2][0][G[2]] && object[1][0][G[1]] + 750 > Ntime && object[1][1][G[1]] == 1 && judghname[1][0] >= 1 && judghname[1][0] <= 3) {
			G[4] = Yline[1];
			charaput = 1;
		}
		//�����C���̃q�b�g�}�[�J�[���������Ă���Ƃ�
		else if (charahit > 0 && G[3] == 0 && object[2][0][G[2]] > object[0][0][G[0]] && object[2][0][G[2]] > object[1][0][G[1]] && object[2][0][G[2]] + 750 > Ntime && object[2][1][G[2]] == 1 && judghname[2][0] >= 1 && judghname[2][0] <= 3) {
			G[4] = Yline[2];
			charaput = 2;
		}
		//�オ������āA���O�̃q�b�g�}�[�J�[���������Ă��Ȃ��Ƃ�
		else if (holdu >= 1 && holdd == 0) {
			G[4] = Yline[0];
			charaput = 0;
		}
		//����������āA���O�̃q�b�g�}�[�J�[���������Ă��Ȃ��Ƃ�
		else if (holdu == 0 && holdd >= 1) {
			G[4] = Yline[2];
			charaput = 2;
		}
		//����������Ă��Ȃ����A�㉺�����ɉ�����Ă��āA���O�̃q�b�g�}�[�J�[���������Ă��Ȃ��Ƃ�
		else {
			G[4] = Yline[1];
			charaput = 1;
		}
		//�L�����O���t�B�b�N��\��
		if (GetNowCount() - charahit > 250) G[5] = 0;
		else G[5] = pals(250, 0, 0, 50, GetNowCount() - charahit);
		if (charahit > 0) {
			if (carrow[0][carrowN] == 1) DrawGraph(Xline[charaput] + G[5] - 160, G[4] - 75, charaimg[maxs(mins((GetNowCount() - charahit) / 125 + 18, 18), 23)], TRUE);
			else DrawTurnGraph(Xline[charaput] - G[5] + 30, G[4] - 75, charaimg[maxs(mins((GetNowCount() - charahit) / 125 + 18, 18), 23)], TRUE);
		}
		else {
			if (carrow[0][carrowN] == 1) DrawGraph(Xline[charaput] - 160, G[4] - 75, charaimg[Ntime * int(bpm) / 20000 % 6 + chamo[charaput][chamoN[charaput]][0] * 6], TRUE);
			else DrawTurnGraph(Xline[0] + 30, G[4] - 75, charaimg[Ntime * int(bpm) / 20000 % 6 + chamo[charaput][chamoN[charaput]][0] * 6], TRUE);
		}
		//�L�[�ݒ�
		if (AutoFlag == 0) {
			if (key[KEY_INPUT_Z] == 0) holda = 0;
			else if (key[KEY_INPUT_Z] == 1) holda++;
			if (key[KEY_INPUT_X] == 0) holdb = 0;
			else if (key[KEY_INPUT_X] == 1) holdb++;
			if (key[KEY_INPUT_C] == 0) holdc = 0;
			else if (key[KEY_INPUT_C] == 1) holdc++;
			if (key[KEY_INPUT_UP] == 0) holdu = 0;
			else if (key[KEY_INPUT_UP] == 1) holdu++;
			if (key[KEY_INPUT_LEFT] == 0) holdl = 0;
			else if (key[KEY_INPUT_LEFT] == 1) holdl++;
			if (key[KEY_INPUT_RIGHT] == 0) holdr = 0;
			else if (key[KEY_INPUT_RIGHT] == 1) holdr++;
			if (key[KEY_INPUT_DOWN] == 0) holdd = 0;
			else if (key[KEY_INPUT_DOWN] == 1) holdd++;
		}
		//�I�[�g�v���C�p�R�[�h
		else if (AutoFlag == 1) {
			if (holdc >= 1) { holdc++; }
			if (holda >= 1) { holda++; }
			if (holdb >= 1) { holdb++; }
			if (holdu >= 1) { holdu++; }
			if (holdd >= 1) { holdd++; }
			if (holdl >= 1) { holdl++; }
			if (holdr >= 1) { holdr++; }
			//�c�A�O�{�^������
			if (object[0][1][objectN[0]] == 3 && object[0][0][objectN[0]] - Ntime <= 40 ||
				object[1][1][objectN[1]] == 3 && object[1][0][objectN[1]] - Ntime <= 40 ||
				object[2][1][objectN[2]] == 3 && object[2][0][objectN[2]] - Ntime <= 40) {
				holdu = 0;
			}
			if (object[0][1][objectN[0]] == 4 && object[0][0][objectN[0]] - Ntime <= 40 ||
				object[1][1][objectN[1]] == 4 && object[1][0][objectN[1]] - Ntime <= 40 ||
				object[2][1][objectN[2]] == 4 && object[2][0][objectN[2]] - Ntime <= 40) {
				holdd = 0;
			}
			if (object[0][1][objectN[0]] == 5 && object[0][0][objectN[0]] - Ntime <= 40 ||
				object[1][1][objectN[1]] == 5 && object[1][0][objectN[1]] - Ntime <= 40 ||
				object[2][1][objectN[2]] == 5 && object[2][0][objectN[2]] - Ntime <= 40) {
				holdl = 0;
			}
			if (object[0][1][objectN[0]] == 6 && object[0][0][objectN[0]] - Ntime <= 40 ||
				object[1][1][objectN[1]] == 6 && object[1][0][objectN[1]] - Ntime <= 40 ||
				object[2][1][objectN[2]] == 6 && object[2][0][objectN[2]] - Ntime <= 40) {
				holdr = 0;
			}
			//�q�b�g�m�[�c����
			G[0] = 0;
			if (object[0][1][objectN[0]] == 1 && object[0][0][objectN[0]] - Ntime <= 8) { G[0]++; }
			if (object[1][1][objectN[1]] == 1 && object[1][0][objectN[1]] - Ntime <= 8) { G[0]++; }
			if (object[2][1][objectN[2]] == 1 && object[2][0][objectN[2]] - Ntime <= 8) { G[0]++; }
			if (G[0] == 1) {
				if (holdc == 0) {
					holdc = 1;
					holda = 0;
					holdb = 0;
				}
				else {
					holda = 1;
					holdc = 0;
					holdb = 0;
				}
			}
			else if (G[0] == 2) {
				if (holdc == 0) {
					holdc = 1;
					holdb = 1;
					holda = 0;
				}
				else {
					holda = 1;
					holdb = 1;
					holdc = 0;
				}
			}
			else if (G[0] == 3) {
				holda = 1;
				holdb = 1;
				holdc = 1;
			}
			//���E�A���[�m�[�c����
			G[0] = 0;
			if (object[0][1][objectN[0]] == 5 && object[0][0][objectN[0]] - Ntime <= 8 ||
				object[1][1][objectN[1]] == 5 && object[1][0][objectN[1]] - Ntime <= 8 ||
				object[2][1][objectN[2]] == 5 && object[2][0][objectN[2]] - Ntime <= 8) {
				if (G[0] == 0) {
					holdu = 0;
					holdd = 0;
					holdl = 0;
					holdr = 0;
				}
				G[0] = 1;
				holdl = 1;
			}
			if (object[0][1][objectN[0]] == 6 && object[0][0][objectN[0]] - Ntime <= 8 ||
				object[1][1][objectN[1]] == 6 && object[1][0][objectN[1]] - Ntime <= 8 ||
				object[2][1][objectN[2]] == 6 && object[2][0][objectN[2]] - Ntime <= 8) {
				if (G[0] == 0) {
					holdu = 0;
					holdd = 0;
					holdl = 0;
					holdr = 0;
				}
				G[0] = 1;
				holdr = 1;
			}
			//�{���m�[�c����
			if (object[0][1][objectN[0]] == 7 && object[0][0][objectN[0]] - Ntime <= 40) { holdu = 0; }
			if (object[2][1][objectN[2]] == 7 && object[2][0][objectN[2]] - Ntime <= 40) { holdd = 0; }
			if (object[1][1][objectN[1]] == 7 && object[1][0][objectN[1]] - Ntime <= 40) {
				if (object[0][1][objectN[0]] == 7 && object[0][0][objectN[0]] - Ntime <= 40 ||
					object[2][1][objectN[2]] == 2 && object[2][0][objectN[2]] - Ntime <= 40) {
					holdu = 0; holdd++;
				}
				else { holdu++; holdd = 0; }
			}
			//�㉺�A���[�m�[�c����
			if (object[0][1][objectN[0]] == 3 && object[0][0][objectN[0]] - Ntime <= 8 ||
				object[1][1][objectN[1]] == 3 && object[1][0][objectN[1]] - Ntime <= 8 ||
				object[2][1][objectN[2]] == 3 && object[2][0][objectN[2]] - Ntime <= 8) {
				if (G[0] == 0) {
					holdu = 0;
					holdd = 0;
					holdl = 0;
					holdr = 0;
				}
				G[0] = 1;
				holdu = 1;
				if (object[0][1][objectN[0]] == 7 && object[0][0][objectN[0]] - Ntime <= 40) { holdd = 1; }
			}
			if (object[0][1][objectN[0]] == 4 && object[0][0][objectN[0]] - Ntime <= 8 ||
				object[1][1][objectN[1]] == 4 && object[1][0][objectN[1]] - Ntime <= 8 ||
				object[2][1][objectN[2]] == 4 && object[2][0][objectN[2]] - Ntime <= 8) {
				if (G[0] == 0) {
					holdu = 0;
					holdd = 0;
					holdl = 0;
					holdr = 0;
				}
				G[0] = 1;
				holdd = 1;
				if (object[2][1][objectN[2]] == 7 && object[2][0][objectN[2]] - Ntime <= 40) { holdu = 1; }
			}
			//�L���b�`�m�[�c����
			if (object[0][1][objectN[0]] == 2 && object[0][0][objectN[0]] - Ntime <= 8) { holdu++; holdd = 0; }
			if (object[1][1][objectN[1]] == 2 && object[1][0][objectN[1]] - Ntime <= 8) { holdu = 0; holdd = 0; }
			if (object[2][1][objectN[2]] == 2 && object[2][0][objectN[2]] - Ntime <= 8) { holdu = 0; holdd++; }
			if (holdc > 10) { holdc = 0; }
			if (holda > 10) { holda = 0; }
			if (holdb > 10) { holdb = 0; }
			if (holdu > 10 &&
				object[0][1][objectN[0]] != 2 && object[1][1][objectN[1]] != 2 && object[2][1][objectN[2]] != 2 &&
				object[0][1][objectN[0]] != 7 && object[1][1][objectN[1]] != 7 && object[2][1][objectN[2]] != 7) {
				holdu = 0;
			}
			if (holdd > 10 &&
				object[0][1][objectN[0]] != 2 && object[1][1][objectN[1]] != 2 && object[2][1][objectN[2]] != 2 &&
				object[0][1][objectN[0]] != 7 && object[1][1][objectN[1]] != 7 && object[2][1][objectN[2]] != 7) {
				holdd = 0;
			}
			if (holdl > 10) { holdl = 0; }
			if (holdr > 10) { holdr = 0; }
		}
		if (key[KEY_INPUT_G] == 0) holdG = 0;
		else if (key[KEY_INPUT_G] == 1) holdG++;
		if (GetWindowUserCloseFlag(TRUE)) return 5;
		//�L���b�`����Ɏg�����l���v�Z
		LaneTrack[charaput] = Ntime;
		if (holdu == 0 && holdd == 0 || holdu > 0 && holdd > 0) { LaneTrack[1] = Ntime; }
		else if (holdu > 0 && holdd == 0) { LaneTrack[0] = Ntime; }
		else if (holdu == 0 && holdd > 0) { LaneTrack[2] = Ntime; }
		if (LaneTrack[0] <= LaneTrack[2]) { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[0]); }
		else { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[2]); }
		//�q�b�g
		if (holda == 1 || holdb == 1 || holdc == 1) charahit = GetNowCount();
		if (charahit + 750 < GetNowCount()) charahit = 0;
		//�R���{�\��
		ShowCombo(combo, ComboFontimg);
		//�����\��
		for (i[0] = 0; i[0] < 2; i[0]++) if (Ntime >= lock[i[0]][1][lockN[i[0]] + 1] && lock[i[0]][1][lockN[i[0]] + 1] >= 0) lockN[i[0]]++;
		if (viewT[0][viewTN + 1] <= Ntime && viewT[0][viewTN + 1] >= 0) viewTN++;
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (speedt[i[0]][speedN[i[0]] + 1][0] <= Ntime && speedt[i[0]][speedN[i[0]] + 1][0] >= 0) speedN[i[0]]++;
			G[0] = G[3] = G[4] = G[5] = 0;
			for (i[1] = objectN[i[0]]; object[i[0]][0][i[1]] > 0; i[1]++) {
				if (object[i[0]][0][i[1]] >= viewT[0][viewTN + G[0] + 1] && viewT[0][viewTN + G[0] + 1] >= 0) G[0]++;
				if (object[i[0]][0][i[1]] - Ntime >= viewT[1][viewTN + G[0]]) continue;
				if (object[i[0]][0][i[1]] - Ntime >= 3000 && 3000 >= viewT[1][viewTN + G[0]]) break;
				if (object[i[0]][0][i[1]] >= lock[0][1][lockN[0] + G[3] + 1] && lock[0][1][lockN[0] + G[3] + 1] >= 0) G[3]++;
				if (object[i[0]][0][i[1]] >= lock[1][1][lockN[1] + G[4] + 1] && lock[1][1][lockN[1] + G[4] + 1] >= 0) G[4]++;
				while (object[i[0]][0][i[1]] >= speedt[i[0]][speedN[i[0]] + G[5] + 1][0] && speedt[i[0]][speedN[i[0]] + G[5] + 1][0] >= 0) G[5]++;
				//�c�ʒu
				if (lock[1][0][lockN[1] + G[4]] == 1) G[2] = object[i[0]][3][i[1]];
				else G[2] = Yline[i[0]];
				//���ʒu
				G[1] = (speedt[i[0]][speedN[i[0]] + G[5]][1] * 20 * (object[i[0]][0][i[1]] - Ntime) + 5000) / 50;
				G[1] += 50;
				if (lock[0][0][lockN[0] + G[3]] == 1) G[1] += object[i[0]][4][i[1]] - 150;
				else G[1] += Xline[i[0]] - 150;
				switch (object[i[0]][1][i[1]]) {
				case 1:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], hitcimg, TRUE);
					break;
				case 2:
					DrawGraph(G[1], G[2], catchimg, TRUE);
					break;
				case 3:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], upimg, TRUE);
					break;
				case 4:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], downimg, TRUE);
					break;
				case 5:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], leftimg, TRUE);
					break;
				case 6:
					DrawGraph(G[1], G[2], hitimg, TRUE);
					DrawGraph(G[1], G[2], rightimg, TRUE);
					break;
				case 7:
					DrawGraph(G[1], G[2], bombimg, TRUE);
					break;
				case 8:
					DrawGraph(G[1], G[2], goustimg, TRUE);
					break;
				}
			}
		}
		//����A�q�b�g�m�[�c
		G[0] = 0;//�������{�^���̐�
		if (holda == 1) { G[0]++; }
		if (holdb == 1) { G[0]++; }
		if (holdc == 1) { G[0]++; }
		while (G[0] > 0) {
			G[1] = -1;//��ԋ߂��q�b�g�m�[�c[-1:�q�b�g�m�[�c���Ȃ��A0:�ヌ�[���A1:�����[���A2:�����[��]
			G[2] = 200;//�������^�C�~���O�ƃm�[�c�̃^�C�~���O�̂���
			for (i[0] = 0; i[0] < 3; i[0]++) {
				G[2] = object[i[0]][0][objectN[i[0]]] - Ntime;
				if (object[i[0]][1][objectN[i[0]]] == 1 && G[2] <= 125 && G[2] >= -100) {
					G[1] = i[0];
					break;
				}
			}
			if (G[1] == -1) { break; }
			for (i[0] = G[1] + 1; i[0] < 3; i[0]++) {
				G[2] = object[i[0]][0][objectN[i[0]]] - Ntime;
				if (object[i[0]][1][objectN[i[0]]] == 1 && G[2] <= 125 && G[2] >= -100 &&
				object[i[0]][0][objectN[i[0]]] <= object[G[1]][0][objectN[G[1]]]) {
				G[1] = i[0];
				}
			}
			G[2] = object[G[1]][0][objectN[G[1]]] - Ntime;
			judghname[G[1]][1] = GetNowCount();
			judghname[G[1]][2] = 1;
			objectN[G[1]]++;
			gap[gapa[1] % 30] = G[2];
			gapa[0] += G[2];
			gapa[2] += G[2] * G[2];
			gapa[1]++;
			//just
			if (G[2] <= 40 && G[2] >= -40) {
				judghname[G[1]][0] = 1;
				combo++;
				judghcount[0]++;
				life += 2;
				Dscore[0] += 2;
				PlaySoundMem(attack, DX_PLAYTYPE_BACK);
			}
			//good
			else if (G[2] <= 70 && G[2] >= -70) {
				judghname[G[1]][0] = 2;
				combo++;
				judghcount[1]++;
				life++;
				Dscore[0]++;
				PlaySoundMem(attack, DX_PLAYTYPE_BACK);
			}
			//safe
			else if (G[2] <= 100 && G[2] >= -100) {
				judghname[G[1]][0] = 3;
				judghcount[2]++;
				life++;
				PlaySoundMem(attack, DX_PLAYTYPE_BACK);
			}
			//fastmiss
			else if (G[2] <= 125) {
				judghname[G[1]][0] = 4;
				combo = 0;
				judghcount[3]++;
				life -= 20;
			}
			G[0]--;
		}
		for (i[0] = 0; i[0] < 3; i[0]++) {
			judgh = object[i[0]][0][objectN[i[0]]] - Ntime;
			//�L���b�`�m�[�c(just�̂�)
			if (LaneTrack[i[0]] + 100 >= object[i[0]][0][objectN[i[0]]] && object[i[0]][1][objectN[i[0]]] == 2 && judgh <= 0) {
				judghname[i[0]][0] = 1;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 2;
				objectN[i[0]]++;
				combo++;
				charahit = 0;
				judghcount[0]++;
				life += 2;
				Dscore[0] += 2;
				PlaySoundMem(catchs, DX_PLAYTYPE_BACK);
			}
			//�A���[�m�[�c�e��
			else if ((holdu == 1) && object[i[0]][1][objectN[i[0]]] == 3 && judgh <= 125 && judgh >= -100) {
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 3;
				objectN[i[0]]++;
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				//just
				if (judgh <= 40 && judgh >= -40) {
					judghname[i[0]][0] = 1;
					combo++;
					judghcount[0]++;
					life += 2;
					Dscore[0] += 2;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
					judghname[i[0]][0] = 2;
					combo++;
					judghcount[1]++;
					life++;
					Dscore[0]++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
					judghname[i[0]][0] = 3;
					judghcount[2]++;
					life++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//fastmiss
				else if (judgh <= 125) {
					judghname[i[0]][0] = 4;
					combo = 0;
					judghcount[3]++;
					life -= 20;
				}
			}
			else if ((holdd == 1) && object[i[0]][1][objectN[i[0]]] == 4 && judgh <= 125 && judgh >= -100) {
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 4;
				objectN[i[0]]++;
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				//just
				if (judgh <= 40 && judgh >= -40) {
					judghname[i[0]][0] = 1;
					combo++;
					judghcount[0]++;
					life += 2;
					Dscore[0] += 2;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
					judghname[i[0]][0] = 2;
					combo++;
					judghcount[1]++;
					life++;
					Dscore[0]++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
					judghname[i[0]][0] = 3;
					judghcount[2]++;
					life++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//fastmiss
				else if (judgh <= 125) {
					judghname[i[0]][0] = 4;
					combo = 0;
					judghcount[3]++;
					life -= 20;
				}
			}
			else if ((holdl == 1) && object[i[0]][1][objectN[i[0]]] == 5 && judgh <= 125 && judgh >= -100) {
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 5;
				objectN[i[0]]++;
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				//just
				if (judgh <= 40 && judgh >= -40) {
					judghname[i[0]][0] = 1;
					combo++;
					judghcount[0]++;
					life += 2;
					Dscore[0] += 2;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
					judghname[i[0]][0] = 2;
					combo++;
					judghcount[1]++;
					life++;
					Dscore[0]++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
					judghname[i[0]][0] = 3;
					judghcount[2]++;
					life++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//fastmiss
				else if (judgh <= 125) {
					judghname[i[0]][0] = 4;
					combo = 0;
					judghcount[3]++;
					life -= 20;
				}
			}
			else if ((holdr == 1) && object[i[0]][1][objectN[i[0]]] == 6 && judgh <= 125 && judgh >= -100) {
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 6;
				objectN[i[0]]++;
				gap[gapa[1] % 30] = judgh;
				gapa[0] += judgh;
				gapa[2] += judgh * judgh;
				gapa[1]++;
				//just
				if (judgh <= 40 && judgh >= -40) {
					judghname[i[0]][0] = 1;
					combo++;
					judghcount[0]++;
					life += 2;
					Dscore[0] += 2;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
					judghname[i[0]][0] = 2;
					combo++;
					judghcount[1]++;
					life++;
					Dscore[0]++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
					judghname[i[0]][0] = 3;
					judghcount[2]++;
					life++;
					if (CheckSoundMem(arrow))StopSoundMem(arrow);
					PlaySoundMem(arrow, DX_PLAYTYPE_BACK);
				}
				//fastmiss
				else if (judgh <= 125) {
					judghname[i[0]][0] = 4;
					combo = 0;
					judghcount[3]++;
					life -= 20;
				}
			}
			//�{���m�[�c
			else if (i[0] == charaput && object[i[0]][1][objectN[i[0]]] == 7 && judgh <= 0 && judgh >= -40) {
				judghname[i[0]][0] = 4;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 7;
				objectN[i[0]]++;
				combo = 0;
				judghcount[3]++;
				life -= 20;
				PlaySoundMem(bomb, DX_PLAYTYPE_BACK);
			}
			else if (object[i[0]][1][objectN[i[0]]] == 7 && judgh < -40) {
				judghname[i[0]][0] = 1;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 7;
				objectN[i[0]]++;
				combo++;
				judghcount[0]++;
				life += 2;
				Dscore[0] += 2;
			}
			//�S�[�X�g�m�[�c
			else if (object[i[0]][1][objectN[i[0]]] == 8 && judgh < 16) objectN[i[0]]++;
			//�S�m�[�cslowmiss
			else while (judgh <= -100 && judgh >= -100000 && object[i[0]][1][objectN[i[0]]] >= 1 && object[i[0]][1][objectN[i[0]]] <= 6) {
				judghname[i[0]][0] = 4;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 0;
				combo = 0;
				objectN[i[0]]++;
				judghcount[3]++;
				life -= 20;
				judgh = object[i[0]][0][objectN[i[0]]] - Ntime;
			}
		}
		if (gapa[2] > 2140000000) {
			gapa[0] /= 2;
			gapa[1] /= 2;
			gapa[2] /= 2;
		}
		Mcombo = mins(Mcombo, combo);
		//����+�q�b�g�G�t�F�N�g�\��
		for (i[0] = 0; i[0] < 3; i[0]++) {
			switch (judghname[i[0]][0]) {
			case(1):
				DrawString(Xline[i[0]] - 25, Yline[i[0]] - 25, L"JUST", Cr);
				break;
			case(2):
				DrawString(Xline[i[0]] - 25, Yline[i[0]] - 25, L"GOOD", Cr);
				break;
			case(3):
				DrawString(Xline[i[0]] - 25, Yline[i[0]] - 25, L"SAFE", Cr);
				break;
			case(4):
				DrawString(Xline[i[0]] - 25, Yline[i[0]] - 25, L"MISS", Cr);
				break;
			}
			if (judghname[i[0]][1] + 250 > GetNowCount() && judghname[i[0]][0] >= 1 && judghname[i[0]][0] <= 3 && judghname[i[0]][2] >= 1 && judghname[i[0]][2] <= 6)DrawGraph(Xline[i[0]] - 10, Yline[i[0]] - 10, effimg[judghname[i[0]][2] - 1][(GetNowCount() - judghname[i[0]][1] + 250) / 50 % 5], TRUE);
			else if (judghname[i[0]][1] + 250 > GetNowCount() && judghname[i[0]][0] == 4 && judghname[i[0]][2] == 7)DrawGraph(Xline[i[0]] - 10, Yline[i[0]] - 10, effimg[6][(GetNowCount() - judghname[i[0]][1] + 250) / 50 % 5], TRUE);
			if (judghname[i[0]][1] + 750 < GetNowCount()) {
				judghname[i[0]][0] = 0;
				judghname[i[0]][1] = 0;
				judghname[i[0]][2] = 0;
			}
		}
		//���C�t��0�����̎��A1���Ɍ��_�X�R�A��20���₷�B
		if (life < 0) {
			score2[2] = maxs(score2[2] - life * 20, score2[0] + score2[1]);
			life = 0;
		}
		//�X�R�A�o�[�\��
		DrawGraph(0, 0, sbarimg, TRUE);
		//�_���\��
		score2[0] = (judghcount[0] * 90000 + judghcount[1] * 85000 + judghcount[2] * 45000) / notes;
		score2[1] = Mcombo * 10000 / notes;
		DrawFormatString(490, 20, Cr, L"SCORE:%d", score2[0] + score2[1] - score2[2]);
		//���C�t�\��
		life = maxs(life, 500);
		if (life > 100) {
			DrawGraph((291 * life - 57000) / 500, 3, Lbarimg[0], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (-51 * life + 25500) / 80);
			DrawGraph((291 * life - 57000) / 500, 3, Lbarimg[1], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
		else DrawGraph((291 * life - 57000) / 500, 3, Lbarimg[2], TRUE);
		DrawFormatString(440, 10, Cr, L"%3d", life);
		//�����\��
		if (drop == 0) {
			Dscore[1] = Dscore[0];
			G[0] = mins(Ntime - noteoff, 0);
		}
		else if (drop) { G[0] = Dscore[2]; }
		if (G[0] > Etime - noteoff) { //CLEARED
			DrawGraph(155, 38, Tbarimg[1], TRUE);
			GD[0] = (Etime - noteoff) / 100000.0;
			DrawFormatString(180, 45, Crb, L"%.3fkm", GD[0] + Dscore[1] / 1000.0);
		}
		else { //PLAYING or DROPED
			DrawGraph((291 * G[0] - 136 * Etime + 136 * noteoff) / (Etime - noteoff), 38, Tbarimg[0], TRUE);
			GD[0] = G[0] / 100000.0;
			DrawFormatString(180, 45, Cr, L"%.3fkm", GD[0] + Dscore[1] / 1000.0);
		}
		Dscore[3] = GD[0] * 1000 + Dscore[1];
		//�X�R�A�o�[�B���\��
		DrawGraph(0, 0, sbbarimg, TRUE);
		//�����j���O�X�e�[�^�X�\��
		G[0] = GetRemainNotes(judghcount, notes);
		G[1] = CalPosScore(score2, G[0], notes, combo, Mcombo);
		RunningStats(judghcount, G[1], HighSrore);
		//������Փx�\��
		if (holdG >= 1) {
			G[0] = ddif[0] * 20 / notzero(ddifG[1]) + 155;
			G[1] = ddif[24] * 20 / notzero(ddifG[1]) + 447;
			for (i[0] = 0; i[0] <= 23; i[0]++) DrawLine((G[0] * (24 - i[0]) + G[1] * i[0]) / 24, -ddif[i[0]] * 34 / notzero(ddifG[1]) + 72, (G[0] * (23 - i[0]) + G[1] * (1 + i[0])) / 24, -ddif[i[0] + 1] * 34 / notzero(ddifG[1]) + 72, Cr);
			DrawFormatString(490, 80, Cr, L"mdif:%.2f", difkey[4][3] / 100.0);
			DrawFormatString(490, 100, Cr, L"ldif:%.2f", difkey[6][3] / 100.0);
			DrawFormatString(490, 120, Cr, L"mrat:%.2f", DifRate);
		}
		//���肸��o�[�\��
		DrawGraph(219, 460, gapbarimg, TRUE);
		G[0] = gapa[1] % 30;
		for (i[0] = 0; i[0] < 30; i[0]++) {
			G[0]--;
			if (G[0] < 0) G[0] += 30;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (510 - G[0] * 17) / 2);
			DrawGraph(318 - gap[i[0]], 460, gaplineimg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 225);
		}
		//�L�[�����󋵕\��(�I�v�V����)
		if (system[5]) {
			if (holda == 1) { KeyPushCount[0]++; }
			if (holdb == 1) { KeyPushCount[1]++; }
			if (holdc == 1) { KeyPushCount[2]++; }
			if (holdu == 1) { KeyPushCount[3]++; }
			if (holdd == 1) { KeyPushCount[4]++; }
			if (holdl == 1) { KeyPushCount[5]++; }
			if (holdr == 1) { KeyPushCount[6]++; }
			DrawGraph(5, 445, KeyViewimg[maxs(holda, 1)], TRUE);
			DrawGraph(40, 445, KeyViewimg[maxs(holdb, 1)], TRUE);
			DrawGraph(75, 445, KeyViewimg[maxs(holdc, 1)], TRUE);
			DrawGraph(570, 410, KeyViewimg[maxs(holdu, 1)], TRUE);
			DrawGraph(570, 445, KeyViewimg[maxs(holdd, 1)], TRUE);
			DrawGraph(535, 445, KeyViewimg[maxs(holdl, 1)], TRUE);
			DrawGraph(605, 445, KeyViewimg[maxs(holdr, 1)], TRUE);
			if (KeyPushCount[0] == 0) { DrawString(10, 450, L"Z", Cr); }
			else { DrawFormatString(10, 450, Cr, L"%2d", KeyPushCount[0] % 100); }
			if (KeyPushCount[1] == 0) { DrawString(45, 450, L"X", Cr); }
			else { DrawFormatString(45, 450, Cr, L"%2d", KeyPushCount[1] % 100); }
			if (KeyPushCount[2] == 0) { DrawString(80, 450, L"C", Cr); }
			else { DrawFormatString(80, 450, Cr, L"%2d", KeyPushCount[2] % 100); }
			if (KeyPushCount[3] == 0) { DrawString(575, 415, L"��", Cr); }
			else { DrawFormatString(575, 415, Cr, L"%2d", KeyPushCount[3] % 100); }
			if (KeyPushCount[4] == 0) { DrawString(575, 450, L"��", Cr); }
			else { DrawFormatString(575, 450, Cr, L"%2d", KeyPushCount[4] % 100); }
			if (KeyPushCount[5] == 0) { DrawString(540, 450, L"��", Cr); }
			else { DrawFormatString(540, 450, Cr, L"%2d", KeyPushCount[5] % 100); }
			if (KeyPushCount[6] == 0) { DrawString(610, 450, L"��", Cr); }
			else { DrawFormatString(610, 450, Cr, L"%2d", KeyPushCount[6] % 100); }
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		//�f�o�b�N
		fps[fps[60]++] = Ntime - fps[61];
		if (fps[60] > 59)fps[60] -= 60;
		fps[61] = Ntime;
		G[0] = 0;
		for (i[0] = 0; i[0] <= 59; i[0]++)G[0] += fps[i[0]];
		if (Ntime != 0) DrawFormatString(20, 80, Cr, L"FPS: %.0f", 60000.0 / notzero(G[0]));
		if (AutoFlag == 1) { DrawFormatString(20, 100, Cr, L"Autoplay"); }
		//���C�t��20%�ȉ��̎��A�댯�M��(�s�N�`��)���o��
		if (life <= 100 && drop == 0) DrawGraph(0, 0, dangerimg, TRUE);
		//���C�t���Ȃ��Ȃ�����DROPED����
		if (life <= 0 && drop == 0) {
			drop = 1;
			Dscore[1] = Dscore[0];
			Dscore[2] = mins(Ntime - noteoff, 0);
		}
		if (drop) { DrawGraph(0, 0, dropimg, TRUE); }
		//�m�[�c���S���Ȃ��Ȃ����u�Ԃ̎��Ԃ��L�^
		if (GetRemainNotes(judghcount, notes) == 0 && AllNotesHitTime < 0) {
			AllNotesHitTime = GetNowCount();
		}
		//�I�[�g�łȂ��A�m�[�~�X�ȏ���o�����牉�o
		if (AutoFlag == 0) {
			ShowBonusEff(judghcount, AllNotesHitTime, BonusSnd, Bonusimg);
		}
		//�I�����Ԃ���5�b�ȏソ���āA�Ȃ��I�������甲����B
		if (Etime + 5000 <= Ntime && (musicmp3 == -1 || CheckSoundMem(musicmp3) == 0)) {
			StopSoundMem(musicmp3);
			DeleteSoundMem(musicmp3);
			break;
		}
		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			StopSoundMem(musicmp3);
			DeleteSoundMem(musicmp3);
			return 2;
		}
		WaitTimer(5);
		Ntime = GetNowCount() - Stime + system[1] * 5;
		ScreenFlip();
	}
	InitGraph();
	if (AutoFlag == 1) { return 2; }
	else { return result(p, n, o, Lv, drop, difkey[4][3], songN, DifFN, judghcount, score2, Mcombo, notes, gapa, Dscore[3]); }
}

int GetHighScore(wchar_t pas[255], int dif) {
	FILE *fp;
	int a[7] = { 0,0,0,0,0,0,0 };
	int G = _wfopen_s(&fp, pas, L"rb");
	if (G == 0) {
		fread(&a, sizeof(int), 6, fp);
		fclose(fp);
	}
	return a[dif];
}

int GetRemainNotes(int *judghcount, int Notes) {
	return Notes - judghcount[0] - judghcount[1] - judghcount[2] - judghcount[3];
}

int CalPosScore(int *score, int RemainNotes, int Notes, int combo, int MaxCombo) {
	int PosCombo = mins(combo + RemainNotes, MaxCombo);
	return score[0] + 90000 * RemainNotes / Notes + 10000 * PosCombo / Notes;
}

void ShowCombo(int combo, int *pic) {
#define ROCATION_X 320
#define ROCATION_Y 150
#define CHARA_WIDTH 50

	if (combo < 10) {
		return;
	}
	int t;
	int xx;
	GetGraphSize(pic[0], &xx, &t);
	t = 0;
	int s;
	for (int i = combo; i > 0; i /= 10) {
		t++;
	}
	for (int i = t - 1; i >= 0; i--) {
		s = combo;
		for (int j = 0; j < i; j++) {
			s /= 10;
		}
		s %= 10;
		DrawGraph(t * CHARA_WIDTH / 2 - CHARA_WIDTH / 2 - i * CHARA_WIDTH - xx / 2 + ROCATION_X, ROCATION_Y, pic[s], TRUE);
	}
}

void ShowBonusEff(int *judghcount, int EffStartTime, int *Snd, int *pic) {
#define NO_MISS 2
#define FULL_COMBO 1
#define PERFECT 0
#define PIC_X 150
#define PIC_Y 200
	int Bonus = -1;
	if (judghcount[3] > 0) {
		return;
	}
	else if (judghcount[2] > 0) {
		Bonus = NO_MISS;
	}
	else if (judghcount[1] > 0) {
		Bonus = FULL_COMBO;
	}
	else {
		Bonus = PERFECT;
	}
	if (EffStartTime + 1 >= GetNowCount()) {
		PlaySoundMem(Snd[Bonus], DX_PLAYTYPE_BACK);
	}
	if (EffStartTime + 2000 > GetNowCount()) {
		DrawGraph(PIC_X, PIC_Y, pic[Bonus], TRUE);
	}
}

void RunningStats(int *judghcount, int PosScore, int HighScore) {
#define x1 6
#define y1 6
#define x2 188
#define y2 6
#define x3 150
#define y3 69
#define x4 6
#define y4 69
	unsigned int CrG = GetColor(63, 63, 63);
	unsigned int CrD = GetColor(255, 63, 127);
	unsigned int CrY = GetColor(255, 255, 0);
	unsigned int CrC = GetColor(0, 255, 255);
	if (judghcount[3] > 0) {
		DrawTriangle(x1, y1, x2, y2, x3, y3, CrG, TRUE);
	}
	else if (judghcount[2] > 0) {
		DrawTriangle(x1, y1, x2, y2, x3, y3, CrD, TRUE);
	}
	else if (judghcount[1] > 0) {
		DrawTriangle(x1, y1, x2, y2, x3, y3, CrY, TRUE);
	}
	else {
		DrawTriangle(x1, y1, x2, y2, x3, y3, CrC, TRUE);
	}
	if (PosScore < HighScore) {
		DrawTriangle(x1, y1, x3, y3, x4, y4, CrG, TRUE);
	}
	else {
		DrawTriangle(x1, y1, x3, y3, x4, y4, GetColor(lins(HighScore, 255, 100000, 0, PosScore), 255, lins(HighScore, 0, 100000, 255, PosScore)), TRUE);
	}
}