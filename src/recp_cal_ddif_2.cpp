
#include <sancur.h>
#include <playbox.h>
#include <RecScoreFile.h>
#include <recp_cal_difkey.h>
#include <recp_cal_ddif.h>
#include <noteLoad.h>

typedef struct ddef_box_2 {
	int maxdif = 0;
	int lastdif = 0;
	int nowdifsection = 1;
	int datanum = -1;
} ddef_box_2;

static int cal_nowdif_m_2(int *difkey, int num, int now, int voidtime) {
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

static int cal_ddif_2(int num, int const *difkey, int Etime, int noteoff, int difsec, int voidtime) {
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

int cal_ddif_3(wchar_t path[]) {
	playnum_box allnum;
	struct note_box note[3][2000];//[��,��,��]���[���̃m�[�c[�ԍ�]
	short int Lv = 0;
	short int notes = 0;
	int objectN[3] = { 0,0,0 }; //���̔ԍ�
	int G[10] = { 0,0,0,0,0,0,0,0,0,0 };
	int i[5] = { 0,0,0,0,0 };
	int Etime = 0; //���ʂ̏I���̎���
	int noteoff = 0; //�m�[�c�̃I�t�Z�b�g
	int difkey[50][4];//��Փx�v�Z�Ɏg��[�ԍ�][���̓L�[,����,��Փx�_,[0]�����:[1]���̔ԍ�:[2]1�O�̔ԍ�:[3]2�O�̔ԍ�:[4]�ō��_:[5]�f�[�^��:[6]�Ō�50�̍��v:[7]�v�Z���珜�O���鎞��]
	difkey[0][2] = 0;
	difkey[1][2] = 0;
	difkey[1][3] = 0;
	difkey[2][3] = -1;
	difkey[3][3] = -2;
	difkey[4][3] = 0;
	int ddif[25] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//�e��Ԃ̓�Փx
	int ddifG[2] = { 1,1 };//0=�������Ԕԍ�(1�`25),1=�ő�l
	int fall[99][2]; //�������w�i��[0:�ԍ�,1:����]
	int chamo[3][99][2]; //�L������[0:��,1:��,2:��]��[0:�O���t�B�b�N,1:�؂�ւ�����]
	int Ymove[5][999][4]; //[��, ��, ��, (�n��), (����)]���[���c�ړ���[0:�J�n����,1:�ʒu,2:�I������,3:���]
	int Xmove[3][999][4]; //[��, ��, ��]���[�����ړ���[0:�J�n����,1:�ʒu,2:�I������,3:���]
	int lock[2][2][99]; //lock = [��,�c]�̉����̈ʒu��[(1=�Œ肷��,-1�ȊO=�Œ肵�Ȃ�),����]
	int carrow[2][99];
	int viewT[2][99];//[�����\������,���s����,[0]=���i���o�[]
	int outpoint[2] = { 0, 0 }; /* 0=����, 1=�G���[�ԍ� */
	double bpm = 120;
	double speedt[5][99][2]; //[��, ��, ��, (�n��), (����)]���[����[0:�؂�ւ�����,1:���x]
	wchar_t mp3FN[255];
	wchar_t skyFN[255] = L"picture/backskynoamal.png";
	wchar_t groundFN[255] = L"picture/groundnaturenormal.png";
	wchar_t waterFN[255] = L"picture/waternormal.png";
	wchar_t songN[255];
	wchar_t songNE[255];
	wchar_t DifFN[255] = L"picture/difanother.png";
	ddef_box_2 ddif2;
	item_box Movie[999];//�A�C�e���\��[�A�C�e���ԍ�,�ړ��`��,�J�n����,�I������,�J�nx�ʒu,�I��x�ʒu,�J�ny�ʒu,�I��y�ʒu,�J�n�T�C�Y,�I���T�C�Y,�J�n�p�x,�I���p�x,�J�n�����x,�I�������x]
	struct camera_box camera[255];
	struct scrool_box scrool[99];
	view_BPM_box v_bpm[100];
	FILE *fp;

	int test[10] = { 0,0,0,0,0,0,0,0,0,0 };

	//���ʃ��[�h
	_wfopen_s(&fp, path, L"rb");//rrs�f�[�^��ǂݍ���
	if (fp == NULL) {
		return 0;
	}
	fread(&allnum, sizeof(playnum_box), 1, fp);//�e�f�[�^�̌�
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
	fread(&Ymove[0], sizeof(int), allnum.Ymovenum[0] * 4, fp);//�ヌ�[���c�ʒu�ړ��^�C�~���O
	fread(&Ymove[1], sizeof(int), allnum.Ymovenum[1] * 4, fp);//�����[���c�ʒu�ړ��^�C�~���O
	fread(&Ymove[2], sizeof(int), allnum.Ymovenum[2] * 4, fp);//�����[���c�ʒu�ړ��^�C�~���O
	fread(&Ymove[3], sizeof(int), allnum.Ymovenum[3] * 4, fp);//�n�ʏc�ʒu�ړ��^�C�~���O
	fread(&Ymove[4], sizeof(int), allnum.Ymovenum[4] * 4, fp);//���ʏc�ʒu�ړ��^�C�~���O
	fread(&Xmove[0], sizeof(int), allnum.Xmovenum[0] * 4, fp);//�ヌ�[�����ʒu�ړ��^�C�~���O
	fread(&Xmove[1], sizeof(int), allnum.Xmovenum[1] * 4, fp);//�����[�����ʒu�ړ��^�C�~���O
	fread(&Xmove[2], sizeof(int), allnum.Xmovenum[2] * 4, fp);//�����[�����ʒu�ړ��^�C�~���O
	fread(&lock, sizeof(int), 396, fp);//�m�[�c�Œ�؂�ւ��^�C�~���O
	fread(&carrow, sizeof(int), 198, fp);//�L���������؂�ւ��^�C�~���O
	fread(&viewT, sizeof(int), 198, fp);//�m�[�c�\�����ԕϊ��^�C�~���O
#if SWITCH_NOTE_BOX_2 == 1
	noteLoadOld(&note[0][0], &note[1][0], &note[2][0],
		allnum.notenum[0] + allnum.notenum[1] + allnum.notenum[2], fp);
#else
	fread(&note[0], sizeof(struct note_box), allnum.notenum[0], fp); /* �ヌ�[���m�[�c�f�[�^ */
	fread(&note[1], sizeof(struct note_box), allnum.notenum[1], fp); /* �����[���m�[�c�f�[�^ */
	fread(&note[2], sizeof(struct note_box), allnum.notenum[2], fp); /* �����[���m�[�c�f�[�^ */
#endif
	fread(&notes, sizeof(short int), 1, fp);//�m�[�c��
	fread(&Etime, sizeof(int), 1, fp);//�ȏI������
	fread(&G, sizeof(int), 2, fp);
	difkey[4][3] = G[0];//�ō���Փx
	difkey[6][3] = G[1];//�ŏI��Փx
	fread(&ddif, sizeof(int), 25, fp);//�e��ԓ�Փx�f�[�^
	fread(&ddifG, sizeof(int), 2, fp);//�e��ԓ�Փx�f�[�^
	fread(&DifFN, 255, 1, fp);//��Փx�o�[��
	fread(&Movie, sizeof(item_box), allnum.movienum, fp);//�A�C�e���f�[�^
	fread(&camera, sizeof(struct camera_box), 255, fp);//�J�����f�[�^
	fread(&scrool, sizeof(struct scrool_box), 99, fp);//�X�N���[���f�[�^
	fread(&v_bpm, sizeof(view_BPM_box), allnum.v_BPMnum, fp);//�����ڂ�BPM�f�[�^
	fread(&outpoint, sizeof(int), 2, fp);//�G���[�f�[�^
	fclose(fp);

	//�e�����l
	objectN[0] = 0;
	objectN[1] = 0;
	objectN[2] = 0;
	difkey[0][3] = notes;
	if (difkey[0][3] > 49)difkey[0][3] = 49;
	difkey[7][3] = (Etime - noteoff) / 25 * 2;
	if (difkey[7][3] < 10000)difkey[7][3] = 10000;
	DifkeyCalInit(notes, Etime - noteoff);
	//�m�[�c���Ȃ��Ȃ�܂ŌJ��Ԃ�
	while (note[0][objectN[0]].hittime >= 0 ||
		note[1][objectN[1]].hittime >= 0 ||
		note[2][objectN[2]].hittime >= 0) {
		G[9]++;
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
		if (G[0] == -1) { break; }
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
		DifkeyCalInsertNote(&note[G[0]][objectN[G[0]]], G[0]);
		//ddif�̌v�Z
		while (note[G[0]][objectN[G[0]]].hittime >=
			(Etime - noteoff) / 25 * ddif2.nowdifsection + noteoff) {
			ddif[ddif2.nowdifsection - 1] = cal_ddif_2(ddif2.datanum, difkey[0],
				Etime, noteoff, ddif2.nowdifsection, difkey[7][3]);
			ddif2.nowdifsection++;
		}
		difkey[difkey[1][3]][0] = note[G[0]][objectN[G[0]]].object;
		difkey[difkey[1][3]][1] = note[G[0]][objectN[G[0]]].hittime;

		switch (difkey[difkey[1][3]][0]) {
		case 1: //hit�m�[�c���
			if (difkey[difkey[2][3]][0] == 1 &&
				difkey[difkey[1][3]][1] - 20 < difkey[difkey[2][3]][1]) {
				difkey[difkey[2][3]][2] *= 1;
				objectN[G[0]]++;
				continue;
			}
			break;
		case 2: //catch�m�[�c���
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
			break;
		case 5: //left�m�[�c���
			if (difkey[difkey[2][3]][0] == 2 || difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9) {
				difkey[difkey[2][3]][0] = 5;
				continue;
			}
			break;
		case 6: //right�m�[�c���
			if (difkey[difkey[2][3]][0] == 2 || difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9) {
				difkey[difkey[2][3]][0] = 6;
				continue;
			}
			break;
		case 7: //bomb�m�[�c���
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
			break;
		}
		/* calculate difkey */
		if (difkey[2][3] != -1 && difkey[3][3] != -1) {
			difkey[difkey[1][3]][2] = cal_difkey(difkey[difkey[1][3]][1],
				difkey[difkey[2][3]][1], difkey[difkey[1][3]][0],
				difkey[difkey[2][3]][0], difkey[difkey[3][3]][0],
				difkey[difkey[2][3]][2]);
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
			cal_nowdif_m_2(difkey[0], difkey[0][3], difkey[1][3], difkey[7][3]));
		for (i[0] = 1; i[0] < 4; i[0]++) {
			difkey[i[0]][3]++;
			if (difkey[i[0]][3] > difkey[0][3]) { difkey[i[0]][3] = 0; }
		}
	}
	ddif2.datanum++;
	for (i[0] = 0; i[0] < 2; i[0]++) {
		if (difkey[i[0]][2] == 0 && difkey[2][2] > 0) { ddif2.datanum--; }
	}
	if (ddif2.datanum < 1) { ddif2.datanum = 1; }
	if (ddif2.datanum > 50) { ddif2.datanum = 50; }
	ddifG[1] = ddif2.maxdif;
	if (ddifG[1] <= 0) { ddifG[1] = 1; }
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

	//���ʃZ�[�u
	_wfopen_s(&fp, path, L"wb");
	if (fp == NULL) { return 0; }
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
#if SWITCH_NOTE_BOX_2 == 1
	noteSaveOld(&note[0][0], &note[1][0], &note[2][0], &allnum, fp);
#else
	fwrite(&note[0], sizeof(struct note_box), allnum.notenum[0], fp); /* �ヌ�[���m�[�c�f�[�^ */
	fwrite(&note[1], sizeof(struct note_box), allnum.notenum[1], fp); /* �����[���m�[�c�f�[�^ */
	fwrite(&note[2], sizeof(struct note_box), allnum.notenum[2], fp); /* �����[���m�[�c�f�[�^ */
#endif
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

	return 0;
}
