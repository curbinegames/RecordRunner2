#include "DxLib.h"
#include "sancur.h"
#include "strcats.h"
#include "strmods.h"
#include "strsans.h"
#include "strands.h"
#include "strcopy.h"
#include "strcur.h"
#include "abss.h"
#include "max.h"
#include "min.h"
#include "notzero.h"
#include "pals.h"
#include "playing.h"
/* next = 0 = タイトル
	= 1 = メニュー
	= 2 = 曲選択画面
	= 3 = プレイ画面
	= 99 = 終了処理*/
int title(void);
int menu(void);
int musicserect(int*);
int play2(int, int);
int collection(void);
int C_item(void);
int C_story(void);
int story(int, int);
int option(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	ChangeWindowMode(TRUE);
	SetAlwaysRunFlag(TRUE);
	SetWindowUserCloseEnableFlag(FALSE);
	SetMainWindowText(L"Record Runner");
	SetWindowSizeChangeEnableFlag(TRUE);
	if (DxLib_Init() == -1)return -1;
	SetDrawScreen(DX_SCREEN_BACK);
	int next = 0, now = -1, bgm[2], mnom[7] = { 0,1,0,1,1,0,0 };
	int G[2];
	unsigned int Cr;
	Cr = GetColor(255, 255, 255);
	bgm[0] = LoadSoundMem(L"song/no.mp3");
	bgm[1] = LoadSoundMem(L"song/Let’s_be_happy.mp3");
	//ゲーム終了(nextが5のとき)までの間、処理を繰り返す
	while (next != 5) {
		InitGraph();
		if (mnom[next] != now) {
			StopSoundMem(bgm[now]);
			InitSoundMem;
			bgm[0] = LoadSoundMem(L"song/no.mp3");
			bgm[1] = LoadSoundMem(L"song/Let’s_be_happy.mp3");
			PlaySoundMem(bgm[mnom[next]], DX_PLAYTYPE_LOOP);
		}
		//表示する画面を選択する
		now = mnom[next];
		switch (next) {
		case(0):
			next = title();
			break;
		case(1):
			next = menu();
			break;
		case(2):
			next = musicserect(&G[0]);
			break;
		case(3):
			next = collection();
			break;
		case(4):
			next = option();
			break;
		case(6):
			next = play2(G[0], G[1]);
			break;
		default:
			ClearDrawScreen();
			DrawString(200, 200, L"error:001\n予期されない数値", Cr);
			ScreenFlip();
			next = 5;
			WaitTimer(100);
			WaitKey();
			break;
		}
	}
	DxLib_End();
	return 0;
}

int title(void) {
	int next = 1;
	//タイトル画面を表示する
	LoadGraphScreen(0, 0, L"picture/title.png", TRUE);
	LoadGraphScreen(0, 0, L"picture/pushkey.png", TRUE);
	ScreenFlip();
	while (1) {
		if (CheckHitKeyAll()) {
			WaitTimer(500);
			break;
		}
		if (GetWindowUserCloseFlag(TRUE)) {
			next = 5;
			break;
		}
	}
	ClearDrawScreen();
	return next;
}

int menu(void) {
	/*
	next = 次のモード
	command = 選択中のモード
	*/
	int next = 1;
	int command = 1;
	int LR = 1;
	int starttime = -250;
	int movetime = 250;
	int menu[4], menuG[4], menuW[4], help, goust, goust2, i, select;
	int key = 1;
	int	lan[5] = { 0,0,0,2,0 };//使うのは[4,言語]だけ
	FILE *fp;
	goust = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (goust == 0) {
		fread(&lan, sizeof(lan), 5, fp);
		fclose(fp);
	}
	unsigned int Cr;
	Cr = GetColor(255, 255, 255);
	menu[0] = LoadGraph(L"picture/FREE PLAY.png");
	menu[1] = LoadGraph(L"picture/COLLECTION.png");
	menu[2] = LoadGraph(L"picture/OPTION.png");
	menu[3] = LoadGraph(L"picture/QUIT.png");
	menuG[0] = LoadGraph(L"picture/FREE PLAY G.png");
	menuG[1] = LoadGraph(L"picture/COLLECT G.png");
	menuG[2] = LoadGraph(L"picture/OPTION G.png");
	menuG[3] = LoadGraph(L"picture/QUIT G.png");
	menuW[0] = LoadGraph(L"picture/backskynoamal.png");
	menuW[1] = LoadGraph(L"picture/COLLECT W.png");
	menuW[2] = LoadGraph(L"picture/OPTION W.png");
	menuW[3] = LoadGraph(L"picture/backstar.png");
	help = LoadGraph(L"picture/help.png");
	select = LoadSoundMem(L"sound/arrow.ogg");
	while (next == 1) {
		ClearDrawScreen();
		//時間計算
		movetime = GetNowCount() - starttime;
		movetime = maxs(movetime, 250);
		//壁表示
		goust = command - LR - 1;
		while (goust < 0) goust += 4;
		while (goust > 3) goust -= 4;
		goust2 = (movetime - 250)*(movetime - 250);
		DrawGraph(0, 0, menuW[goust], TRUE);
		goust = command - 1;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 51 * movetime / 50);
		DrawGraph(0, 0, menuW[goust], TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		//地面表示
		goust = command - LR - 1;
		while (goust < 0) goust += 4;
		while (goust > 3) goust -= 4;
		goust2 = (movetime - 250)*(movetime - 250);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, -51 * movetime / 50 + 255);
		DrawGraph(0, (-12 * goust2) / 3125 + 240, menuG[goust], TRUE);
		goust = command - 1;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 51 * movetime / 50);
		DrawGraph(0, (12 * goust2) / 3125, menuG[goust], TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		//今のコマンドを表示する
		goust = command - 3;
		while (goust < 0) goust += 4;
		for (i = 0; i < 5; i++) {
			goust2 = movetime - 250 * i * LR + 500 * LR - 250;
			DrawGraph((21 * LR * movetime * movetime - 10500 * LR * movetime + 1312500 * LR + 1312500 * i - 2125000) / 3125, (-1 * goust2 * goust2 + 250000) / 2500, menu[goust], TRUE);
			goust++;
			if (goust > 3) goust -= 4;
		}
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"左右キー:選択   Enterキー:決定", Cr);
		else if (lan[4] == 1)DrawString(5, 460, L"←→key:choose   Enter key:enter", Cr);
		ScreenFlip();
		//ここからキー判定
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//左が押された
			if (key == 0) {
				command -= 1;
				LR = -1;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				starttime = GetNowCount();
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//右が押された
			if (key == 0) {
				command += 1;
				LR = 1;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				starttime = GetNowCount();
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN)) {
			//エンターが押された
			if (key == 0) next = command + 1;
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			//閉じるボタンが押された
			next = 5;
			break;
		}
		//特定のキーが押されていない
		else key = 0;
		//コマンドの端を過ぎたとき、もう片方の端に移動する
		if (command <= 0) command = 4;
		else if (command >= 5) command = 1;
		WaitTimer(5);
	}
	return next;
}

int musicserect(int *p1) {
	//現在64曲しか入らないので、必要に応じて増やしてね。
	unsigned int Cr[7];
	Cr[0] = Cr[1] = Cr[2] = Cr[4] = Cr[5] = Cr[6] = GetColor(255, 255, 255);
	Cr[3] = GetColor(0, 0, 0);
	int songT, musicT, picsong, e, i, n = 0, UD = 1, LR = 1, key = 1, next = 99, startC = -250, XstartC = -250, previewC = 0, moveC = 250, XmoveC = 250, command[2] = { 0,1 }, rimit[64], level[64][6], Hscore[64][6];
	int G[2];
	double diskr = 0, rate[10], preview[64][6][2];
	wchar_t songname[64][6][256], artist[64][6][256], songM[64][6][256], jacketP[64][6][256], mapT[255], GT2[255], GT4[255], GT11[255], GT18[255];
	wchar_t GT[] = { L"record/" };
	wchar_t GT3[6][8] = { L"/0.txt" ,L"/1.txt" ,L"/2.txt" ,L"/3.txt" ,L"/4.txt" ,L"/5.txt" };
	wchar_t GT5[] = { L"#TITLE:" };
	wchar_t GT5E[] = { L"#E.TITLE:" };
	wchar_t GT6[] = { L"#ARTIST:" };
	wchar_t GT6E[] = { L"#E.ARTIST:" };
	wchar_t GT7[] = { L"#MUSIC:" };
	wchar_t GT8[] = { L"#LEVEL:" };
	wchar_t GT9[] = { L"#PREVIEW:" };
	wchar_t GT10[] = { L"#JACKET:" };
	wchar_t GT12[] = { L"score/" };
	wchar_t GT13[] = { L".dat" };
	wchar_t GT14[] = { L"NULL" };
	wchar_t GT15[] = { L"song/" };
	wchar_t GT16[] = { L"picture/NULL jucket.png" };
	wchar_t GT17[] = { L"picture/" };
	wchar_t GT19[] = { L"/" };
	wchar_t GT20[] = { L"#MAP:" };
	wchar_t GT21[255] = L"record/";
	wchar_t playingsong[255] = { L"NULL" };
	wchar_t viewingjacket[255] = { L"NULL" };
	int bar[7], difbar[6], difC[5][2], detail, back, disk, help, select, jacketpic, previewM;
	bar[0] = bar[1] = bar[2] = bar[4] = bar[5] = bar[6] = LoadGraph(L"picture/songbarB.png");
	bar[3] = LoadGraph(L"picture/songbarY.png");
	difbar[0] = LoadGraph(L"picture/difauto.png");
	difbar[1] = LoadGraph(L"picture/difeasy.png");
	difbar[2] = LoadGraph(L"picture/difnormal.png");
	difbar[3] = LoadGraph(L"picture/difhard.png");
	difbar[4] = difbar[5] = LoadGraph(L"picture/difhope.png");
	difC[0][0] = LoadGraph(L"picture/dif0S.png");
	difC[0][1] = LoadGraph(L"picture/dif0B.png");
	difC[1][0] = LoadGraph(L"picture/dif1S.png");
	difC[1][1] = LoadGraph(L"picture/dif1B.png");
	difC[2][0] = LoadGraph(L"picture/dif2S.png");
	difC[2][1] = LoadGraph(L"picture/dif2B.png");
	difC[3][0] = LoadGraph(L"picture/dif3S.png");
	difC[3][1] = LoadGraph(L"picture/dif3B.png");
	difC[4][0] = LoadGraph(L"picture/dif4S.png");
	difC[4][1] = LoadGraph(L"picture/dif4B.png");
	detail = LoadGraph(L"picture/detail.png");
	back = LoadGraph(L"picture/MSback.png");
	disk = LoadGraph(L"picture/disk.png");
	help = LoadGraph(L"picture/help.png");
	jacketpic = LoadGraph(L"picture/NULL jucket.png");
	select = LoadSoundMem(L"sound/arrow.ogg");
	previewM = LoadSoundMem(L"null.mp3");
	PlaySoundMem(previewM, DX_PLAYTYPE_BACK);
	int	lan[5] = { 0,0,0,2,0 };//使うのは[4,言語]だけ
	FILE *fp;
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&lan, sizeof(lan), 5, fp);
		fclose(fp);
	}
	songT = FileRead_open(L"song.txt");
	while (FileRead_eof(songT) == 0) {
		FileRead_gets(GT2, 256, songT);
		rimit[n] = 3;
		for (i = 0; i < 6; i++) {
			strcopy(GT, mapT, 1);
			strcats(mapT, GT2);
			strcopy(mapT, GT18, 1);
			strcats(mapT, GT3[i]);
			musicT = FileRead_open(mapT);
			strcopy(GT14, songname[n][i], 1);
			strcopy(GT14, artist[n][i], 1);
			strcopy(GT14, songM[n][i], 1);
			strcopy(GT16, jacketP[n][i], 1);
			level[n][i] = -1;
			preview[n][i][0] = 441000;
			preview[n][i][1] = 2646000;
			while (FileRead_eof(musicT) == 0) {
				FileRead_gets(GT4, 256, musicT);
				//曲名を読み込む
				if (strands(GT4, GT5) && (lan[4] == 0 || strands(songname[n][i], GT14))) {
					strmods(GT4, 7);
					strcopy(GT4, songname[n][i], 1);
				}
				if (strands(GT4, GT5E) && lan[4] == 1) {
					strmods(GT4, 9);
					strcopy(GT4, songname[n][i], 1);
				}
				//作曲者を読み込む
				if (strands(GT4, GT6) && (lan[4] == 0 || strands(artist[n][i], GT14))) {
					strmods(GT4, 8);
					strcopy(GT4, artist[n][i], 1);
				}
				if (strands(GT4, GT6E) && lan[4] == 1) {
					strmods(GT4, 10);
					strcopy(GT4, artist[n][i], 1);
				}
				//曲ファイル名を読み込む
				if (strands(GT4, GT7)) {
					strcopy(GT15, songM[n][i], 1);
					strmods(GT4, 7);
					strcats(songM[n][i], GT4);
				}
				//難易度を読み込む
				if (strands(GT4, GT8)) {
					strmods(GT4, 7);
					level[n][i] = strsans(GT4);
				}
				//プレビュー時間を読み込む
				if (strands(GT4, GT9)) {
					strmods(GT4, 9);
					preview[n][i][0] = strsans(GT4) * 44100.0 / 1000;
					while (GT4[0] >= '0'&&GT4[0] <= '9') strmods(GT4, 1);
					strmods(GT4, 1);
					preview[n][i][1] = strsans(GT4) * 44100.0 / 1000;
				}
				//ジャケット写真を読み込む
				if (strands(GT4, GT10)) {
					strcopy(GT18, jacketP[n][i], 1);
					strcats(jacketP[n][i], GT19);
					strmods(GT4, 8);
					strcats(jacketP[n][i], GT4);
				}
				//マップに入ったら抜ける
				if (strands(GT4, GT20)) break;
			}
		}
		//ハイスコアを読み込む
		strcopy(GT12, GT11, 1);
		strcats(GT11, GT2);
		strcats(GT11, GT13);
		e = _wfopen_s(&fp, GT11, L"rb");
		if (e == 0) {
			fread(&Hscore[n], sizeof(Hscore[n]), 6, fp);
			fclose(fp);
		}
		for (i = 0; i < 6; i++) if (Hscore[n][i] < 0) Hscore[n][i] = 0;
		if (Hscore[n][5] > 0) rimit[n] = 4;
		FileRead_close(musicT);
		n++;
	}
	n--;
	//ここまで、曲情報の読み込み
	e = _wfopen_s(&fp, L"save/rateN.dat", L"rb");
	if (e == 0) {
		fread(&rate, sizeof(rate), 10, fp);
		fclose(fp);
	}
	for (i = 0; i < 10; i++) if (rate[i] < 0) rate[i] = 0;
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, back, TRUE);
		//時間操作
		moveC = -1 * (GetNowCount() - startC) + 250;
		XmoveC = -1 * (GetNowCount() - XstartC) + 250;
		if (moveC < 0) moveC = 0;
		if (XmoveC < 0) XmoveC = 0;
		//ジャケットを表示する
		if (strands(viewingjacket, jacketP[command[0]][command[1]]) == 0) {
			DeleteGraph(jacketpic);
			strcopy(jacketP[command[0]][command[1]], viewingjacket, 1);
			jacketpic = LoadGraph(viewingjacket);
		}
		DrawGraph(305, 75, jacketpic, TRUE);
		//曲名その他もろもろを表示する
		picsong = command[0] - 3;
		while (picsong < 0) picsong += n + 1;
		for (i = 0; i < 7; i++) {
			DrawGraph((UD*moveC*moveC + 62500 * i - 375000) / 3125, (6 * UD*moveC*moveC + 375000 * i - 531250) / 3125, bar[i], TRUE);
			DrawString((UD*moveC*moveC + 62500 * i - 93750) / 3125, (6 * UD*moveC*moveC + 375000 * i - 468750) / 3125, songname[picsong][command[1]], Cr[i]);
			DrawString((UD*moveC*moveC + 62500 * i - 93750) / 3125, (6 * UD*moveC*moveC + 375000 * i - 406250) / 3125, artist[picsong][command[1]], Cr[i]);
			DrawFormatString((UD*moveC*moveC + 62500 * i + 62500) / 3125, (6 * UD*moveC*moveC + 375000 * i - 312500) / 3125, Cr[i], L"HIGHSCORE:%d", Hscore[picsong][command[1]]);
			picsong++;
			if (picsong > n) picsong -= n + 1;
		}
		//難易度マーカーを表示する
		G[1] = 0;
		for (i = 0; i <= 4; i++) {
			G[0] = 0;
			if (command[1] == i) G[0] = 1;
			if (command[1] < i) G[1] = 1;
			if (strands(songM[command[0]][i], GT14) == 0 && i <= rimit[command[0]]) DrawGraph(550 + 11 * G[1] + 16 * i, 290 - 11 * G[0], difC[i][G[0]], TRUE);
		}
		//難易度の名前を表示する
		if (LR == 1) {
			DrawGraph(460, 320, difbar[command[1]], TRUE);
			DrawGraph(-0.00288*XmoveC*XmoveC + 640, 320, difbar[command[1] + 1], TRUE);
		}
		else if (LR == -1) {
			DrawGraph(460, 320, difbar[command[1] - 1], TRUE);
			DrawGraph(0.00288*XmoveC*XmoveC + 460, 320, difbar[command[1]], TRUE);
		}
		//詳細を表示する
		DrawGraph(316, 370, detail, TRUE);
		DrawFormatString(500, 390, Cr[3], L"LEVEL:%d", level[command[0]][command[1]]);
		//プレビューを流す
		if (moveC == 0 && XmoveC == 0 && strands(songM[command[0]][command[1]], GT14) == 0 && strands(playingsong, songM[command[0]][command[1]]) == 0) {
			StopSoundMem(previewM);
			DeleteSoundMem(previewM);
			strcopy(songM[command[0]][command[1]], playingsong, 1);
			previewM = LoadSoundMem(playingsong);
			SetCurrentPositionSoundMem(preview[command[0]][command[1]][0], previewM);
			PlaySoundMem(previewM, DX_PLAYTYPE_BACK, FALSE);
			WaitTimer(30);
		}
		//ディスクを表示する(レート表示場所)
		DrawRotaGraph(640, 25, 1, diskr, disk, TRUE);
		diskr += 0.02;
		if (diskr > 6.28) diskr -= 6.28;
		DrawFormatString(550, 100, Cr[0], L"RATE:%5.2f", rate[0] + rate[1] + rate[2] + rate[3] + rate[4] + rate[5] + rate[6] + rate[7] + rate[8] + rate[9]);
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"上下キー:曲選択　左右キー:難易度選択   Enterキー:決定   BackSpaceキー:戻る", Cr[0]);
		else if (lan[4] == 1)DrawString(5, 460, L"↑↓key:music select, ←→key:dif select, Enter key:start, BackSpace key:back to menu", Cr[0]);
		//デバッグ(320,410スタート)
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_UP) == 1) {
			//上が押された
			if (key == 0) {
				command[0]--;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				UD = -1;
				startC = GetNowCount();
				//縦コマンド(曲)の端を過ぎたとき、もう片方の端に移動する
				if (command[0] < 0) command[0] = n;
				else if (command[0] > n) command[0] = 0;
				//今選んだ曲の難易度に譜面が無かったら、譜面がある難易度を探す。
				strands(songname[command[0]][command[1]], GT14);
				if (strands(songname[command[0]][command[1]], GT14)) {
					if (strands(songname[command[0]][0], GT14) != 1) command[1] = 0;
					for (i = 1; i <= 3; i++) {
						if (strands(songname[command[0]][i], GT14) != 1) {
							command[1] = i;
							break;
						}
					}
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN) == 1) {
			//下が押された
			if (key == 0) {
				command[0]++;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				UD = 1;
				startC = GetNowCount();
				//縦コマンド(曲)の端を過ぎたとき、もう片方の端に移動する
				if (command[0] < 0) command[0] = n;
				else if (command[0] > n) command[0] = 0;
				//今選んだ曲の難易度に譜面が無かったら、譜面がある難易度を探す。
				strands(songname[command[0]][command[1]], GT14);
				if (strands(songname[command[0]][command[1]], GT14)) {
					if (strands(songname[command[0]][0], GT14) != 1) command[1] = 0;
					for (i = 1; i <= 3; i++) {
						if (strands(songname[command[0]][i], GT14) != 1) {
							command[1] = i;
							break;
						}
					}
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_LEFT) == 1) {
			//左が押された
			if (key == 0) {
				command[1]--;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				LR = 1;
				XstartC = GetNowCount();
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT) == 1) {
			//右が押された
			if (key == 0) {
				command[1]++;
				PlaySoundMem(select, DX_PLAYTYPE_BACK);
				LR = -1;
				XstartC = GetNowCount();
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN) == 1) {
			//エンターが押された(曲ファイル名がNULLの場合はスキップ)
			if (key == 0 && strands(songM[command[0]][command[1]], GT14) == 0) {
				//隠し曲用
				if (command[1] == 3 && Hscore[command[0]][3] >= 90000 && strands(songM[command[0]][5], GT14) == 0 && Hscore[command[0]][5] <= 0) {
					G[0] = 0;
					if (Hscore[command[0]][3] >= 90000 && Hscore[command[0]][3] < 92500)G[0] = Hscore[command[0]][3] / 250000.0 * Hscore[command[0]][3] - Hscore[command[0]][3] * 0.72 + 32400;
					else if (Hscore[command[0]][3] >= 92500 && Hscore[command[0]][3] < 95000)G[0] = -1 * Hscore[command[0]][3] / 250000.0 * Hscore[command[0]][3] + Hscore[command[0]][3] * 0.76 - 36050;
					else if (Hscore[command[0]][3] >= 95000 && Hscore[command[0]][3] < 98000)G[0] = Hscore[command[0]][3] / 12857.14 * Hscore[command[0]][3] - Hscore[command[0]][3] * 14.777777 + 701994;
					else if (Hscore[command[0]][3] >= 98000 && Hscore[command[0]][3] < 99000)G[0] = -1 * Hscore[command[0]][3] / 4000.0 * Hscore[command[0]][3] + Hscore[command[0]][3] * 49.5 - 2449250;
					else if (Hscore[command[0]][3] >= 99000)G[0] = 1000;
					if (GetRand(1000) <= G[0])command[1] = 5;
				}
				StopSoundMem(previewM);
				ClearDrawScreen();
				InitSoundMem();
				InitGraph();
				*p1 = command[0];
				p1++;
				*p1 = command[1];
				next = 6;
				break;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK) == 1) {
			//バックスペースが押された
			if (key == 0) {
				StopSoundMem(previewM);
				ClearDrawScreen();
				next = 1;
				break;
			}
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			next = 5;
			break;
		}
		else key = 0;
		//横コマンド(難易度)の端を過ぎないようにする
		if (command[1] < 0) {
			command[1] = 0;
			XstartC -= 250;
		}
		else if (command[1] > rimit[command[0]]) {
			command[1] = rimit[command[0]];
			XstartC -= 250;
		}
		WaitTimer(5);
	}
	return next;
}

