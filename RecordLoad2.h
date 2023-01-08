#include "playing.h"
#include "playbox.h"
#include "recp_cal_ddif.h"

typedef struct ddef_box {
	int maxdif = 0;
	int lastdif = 0;
	int nowdifsection = 1;
	int datanum = -1;
} ddef_box;

int IsNoteCode(wchar_t c);
int cal_ddif(int num, int const *difkey, int Etime, int noteoff, int difsec, int voidtime);
int cal_nowdif_m(int *difkey, int num, int now, int voidtime);
item_eff_box set_pic_mat(wchar_t *s);
int MapErrorCheck(int nownote, int nowtime, int befnote, int beftime, int dif, int wl);

void RecordLoad2(int p, int n, int o) {
	//n: �ȃi���o�[
	//o: ��Փx�i���o�[
	short int i[2];
	short int Lv = 0;
	short int notes = 0;
	int G[20], songT;
	int noteoff = 0; //�m�[�c�̃I�t�Z�b�g
	int Etime = 0; //���ʂ̏I���̎���
	int songdata = 0;
	int waningLv = 2;
	double GD[5];
	//int item[99]; //�A�C�e���̃A�h���X�ADrawGraph�ŌĂׂ�B
	//short int itemN = 0; //���̔ԍ�
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
	item_box Movie[999];//�A�C�e���\��[�A�C�e���ԍ�,�ړ��`��,�J�n����,�I������,�J�nx�ʒu,�I��x�ʒu,�J�ny�ʒu,�I��y�ʒu,�J�n�T�C�Y,�I���T�C�Y,�J�n�p�x,�I���p�x,�J�n�����x,�I�������x]
	item_set_box item_set[99];
	short int MovieN = 0;
	struct camera_box camera[255];
	camera[0].starttime = 0;
	camera[0].endtime = 0;
	camera[0].xpos = 0;
	camera[0].ypos = 0;
	camera[0].zoom = 1;
	camera[0].rot = 0;
	camera[0].mode = 0;
	short int cameraN = 1; //���̔ԍ�
	struct custom_note_box customnote[9];
	struct scrool_box scrool[99];
	scrool[0].starttime = 0;
	scrool[0].basetime = 0;
	scrool[0].speed = 1;
	short int scroolN = 1;
	struct note_box note[3][2000];//[��,��,��]���[���̃m�[�c[�ԍ�]
	note[0][0].ypos = 300;
	note[1][0].ypos = 350;
	note[2][0].ypos = 400;
	short int objectN[3] = { 0,0,0 }; //���̔ԍ�
	int difkey[50][4];//��Փx�v�Z�Ɏg��[�ԍ�][���̓L�[,����,��Փx�_,[0]�����:[1]���̔ԍ�:[2]1�O�̔ԍ�:[3]2�O�̔ԍ�:[4]�ō��_:[5]�f�[�^��:[6]�Ō�50�̍��v:[7]�v�Z���珜�O���鎞��]
	difkey[0][2] = 0;
	difkey[1][2] = 0;
	difkey[1][3] = 0;
	difkey[2][3] = -1;
	difkey[3][3] = -2;
	difkey[4][3] = 0;
	int ddif[25] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//�e��Ԃ̓�Փx
	int ddifG[2] = { 1,1 };//0=�������Ԕԍ�(1�`25),1=�ő�l
	ddef_box ddif2;
	int outpoint[2] = { 0, 0 }; /* 0=����, 1=�G���[�ԍ� */
	view_BPM_box v_bpm[100];
	double bpm = 120, bpmG = 120;
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
	wchar_t songN[255];
	wchar_t songNE[255];
	songN[0] = L'\0';
	wchar_t fileN[255];
	wchar_t dataE[255] = L"record/";//�t�H���_�̖��O
	wchar_t RRS[255]; //PC�p���ʃf�[�^�̕ۑ��ꏊ
	wchar_t mp3FN[255];
	wchar_t skyFN[255] = L"picture/backskynoamal.png";
	wchar_t groundFN[255] = L"picture/groundnaturenormal.png";
	wchar_t waterFN[255] = L"picture/waternormal.png";
	wchar_t DifFN[255] = L"picture/difanother.png";
	wchar_t GT1[255];
	wchar_t GT2[255];
	wchar_t GT24[] = L"picture/";
	wchar_t GT25[6][7] = { L"/0.txt" ,L"/1.txt" ,L"/2.txt" ,L"/3.txt" ,L"/4.txt" ,L"/5.txt" };
	wchar_t GT26[6][7] = { L"/0.rrs" ,L"/1.rrs" ,L"/2.rrs" ,L"/3.rrs" ,L"/4.rrs" ,L"/5.rrs" };
	wchar_t ST1[] = L"record/";
	wchar_t ST2[] = L"list.txt";
	playnum_box allnum;
	FILE *fp;
	songT = FileRead_open(L"RecordPack.txt");
	for (i[0] = 0; i[0] <= p; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcats(dataE, GT1); //"record/<�p�b�N��>"
	stradds(dataE, L'/'); //"record/<�p�b�N��>/"
	strcopy(dataE, GT2, 1);
	strcats(GT2, ST2); //"record/<�p�b�N��>/list.txt"
	songT = FileRead_open(GT2);
	for (i[0] = 0; i[0] <= n; i[0]++) FileRead_gets(GT1, 256, songT);
	FileRead_close(songT);
	strcopy(GT1, fileN, 1); //"<�Ȗ�>"
	strcats(dataE, GT1); //"record/<�p�b�N��>/<�Ȗ�>"
	strcopy(dataE, GT1, 1);
	strcats(GT1, GT25[o]); //"record/<�p�b�N��>/<�Ȗ�>/<��Փx>.txt"
	songdata = FileRead_open(GT1);
	if (songdata == 0) {
		return;
	}
	//�e�L�X�g�f�[�^��ǂ�
	while (FileRead_eof(songdata) == 0) {
		FileRead_gets(GT1, 256, songdata);
		//���y�t�@�C����ǂݍ���
		if (strands(GT1, L"#MUSIC:")) {
			strmods(GT1, 7);
			strcopy(dataE, mp3FN, 1);
			strcats(mp3FN, L"/");
			strcats(mp3FN, GT1);
		}
		//BPM��ǂݍ���
		else if (strands(GT1, L"#BPM:")) {
			bpmG = bpm = SETbpm(GT1);
			v_bpm[0].time = noteoff;
			v_bpm[0].BPM = (unsigned short)bpm;
		}
		//�m�[�g�̃I�t�Z�b�g��ǂݍ���
		else if (strands(GT1, L"#NOTEOFFSET:")) timer[0] = timer[1] = timer[2] = noteoff = SEToffset(GT1);
		//��̔w�i��ǂݍ���
		else if (strands(GT1, L"#SKY:")) {
			strcopy(GT24, skyFN, 1);
			strmods(GT1, 5);
			strcats(skyFN, GT1);
		}
		//�n�ʂ̉摜��ǂݍ���
		else if (strands(GT1, L"#FIELD:")) {
			strcopy(GT24, groundFN, 1);
			strmods(GT1, 7);
			strcats(groundFN, GT1);
		}
		//�����̉摜��ǂݍ���
		else if (strands(GT1, L"#WATER:")) {
			strcopy(GT24, waterFN, 1);
			strmods(GT1, 7);
			strcats(waterFN, GT1);
		}
		//��Փx�o�[(another)��ǂݍ���
		else if (strands(GT1, L"#DIFBAR:")) {
			strcopy(dataE, DifFN, 1);
			strmods(GT1, 8);
			stradds(DifFN, L'/');
			strcats(DifFN, GT1);
		}
		//�Ȗ���ǂݍ���
		else if (strands(GT1, L"#TITLE:")) {
			strmods(GT1, 7);
			strcopy(GT1, songN, 1);
		}
		//�p��
		else if (strands(GT1, L"#E.TITLE:")) {
			strmods(GT1, 7);
			strcopy(GT1, songNE, 1);
		}
		//���x����ǂݍ���
		else if (strands(GT1, L"#LEVEL:")) Lv = SETLv(GT1);
		//�A�C�e����ǂݍ���
		/*else if (strands(GT1, L"#ITEM:")) {
			strmods(GT1, 6);
			strcopy(dataE, GT15, 1);
			stradds(GT15, L'/');
			strcats(GT15, GT1);
			item[itemN++] = LoadGraph(GT15);
		}*/
		//�������w�i�w��
		else if (strands(GT1, L"#FALL:")) {
			strmods(GT1, 6);
			fall[0][0] = strsans(GT1);
			fall[0][1] = 0;
		}
		//���ʓ�Փx�t�B���^�̃��x��
		else if (strands(GT1, L"#WANING:")) {
			strmods(GT1, 8);
			waningLv = strsans(GT1);
		}
		//���ʂ�ǂݍ���
		else if (strands(GT1, L"#MAP:")) {
			while (FileRead_eof(songdata) == 0 && strands(GT1, L"#END") == 0) {
				FileRead_gets(GT1, 256, songdata);
				//���m�[�c���x�ω�
				if (strands(GT1, L"#SPEED")) {
					G[0] = maxs(mins(GT1[6] - 49, 0), 4);
					strmods(GT1, 8);
					speedt[G[0]][speedN[G[0]]][1] = strsans2(GT1);
					strnex(GT1);
					if (GT1[0] >= L'0' && GT1[0] <= L'9' || GT1[0] == L'-') {
						speedt[G[0]][speedN[G[0]]][0] = timer[G[0]] + 240000 * (speedt[G[0]][speedN[G[0]]][1] - 1) / (bpmG * 16) - 10;
						speedt[G[0]][speedN[G[0]]][1] = strsans2(GT1);
					}
					else {
						speedt[G[0]][speedN[G[0]]][0] = timer[G[0]] - 10;
					}
					speedN[G[0]]++;
				}
				//�f�[�^������BPM�ω�
				else if (strands(GT1, L"#BPM:")) bpmG = SETbpm(GT1);
				//�����ڂ�BPM�ω�
				else if (strands(GT1, L"#V-BPM:")) {
					strmods(GT1, 7);
					v_bpm[allnum.v_BPMnum].time = shifttime(strsans(GT1), bpmG, timer[0]);
					strnex(GT1);
					v_bpm[allnum.v_BPMnum].BPM = strsans(GT1);
					allnum.v_BPMnum++;
				}
				//�L�����O���ω�
				else if (strands(GT1, L"#CHARA")) {
					G[0] = GT1[6] - 49;
					strmods(GT1, 8);
					chamo[G[0]][chamoN[G[0]]][0] = maxs(mins(strsans(GT1), 0), 2);
					chamo[G[0]][chamoN[G[0]]][1] = timer[G[0]];
					chamoN[G[0]]++;
				}
				//�c�ړ�
				else if (strands(GT1, L"#MOVE")) {
					if (GT1[8] == L'A') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 0;
					}
					else if (GT1[8] == L'B') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 1;
					}
					else if (GT1[8] == L'C') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 2;
					}
					else if (GT1[8] == L'D') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 3;
					}
					else {
						G[2] = G[0] = maxs(mins(GT1[8] - 49, 0), 4);
						G[3] = 0;
					}
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
					case('e'):
						G[1] = 7;
						break;
					}
					strmods(GT1, 10);
					GD[0] = strsans2(GT1);
					strnex(GT1);
					GD[1] = strsans2(GT1);
					strnex(GT1);
					GD[2] = strsans2(GT1);
					for (i[0] = G[0]; i[0] <= G[2]; i[0]++) {
						switch (G[1]) {
						case 1:
						case 2:
						case 3:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3],
								G[1], GD[2], bpmG,
								&Ymove[i[0]][YmoveN[i[0]]][0],
								&Ymove[i[0]][YmoveN[i[0]]][1],
								&Ymove[i[0]][YmoveN[i[0]]][2],
								&Ymove[i[0]][YmoveN[i[0]]][3]);
							break;
						case 4:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3],
								1, GD[2], bpmG, &Ymove[i[0]][YmoveN[i[0]]][0],
								&Ymove[i[0]][YmoveN[i[0]]][1],
								&Ymove[i[0]][YmoveN[i[0]]][2],
								&Ymove[i[0]][YmoveN[i[0]]][3]);
							Ymove[i[0]][YmoveN[i[0]]][0] -= 5;
							Ymove[i[0]][YmoveN[i[0]]][2] -= 5;
							break;
						case 5:
							SETMove(timer[0], GD[0],
								(Ymove[i[0]][YmoveN[i[0]] - 1][1] +
								(GD[1] + G[3] * i[0] - G[3]) * 50 - 100) / 100,
								2, (GD[2] + GD[0]) / 2.0, bpmG,
								&Ymove[i[0]][YmoveN[i[0]]][0],
								&Ymove[i[0]][YmoveN[i[0]]][1],
								&Ymove[i[0]][YmoveN[i[0]]][2],
								&Ymove[i[0]][YmoveN[i[0]]][3]);
							YmoveN[i[0]]++;
							allnum.Ymovenum[i[0]]++;
							SETMove(timer[0], (GD[2] + GD[0]) / 2.0,
								GD[1] + G[3] * i[0] - G[3], 3, GD[2], bpmG,
								&Ymove[i[0]][YmoveN[i[0]]][0],
								&Ymove[i[0]][YmoveN[i[0]]][1],
								&Ymove[i[0]][YmoveN[i[0]]][2],
								&Ymove[i[0]][YmoveN[i[0]]][3]);
							break;
						case 6:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3],
								3, (GD[2] + GD[0]) / 2.0, bpmG,
								&Ymove[i[0]][YmoveN[i[0]]][0],
								&Ymove[i[0]][YmoveN[i[0]]][1],
								&Ymove[i[0]][YmoveN[i[0]]][2],
								&Ymove[i[0]][YmoveN[i[0]]][3]);
							YmoveN[i[0]]++;
							allnum.Ymovenum[i[0]]++;
							SETMove(timer[0], (GD[2] + GD[0]) / 2.0,
								(Ymove[i[0]][YmoveN[i[0]] - 2][1] - 100.0)
								/ 50.0, 2, GD[2], bpmG,
								&Ymove[i[0]][YmoveN[i[0]]][0],
								&Ymove[i[0]][YmoveN[i[0]]][1],
								&Ymove[i[0]][YmoveN[i[0]]][2],
								&Ymove[i[0]][YmoveN[i[0]]][3]);
							break;
						case 7:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3],
								2, (GD[2] + GD[0]) / 2.0, bpmG,
								&Ymove[i[0]][YmoveN[i[0]]][0],
								&Ymove[i[0]][YmoveN[i[0]]][1],
								&Ymove[i[0]][YmoveN[i[0]]][2],
								&Ymove[i[0]][YmoveN[i[0]]][3]);
							YmoveN[i[0]]++;
							allnum.Ymovenum[i[0]]++;
							SETMove(timer[0], (GD[2] + GD[0]) / 2.0,
								(Ymove[i[0]][YmoveN[i[0]] - 2][1] - 100.0)
								/ 50.0, 3, GD[2], bpmG,
								&Ymove[i[0]][YmoveN[i[0]]][0],
								&Ymove[i[0]][YmoveN[i[0]]][1],
								&Ymove[i[0]][YmoveN[i[0]]][2],
								&Ymove[i[0]][YmoveN[i[0]]][3]);
							break;
						}
						YmoveN[i[0]]++;
						allnum.Ymovenum[i[0]]++;
					}
				}
				//���ړ�
				else if (strands(GT1, L"#XMOV")) {
					if (GT1[8] == L'A') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 0;
					}
					else if (GT1[8] == L'B') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 1;
					}
					else if (GT1[8] == L'C') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 2;
					}
					else if (GT1[8] == L'D') {
						G[0] = 0;
						G[2] = 2;
						G[3] = 3;
					}
					else {
						G[2] = G[0] = maxs(mins(GT1[8] - 49, 0), 2);
						G[3] = 0;
					}
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
					case('e'):
						G[1] = 7;
						break;
					}
					strmods(GT1, 10);
					GD[0] = strsans2(GT1);
					strnex(GT1);
					GD[1] = strsans2(GT1);
					strnex(GT1);
					GD[2] = strsans2(GT1);
					for (i[0] = G[0]; i[0] <= G[2]; i[0]++) {
						switch (G[1]) {
						case 1:
						case 2:
						case 3:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3], G[1], GD[2], bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							break;
						case 4:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3], 1, GD[0], bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							Xmove[i[0]][XmoveN[i[0]]][0] -= 5;
							Xmove[i[0]][XmoveN[i[0]]][2] -= 5;
							break;
						case 5:
							SETMove(timer[0], GD[0], (Xmove[i[0]][XmoveN[i[0]] - 1][1] + GD[1] * 50 - 100) / 100, 2, (GD[2] + GD[0]) / 2.0, bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							XmoveN[i[0]]++;
							allnum.Xmovenum[i[0]]++;
							SETMove(timer[0], (GD[2] + GD[0]) / 2.0, GD[1] + G[3] * i[0] - G[3], 3, GD[2], bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							break;
						case 6:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3], 3, (GD[2] + GD[0]) / 2.0, bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							XmoveN[i[0]]++;
							allnum.Xmovenum[i[0]]++;
							SETMove(timer[0], (GD[2] + GD[0]) / 2.0, (Xmove[i[0]][XmoveN[i[0]] - 2][1] - 100.0) / 50.0, 2, GD[2], bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							break;
						case 7:
							SETMove(timer[0], GD[0], GD[1] + G[3] * i[0] - G[3], 2, (GD[2] + GD[0]) / 2.0, bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							XmoveN[i[0]]++;
							allnum.Xmovenum[i[0]]++;
							SETMove(timer[0], (GD[2] + GD[0]) / 2.0, (Xmove[i[0]][XmoveN[i[0]] - 2][1] - 100.0) / 50.0, 3, GD[2], bpmG, &Xmove[i[0]][XmoveN[i[0]]][0], &Xmove[i[0]][XmoveN[i[0]]][1], &Xmove[i[0]][XmoveN[i[0]]][2], &Xmove[i[0]][XmoveN[i[0]]][3]);
							break;
						}
						XmoveN[i[0]]++;
						allnum.Xmovenum[i[0]]++;
					}
				}
				//�U��
				else if (strands(GT1, L"#DIV")) {
					G[0] = betweens(0, GT1[5] - L'1', 2);
					G[1] = 0;
					if (GT1[4] == L'Y') { G[1] = 1; }
					strmods(GT1, 7);
					GD[0] = strsans2(GT1);//�J�n����
					strnex(GT1);
					GD[1] = strsans2(GT1);//�U���ʒu
					strnex(GT1);
					GD[2] = strsans2(GT1) / 2.0;//��������
					strnex(GT1);
					GD[3] = strsans2(GT1);//������
					if (G[1] == 1) {
						for (i[0] = 0; i[0] < GD[3]; i[0]++) {
							SETMove(timer[0], GD[0],GD[1], 1, GD[0] + GD[2],
								bpmG, &Ymove[G[0]][YmoveN[G[0]]][0],
								&Ymove[G[0]][YmoveN[G[0]]][1],
								&Ymove[G[0]][YmoveN[G[0]]][2],
								&Ymove[G[0]][YmoveN[G[0]]][3]);
							SETMove(timer[0], GD[0] + GD[2],
								(Ymove[G[0]][YmoveN[G[0]] - 1][1] - 100.0) /
								50.0, 1, GD[0] + GD[2] * 2, bpmG,
								&Ymove[G[0]][YmoveN[G[0]] + 1][0],
								&Ymove[G[0]][YmoveN[G[0]] + 1][1],
								&Ymove[G[0]][YmoveN[G[0]] + 1][2],
								&Ymove[G[0]][YmoveN[G[0]] + 1][3]);
							GD[0] += GD[2] * 2;
							YmoveN[G[0]] += 2;
							allnum.Ymovenum[G[0]] += 2;
						}
					}
					else {
						for (i[0] = 0; i[0] < GD[3]; i[0]++) {
							SETMove(timer[0], GD[0],
								GD[1],
								1, GD[0] + GD[2], bpmG,
								&Xmove[G[0]][XmoveN[G[0]]][0],
								&Xmove[G[0]][XmoveN[G[0]]][1],
								&Xmove[G[0]][XmoveN[G[0]]][2],
								&Xmove[G[0]][XmoveN[G[0]]][3]);
							SETMove(timer[0], GD[0] + GD[2],
								(Xmove[G[0]][XmoveN[G[0]] - 1][1] - 100.0) /
								50.0, 1, GD[0] + GD[2] * 2, bpmG,
								&Xmove[G[0]][XmoveN[G[0]] + 1][0],
								&Xmove[G[0]][XmoveN[G[0]] + 1][1],
								&Xmove[G[0]][XmoveN[G[0]] + 1][2],
								&Xmove[G[0]][XmoveN[G[0]] + 1][3]);
							GD[0] += GD[2] * 2;
							XmoveN[G[0]] += 2;
							allnum.Xmovenum[G[0]] += 2;
						}
					}
				}
				//GMOVE
				else if (strands(GT1, L"#GMOVE")) {
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
						allnum.Ymovenum[i[0]]++;
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
						allnum.Ymovenum[i[0]]++;
						break;
					}
					YmoveN[G[0]]++;
					allnum.Ymovenum[i[0]]++;
				}
				//�����b�N
				else if (strands(GT1, L"#XLOCK")) {
					strmods(GT1, 7);
					lock[0][0][lockN[0]] = lock[0][0][lockN[0] - 1] * -1;
					lock[0][1][lockN[0]] = shifttime(strsans(GT1), bpmG, timer[0]);
					lockN[0]++;
				}
				//�c���b�N
				else if (strands(GT1, L"#YLOCK")) {
					strmods(GT1, 7);
					lock[1][0][lockN[1]] = lock[1][0][lockN[1] - 1] * -1;
					lock[1][1][lockN[1]] = shifttime(strsans(GT1), bpmG, timer[0]);
					lockN[1]++;
				}
				//�L���������ω�
				else if (strands(GT1, L"#CARROW")) {
					strmods(GT1, 8);
					carrow[0][carrowN] = carrow[0][carrowN - 1] * -1;
					carrow[1][carrowN] = shifttime(strsans(GT1), bpmG, timer[0]);
					carrowN++;
				}
				//�������w�i�؂�ւ�
				else if (strands(GT1, L"#FALL")) {
					strmods(GT1, 6);
					fall[fallN][0] = strsans(GT1);
					strnex(GT1);
					fall[fallN][1] = strsans(GT1);
					fallN++;
				}
				//�����\������
				else if (strands(GT1, L"#VIEW:")) {
					strmods(GT1, 6);
					viewT[0][viewTN] = shifttime(strsans(GT1), bpmG, timer[0]);
					strnex(GT1);
					viewT[1][viewTN] = strsans(GT1);
					viewTN++;
				}
				//�A�C�e���\��
				else if (strands(GT1, L"#MOVIE:")) {
					strmods(GT1, 7);
					Movie[MovieN].ID = strsans(GT1);
					strnex(GT1);
					switch (GT1[0]) {
					case L'l':
						Movie[MovieN].movemode = 1;
						break;
					case L'a':
						Movie[MovieN].movemode = 2;
						break;
					case L'd':
						Movie[MovieN].movemode = 3;
						break;
					}
					strnex(GT1);
					Movie[MovieN].starttime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					Movie[MovieN].endtime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					Movie[MovieN].startXpos = strsans2(GT1) * 50 + 115;
					strnex(GT1);
					Movie[MovieN].endXpos = strsans2(GT1) * 50 + 115;
					strnex(GT1);
					Movie[MovieN].startYpos = strsans2(GT1) * 50 + 115;
					strnex(GT1);
					Movie[MovieN].endYpos = strsans2(GT1) * 50 + 115;
					strnex(GT1);
					Movie[MovieN].startsize = strsans2(GT1) * 100;
					strnex(GT1);
					Movie[MovieN].endsize = strsans2(GT1) * 100;
					strnex(GT1);
					Movie[MovieN].startrot = strsans(GT1);
					strnex(GT1);
					Movie[MovieN].endrot = strsans(GT1);
					strnex(GT1);
					Movie[MovieN].startalpha = strsans2(GT1)*255.0;
					strnex(GT1);
					Movie[MovieN].endalpha = strsans2(GT1)*255.0;
					strnex(GT1);
					Movie[MovieN].eff = set_pic_mat(GT1);
					MovieN++;
					allnum.movienum++;
				}
				//�A�C�e���Z�b�g������
				else if (strands(GT1, L"#INIT_ITEM_SET:")){
					strmods(GT1, 15);
					G[0] = strsans(GT1);
					item_set[G[0]].num = 0;
				}
				//�A�C�e���Z�b�g�ǉ�
				else if (strands(GT1, L"#ADD_ITEM_SET:")) {
					if (item_set[G[0]].num <= 10) {
						strmods(GT1, 14);
						G[0] = strsans(GT1); /* G[0] = item set�̔ԍ� */
						strnex(GT1);
						item_set[G[0]].picID[item_set[G[0]].num].picID = strsans(GT1);
						strnex(GT1);
						item_set[G[0]].picID[item_set[G[0]].num].Xpos = strsans2(GT1) * 50;
						strnex(GT1);
						item_set[G[0]].picID[item_set[G[0]].num].Ypos = strsans2(GT1) * 50;
						strnex(GT1);
						item_set[G[0]].picID[item_set[G[0]].num].size = strsans2(GT1) * 100;
						strnex(GT1);
						item_set[G[0]].picID[item_set[G[0]].num].rot = strsans(GT1);
						strnex(GT1);
						item_set[G[0]].picID[item_set[G[0]].num].alpha = strsans2(GT1) * 255;
						strnex(GT1);
						item_set[G[0]].picID[item_set[G[0]].num].eff = set_pic_mat(GT1);
						item_set[G[0]].num++;
					}
				}
				//�A�C�e���Z�b�g�\��
				else if (strands(GT1, L"#ITEM_SET:")) {
					strmods(GT1, 10);
					G[0] = strsans(GT1); /* G[0] = item box�̔ԍ� */
					strnex(GT1);
					switch (GT1[0]) {
					case L'l':
						G[1] = 1;
						break;
					case L'a':
						G[1] = 2;
						break;
					case L'd':
						G[1] = 3;
						break;
					} /* G[1] = �ړ����[�h */
					strnex(GT1);
					G[2] = shifttime(strsans2(GT1), bpmG, timer[0]); /* stime */
					strnex(GT1);
					G[3] = shifttime(strsans2(GT1), bpmG, timer[0]); /* etime */
					strnex(GT1);
					G[4] = strsans2(GT1) * 50 + 115; /* sx */
					strnex(GT1);
					G[5] = strsans2(GT1) * 50 + 115; /* ex */
					strnex(GT1);
					G[6] = strsans2(GT1) * 50 + 115; /* sy */
					strnex(GT1);
					G[7] = strsans2(GT1) * 50 + 115; /* ey */
					strnex(GT1);
					G[8] = strsans2(GT1) * 100; /* ss */
					strnex(GT1);
					G[9] = strsans2(GT1) * 100; /* es */
					strnex(GT1);
					G[10] = strsans(GT1); /* sr */
					strnex(GT1);
					G[11] = strsans(GT1); /* er */
					strnex(GT1);
					G[12] = strsans2(GT1) * 255.0; /* sa */
					strnex(GT1);
					G[13] = strsans2(GT1) * 255.0; /* ea */
					for (i[0] = 0; i[0] < item_set[G[0]].num; i[0]++) {
						Movie[MovieN].ID = item_set[G[0]].picID[i[0]].picID;
						Movie[MovieN].movemode = G[1];
						Movie[MovieN].eff = item_set[G[0]].picID[i[0]].eff;
						Movie[MovieN].starttime = G[2];
						Movie[MovieN].endtime = G[3];
						Movie[MovieN].startXpos = item_set[G[0]].picID[i[0]].Xpos * G[8] / 100;
						Movie[MovieN].endXpos = item_set[G[0]].picID[i[0]].Xpos * G[9] / 100;
						Movie[MovieN].startYpos = item_set[G[0]].picID[i[0]].Ypos * G[8] / 100;
						Movie[MovieN].endYpos = item_set[G[0]].picID[i[0]].Ypos * G[9] / 100;
						rot_xy_pos(G[10], &Movie[MovieN].startXpos, &Movie[MovieN].startYpos);
						rot_xy_pos(G[10], &Movie[MovieN].endXpos, &Movie[MovieN].endYpos);
						Movie[MovieN].startXpos += G[4];
						Movie[MovieN].endXpos += G[5];
						Movie[MovieN].startYpos += G[6];
						Movie[MovieN].endYpos += G[7];
						Movie[MovieN].startsize = G[8] * item_set[G[0]].picID[i[0]].size / 100;
						Movie[MovieN].endsize = G[9] * item_set[G[0]].picID[i[0]].size / 100;
						Movie[MovieN].startrot = G[10] + item_set[G[0]].picID[i[0]].rot;
						Movie[MovieN].endrot = G[11] + item_set[G[0]].picID[i[0]].rot;
						Movie[MovieN].startalpha = G[12] * item_set[G[0]].picID[i[0]].alpha / 255;
						Movie[MovieN].endalpha = G[13] * item_set[G[0]].picID[i[0]].alpha / 255;
						MovieN++;
						allnum.movienum++;
					}
				}
				//�J�����ړ�+�Y�[��+�p�x(������)
				else if (strands(GT1, L"#CAMERA:")) {
					strmods(GT1, 8);
					camera[cameraN].starttime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					camera[cameraN].endtime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					camera[cameraN].xpos = strsans2(GT1) * 50;
					strnex(GT1);
					camera[cameraN].ypos = strsans2(GT1) * 50;
					strnex(GT1);
					camera[cameraN].zoom = strsans2(GT1);
					strnex(GT1);
					camera[cameraN].rot = strsans2(GT1);
					strnex(GT1);
					switch (GT1[0]) {
					case L'a':
						camera[cameraN].mode = 2;
						break;
					case L'd':
						camera[cameraN].mode = 3;
						break;
					default:
						camera[cameraN].mode = 1;
						break;
					}
					cameraN++;
				}
				//�J�����ړ�
				else if (strands(GT1, L"#CAMMOVE:")) {
					strmods(GT1, 9);
					camera[cameraN].starttime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					camera[cameraN].endtime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					camera[cameraN].xpos = strsans2(GT1) * 50;
					strnex(GT1);
					camera[cameraN].ypos = strsans2(GT1) * 50;
					strnex(GT1);
					switch (GT1[0]) {
					case L'a':
						camera[cameraN].mode = 2;
						break;
					case L'd':
						camera[cameraN].mode = 3;
						break;
					default:
						camera[cameraN].mode = 1;
						break;
					}
					cameraN++;
				}
				//�X�N���[��
				else if (strands(GT1, L"#SCROOL:")) {
					strmods(GT1, 8);
					scrool[scroolN].starttime = shifttime(strsans2(GT1), bpmG, timer[0]);
					strnex(GT1);
					scrool[scroolN].speed = strsans2(GT1);
					G[0]= scrool[scroolN - 1].speed * scrool[scroolN].starttime + scrool[scroolN - 1].basetime;
					scrool[scroolN].basetime = G[0] - scrool[scroolN].speed * scrool[scroolN].starttime;
					scroolN++;
				}
				//�J�X�^���m�[�c�Z�b�g
				else if (strands(GT1, L"#CUSTOM:")) {
					strmods(GT1, 8);
					G[0] = strsans2(GT1) - 1;
					customnote[G[0]].color = 0;
					customnote[G[0]].melody = MELODYSOUND_NONE;
					customnote[G[0]].note = 0;
					customnote[G[0]].sound = 0;
					strnex(GT1);
					while (GT1[0] != L'\0') {
						if (strands(GT1, L"NOTE=")) {
							strmods(GT1, 5);
							customnote[G[0]].note = GT1[0];
						}
						else if (strands(GT1, L"SOUND=")) {
							strmods(GT1, 6);
							if (GT1[0] == L'L' || GT1[0] == L'H') {
								switch (GT1[1]) {
								case L'F':
									customnote[G[0]].melody = (enum melodysound)(LOW_F + (GT1[0] == L'H' ? 12 : 0) + (GT1[2] == L'#' ? 1 : 0));
									break;
								case L'G':
									customnote[G[0]].melody = (enum melodysound)(LOW_G + (GT1[0] == L'H' ? 12 : 0) + (GT1[2] == L'#' ? 1 : 0));
									break;
								case L'A':
									customnote[G[0]].melody = (enum melodysound)(LOW_A + (GT1[0] == L'H' ? 12 : 0) + (GT1[2] == L'#' ? 1 : 0));
									break;
								case L'B':
									customnote[G[0]].melody = (enum melodysound)(LOW_B + (GT1[0] == L'H' ? 12 : 0));
									break;
								case L'C':
									customnote[G[0]].melody = (enum melodysound)(LOW_C + (GT1[0] == L'H' ? 12 : 0) + (GT1[2] == L'#' ? 1 : 0));
									break;
								case L'D':
									customnote[G[0]].melody = (enum melodysound)(LOW_D + (GT1[0] == L'H' ? 12 : 0) + (GT1[2] == L'#' ? 1 : 0));
									break;
								case L'E':
									customnote[G[0]].melody = (enum melodysound)(LOW_E + (GT1[0] == L'H' ? 12 : 0));
									break;
								default:
									customnote[G[0]].melody = MELODYSOUND_NONE;
									break;
								}
							}
							else {
								customnote[G[0]].sound = strsans2(GT1);
							}
						}
						else if (strands(GT1, L"COLOR=")) {
							strmods(GT1, 6);
							customnote[G[0]].color = strsans2(GT1);
						}
						else {
							break;
						}
						strnex(GT1);
					}
				}
				//�I���
				else if (strands(GT1, L"#END")) { break; }
				//��
				else if (GT1[0] == L'\0') {}
				//����ȊO
				else {
					for (i[0] = 0; i[0] <= 2; i[0]++) {
						G[0] = 0;
						while (GT1[G[0]] != L'\0' && GT1[G[0]] != L',') G[0]++;
						for (i[1] = 0; i[1] < G[0]; i[1]++) {
							if (IsNoteCode(GT1[i[1]]) == 0) {
								continue;
							}
							note[i[0]][objectN[i[0]]].hittime = timer[i[0]] + 240000 * i[1] / (bpmG * G[0]);
							if (L'1' <= GT1[i[1]] && GT1[i[1]] <= L'9') {
								G[2] = (int)(customnote[GT1[i[1]] - L'1'].note);
							}
							else {
								G[2] = (int)(GT1[i[1]]);
							}
							switch ((wchar_t)(G[2])) {
							case L'H':
								note[i[0]][objectN[i[0]]].object = 1;
								break;
							case L'C':
								note[i[0]][objectN[i[0]]].object = 2;
								break;
							case L'U':
								note[i[0]][objectN[i[0]]].object = 3;
								break;
							case L'D':
								note[i[0]][objectN[i[0]]].object = 4;
								break;
							case L'L':
								note[i[0]][objectN[i[0]]].object = 5;
								break;
							case L'R':
								note[i[0]][objectN[i[0]]].object = 6;
								break;
							case L'B':
								note[i[0]][objectN[i[0]]].object = 7;
								break;
							case L'G':
								note[i[0]][objectN[i[0]]].object = 8;
								break;
							case L'?':
								note[i[0]][objectN[i[0]]].object = GetRand(4) + 2;
								if (note[i[0]][objectN[i[0]]].object == 2) {
									note[i[0]][objectN[i[0]]].object = 1;
								}
								break;
							case L'!':
								note[i[0]][objectN[i[0]]].object = GetRand(7) + 1;
								break;
							}
							//viewtime���v�Z����
							G[1] = 0;
							while (0 <= scrool[G[1] + 1].starttime &&
								scrool[G[1] + 1].starttime <= note[i[0]][objectN[i[0]]].hittime) {
								G[1]++;
							}
							note[i[0]][objectN[i[0]]].viewtime = note[i[0]][objectN[i[0]]].hittime * scrool[G[1]].speed + scrool[G[1]].basetime;
							note[i[0]][objectN[i[0]]].ypos = 50 * i[0] + 300;
							note[i[0]][objectN[i[0]]].xpos = 150;
							//�c�ʒu���v�Z����
							while (Ymove[i[0]][YmoveN2[i[0]]][2] <= note[i[0]][objectN[i[0]]].hittime &&
								Ymove[i[0]][YmoveN2[i[0]]][2] >= 0) {
								YmoveN2[i[0]]++;
							}
							if (Ymove[i[0]][YmoveN2[i[0]]][0] >= 0 &&
								Ymove[i[0]][YmoveN2[i[0]]][0] <= note[i[0]][objectN[i[0]]].hittime &&
								Ymove[i[0]][YmoveN2[i[0]]][2] > note[i[0]][objectN[i[0]]].hittime) {
								note[i[0]][objectN[i[0]]].ypos = movecal(Ymove[i[0]][YmoveN2[i[0]]][3],
									Ymove[i[0]][YmoveN2[i[0]]][0], Ymove[i[0]][YmoveN2[i[0]] - 1][1],
									Ymove[i[0]][YmoveN2[i[0]]][2], Ymove[i[0]][YmoveN2[i[0]]][1],
									note[i[0]][objectN[i[0]]].hittime);
							}
							else {
								note[i[0]][objectN[i[0]]].ypos = Ymove[i[0]][YmoveN2[i[0]] - 1][1];
							}
							//���ʒu���v�Z����
							while (Xmove[i[0]][XmoveN2[i[0]]][2] <= note[i[0]][objectN[i[0]]].hittime &&
								Xmove[i[0]][XmoveN2[i[0]]][2] >= 0) {
								XmoveN2[i[0]]++;
							}
							if (Xmove[i[0]][XmoveN2[i[0]]][0] >= 0 &&
								Xmove[i[0]][XmoveN2[i[0]]][0] <= note[i[0]][objectN[i[0]]].hittime &&
								Xmove[i[0]][XmoveN2[i[0]]][2] > note[i[0]][objectN[i[0]]].hittime) {
								note[i[0]][objectN[i[0]]].xpos = movecal(Xmove[i[0]][XmoveN2[i[0]]][3],
									Xmove[i[0]][XmoveN2[i[0]]][0], Xmove[i[0]][XmoveN2[i[0]] - 1][1],
									Xmove[i[0]][XmoveN2[i[0]]][2], Xmove[i[0]][XmoveN2[i[0]]][1],
									note[i[0]][objectN[i[0]]].hittime);
							}
							else {
								note[i[0]][objectN[i[0]]].xpos = Xmove[i[0]][XmoveN2[i[0]] - 1][1];
							}
							//���ʉ���ݒ肷��
							if (L'1' <= GT1[i[1]] && GT1[i[1]] <= L'9') {
								note[i[0]][objectN[i[0]]].sound = customnote[GT1[i[1]] - L'1'].sound;
								note[i[0]][objectN[i[0]]].melody = customnote[GT1[i[1]] - L'1'].melody;
							}
							else {
								note[i[0]][objectN[i[0]]].sound = 0;
							}
							//�F��ݒ肷��
							if (L'1' <= GT1[i[1]] && GT1[i[1]] <= L'9') {
								note[i[0]][objectN[i[0]]].color = customnote[GT1[i[1]] - L'1'].color;
							}
							else {
								note[i[0]][objectN[i[0]]].color = 0;
							}
							if (note[i[0]][objectN[i[0]]].object != 8) {
								notes++;
							}
							objectN[i[0]]++;
							allnum.notenum[i[0]]++;
						}
						if (i[0] <= 1) FileRead_gets(GT1, 256, songdata);
					}
					timer[0] = timer[1] = timer[2] += 240000.0 / bpmG;
				}
			}
		}
	}
	FileRead_close(songdata);
	//���ʂ̍Ō��goust��u��
	for (i[0] = 0; i[0] < 3; i[0]++) {
		note[i[0]][objectN[i[0]]].hittime = timer[i[0]];
		note[i[0]][objectN[i[0]] + 1].hittime = -1;
		note[i[0]][objectN[i[0]]].object = 8;
		note[i[0]][objectN[i[0]]].ypos = 1000;
	}
	lock[0][0][lockN[0]] = 1;
	lock[0][1][lockN[0]] = -1;
	lock[1][0][lockN[1]] = -1;
	lock[1][1][lockN[1]] = -1;
	allnum.notenum[0]++;
	allnum.notenum[1]++;
	allnum.notenum[2]++;
	Etime = timer[0];

	/*��Փx�v�Z
	(3000000/�ЂƂO�̃m�[�c�Ƃ̊Ԋu)���A���̃m�[�c�̓�Փx�_���Ƃ���B
	�@�����L�[��50�L�����A���̑g�ݍ��킹�łł����ō��l���Ȃ̓�Փx�B
	1�O�Ɠ����L�[�̎���(�Ώۂ̃L�[��arrow�̂�)
	���Ԋu��150ms�ȏ�̎��͕ϓ��Ȃ�(BPM200��8��)
	���Ԋu��150ms�ȉ�75ms�ȏ�̎��͓��_("�Ԋu"/(-75)+3)�{(1�`2�{)
	���Ԋu��75ms�ȉ��̎��͓��_2�{(BPM200��16��)
	1=hit,2=non,3=up,4=down,5=left,6=right,7=non or down,8=up or down,9=up or non
	*/

