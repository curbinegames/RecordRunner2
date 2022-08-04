#include "fontcur.h"
#include "playbox.h"

int result(int p, int n, int o, short int Lv, short int drop, int difkey, wchar_t songN[255], wchar_t DifFN[255], struct judge_box judge, int score, int Mcombo, short int notes, int gapa[3], int Dscore) {
	short int i[3];
	short int rank;
	short int Clear;
	int G[20], songT;
	int system[6] = { 0,0,0,2,0,0 };
	double GD[5];
	double rate;
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
	//グラフィックと効果音の準備
	int resultimg, difberimg, rankimg[6], coleimg[5], Rchaimg, musicmp3;
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
	resultimg = LoadGraph(L"picture/result.png");
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
	switch (system[0]) {
	case 0:
		Rchaimg = LoadGraph(L"picture/RePicker.png");
		break;
	case 1:
		Rchaimg = LoadGraph(L"picture/ReGator.png");
		break;
	case 2:
		Rchaimg = LoadGraph(L"picture/ReTaylor.png");
		break;
	}
	musicmp3 = LoadSoundMem(L"song/Balloon Art.mp3");
	//ゲーム開始前の下準備
	GD[0] = difkey / 100.0 - Lv;//mdifと難易度表記の差
	if (Lv == 0) { DifRate = 0; }
	else if (2 <= GD[0]) { DifRate = Lv + 0.9; }
	else if (0 <= GD[0] && GD[0] < 2) { DifRate = Lv + 0.45 * GD[0]; }
	else { DifRate = difkey / 100.0; }
	PlaySoundMem(musicmp3, DX_PLAYTYPE_LOOP);
	WaitTimer(10);

	//ゲーム処理だった場所

	int	read[7] = { 0,0,0,0,0,0,0 };
	double ReadAcc[7] = { 0,0,0,0,0,0,0 };
	int	Readdis[7] = { 0,0,0,0,0,0,0 };
	int	ReadRank[7] = { 6,6,6,6,6,6,6 };
	int	ReadClear[7] = { 0,0,0,0,0,0,0 };
	int chap[3] = { 0,0,0 };
	double readR[10] = { 0,0,0,0,0,0,0,0,0,0 };
	double acc = (judge.just * 10000 + judge.good * 9500 + judge.safe * 5500) / (notes*100.0);
	wchar_t savec[10][255];
	wchar_t save[255] = L"score/";
	wchar_t save2[255] = L".dat";
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
	//レート計算(level0なら0固定)"譜面定数" - "miss数" x "譜面定数" x 0.03(下限=0)
	if (DifRate == 0) rate = 0;
	else if (judge.miss > 0) {
		rate = DifRate - judge.miss * DifRate*0.03;
		rate = mins_D(rate, 0);
	}
	//NO MISS,"譜面定数" + 1 - "safe数" x 0.05(下限="譜面定数")
	else if (judge.miss == 0 && judge.safe > 0) {
		rate = DifRate + 1 - judge.safe * 0.05;
		rate = mins_D(rate, DifRate);
	}
	//FULL COMBO,"譜面定数" + 2 - "good数" x 0.01(下限="譜面定数" + 1)
	else if (judge.miss == 0 && judge.safe == 0 && judge.good > 0) {
		rate = DifRate + 2 - judge.good * 0.01;
		rate = mins_D(rate, DifRate + 1);
	}
	//PERFECT, "譜面定数" + 2
	else if (judge.miss == 0 && judge.safe == 0 && judge.good == 0) rate = DifRate + 2;
	//レート保存
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
	for (i[0] = 0; i[0] < 10; i[0]++) { SumRate[0] += mins_D(readR[i[0]], 0); } //プレイ前のレートを計算
	SumRate[1] = SumRate[0];
	G[0] = -1;
	//同じ曲、または未収録を探す
	for (i[0] = 0; i[0] < 10; i[0]++)if (strands(fileN, savec[i[0]]) || savec[i[0]] == '\0') {
		G[0] = i[0];
		break;
	}
	//なかったら、一番低いレートを探す
	if (G[0] == -1) {
		G[0] = 0;
		for (i[0] = 1; i[0] < 10; i[0]++)if (readR[G[0]] > readR[i[0]]) G[0] = i[0];
	}
	//レートが高かったら更新する
	if (readR[G[0]] < rate) {
		readR[G[0]] = rate;
		SumRate[1] = 0;
		strcopy(fileN, savec[G[0]], 1);
		for (i[0] = 0; i[0] < 10; i[0]++) { SumRate[1] += mins_D(readR[i[0]], 0); } //変化後のレートを計算
		G[0] = _wfopen_s(&fp, L"save/rateS.dat", L"wb");
		fwrite(&savec, 255, 10, fp);
		fclose(fp);
		G[0] = _wfopen_s(&fp, L"save/rateN.dat", L"wb");
		fwrite(&readR, sizeof(double), 10, fp);
		fclose(fp);
	}
	//リザルト画面
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, resultimg, TRUE);
		DrawGraph(460, 20, difberimg, TRUE);
		DrawString(100, 13, songN, Cr);
		DrawCurFont(judge.just, 140, 52, 30, 4);
		DrawCurFont(judge.good, 140, 93, 30, 2);
		DrawCurFont(judge.safe, 140, 134, 30, 3);
		DrawCurFont(judge.miss, 140, 175, 30, 1);
		DrawCurFont(Mcombo, 155, 215, 30, 4);
		DrawCurFont(notes, 265, 215, 30, 5);
		DrawFormatString(10, 320, Cr, L"%.2f", SumRate[1]);
		if (SumRate[1] != SumRate[0]) { DrawFormatString(10, 340, Cr, L"+%.2f", SumRate[1] - SumRate[0]); }
		else { DrawString(10, 340, L"not rise", Cr); }
		switch (rank) {
		case 0:
			G[0] = 6;
			break;
		case 1:
			G[0] = 4;
			break;
		case 2:
			G[0] = 2;
			break;
		case 3:
			G[0] = 3;
			break;
		case 4:
			G[0] = 5;
			break;
		case 5:
			G[0] = 1;
			break;
		}
		DrawCurFont(score, 310, 75, 55, G[0]);
		DrawCurFont(acc, 430, 150, 30, G[0], 2);
		if (gapa[1] == 0) gapa[1] = 1;
		DrawCurFont(gapa[0] / gapa[1], 510, 205, 20, 0);
		DrawCurFont(gapa[2] / gapa[1] - gapa[0] * gapa[0] / gapa[1] / gapa[1], 500, 230, 20, 0);
		DrawGraph(140, 260, rankimg[rank], TRUE);
		DrawGraph(5, 420, coleimg[Clear - 1], TRUE);
		DrawGraph(336, 252, Rchaimg, TRUE);
		ScreenFlip();
		//エンターが押された
		if (CheckHitKey(KEY_INPUT_RETURN) == 1) {
			StopSoundMem(musicmp3);
			DeleteSoundMem(musicmp3);
			break;
		}
		else if (GetWindowUserCloseFlag(TRUE)) return 5;
	}
	InitGraph();
	return 2;
}
