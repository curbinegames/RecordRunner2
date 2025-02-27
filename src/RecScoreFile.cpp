
#include <stdio.h>
#include "system.h"
#include "RecScoreFile.h"

#define REC_LINE_READ(pointer, size, count, fp)  fread((pointer), (size), (count), (fp))
#define REC_LINE_WRITE(pointer, size, count, fp) fwrite((pointer), (size), (count), (fp))
#define REC_LINE_SEEK(pointer, size, count, fp)  fseek((fp), ((size) * (count)), SEEK_CUR)

/**
 * rrs�t�@�C������allnum��ǂݍ���
 * @return 0: OK, -1: error
 */
static int RecScoreGetAllnum(playnum_box *allnum, const TCHAR *path) {
	FILE *fp;

	_wfopen_s(&fp, path, L"rb");
	if (allnum == NULL || fp == NULL) { return -1; }
	fread(allnum, sizeof(playnum_box), 1, fp);
	fclose(fp);

	return 0;
}

/**
 * rrs�t�@�C������S�Ă̏���ǂݍ���
 * @return 0: OK, -1: error
 */
int rec_score_fread(rec_score_file_t *recfp, const TCHAR *path) {
	FILE *fp;

	_wfopen_s(&fp, path, L"rb");

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
	{
		double buf[5][99][2];
		fread(&buf, sizeof(double), 990, fp);//���[�����x
		for (int iLane = 0; iLane < 5; iLane++) {
			for (int inum = 0; inum < 99; inum++) {
				recfp->mapeff.speedt[iLane].d[inum].time  = buf[iLane][inum][0];
				recfp->mapeff.speedt[iLane].d[inum].speed = buf[iLane][inum][1];
			}
		}
	}
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
	fread(&recfp->mapeff.camera, sizeof(rec_camera_data_t), 255, fp);//�J�����f�[�^
	fread(&recfp->mapeff.scrool, sizeof(rec_scrool_data_t), 99, fp);//�X�N���[���f�[�^
	fread(&recfp->mapeff.v_BPM.data[0], sizeof(view_BPM_box), recfp->allnum.v_BPMnum, fp);//�����ڂ�BPM�f�[�^
	REC_LINE_READ(&recfp->mapeff.viewLine.d[0], sizeof(rec_viewline_data_t), 99, fp);//���C���K�C�h�̕\��/��\��
	fread(&recfp->outpoint, sizeof(int), 2, fp);//�G���[�f�[�^
	REC_LINE_READ(&recfp->mapdata.mpal, sizeof(rec_ddif_pal_t), 1, fp);//��Փx���̓f�[�^

	fclose(fp);

	return 0;
}

/**
 * rrs�t�@�C������S�Ă̏�����������
 * @return 0: OK, -1: error
 */