#define ARROW_TRICK_MLP(base) (base * 18 / 10) /* arrow�Ђ�������1.8�{ */
#define DIF_BBEF_MLP(base) (base * 12 / 10) /* 2�O�ƈႤ�L�[�̎��͓��_1.2�{(�S�L�[�Ώ�) */
#define ARROW_MLP(base) (base * 12 / 10) /* arrow�L�[�͓��_1.2�{ */

	/* ���̑��{����recp_cal_ddif.cpp�ɋL�� */

	objectN[0] = 0;
	objectN[1] = 0;
	objectN[2] = 0;
	difkey[0][3] = notes;
	if (difkey[0][3] > 49)difkey[0][3] = 49;
	difkey[7][3] = (Etime - noteoff) / 25 * 2;
	if (difkey[7][3] < 10000)difkey[7][3] = 10000;
	//�m�[�c���Ȃ��Ȃ�܂ŌJ��Ԃ�
	while (note[0][objectN[0]].hittime >= 0 ||
		note[1][objectN[1]].hittime >= 0 ||
		note[2][objectN[2]].hittime >= 0) {
		//GHOST�m�[�c���X�L�b�v
		for (i[0] = 0; i[0] < 3; i[0]++) {
			while (note[i[0]][objectN[i[0]]].object == 8 &&
				note[i[0]][objectN[i[0]]].hittime >= 0) {
				objectN[i[0]]++;
			}
		}
		G[0] = -1;
		//��ԑ����m�[�c��T����G[0]�ɑ��
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (note[i[0]][objectN[i[0]]].hittime >= 0) {
				G[0] = i[0];
				break;
			}
		}
		//����������u���[�N
		if (G[0] == -1) break;
		//��ԑ����m�[�c��T����G[0]�ɑ��
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (G[0] != i[0] && note[G[0]][objectN[G[0]]].hittime >
				note[i[0]][objectN[i[0]]].hittime &&
				note[i[0]][objectN[i[0]]].hittime >= 0) {
				G[0] = i[0];
			}
			else if (G[0] != i[0] &&
				note[G[0]][objectN[G[0]]].hittime ==
				note[i[0]][objectN[i[0]]].hittime &&
				note[G[0]][objectN[G[0]]].object == 2 &&
				note[i[0]][objectN[i[0]]].object != 2 &&
				note[i[0]][objectN[i[0]]].hittime >= 0) {
				G[0] = i[0];
			}
		}
		//ddif�̌v�Z
		while (note[G[0]][objectN[G[0]]].hittime >=
			(Etime - noteoff) / 25 * ddif2.nowdifsection + noteoff) {
			ddif[ddif2.nowdifsection - 1] = cal_ddif(ddif2.datanum, difkey[0],
				Etime, noteoff, ddif2.nowdifsection, difkey[7][3]);
			ddif2.nowdifsection++;
		}
		difkey[difkey[1][3]][0] = note[G[0]][objectN[G[0]]].object;
		difkey[difkey[1][3]][1] = note[G[0]][objectN[G[0]]].hittime;

		G[2] = MapErrorCheck(difkey[difkey[1][3]][0], difkey[difkey[1][3]][1],
			difkey[difkey[2][3]][0], difkey[difkey[2][3]][1], o, waningLv);
		if (G[2] != 0 && outpoint[1] == 0) {
			outpoint[0] = difkey[difkey[1][3]][1];
			outpoint[1] = G[2];
		}

		//hit�m�[�c���
		if (difkey[difkey[1][3]][0] == 1) {
			if (difkey[difkey[2][3]][0] == 1 &&
				difkey[difkey[1][3]][1] - 20 < difkey[difkey[2][3]][1]) {
				difkey[difkey[2][3]][2] *= 1;
				objectN[G[0]]++;
				continue;
			}
		}
		//catch�m�[�c���
		else if (difkey[difkey[1][3]][0] == 2) {
			if (G[0] != 1) difkey[difkey[1][3]][0] = G[0] / 2 + 3;
			if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0]) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 3 &&
				(difkey[difkey[2][3]][0] == 8 ||
				difkey[difkey[2][3]][0] == 9)) {
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
		//left�m�[�c���
		else if (difkey[difkey[1][3]][0] == 5) {
			if (difkey[difkey[2][3]][0] == 2 || difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9) {
				difkey[difkey[2][3]][0] = 5;
				continue;
			}
		}
		//right�m�[�c���
		else if (difkey[difkey[1][3]][0] == 6) {
			if (difkey[difkey[2][3]][0] == 2 || difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9) {
				difkey[difkey[2][3]][0] = 6;
				continue;
			}
		}
		//bomb�m�[�c���
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
		/* calculate difkey */
		if (difkey[2][3] != -1 && difkey[3][3] != -1) {
#if 1
			difkey[difkey[1][3]][2] = cal_difkey(difkey[difkey[1][3]][1],
				difkey[difkey[2][3]][1], difkey[difkey[1][3]][0],
				difkey[difkey[2][3]][0], difkey[difkey[3][3]][0],
				difkey[difkey[2][3]][2]);
#else
			G[1] = difkey[difkey[1][3]][1] - difkey[difkey[2][3]][1];
			if (G[1] == 0)G[1] = 1;
			if (G[1] <= 5 &&
				(difkey[difkey[1][3]][0] != difkey[difkey[2][3]][0] ||
				(difkey[difkey[1][3]][0] == 1 && difkey[difkey[2][3]][0] == 1))) {
				if (3 <= difkey[difkey[1][3]][0] && difkey[difkey[1][3]][0] <= 6) {
					difkey[difkey[1][3]][2] = difkey[difkey[2][3]][2] * 1.6;
				}
				else {
					difkey[difkey[1][3]][2] = difkey[difkey[2][3]][2] * 1.3;
				}
			}
			else {
				difkey[difkey[1][3]][2] = 3000000 / G[1];
				if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0] && difkey[difkey[1][3]][0] >= 3 && difkey[difkey[1][3]][0] <= 6 && difkey[difkey[1][3]][2] >= 40000) difkey[difkey[1][3]][2] *= 2;
				else if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0] && difkey[difkey[1][3]][0] >= 3 && difkey[difkey[1][3]][0] <= 6 && difkey[difkey[1][3]][2] <= 40000 && difkey[difkey[1][3]][2] >= 20000) difkey[difkey[1][3]][2] = difkey[difkey[1][3]][2] / -75.0 + 3.0;
				if (difkey[difkey[1][3]][0] != difkey[difkey[3][3]][0]) difkey[difkey[1][3]][2] *= 1.2;
				if (difkey[difkey[1][3]][0] >= 3 && difkey[difkey[1][3]][0] <= 6)difkey[difkey[1][3]][2] *= 1.2;
			}