int play2(int n, int o) {
	short int i[2];
	short int Lv = 0;
	short int notes = 0;
	short int bgp[3] = { 0,0,0 }; //[0:sky,1:ground,2:water]の横位置
	short int bgf[2] = { 0,0 }; //落ち物背景の[0:横位置,1:縦位置]
	short int charaput = 1; //キャラの今の位置[0で上,1で中,2で下]
	short int drop = 0;
	int judgh = 0; //ノーツの時間距離
	int charahit = 0; //キャラがノーツをたたいた後であるかどうか。[1以上で叩いた、0で叩いてない]
	int G[10], songT;
	int system[5] = { 0,0,0,2,0 };
	int noteoff = 0; //ノーツのオフセット
	int Etime = 0; //譜面の終わりの時間
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
	int Dscore[3] = { 0,0,0 }; //距離に当たる部分[加点用,加点保存用,距離保存用]
	int judghcount[4] = { 0,0,0,0 };
	int life = 500;
	int gap[30] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//gap = 判定ずれ
	int gapa[3] = { 0,0,0 };//gapa = 判定ずれ[合計, 個数, 2乗の合計]
	int score2[4] = { 0,0,0,0 }; //[通常スコア(90000), コンボスコア(10000), 減点, 合計]
	int fps[62];//0〜59=1フレーム間隔の時間,60=次の代入先,61=前回の時間
	for (i[0] = 0; i[0] <= 59; i[0]++)fps[i[0]] = 17;
	fps[60] = 0;
	fps[61] = 0;
	double GD[5];
	int item[99]; //アイテムのアドレス、DrawGraphで呼べる。
	short int itemN = 0; //↑の番号
	int chamo[3][99][2]; //キャラの[0:上,1:中,2:下]の[0:グラフィック,1:切り替え時間]
	chamo[0][0][0] = 0;
	chamo[0][0][1] = 0;
	chamo[1][0][0] = 1;
	chamo[1][0][1] = 0;
	chamo[2][0][0] = 1;
	chamo[2][0][1] = 0;
	short int chamoN[3] = { 1,1,1 }; //↑の番号
	int fall[99][2]; //落ち物背景の[0:番号,1:時間]
	fall[0][0] = 0;
	fall[0][1] = -1;
	short int fallN = 1; //↑の番号
	int Ymove[5][999][4]; //[上, 中, 下, (地面), (水中)]レーン縦移動の[0:開始時間,1:位置,2:終了時間,3:種類]
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
	short int YmoveN[5] = { 1,1,1,1,1 }; //↑の番号
	short int YmoveN2[3] = { 0,0,0 };
	int Xmove[3][999][4]; //[上, 中, 下]レーン横移動の[0:開始時間,1:位置,2:終了時間,3:種類]
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
	short int XmoveN[3] = { 1,1,1 }; //↑の番号
	short int XmoveN2[3] = { 0,0,0 };
	int lock[2][2][99]; //lock = [横,縦]の音符の位置を[(1=固定する,-1以外=固定しない),時間]
	lock[0][0][0] = -1;
	lock[0][1][0] = 0;
	lock[1][0][0] = 1;
	lock[1][1][0] = 0;
	short int lockN[2] = { 1,1 }; //↑の番号
	int carrow[2][99];
	carrow[0][0] = 1;
	carrow[1][0] = 0;
	short int carrowN = 1;
	int viewT[2][99];//[音符表示時間,実行時間,[0]=現ナンバー]
	viewT[0][0] = 0;
	viewT[1][0] = 3000;
	short int viewTN = 1;
	int object[3][5][999]; //[上,中,下]レーンの音符の[時間,種類,(使ってない),縦位置,横位置]
	object[0][3][0] = 300;
	object[1][3][0] = 350;
	object[2][3][0] = 400;
	short int objectN[3] = { 0,0,0 }; //↑の番号
	int difkey[50][4];//難易度計算に使う[番号][入力キー,時間,難易度点,[0]個数上限:[1]今の番号:[2]1個前の番号:[3]2個前の番号:[4]最高点:[5]データ個数:[6]最後50個の合計:[7]計算から除外する時間]
	difkey[0][2] = 0;
	difkey[1][2] = 0;
	difkey[1][3] = 0;
	difkey[2][3] = -1;
	difkey[3][3] = -2;
	difkey[4][3] = 0;
	difkey[5][3] = -1;
	int ddif[25] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//各区間の難易度
	int ddifG[2] = { 1,1 };//0=今いる区間番号(1〜25),1=最大値
	int Yline[5] = { 300,350,400,350,600 };//[上,中,下,地面,水中]レーンの縦位置
	int Xline[3] = { 150,150,150 };//[上,中,下]レーンの横位置
	double rate, bpm = 120, bpmG = 120;
	double timer[3]; //[上, 中, 下]レーンの時間
	double speedt[5][99][2]; //[上, 中, 下, (地面), (水中)]レーンの[0:切り替え時間,1:速度]
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
	short int speedN[5] = { 1,1,1,1,1 }; //↑の番号
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
	//システムロード
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
	//テキストデータを読む
	while (FileRead_eof(songdata) == 0) {
		FileRead_gets(GT1, 256, songdata);
		//音楽ファイルを読み込む
		if (strands(GT1, RecordCode[0])) {
			strmods(GT1, 7);
			strcats(mp3FN, GT1);
		}
		//BPMを読み込む
		else if (strands(GT1, RecordCode[1])) bpmG = bpm = SETbpm(GT1);
		//ノートのオフセットを読み込む
		else if (strands(GT1, RecordCode[2])) timer[0] = timer[1] = timer[2] = noteoff = SEToffset(GT1, system[1]);
		//空の背景を読み込む
		else if (strands(GT1, RecordCode[3])) {
			strcopy(GT24, skyFN, 1);
			strmods(GT1, 5);
			strcats(skyFN, GT1);
		}
		//地面の画像を読み込む
		else if (strands(GT1, RecordCode[4])) {
			strcopy(GT24, groundFN, 1);
			strmods(GT1, 7);
			strcats(groundFN, GT1);
		}
		//水中の画像を読み込む
		else if (strands(GT1, RecordCode[5])) {
			strcopy(GT24, waterFN, 1);
			strmods(GT1, 7);
			strcats(waterFN, GT1);
		}
		//曲名を読み込む
		else if (strands(GT1, RecordCode[6])) {
			strmods(GT1, 7);
			if (system[4] == 0 || songN[0] == L'\0')strcopy(GT1, songN, 1);
		}
		//英語
		else if (strands(GT1, RecordCode[21])) {
			strmods(GT1, 7);
			if (system[4] == 1 || songN[0] == L'\0')strcopy(GT1, songN, 1);
		}
		//レベルを読み込む
		else if (strands(GT1, RecordCode[7])) Lv = SETLv(GT1);
		//アイテムを読み込む
		else if (strands(GT1, RecordCode[8])) {
			strmods(GT1, 6);
			strcopy(dataE, GT15, 1);
			stradds(GT15, L'/');
			strcats(GT15, GT1);
			item[itemN++] = LoadGraph(GT15);
		}
		//落ち物背景指定
		else if (strands(GT1, RecordCode[9])) {
			strmods(GT1, 6);
			fall[0][0] = strsans(GT1);
			fall[0][1] = 0;
		}
		//譜面を読み込む
		else if (strands(GT1, RecordCode[10])) {
			while (FileRead_eof(songdata) == 0 && strands(GT1, RecordCode[11]) == 0) {
				FileRead_gets(GT1, 256, songdata);
				//横ノーツ速度変化
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
				//BPM変化
				else if (strands(GT1, RecordCode[1])) bpmG = SETbpm(GT1);
				//キャラグラ変化
				else if (strands(GT1, RecordCode[13])) {
					G[0] = GT1[6] - 49;
					strmods(GT1, 8);
					chamo[G[0]][chamoN[G[0]]][0] = maxs(mins(strsans(GT1), 0), 2);
					chamo[G[0]][chamoN[G[0]]][1] = timer[G[0]];
					chamoN[G[0]]++;
				}
				//縦移動
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
				//横移動
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
				//横ロック
				else if (strands(GT1, RecordCode[17])) {
					strmods(GT1, 7);
					lock[0][0][lockN[0]] = lock[0][0][lockN[0] - 1] * -1;
					lock[0][1][lockN[0]] = shifttime(strsans(GT1), bpmG, timer[0]);
					lockN[0]++;
				}
				//縦ロック
				else if (strands(GT1, RecordCode[18])) {
					strmods(GT1, 7);
					lock[1][0][lockN[1]] = lock[1][0][lockN[1] - 1] * -1;
					lock[1][1][lockN[1]] = shifttime(strsans(GT1), bpmG, timer[0]);
					lockN[1]++;
				}
				//キャラ向き変化
				else if (strands(GT1, RecordCode[22])) {
					strmods(GT1, 8);
					carrow[0][carrowN] = carrow[0][carrowN - 1] * -1;
					carrow[1][carrowN] = shifttime(strsans(GT1), bpmG, timer[0]);
					carrowN++;
				}
				//落ち物背景切り替え
				else if (strands(GT1, RecordCode[19])) {
					strmods(GT1, 6);
					fall[fallN][0] = strsans(GT1);
					strnex(GT1);
					fall[fallN][1] = strsans(GT1);
					fallN++;
				}
				//音符表示時間
				else if (strands(GT1, RecordCode[20])) {
					strmods(GT1, 6);
					viewT[0][viewTN] = shifttime(strsans(GT1), bpmG, timer[0]);
					strnex(GT1);
					viewT[1][viewTN] = strsans(GT1);
					viewTN++;
				}
				//終わり
				else if (strands(GT1, RecordCode[11])) {
					break;
				}
				//これ以外
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
							//object[][2][]を使いたかったらここに入れる(ゲーム内時間になる予定)
							object[i[0]][3][objectN[i[0]]] = 50 * i[0] + 300;
							object[i[0]][4][objectN[i[0]]] = 150;
							//縦位置を計算する
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
							//横位置を計算する
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
	//譜面の最後にgoustを置く
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
	/*難易度計算
	(3000000/ひとつ前のノーツとの間隔)を、そのノーツの難易度点数とする。
	叩いたキーを50個記憶し、この組み合わせでできた最高値が曲の難易度。
	1個前と同じキーの時は(対象のキーはarrowのみ)
	┣間隔が150ms以上の時は変動なし
	┣間隔が150ms以下75ms以上の時は得点("間隔"/(-75)+3)倍(1〜2倍)
	┗間隔が75ms以下の時は得点2倍
	arrowひっかけは2.5倍
	2個前と違うキーの時は得点1.2倍(全キー対象)
	arrowキーは得点1.2倍
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
		//ddifの計算
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
	//モーニンググローリーLv.1->884, Trick MonitorLv.9->28632, "測定"/31.8+47.1="Lv"*100
	difkey[4][3] = difkey[4][3] / 31.8 + 47.1;
	difkey[6][3] = difkey[6][3] / 31.8 + 47.1;
	//ナンバーリセット
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
	//グラフィックと効果音の準備
	/*グラフィック用変数
	judghimg = 判定マーク
	hitimg = hit
	hitcimg = hitの周りの色
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
	musicmp3 = 音楽*/
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
	//ゲーム開始前の下準備
	notes = notzero(notes);
	PlaySoundMem(musicmp3, DX_PLAYTYPE_BACK);
	WaitTimer(10);
	int Stime = GetNowCount();
	//ゲーム開始
	while (1) {
		ClearDrawScreen();
		GetHitKeyStateAll(key);
		//背景を真っ暗にしている場合、背景を表示しない。
		if (system[3] != 0) {
			//背景の横位置計算
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
			//背景の縦位置計算
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
			//背景表示
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
			//落ち物背景表示
			if (Ntime >= fall[fallN + 1][0] && fall[fallN + 1][0] >= 0) fallN++;
			if (fall[fallN][1] >= 0) {
				G[0] = bgf[0];//横
				G[1] = bgf[1] + Yline[3];//縦
				for (i[0] = 0; i[0] < 2; i[0]++) for (i[1] = 0; i[1] < 3; i[1]++) DrawGraph(G[0] + i[0] * 640, G[1] - i[1] * 480, item[fall[fallN][1]], TRUE);
				bgf[0] -= 5;
				bgf[1] += 2;
			}
			if (bgf[0] <= -640)bgf[0] += 640;
			if (bgf[1] >= 640)bgf[1] -= 480;
			//アイテム表示

		}
		//フィルター表示
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
		//キャラ位置ガイドの表示
		if (carrow[1][carrowN + 1] < Ntime && carrow[1][carrowN + 1] >= 0) carrowN++;
		if (carrow[0][carrowN] == 1) { DrawGraph(Xline[charaput] - 4, Yline[charaput] - 4, charaguideimg, TRUE); }
		else { DrawTurnGraph(Xline[charaput] - 56, Yline[charaput] - 4, charaguideimg, TRUE); }
		//判定マーカーの表示
		for (i[0] = 0; i[0] < 3; i[0]++) {
			//縦移動
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
			//横移動
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
		//キャラグラ変換
		for (i[0] = 0; i[0] < 3; i[0]++)while (Ntime >= chamo[i[0]][chamoN[i[0]] + 1][1] && chamo[i[0]][chamoN[i[0]] + 1][1] >= 0) chamoN[i[0]]++;
		G[3] = 0;
		//キャラ位置決め
		for (i[0] = 0; i[0] <= 2; i[0]++) if (object[i[0]][0][objectN[i[0]]] <= Ntime + 16 && object[i[0]][1][objectN[i[0]]] == 2) G[3] = 1;
		for (i[0] = 0; i[0] <= 2; i[0]++) {
			G[i[0]] = objectN[i[0]] - 1;
			while (object[i[0]][1][G[i[0]]] == 8 && G[i[0]] >= 1) G[i[0]]--;
		}
		//上ラインのヒットマーカーをたたいているとき
		if (charahit > 0 && G[3] == 0 && object[0][0][G[0]] > object[1][0][G[1]] && object[0][0][G[0]] > object[2][0][G[2]] && object[0][0][G[0]] + 750 > Ntime && object[0][1][G[0]] == 1 && judghname[0][0] >= 1 && judghname[0][0] <= 3) {
			G[4] = Yline[0];
			charaput = 0;
		}
		//中ラインのヒットマーカーをたたいているとき
		else if (charahit > 0 && G[3] == 0 && object[1][0][G[1]] > object[0][0][G[0]] && object[1][0][G[1]] > object[2][0][G[2]] && object[1][0][G[1]] + 750 > Ntime && object[1][1][G[1]] == 1 && judghname[1][0] >= 1 && judghname[1][0] <= 3) {
			G[4] = Yline[1];
			charaput = 1;
		}
		//下ラインのヒットマーカーをたたいているとき
		else if (charahit > 0 && G[3] == 0 && object[2][0][G[2]] > object[0][0][G[0]] && object[2][0][G[2]] > object[1][0][G[1]] && object[2][0][G[2]] + 750 > Ntime && object[2][1][G[2]] == 1 && judghname[2][0] >= 1 && judghname[2][0] <= 3) {
			G[4] = Yline[2];
			charaput = 2;
		}
		//上が押されて、直前のヒットマーカーをたたいていないとき
		else if (key[KEY_INPUT_UP] == 1 && key[KEY_INPUT_DOWN] == 0) {
			G[4] = Yline[0];
			charaput = 0;
		}
		//下が押されて、直前のヒットマーカーをたたいていないとき
		else if (key[KEY_INPUT_UP] == 0 && key[KEY_INPUT_DOWN] == 1) {
			G[4] = Yline[2];
			charaput = 2;
		}
		//何も押されていないか、上下同時に押されていて、直前のヒットマーカーをたたいていないとき
		else {
			G[4] = Yline[1];
			charaput = 1;
		}
		//キャラグラフィックを表示
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
		//キャッチ判定に使う数値を計算
		if (holdu == 0 && holdd == 0 || holdu > 0 && holdd > 0) { LaneTrack[1] = Ntime; }
		else if (holdu > 0 && holdd == 0) { LaneTrack[0] = Ntime; }
		else if (holdu == 0 && holdd > 0) { LaneTrack[2] = Ntime; }
		if (LaneTrack[0] <= LaneTrack[2]) { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[0]); }
		else { LaneTrack[1] = mins(LaneTrack[1], LaneTrack[2]); }
		//ヒット
		if (holda == 1 || holdb == 1) charahit = GetNowCount();
		if (charahit + 750 < GetNowCount()) charahit = 0;
		//音符表示
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
				//縦位置
				if (lock[1][0][lockN[1] + G[4]] == 1) G[2] = object[i[0]][3][i[1]];
				else G[2] = Yline[i[0]];
				//横位置
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
		//判定
		G[1] = 0;
		for (i[0] = 0; i[0] < 3; i[0]++) {
			judgh = object[i[0]][0][objectN[i[0]]] - Ntime;
			//ヒットノーツ
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
			//キャッチノーツ(justのみ)
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
			//アローノーツ各種
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
			//ボムノーツ
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
			//ゴーストノーツ
			else if (object[i[0]][1][objectN[i[0]]] == 8 && judgh < 16) objectN[i[0]]++;
			//全ノーツslowmiss
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
		//判定+ヒットエフェクト表示
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
		//ライフが0未満の時、1毎に減点スコアを20増やす。
		if (life < 0) {
			score2[2] = maxs(score2[2] - life * 20, score2[0] + score2[1]);
			life = 0;
		}
		//スコアバー表示
		DrawGraph(0, 0, sbarimg, TRUE);
		//点数表示
		score2[0] = (judghcount[0] * 90000 + judghcount[1] * 85000 + judghcount[2] * 45000) / notes;
		score2[1] = Mcombo * 10000 / notes;
		DrawFormatString(490, 20, Cr, L"SCORE:%d", score2[0] + score2[1] - score2[2]);
		//ライフ表示
		life = maxs(life, 500);
		if (life > 100) {
			DrawGraph((291 * life - 57000) / 500, 3, Lbarimg[0], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (-51 * life + 25500) / 80);
			DrawGraph((291 * life - 57000) / 500, 3, Lbarimg[1], TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
		else DrawGraph((291 * life - 57000) / 500, 3, Lbarimg[2], TRUE);
		DrawFormatString(440, 10, Cr, L"%3d", life);
		//距離表示
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
		//部分難易度表示
		if (holdG >= 1) {
			G[0] = ddif[0] * 20 / notzero(ddifG[1]) + 155;
			G[1] = ddif[24] * 20 / notzero(ddifG[1]) + 447;
			for (i[0] = 0; i[0] <= 23; i[0]++) DrawLine((G[0] * (24 - i[0]) + G[1] * i[0]) / 24, -ddif[i[0]] * 34 / notzero(ddifG[1]) + 72, (G[0] * (23 - i[0]) + G[1] * (1 + i[0])) / 24, -ddif[i[0] + 1] * 34 / notzero(ddifG[1]) + 72, Cr);
			DrawFormatString(490, 80, Cr, L"mdif:%.2f", difkey[4][3] / 100.0);
			DrawFormatString(490, 100, Cr, L"ldif:%.2f", difkey[6][3] / 100.0);
		}
		//スコアバー隠し表示
		DrawGraph(0, 0, sbbarimg, TRUE);
		//コンボ表示
		if (combo >= 1) DrawFormatString(15, 15, Cr, L"%d combo", combo);
		//判定ずれバー表示
		DrawGraph(219, 460, gapbarimg, TRUE);
		G[0] = gapa[1] % 30;
		for (i[0] = 0; i[0] < 30; i[0]++) {
			G[0]--;
			if (G[0] < 0) G[0] += 30;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (510 - G[0] * 17) / 2);
			DrawGraph(318 - gap[i[0]], 460, gaplineimg, TRUE);
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		//デバック
		fps[fps[60]++] = Ntime - fps[61];
		if (fps[60] > 59)fps[60] -= 60;
		fps[61] = Ntime;
		G[0] = 0;
		for (i[0] = 0; i[0] <= 59; i[0]++)G[0] += fps[i[0]];
		if (Ntime != 0) DrawFormatString(20, 80, Cr, L"FPS: %.0f", 60000.0 / notzero(G[0]));
		//for (i[0] = 0; i[0] < 3; i[0]++) { DrawFormatString(20, 100 + i[0] * 20, Cr, L"LaneTrack%d: %d", i[0], LaneTrack[i[0]]); }
		//ライフが20%以下の時、危険信号(ピクチャ)を出す
		if (life <= 100 && drop == 0) DrawGraph(0, 0, dangerimg, TRUE);
		//ライフがなくなったらDROPED扱い
		if (life <= 0 && drop == 0) {
			drop = 1;
			Dscore[1] = Dscore[0];
			Dscore[2] = mins(Ntime - noteoff, 0);
		}
		if (drop) { DrawGraph(0, 0, dropimg, TRUE); }
		//終了時間から1秒以上たって、曲が終了したら抜ける。
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
	//セーブ処理
	strcats(save, fileN);
	strcats(save, save2);
	//ハイスコア保存
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
	//プレイ回数保存
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
	//レート計算(level0なら0固定)"レベル" - "miss数" x "レベル" x 0.03(下限=0)
	if (Lv == 0) rate = 0;
	else if (judghcount[3] > 0) {
		rate = Lv - judghcount[3] * Lv*0.03;
		rate = mins(rate, 0);
	}
	//NO MISS,"レベル" + 1 - "safe数" x 0.05(下限="レベル")
	else if (judghcount[3] == 0 && judghcount[2] > 0) {
		rate = Lv + 1 - judghcount[2] * 0.05;
		rate = mins(rate, Lv);
	}
	//FULL COMBO,"レベル" + 2 - "good数" x 0.01(下限="レベル" + 1)
	else if (judghcount[3] == 0 && judghcount[2] == 0 && judghcount[1] > 0) {
		rate = Lv + 2 - judghcount[1] * 0.01;
		rate = mins(rate, Lv + 1);
	}
	//PERFECT, "レベル" + 2
	else if (judghcount[3] == 0 && judghcount[2] == 0 && judghcount[1] == 0) rate = Lv + 2;
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
		strcopy(fileN, savec[G[0]], 1);
	}
	G[0] = _wfopen_s(&fp, L"save/rateS.dat", L"wb");
	fwrite(&savec, 255, 10, fp);
	fclose(fp);
	G[0] = _wfopen_s(&fp, L"save/rateN.dat", L"wb");
	fwrite(&readR, sizeof(double), 10, fp);
	fclose(fp);
	//リザルト画面
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
		//エンターが押された
		if (CheckHitKey(KEY_INPUT_RETURN) == 1) break;
		else if (GetWindowUserCloseFlag(TRUE)) return 5;
	}
	InitGraph();
	return 2;
}

int collection(void) {
	int key = 1, command = 0, end = 0;
	int backimg, icon[4], G[2], help, sel;
	unsigned int Cr;
	Cr = GetColor(255, 255, 255);
	backimg = LoadGraph(L"picture/COLLECT back.png");
	icon[0] = LoadGraph(L"picture/CLI.png");
	icon[1] = LoadGraph(L"picture/CLIG.png");
	icon[2] = LoadGraph(L"picture/CLSG.png");
	icon[3] = LoadGraph(L"picture/CLS.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.ogg");
	int	lan[5] = { 0,0,0,2,0 };//使うのは[4,言語]だけ
	FILE *fp;
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&lan, sizeof(lan), 5, fp);
		fclose(fp);
	}
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		DrawGraph(70, 130, icon[0 + command], TRUE);
		DrawGraph(350, 130, icon[2 + command], TRUE);
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"左右キー:選択   Enterキー:決定   BackSpaceキー:戻る", Cr);
		if (lan[4] == 1)DrawString(5, 460, L"←→key:select   Enter key:enter   BackSpace key:back to menu", Cr);
		ScreenFlip();
		//ここからキー判定
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//左が押された
			if (key == 0) {
				command--;
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//右が押された
			if (key == 0) {
				command++;
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN)) {
			//エンターが押された
			if (key == 0) {
				switch (command) {
				case 0:
					end = C_item();
					break;
				case 1:
					end = C_story();
					break;
				}
			}
			if (end == 5) {
				return 5;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK) == 1) {
			//バックスペースが押された
			if (key == 0) {
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			//閉じるボタンが押された
			return 5;
		}
		else {
			//特定のキーが押されていない
			key = 0;
		}
		if (command < 0) {
			command += 2;
		}
		if (command > 1) {
			command -= 2;
		}
	}
	return 1;
}

int C_item(void) {
	//ロード
	int e, i, j, win, cover, backimg, item[11], G[2], help, sel;
	int flag[10] = { 0,0,0,0,0,0,0,0,0,0 };
	double Grate = 0;
	int	play[7];
	double rate[10];
	int	lan[5] = { 0,0,0,2,0 };//使うのは[4,言語]だけ
	FILE *fp;
	G[0] = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (G[0] == 0) {
		fread(&lan, sizeof(lan), 5, fp);
		fclose(fp);
	}
	win = LoadGraph(L"picture/item枠.png");
	cover = LoadGraph(L"picture/itemカバー.png");
	backimg = LoadGraph(L"picture/COLLECT back.png");
	item[0] = LoadGraph(L"picture/item0.png");
	item[1] = LoadGraph(L"picture/item1.png");
	item[2] = LoadGraph(L"picture/item2.png");
	item[3] = LoadGraph(L"picture/item3.png");
	item[4] = LoadGraph(L"picture/item4.png");
	item[5] = LoadGraph(L"picture/item5.png");
	item[6] = LoadGraph(L"picture/item6.png");
	item[7] = LoadGraph(L"picture/item7.png");
	item[8] = LoadGraph(L"picture/item8.png");
	item[9] = LoadGraph(L"picture/item9.png");
	item[10] = LoadGraph(L"picture/item10.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.ogg");
	e = _wfopen_s(&fp, L"save/data.dat", L"rb");
	if (e == 0) {
		fread(&play, sizeof(play), 7, fp);
		fclose(fp);
	}
	e = _wfopen_s(&fp, L"save/rateN.dat", L"rb");
	if (e == 0) {
		fread(&rate, sizeof(rate), 10, fp);
		fclose(fp);
	}
	for (e = 0; e < 10; e++) if (rate[e] >= 0 && rate[e] <= 15) Grate += rate[e] * 1.0;
	//達成フラグ
	if (play[1] >= 1) flag[0] = 1;
	if (play[3] >= 1) flag[1] = 1;
	if (play[4] >= 1) flag[2] = 1;
	if (play[5] >= 1) flag[3] = 1;
	if (play[6] >= 1) flag[4] = 1;
	if (play[0] >= 1) flag[5] = 1;
	if (play[0] >= 10) flag[6] = 1;
	if (Grate >= 25) flag[7] = 1;
	if (Grate >= 55) flag[8] = 1;
	if (Grate >= 90) flag[9] = 1;
	//動作
	int key = 1;
	int	command[2] = { 0,0 };
	unsigned int Cr;
	Cr = GetColor(255, 255, 255);
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		for (i = 0; i < 2; i++) {
			for (j = 0; j < 3; j++) {
				DrawGraph(110 + j * 150, 20 + i * 240, win, TRUE);
				if (flag[j + i * 7]) {
					DrawGraph(113 + j * 150, 23 + i * 240, item[1 + j + i * 7], TRUE);
				}
				else {
					DrawGraph(113 + j * 150, 23 + i * 240, item[0], TRUE);
				}
				if (j + i * 7 != (7 * command[1] + 2 * command[0]) / 2) {
					DrawGraph(110 + j * 150, 20 + i * 240, cover, TRUE);
				}
			}
		}
		for (i = 0; i < 4; i++) {
			DrawGraph(40 + i * 150, 140, win, TRUE);
			if (flag[3 + i]) DrawGraph(43 + i * 150, 143, item[4 + i], TRUE);
			else DrawGraph(43 + i * 150, 143, item[0], TRUE);
			if (3 + i != 3 * command[1] + command[0] || command[1] != 1) DrawGraph(40 + i * 150, 140, cover, TRUE);
		}
		switch (command[1]) {
		case 0:
			switch (command[0]) {
			case 0:
				if (play[1] >= 1 && lan[4] == 0)DrawString(50, 400, L"割れたレコード:\n衝撃に耐えられずに割れてしまったもの。\nもう使えない…。[1回以上DROPED]", Cr);
				else if (play[1] >= 1 && lan[4] == 1)DrawString(50, 400, L"Broken Record Disc:\nThe broken record disc by shock.\nIt cannot be used... [Finish by DROPED]", Cr);
				else if (play[1] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1回以上DROPED]", Cr);
				else if (play[1] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by DROPED]", Cr);
				break;
			case 1:
				if (play[3] >= 1 && lan[4] == 0)DrawString(50, 400, L"サウンドロップ:\n今、人気のお菓子、マップゲーターも持ち歩いている。\n1袋13pop(110円)。[1回以上CLEARED]", Cr);
				else if (play[3] >= 1 && lan[4] == 1)DrawString(50, 400, L"Soundrop:\nThe popular candy now.\nMapgator have it on his bag. [Finish by CLEARED]", Cr);
				else if (play[3] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1回以上CLEARED]", Cr);
				else if (play[3] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by CLEARED]", Cr);
				break;
			case 2:
				if (play[4] >= 1 && lan[4] == 0)DrawString(50, 400, L"ミルク:\n株式会社myumyuが製造しているミルク。さっぱりした味わいだ。\nピッカーの好物。[1回以上NO MISS]", Cr);
				else if (play[4] >= 1 && lan[4] == 1)DrawString(50, 400, L"Milk:\nThe milk made by company \"myumyu\". It have tidy taste.\nPicker like it. [Finish by NO MISS]", Cr);
				else if (play[4] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1回以上NO MISS]", Cr);
				else if (play[4] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by NO MISS]", Cr);
				break;
			}
			break;
		case 1:
			switch (command[0]) {
			case 0:
				if (play[5] >= 1 && lan[4] == 0)DrawString(50, 400, L"ワッフル:\nすべての音を調和したときにできる結晶。\nとてもおいしいが、味の好みが分かれるらしい。[1回以上FULL COMBO]", Cr);
				else if (play[5] >= 1 && lan[4] == 1)DrawString(50, 400, L"Waffle:\nThe crystal produced by harmoning all sound.It's very good \ntaste but some people feel that a bad. [Finish by FULL COMBO]", Cr);
				else if (play[5] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1回以上FULL COMBO]", Cr);
				else if (play[5] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by FULL COMBO]", Cr);
				break;
			case 1:
				if (play[6] >= 1 && lan[4] == 0)DrawString(50, 400, L"パフェ:\n音を完璧に調和したときにできる結晶。\nテイラーの大好物。とってもあまい。[1回以上PERFECT]", Cr);
				else if (play[6] >= 1 && lan[4] == 1)DrawString(50, 400, L"Parfait:\nThe parfait produced by harmoning perfectly the sound.\nTaylor is love it. It's very sweet. [Finish by PERFECT]", Cr);
				else if (play[6] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1回以上PERFECT]", Cr);
				else if (play[6] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Finish by PERFECT]", Cr);
				break;
			case 2:
				if (play[0] >= 1 && lan[4] == 0)DrawString(50, 400, L"ビギナーランナー証明カード:\nランナーであることを証明するもの。\n入ったばかりの初心者さんの証。[1回以上プレイ]", Cr);
				else if (play[0] >= 1 && lan[4] == 1)DrawString(50, 400, L"Beginner Runner Card:\nThe card indicating that you are a Runner.\nAnd the beginner just joined. [Play the game]", Cr);
				else if (play[0] < 1 && lan[4] == 0)DrawString(50, 400, L"???\n[1回以上プレイ]", Cr);
				else if (play[0] < 1 && lan[4] == 1)DrawString(50, 400, L"???\n[Play the game]", Cr);
				break;
			case 3:
				if (play[0] >= 10 && lan[4] == 0)DrawString(50, 400, L"ランナー手帳:\nランナーの記録手帳。事後調査や報告もこれで済ませる。\n走りながらメモする上級者もいる。[10回以上プレイ]", Cr);
				else if (play[0] >= 10 && lan[4] == 1)DrawString(50, 400, L"Runner's Note:\nRunner's comprehensive note. Job reports are written on this.\nSome experts write while running. [Play the game 10 times]", Cr);
				else if (play[0] < 10 && lan[4] == 0)DrawString(50, 400, L"???\n[10回以上プレイ]", Cr);
				else if (play[0] < 10 && lan[4] == 1)DrawString(50, 400, L"???\n[Play the game 10 times]", Cr);
				break;
			}
			break;
		case 2:
			switch (command[0]) {
			case 0:
				if (Grate >= 25 && lan[4] == 0)DrawString(50, 400, L"ブロンズランナー証明カード:\nランナーに慣れてきた人に贈られるカード。\nこれでやっとランナーとして認められる。[レート25.00達成]", Cr);
				else if (Grate >= 25 && lan[4] == 1)DrawString(50, 400, L"Bronze Runner Card:\nThe card given to Runner who are used to work.\nYou are admitted as the Runner. [Reach 25.00 rate]", Cr);
				else if (Grate < 25 && lan[4] == 0)DrawString(50, 400, L"???\n[レート25.00達成]", Cr);
				else if (Grate < 25 && lan[4] == 1)DrawString(50, 400, L"???\n[Reach 25.00 rate]", Cr);
				break;
			case 1:
				if (Grate >= 55 && lan[4] == 0)DrawString(50, 400, L"シルバーランナー証明カード:\nランナーとして十分功績を挙げた人に贈られるカード。\n難しい仕事も来るようになる。[レート55.00達成]", Cr);
				else if (Grate >= 55 && lan[4] == 1)DrawString(50, 400, L"Silver Runner Card:\nThe card given to great Runner.\nDifficult job will come to you. [Reach 55.00 rate]", Cr);
				else if (Grate < 55 && lan[4] == 0)DrawString(50, 400, L"???\n[レート55.00達成]", Cr);
				else if (Grate < 55 && lan[4] == 1)DrawString(50, 400, L"???\n[Reach 55.00 rate]", Cr);
				break;
			case 2:
				if (Grate >= 90 && lan[4] == 0)DrawString(50, 400, L"ゴールドランナー証明カード:\nランナーを極めた人に贈られるカード。\n世界各地のかなり難しい仕事も来るようになる。[レート90.00達成]", Cr);
				else if (Grate >= 90 && lan[4] == 1)DrawString(50, 400, L"Gold Runner Card:\nThe card given to EXCELLENT Runner. More difficult \njob all over the world will come to you. [Reach 90.00 rate]", Cr);
				else if (Grate < 90 && lan[4] == 0)DrawString(50, 400, L"???\n[レート90.00達成]", Cr);
				else if (Grate < 90 && lan[4] == 1)DrawString(50, 400, L"???\n[Reach 90.00 rate]", Cr);
				break;
			}
			break;
		}
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"上下左右キー:選択   BackSpaceキー:戻る", Cr);
		else if (lan[4] == 1)DrawString(5, 460, L"←↓↑→key:select   BackSpace key:back", Cr);
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//左が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[0]--;
				if (command[0] < 0) {
					command[0] = 3;
				}
				if (command[0] == 3 && command[1] == 0) {
					command[0] = 2;
				}
				if (command[0] == 3 && command[1] == 2) {
					command[0] = 2;
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//右が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[0]++;
				if (command[0] > 3) {
					command[0] = 0;
				}
				if (command[0] == 3 && command[1] == 0) {
					command[0] = 0;
				}
				if (command[0] == 3 && command[1] == 2) {
					command[0] = 0;
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_UP)) {
			//上が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[1]--;
				if (command[0] == 3 && command[1] == 0) {
					command[0] = 2;
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN)) {
			//下が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[1]++;
				if (command[0] == 3 && command[1] == 2) {
					command[0] = 2;
				}
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK)) {
			//バックスペースが押された
			if (key == 0) {
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			//閉じるボタンが押された
			return 5;
		}
		else {
			//特定のキーが押されていない
			key = 0;
		}
		if (command[1] < 0) {
			command[1] = 2;
		}
		if (command[1] > 2) {
			command[1] = 0;
		}
	}
	return 0;
}