int rec_score_fwrite(rec_score_file_t *recfp, const TCHAR *path) {
	int dummy = 0;
	FILE *fp;

	_wfopen_s(&fp, path, L"wb");

	if (recfp == NULL || fp == NULL) { return -1; }

	fwrite(&recfp->allnum, sizeof(playnum_box), 1, fp);//�e�f�[�^�̌�
	fwrite(&recfp->nameset.mp3FN, 255, 1, fp);//���y�t�@�C����
	fwrite(&recfp->mapdata.bpm, sizeof(double), 1, fp);//BPM
	fwrite(&recfp->time.offset, sizeof(int), 1, fp);//offset
	fwrite(&recfp->nameset.sky, 255, 1, fp);//��w�i��
	fwrite(&recfp->nameset.ground, 255, 1, fp);//�n�ʉ摜��
	fwrite(&recfp->nameset.water, 255, 1, fp);//�����摜��
	fwrite(&recfp->nameset.songN, 255, 1, fp);//�Ȗ�
	fwrite(&recfp->nameset.songNE, 255, 1, fp);//�Ȗ�(�p��)
	fwrite(&recfp->mapdata.Lv, sizeof(short int), 1, fp);//���x��
	//fwrite(&item, sizeof(int), 99, fp);//�A�C�e���摜�f�[�^(���얢�m�F)
	{
		int buf[99][2];
		for (int inum = 0; inum < 99; inum++) {
			buf[inum][0] = recfp->mapeff.fall.d[inum].No;
			buf[inum][1] = recfp->mapeff.fall.d[inum].time;
		}
		fwrite(&buf, sizeof(int), 198, fp);//�������w�i�؂�ւ��^�C�~���O
	}
	{
		double buf[5][99][2];
		for (int iLane = 0; iLane < 5; iLane++) {
			for (int inum = 0; inum < 99; inum++) {
				buf[iLane][inum][0] = recfp->mapeff.speedt[iLane].d[inum].time;
				buf[iLane][inum][1] = recfp->mapeff.speedt[iLane].d[inum].speed;
			}
		}
		fwrite(&buf, sizeof(double), 990, fp);//���[�����x
	}
	{
		int buf[3][99][2];
		for (int ilane = 0; ilane < 3; ilane++) {
			for (int inum = 0; inum < 99; inum++) {
				buf[ilane][inum][0] = recfp->mapeff.chamo[ilane].gra[inum];
				buf[ilane][inum][1] = recfp->mapeff.chamo[ilane].time[inum];
			}
		}
		fwrite(&buf, sizeof(int), 594, fp);//�L�����O���ϊ��^�C�~���O
	}
	fwrite(&recfp->mapeff.move.y[0].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[0], fp);//�ヌ�[���c�ʒu�ړ��^�C�~���O
	fwrite(&recfp->mapeff.move.y[1].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[1], fp);//�����[���c�ʒu�ړ��^�C�~���O
	fwrite(&recfp->mapeff.move.y[2].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[2], fp);//�����[���c�ʒu�ړ��^�C�~���O
	fwrite(&recfp->mapeff.move.y[3].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[3], fp);//�n�ʏc�ʒu�ړ��^�C�~���O
	fwrite(&recfp->mapeff.move.y[4].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[4], fp);//���ʏc�ʒu�ړ��^�C�~���O
	fwrite(&recfp->mapeff.move.x[0].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[0], fp);//�ヌ�[�����ʒu�ړ��^�C�~���O
	fwrite(&recfp->mapeff.move.x[1].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[1], fp);//�����[�����ʒu�ړ��^�C�~���O
	fwrite(&recfp->mapeff.move.x[2].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[2], fp);//�����[�����ʒu�ړ��^�C�~���O
	fwrite(&recfp->mapeff.lock, sizeof(int), 396, fp);//�m�[�c�Œ�؂�ւ��^�C�~���O
	{
		int buf[2][99];
		for (int inum = 0; inum < 99; inum++) {
			buf[0][inum] = recfp->mapeff.carrow.d[inum].data;
			buf[1][inum] = recfp->mapeff.carrow.d[inum].time;
		}
		fwrite(&buf, sizeof(int), 198, fp);//�L���������؂�ւ��^�C�~���O
	}
	fwrite(&recfp->mapeff.viewT, sizeof(int), 198, fp);//�m�[�c�\�����ԕϊ��^�C�~���O
#if SWITCH_NOTE_BOX_2
	fwrite(&recfp->mapdata.note, sizeof(note_box_2_t), recfp->allnum.notenum[0] + recfp->allnum.notenum[1] + recfp->allnum.notenum[2], fp); /* �m�[�c�f�[�^ */
#else
	fwrite(&recfp->mapdata.note[0], sizeof(struct note_box), recfp->allnum.notenum[0], fp); /* �ヌ�[���m�[�c�f�[�^ */
	fwrite(&recfp->mapdata.note[1], sizeof(struct note_box), recfp->allnum.notenum[1], fp); /* �����[���m�[�c�f�[�^ */
	fwrite(&recfp->mapdata.note[2], sizeof(struct note_box), recfp->allnum.notenum[2], fp); /* �����[���m�[�c�f�[�^ */
#endif
	fwrite(&recfp->mapdata.notes, sizeof(short int), 1, fp);//�m�[�c��
	fwrite(&recfp->time.end, sizeof(int), 1, fp);//�ȏI������
	fwrite(&recfp->mapdata.ddifG[0], sizeof(int), 1, fp);//�ō���Փx
	fwrite(&recfp->mapdata.ddifG[1], sizeof(int), 1, fp);//�ŏI��Փx
	fwrite(&recfp->mapdata.ddif, sizeof(int), 25, fp);//�e��ԓ�Փx�f�[�^
	fwrite(&dummy, sizeof(int), 1, fp);//�e��ԓ�Փx�f�[�^
	fwrite(&recfp->mapdata.ddifG[1], sizeof(int), 1, fp);//�e��ԓ�Փx�f�[�^
	fwrite(&recfp->nameset.DifFN, 255, 1, fp);//��Փx�o�[��
	fwrite(&recfp->mapeff.Movie, sizeof(item_box), recfp->allnum.movienum, fp);//����f�[�^
	fwrite(&recfp->mapeff.camera, sizeof(rec_camera_data_t), 255, fp);//�J�����f�[�^
	fwrite(&recfp->mapeff.scrool, sizeof(rec_scrool_data_t), 99, fp);//�X�N���[���f�[�^
	fwrite(&recfp->mapeff.v_BPM.data[0], sizeof(view_BPM_box), recfp->allnum.v_BPMnum, fp);//�����ڂ�BPM�f�[�^
	REC_LINE_WRITE(&recfp->mapeff.viewLine.d[0], sizeof(rec_viewline_data_t), 99, fp);//���C���K�C�h�̕\��/��\��
	fwrite(&recfp->outpoint, sizeof(int), 2, fp);//���ʃG���[
	REC_LINE_WRITE(&recfp->mapdata.mpal, sizeof(rec_ddif_pal_t), 1, fp);//��Փx���̓f�[�^

	fclose(fp);

	return 0;
}