#endif
		}
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (note[G[0]][objectN[G[0]]].object >= 3 && note[G[0]][objectN[G[0]]].object <= 6 &&
				G[0] != i[0] && note[G[0]][objectN[G[0]]].object == note[i[0]][objectN[i[0]]].object &&
				note[G[0]][objectN[G[0]]].hittime + 5 >= note[i[0]][objectN[i[0]]].hittime) {
				objectN[i[0]]++;
			}
		}
		objectN[G[0]]++;
		ddif2.datanum++;
		G[0] = 0;
		ddif2.maxdif = mins(ddif2.maxdif,
			cal_nowdif_m(difkey[0], difkey[0][3], difkey[1][3], difkey[7][3]));
		for (i[0] = 1; i[0] < 4; i[0]++) {
			difkey[i[0]][3]++;
			if (difkey[i[0]][3] > difkey[0][3])difkey[i[0]][3] = 0;
		}
	}
	ddif2.datanum++;
	for (i[0] = 0; i[0] < 2; i[0]++)if (difkey[i[0]][2] == 0 && difkey[2][2] > 0)ddif2.datanum--;
	if (ddif2.datanum < 1)ddif2.datanum = 1;
	if (ddif2.datanum > 50)ddif2.datanum = 50;
	ddifG[1] = ddif2.maxdif;
	if (ddifG[1] <= 0)ddifG[1] = 1;
	ddif2.maxdif /= 50;
	ddif[ddif2.nowdifsection - 1] = 0;
	for (i[0] = 0; i[0] < ddif2.datanum; i[0]++) {
		if (difkey[i[0]][1] > Etime - difkey[7][3]) {
			ddif[ddif2.nowdifsection - 1] += difkey[i[0]][2];
		}
	}
	for (i[0] = ddif2.nowdifsection - 1; i[0] <= 24; i[0]++) {
		ddif[i[0]] = ddif[ddif2.nowdifsection - 1];
	}
	ddif2.lastdif = ddif[ddif2.nowdifsection - 1] / 50;
	//NEEDYOU:Lv.1.0->2713, Co-op katyohugetsu:Lv.8.0->34733
	ddif2.maxdif = lins(2713, 100, 34733, 800, ddif2.maxdif);
	ddif2.lastdif = lins(2713, 100, 34733, 800, ddif2.lastdif);

	//��������PC�p���ʃf�[�^�̃t�@�C���̍쐬(�Z�[�u���)
	strcopy(dataE, RRS, 1);
	strcats(RRS, GT26[o]);
	G[2] = _wfopen_s(&fp, RRS, L"wb");
	fwrite(&allnum, sizeof(playnum_box), 1, fp);//�e�f�[�^�̌�
	fwrite(&mp3FN, 255, 1, fp);//���y�t�@�C����
	fwrite(&bpm, sizeof(double), 1, fp);//BPM
	fwrite(&noteoff, sizeof(int), 1, fp);//offset
	fwrite(&skyFN, 255, 1, fp);//��w�i��
	fwrite(&groundFN, 255, 1, fp);//�n�ʉ摜��
	fwrite(&waterFN, 255, 1, fp);//�����摜��
	fwrite(&songN, 255, 1, fp);//�Ȗ�
	fwrite(&songNE, 255, 1, fp);//�Ȗ�(�p��)
	fwrite(&Lv, sizeof(short int), 1, fp);//���x��
	//fwrite(&item, sizeof(int), 99, fp);//�A�C�e���摜�f�[�^(���얢�m�F)
	fwrite(&fall, sizeof(int), 198, fp);//�������w�i�؂�ւ��^�C�~���O
	fwrite(&speedt, sizeof(double), 990, fp);//���[�����x
	fwrite(&chamo, sizeof(int), 594, fp);//�L�����O���ϊ��^�C�~���O
	fwrite(&Ymove[0], sizeof(int), allnum.Ymovenum[0] * 4, fp);//�ヌ�[���c�ʒu�ړ��^�C�~���O
	fwrite(&Ymove[1], sizeof(int), allnum.Ymovenum[1] * 4, fp);//�����[���c�ʒu�ړ��^�C�~���O
	fwrite(&Ymove[2], sizeof(int), allnum.Ymovenum[2] * 4, fp);//�����[���c�ʒu�ړ��^�C�~���O
	fwrite(&Ymove[3], sizeof(int), allnum.Ymovenum[3] * 4, fp);//�n�ʏc�ʒu�ړ��^�C�~���O
	fwrite(&Ymove[4], sizeof(int), allnum.Ymovenum[4] * 4, fp);//���ʏc�ʒu�ړ��^�C�~���O
	fwrite(&Xmove[0], sizeof(int), allnum.Xmovenum[0] * 4, fp);//�ヌ�[�����ʒu�ړ��^�C�~���O
	fwrite(&Xmove[1], sizeof(int), allnum.Xmovenum[1] * 4, fp);//�����[�����ʒu�ړ��^�C�~���O
	fwrite(&Xmove[2], sizeof(int), allnum.Xmovenum[2] * 4, fp);//�����[�����ʒu�ړ��^�C�~���O
	fwrite(&lock, sizeof(int), 396, fp);//�m�[�c�Œ�؂�ւ��^�C�~���O
	fwrite(&carrow, sizeof(int), 198, fp);//�L���������؂�ւ��^�C�~���O
	fwrite(&viewT, sizeof(int), 198, fp);//�m�[�c�\�����ԕϊ��^�C�~���O
	fwrite(&note[0], sizeof(struct note_box), allnum.notenum[0], fp); /* �ヌ�[���m�[�c�f�[�^ */
	fwrite(&note[1], sizeof(struct note_box), allnum.notenum[1], fp); /* �����[���m�[�c�f�[�^ */
	fwrite(&note[2], sizeof(struct note_box), allnum.notenum[2], fp); /* �����[���m�[�c�f�[�^ */
	fwrite(&notes, sizeof(short int), 1, fp);//�m�[�c��
	fwrite(&Etime, sizeof(int), 1, fp);//�ȏI������
	G[0] = ddif2.maxdif;//�ō���Փx
	G[1] = ddif2.lastdif;//�ŏI��Փx
	fwrite(&G, sizeof(int), 2, fp);
	fwrite(&ddif, sizeof(int), 25, fp);//�e��ԓ�Փx�f�[�^
	fwrite(&ddif2.nowdifsection, sizeof(int), 1, fp);//�e��ԓ�Փx�f�[�^
	fwrite(&ddifG[1], sizeof(int), 1, fp);//�e��ԓ�Փx�f�[�^
	fwrite(&DifFN, 255, 1, fp);//��Փx�o�[��
	fwrite(&Movie, sizeof(item_box), allnum.movienum, fp);//����f�[�^
	fwrite(&camera, sizeof(struct camera_box), 255, fp);//�J�����f�[�^
	fwrite(&scrool, sizeof(struct scrool_box), 99, fp);//�X�N���[���f�[�^
	fwrite(&v_bpm, sizeof(view_BPM_box), allnum.v_BPMnum, fp);//�����ڂ�BPM�f�[�^
	fwrite(&outpoint, sizeof(int), 2, fp);//���ʃG���[
	fclose(fp);
	return;
}

