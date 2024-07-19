
#include <stdio.h>
#include "RecScoreFile.h"

/* 0: OK, -1: error */
int rec_score_fread(rec_score_file_t *recfp, FILE *fp) {
	if (recfp == NULL || fp == NULL) { return -1; }

	fread(&recfp->allnum, sizeof(playnum_box), 1, fp);//�e�f�[�^�̌�
	fread(&recfp->nameset.mp3FN, 255, 1, fp);//���y�t�@�C����
	fread(&recfp->mapdata.bpm, sizeof(double), 1, fp);//BPM
	fread(&recfp->time.offset, sizeof(int), 1, fp);//offset
	fread(&recfp->nameset.sky, 255, 1, fp);//��w�i��
	fread(&recfp->nameset.ground, 255, 1, fp);//�n�ʉ摜��
	fread(&recfp->nameset.water, 255, 1, fp);//�����摜��
	fread(&recfp->nameset.songN, 255, 1, fp);//�Ȗ�
	fread(&recfp->nameset.songNE, 255, 1, fp);//�Ȗ�(�p��)
	fread(&recfp->mapdata.Lv, sizeof(short int), 1, fp);//���x��
	//fread(&item, sizeof(int), 99, fp);//�A�C�e���摜�f�[�^(���얢�m�F)
	{
		int buf[99][2];
		fread(buf, sizeof(int), 198, fp);//�������w�i�؂�ւ��^�C�~���O
		for (int i = 0; i < 99; i++) {
			recfp->mapeff.fall.d[i].time = buf[i][0];
			recfp->mapeff.fall.d[i].No   = buf[i][1];
		}
	}
	fread(&recfp->mapeff.speedt, sizeof(double), 990, fp);//���[�����x
	{
		int buf[3][99][2];
		fread(buf, sizeof(int), 594, fp);//�L�����O���ϊ��^�C�~���O
		for (int i = 0; i < 99; i++) {
			recfp->mapeff.chamo[0].gra[i]  = buf[0][i][0];
			recfp->mapeff.chamo[0].time[i] = buf[0][i][1];
			recfp->mapeff.chamo[1].gra[i]  = buf[1][i][0];
			recfp->mapeff.chamo[1].time[i] = buf[1][i][1];
			recfp->mapeff.chamo[2].gra[i]  = buf[2][i][0];
			recfp->mapeff.chamo[2].time[i] = buf[2][i][1];
		}
	}
	{
#if 0
		int buf[999][4];
		fread(buf, sizeof(int), recfp->allnum.Ymovenum[0] * 4, fp);//�ヌ�[���c�ʒu�ړ��^�C�~���O
		for (int i = 0; i < recfp->allnum.Ymovenum[0]; i++) {
			recfp->mapeff.move.y[0].d[i].Stime = buf[i][0];
			recfp->mapeff.move.y[0].d[i].pos = buf[i][1];
			recfp->mapeff.move.y[0].d[i].Etime = buf[i][2];
			recfp->mapeff.move.y[0].d[i].mode = buf[i][3];
		}
		fread(buf, sizeof(int), recfp->allnum.Ymovenum[1] * 4, fp);//�����[���c�ʒu�ړ��^�C�~���O
		for (int i = 0; i < recfp->allnum.Ymovenum[1]; i++) {
			recfp->mapeff.move.y[1].d[i].Stime = buf[i][0];
			recfp->mapeff.move.y[1].d[i].pos = buf[i][1];
			recfp->mapeff.move.y[1].d[i].Etime = buf[i][2];
			recfp->mapeff.move.y[1].d[i].mode = buf[i][3];
		}
		fread(buf, sizeof(int), recfp->allnum.Ymovenum[2] * 4, fp);//�����[���c�ʒu�ړ��^�C�~���O
		for (int i = 0; i < recfp->allnum.Ymovenum[2]; i++) {
			recfp->mapeff.move.y[2].d[i].Stime = buf[i][0];
			recfp->mapeff.move.y[2].d[i].pos = buf[i][1];
			recfp->mapeff.move.y[2].d[i].Etime = buf[i][2];
			recfp->mapeff.move.y[2].d[i].mode = buf[i][3];
		}
		fread(buf, sizeof(int), recfp->allnum.Ymovenum[3] * 4, fp);//�n�ʏc�ʒu�ړ��^�C�~���O
		for (int i = 0; i < recfp->allnum.Ymovenum[3]; i++) {
			recfp->mapeff.move.y[3].d[i].Stime = buf[i][0];
			recfp->mapeff.move.y[3].d[i].pos = buf[i][1];
			recfp->mapeff.move.y[3].d[i].Etime = buf[i][2];
			recfp->mapeff.move.y[3].d[i].mode = buf[i][3];
		}
		fread(buf, sizeof(int), recfp->allnum.Ymovenum[4] * 4, fp);//���ʏc�ʒu�ړ��^�C�~���O
		for (int i = 0; i < recfp->allnum.Ymovenum[4]; i++) {
			recfp->mapeff.move.y[4].d[i].Stime = buf[i][0];
			recfp->mapeff.move.y[4].d[i].pos = buf[i][1];
			recfp->mapeff.move.y[4].d[i].Etime = buf[i][2];
			recfp->mapeff.move.y[4].d[i].mode = buf[i][3];
		}
		fread(buf, sizeof(int), recfp->allnum.Xmovenum[0] * 4, fp);//�ヌ�[�����ʒu�ړ��^�C�~���O
		for (int i = 0; i < recfp->allnum.Xmovenum[0]; i++) {
			recfp->mapeff.move.x[0].d[i].Stime = buf[i][0];
			recfp->mapeff.move.x[0].d[i].pos = buf[i][1];
			recfp->mapeff.move.x[0].d[i].Etime = buf[i][2];
			recfp->mapeff.move.x[0].d[i].mode = buf[i][3];
		}
		fread(buf, sizeof(int), recfp->allnum.Xmovenum[1] * 4, fp);//�����[�����ʒu�ړ��^�C�~���O
		for (int i = 0; i < recfp->allnum.Xmovenum[1]; i++) {
			recfp->mapeff.move.x[1].d[i].Stime = buf[i][0];
			recfp->mapeff.move.x[1].d[i].pos = buf[i][1];
			recfp->mapeff.move.x[1].d[i].Etime = buf[i][2];
			recfp->mapeff.move.x[1].d[i].mode = buf[i][3];
		}
		fread(buf, sizeof(int), recfp->allnum.Xmovenum[2] * 4, fp);//�����[�����ʒu�ړ��^�C�~���O
		for (int i = 0; i < recfp->allnum.Xmovenum[2]; i++) {
			recfp->mapeff.move.x[2].d[i].Stime = buf[i][0];
			recfp->mapeff.move.x[2].d[i].pos = buf[i][1];
			recfp->mapeff.move.x[2].d[i].Etime = buf[i][2];
			recfp->mapeff.move.x[2].d[i].mode = buf[i][3];
		}
#else
		fread(&recfp->mapeff.move.y[0].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[0], fp);//�ヌ�[���c�ʒu�ړ��^�C�~���O
		fread(&recfp->mapeff.move.y[1].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[1], fp);//�ヌ�[���c�ʒu�ړ��^�C�~���O
		fread(&recfp->mapeff.move.y[2].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[2], fp);//�ヌ�[���c�ʒu�ړ��^�C�~���O
		fread(&recfp->mapeff.move.y[3].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[3], fp);//�ヌ�[���c�ʒu�ړ��^�C�~���O
		fread(&recfp->mapeff.move.y[4].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[4], fp);//�ヌ�[���c�ʒu�ړ��^�C�~���O
		fread(&recfp->mapeff.move.x[0].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[0], fp);//�ヌ�[���c�ʒu�ړ��^�C�~���O
		fread(&recfp->mapeff.move.x[1].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[1], fp);//�ヌ�[���c�ʒu�ړ��^�C�~���O
		fread(&recfp->mapeff.move.x[2].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[2], fp);//�ヌ�[���c�ʒu�ړ��^�C�~���O
		recfp->mapeff.move.y[0].num = 0;
		recfp->mapeff.move.y[1].num = 0;
		recfp->mapeff.move.y[2].num = 0;
		recfp->mapeff.move.y[3].num = 0;
		recfp->mapeff.move.y[4].num = 0;
		recfp->mapeff.move.x[0].num = 0;
		recfp->mapeff.move.x[1].num = 0;
		recfp->mapeff.move.x[2].num = 0;
#endif
	}
	fread(&recfp->mapeff.lock, sizeof(int), 396, fp);//�m�[�c�Œ�؂�ւ��^�C�~���O
	{
		int buf[2][99];
		fread(buf, sizeof(int), 198, fp);//�L���������؂�ւ��^�C�~���O
		for (int i = 0; i < 99; i++) {
			recfp->mapeff.carrow.d[i].data = buf[0][i];
			recfp->mapeff.carrow.d[i].time = buf[1][i];
		}
	}
	fread(&recfp->mapeff.viewT, sizeof(int), 198, fp);//�m�[�c�\�����ԕϊ��^�C�~���O
#if SWITCH_NOTE_BOX_2 == 1
	fread(&recfp->mapdata.note, sizeof(note_box_2_t),
		recfp->allnum.notenum[0] + recfp->allnum.notenum[1] + recfp->allnum.notenum[2], fp); /* �m�[�c�f�[�^ */
#else
	fread(&recfp->mapdata.note2.up[0], sizeof(struct note_box), recfp->allnum.notenum[0], fp); /* �ヌ�[���m�[�c�f�[�^ */
	fread(&recfp->mapdata.note2.mid[0], sizeof(struct note_box), recfp->allnum.notenum[1], fp); /* �����[���m�[�c�f�[�^ */
	fread(&recfp->mapdata.note2.low[0], sizeof(struct note_box), recfp->allnum.notenum[2], fp); /* �����[���m�[�c�f�[�^ */
#endif
	fread(&recfp->mapdata.notes, sizeof(short int), 1, fp);//�m�[�c��
	fread(&recfp->time.end, sizeof(int), 1, fp);//�ȏI������
	{
		int buf[2];
		fread(buf, sizeof(int), 2, fp);
		recfp->mapdata.mdif = buf[0];//�ō���Փx
		recfp->mapdata.ldif = buf[1];//�ŏI��Փx
	}
	fread(&recfp->mapdata.ddif, sizeof(int), 25, fp);//�e��ԓ�Փx�f�[�^
	fread(&recfp->mapdata.ddifG, sizeof(int), 2, fp);//�e��ԓ�Փx�f�[�^
	fread(&recfp->nameset.DifFN, 255, 1, fp);//��Փx�o�[��
	fread(&recfp->mapeff.Movie, sizeof(item_box), recfp->allnum.movienum, fp);//�A�C�e���f�[�^
	fread(&recfp->mapeff.camera, sizeof(struct camera_box), 255, fp);//�J�����f�[�^
	fread(&recfp->mapeff.scrool, sizeof(struct scrool_box), 99, fp);//�X�N���[���f�[�^
	fread(&recfp->mapeff.v_BPM.data[0], sizeof(view_BPM_box), recfp->allnum.v_BPMnum, fp);//�����ڂ�BPM�f�[�^
	fread(&recfp->outpoint, sizeof(int), 2, fp);//�G���[�f�[�^

	return 0;
}