/**
 * rrs�t�@�C������ddif�v�Z�ɕK�v�ȏ���ǂݍ���
 * @return 0: OK, -1: error
 */
int RecScoreReadForDdif(rec_score_file_row_t *recfp, const TCHAR *path) {
	FILE *fp;

	_wfopen_s(&fp, path, L"rb");

	if (recfp == NULL || fp == NULL) { return -1; }

	fread(&recfp->allnum, sizeof(playnum_box), 1, fp);//�e�f�[�^�̌�
	fseek(fp, 255, SEEK_CUR);//���y�t�@�C����
	fread(&recfp->mapdata.bpm, sizeof(double), 1, fp);//BPM
	fseek(fp, sizeof(int), SEEK_CUR);//offset
	fseek(fp, 255, SEEK_CUR);//��w�i��
	fseek(fp, 255, SEEK_CUR);//�n�ʉ摜��
	fseek(fp, 255, SEEK_CUR);//�����摜��
	fseek(fp, 255, SEEK_CUR);//�Ȗ�
	fseek(fp, 255, SEEK_CUR);//�Ȗ�(�p��)
	fread(&recfp->mapdata.Lv, sizeof(short int), 1, fp);//���x��
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//�������w�i�؂�ւ��^�C�~���O
	fseek(fp, sizeof(double) * 990, SEEK_CUR);//���[�����x
	fseek(fp, sizeof(int) * 594, SEEK_CUR);//�L�����O���ϊ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Ymovenum[0], SEEK_CUR);//�ヌ�[���c�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Ymovenum[1], SEEK_CUR);//�����[���c�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Ymovenum[2], SEEK_CUR);//�����[���c�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Ymovenum[3], SEEK_CUR);//�n�ʏc�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Ymovenum[4], SEEK_CUR);//�����c�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Xmovenum[0], SEEK_CUR);//�ヌ�[�����ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Xmovenum[1], SEEK_CUR);//�����[�����ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Xmovenum[2], SEEK_CUR);//�����[�����ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(int) * 396, SEEK_CUR);//�m�[�c�Œ�؂�ւ��^�C�~���O
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//�L���������؂�ւ��^�C�~���O
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//�m�[�c�\�����ԕϊ��^�C�~���O
#if SWITCH_NOTE_BOX_2 == 1
	fread(&recfp->mapdata.note, sizeof(note_box_2_t),
		recfp->allnum.notenum[0] + recfp->allnum.notenum[1] + recfp->allnum.notenum[2], fp); /* �m�[�c�f�[�^ */
#else
	fread(&recfp->mapdata.note2.up[0], sizeof(struct note_box), recfp->allnum.notenum[0], fp); /* �ヌ�[���m�[�c�f�[�^ */
	fread(&recfp->mapdata.note2.mid[0], sizeof(struct note_box), recfp->allnum.notenum[1], fp); /* �����[���m�[�c�f�[�^ */
	fread(&recfp->mapdata.note2.low[0], sizeof(struct note_box), recfp->allnum.notenum[2], fp); /* �����[���m�[�c�f�[�^ */
#endif
	fread(&recfp->mapdata.notes, sizeof(short int), 1, fp);//�m�[�c��
	fseek(fp, sizeof(int), SEEK_CUR);//�ȏI������
	{
		int buf[2];
		fread(buf, sizeof(int), 2, fp);
		recfp->mapdata.mdif = buf[0];//�ō���Փx
		recfp->mapdata.ldif = buf[1];//�ŏI��Փx
	}
	fread(&recfp->mapdata.ddif, sizeof(int), 25, fp);//�e��ԓ�Փx�f�[�^
	fread(&recfp->mapdata.ddifG, sizeof(int), 2, fp);//�e��ԓ�Փx�f�[�^
	// fseek(fp, 255, SEEK_CUR);//��Փx�o�[��
	// fseek(fp, sizeof(item_box) * recfp->allnum.movienum, SEEK_CUR);//�A�C�e���f�[�^
	// fseek(fp, sizeof(rec_camera_data_t) * 255, SEEK_CUR);//�J�����f�[�^
	// fseek(fp, sizeof(rec_scrool_data_t) * 99, SEEK_CUR);//�X�N���[���f�[�^
	// fseek(fp, sizeof(view_BPM_box) * recfp->allnum.v_BPMnum, SEEK_CUR);//�X�N���[���f�[�^
	// REC_LINE_SEEK(&recfp->mapeff.viewLine.d[0], sizeof(rec_viewline_data_t), 99, fp);//���C���K�C�h�̕\��/��\��
	// fseek(fp, sizeof(int) * 2, SEEK_CUR);//�G���[�f�[�^

	fclose(fp);

	return 0;
}

