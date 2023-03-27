#include "define.h"
#include "fontcur.h"
#include "playbox.h"

#define CAL_ACC(judge, notes)												\
	(((judge).just * 10000 + (judge).good * 9500 +							\
	(judge).safe * 5500) / ((notes) * 100.0))
#define CAL_DIF_RATE(mdif, Lv)												\
	((Lv) == 0 ? 0 : maxs((mdif), (Lv) * 100 + 90))

char CalScoreRank(int score);
int CalPlayRate(judge_box judge, double DifRate);
char JudgeClearRank(char drop, judge_box judge);

void ViewResult(int dif, wchar_t DifFN[255], wchar_t songN[255],
	struct judge_box* const judge, int Mcombo, short int notes,
	double SumRate[], short int rank, int score, double acc, int gapa[],
	short int Clear, char charNo) {
	/* num */
	short int fontNo;
	switch (rank) {
	case 0:
		fontNo = 6;
		break;
	case 1:
		fontNo = 4;
		break;
	case 2:
		fontNo = 2;
		break;
	case 3:
		fontNo = 3;
		break;
	case 4:
		fontNo = 5;
		break;
	case 5:
		fontNo = 1;
		break;
	default:
		fontNo = 4;
		break;
	}
	int G[10] = { 0,0,0,0,0,0,0,0,0,0 };
	/* image */
	int coleimg;
	switch (Clear) {
	case 0:
		coleimg = LoadGraph(L"picture/DROPED.png");
		break;
	case 1:
		coleimg = LoadGraph(L"picture/CLEARED.png");
		break;
	case 2:
		coleimg = LoadGraph(L"picture/NOMISS.png");
		break;
	case 3:
		coleimg = LoadGraph(L"picture/FULLCOMBO.png");
		break;
	case 4:
		coleimg = LoadGraph(L"picture/PERFECT.png");
		break;
	default:
		coleimg = LoadGraph(L"picture/DROPED.png");
		break;
	}
	int	difberimg;
	switch (dif) {
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
	default:
		difberimg = LoadGraph(L"picture/difeasy.png");
		break;
	}
	int rankimg;
	switch (rank) {
	case 0:
		rankimg = LoadGraph(L"picture/rankEX.png");
		break;
	case 1:
		rankimg = LoadGraph(L"picture/rankS.png");
		break;
	case 2:
		rankimg = LoadGraph(L"picture/rankA.png");
		break;
	case 3:
		rankimg = LoadGraph(L"picture/rankB.png");
		break;
	case 4:
		rankimg = LoadGraph(L"picture/rankC.png");
		break;
	case 5:
	default:
		rankimg = LoadGraph(L"picture/rankD.png");
		break;
	}
	int Rchaimg;
	switch (charNo) {
	case 0:
		Rchaimg = LoadGraph(L"picture/RePicker.png");
		break;
	case 1:
		Rchaimg = LoadGraph(L"picture/ReGator.png");
		break;
	case 2:
		Rchaimg = LoadGraph(L"picture/ReTaylor.png");
		break;
	default:
		Rchaimg = LoadGraph(L"picture/RePicker.png");
		break;
	}
	int resultimg = LoadGraph(L"picture/result.png");
	/* audio */
	int musicmp3 = LoadSoundMem(L"song/Balloon Art.mp3");
	PlaySoundMem(musicmp3, DX_PLAYTYPE_LOOP);
	WaitTimer(10);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, resultimg, TRUE);
		DrawGraph(460, 20, difberimg, TRUE);
		DrawString(100, 13, songN, 0xFFFFFFFF);
		DrawCurFont(judge->just, 140, 52, 30, 4);
		DrawCurFont(judge->good, 140, 93, 30, 2);
		DrawCurFont(judge->safe, 140, 134, 30, 3);
		DrawCurFont(judge->miss, 140, 175, 30, 1);
		DrawCurFont(Mcombo, 155, 215, 30, 4);
		DrawCurFont(notes, 265, 215, 30, 5);
		DrawFormatString(10, 320, 0xFFFFFFFF, L"%.2f", SumRate[1]);
		if (SumRate[1] != SumRate[0]) {
			DrawFormatString(10, 340, 0xFFFFFFFF, L"+%.2f",
				SumRate[1] - SumRate[0]);
		}
		else { DrawString(10, 340, L"not rise", 0xFFFFFFFF); }
		DrawCurFont(score, 310, 75, 55, fontNo);
		DrawCurFont(acc, 430, 150, 30, fontNo, 2);
		if (gapa[1] == 0) gapa[1] = 1;
		DrawCurFont(gapa[0] / gapa[1], 510, 205, 20, 0);
		DrawCurFont((gapa[2] / gapa[1]) - (gapa[0] * gapa[0]) / (gapa[1] * gapa[1]),
			500, 230, 20, 0);
		DrawGraph(140, 260, rankimg, TRUE);
		DrawGraph(5, 420, coleimg, TRUE);
		DrawGraph(336, 252, Rchaimg, TRUE);
		ScreenFlip();
		//�G���^�[�������ꂽ
		if (CheckHitKey(KEY_INPUT_RETURN) == 1) {
			StopSoundMem(musicmp3);
			DeleteSoundMem(musicmp3);
			break;
		}
		else if (GetWindowUserCloseFlag(TRUE)) return;
	}
	InitGraph();
	return;
}

