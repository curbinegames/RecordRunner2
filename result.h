#include "define.h"
#include "fontcur.h"
#include "playbox.h"

int CalPlayRate(judge_box judge, double DifRate);

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
		//エンターが押された
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

int result(int p, int n, int o, short int Lv, short int drop, int difkey,
	wchar_t songN[255], wchar_t DifFN[255], struct judge_box judge, int score,
	int Mcombo, short int notes, int gapa[3], int Dscore) {
	short int i[3];
	short int rank;
	short int Clear;
	int G[20], songT;
	int system[6] = { 0,0,0,2,0,0 };
	double GD[5];
	double rate = 0;
	double DifRate; //譜面定数
	double SumRate[2] = { 0,0 };
	wchar_t dataE[255] = L"record/";
	wchar_t fileN[255];
	wchar_t GT1[255];
	wchar_t GT2[255];
	wchar_t GT26[6][7] = { L"/0.rrs" ,L"/1.rrs" ,L"/2.rrs" ,L"/3.rrs" ,L"/4.rrs" ,L"/5.rrs" };
	wchar_t ST1[] = L"record/";
	wchar_t ST2[] = L"/list.txt";
	unsigned int Cr, Crb;
	Cr = GetColor(255, 255, 255);
	Crb = GetColor(0, 0, 0);
	FILE *fp;
	//システムロード
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
	for (i[0] = 0; i[0] <= n; i[0]++) FileRead_gets(GT1, 256, songT);//GT1に曲のフォルダ名を入れる
	FileRead_close(songT);
	strcopy(GT1, fileN, 1);//fileNに曲のフォルダ名を入れる
	strcats(dataE, GT1);//dataEに"record/曲のフォルダ名"を入れる
	strcopy(dataE, GT1, 1);//GT1に"record/'曲のフォルダ名'"を入れる
	strcats(GT1, GT26[o]);//GT1に"record/'曲のフォルダ名'/.'難易度番号'rrs"を入れる
	//ゲーム開始前の下準備
	GD[0] = difkey / 100.0 - Lv;//mdifと難易度表記の差
	if (Lv == 0) { DifRate = 0; }
	else if (2 <= GD[0]) { DifRate = Lv + 0.9; }
	else if (0 <= GD[0] && GD[0] < 2) { DifRate = Lv + 0.45 * GD[0]; }
	else { DifRate = difkey / 100.0; }

	//ゲーム処理だった場所

	int	read[7] = { 0,0,0,0,0,0,0 };
	double ReadAcc[7] = { 0,0,0,0,0,0,0 };
	int	Readdis[7] = { 0,0,0,0,0,0,0 };
	int	ReadRank[7] = { 6,6,6,6,6,6,6 };
	int	ReadClear[7] = { 0,0,0,0,0,0,0 };
	int chap[3] = { 0,0,0 };
	double acc = (judge.just * 10000 + judge.good * 9500 + judge.safe * 5500) / (notes*100.0);
	wchar_t save[255] = L"score/";
	wchar_t save2[255] = L".dat";
	play_rate_t prate[RATE_NUM];
	//ランク判定
	if (score >= 98000) rank = 0;
	else if (score >= 95000) rank = 1;
	else if (score >= 90000) rank = 2;
	else if (score >= 85000) rank = 3;
	else if (score >= 80000) rank = 4;
	else rank = 5;
	//クリアレート判定
	if (drop == 1) { Clear = 1; }
	else if (drop == 0 && judge.miss > 0) { Clear = 2; }
	else if (drop == 0 && judge.miss == 0 && judge.safe > 0) { Clear = 3; }
	else if (drop == 0 && judge.miss == 0 && judge.safe == 0 && judge.good > 0) { Clear = 4; }
	else Clear = 5;
	strcats(save, fileN);
	strcats(save, save2);
	G[0] = _wfopen_s(&fp, save, L"rb"); //記録読み込み
	if (G[0] == 0) {
		fread(&read, sizeof(int), 6, fp);
		fread(&ReadAcc, sizeof(double), 6, fp);
		fread(&Readdis, sizeof(int), 6, fp);
		fread(&ReadRank, sizeof(int), 6, fp);
		fread(&ReadClear, sizeof(int), 6, fp);
		fclose(fp);
	}
	G[9] = 0;
	if (read[o] < score) { read[o] = score; } //ハイスコア保存
	if (ReadAcc[o] < acc) { ReadAcc[o] = acc; } //ACC保存
	if (Readdis[o] < Dscore) { Readdis[o] = Dscore; } //最長走行距離保存
	if (ReadRank[o] > rank || ReadRank[o] < 0) { ReadRank[o] = rank; } //ランク保存
	if (ReadClear[o] < Clear) { ReadClear[o] = Clear; } //クリアレート保存
	G[0] = _wfopen_s(&fp, save, L"wb");
	fwrite(&read, sizeof(int), 6, fp);
	fwrite(&ReadAcc, sizeof(double), 6, fp);
	fwrite(&Readdis, sizeof(int), 6, fp);
	fwrite(&ReadRank, sizeof(int), 6, fp);
	fwrite(&ReadClear, sizeof(int), 6, fp);
	fclose(fp);
	//プレイ回数保存
	G[0] = _wfopen_s(&fp, L"save/data.dat", L"rb");
	if (G[0] == 0) {
		fread(&read, sizeof(read), 7, fp);
		fclose(fp);
	}
	read[0]++;
	if (drop == 1) read[1]++;
	else read[3]++;
	if (judge.miss == 0)read[4]++;
	if (judge.miss == 0 && judge.safe == 0)read[5]++;
	if (judge.miss == 0 && judge.safe == 0 && judge.good == 0)read[6]++;
	G[0] = _wfopen_s(&fp, L"save/data.dat", L"wb");
	fwrite(&read, sizeof(int), 7, fp);
	fclose(fp);
	//キャラプレイ数保存
	G[0] = _wfopen_s(&fp, L"save/chap.dat", L"rb");
	if (G[0] == 0) {
		fread(&chap, sizeof(chap), 3, fp);
		fclose(fp);
	}
	chap[system[0]]++;
	G[0] = _wfopen_s(&fp, L"save/chap.dat", L"wb");
	fwrite(&chap, sizeof(int), 3, fp);
	fclose(fp);
	rate = (int)CalPlayRate(judge, DifRate);
	//レート保存
	G[0] = _wfopen_s(&fp, RATE_FILE_NAME, L"rb");
	if (fp != NULL) {
		fread(&prate, sizeof(play_rate_t), RATE_NUM, fp);
		fclose(fp);
	}
	for (i[0] = 0; i[0] < RATE_NUM; i[0]++) { //プレイ前のレートを計算
		SumRate[0] += mins_D(prate[i[0]].num, 0);
	}
	SumRate[0] /= 2;
	SumRate[1] = SumRate[0];
	G[0] = -1;
	//同じ曲、または未収録を探す
	for (i[0] = 0; i[0] < RATE_NUM; i[0]++) {
		if ((strands(fileN, prate[i[0]].name) ||
			(prate[i[0]].name[0] == L'\0')) &&
			prate[i[0]].num == 0) {
			G[0] = i[0];
			break;
		}
	}
	//なかったら、一番低いレートを探す
	if (G[0] == -1) {
		G[0] = 0;
		for (i[0] = 1; i[0] < RATE_NUM; i[0]++) {
			if (prate[G[0]].num > prate[i[0]].num) {
				G[0] = i[0];
			}
		}
	}
	//レートが高かったら更新する
	if (prate[G[0]].num < rate) {
		prate[G[0]].num = rate;
		SumRate[1] = 0;
		strcopy(fileN, prate[G[0]].name, 1);
		for (i[0] = 0; i[0] < RATE_NUM; i[0]++) { //変化後のレートを計算
		SumRate[1] += mins_D(prate[i[0]].num, 0);
	}
	SumRate[1] /= 2;
		G[0] = _wfopen_s(&fp, RATE_FILE_NAME, L"wb");
		fwrite(&prate, sizeof(play_rate_t), RATE_NUM, fp);
		fclose(fp);
	}
	//リザルト画面
	ViewResult(o, DifFN, songN, &judge, Mcombo, notes, SumRate, rank, score,
		acc, gapa, Clear - 1, (char)system[0]);
	return 2;
}

int CalPlayRate(judge_box judge, double DifRate) {
	double rate = 0;
	// level0なら0固定
	if (DifRate == 0) rate = 0;
	// "譜面定数" - "miss数" x "譜面定数" x 0.03 (下限=0)
	else if (judge.miss > 0) {
		rate = DifRate - judge.miss * DifRate * 0.03;
		rate = mins_D(rate, 0);
	}
	// NO MISS,"譜面定数" + 1 - "safe数" x 0.05 (下限="譜面定数")
	else if (judge.miss == 0 && judge.safe > 0) {
		rate = DifRate + 1 - judge.safe * 0.05;
		rate = mins_D(rate, DifRate);
	}
	// FULL COMBO,"譜面定数" + 2 - "good数" x 0.01 (下限="譜面定数" + 1)
	else if (judge.miss == 0 && judge.safe == 0 && judge.good > 0) {
		rate = DifRate + 2 - judge.good * 0.01;
		rate = mins_D(rate, DifRate + 1);
	}
	// PERFECT, "譜面定数" + 2
	else if (judge.miss == 0 && judge.safe == 0 && judge.good == 0) {
		rate = DifRate + 2;
	}
	return (int)(rate * 100);
}