/**
 * rrs�t�@�C������Ȗ���ǂݍ���
 * @return 0: OK, -1: error
 */
int RecScoreReadSongName(TCHAR *songName, const TCHAR *path) {
	FILE *fp;

	_wfopen_s(&fp, path, L"rb");

	if (songName == NULL || fp == NULL) { return -1; }

	REC_LINE_SEEK(&recfp->allnum, sizeof(playnum_box), 1, fp);//�e�f�[�^�̌�
	REC_LINE_SEEK(&recfp->nameset.mp3FN, 255, 1, fp);//���y�t�@�C����
	REC_LINE_SEEK(&recfp->mapdata.bpm, sizeof(double), 1, fp);//BPM
	REC_LINE_SEEK(&recfp->time.offset, sizeof(int), 1, fp);//offset
	REC_LINE_SEEK(&recfp->nameset.sky, 255, 1, fp);//��w�i��
	REC_LINE_SEEK(&recfp->nameset.ground, 255, 1, fp);//�n�ʉ摜��
	REC_LINE_SEEK(&recfp->nameset.water, 255, 1, fp);//�����摜��
	REC_LINE_READ(songName, 255, 1, fp);//�Ȗ�

	fclose(fp);

	return 0;
}

/**
 * rrs�t�@�C������ddif��ǂݍ���
 * @return 0: OK, -1: error
 */