int result(int o, short int Lv, char drop, int difkey,
	wchar_t songN[255], wchar_t DifFN[255], struct judge_box judge, int score,
	int Mcombo, short int notes, int gapa[3], int Dscore) {
	/* char */
	char Clear = JudgeClearRank(drop, judge);
	/* short */
	short int i[3];
	short int rank = CalScoreRank(score);
	/* int */
	int G[20];
	int system[6] = { 0,0,0,2,0,0 };
	int	read[7] = { 0,0,0,0,0,0,0 };
	int	Readdis[7] = { 0,0,0,0,0,0,0 };
	int	ReadRank[7] = { 6,6,6,6,6,6,6 };
	int	ReadClear[7] = { 0,0,0,0,0,0,0 };
	int chap[3] = { 0,0,0 };
	/* double */
	double DifRate = CAL_DIF_RATE(difkey, Lv) / 100; //���ʒ萔
	double rate = (double)CalPlayRate(judge, DifRate) / 100.0;
	double SumRate[2] = { 0,0 };
	double ReadAcc[7] = { 0,0,0,0,0,0,0 };
	double acc = CAL_ACC(judge, notes);
	/* wchar_t */
	wchar_t save[255] = L"score/";
	/* struct */
	play_rate_t prate[RATE_NUM];
	/* FILE */
	FILE *fp;
	// �V�X�e�����[�h
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&system, sizeof(int), 6, fp);
		fclose(fp);
	}
	strcats(save, songN); // save = score/<�Ȗ�>
	strcats(save, L".dat"); // save = score/<�Ȗ�>.dat
	G[0] = _wfopen_s(&fp, save, L"rb"); // �L�^�ǂݍ���
	if (G[0] == 0) {
		fread(&read, sizeof(int), 6, fp);
		fread(&ReadAcc, sizeof(double), 6, fp);
		fread(&Readdis, sizeof(int), 6, fp);
		fread(&ReadRank, sizeof(int), 6, fp);
		fread(&ReadClear, sizeof(int), 6, fp);
		fclose(fp);
	}
	G[9] = 0;
	if (read[o] < score) { read[o] = score; } //�n�C�X�R�A�ۑ�
	if (ReadAcc[o] < acc) { ReadAcc[o] = acc; } //ACC�ۑ�
	if (Readdis[o] < Dscore) { Readdis[o] = Dscore; } //�Œ����s�����ۑ�
	if (ReadRank[o] > rank || ReadRank[o] < 0) { ReadRank[o] = rank; } //�����N�ۑ�
	if (ReadClear[o] < Clear) { ReadClear[o] = Clear; } //�N���A���[�g�ۑ�
	G[0] = _wfopen_s(&fp, save, L"wb");
	fwrite(&read, sizeof(int), 6, fp);
	fwrite(&ReadAcc, sizeof(double), 6, fp);
	fwrite(&Readdis, sizeof(int), 6, fp);
	fwrite(&ReadRank, sizeof(int), 6, fp);
	fwrite(&ReadClear, sizeof(int), 6, fp);
	fclose(fp);
	//�v���C�񐔕ۑ�
	G[0] = _wfopen_s(&fp, L"save/data.dat", L"rb");
	if (G[0] == 0) {
		fread(&read, sizeof(read), 7, fp);
		fclose(fp);
	}
	read[0]++;
	if (drop == 1) read[1]++;
	else read[3]++;
	if (judge.miss == 0) read[4]++;
	if (judge.miss == 0 && judge.safe == 0) read[5]++;
	if (judge.miss == 0 && judge.safe == 0 && judge.good == 0) read[6]++;
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
	//���[�g�ۑ�
	G[0] = _wfopen_s(&fp, RATE_FILE_NAME, L"rb");
	if (fp != NULL) {
		fread(&prate, sizeof(play_rate_t), RATE_NUM, fp);
		fclose(fp);
	}
	for (i[0] = 0; i[0] < RATE_NUM; i[0]++) { //�v���C�O�̃��[�g���v�Z
		SumRate[0] += mins_D(prate[i[0]].num, 0);
	}
	SumRate[0] /= 2;
	SumRate[1] = SumRate[0];
	G[0] = -1;
	//�����ȁA�܂��͖����^��T��
	for (i[0] = 0; i[0] < RATE_NUM; i[0]++) {
		if ((strands(songN, prate[i[0]].name) ||
			(prate[i[0]].name[0] == L'\0')) &&
			prate[i[0]].num == 0) {
			G[0] = i[0];
			break;
		}
	}
	//�Ȃ�������A��ԒႢ���[�g��T��
	if (G[0] == -1) {
		G[0] = 0;
		for (i[0] = 1; i[0] < RATE_NUM; i[0]++) {
			if (prate[G[0]].num > prate[i[0]].num) {
				G[0] = i[0];
			}
		}
	}
	//���[�g������������X�V����
	if (prate[G[0]].num < rate) {
		prate[G[0]].num = rate;
		SumRate[1] = 0;
		strcopy(songN, prate[G[0]].name, 1);
		for (i[0] = 0; i[0] < RATE_NUM; i[0]++) { //�ω���̃��[�g���v�Z
			SumRate[1] += mins_D(prate[i[0]].num, 0);
		}
	SumRate[1] /= 2;
		G[0] = _wfopen_s(&fp, RATE_FILE_NAME, L"wb");
		fwrite(&prate, sizeof(play_rate_t), RATE_NUM, fp);
		fclose(fp);
	}
	//���U���g���
	ViewResult(o, DifFN, songN, &judge, Mcombo, notes, SumRate, rank, score,
		acc, gapa, Clear - 1, (char)system[0]);
	return 2;
}