int C_story(void) {
	int e, i, j, end = 0, key = 1, backimg, noteimg, help, sel, chac[3] = { 0,0,0 }, sub1[6], command[2] = { 0,0 }, g[7];
	wchar_t chan[4][40] = { L"ピッカー",L"マップゲーター",L"テイラー",L"サブストーリー1" };
	wchar_t chanE[4][40] = { L"Picker",L"Mapgator",L"Taylor",L"Sub Story 1" };
	unsigned int Cr, Crw;
	int	lan[5] = { 0,0,0,2,0 };//使うのは[4,言語]だけ
	FILE *fp;
	e = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (e == 0) {
		fread(&lan, sizeof(lan), 5, fp);
		fclose(fp);
	}
	//キャラプレイ回数読み込み
	e = _wfopen_s(&fp, L"save/chap.dat", L"rb");
	if (e == 0) {
		fread(&chac, sizeof(chac), 3, fp);
		fclose(fp);
	}
	//サブスト1のスコア読み込み
	for (i = 0; i < 6; i++) {
		switch (i) {
		case 0:
			e = _wfopen_s(&fp, L"score/snow town story.dat", L"rb");
			g[6] = 1;
			break;
		case 1:
			e = _wfopen_s(&fp, L"score/グラデーション・ワールド.dat", L"rb");
			g[6] = 1;
			break;
		case 2:
			e = _wfopen_s(&fp, L"score/What Color Is The Sky？.dat", L"rb");
			g[6] = 1;
			break;
		case 3:
			e = _wfopen_s(&fp, L"score/トリノユメ.dat", L"rb");
			g[6] = 2;
			break;
		case 4:
			e = _wfopen_s(&fp, L"score/トリノユメ.dat", L"rb");
			g[6] = 4;
			break;
		case 5:
			e = _wfopen_s(&fp, L"score/トリノユメ.dat", L"rb");
			g[6] = 5;
			break;
		}
		sub1[i] = 0;
		if (e == 0) {
			fread(&g, sizeof(g), 6, fp);
			fclose(fp);
			for (j = g[6]; j <= 5; j++) sub1[i] = mins(sub1[i], g[j]);
		}
		switch (i) {
		case 0:
			e = _wfopen_s(&fp, L"score/snow town story.dat", L"rb");
			g[6] = 1;
			break;
		case 1:
			e = _wfopen_s(&fp, L"score/Gradation-world.dat", L"rb");
			g[6] = 1;
			break;
		case 2:
			e = _wfopen_s(&fp, L"score/What Color Is The Sky.dat", L"rb");
			g[6] = 1;
			break;
		case 3:
			e = _wfopen_s(&fp, L"score/Torinoyume.dat", L"rb");
			g[6] = 2;
			break;
		case 4:
			e = _wfopen_s(&fp, L"score/Torinoyume.dat", L"rb");
			g[6] = 4;
			break;
		case 5:
			e = _wfopen_s(&fp, L"score/Torinoyume.dat", L"rb");
			g[6] = 5;
			break;
		}
		if (e == 0) {
			fread(&g, sizeof(g), 6, fp);
			fclose(fp);
			for (j = g[6]; j <= 5; j++) sub1[i] = mins(sub1[i], g[j]);
		}
	}
	Cr = GetColor(0, 0, 0);
	Crw = GetColor(255, 255, 255);
	backimg = LoadGraph(L"picture/COLLECT back.png");
	noteimg = LoadGraph(L"picture/Cnote.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.ogg");
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		DrawGraph(160, 0, noteimg, TRUE);
		if (lan[4] == 0)DrawFormatString(330, 220, Cr, L"%s", chan[command[1]]);
		else if (lan[4] == 1)DrawFormatString(330, 220, Cr, L"%s", chanE[command[1]]);
		if (command[1] < 3) {
			if (command[0] == 0) {
				if (chac[command[1]] >= 1) {
					if (lan[4] == 0)DrawString(300, 245, L"エピソード1", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 1", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawFormatString(300, 245, Cr, L"%sを使って\n1回プレイしよう。", chan[command[1]]);
					else if (lan[4] == 1)DrawFormatString(300, 245, Cr, L"Play the game\nwith %s.", chanE[command[1]]);
					e = 0;
				}
			}
			else if (command[0] == 1) {
				if (chac[command[1]] >= 2) {
					if (lan[4] == 0)DrawString(300, 245, L"エピソード2", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 2", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawFormatString(300, 245, Cr, L"%sを使って\n2回プレイしよう。", chan[command[1]]);
					else if (lan[4] == 1)DrawFormatString(300, 245, Cr, L"Play the game\n2 times with %s.", chanE[command[1]]);
					e = 0;
				}
			}
			else {
				if (chac[command[1]] >= command[0] * 3 - 2) {
					if (lan[4] == 0)DrawFormatString(300, 245, Cr, L"エピソード%d", command[0] + 1);
					else if (lan[4] == 1)DrawFormatString(300, 245, Cr, L"Episode %d", command[0] + 1);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawFormatString(300, 245, Cr, L"%sを使って\n%d回プレイしよう。", chan[command[1]], command[0] * 3 - 2);
					else if (lan[4] == 1)DrawFormatString(300, 245, Cr, L"Play the game\n%d times with %s.", command[0] * 3 - 2, chanE[command[1]]);
					e = 0;
				}
			}
		}
		else if (command[1] == 3) {
			switch (command[0]) {
			case 0:
				if (sub1[command[0]] >= 90000) {
					if (lan[4] == 0)DrawString(300, 245, L"エピソード1", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 1", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"snow town story[EASY]以上で\nAランクを取ろう", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath A RANK at\nsnow town story[EASY] or more dif.", Cr);
					e = 0;
				}
				break;
			case 1:
				if (sub1[command[0]] >= 90000) {
					if (lan[4] == 0)DrawString(300, 245, L"エピソード2", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 2", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"グラデーション・ワールド[EASY]以上で\nAランクを取ろう", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath A RANK at\nGradation-world[EASY] or more dif.", Cr);
					e = 0;
				}
				break;
			case 2:
				if (sub1[command[0]] >= 95000) {
					if (lan[4] == 0)DrawString(300, 245, L"エピソード3", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 3", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"What Color Is The Sky?[EASY]以上で\nSランクを取ろう", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath S RANK at\nWhat Color Is The Sky?[EASY] or more dif.", Cr);
					e = 0;
				}
				break;
			case 3:
				if (sub1[command[0]] >= 90000) {
					if (lan[4] == 0)DrawString(300, 245, L"エピソード4", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Episode 4", Cr);
					e = 1;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"トリノユメ[NOAMAL]以上で\nAランクを取ろう", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath A RANK at\nTorinoyume[NOAMAL] or more dif.", Cr);
					e = 0;
				}
				break;
			case 4:
				if (sub1[command[0]] >= 98000) {
					DrawString(300, 245, L"appendix-1", Cr);
					e = 1;
				}
				else if (sub1[5] >= 1) {
					if (lan[4] == 0)DrawString(300, 245, L"トリノユメ[HOPENESS]で\nEXランクを取れ", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath EX RANK at\nTorinoyume[HOPENESS].", Cr);
					e = 0;
				}
				else if (sub1[3] >= 90000) {
					if (lan[4] == 0)DrawString(300, 245, L"トリノユx功簾M_^簸]で\nEXランクを取れ", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Reath EX RANK at\nTorim.v?&1/0)\\$cRD].", Cr);
					e = 0;
				}
				else {
					if (lan[4] == 0)DrawString(300, 245, L"刈ョ切簾M_^簸]\ne\\ny×蘭区奪", Cr);
					else if (lan[4] == 1)DrawString(300, 245, L"Rns(}1)Op&rrZ\ne\\ny456Ge/t0Z<=00ax(Q).", Cr);
					e = 0;
				}
				break;
			}
		}
		DrawGraph(0, 0, help, TRUE);
		if (lan[4] == 0)DrawString(5, 460, L"上下左右キー:選択   Enterキー:決定   BackSpaceキー:戻る", Crw);
		else if (lan[4] == 1)DrawString(5, 460, L"←↓↑→key:select   Enter key:enter   BackSpace key:back", Crw);
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//左が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[1]--;
				command[0] = 0;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//右が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[1]++;
				command[0] = 0;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_UP)) {
			//上が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[0]--;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN)) {
			//下が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command[0]++;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RETURN)) {
			//エンターが押された
			if (key == 0 && e == 1) end = story(command[0], command[1]);
			if (end == 5) return 5;
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK)) {
			//バックスペースが押された
			if (key == 0) {
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
		//閉じるボタンが押された
		else if (GetWindowUserCloseFlag(TRUE)) return 5;
		//特定のキーが押されていない
		else key = 0;
		if (command[0] < 0) command[0] = 4;
		if (command[0] > 4) command[0] = 0;
		if (command[1] < 0) command[1] = 3;
		if (command[1] > 3) command[1] = 0;
	}
	return 0;
}

int story(int a, int b) {
	int key = 1, com = 0, Cx = 220, Cy = 75, backimg, noteimg, pageimg, help, sel;
	unsigned int Cr, Crw;
	Cr = GetColor(0, 0, 0);
	Crw = GetColor(255, 255, 255);
	backimg = LoadGraph(L"picture/COLLECT back.png");
	noteimg = LoadGraph(L"picture/Snote.png");
	pageimg = LoadGraph(L"picture/page.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.ogg");
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		DrawGraph(160, 0, noteimg, TRUE);
		//文字は1行20文字、1ページ20行、3ページまで。
		switch (10 * a + 1000 * b + com) {
			//ピッカー
		case 0:
			DrawString(Cx, Cy, L"「市立松坂音楽高等学校前」\n\n　ボクが乗っているバスは、このバス停の\n前で止まった。ボクは席から降りて、同じ\nバス停で降りる学生に紛れて、運賃箱がある\n前方へ向かった。運賃と整理券を握りしめて\nいる右手が、心なしか小さく震えている\nような気がした。その原因は、緊張なのか、\n期待なのか、よくわからなかった。\n　息苦しいバスから降りて、澄んだ空気が\n充満する外へ出た。そしてボクは、学校へ\n向かう学生とは反対方向へ歩を進めた。\nボクは学校に用があるんじゃない。\n目指すは、この学校の裏にある、「ランナー\n事務所松坂支所」だ。", Cr);
			break;
		case 1:
			DrawString(Cx, Cy, L"　この世界では、音を形に変えて売る商売が\n盛んだ。お菓子にしたり、おもちゃに\nしたり、大きいところだと発電にも使って\nいるんだとか。音を形にする技術を考えた\n人は、世界的な表彰を受けたのだとか。いい\nよなー、頭がいい人は。ボクなんかドッヂ\nボールとピアノくらいしか取り柄がないん\nだもんなー。おっと、ボクのことは\n置いといて…。\n　そんな中、やっぱり悪いことを考える\nやつがいるものだ。音を形にする技術を\n使って、犯罪や強盗、治安が悪いところじゃ\nテロにも使われているらしい。本当なら\n警察が動くんだけど、音楽を相手にできる\n警察はなかなかいないらしい。\n　そこで、ランナーの出番だ。ランナーは\n音楽の専門職業。音楽に関するちょっとした\n困りごとや大規模な事件まで、何でも取り\n締まるのがランナーだ。", Cr);
			break;
		case 2:
			DrawString(Cx, Cy, L"　ボクのお父さんも、ランナーの仕事を\nしていた。結構えらい地位に立っていた\nらしい。そりゃ給料もバカみたいに高かった\nらしい。ランナーを辞めても遊んで暮らせる\nくらいの給料だったらしいけど、お父さんは\nランナーを辞めなかった。\n　そんなお父さんが行方不明になったのは、\n大体３年前のことだった。本当に急だった。\n仕事先で通話が切れてしまい、それっきり\nらしい。この時お父さんは、音を使った\n発電所の暴走を止めに行っていたらしい。\n通話が切れた後、すぐに駆け付けた他の\nランナーによると、遺体どころか、身に\n着けていたものすべてが見つからなかった\nらしい。生死は絶望的だったけど、\nもしかしたら生きているのかも？\n　ランナー試験にも合格して、やっと就職する\nことができた。単純にランナーになってみたい\nというのもあるけど、一番の目的はお父さん\n探し。果てしない目標だけど、がんばるぞ。", Cr);
			break;
		case 10:
			DrawString(Cx, Cy, L"「ランナー事務所松坂支所」\n２階建ての建物の扉の横に、こう書いて\nある。間違いない、ここが事務所のようだ。\nえっと、勝手に入っていいのかな…？\nそれとも、どこかにインターホンがあったり\nして…？そう考えていると…、\n「あっ、どうぞお入りくださいー♪」\nとても軽い声が上にあるスピーカーから\n流れた。よく見たらカメラまである。\n見られてた…。恥ずかしー。\n　とりあえず、扉を押して中に入った。\n中は思ったより狭い。そして人もいない。\n「えっと、新人さんですよね？ちょっと\nそのままでお待ちくださいー♪」\nまたさっきの声だ。今度はどこにカメラが\nあるんだろう…？そう思いつつも待つことに\nした。", Cr);
			break;
		case 11:
			DrawString(Cx, Cy, L"　しばらくして、\n「おー！お前が新人かー！思ったより細い体\nしてんなー」\n近くにあった階段から降りてきたこの黄色い\nワニは、ボクを見るなりいきなりこんな\nことを言ってきた。\n「ほっといてください…」\n対するこのワニは、腕も足も体も太くて、\nスポーツ選手でもやっていけそうな体をして\nいる。正直羨ましい。\n「俺はマップゲーター。長いからゲーターで\nいいぞ。お前は？」\n「ピッカーです」\n「ピッカーな、よし覚えた。そうだ、\nLINEとかやってる？」\n「(交換したく)ないです」\n「そうか、それじゃピッチャー、早速この\n事務所を案内するぜ」\n名前覚えれてないじゃん…。頭は良く\nなさそう。", Cr);
			break;
		case 12:
			DrawString(Cx, Cy, L"　ゲーターに連れられて、ボクは事務所の\n２階にやってきた。１階には受付や待合室が\nあったけど、ここにはランナーの待機部屋や\n会議室、さらにトレーニングルームまで\nあった。\n「ランナーは音楽専門ってよく言うけど、\n体力もいるんだぜ。ここでちったぁ体鍛え\nとけよ、ピッチャー」\n「ピッカーです」\nさて、多分これで事務所は全部見て回った\nかな？そう思ってると。\n「じゃ、次は地下だ。ついて来い」\nおっと、ここって地下にも部屋があるんだ。\n思ったより広いんだなー。\n\nあ、待って、そういえば…。\n\nこの事務所に入ってから、ゲーター以外に\n会っていない…。", Cr);
			break;
		case 20:
			DrawString(Cx, Cy, L"　地下にあったのは、地上の雰囲気とは全く\n違う、機械だらけの部屋だった。部屋の壁に\n沿っていろいろなロボットが並んでいて、\n部屋の片隅には５個ほどモニターが並んで\nいた。\n「ピーター、あれを見てみろ」\n「ピッカーです。なんですか？」\nゲーターが指さした先にあったのは、鎧を\n付けた赤いドラゴンのようなロボット\nだった。\n「近づいてよーく見てみろ。いいから」\n…ゲーターがにやにやしながら言った。\n嫌な予感を感じつつも、言われるままその\nロボットに近づいたみた。すると、\n「うわっ！？」\n突然そのロボットが動き出した。閉じていた\n羽を大きく伸ばして、床に付けていた体を\nゆっくり起こして、\n「やぁ、びっくりしたかい？」\nこう言った。とびらの前で聞いた声だった。", Cr);
			break;
		case 21:
			DrawString(Cx, Cy, L"「さてと、改めまして、ランナー事務所に\nようこそ！ボクがここの管理人のテイラー\nだよ。よろしくね♪」\n「よ、よろしくお願いします」\n予想外の出来事にちょっと驚いてしまった。\n「ゲーターに会って早々連絡先とか聞かれ\nたりしたんじゃない？ごめんね、彼は悪気は\nないのよ♪」\n「あ、まぁ、はい」\n「そうだ、私って生き物なのかロボット\nなのかよく聞かれるんだけど、先に答えて\nおくね♪私はその二つの中間。つまり\nサイボーグなの。まぁ、別に気にしなくても\nいいんだけどね♪」\n「へぇ、そうなんですか」\nこのドラゴンはよくしゃべるなぁ。", Cr);
			break;
		case 22:
			DrawString(Cx, Cy, L"「まぁ、これでここの紹介は終わりだな」\nゲーターが言った。\n「ありがとうございます。ところで、ここ\nって他に人はいないんですか？」\n「いないぜ。俺ら二匹だけだ。もともとこの\n近くは治安がいいからな。俺たちの出番は\nほとんどないのさ」\n「それじゃ、仕事もあまりないの？」\n「そうでもないぜ。最近は音が溢れすぎ\nてて、それを集めてまわるのが俺たちの\n仕事だ」\nそういえば、道に音のかけらが落ちてた\nことがあるなぁ。そっか、ランナーが回収\nしてたのか。\n「それじゃ、早速やってみるか？テイラー、\nサポートできるよな？」\n「できるけど、めんどくさい♪」\n「やっぱダメか。じゃ、俺がサポートして\nやるか」\nこうして、早速仕事が始まった。", Cr);
			break;
		case 30:
			DrawString(Cx, Cy, L"　ゲーターが事務所の中にあった車を運転\nして、誰もいない森の中へ向かった。\nゲーターによると、ポイ捨てが頻繁に\n起きてるらしい。\n「あんなところでこそこそ捨てるなんて\nよぉ、どうせ捨てるなら街中で捨てろよ」\nとゲーターが愚痴をこぼす。いや、それは\nそれで問題だと思うけど。\n「ポイ捨てを見つけたら捕まえるの？」\n「当たり前だろぉ？でもポイ捨てって、\n高くても400pop(約3300円)の罰金で済んじ\nまうんだ。無くならねぇんだよなぁ、ポイ\n捨て。まぁ、ボーナス出るからいいけど」\n\n　そして、だいたい30分後。目的地に\n着いた。", Cr);
			break;
		case 31:
			DrawString(Cx, Cy, L"　ポイ捨てが多い、とは聞いたけど、見た\nところそんな雰囲気はなく、むしろきれい\nだった。鳥の鳴き声も聞こえるし、草木も\n元気そうだった。\n「ねぇ、本当にポイ捨てされてるの？」\nボクは聞いた。\n「きれいに見えるだろ？でもされてるんだ\nなぁ、これが。ポイ捨てっつっても見えない\n場所に捨てるからな。まぁ、ランナーには\nバレバレだけどな」\n「…もう見つけたの？」\n「今から見つけるんだよ」\nゲーターがそういって車から取り出した\nのは、紫色のヘッドホンと、円盤が二枚\n付いたDJキット。DJキットの電源を\n入れると、ブゥゥーーン、という音がして、\nキットが宙に浮いた。\n「さて、今から走るぞ。チーター、ついて\n来いよ！」\n「ピッカーです」", Cr);
			break;
		case 32:
			DrawString(Cx, Cy, L"　ゲーターが走り出したと同時に、DJキット\nから音楽が流れてきた。笛の音を中心とした\n曲、この森にぴったりの曲だった。そして、\nこの曲に合わせるかのように、鳥、リス、\n他の動物たち、さらに草木まで、それぞれの\n音を奏で始めた。走りながら聞いていて、\nボクまで歌ってしまいたくなるほどだった。\n全然知らない曲だけど。\n　そんな時だった。この曲を遮るかのように\n雑音がどこからか聞こえてきた。ゲーターは\nいち早くその場所を特定し、そこに\n向かった。そしてそこにあったのは、誰かが\n捨てたであろう、音のかけらだった。自分も\nなんとか音を出そうとしているようだが、\nうまく出せていなくて、それが雑音になって\n聞こえていたのだ。\n　ゲーターは演奏しながら器用に回収して、\nまた次の場所へ向かう。この曲が終わる\nまで、ずっとこれの繰り返しだった。\nこれが、ランナーの仕事…。", Cr);
			break;
		case 40:
			DrawString(Cx, Cy, L"「終わったぜ、変わりないか？」\n「お疲れさん。こっちは問題ないよ♪」\n演奏が終わって、かなりの音のかけらを\n集めたボクたちは、すぐに事務所に帰った。\n集めた音のかけらは、これから業者に売る\nらしい。\n「これだけ集めれたら、ざっと150pop\n(約1250円)くらいになるかな。大漁だね♪」\n「レートは？」\n「上がんないよ♪」\n「あれでもダメかよぉ、厳しいなぁ…」\n「いつになったら私を抜けるかなー♪」\n「見てろよ、すぐに抜いてやるからな」\n話しているゲーターとテイラーをよそに、\nボクはかけらを見ていた。さっきまで騒がし\nかったかけらが、今は静かにたたずんで\n「あ、もしかしてほしい？一個5pop(約40円)\nで売るよ♪」\n「おいおい、それはぼったくりだろｗ」", Cr);
			break;
		case 41:
			DrawString(Cx, Cy, L"「そういえばキッカー、お前目標とかある\nのか？」\nゲーターが聞いてきた。\n「ピッカーです。目標かぁ、とにかく、\nお父さん探しをしたいんだけど、まずどう\nすればいいんだろう？」\n「お父さんもランナーなのか？」\n「それなら、名前とかがあれば探せるよ♪」\nテイラーが言った。\n「それじゃ、私の部屋に来て。一緒に\n探してあげる♪」\n「え、でも、売りに行くんじゃないの？」\n「そんなのゲーターに任せたらいい\nでしょ♪早速行こうか♪」\nボクはテイラーに連れられて地下に行った。", Cr);
			break;
		case 42:
			DrawString(Cx, Cy, L"「えーと、本当にこの人なの？」\n「うん、そうだけど」\nモニターに映っているのは、ボクのお父さん\nの顔写真と、なんかいろいろと文字が並んで\nいた。多すぎて全部を読むのはめんどくさい。\n要するに、いろいろ功績をあげていて、\nかなり上の地位にいますよ的なことが書いて\nある。\n　そして、事故に遭ってから行方不明だという\nことも書いてある。\n「あまり言いたくないけど、亡くなってるかも\nしれないよ。それでも探すのかい？」\n「うん、そのためにランナーになったから」\n探しに行くことに意味がある。少なくとも、\nボクはそう思ってる。\n\n　さぁ、これから頑張るぞ。", Cr);
			break;
			//ゲーター
		case 1000:
			DrawString(Cx, Cy, L"　ランナー事務所の中。俺は自然に目を\n覚ました。ゆっくりと体を起こして、伸びを\nした。ベッドから出て、部屋のカーテンを\n開ける。外はまだ暗いが、すぐに明るくなる\nだろう。空にはちらほらと雲が見える。奥の\n方に見える町は、まだぐっすり眠っている\nようだ。\n　にしても、この時間はマジで静かだ。\nSilentroomのGAMEBOYの前奏くらいに静かだ。\n昼と夕方あたりで盛り上がって、夜に\nなったらまた静かになっていくんだろうな。\nこうやってのんびり考え事ができるのは、\nここが平和だからだろうな。のんきなもん\nだぜ、ここの人たちは。\n\n　日が昇ってきた。良い朝だ。地下で寝てる\nあいつもそろそろ起きるだろう。今日も\n元気に走るぜ。", Cr);
			break;
		case 1001:
			DrawString(Cx, Cy, L"「おはよーさん、ゲーター♪。もしかして\nもう起きてた？」\n１階でテイラーと話している。こいつは半分\n機械だから、時間にほとんど正確だ。正直\n羨ましい。ランナーはタイミングも大事な\n仕事だからな。演奏にブレがないのはかなり\n有利だ。でもまぁ、俺はそこまでして機械に\nなりたくはないけどな。\n「おう、今日は早起きしたぜ。で、今日の\n仕事は何だ？」\n「今日もポイ捨て回収だよ♪。今回は海\nだよ♪海好きでしょ？」\n「まぁ、好きだけど、またゴミ回収かよ…」\n「ゴミって言わないの♪」\nこの町は音楽犯罪がない代わりに、ポイ\n捨てが多い。楽でいいが、せっかくシルバー\nランナーになったんだから、もっと派手な\nことがしたいな。", Cr);
			break;
		case 1002:
			DrawString(Cx, Cy, L"「そうだ、明日ここに新人くんが来るよ♪\nゲーター後輩が欲しいって言ってたよね♪」\n「お、まじか！嬉しいぜ、どんなやつ？」\n新人はマジでありがてぇ、ここは俺と\nテイラー二匹しかいなかったから、派遣の\n仕事ができなかったんだ。事務所に最低\n二匹のランナーがいなければいけないって\nいう決まりがあるからな。\n「とっても耳がいいウサギさんだよ♪」\n「おお、演奏得意そうなやつだな」\n「すぐそこにある音楽学校出身だってさ♪」\n「あそこからかぁ、結構優等生？」\n「そうらしいね♪」\nあそこを卒業したやつは、だいたい都会の\nランナーとか、バンドとか作曲家とかに行く\nもんだ。新人は地元で働きたいっていうこと\nかな？楽しみだ。", Cr);
			break;
		case 1010:
			DrawString(Cx, Cy, L"　次の日、今日は新人が来るらしい。\nめっちゃ楽しみ。でも、今日の仕事もゴミ\nひr…ポイ捨て回収だ。今回は森だ。新人に\n仕事を見せるために、俺は新人が来るまで\n待機だ。早く来ないかな…。新しく話し\n相手ができるのはマジでうれしい。あ、\nいや、テイラーと話すのが飽きたってわけ\nじゃなくて、人は多いほうがいいだろ？\n俺ってば人に教えるのが好きだから。\nテイラーからは教えてもらってばかり\nだから。\n俺はさっきから部屋の窓から事務所の\n入り口を見つめている。多分テイラーも\n部屋のモニターから見つめているんだろう\nな。とか思っていたら、来たようだ。", Cr);
			break;
		case 1011:
			DrawString(Cx, Cy, L"　よし、会いに行こう！早速部屋から出て\n迎え入れようとしたら、\n　ピッコン、ピッコン。\n　電話が鳴った。タイミング悪すぎだろ…。\nしゃーねー、電話優先だ。俺は受話器を\n取った。\n「やっほ♪。起きてる？」\nテイラーだった…。\n「起きてるよ、なんだ？」\n「新人さん来たよ♪」\n…………………………。\n「うん、知ってるって」\n「さっすがダイナー♪。じゃ、３分後に\n１階に来て♪」\nなぜ３分？\n「じゃ、あとはよろしくねー♪」\n意味が分からないまま電話が切れた。\nまぁいいや、３分後に行こう。", Cr);
			break;
		case 1012:
			DrawString(Cx, Cy, L"　３分後。俺は１階に降りた。そこには\nさっき見た新人がいた。改めて見ると、長い\n耳を持った、緑色のウサギだった。思った\nより体が細くて、\n「おー！お前が新人かー！思ったより細い体\nしてんなー」\nついうっかりこんなことを言ってた。\n「ほっといてください…」\nおっと、まずは自己紹介だっけ？\n「俺はマップゲーター。長いからゲーターで\nいいぞ。お前は？」\n「ピッカーです」\n「ピッカーな、よし覚えた。そうだ、\nLINEとかやってる？」\n一応連絡先も聞いておこう。あとでいると\n思うから。\n「…ないです」\nないんかい。じゃー…、次は案内だな。\n「それじゃピッチャー、早速この事務所を\n案内するぜ」", Cr);
			break;
		case 1020:
			DrawString(Cx, Cy, L"　俺はこいつを連れて、２階にやってきた。\n一応、端の部屋から１個ずつ紹介して\nいった。とはいっても、そんなに部屋の数は\n多くないから、すぐに終わった。もともと\n人数が少ないからなぁ、この事務所。今、\n俺らがいるトレーニングルームも、俺が来る\nまではまったく使われてなかったらしい。\n今は俺がバリバリ使ってて、嫌がる\nテイラーを連れてたまに一緒にやってる。\n「ランナーは音楽専門ってよく言うけど、\n体力もいるんだぜ。ここでちったぁ体鍛え\nとけよ、ピッチャー」\n一応ピッチャーにも勧めておく。\n「ピッカーです」\nあ、名前間違えてた。\n「じゃ、次は地下だ。ついて来い」\nそういや、テイラーは今何やってんだ？", Cr);
			break;
		case 1021:
			DrawString(Cx, Cy, L"　とりあえず地下に連れてきた。俺もあまり\n入ったことはないが、相変わらずテイラーの\nコレクションが部屋に並んでる。テイラーは\nいつもモニターの前にいるんだが、今は\nいない。ああ、あいつやる気だな。俺は\nあたりを探す。\n「ピーター、あれを見てみろ」\n俺は、赤いドラゴンのロボットを指さして\n言った。\n「ピッカーです。なんですか？」\n「近づいてよーく見てみろ。いいから」\n俺も前にやられたやつだ。あいつはこれの\n準備をしてたんだな。相変わらず性格\n悪いな。ピッカーがロボットに近づくと、\n「うわっ！？」\n突然そのロボットが動き出した。羽を大きく\n伸ばして、大げさな起動の仕方をして、\n「やぁ、びっくりしたかい？」\nそのロボット(テイラー)こう言った。初見の\n俺はすげーびっくりした。", Cr);
			break;
		case 1022:
			DrawString(Cx, Cy, L"「まぁ、これでここの紹介は終わりだな」\n俺はこう言った。\n「ありがとうございます。ところで、ここ\nって他に人はいないんですか？」\nまぁ、そりゃ気づくよな。\n「いないぜ。俺ら二匹だけだ。もともとこの\n近くは治安がいいからな。俺たちの出番は\nほとんどないのさ」\n「それじゃ、仕事もあまりないの？」\n「そうでもないぜ。最近は音が溢れすぎ\nてて、それを集めてまわるのが俺たちの\n仕事だ」\nまぁ、これが今日の仕事なんだがな。\n「それじゃ、早速やってみるか？テイラー、\nサポートできるよな？」\nテイラーは教えるのがうまいし、手本に\nなるだろうと思ったが…、\n「できるけど、めんどくさい♪」\nやっぱダメか、このめんどくさがりめ。\n「じゃ、俺がサポートしてやるか」", Cr);
			break;
		case 1030:
			DrawString(Cx, Cy, L"「サッカー、車の運転免許は？」\n「ピッカーです。持ってません」\nということで、俺の運転で今日の仕事場に\n向かう。とりあえず、最近のポイ捨ての\n現状について話しといた。\n最近は、音楽犯罪が全く起こらない代わり\nに、ポイ捨てとかその他もろもろの軽犯罪が\n起こってる。あ、一応犯罪なのか。\n「あんなところでこそこそ捨てるなんて\nよぉ、どうせ捨てるなら街中で捨てろよ」\nついつい愚痴を言っちゃった。\n「ポイ捨てを見つけたら捕まえるの？」\n「当たり前だろぉ？でもポイ捨てって、\n高くても400pop(約3300円)の罰金で済んじ\nまうんだ。無くならねぇんだよなぁ、ポイ\n捨て。まぁ、ボーナス出るからいいけど」\nボーナス出るのはまぁ嬉しい。本音だ。\n\nまぁ、目的地には30分くらいで着いた。\nさて、仕事だ。", Cr);
			break;
		case 1031:
			DrawString(Cx, Cy, L"　さて、見た感じはきれいだが、前に来た時\nから２週間は経ってる。そろそろ溜まって\nきてるだろ。\n「ねぇ、本当にポイ捨てされてるの？」\n「きれいに見えるだろ？でもされてるんだ\nなぁ、これが。ポイ捨てっつっても見えない\n場所に捨てるからな。まぁ、ランナーには\nバレバレだけどな」\n「…もう見つけたの？」\n「今から見つけるんだよ」\n俺はそういって、車に積んでた愛用のヘッド\nホンとDJキットを取り出した。俺用に改造\nした特性のキットだ。聞こえはいいし、\n音色もばっちりだ。早速電源を付けた。\n聞きなれた起動音が鳴る。今日も好調のよう\nだな。\n「さて、今から走るぞ。チーター、ついて\n来いよ！」\n「ピッカーです」", Cr);
			break;
		case 1032:
			DrawString(Cx, Cy, L"　曲を流す準備をして、俺は走った。走ると\n同時に音楽がかかる。ここは森だ。フルート\nとサックスが中心の曲を選んだ。猫叉master\nのcaring danceでもよかったが、著作権的に\n危なそうだから別の曲にした。俺が流してる\n曲に合わせて鳥たちが鳴いてくれるのは\nいいが、俺の目的はゴミあつm…じゃない、\nかけら集めだ。さーて、どこだ？\n　っと、早速聞こえたぞ。雑音が。自分も\n曲に合わせて音を出そうとしてるが、うまく\n出せなくて雑音になってるぞ、かけらくん。\n俺は聞こえる場所に走った。そこには思った\nより多いかけらが捨てられてた。誰がか\nめっちゃ捨ててったな。こんにゃろー。俺は\n走りながらかけらを回収していった。さて、\n早く次に行くぞ。この曲は２分半しかない\nんだ。", Cr);
			break;
		case 1040:
			DrawString(Cx, Cy, L"　正直に言うと、後ろからついてくる\nピーターのことをすっかり忘れてた。でも\nちゃんとついてきてたようだ。\n「終わったぜ、変わりないか？」\n「お疲れさん。こっちは問題ないよ♪」\n俺は一通り仕事を終わらして、いつもより\n多いかけらを集めて帰った。\n「これだけ集めれたら、ざっと150pop\n(約1250円)くらいになるかな。大漁だね♪」\n「レートは？」\n「上がんないよ♪」\nあれでもダメかよ。もっとむずい曲にすれば\nよかったか？\n「いつになったら私を抜けるかなー♪」\n「見てろよ、すぐに抜いてやるからな」\nそもそも、テイラーと同じゴールドランナー\nになるの自体むずんだなぁ。はぁ、きつい。", Cr);
			break;
		case 1041:
			DrawString(Cx, Cy, L"「そういえばキッカー、お前目標とかある\nのか？」\nわざわざこういう特殊な仕事に来るもん\nだから、何かあるんだろうなって思った。\n「ピッカーです。目標かぁ、とにかく、\nお父さん探しをしたいんだけど、まずどう\nすればいいんだろう？」\n「お父さんもランナーなのか？」\n「それなら、名前とかがあれば探せるよ♪」\nテイラーが会話に入ってきた。\n「それじゃ、私の部屋に来て。一緒に\n探してあげる♪」\n「え、でも、売りに行くんじゃないの？」\n「そんなのゲーターに任せたらいい\nでしょ♪早速行こうか♪」\nテイラーはそう言って、ピッカーを地下に\n連れ去って行った。\n\n「テイラー、作業したくないだけだろ…」\n俺は小さくつぶやいた。", Cr);
			break;
		case 1042:
			DrawString(Cx, Cy, L"「親父かぁ…」\n俺は、生まれてすぐに親父に捨てられた。\nおかんと一緒に捨てられた。むりやり交尾\nさせて、妊娠したと分かればすぐに捨て\nやがった。旅行先で二人分の荷物を盗んで\n飛行機で帰ったらしい。金もチケットも\nないおかんは困って、警察とかに相談して、\nなんとか帰れたらしいが、家のものも持ち\n去られてたらしい。それからはもう本当に\n大変で、本当に苦労した。\n　俺は、親父がでっかい音楽犯罪をしている\nって聞いて、ランナーになりたいって思った。\n世界中で起こってる犯罪を無くしたいってな。\nでも、俺は音楽が好きだが、演奏の才能が\nないから、７年たった今でもシルバー\nランナー止まりだ。でっかい派遣の仕事が\n来るのはゴールドランナーからだ。\n　…今ここにあるかけらも、捨てられたくて\n作られたわけじゃないんだよな。あぁ、ゴミ\nって言っちゃだめだな。", Cr);
			break;
			//テイラー
		case 2000:
			DrawString(Cx, Cy, L"　朝だ。地下のこの部屋からじゃ分からない\nけど、時間的には日の出の時間だ。まずは、\nパソコンの電源を入れよう。起動するまでの\n間に、部屋の電気を付けて、私の体の点検を\nする。特に大きい仕事はないけど、点検は\nしないとガタがきちゃうから、これだけは\nさぼれない。\n　関節、メモリ、バッテリー。異常なし。\n　メインと予備のバッテリー、充電済み。\n　羽の動作確認。問題なく飛べます。\n　軽い運動。いざというときに動くため。\n　一通り終わって、やっとパソコンが\n起動した。といっても、５個あるモニターの\nうち３個はカメラだけど。メインで使ってる\nモニターを見ると、いつも通りメールが\n届いていた、ほとんどは仕事のメール\nだから、軽く読み飛ばす。\nおっと、一つだけ違うメールが来てた。\n\n『明日、そこの事務所に新人を入れる』", Cr);
			break;
		case 2001:
			DrawString(Cx, Cy, L"「お、まじか！嬉しいぜ、どんなやつ？」\n私は１階にあがって、もう一人のメンバー、\nゲーターに新人がくるってことを教えた。\n新人はイングリッシュロップ(ウサギ)。\nすぐそこの松坂音楽学校を卒業したばかり。\n成績は、音楽と工作が良くて、数学と英語が\nちょっとダメ。在学中にランナー試験に合格\nしてる。あと、ピアノの小さい演奏会に\n何回か出てる。\n「新人かぁ、楽しみだな」\n「ここって私とゲーターの二人だけだった\nからね♪」\n「なんで今まで来なかったんだろうな？」\n「求人は出してたけど、志願者がいなかった\nんだよね♪」\nランナーは、基礎知識を見る試験に合格\nしないとなれないって言うのもあるけど、\nそもそも敷居が高いっていうのもあるのよ\nね。そんなに難しく考えなくていいのに。", Cr);
			break;
		case 2002:
			DrawString(Cx, Cy, L"「そうだテイラー、たまにはランナーとして\n走ってみないか？」\n「事務所の管理があるからヤダ♪」\n「そんなの俺に任せろって」\n「まだ最後に走ってからたったの３日しか\n経ってないよ♪」\n正直に言うと、走るのがめんどくさい。\n走った後はメンテが大変だし、簡単な仕事の\nためにわざわざ体を動かしたくない。\n「いざっていうときに動けないぞ」\n「私はサイボーグだもん♪だいじょーぶ♪」\nサイボーグっていうのは、体の一部を機械に\n変える技術のこと。私の場合は半分以上が\n機械だから、もうロボットって言われても\nおかしくないし、私も特に気にしてない。\nなんでロボットになったかっていうと、\n音楽事故で瀕死だったところを博士に助けて\nもらったからなんだ。でも、みんなには\n自分が好きでこうしてもらったって言って\nる。あまり知られたくないからね。", Cr);
			break;
		case 2010:
			DrawString(Cx, Cy, L"　次の日、新人くんが来る日だ。ここに\n新人を迎えるのは久しぶりだな。４年前に\nゲーターがここに異動して以来だ。新しい\nランナー仲間が増えるのはうれしいし\n助かる。私の代わりに働いてくれる人が\n増えるからね♪。\nそれに、派遣の仕事ができるようになる\nのもうれしいな。事務所には最低二人は\nいないとだめで、元々二人しかいなかった\nここは派遣の仕事が絶対に来なかったから、\nゴールドランナーの私は退屈で仕方なかった\nんだ。\n\n　とか考えてたら、見えてきた。モニター\n越しだけど、はっきりと見える。思った\nよりは背は低くて、かわいらしいイング\nリッシュロップ(ウサギ)だ。", Cr);
			break;
		case 2011:
			DrawString(Cx, Cy, L"　新人くんが事務所の扉の前でおどおど\nしてるのがモニター越しに見えた。\nかわいい。私はマイクを手に取って、\n「あっ、どうぞお入りくださいー♪」\n新人くんにこう言った。そのあとは、\nカメラに気づいたみたいで、恥ずかしそうな\n顔をしてた。新人くんが入った後は、\n「えっと、新人さんですよね？ちょっと\nそのままでお待ちくださいー♪」\nそう言って、新人くんを待たせた。あとは、\n今頃２階でグースカ寝てるだろうダイナーを\n起こして、あとはダイナーに任せよう。\n　ダイナーの希望で、ダイナーの部屋に\nカメラは置いてない。めんどくさいときは\n電話でやり取りするのが習慣だ。私は\n受話器を取って、ダイナーに電話をかける。", Cr);
			break;
		case 2012:
			DrawString(Cx, Cy, L"　思ったよりすぐに電話がつながった。\n「やっほ♪。起きてる？」\n「起きてるよ、なんだ？」\nあら、ダイナーが私より早起きするなんて\n珍しいなぁ。\n「新人さん来たよ♪」\n「うん、知ってるって」\nそっか、ダイナーも新人くんが来るのを\n楽しみにしてたんだね。\n「さっすがダイナー♪。じゃ、３分後に\n１階に来て♪。あとはよろしくねー♪」\n３分の間を開けるのは、新人くんを落ち\n着かせるため。ちょっと緊張してるみたい\nだったからね。あとはダイナーがやって\nくれるでしょ。\n　あ、そうだ。せっかくだし、私が\nダイナーにやったサプライズを新人くんにも\nやってあげよう。えっと、名前は確か\nピッカーだったよね。よし、大丈夫だ。", Cr);
			break;
		case 2020:
			DrawString(Cx, Cy, L"　まぁ、これはある種の初見殺しだよね♪\n私は今、部屋のコレクションの中に隠れて\nいる。やっぱり、ロボットが隠れるには\nロボットの中だよね。近くを通ったところで\n動き出して、ビックリさせる作戦だ。多分、\n今頃ピッカーはゲーターと一緒に事務所を\n見て回ってるんだろうね。…ゲーター、\nピッカーに失礼なこと言ってないかな？\nいきなり連絡先とか聞いてるかもなー。\nあとで私が代わりに謝っとこ。\n\n　しばらくして、階段から二匹分の足音が\n聞こえてきた。来たね。まずはゲーター、\n次にピッカーが出てきた。へぇ、ピッカーて\n思ったより小さいんだね。", Cr);
			break;
		case 2021:
			DrawString(Cx, Cy, L"　ピッカーがきょろきょろとあたりを\n見渡している。まぁ、珍しいものばかり\nだし、当然かな。あ、モニターつけっぱなし\nだった。別にいいや。デスクトップとカメラ\nしか映ってないし、勝手にいじることも\nしないでしょ。ゲーターじゃあるまいし。\n「ピーター、あれを見てみろ」\n「ピッカーです。なんですか？」\n名前間違ってるよ、ゲーター。ゲーターは\n私を指さして言った。\n「近づいてよーく見てみろ。いいから」\nフォローしてくれるのはありがたいん\nだけど、イタズラ心が顔に出てる。\nピッカーが私にある程度近づいたところで、\n「うわっ！？」\n私は動き出した。止まってたものが突然動き\nだすんだから、そりゃビックリするよね。\nでも私はこれだけじゃ物足りなくて、羽を\n大きく広げてみたり、大げさな登場をして\nみた。", Cr);
			break;
		case 2022:
			DrawString(Cx, Cy, L"「さてと、改めまして、ランナー事務所に\nようこそ！ボクがここの管理人のテイラー\nだよ。よろしくね♪」\n「よ、よろしくお願いします」\n「ゲーターに会って早々連絡先とか聞かれ\nたりしたんじゃない？ごめんね、彼は悪気は\nないのよ♪」\n「あ、まぁ、はい」\n私はこの後も、おしゃべりがしたくて話し\n続けてた。そもそもこの事務所から出てない\nから、ゲーター以外の人と話すのって久し\nぶりで、嬉しかったんだ。ついでに、私が\nサイボーグだってこともピッカーに話して\nおいた。\n「テイラー、もういいんじゃないか？」\n私のほぼ一方的な会話は、ゲーターのこの\n言葉で強制終了となった。私の体内時計\nだと、約３分くらいだった。", Cr);
			break;
		case 2030:
			DrawString(Cx, Cy, L"　ピッカーの事務所案内もこれで終わった\nみたいで、次はさっそく仕事の話になって\nいた。\n「テイラー、サポートできるよな？」\nゲーターにこう聞かれたけど、\n「できるけど、めんどくさい♪」\n断った♪\n「じゃ、俺がサポートしてやるか。\nピッカー、ついて来い」\nそういって、ゲーターとピッカーは仕事に\n向かっていった。\nさて、部屋には私しかいない。部屋に\nカギもかけた。カギは念のため、３時間後に\n自動で外されるけど、それでも十分だ。私は\n私だけの秘密の作業に入る。", Cr);
			break;
		case 2031:
			DrawString(Cx, Cy, L"　私はゴールドランナー。ランナーの中でも\nトップクラスの実力を持つランナーだ。\nランナーには４種類いて、ランナーになった\nばかりのビギナーランナー。仕事ができる\nようになるブロンズランナーが約95％。\n難しい仕事も来るようになるシルバー\nランナーが約25％。世界中の大きな仕事を\n任せられるゴールドランナーが約１％だ。\n他にもあるらしいけど、私が知っているのは\nこれだけ。\n私が見ている画面は、ゴールドランナーの\n中の情報サイト。ほかの人に見られては\nいけない情報まである。カギをかけている\nのはこれのため。ゲーターには、部屋を\n閉めているときは寝ているって言っている。\n起こしたらハグルマミキサーの刑って\n言ってる(冗談)♪。", Cr);
			break;
		case 2032:
			DrawString(Cx, Cy, L"　普段から情報だけは集めてた私。ピッカー\nが入ったから、私にも派遣の仕事が来るん\nだろうな。簡単な仕事は全くやる気がないん\nだけど、難しい仕事となると、なんだか\nワクワクする。これがドラゴンの本能なの\nかな？いや、よくわかんないけど。\n　私は特にランナーになりたかったわけじゃ\nなくて、街で音ゲーやってたらスカウト\nされたから入っただけ。普段家から出ずに、\nおうち音ゲーばっかりやってた私をどう\nやって見つけたのかは不明。ランナー試験は\nスカウトされてから受けた。それから12年が\n経って、今まではゴールドランナーとして\nというより、ネットで情報提供やアドバイス\nをしてきた。派遣の仕事がないから、世界\n中で何が起こっているのかは、写真と文字で\nしか知ることができなかった。\n　…やっぱり、嬉しいんだろうな、私。まだ\n仕事がくるって決まったわけじゃないけど、\n来たら頑張ろ♪", Cr);
			break;
		case 2040:
			DrawString(Cx, Cy, L"「終わったぜ、変わりないか？」\n「お疲れさん。こっちは問題ないよ♪」\nゲーターとピッカーが仕事を終えて帰って\nきた。今回の仕事は森に捨てられたかけら\n集め、別名、ポイ捨て回収。平和なこの地域\nじゃ、ランナーの仕事はほとんどこれ。\n「これだけ集めれたら、ざっと150pop\n(約1250円)くらいになるかな。大漁だね♪」\n「レートは？」\n「上がんないよ♪」\nレートっていうのは、ランナーの腕前を示す\n数値。ランナーの基準は全部これで決まって\nいる。私はゴールドの中でも上のほう。\n「いつになったら私を抜けるかなー♪」\n「見てろよ、すぐに抜いてやるからな」\nゲーターは、シルバーランナーになるのに\n５年もかかってる。成長が遅いけど、音楽\n好きだし努力家だから、あと８年したら\nゴールドになれるんじゃないかな。頑張れ♪", Cr);
			break;
		case 2041:
			DrawString(Cx, Cy, L"「そういえばキッカー、お前目標とかある\nのか？」\n名前間違えてるよ。\n「ピッカーです。目標かぁ、とにかく、\nお父さん探しをしたいんだけど、まずどう\nすればいいんだろう？」\n「お父さんもランナーなのか？」\nお、これはチャンスだ♪\n「それなら、名前とかがあれば探せるよ♪。\n私の部屋に来て。一緒に探してあげる♪」\n「え、でも、売りに行くんじゃないの？」\n「そんなのゲーターに任せたらいい\nでしょ♪早速行こうか♪」\n情報探しを口実に、めんどくさいかけらの\n集計をゲーターに投げつける。というか、\nピッカーとお話がしたい。ゲーターが何が\n愚痴った気がするけど、私には聞こえ\nなーい♪", Cr);
			break;
		case 2042:
			DrawString(Cx, Cy, L"「えーと、本当にこの人なの？」\nピッカーのお父さんの名前と、出身地とか\nその他もろもろを入れて、出てきた検索\n結果は、とんでもないものだった。\n『ブrikppぽエ』っていう、名前の発音が\nすごく難しい国にある音響発電所で起きた\n大規模爆発。彼はその当事者だったのだ。\n生きているかもわからない絶望的な状況。\n多分、ピッカーも知ってるんだろうけど、\n一応聞いた。\n「あまり言いたくないけど、亡くなってる\nかもしれないよ。それでも探すのかい？」\n「うん、そのためにランナーになったから」\n「分かった。私もできるだけ協力するよ」\n以前まではネットでしか知れなかった情報。\n派遣という手段も増えた今、私にしかでき\nないこともあるだろう。ついでに、って\n言うと悪いかもしれないけど、情報くらいは\n集めてこようかな。", Cr);
			break;
			//nature prhysm コラボストーリー(ただしnature prhysm作者(Take_Maさん)は関わっていません)
		case 3000:
			DrawString(Cx, Cy, L"「お前がテイラーか?\n見るからにメカメカしいな」\n「はい、少しの間ですが、\nよろしくお願いします♪」\nまさか派遣先でもゲーターみたいな人に\n会うとは思わなかったな。\n　私は今、ハーリッツという場所にある、\n自然管理センターのハーリッツ支所にいる。\n今私と話しているゴリラのボリスさんは、\nここで一人で働いているのだそうだ。\n「人手が足りないとのことで派遣された\nのですが、やはり一人だと大変ですか?」\n「勘違いするなよ。上司からの命令で\n呼んだんだ。俺はまだまだやれるさ」\nまぁ、まだ若そうなムキムキのゴリラさん\nだし、わざわざ私の助けはいらないかな。\n…あれ、それじゃどうして私は\n呼ばれたんだろう?", Cr);
			break;
		case 3001:
			DrawString(Cx, Cy, L"　あたりを見渡すと、額縁に入った一枚の\n写真が壁にかかっているのが見えた。\n「あれは、センターの社長の自画像さ」\nボリスさんが教えてくれた。青い髪を\n持った人間の顔が写ってた、けど、\n「画素数低くないですか?\nまるでドット絵のようですよ♪」\n「ドット絵なんだよ(怒)」\nおっと、怒らせちゃったかな…? 私って\n人を怒らせやすいからね、気をつけなきゃ。\n「社長もランナーなんですか?」\n「俺達はランナーとは違う。様々な異常\n気象を予測し、報道、必要なら調査を行う\nのが仕事だ。ランナーみたいに隙あらば\n突入しに行くような仕事じゃないんだ」\nランナーの仕事については75%合ってる。\nでもそれだけじゃないんだぞ。\nってことを言うと怒らせそうだから\n言わないでおいた。", Cr);
			break;
		case 3002:
			DrawString(Cx, Cy, L"「ところで、私は何をすれば良いですか?」\n「そうだな…、これからセンターの裏にある\n気象レーダーのメンテナンスをしに\n行くんだが、一緒に来るか?」\n「あ、是非行きます♪」\n「いや、待て、そういえば、あと3日で\nまとめて出さないといかん気象観測\nデータが残ってるんだった。\nこっちにするか」\n「あ、大丈夫ですっ。私機械いじりは\n得意なんですっ♪」\n「え、でも、ムズいぞ?\n結構精密な機械だぞ?」\n「私だって精密な機械ですからっ!\n早くメンテ行きましょっ♪」\nデータ整理なんてたまったものじゃない。\n機械いじりしてる方が92.4倍マシよ。", Cr);
			break;
		case 3010:
			DrawString(Cx, Cy, L"　翌日、あまり仕事がないらしく、結局\nデータ整理をやらされることになった。\nっていうかどうしてデジタル情報を\nわざわざ紙に書いて郵便で送らないと\nいけないの(怒)? せめてプリンターくらい\n設置しなさいよ。\n　私の頭の中ではもう計算は終わって\nいて、シャーペンを持っている手を動かす\n以外にすることがなくて退屈。もし今私が\n生身のドラゴンのままだったらとっくに\n寝てるわよ。\n　そうだ、派遣から帰った後のことを\n考えよう。とりあえず、帰ったら一曲作って\nみようかな。\"Digital of Analog\"って曲で、\nアナログの環境に拉致られたデジタルくんを\n想像してみよう。最初からBPMは215\nあたりで飛ばしていこう。アナログ(悪役)と\n戦うデジタルくんを想像するんだ。\n「お前、歌ってないでさっさとやれ」\n気がついたら歌ってた♪", Cr);
			break;
		case 3011:
			DrawString(Cx, Cy, L"「多いっ! こんなに書く必要あるんですか?」\n「ある。今後の気象予測には欠かせん」\n「とはいえ1週間分のデータを30分毎に\n手書きするなんて酷ですよー。\n推定8.2時間かかりますよ」\n「いや、そんなに早く終わらないだろ。\n計算もあるんだぞ」\n「私の機械の頭にかかれば計算なんて\n一瞬なのである♪」\n「すごいなお前」\n　自分からすすんで機械になったわけじゃ\nないけど、こういう時はよく助けて\nもらってる。バッテリーさえあれば\nちゃんと動くし♪\n「俺はちょっと食い物買いに行って来る。\nデータ整理頼むぞ」\n「はーい♪」\nボリスさんがセンターを出ていった。\n多分45分は帰ってこないかな。\nよし、ちょっとサボろ♪", Cr);
			break;
		case 3012:
			DrawString(Cx, Cy, L"\n　　ピガシャン!!\n\n　私は驚いて窓の外の景色を見る。\n大雨。しかも雷も一緒。35.6分前まで晴れて\nいたのに、こんなことがあるの!?\n　私の体がブルッと震える。大粒の雨が\n地面を突く音と、雨雲の中で電気が流れる\n音、そして雷がハーリッツの街を襲う音。\nこれらに混ざって、人々が騒ぎながら街を\n掛ける音がかすかに聞こえる。\n　私はこんな時でも、これらの音を\"曲\"\nとして聞いてしまう性癖がある。なんて\n迫力がある曲なんだ。私もこの曲の一員に\nなりたい。\n\n　気がついたら、私はセンターから\n飛び出していた。", Cr);
			break;
		case 3020:
			DrawString(Cx, Cy, L"　雨がピアノを叩き、人の声がバイオリンを\n弾き、時折雷がシンバルを打つ曲が流れる街。\n私は私の音を鳴らしながら走っていると、\nボリスさんに会った。\n「何やってるんだよお前…」\n「ごめんなさい、ついうっかり\n一曲奏でてました♪」\n「さすがランナーだな…」\nボリスさんの手には、何かが入っている\nビニール袋があった。多分お弁当かな?\n「でも、急に降り始めましたね。\nびっくりしましたよ」私が言った。\n「俺もだ、こんなの予測すら\nできてなかったぞ」\n再び雷がシンバルを打った。\n「テイラー、演奏の準備はできてるよな?」\n「あ、はい、バッチリ♪」\n「一緒に演奏するぞ。今回ばかりは\n俺一人じゃ厳しい」", Cr);
			break;
		case 3021:
			DrawString(Cx, Cy, L"　こうして私達の協奏が始まった。私は\nいろんな楽器の音を口から出して演奏した。\nボリスさんは、ランナーとは違う方法で演奏\nしていた。よく聞くと、3種類の音しか\n出していないことに気づいた。必要な音\nだけを鳴らして演奏する方法は、私が\n知ってる中だとクラシックに近いものだった。\n(最近、クラシックは古いとかショボいとか\n言われてるけど、クラシックなめないでよ♪\nクラシックは基礎がしっかり構築されてる曲\nなんだからね♪)\n「無駄がないすっきりした音色ですね♪」\nボリスさんはちょっと驚いた様子でこっちを\n見た。\n「お ? これの良さがわかるのか?」\n「はい、私は音と友達なので♪」\nボリスさんが少し笑ったような気がした。\n「よし、調査を続けるぞ。この気象の\nパターンを見つけるんだ」", Cr);
			break;
		case 3022:
			DrawString(Cx, Cy, L"　私たちは雨の中それぞれの方法で協奏\nしていた。私は大きい雷の音に負けない\nように、力強いエレキギターの音を出した。\n(シンセの音がどうしても混ざっちゃうけど)\nもはや私が雷に合わせるんじゃなくて、雷が\n私に合わせるんだ。\n　ほかの人はとっくに建物の中に\n入っちゃった。雨の中演奏しながら外を\n走ってる人って、よく考えたら変だよねw\n近所迷惑で怒られたら謝っとこ♪\n\n　体内時計じゃ1.24時間。私たちはずっと\n協奏してた。振っていた雨はだんだん弱く\nなって、雷はもう止んでいた。私たちは\nすっかりびしょ濡れになっていて、ボリス\nさんは一つくしゃみをした。私は体内に\nヒーターがあるから平気♪\n「一回センターに戻るぞ…」\nボリスさんが言った。\n「はい、お疲れ様です♪」", Cr);
			break;
		case 3030:
			DrawString(Cx, Cy, L"「えっと、それ本当ですか…?」\nあの楽しかった演奏会を終えた後、ボリス\nさんはまずシャワーを浴びに行った。私は\n今、私の体のメンテナンスをするために、\n身体のパーツを分解して、細かく見て\n回ってる。そんなにガタはないけど、一応\n全部見ておく。\n　そしてボリスさんは今、受話器の前で\nあんなことを言っていた。\n「ちょっと、それこっちじゃ予測できな\nかったんですが…、どういうことですか?」\nあのボリスさんが丁寧な言い方をしている。\nお偉いさんかな?\n「ああ、データならちゃんと取れましたよ。\nテイラーもよく働いてくれました」\n突然名前を呼ばれてびっくりした。私は\n結構好き勝手にやってたんだけどな♪", Cr);
			break;
		case 3031:
			DrawString(Cx, Cy, L"　ボリスさんの通話が終わったようで、私の\n方へ歩いてきた。私を見ると、少し驚いた\n様子だった。\n「すごいな、本当にロボットなんだな」\n「ああ、ちょうどメンテナンスしてたところ\nなんです♪」\nボリスさんはメンテ中の私を興味深そうに\n見ていた。\n「それで、何のお話だったんですか?」\n「え? ああ、あれか。さっきの大嵐だが、\n他のセンターでは予測できていたらしいんだ」\n「あら? それじゃ、ここのが壊れてたの\nでしょうか? 」\n「いや、ここのはデータが古いって言われた」\n「冗談でしょ!?頻繁に更新して\nなかったの!?」\nつい普段の口調になってしまった。\n「アナログなんだようちは…」\n\n　最悪。", Cr);
			break;
		case 3032:
			DrawString(Cx, Cy, L"「まぁ、俺がめんどくさがって、やって\nなかっただけだからな。叱られちまった」\nボリスさんが言った。ちょっといじめて\n差し上げようかな。\n「めんどくさがりの気持ちわかりますよ〜、\n私がそうですから。でも、\nめんどくさいことには案外大切な\nものもあるんですよ〜♪」\n「データ整理とかな」\n「あっ…」\nそういえばまだ仕事残ってたんだった。\n「あーもうアナログなんか大っ嫌いっ!」\n「そう言うな、俺も手伝うから」\n「せめてプリンターくらい置いてください…」\n「まぁ、そのうちな」", Cr);
			break;
		case 3040:
			DrawString(Cx, Cy, L"\n\n\n　　　　　　　　　　　　　3018年3月13日\n　　　　　　　オリヴァー・ハンビューヘン\n\n　　　　　音楽災害調査報告書\n\n1.発生日時: 3018年3月10日　AM0:50ごろ\n\n2.発生場所: Irreversible地方", Cr);
			break;
		case 3041:
			DrawString(Cx, Cy, L"\n3.災害状況:\n　特に何の前触れもないにも関わらず、\n時空間の歪みが各地に発生。あるところでは\n昼であり、他のある所では夜であった。また、\nある所では1秒が1分になっており、他のある\n所では1分が1秒になっていた。空の様子は\n滅茶苦茶であり、もはやこの場所には空と\nいうものは存在していないかのように思えた。\n　この場所が奏でていた音楽は、BPMは200と、\n音楽災害にしては遅めのペースに見えたが、\n32分の音が当たり前のように鳴っていたため、\n事実上は400であった。", Cr);
			break;
		case 3042:
			DrawString(Cx, Cy, L"\n4.処理状況:\n　現在、この地域は特別指定区域として、\n一般人及びゴールド以下のランナーの立ち\n入りを禁止している。時空間の歪みは日が\n経つとともに収まっているが、不測の事態に\n備えて、私、ハンビューヘンと、助手の\nマヒューズを配属している。常に監視し、\n状況をレポートしていく。\n\n　<追記>\n　現在は音楽社会であり、音楽製造物の扱い\nには十分気をつけなければならない。一人\nひとりがその自覚を持ち、安全に、快適に\n音楽を扱う必要がある。", Cr);
			break;
		}
		DrawGraph(0, 405, pageimg, TRUE);
		DrawFormatString(15, 420, Cr, L"%d / 3", com + 1);
		DrawGraph(0, 0, help, TRUE);
		DrawString(5, 460, L"上下キー:ページ選択   BackSpaceキー:戻る", Crw);
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_UP)) {
			//左が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				com--;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN)) {
			//右が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				com++;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK)) {
			//バックスペースが押された
			if (key == 0) {
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
		//閉じるボタンが押された
		else if (GetWindowUserCloseFlag(TRUE)) return 5;
		//特定のキーが押されていない
		else key = 0;
		com = mins(maxs(com, 2), 0);
	}
	return 0;
}