int RecScoreReadDdif(rec_ddif_pal_t *ddif, const TCHAR *path) {
	FILE *fp;

	playnum_box allnum;
	if (RecScoreGetAllnum(&allnum, path) != 0) { return -1; }

	_wfopen_s(&fp, path, L"rb");

	if (ddif == NULL || fp == NULL) { return -1; }

	REC_LINE_SEEK(&allnum, sizeof(playnum_box), 1, fp);//�e�f�[�^�̌�
	fseek(fp, 255, SEEK_CUR);//���y�t�@�C����
	REC_LINE_SEEK(&recfp->mapdata.bpm, sizeof(double), 1, fp);//BPM
	fseek(fp, sizeof(int), SEEK_CUR);//offset
	fseek(fp, 255, SEEK_CUR);//��w�i��
	fseek(fp, 255, SEEK_CUR);//�n�ʉ摜��
	fseek(fp, 255, SEEK_CUR);//�����摜��
	fseek(fp, 255, SEEK_CUR);//�Ȗ�
	fseek(fp, 255, SEEK_CUR);//�Ȗ�(�p��)
	REC_LINE_SEEK(&recfp->mapdata.Lv, sizeof(short int), 1, fp);//���x��
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//�������w�i�؂�ւ��^�C�~���O
	fseek(fp, sizeof(double) * 990, SEEK_CUR);//���[�����x
	fseek(fp, sizeof(int) * 594, SEEK_CUR);//�L�����O���ϊ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[0], SEEK_CUR);//�ヌ�[���c�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[1], SEEK_CUR);//�����[���c�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[2], SEEK_CUR);//�����[���c�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[3], SEEK_CUR);//�n�ʏc�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[4], SEEK_CUR);//�����c�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Xmovenum[0], SEEK_CUR);//�ヌ�[�����ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Xmovenum[1], SEEK_CUR);//�����[�����ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Xmovenum[2], SEEK_CUR);//�����[�����ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(int) * 396, SEEK_CUR);//�m�[�c�Œ�؂�ւ��^�C�~���O
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//�L���������؂�ւ��^�C�~���O
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//�m�[�c�\�����ԕϊ��^�C�~���O
#if SWITCH_NOTE_BOX_2 == 1
	REC_LINE_SEEK(&recfp->mapdata.note, sizeof(note_box_2_t), allnum.notenum[0] + allnum.notenum[1] + allnum.notenum[2], fp); /* �m�[�c�f�[�^ */
#else
	REC_LINE_SEEK(&recfp->mapdata.note2.up[0], sizeof(struct note_box), allnum.notenum[0], fp); /* �ヌ�[���m�[�c�f�[�^ */
	REC_LINE_SEEK(&recfp->mapdata.note2.mid[0], sizeof(struct note_box), allnum.notenum[1], fp); /* �����[���m�[�c�f�[�^ */
	REC_LINE_SEEK(&recfp->mapdata.note2.low[0], sizeof(struct note_box), allnum.notenum[2], fp); /* �����[���m�[�c�f�[�^ */
#endif
	REC_LINE_SEEK(&recfp->mapdata.notes, sizeof(short int), 1, fp);//�m�[�c��
	fseek(fp, sizeof(int), SEEK_CUR);//�ȏI������
	REC_LINE_SEEK(buf, sizeof(int), 2, fp);
	REC_LINE_SEEK(&recfp->mapdata.ddif, sizeof(int), 25, fp);//�e��ԓ�Փx�f�[�^
	REC_LINE_SEEK(&recfp->mapdata.ddifG, sizeof(int), 2, fp);//�e��ԓ�Փx�f�[�^
	fseek(fp, 255, SEEK_CUR);//��Փx�o�[��
	fseek(fp, sizeof(item_box) * allnum.movienum, SEEK_CUR);//�A�C�e���f�[�^
	fseek(fp, sizeof(rec_camera_data_t) * 255, SEEK_CUR);//�J�����f�[�^
	fseek(fp, sizeof(rec_scrool_data_t) * 99, SEEK_CUR);//�X�N���[���f�[�^
	fseek(fp, sizeof(view_BPM_box) * allnum.v_BPMnum, SEEK_CUR);//�X�N���[���f�[�^
	fseek(fp, sizeof(int) * 2, SEEK_CUR);//�G���[�f�[�^
	REC_LINE_SEEK(&recfp->mapeff.viewLine.d[0], sizeof(rec_viewline_data_t), 99, fp);//���C���K�C�h�̕\��/��\��
	REC_LINE_READ(ddif, sizeof(rec_ddif_pal_t), 1, fp);//��Փx���̓f�[�^

	fclose(fp);

	return 0;
}

/**
 * rrs�t�@�C������ddif����������
 * @return 0: OK, -1: error
 */
