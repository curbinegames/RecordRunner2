#include "playing.h"

int play2(int n, int o) {
	short int i[2];
	short int Lv = 0;
	short int notes = 0;
	short int bgp[3] = { 0,0,0 }; //[0:sky,1:ground,2:water]�̉��ʒu
	short int bgf[2] = { 0,0 }; //�������w�i��[0:���ʒu,1:�c�ʒu]
	short int charaput = 1; //�L�����̍��̈ʒu[0�ŏ�,1�Œ�,2�ŉ�]
	short int drop = 0;
	int judgh = 0; //�m�[�c�̎��ԋ���
	int charahit = 0; //�L�������m�[�c������������ł��邩�ǂ����B[1�ȏ�Œ@�����A0�Œ@���ĂȂ�]
	int G[10], songT;
	int system[5] = { 0,0,0,2,0 };
	int noteoff = 0; //�m�[�c�̃I�t�Z�b�g
	int Etime = 0; //���ʂ̏I���̎���
	int songdata = 0;
	int Ntime = 0;
	int judghname[3][3]{ 0,0,0,0,0,0,0,0,0 };
	int holda = 0;
	int holdb = 0;
	int holdu = 0;
	int holdd = 0;
	int holdl = 0;
	int holdr = 0;
	int holdG = 0;
	int combo = 0;
	int LaneTrack[3] = { -100,-100,-100 };
	int Mcombo = 0;
	int Dscore[3] = { 0,0,0 }; //�����ɓ����镔��[���_�p,���_�ۑ��p,�����ۑ��p]
	int judghcount[4] = { 0,0,0,0 };
	int life = 500;
	int gap[30] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//gap = ���肸��
	int gapa[3] = { 0,0,0 };//gapa = ���肸��[���v, ��, 2��̍��v]
	int score2[4] = { 0,0,0,0 }; //[�ʏ�X�R�A(90000), �R���{�X�R�A(10000), ���_, ���v]
	int fps[62];//0�`59=1�t���[���Ԋu�̎���,60=���̑����,61=�O��̎���
	for (i[0] = 0; i[0] <= 59; i[0]++)fps[i[0]] = 17;
	fps[60] = 0;
	fps[61] = 0;
	double GD[5];
	int item[99]; //�A�C�e���̃A�h���X�ADrawGraph�ŌĂׂ�B
	short int itemN = 0; //���̔ԍ�
	int chamo[3][99][2]; //�L������[0:��,1:��,2:��]��[0:�O���t�B�b�N,1:�؂�ւ�����]
	chamo[0][0][0] = 0;
	chamo[0][0][1] = 0;
	chamo[1][0][0] = 1;
	chamo[1][0][1] = 0;
	chamo[2][0][0] = 1;
	chamo[2][0][1] = 0;
	short int chamoN[3] = { 1,1,1 }; //���̔ԍ�
	int fall[99][2]; //�������w�i��[0:�ԍ�,1:����]
	fall[0][0] = 0;
	fall[0][1] = -1;
	short int fallN = 1; //���̔ԍ�
	int Ymove[5][999][4]; //[��, ��, ��, (�n��), (����)]���[���c�ړ���[0:�J�n����,1:�ʒu,2:�I������,3:���]
	Ymove[0][0][0] = 0;
	Ymove[0][0][1] = 300;
	Ymove[0][0][2] = 0;
	Ymove[0][0][3] = 1;
	Ymove[1][0][0] = 0;
	Ymove[1][0][1] = 350;
	Ymove[1][0][2] = 0;
	Ymove[1][0][3] = 1;
	Ymove[2][0][0] = 0;
	Ymove[2][0][1] = 400;
	Ymove[2][0][2] = 0;
	Ymove[2][0][3] = 1;
	Ymove[3][0][0] = 0;
	Ymove[3][0][1] = 350;
	Ymove[3][0][2] = 0;
	Ymove[3][0][3] = 1;
	Ymove[4][0][0] = 0;
	Ymove[4][0][1] = 600;
	Ymove[4][0][2] = 0;
	Ymove[4][0][3] = 1;
	short int YmoveN[5] = { 1,1,1,1,1 }; //���̔ԍ�
	short int YmoveN2[3] = { 0,0,0 };
	int Xmove[3][999][4]; //[��, ��, ��]���[�����ړ���[0:�J�n����,1:�ʒu,2:�I������,3:���]
	Xmove[0][0][0] = 0;
	Xmove[0][0][1] = 150;
	Xmove[0][0][2] = 0;
	Xmove[0][0][3] = 1;
	Xmove[1][0][0] = 0;
	Xmove[1][0][1] = 150;
	Xmove[1][0][2] = 0;
	Xmove[1][0][3] = 1;
	Xmove[2][0][0] = 0;
	Xmove[2][0][1] = 150;
	Xmove[2][0][2] = 0;
	Xmove[2][0][3] = 1;
	short int XmoveN[3] = { 1,1,1 }; //���̔ԍ�
	short int XmoveN2[3] = { 0,0,0 };
	int lock[2][2][99]; //lock = [��,�c]�̉����̈ʒu��[(1=�Œ肷��,-1�ȊO=�Œ肵�Ȃ�),����]
	lock[0][0][0] = -1;
	lock[0][1][0] = 0;
	lock[1][0][0] = 1;
	lock[1][1][0] = 0;
	short int lockN[2] = { 1,1 }; //���̔ԍ�
	int carrow[2][99];
	carrow[0][0] = 1;
	carrow[1][0] = 0;
	short int carrowN = 1;
	int viewT[2][99];//[�����\������,���s����,[0]=���i���o�[]
	viewT[0][0] = 0;
	viewT[1][0] = 3000;
	short int viewTN = 1;
	int object[3][5][999]; //[��,��,��]���[���̉�����[����,���,(�g���ĂȂ�),�c�ʒu,���ʒu]
	object[0][3][0] = 300;
	object[1][3][0] = 350;
	object[2][3][0] = 400;
	short int objectN[3] = { 0,0,0 }; //���̔ԍ�
	int difkey[50][4];//��Փx�v�Z�Ɏg��[�ԍ�][���̓L�[,����,��Փx�_,[0]�����:[1]���̔ԍ�:[2]1�O�̔ԍ�:[3]2�O�̔ԍ�:[4]�ō��_:[5]�f�[�^��:[6]�Ō�50�̍��v:[7]�v�Z���珜�O���鎞��]
	difkey[0][2] = 0;
	difkey[1][2] = 0;
	difkey[1][3] = 0;
	difkey[2][3] = -1;
	difkey[3][3] = -2;
	difkey[4][3] = 0;
	difkey[5][3] = -1;
	int ddif[25] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//�e��Ԃ̓�Փx
	int ddifG[2] = { 1,1 };//0=�������Ԕԍ�(1�`25),1=�ő�l
	int Yline[5] = { 300,350,400,350,600 };//[��,��,��,�n��,����]���[���̏c�ʒu
	int Xline[3] = { 150,150,150 };//[��,��,��]���[���̉��ʒu
	double rate, bpm = 120, bpmG = 120;
	double timer[3]; //[��, ��, ��]���[���̎���
	double speedt[5][99][2]; //[��, ��, ��, (�n��), (����)]���[����[0:�؂�ւ�����,1:���x]
	speedt[0][0][0] = 0;
	speedt[0][0][1] = 1;
	speedt[1][0][0] = 0;
	speedt[1][0][1] = 1;
	speedt[2][0][0] = 0;
	speedt[2][0][1] = 1;
	speedt[3][0][0] = 0;
	speedt[3][0][1] = 1;
	speedt[4][0][0] = 0;
	speedt[4][0][1] = 1;
	short int speedN[5] = { 1,1,1,1,1 }; //���̔ԍ�
	char key[256];
	wchar_t songN[255];
	songN[0] = L'\0';
	wchar_t fileN[255];
	wchar_t dataE[255] = L"record/";
	wchar_t mp3FN[255] = L"song/";
	wchar_t skyFN[255] = L"picture/backskynoamal.png";
	wchar_t groundFN[255] = L"picture/groundnaturenormal.png";
	wchar_t waterFN[255] = L"picture/waternormal.png";
	wchar_t GT1[255];
	wchar_t GT15[255];
	wchar_t GT24[] = L"picture/";
	wchar_t GT25[6][8] = { L"/0.txt" ,L"/1.txt" ,L"/2.txt" ,L"/3.txt" ,L"/4.txt" ,L"/5.txt" };
	wchar_t RecordCode[23][13] = { L"#MUSIC:",L"#BPM:",L"#NOTEOFFSET:",L"#SKY:",L"#FIELD:",
		L"#WATER:",L"#TITLE:",L"#LEVEL:",L"#ITEM:",L"#FALL:",
		L"#MAP:",L"#END",L"#SPEED",L"#CHARA",L"#MOVE",
		L"#XMOV",L"#GMOVE",L"#XLOCK",L"#YLOCK",L"#FALL",
		L"#VIEW:",L"#E.TITLE:",L"#CARROW"
	};
	//wchar_t GT2[] = L"#MUSIC:";
	//wchar_t GT3[] = L"#BPM:";
	//wchar_t GT4[] = L"#NOTEOFFSET:";
	//wchar_t GT5[] = L"#SKY:";
	//wchar_t GT6[] = L"#FIELD:";
	//wchar_t GT7[] = L"#WATER:";
	//wchar_t GT8[] = L"#TITLE:";
	//wchar_t GT9[] = L"#LEVEL:";
	//wchar_t GT10[] = L"#ITEM:";
	//wchar_t GT11[] = L"#FALL:";
	//wchar_t GT12[] = L"#MAP:";
	//wchar_t GT13[] = L"#END";
	//wchar_t GT14[] = L"#SPEED";
	//wchar_t GT16[] = L"#CHARA";
	//wchar_t GT17[] = L"#MOVE";
	//wchar_t GT18[] = L"#XMOV";
	//wchar_t GT19[] = L"#GMOVE";
	//wchar_t GT20[] = L"#XLOCK";
	//wchar_t GT21[] = L"#YLOCK";
	//wchar_t GT22[] = L"#FALL";
	//wchar_t GT23[] = L"#VIEW:";
	//wchar_t GT26[] = L"#E.TITLE:";
	//wchar_t GT27[] = L"#CARROW";
	unsigned int Cr, Crb;
	Cr = GetColor(255, 255, 255);
	Crb = GetColor(0, 0, 0);
	FILE *fp;
	//�V�X�e�����[�h
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&system, sizeof(system), 5, fp);
		fclose(fp);
	}
	songT = FileRead_open(L"song.txt");
	for (i[0] = 0; i[0] <= n; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcopy(GT1, fileN, 1);
	strcats(dataE, GT1);
	strcopy(dataE, GT1, 1);
	strcats(GT1, GT25[o]);
	songdata = FileRead_open(GT1);
	//�e�L�X�g�f�[�^��ǂ�
	while (FileRead_eof(songdata) == 0) {
		FileRead_gets(GT1, 256, songdata);
		//���y�t�@�C����ǂݍ���
		if (strands(GT1, RecordCode[0])) {
			strmods(GT1, 7);
			strcats(mp3FN, GT1);
		}
		//BPM��ǂݍ���
		else if (strands(GT1, RecordCode[1])) bpmG = bpm = SETbpm(GT1);
		//�m�[�g�̃I�t�Z�b�g��ǂݍ���
		else if (strands(GT1, RecordCode[2])) timer[0] = timer[1] = timer[2] = noteoff = SEToffset(GT1, system[1]);
		//��̔w�i��ǂݍ���
		else if (strands(GT1, RecordCode[3])) {
			strcopy(GT24, skyFN, 1);
			strmods(GT1, 5);
			strcats(skyFN, GT1);
		}
		//�n�ʂ̉摜��ǂݍ���
		else if (strands(GT1, RecordCode[4])) {
			strcopy(GT24, groundFN, 1);
			strmods(GT1, 7);
			strcats(groundFN, GT1);
		}
		//�����̉摜��ǂݍ���
		else if (strands(GT1, RecordCode[5])) {
			strcopy(GT24, waterFN, 1);
			strmods(GT1, 7);
			strcats(waterFN, GT1);
		}
		//�Ȗ���ǂݍ���
		else if (strands(GT1, RecordCode[6])) {
			strmods(GT1, 7);
			if (system[4] == 0 || songN[0] == L'\0')strcopy(GT1, songN, 1);
		}
		//�p��
		else if (strands(GT1, RecordCode[21])) {
			strmods(GT1, 7);
			if (system[4] == 1 || songN[0] == L'\0')strcopy(GT1, songN, 1);
		}
		//���x����ǂݍ���
		else if (strands(GT1, RecordCode[7])) Lv = SETLv(GT1);
		//�A�C�e����ǂݍ���
		else if (strands(GT1, RecordCode[8])) {
			strmods(GT1, 6);
			strcopy(dataE, GT15, 1);
			stradds(GT15, L'/');
			strcats(GT15, GT1);
			item[itemN++] = LoadGraph(GT15);
		}
		//�������w�i�w��
		else if (strands(GT1, RecordCode[9])) {
			strmods(GT1, 6);
			fall[0][0] = strsans(GT1);
			fall[0][1] = 0;
		}
		//���ʂ�ǂݍ���
		else if (strands(GT1, RecordCode[10])) {
			while (FileRead_eof(songdata) == 0 && strands(GT1, RecordCode[11]) == 0) {
				FileRead_gets(GT1, 256, songdata);
				//���m�[�c���x�ω�
				if (strands(GT1, RecordCode[12])) {
					G[0] = maxs(mins(GT1[6] - 49, 0), 4);
					strmods(GT1, 8);
					speedt[G[0]][speedN[G[0]]][1] = strsans2(GT1);
					strnex(GT1);
					if (GT1[0] >= L'0' && GT1[0] <= L'9' || GT1[0] == L'-') {
						speedt[G[0]][speedN[G[0]]][0] = timer[G[0]] + 240000 * (speedt[G[0]][speedN[G[0]]][1] - 1) / (bpm * 16) - 10;
						speedt[G[0]][speedN[G[0]]][1] = strsans2(GT1);
					}
					else {
						speedt[G[0]][speedN[G[0]]][0] = timer[G[0]] - 10;
					}
					speedN[G[0]]++;
				}
				//BPM�ω�
				else if (strands(GT1, RecordCode[1])) bpmG = SETbpm(GT1);
				//�L�����O���ω�
				else if (strands(GT1, RecordCode[13])) {
					G[0] = GT1[6] - 49;
					strmods(GT1, 8);
					chamo[G[0]][chamoN[G[0]]][0] = maxs(mins(strsans(GT1), 0), 2);
					chamo[G[0]][chamoN[G[0]]][1] = timer[G[0]];
					chamoN[G[0]]++;
				}
				//�c�ړ�
				else if (strands(GT1, RecordCode[14])) {
					G[0] = maxs(mins(GT1[8] - 49, 0), 4);
					switch (GT1[5]) {
					case('l'):
						G[1] = 1;
						break;
					case('a'):
						G[1] = 2;
						break;
					case('d'):
						G[1] = 3;
						break;
					case('m'):
						G[1] = 4;
						break;
					case('s'):
						G[1] = 5;
						break;
					case('p'):
						G[1] = 6;
						break;
					}
					strmods(GT1, 10);
					GD[0] = strsans2(GT1);
					strnex(GT1);
					GD[1] = strsans2(GT1);
					strnex(GT1);
					GD[2] = strsans2(GT1);
					switch (G[1]) {
					case 1:
					case 2:
					case 3:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]]][3] = G[1];
						break;
					case 4:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime(GD[2], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 1;
						break;
					case 5:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = (Ymove[G[0]][YmoveN[G[0]] - 1][1] + GD[1] * 50 + 100) / 2;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 2;
						Ymove[G[0]][YmoveN[G[0]] + 1][0] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]] + 1][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]] + 1][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]] + 1][3] = 3;
						YmoveN[G[0]]++;
						break;
					case 6:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 3;
						Ymove[G[0]][YmoveN[G[0]] + 1][0] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]] + 1][1] = Ymove[G[0]][YmoveN[G[0]] - 1][1];
						Ymove[G[0]][YmoveN[G[0]] + 1][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]] + 1][3] = 2;
						YmoveN[G[0]]++;
						break;
					}
					YmoveN[G[0]]++;
				}
				//���ړ�
				else if (strands(GT1, RecordCode[15])) {
					G[0] = maxs(mins(GT1[8] - 49, 0), 2);
					switch (GT1[5]) {
					case('l'):
						G[1] = 1;
						break;
					case('a'):
						G[1] = 2;
						break;
					case('d'):
						G[1] = 3;
						break;
					case('m'):
						G[1] = 4;
						break;
					case('s'):
						G[1] = 5;
						break;
					case('p'):
						G[1] = 6;
						break;
					}
					strmods(GT1, 10);
					GD[0] = strsans2(GT1);
					strnex(GT1);
					GD[1] = strsans2(GT1);
					strnex(GT1);
					GD[2] = strsans2(GT1);
					switch (G[1]) {
					case 1:
					case 2:
					case 3:
						Xmove[G[0]][XmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Xmove[G[0]][XmoveN[G[0]]][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Xmove[G[0]][XmoveN[G[0]]][3] = G[1];
						break;
					case 4:
						Xmove[G[0]][XmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]) - 5;
						Xmove[G[0]][XmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Xmove[G[0]][XmoveN[G[0]]][2] = shifttime(GD[2], bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]]][3] = 1;
						break;
					case 5:
						Xmove[G[0]][XmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]]][1] = (Xmove[G[0]][XmoveN[G[0]] - 1][1] + GD[1] * 50 + 100) / 2;
						Xmove[G[0]][XmoveN[G[0]]][2] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]]][3] = 2;
						Xmove[G[0]][XmoveN[G[0]] + 1][0] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]] + 1][1] = GD[1] * 50.0 + 100.0;
						Xmove[G[0]][XmoveN[G[0]] + 1][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Xmove[G[0]][XmoveN[G[0]] + 1][3] = 3;
						XmoveN[G[0]]++;
						break;
					case 6:
						Xmove[G[0]][XmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Xmove[G[0]][XmoveN[G[0]]][2] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]]][3] = 3;
						Xmove[G[0]][XmoveN[G[0]] + 1][0] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Xmove[G[0]][XmoveN[G[0]] + 1][1] = Xmove[G[0]][XmoveN[G[0]] - 1][1];
						Xmove[G[0]][XmoveN[G[0]] + 1][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Xmove[G[0]][XmoveN[G[0]] + 1][3] = 2;
						XmoveN[G[0]]++;
						break;
					}
					XmoveN[G[0]]++;
				}
				//GMOVE
				else if (strands(GT1, RecordCode[16])) {
					G[0] = 3;
					switch (GT1[6]) {
					case('l'):
						G[1] = 1;
						break;
					case('a'):
						G[1] = 2;
						break;
					case('d'):
						G[1] = 3;
						break;
					case('m'):
						G[1] = 4;
						break;
					case('s'):
						G[1] = 5;
						break;
					case('p'):
						G[1] = 6;
						break;
					}
					strmods(GT1, 10);
					GD[0] = strsans2(GT1);
					strnex(GT1);
					GD[1] = strsans2(GT1);
					strnex(GT1);
					GD[2] = strsans2(GT1);
					switch (G[1]) {
					case 1:
					case 2:
					case 3:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]]][3] = G[1];
						break;
					case 4:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime(GD[2], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 1;
						break;
					case 5:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = (Ymove[G[0]][YmoveN[G[0]] - 1][1] + GD[1] * 50 + 100) / 2;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 2;
						Ymove[G[0]][YmoveN[G[0]] + 1][0] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]] + 1][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]] + 1][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]] + 1][3] = 3;
						YmoveN[G[0]]++;
						break;
					case 6:
						Ymove[G[0]][YmoveN[G[0]]][0] = shifttime(GD[0], bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][1] = GD[1] * 50.0 + 100.0;
						Ymove[G[0]][YmoveN[G[0]]][2] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]]][3] = 3;
						Ymove[G[0]][YmoveN[G[0]] + 1][0] = shifttime((GD[2] + GD[0]) / 2.0, bpmG, timer[0]);
						Ymove[G[0]][YmoveN[G[0]] + 1][1] = Ymove[G[0]][YmoveN[G[0]] - 1][1];
						Ymove[G[0]][YmoveN[G[0]] + 1][2] = shifttime(GD[2], bpmG, timer[0]) - 5;
						Ymove[G[0]][YmoveN[G[0]] + 1][3] = 2;
						YmoveN[G[0]]++;
						break;
					}
					YmoveN[G[0]]++;
				}
				//�����b�N
				else if (strands(GT1, RecordCode[17])) {
					strmods(GT1, 7);
					lock[0][0][lockN[0]] = lock[0][0][lockN[0] - 1] * -1;
					lock[0][1][lockN[0]] = shifttime(strsans(GT1), bpmG, timer[0]);
					lockN[0]++;
				}
				//�c���b�N
				else if (strands(GT1, RecordCode[18])) {
					strmods(GT1, 7);
					lock[1][0][lockN[1]] = lock[1][0][lockN[1] - 1] * -1;
					lock[1][1][lockN[1]] = shifttime(strsans(GT1), bpmG, timer[0]);
					lockN[1]++;
				}
				//�L���������ω�
				else if (strands(GT1, RecordCode[22])) {
					strmods(GT1, 8);
					carrow[0][carrowN] = carrow[0][carrowN - 1] * -1;
					carrow[1][carrowN] = shifttime(strsans(GT1), bpmG, timer[0]);
					carrowN++;
				}
				//�������w�i�؂�ւ�
				else if (strands(GT1, RecordCode[19])) {
					strmods(GT1, 6);
					fall[fallN][0] = strsans(GT1);
					strnex(GT1);
					fall[fallN][1] = strsans(GT1);
					fallN++;
				}
				//�����\������
				else if (strands(GT1, RecordCode[20])) {
					strmods(GT1, 6);
					viewT[0][viewTN] = shifttime(strsans(GT1), bpmG, timer[0]);
					strnex(GT1);
					viewT[1][viewTN] = strsans(GT1);
					viewTN++;
				}
				//�I���
				else if (strands(GT1, RecordCode[11])) {
					break;
				}
				//����ȊO
				else {
					for (i[0] = 0; i[0] <= 2; i[0]++) {
						G[0] = 0;
						while (GT1[G[0]] != L'\0' && GT1[G[0]] != L',') G[0]++;
						for (i[1] = 0; i[1] < G[0]; i[1]++) {
							if (GT1[i[1]] == L'0')continue;
							object[i[0]][0][objectN[i[0]]] = timer[i[0]] + 240000 * i[1] / (bpmG * G[0]);
							switch (GT1[i[1]]) {
							case L'H':
								object[i[0]][1][objectN[i[0]]] = 1;
								break;
							case L'C':
								object[i[0]][1][objectN[i[0]]] = 2;
								break;
							case L'U':
								object[i[0]][1][objectN[i[0]]] = 3;
								break;
							case L'D':
								object[i[0]][1][objectN[i[0]]] = 4;
								break;
							case L'L':
								object[i[0]][1][objectN[i[0]]] = 5;
								break;
							case L'R':
								object[i[0]][1][objectN[i[0]]] = 6;
								break;
							case L'B':
								object[i[0]][1][objectN[i[0]]] = 7;
								break;
							case L'G':
								object[i[0]][1][objectN[i[0]]] = 8;
								break;
							}
							//object[][2][]���g�����������炱���ɓ����(�Q�[�������ԂɂȂ�\��)
							object[i[0]][3][objectN[i[0]]] = 50 * i[0] + 300;
							object[i[0]][4][objectN[i[0]]] = 150;
							//�c�ʒu���v�Z����
							while (Ymove[i[0]][YmoveN2[i[0]]][2] <= object[i[0]][0][objectN[i[0]]] && Ymove[i[0]][YmoveN2[i[0]]][2] >= 0)YmoveN2[i[0]]++;
							if (Ymove[i[0]][YmoveN2[i[0]]][0] >= 0 && Ymove[i[0]][YmoveN2[i[0]]][0] <= object[i[0]][0][objectN[i[0]]] && Ymove[i[0]][YmoveN2[i[0]]][2] > object[i[0]][0][objectN[i[0]]]) {
								switch (Ymove[i[0]][YmoveN2[i[0]]][3]) {
								case 1:
									object[i[0]][3][objectN[i[0]]] = lins(Ymove[i[0]][YmoveN2[i[0]]][0], Ymove[i[0]][YmoveN2[i[0]] - 1][1], Ymove[i[0]][YmoveN2[i[0]]][2], Ymove[i[0]][YmoveN2[i[0]]][1], object[i[0]][0][objectN[i[0]]]);
									break;
								case 2:
									object[i[0]][3][objectN[i[0]]] = pals(Ymove[i[0]][YmoveN2[i[0]]][0], Ymove[i[0]][YmoveN2[i[0]] - 1][1], Ymove[i[0]][YmoveN2[i[0]]][2], Ymove[i[0]][YmoveN2[i[0]]][1], object[i[0]][0][objectN[i[0]]]);
									break;
								case 3:
									object[i[0]][3][objectN[i[0]]] = pals(Ymove[i[0]][YmoveN2[i[0]]][2], Ymove[i[0]][YmoveN2[i[0]]][1], Ymove[i[0]][YmoveN2[i[0]]][0], Ymove[i[0]][YmoveN2[i[0]] - 1][1], object[i[0]][0][objectN[i[0]]]);
									break;
								}
							}
							else object[i[0]][3][objectN[i[0]]] = Ymove[i[0]][YmoveN2[i[0]] - 1][1];
							//���ʒu���v�Z����
							while (Xmove[i[0]][XmoveN2[i[0]]][2] <= object[i[0]][0][objectN[i[0]]] && Xmove[i[0]][XmoveN2[i[0]]][2] >= 0) XmoveN2[i[0]]++;
							if (Xmove[i[0]][XmoveN2[i[0]]][0] >= 0 && Xmove[i[0]][XmoveN2[i[0]]][0] <= object[i[0]][0][objectN[i[0]]] && Xmove[i[0]][XmoveN2[i[0]]][2] > object[i[0]][0][objectN[i[0]]]) {
								switch (Xmove[i[0]][XmoveN2[i[0]]][3]) {
								case 1:
									object[i[0]][4][objectN[i[0]]] = lins(Xmove[i[0]][XmoveN2[i[0]]][0], Xmove[i[0]][XmoveN2[i[0]] - 1][1], Xmove[i[0]][XmoveN2[i[0]]][2], Xmove[i[0]][XmoveN2[i[0]]][1], object[i[0]][0][objectN[i[0]]]);
									break;
								case 2:
									object[i[0]][4][objectN[i[0]]] = pals(Xmove[i[0]][XmoveN2[i[0]]][0], Xmove[i[0]][XmoveN2[i[0]] - 1][1], Xmove[i[0]][XmoveN2[i[0]]][2], Xmove[i[0]][XmoveN2[i[0]]][1], object[i[0]][0][objectN[i[0]]]);
									break;
								case 3:
									object[i[0]][4][objectN[i[0]]] = pals(Xmove[i[0]][XmoveN2[i[0]]][2], Xmove[i[0]][XmoveN2[i[0]]][1], Xmove[i[0]][XmoveN2[i[0]]][0], Xmove[i[0]][XmoveN2[i[0]] - 1][1], object[i[0]][0][objectN[i[0]]]);
									break;
								}
							}
							else object[i[0]][4][objectN[i[0]]] = Xmove[i[0]][XmoveN2[i[0]] - 1][1];
							if (object[i[0]][1][objectN[i[0]]] != 8) notes++;
							objectN[i[0]]++;
						}
						if (i[0] <= 1) FileRead_gets(GT1, 256, songdata);
					}
					timer[0] = timer[1] = timer[2] += 240000.0 / bpmG;
				}
			}
		}
	}
	//���ʂ̍Ō��goust��u��
	for (i[0] = 0; i[0] <= 2; i[0]++) {
		object[i[0]][0][objectN[i[0]]] = timer[i[0]];
		object[i[0]][0][objectN[i[0]] + 1] = -1;
		object[i[0]][1][objectN[i[0]]] = 8;
		object[i[0]][3][objectN[i[0]]] = 1000;
	}
	lock[0][0][lockN[0]] = 1;
	lock[0][1][lockN[0]] = -1;
	lock[1][0][lockN[1]] = -1;
	lock[1][1][lockN[1]] = -1;
	Etime = timer[0];
	/*��Փx�v�Z
	(3000000/�ЂƂO�̃m�[�c�Ƃ̊Ԋu)���A���̃m�[�c�̓�Փx�_���Ƃ���B
	�@�����L�[��50�L�����A���̑g�ݍ��킹�łł����ō��l���Ȃ̓�Փx�B
	1�O�Ɠ����L�[�̎���(�Ώۂ̃L�[��arrow�̂�)
	���Ԋu��150ms�ȏ�̎��͕ϓ��Ȃ�
	���Ԋu��150ms�ȉ�75ms�ȏ�̎��͓��_("�Ԋu"/(-75)+3)�{(1�`2�{)
	���Ԋu��75ms�ȉ��̎��͓��_2�{
	arrow�Ђ�������2.5�{
	2�O�ƈႤ�L�[�̎��͓��_1.2�{(�S�L�[�Ώ�)
	arrow�L�[�͓��_1.2�{
	1=hit,2=non,3=up,4=down,5=left,6=right,7=non or douw,8=up or down,9=up or non
	*/
	objectN[0] = 0;
	objectN[1] = 0;
	objectN[2] = 0;
	difkey[0][3] = notes;
	if (difkey[0][3] > 49)difkey[0][3] = 49;
	difkey[7][3] = (Etime - noteoff) / 25 * 2;
	if (difkey[7][3] < 10000)difkey[7][3] = 10000;
	while (object[0][0][objectN[0]] >= 0 || object[1][0][objectN[1]] >= 0 || object[2][0][objectN[2]] >= 0) {
		for (i[0] = 0; i[0] < 3; i[0]++) while (object[i[0]][1][objectN[i[0]]] == 8 && object[i[0]][0][objectN[i[0]]] >= 0) objectN[i[0]]++;
		G[0] = -1;
		for (i[0] = 0; i[0] < 3; i[0]++) if (object[i[0]][0][objectN[i[0]]] >= 0) {
			G[0] = i[0];
			break;
		}
		if (G[0] == -1) break;
		for (i[0] = 0; i[0] < 3; i[0]++) if (G[0] != i[0] && object[G[0]][0][objectN[G[0]]] > object[i[0]][0][objectN[i[0]]] && object[i[0]][0][objectN[i[0]]] >= 0) G[0] = i[0];
		//ddif�̌v�Z
		//old
		while (object[G[0]][0][objectN[G[0]]] >= (Etime - noteoff) / 25 * ddifG[0] + noteoff) {
			if (difkey[5][3] < 49) {
				for (i[0] = 0; i[0] < difkey[5][3]; i[0]++) if (difkey[i[0]][1] > (Etime - noteoff) / 25 * ddifG[0] - difkey[7][3] + noteoff)ddif[ddifG[0] - 1] += difkey[i[0]][2];
			}
			else {
				for (i[0] = 0; i[0] <= 49; i[0]++)if (difkey[i[0]][1] > (Etime - noteoff) / 25 * ddifG[0] - difkey[7][3] + noteoff) ddif[ddifG[0] - 1] += difkey[i[0]][2];
			}
			ddifG[0]++;
		}
		difkey[difkey[1][3]][0] = object[G[0]][1][objectN[G[0]]];
		if (difkey[difkey[1][3]][0] == 2) {
			if (G[0] != 1) difkey[difkey[1][3]][0] = G[0] / 2 + 3;
			if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0]) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 3 && (difkey[difkey[2][3]][0] == 8 || difkey[difkey[2][3]][0] == 9)) {
				difkey[difkey[2][3]][0] = 3;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 2 && (difkey[difkey[2][3]][0] == 1 || difkey[difkey[2][3]][0] == 5 || difkey[difkey[2][3]][0] == 6)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 2 && (difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9)) {
				difkey[difkey[2][3]][0] = 2;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 4 && (difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 8)) {
				difkey[difkey[2][3]][0] = 4;
				objectN[G[0]]++;
				continue;
			}
		}
		else if (difkey[difkey[1][3]][0] == 7) {
			if (G[0] != 0)difkey[difkey[1][3]][0] = G[0] + 7;
			if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0]) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && difkey[difkey[2][3]][0] == 8 || difkey[difkey[1][3]][0] == 8 && difkey[difkey[2][3]][0] == 7) {
				difkey[difkey[2][3]][0] = 4;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && difkey[difkey[2][3]][0] == 9 || difkey[difkey[1][3]][0] == 9 && difkey[difkey[2][3]][0] == 7) {
				difkey[difkey[2][3]][0] = 2;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 8 && difkey[difkey[2][3]][0] == 9 || difkey[difkey[1][3]][0] == 9 && difkey[difkey[2][3]][0] == 8) {
				difkey[difkey[2][3]][0] = 3;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && (difkey[difkey[2][3]][0] != 3)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 8 && (difkey[difkey[2][3]][0] == 3 || difkey[difkey[2][3]][0] == 4)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 9 && (difkey[difkey[2][3]][0] != 4)) {
				objectN[G[0]]++;
				continue;
			}
		}
		difkey[difkey[1][3]][1] = object[G[0]][0][objectN[G[0]]];
		if (difkey[2][3] != -1 && difkey[3][3] != -1) {
			G[1] = difkey[difkey[1][3]][1] - difkey[difkey[2][3]][1];
			if (G[1] == 0)G[1] = 1;
			if (G[1] <= 5 && difkey[difkey[1][3]][0] != difkey[difkey[2][3]][0]) difkey[difkey[1][3]][2] = difkey[difkey[2][3]][2] * 1.3;
			else {
				difkey[difkey[1][3]][2] = 3000000 / G[1];
				if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0] && difkey[difkey[1][3]][0] >= 3 && difkey[difkey[1][3]][0] <= 6 && difkey[difkey[1][3]][2] >= 40000) difkey[difkey[1][3]][2] *= 2;
				else if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0] && difkey[difkey[1][3]][0] >= 3 && difkey[difkey[1][3]][0] <= 6 && difkey[difkey[1][3]][2] <= 40000 && difkey[difkey[1][3]][2] >= 20000) difkey[difkey[1][3]][2] = difkey[difkey[1][3]][2] / -75.0 + 3.0;
				if (difkey[difkey[1][3]][0] != difkey[difkey[3][3]][0]) difkey[difkey[1][3]][2] *= 1.2;
				if (difkey[difkey[1][3]][0] >= 3 && difkey[difkey[1][3]][0] <= 6)difkey[difkey[1][3]][2] *= 1.2;
			}
		}
		for (i[0] = 0; i[0] < 3; i[0]++) if (object[G[0]][1][objectN[G[0]]] >= 3 && object[G[0]][1][objectN[G[0]]] <= 6 && G[0] != i[0] && object[G[0]][1][objectN[G[0]]] == object[i[0]][1][objectN[i[0]]] && object[G[0]][0][objectN[G[0]]] + 5 >= object[i[0]][0][objectN[i[0]]]) objectN[i[0]]++;
		objectN[G[0]]++;
		difkey[5][3]++;
		G[0] = 0;
		for (i[0] = 0; i[0] <= difkey[0][3]; i[0]++) {
			if (difkey[i[0]][2] < 0) break;
			if (difkey[i[0]][1] > difkey[difkey[1][3]][1] - difkey[7][3])G[0] += difkey[i[0]][2];
		}
		if (difkey[4][3] < G[0])difkey[4][3] = G[0];
		for (i[0] = 1; i[0] < 4; i[0]++) {
			difkey[i[0]][3]++;
			if (difkey[i[0]][3] > difkey[0][3])difkey[i[0]][3] = 0;
		}
	}
	difkey[5][3]++;
	for (i[0] = 0; i[0] < 2; i[0]++)if (difkey[i[0]][2] == 0 && difkey[2][2] > 0)difkey[5][3]--;
	if (difkey[5][3] < 1)difkey[5][3] = 1;
	if (difkey[5][3] > 50)difkey[5][3] = 50;
	ddifG[1] = difkey[4][3];
	if (ddifG[1] <= 0)ddifG[1] = 1;
	difkey[4][3] /= 50;
	ddif[ddifG[0] - 1] = 0;
	for (i[0] = 0; i[0] < difkey[5][3]; i[0]++) if (difkey[i[0]][1] > Etime - difkey[7][3])ddif[ddifG[0] - 1] += difkey[i[0]][2];
	for (i[0] = ddifG[0] - 1; i[0] <= 24; i[0]++)ddif[i[0]] = ddif[ddifG[0] - 1];
	difkey[6][3] = ddif[ddifG[0] - 1] / 50;
	//���[�j���O�O���[���[Lv.1->884, Trick MonitorLv.9->28632, "����"/31.8+47.1="Lv"*100
	difkey[4][3] = difkey[4][3] / 31.8 + 47.1;
	difkey[6][3] = difkey[6][3] / 31.8 + 47.1;
	//�i���o�[���Z�b�g
	objectN[0] = 0;
	objectN[1] = 0;
	objectN[2] = 0;
	YmoveN[0] = 0;
	YmoveN[1] = 0;
	YmoveN[2] = 0;
	YmoveN[3] = 0;
	YmoveN[4] = 0;
	XmoveN[0] = 0;
	XmoveN[1] = 0;
	XmoveN[2] = 0;
	lockN[0] = 0;
	lockN[1] = 0;
	chamoN[0] = 0;
	chamoN[1] = 0;
	chamoN[2] = 0;
	speedN[0] = 0;
	speedN[1] = 0;
	speedN[2] = 0;
	speedN[3] = 0;
	carrowN = 0;
	fallN = 0;
	viewTN = 0;
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
	int Rchaimg;
	int musicmp3, attack, catchs, arrow;
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
		difberimg = LoadGraph(L"picture/difhope.png");
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
	}
	else {
		attack = LoadSoundMem(L"sound/non.wav");
		catchs = LoadSoundMem(L"sound/non.wav");
		arrow = LoadSoundMem(L"sound/non.wav");
	}
	//�Q�[���J�n�O�̉�����
	notes = notzero(notes);
	PlaySoundMem(musicmp3, DX_PLAYTYPE_BACK);
	WaitTimer(10);
	int Stime = GetNowCount();
	//�Q�[���J�n
	while (1) {
		ClearDrawScreen();
		GetHitKeyStateAll(key);
		//�w�i��^���Âɂ��Ă���ꍇ�A�w�i��\�����Ȃ��B
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
			//�A�C�e���\��

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
		for (i[0] = 0; i[0] < 3; i[0]++)while (Ntime >= chamo[i[0]][chamoN[i[0]] + 1][1] && chamo[i[0]][chamoN[i[0]] + 1][1] >= 0) chamoN[i[0]]++;
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
		else if (key[KEY_INPUT_UP] == 1 && key[KEY_INPUT_DOWN] == 0) {
			G[4] = Yline[0];
			charaput = 0;
		}
		//����������āA���O�̃q�b�g�}�[�J�[���������Ă��Ȃ��Ƃ�
		else if (key[KEY_INPUT_UP] == 0 && key[KEY_INPUT_DOWN] == 1) {
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
		if (key[KEY_INPUT_Z] == 0) holda = 0;
		else if (key[KEY_INPUT_Z] == 1) holda++;
		if (key[KEY_INPUT_X] == 0) holdb = 0;
		else if (key[KEY_INPUT_X] == 1) holdb++;
		if (key[KEY_INPUT_UP] == 0) holdu = 0;
		else if (key[KEY_INPUT_UP] == 1) holdu++;
		if (key[KEY_INPUT_LEFT] == 0) holdl = 0;
		else if (key[KEY_INPUT_LEFT] == 1) holdl++;
		if (key[KEY_INPUT_RIGHT] == 0) holdr = 0;
		else if (key[KEY_INPUT_RIGHT] == 1) holdr++;
		if (key[KEY_INPUT_DOWN] == 0) holdd = 0;
		else if (key[KEY_INPUT_DOWN] == 1) holdd++;
		if (key[KEY_INPUT_G] == 0) holdG = 0;
		else if (key[KEY_INPUT_G] == 1) holdG++;
		if (GetWindowUserCloseFlag(TRUE)) return 5;
		//�L���b�`����Ɏg�����l���v�Z
		if (holdu == 0 && holdd == 0 || holdu > 0 && holdd > 0) { LaneTrack[1] = Ntime; }
		else if (holdu > 0 && holdd == 0) { LaneTrack[0] = Ntime; }
		else if (holdu == 0 && holdd > 0) { LaneTrack[2] = Ntime; }
		if (LaneTrack[0] <= LaneTrack[2]) { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[0]); }
		else { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[2]); }
		//�q�b�g
		if (holda == 1 || holdb == 1) charahit = GetNowCount();
		if (charahit + 750 < GetNowCount()) charahit = 0;
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
				if (object[i[0]][0][i[1]] >= speedt[i[0]][speedN[i[0]] + G[5] + 1][0] && speedt[i[0]][speedN[i[0]] + G[5] + 1][0] >= 0) G[5]++;
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
		//����
		G[1] = 0;
		for (i[0] = 0; i[0] < 3; i[0]++) {
			judgh = object[i[0]][0][objectN[i[0]]] - Ntime;
			//�q�b�g�m�[�c
			if ((holda == 1 || holdb == 1) && G[1] == 0 && object[i[0]][1][objectN[i[0]]] == 1 && judgh <= 125 && judgh >= -100 &&
				(object[i[0]][0][objectN[i[0]]] <= object[0][0][objectN[0]]) &&
				(object[i[0]][0][objectN[i[0]]] <= object[1][0][objectN[1]]) &&
				(object[i[0]][0][objectN[i[0]]] <= object[2][0][objectN[2]])) {
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 1;
				objectN[i[0]]++;
				G[1] = 1;
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
					PlaySoundMem(attack, DX_PLAYTYPE_BACK);
				}
				//good
				else if (judgh <= 70 && judgh >= -70) {
					judghname[i[0]][0] = 2;
					combo++;
					judghcount[1]++;
					life++;
					Dscore[0]++;
					PlaySoundMem(attack, DX_PLAYTYPE_BACK);
				}
				//safe
				else if (judgh <= 100 && judgh >= -100) {
					judghname[i[0]][0] = 3;
					judghcount[2]++;
					life++;
					PlaySoundMem(attack, DX_PLAYTYPE_BACK);
				}
				//fastmiss
				else if (judgh <= 125) {
					judghname[i[0]][0] = 4;
					combo = 0;
					judghcount[3]++;
					life -= 20;
				}
			}
			//�L���b�`�m�[�c(just�̂�)
			else if (LaneTrack[i[0]] + 100 >= object[i[0]][0][objectN[i[0]]] && object[i[0]][1][objectN[i[0]]] == 2 && judgh <= 0) {
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
			else if (judgh <= -100 && judgh >= -100000 && object[i[0]][1][objectN[i[0]]] >= 1 && object[i[0]][1][objectN[i[0]]] <= 6) {
				judghname[i[0]][0] = 4;
				judghname[i[0]][1] = GetNowCount();
				judghname[i[0]][2] = 0;
				combo = 0;
				objectN[i[0]]++;
				judghcount[3]++;
				life -= 20;
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
		//������Փx�\��
		if (holdG >= 1) {
			G[0] = ddif[0] * 20 / notzero(ddifG[1]) + 155;
			G[1] = ddif[24] * 20 / notzero(ddifG[1]) + 447;
			for (i[0] = 0; i[0] <= 23; i[0]++) DrawLine((G[0] * (24 - i[0]) + G[1] * i[0]) / 24, -ddif[i[0]] * 34 / notzero(ddifG[1]) + 72, (G[0] * (23 - i[0]) + G[1] * (1 + i[0])) / 24, -ddif[i[0] + 1] * 34 / notzero(ddifG[1]) + 72, Cr);
			DrawFormatString(490, 80, Cr, L"mdif:%.2f", difkey[4][3] / 100.0);
			DrawFormatString(490, 100, Cr, L"ldif:%.2f", difkey[6][3] / 100.0);
		}
		//�X�R�A�o�[�B���\��
		DrawGraph(0, 0, sbbarimg, TRUE);
		//�R���{�\��
		if (combo >= 1) DrawFormatString(15, 15, Cr, L"%d combo", combo);
		//���肸��o�[�\��
		DrawGraph(219, 460, gapbarimg, TRUE);
		G[0] = gapa[1] % 30;
		for (i[0] = 0; i[0] < 30; i[0]++) {
			G[0]--;
			if (G[0] < 0) G[0] += 30;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (510 - G[0] * 17) / 2);
			DrawGraph(318 - gap[i[0]], 460, gaplineimg, TRUE);
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		//�f�o�b�N
		fps[fps[60]++] = Ntime - fps[61];
		if (fps[60] > 59)fps[60] -= 60;
		fps[61] = Ntime;
		G[0] = 0;
		for (i[0] = 0; i[0] <= 59; i[0]++)G[0] += fps[i[0]];
		if (Ntime != 0) DrawFormatString(20, 80, Cr, L"FPS: %.0f", 60000.0 / notzero(G[0]));
		//for (i[0] = 0; i[0] < 3; i[0]++) { DrawFormatString(20, 100 + i[0] * 20, Cr, L"LaneTrack%d: %d", i[0], LaneTrack[i[0]]); }
		//���C�t��20%�ȉ��̎��A�댯�M��(�s�N�`��)���o��
		if (life <= 100 && drop == 0) DrawGraph(0, 0, dangerimg, TRUE);
		//���C�t���Ȃ��Ȃ�����DROPED����
		if (life <= 0 && drop == 0) {
			drop = 1;
			Dscore[1] = Dscore[0];
			Dscore[2] = mins(Ntime - noteoff, 0);
		}
		if (drop) { DrawGraph(0, 0, dropimg, TRUE); }
		//�I�����Ԃ���1�b�ȏソ���āA�Ȃ��I�������甲����B
		if (CheckSoundMem(musicmp3) == 0 && Etime <= Ntime + 5000) {
			StopSoundMem(musicmp3);
			DeleteSoundMem(musicmp3);
			break;
		}
		WaitTimer(5);
		Ntime = GetNowCount() - Stime;
		ScreenFlip();
	}
	int	read[7] = { 0,0,0,0,0,0,0 };
	int chap[3] = { 0,0,0 };
	double readR[10];
	wchar_t savec[10][255];
	wchar_t save[255] = L"score/";
	wchar_t save2[255] = L".dat";
	score2[3] = score2[0] + score2[1] - score2[2];
	//�Z�[�u����
	strcats(save, fileN);
	strcats(save, save2);
	//�n�C�X�R�A�ۑ�
	G[0] = _wfopen_s(&fp, save, L"rb");
	if (G[0] == 0) {
		fread(&read, sizeof(read), 6, fp);
		fclose(fp);
	}
	if (read[o] < score2[3]) {
		read[o] = score2[3];
		G[0] = _wfopen_s(&fp, save, L"wb");
		fwrite(&read, sizeof(int), 6, fp);
		fclose(fp);
	}
	//�v���C�񐔕ۑ�
	G[0] = _wfopen_s(&fp, L"save/data.dat", L"rb");
	if (G[0] == 0) {
		fread(&read, sizeof(read), 7, fp);
		fclose(fp);
	}
	read[0]++;
	if (drop == 1) read[1]++;
	else read[3]++;
	if (judghcount[3] == 0)read[4]++;
	if (judghcount[2] == 0 && judghcount[3] == 0)read[5]++;
	if (judghcount[1] == 0 && judghcount[2] == 0 && judghcount[3] == 0)read[6]++;
	G[0] = _wfopen_s(&fp, L"save/data.dat", L"wb");
	fwrite(&read, sizeof(int), 7, fp);
	fclose(fp);
	//�L�����v���C���ۑ�
	G[0] = _wfopen_s(&fp, L"save/chap.dat", L"rb");
	if (G[0] == 0) {
		fread(&chap, sizeof(chap), 3, fp);
		fclose(fp);
	}
	chap[system[0]]++;
	G[0] = _wfopen_s(&fp, L"save/chap.dat", L"wb");
	fwrite(&chap, sizeof(int), 3, fp);
	fclose(fp);
	//���[�g�v�Z(level0�Ȃ�0�Œ�)"���x��" - "miss��" x "���x��" x 0.03(����=0)
	if (Lv == 0) rate = 0;
	else if (judghcount[3] > 0) {
		rate = Lv - judghcount[3] * Lv*0.03;
		rate = mins(rate, 0);
	}
	//NO MISS,"���x��" + 1 - "safe��" x 0.05(����="���x��")
	else if (judghcount[3] == 0 && judghcount[2] > 0) {
		rate = Lv + 1 - judghcount[2] * 0.05;
		rate = mins(rate, Lv);
	}
	//FULL COMBO,"���x��" + 2 - "good��" x 0.01(����="���x��" + 1)
	else if (judghcount[3] == 0 && judghcount[2] == 0 && judghcount[1] > 0) {
		rate = Lv + 2 - judghcount[1] * 0.01;
		rate = mins(rate, Lv + 1);
	}
	//PERFECT, "���x��" + 2
	else if (judghcount[3] == 0 && judghcount[2] == 0 && judghcount[1] == 0) rate = Lv + 2;
	//���[�g�ۑ�
	G[0] = _wfopen_s(&fp, L"save/rateS.dat", L"rb");
	if (G[0] == 0) {
		fread(&savec, 255, 10, fp);
		fclose(fp);
	}
	G[0] = _wfopen_s(&fp, L"save/rateN.dat", L"rb");
	if (G[0] == 0) {
		fread(&readR, sizeof(readR), 10, fp);
		fclose(fp);
	}
	G[0] = -1;
	//�����ȁA�܂��͖����^��T��
	for (i[0] = 0; i[0] < 10; i[0]++)if (strands(fileN, savec[i[0]]) || savec[i[0]] == '\0') {
		G[0] = i[0];
		break;
	}
	//�Ȃ�������A��ԒႢ���[�g��T��
	if (G[0] == -1) {
		G[0] = 0;
		for (i[0] = 1; i[0] < 10; i[0]++)if (readR[G[0]] > readR[i[0]]) G[0] = i[0];
	}
	//���[�g������������X�V����
	if (readR[G[0]] < rate) {
		readR[G[0]] = rate;
		strcopy(fileN, savec[G[0]], 1);
	}
	G[0] = _wfopen_s(&fp, L"save/rateS.dat", L"wb");
	fwrite(&savec, 255, 10, fp);
	fclose(fp);
	G[0] = _wfopen_s(&fp, L"save/rateN.dat", L"wb");
	fwrite(&readR, sizeof(double), 10, fp);
	fclose(fp);
	//���U���g���
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	while (1) {
		double acc;
		ClearDrawScreen();
		acc = (judghcount[0] * 10000 + judghcount[1] * 9500 + judghcount[2] * 5500) / (notes*100.0);
		DrawGraph(0, 0, resultimg, TRUE);
		DrawGraph(460, 20, difberimg, TRUE);
		DrawString(100, 13, songN, Cr);
		DrawFormatString(150, 60, Cr, L"%d", judghcount[0]);
		DrawFormatString(150, 100, Cr, L"%d", judghcount[1]);
		DrawFormatString(150, 140, Cr, L"%d", judghcount[2]);
		DrawFormatString(150, 180, Cr, L"%d", judghcount[3]);
		DrawFormatString(180, 220, Cr, L"%d", Mcombo);
		DrawFormatString(280, 220, Cr, L"%d", notes);
		DrawFormatString(450, 90, Cr, L"%d", score2[3]);
		DrawFormatString(450, 170, Cr, L"%.2f", acc);
		if (gapa[1] == 0) gapa[1] = 1;
		DrawFormatString(510, 210, Cr, L"%2d", gapa[0] / gapa[1]);
		DrawFormatString(510, 235, Cr, L"%2d", gapa[2] / gapa[1] - gapa[0] * gapa[0] / gapa[1] / gapa[1]);
		if (score2[3] >= 98000) DrawGraph(95, 280, rankimg[0], TRUE);
		else if (score2[3] >= 95000) DrawGraph(140, 260, rankimg[1], TRUE);
		else if (score2[3] >= 90000) DrawGraph(140, 260, rankimg[2], TRUE);
		else if (score2[3] >= 85000) DrawGraph(140, 260, rankimg[3], TRUE);
		else if (score2[3] >= 80000) DrawGraph(140, 260, rankimg[4], TRUE);
		else DrawGraph(140, 260, rankimg[5], TRUE);
		if (drop == 1) DrawGraph(5, 420, coleimg[0], TRUE);
		if (drop == 0 && judghcount[3] > 0) DrawGraph(5, 420, coleimg[1], TRUE);
		if (judghcount[3] == 0 && judghcount[2] > 0) DrawGraph(5, 420, coleimg[2], TRUE);
		if (judghcount[3] == 0 && judghcount[2] == 0 && judghcount[1] > 0) DrawGraph(5, 420, coleimg[3], TRUE);
		if (judghcount[3] == 0 && judghcount[2] == 0 && judghcount[1] == 0) DrawGraph(5, 420, coleimg[4], TRUE);
		DrawGraph(336, 252, Rchaimg, TRUE);
		ScreenFlip();
		//�G���^�[�������ꂽ
		if (CheckHitKey(KEY_INPUT_RETURN) == 1) break;
		else if (GetWindowUserCloseFlag(TRUE)) return 5;
	}
	InitGraph();
	return 2;
}