int option(void) {
	//ロード
	//data = [キャラ,オフセット,SE,背景明るさ,言語]
	int e, sel;
	int	data[5] = { 0,0,0,2,0 };
	FILE *fp;
	e = _wfopen_s(&fp, L"save/system.dat", L"rb");
	if (e == 0) {
		fread(&data, sizeof(data), 5, fp);
		fclose(fp);
	}
	//処理
	int key;
	int command = 0;
	unsigned int Cr;
	int backimg, cimg, help;
	backimg = LoadGraph(L"picture/OPTION back.png");
	cimg = LoadGraph(L"picture/OC.png");
	help = LoadGraph(L"picture/help.png");
	sel = LoadSoundMem(L"sound/select.ogg");
	Cr = GetColor(255, 255, 255);
	while (1) {
		ClearDrawScreen();
		DrawGraph(0, 0, backimg, TRUE);
		DrawGraph(40, 45 + command * 50, cimg, TRUE);
		switch (data[0]) {
		case 0:
			if (data[4] == 0)DrawString(100, 50, L"キャラクター: ピッカー", Cr);
			else if (data[4] == 1)DrawString(100, 50, L"Character: Picker", Cr);
			break;
		case 1:
			if (data[4] == 0)DrawString(100, 50, L"キャラクター: マップゲーター", Cr);
			else if (data[4] == 1)DrawString(100, 50, L"Character: MapGator", Cr);
			break;
		case 2:
			if (data[4] == 0)DrawString(100, 50, L"キャラクター: テイラー", Cr);
			else if (data[4] == 1)DrawString(100, 50, L"Character: Taylor", Cr);
			break;
		}
		if (data[4] == 0)DrawFormatString(100, 100, Cr, L"オフセット: %dms", data[1] * 5);
		else if (data[4] == 1)DrawFormatString(100, 100, Cr, L"Offset: %dms", data[1] * 5);
		if (data[2] == 0 && data[4] == 0) DrawString(100, 150, L"効果音: あり", Cr);
		else if (data[2] == 0 && data[4] == 1) DrawString(100, 150, L"SE: ON", Cr);
		else if (data[2] == 1 && data[4] == 0)DrawString(100, 150, L"効果音: なし", Cr);
		else if (data[2] == 1 && data[4] == 1)DrawString(100, 150, L"SE: OFF", Cr);
		switch (data[3]) {
		case 0:
			if (data[4] == 0)DrawString(100, 200, L"背景の明るさ: 真っ黒", Cr);
			else if (data[4] == 1)DrawString(100, 200, L"BackGroundBrightness: Black", Cr);
			break;
		case 1:
			if (data[4] == 0)DrawString(100, 200, L"背景の明るさ: 暗い", Cr);
			else if (data[4] == 1)DrawString(100, 200, L"BackGroundBrightness: Dark", Cr);
			break;
		case 2:
			if (data[4] == 0)DrawString(100, 200, L"背景の明るさ: 中間", Cr);
			else if (data[4] == 1)DrawString(100, 200, L"BackGroundBrightness: Middle", Cr);
			break;
		case 3:
			if (data[4] == 0)DrawString(100, 200, L"背景の明るさ: 明るい", Cr);
			else if (data[4] == 1)DrawString(100, 200, L"BackGroundBrightness: Bright", Cr);
			break;
		}
		switch (data[4]) {
		case 0:
			DrawString(100, 250, L"言語 Language: 日本語", Cr);
			break;
		case 1:
			DrawString(100, 250, L"言語 Language: English", Cr);
			break;
		}
		switch (command) {
		case 0:
			if (data[4] == 0)DrawString(20, 420, L"使用するキャラクターを変えます。", Cr);
			else if (data[4] == 1)DrawString(20, 420, L"Choose the character you use.", Cr);
			break;
		case 1:
			if (data[4] == 0)DrawString(20, 420, L"音符が流れてくるタイミングを変えます。\n増やすと遅れて、減らすと早めに流れます。", Cr);
			else if (data[4] == 1)DrawString(20, 420, L"Change the timing of note. Increase to late, Decrease to early.", Cr);
			break;
		case 2:
			if (data[4] == 0)DrawString(20, 420, L"音符をたたいた時の効果音を鳴らすかどうかを変えます", Cr);
			else if (data[4] == 1)DrawString(20, 420, L"Choose whether to play the sound of hitting the note", Cr);
			break;
		case 3:
			if (data[4] == 0)DrawString(20, 420, L"背景の明るさを変えます。", Cr);
			else if (data[4] == 1)DrawString(20, 420, L"Choose brightness of background.", Cr);
			break;
		case 4:
			DrawString(20, 420, L"ゲームで使う言語を変えます。\nChoose the lunguage in this game.", Cr);
			break;
		}
		DrawGraph(0, 0, help, TRUE);
		if (data[4] == 0)DrawString(5, 460, L"上下左右キー:選択   BackSpaceキー:保存して戻る", Cr);
		else if (data[4] == 1)DrawString(5, 460, L"←↓↑→ key:choose   BackSpace key:save the option and return", Cr);
		ScreenFlip();
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			//左が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				data[command]--;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_RIGHT)) {
			//右が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				data[command]++;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_UP)) {
			//上が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command--;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_DOWN)) {
			//下が押された
			if (key == 0) {
				PlaySoundMem(sel, DX_PLAYTYPE_NORMAL);
				command++;
			}
			key = 1;
		}
		else if (CheckHitKey(KEY_INPUT_BACK) == 1) {
			//バックスペースが押された
			if (key == 0) {
				e = _wfopen_s(&fp, L"save/system.dat", L"wb");
				fwrite(&data, sizeof(int), 5, fp);
				fclose(fp);
				ClearDrawScreen();
				break;
			}
			key = 1;
		}
		else if (GetWindowUserCloseFlag(TRUE)) {
			//閉じるボタンが押された
			return 5;
		}
		else {
			//特定のキーが押されていない
			key = 0;
		}
		if (command < 0) {
			command = 4;
		}
		if (command > 4) {
			command = 0;
		}
		if (data[0] == -1) {
			data[0] = 2;
		}
		if (data[0] == 3) {
			data[0] = 0;
		}
		if (data[1] < -10000) {
			data[1] = -10000;
		}
		if (data[1] > 10000) {
			data[1] = 10000;
		}
		if (data[2] == -1) {
			data[2] = 1;
		}
		if (data[2] == 2) {
			data[2] = 0;
		}
		if (data[3] == -1) {
			data[3] = 3;
		}
		if (data[3] == 4) {
			data[3] = 0;
		}
		if (data[4] == -1) {
			data[4] = 1;
		}
		if (data[4] == 2) {
			data[4] = 0;
		}
	}
	return 1;
}