int RecScoreWriteDdif(rec_ddif_pal_t *ddif, const TCHAR *path) {
	FILE *fp;

	playnum_box allnum;
	if (RecScoreGetAllnum(&allnum, path) != 0) { return -1; }

	_wfopen_s(&fp, path, L"ab");

	if (ddif == NULL || fp == NULL) { return -1; }

	REC_LINE_SEEK(&allnum, sizeof(playnum_box), 1, fp);//�e�f�[�^�̌�
	fseek(fp, 255, SEEK_CUR);//���y�t�@�C����
	REC_LINE_SEEK(&recfp->mapdata.bpm, sizeof(double), 1, fp);//BPM
	fseek(fp, sizeof(int), SEEK_CUR);//offset
	fseek(fp, 255, SEEK_CUR);//��w�i��
	fseek(fp, 255, SEEK_CUR);//�n�ʉ摜��
	fseek(fp, 255, SEEK_CUR);//�����摜��
	fseek(fp, 255, SEEK_CUR);//�Ȗ�
	fseek(fp, 255, SEEK_CUR);//�Ȗ�(�p��)
	REC_LINE_SEEK(&recfp->mapdata.Lv, sizeof(short int), 1, fp);//���x��
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//�������w�i�؂�ւ��^�C�~���O
	fseek(fp, sizeof(double) * 990, SEEK_CUR);//���[�����x
	fseek(fp, sizeof(int) * 594, SEEK_CUR);//�L�����O���ϊ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[0], SEEK_CUR);//�ヌ�[���c�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[1], SEEK_CUR);//�����[���c�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[2], SEEK_CUR);//�����[���c�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[3], SEEK_CUR);//�n�ʏc�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[4], SEEK_CUR);//�����c�ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Xmovenum[0], SEEK_CUR);//�ヌ�[�����ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Xmovenum[1], SEEK_CUR);//�����[�����ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(rec_move_data_t) * allnum.Xmovenum[2], SEEK_CUR);//�����[�����ʒu�ړ��^�C�~���O
	fseek(fp, sizeof(int) * 396, SEEK_CUR);//�m�[�c�Œ�؂�ւ��^�C�~���O
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//�L���������؂�ւ��^�C�~���O
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//�m�[�c�\�����ԕϊ��^�C�~���O
#if SWITCH_NOTE_BOX_2 == 1
	REC_LINE_SEEK(&recfp->mapdata.note, sizeof(note_box_2_t), allnum.notenum[0] + allnum.notenum[1] + allnum.notenum[2], fp); /* �m�[�c�f�[�^ */
#else
	REC_LINE_SEEK(&recfp->mapdata.note2.up[0], sizeof(struct note_box), allnum.notenum[0], fp); /* �ヌ�[���m�[�c�f�[�^ */
	REC_LINE_SEEK(&recfp->mapdata.note2.mid[0], sizeof(struct note_box), allnum.notenum[1], fp); /* �����[���m�[�c�f�[�^ */
	REC_LINE_SEEK(&recfp->mapdata.note2.low[0], sizeof(struct note_box), allnum.notenum[2], fp); /* �����[���m�[�c�f�[�^ */
#endif
	REC_LINE_SEEK(&recfp->mapdata.notes, sizeof(short int), 1, fp);//�m�[�c��
	fseek(fp, sizeof(int), SEEK_CUR);//�ȏI������
	REC_LINE_SEEK(buf, sizeof(int), 2, fp);
	REC_LINE_SEEK(&recfp->mapdata.ddif, sizeof(int), 25, fp);//�e��ԓ�Փx�f�[�^
	REC_LINE_SEEK(&recfp->mapdata.ddifG, sizeof(int), 2, fp);//�e��ԓ�Փx�f�[�^
	fseek(fp, 255, SEEK_CUR);//��Փx�o�[��
	fseek(fp, sizeof(item_box) * allnum.movienum, SEEK_CUR);//�A�C�e���f�[�^
	fseek(fp, sizeof(rec_camera_data_t) * 255, SEEK_CUR);//�J�����f�[�^
	fseek(fp, sizeof(rec_scrool_data_t) * 99, SEEK_CUR);//�X�N���[���f�[�^
	fseek(fp, sizeof(view_BPM_box) * allnum.v_BPMnum, SEEK_CUR);//�X�N���[���f�[�^
	REC_LINE_SEEK(&recfp->mapeff.viewLine.d[0], sizeof(rec_viewline_data_t), 99, fp);//���C���K�C�h�̕\��/��\��
	fseek(fp, sizeof(int) * 2, SEEK_CUR);//�G���[�f�[�^
	REC_LINE_WRITE(ddif, sizeof(rec_ddif_pal_t), 1, fp);//��Փx���̓f�[�^

	fclose(fp);

	return 0;
}