int IsNoteCode(wchar_t c) {
	if (c == L'H' || c == L'C' || c == L'U' || c == L'D' || c == L'L' || c == L'R' || c == L'B' ||
		c == L'G' || c == L'?' || c == L'!' || (L'1' <= c && c <= L'9')) {
		return 1;
	}
	return 0;
}

int cal_ddif(int num, int const *difkey, int Etime, int noteoff, int difsec, int voidtime) {
	int ret = 0;
	int count = 0;
	if (num >= 50) {
		num = 49;
	}
	for (int i = 0; i < num; i++) {
		if (difkey[i * 4 + 1] > (Etime - noteoff) / 25 *
			difsec - voidtime + noteoff) {
			count++;
			ret += difkey[i * 4 + 2];
		}
	}
	if (count == 0) {
		return 0;
	}
	else {
		return ret * 50 / count;
	}
}

int cal_nowdif_m(int *difkey, int num, int now, int voidtime) {
	int ret = 0;
	int count = 0;
	for (int i = 0; i <= num; i++) {
		if (difkey[i * 4 + 2] < 0) {
			break;
		}
		if (difkey[i * 4 + 1] > difkey[now * 4 + 1] - voidtime) {
			ret += difkey[i * 4 + 2];
			count++;
		}
	}
	if (count == 0) {
		return 0;
	}
	else {
		return ret * 50 / count;
	}
}