int CalPlayRate(judge_box judge, double DifRate) {
	double rate = 0;
	// level0�Ȃ�0�Œ�
	if (DifRate == 0) rate = 0;
	// "���ʒ萔" - "miss��" x "���ʒ萔" x 0.03 (����=0)
	else if (judge.miss > 0) {
		rate = DifRate - judge.miss * DifRate * 0.03;
		rate = mins_D(rate, 0);
	}
	// NO MISS,"���ʒ萔" + 1 - "safe��" x 0.05 (����="���ʒ萔")
	else if (judge.miss == 0 && judge.safe > 0) {
		rate = DifRate + 1 - judge.safe * 0.05;
		rate = mins_D(rate, DifRate);
	}
	// FULL COMBO,"���ʒ萔" + 2 - "good��" x 0.01 (����="���ʒ萔" + 1)
	else if (judge.miss == 0 && judge.safe == 0 && judge.good > 0) {
		rate = DifRate + 2 - judge.good * 0.01;
		rate = mins_D(rate, DifRate + 1);
	}
	// PERFECT, "���ʒ萔" + 2
	else if (judge.miss == 0 && judge.safe == 0 && judge.good == 0) {
		rate = DifRate + 2;
	}
	return (int)(rate * 100);
}

char CalScoreRank(int score) {
	if (score >= 98000) { return 0; }
	if (score >= 95000) { return 1; }
	if (score >= 90000) { return 2; }
	if (score >= 85000) { return 3; }
	if (score >= 80000) { return 4; }
	return 5;
}

char JudgeClearRank(char drop, judge_box judge) {
	if (drop == 1) { return 1; }
	if (0 < judge.miss) { return 2; }
	if (0 < judge.safe) { return 3; }
	if (0 < judge.good) { return 4; }
	return 5;
}