item_eff_box set_pic_mat(wchar_t *s) {
	item_eff_box eff;
	while (s[0] != L'\0' && s[0] != L'\n') {
		if (strands(s, L"bpm_a")) {
			eff.bpm_alphr = 1;
		}
		else if (strands(s, L"bpm_s")) {
			eff.bpm_size = 1;
		}
		else if (strands(s, L"lock")) {
			eff.lock = 1;
		}
		else if (strands(s, L"cha_a")) {
			eff.chara_alphr = 1;
		}
		else if (strands(s, L"edge_s")) {
			eff.edge_size = 1;
		}
		else {
			break;
		}
		strnex(s);
	}
	return eff;
}

int MapErrorCheck(int nownote, int nowtime, int befnote, int beftime, int dif, int wl) {
	if (nowtime <= 0 || beftime <= 0) {
		return 0;
	}
	switch (dif * 10 + wl) {
	case 11:
		if (nownote == 1 && befnote == 1 && nowtime - beftime <= 1200) {
			return HITNOTETOONEAR;
		}
		break;
	case 12:
		if (nownote == 1 && befnote == 1 && nowtime - beftime <= 600) {
			return HITNOTETOONEAR;
		}
		break;
	case 13:
		if (nownote == 1 && befnote == 1 && nowtime - beftime <= 300) {
			return HITNOTETOONEAR;
		}
		break;
	case 21:
		if (nownote == 1 && befnote == 1 && nowtime - beftime <= 600) {
			return HITNOTETOONEAR;
		}
		break;
	case 22:
		if (nownote == 1 && befnote == 1 && nowtime - beftime <= 300) {
			return HITNOTETOONEAR;
		}
		break;
	case 23:
		if (nownote == 1 && befnote == 1 && nowtime - beftime <= 150) {
			return HITNOTETOONEAR;
		}
		break;
	default: break;
	}
	return 0;
}
