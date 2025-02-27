
#include <stdio.h>
#include "system.h"
#include "RecScoreFile.h"

#define REC_LINE_READ(pointer, size, count, fp)  fread((pointer), (size), (count), (fp))
#define REC_LINE_WRITE(pointer, size, count, fp) fwrite((pointer), (size), (count), (fp))
#define REC_LINE_SEEK(pointer, size, count, fp)  fseek((fp), ((size) * (count)), SEEK_CUR)

/**
 * rrsファイルからallnumを読み込む
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
 * rrsファイルから全ての情報を読み込む
 * @return 0: OK, -1: error
 */
int rec_score_fread(rec_score_file_t *recfp, const TCHAR *path) {
	FILE *fp;

	_wfopen_s(&fp, path, L"rb");

	if (recfp == NULL || fp == NULL) { return -1; }

	fread(&recfp->allnum, sizeof(playnum_box), 1, fp);//各データの個数
	fread(&recfp->nameset.mp3FN, 255, 1, fp);//音楽ファイル名
	fread(&recfp->mapdata.bpm, sizeof(double), 1, fp);//BPM
	fread(&recfp->time.offset, sizeof(int), 1, fp);//offset
	fread(&recfp->nameset.sky, 255, 1, fp);//空背景名
	fread(&recfp->nameset.ground, 255, 1, fp);//地面画像名
	fread(&recfp->nameset.water, 255, 1, fp);//水中画像名
	fread(&recfp->nameset.songN, 255, 1, fp);//曲名
	fread(&recfp->nameset.songNE, 255, 1, fp);//曲名(英語)
	fread(&recfp->mapdata.Lv, sizeof(short int), 1, fp);//レベル
	//fread(&item, sizeof(int), 99, fp);//アイテム画像データ(動作未確認)
	{
		int buf[99][2];
		fread(buf, sizeof(int), 198, fp);//落ち物背景切り替えタイミング
		for (int i = 0; i < 99; i++) {
			recfp->mapeff.fall.d[i].time = buf[i][0];
			recfp->mapeff.fall.d[i].No   = buf[i][1];
		}
	}
	{
		double buf[5][99][2];
		fread(&buf, sizeof(double), 990, fp);//レーン速度
		for (int iLane = 0; iLane < 5; iLane++) {
			for (int inum = 0; inum < 99; inum++) {
				recfp->mapeff.speedt[iLane].d[inum].time  = buf[iLane][inum][0];
				recfp->mapeff.speedt[iLane].d[inum].speed = buf[iLane][inum][1];
			}
		}
	}
	{
		int buf[3][99][2];
		fread(buf, sizeof(int), 594, fp);//キャラグラ変換タイミング
		for (int i = 0; i < 99; i++) {
			recfp->mapeff.chamo[0].gra[i]  = buf[0][i][0];
			recfp->mapeff.chamo[0].time[i] = buf[0][i][1];
			recfp->mapeff.chamo[1].gra[i]  = buf[1][i][0];
			recfp->mapeff.chamo[1].time[i] = buf[1][i][1];
			recfp->mapeff.chamo[2].gra[i]  = buf[2][i][0];
			recfp->mapeff.chamo[2].time[i] = buf[2][i][1];
		}
	}
	fread(&recfp->mapeff.move.y[0].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[0], fp);//上レーン縦位置移動タイミング
	fread(&recfp->mapeff.move.y[1].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[1], fp);//上レーン縦位置移動タイミング
	fread(&recfp->mapeff.move.y[2].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[2], fp);//上レーン縦位置移動タイミング
	fread(&recfp->mapeff.move.y[3].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[3], fp);//上レーン縦位置移動タイミング
	fread(&recfp->mapeff.move.y[4].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[4], fp);//上レーン縦位置移動タイミング
	fread(&recfp->mapeff.move.x[0].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[0], fp);//上レーン縦位置移動タイミング
	fread(&recfp->mapeff.move.x[1].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[1], fp);//上レーン縦位置移動タイミング
	fread(&recfp->mapeff.move.x[2].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[2], fp);//上レーン縦位置移動タイミング
	recfp->mapeff.move.y[0].num = 0;
	recfp->mapeff.move.y[1].num = 0;
	recfp->mapeff.move.y[2].num = 0;
	recfp->mapeff.move.y[3].num = 0;
	recfp->mapeff.move.y[4].num = 0;
	recfp->mapeff.move.x[0].num = 0;
	recfp->mapeff.move.x[1].num = 0;
	recfp->mapeff.move.x[2].num = 0;
	fread(&recfp->mapeff.lock, sizeof(int), 396, fp);//ノーツ固定切り替えタイミング
	{
		int buf[2][99];
		fread(buf, sizeof(int), 198, fp);//キャラ向き切り替えタイミング
		for (int i = 0; i < 99; i++) {
			recfp->mapeff.carrow.d[i].data = buf[0][i];
			recfp->mapeff.carrow.d[i].time = buf[1][i];
		}
	}
	fread(&recfp->mapeff.viewT, sizeof(int), 198, fp);//ノーツ表示時間変換タイミング
#if SWITCH_NOTE_BOX_2 == 1
	fread(&recfp->mapdata.note, sizeof(note_box_2_t),
		recfp->allnum.notenum[0] + recfp->allnum.notenum[1] + recfp->allnum.notenum[2], fp); /* ノーツデータ */
#else
	fread(&recfp->mapdata.note2.up[0], sizeof(struct note_box), recfp->allnum.notenum[0], fp); /* 上レーンノーツデータ */
	fread(&recfp->mapdata.note2.mid[0], sizeof(struct note_box), recfp->allnum.notenum[1], fp); /* 中レーンノーツデータ */
	fread(&recfp->mapdata.note2.low[0], sizeof(struct note_box), recfp->allnum.notenum[2], fp); /* 下レーンノーツデータ */
#endif
	fread(&recfp->mapdata.notes, sizeof(short int), 1, fp);//ノーツ数
	fread(&recfp->time.end, sizeof(int), 1, fp);//曲終了時間
	{
		int buf[2];
		fread(buf, sizeof(int), 2, fp);
		recfp->mapdata.mdif = buf[0];//最高難易度
		recfp->mapdata.ldif = buf[1];//最終難易度
	}
	fread(&recfp->mapdata.ddif, sizeof(int), 25, fp);//各区間難易度データ
	fread(&recfp->mapdata.ddifG, sizeof(int), 2, fp);//各区間難易度データ
	fread(&recfp->nameset.DifFN, 255, 1, fp);//難易度バー名
	fread(&recfp->mapeff.Movie, sizeof(item_box), recfp->allnum.movienum, fp);//アイテムデータ
	fread(&recfp->mapeff.camera, sizeof(rec_camera_data_t), 255, fp);//カメラデータ
	fread(&recfp->mapeff.scrool, sizeof(rec_scrool_data_t), 99, fp);//スクロールデータ
	fread(&recfp->mapeff.v_BPM.data[0], sizeof(view_BPM_box), recfp->allnum.v_BPMnum, fp);//見た目のBPMデータ
	REC_LINE_READ(&recfp->mapeff.viewLine.d[0], sizeof(rec_viewline_data_t), 99, fp);//ラインガイドの表示/非表示
	fread(&recfp->outpoint, sizeof(int), 2, fp);//エラーデータ
	REC_LINE_READ(&recfp->mapdata.mpal, sizeof(rec_ddif_pal_t), 1, fp);//難易度分析データ

	fclose(fp);

	return 0;
}

/**
 * rrsファイルから全ての情報を書き込む
 * @return 0: OK, -1: error
 */
int rec_score_fwrite(rec_score_file_t *recfp, const TCHAR *path) {
	int dummy = 0;
	FILE *fp;

	_wfopen_s(&fp, path, L"wb");

	if (recfp == NULL || fp == NULL) { return -1; }

	fwrite(&recfp->allnum, sizeof(playnum_box), 1, fp);//各データの個数
	fwrite(&recfp->nameset.mp3FN, 255, 1, fp);//音楽ファイル名
	fwrite(&recfp->mapdata.bpm, sizeof(double), 1, fp);//BPM
	fwrite(&recfp->time.offset, sizeof(int), 1, fp);//offset
	fwrite(&recfp->nameset.sky, 255, 1, fp);//空背景名
	fwrite(&recfp->nameset.ground, 255, 1, fp);//地面画像名
	fwrite(&recfp->nameset.water, 255, 1, fp);//水中画像名
	fwrite(&recfp->nameset.songN, 255, 1, fp);//曲名
	fwrite(&recfp->nameset.songNE, 255, 1, fp);//曲名(英語)
	fwrite(&recfp->mapdata.Lv, sizeof(short int), 1, fp);//レベル
	//fwrite(&item, sizeof(int), 99, fp);//アイテム画像データ(動作未確認)
	{
		int buf[99][2];
		for (int inum = 0; inum < 99; inum++) {
			buf[inum][0] = recfp->mapeff.fall.d[inum].No;
			buf[inum][1] = recfp->mapeff.fall.d[inum].time;
		}
		fwrite(&buf, sizeof(int), 198, fp);//落ち物背景切り替えタイミング
	}
	{
		double buf[5][99][2];
		for (int iLane = 0; iLane < 5; iLane++) {
			for (int inum = 0; inum < 99; inum++) {
				buf[iLane][inum][0] = recfp->mapeff.speedt[iLane].d[inum].time;
				buf[iLane][inum][1] = recfp->mapeff.speedt[iLane].d[inum].speed;
			}
		}
		fwrite(&buf, sizeof(double), 990, fp);//レーン速度
	}
	{
		int buf[3][99][2];
		for (int ilane = 0; ilane < 3; ilane++) {
			for (int inum = 0; inum < 99; inum++) {
				buf[ilane][inum][0] = recfp->mapeff.chamo[ilane].gra[inum];
				buf[ilane][inum][1] = recfp->mapeff.chamo[ilane].time[inum];
			}
		}
		fwrite(&buf, sizeof(int), 594, fp);//キャラグラ変換タイミング
	}
	fwrite(&recfp->mapeff.move.y[0].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[0], fp);//上レーン縦位置移動タイミング
	fwrite(&recfp->mapeff.move.y[1].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[1], fp);//中レーン縦位置移動タイミング
	fwrite(&recfp->mapeff.move.y[2].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[2], fp);//下レーン縦位置移動タイミング
	fwrite(&recfp->mapeff.move.y[3].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[3], fp);//地面縦位置移動タイミング
	fwrite(&recfp->mapeff.move.y[4].d, sizeof(rec_move_data_t), recfp->allnum.Ymovenum[4], fp);//水面縦位置移動タイミング
	fwrite(&recfp->mapeff.move.x[0].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[0], fp);//上レーン横位置移動タイミング
	fwrite(&recfp->mapeff.move.x[1].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[1], fp);//中レーン横位置移動タイミング
	fwrite(&recfp->mapeff.move.x[2].d, sizeof(rec_move_data_t), recfp->allnum.Xmovenum[2], fp);//下レーン横位置移動タイミング
	fwrite(&recfp->mapeff.lock, sizeof(int), 396, fp);//ノーツ固定切り替えタイミング
	{
		int buf[2][99];
		for (int inum = 0; inum < 99; inum++) {
			buf[0][inum] = recfp->mapeff.carrow.d[inum].data;
			buf[1][inum] = recfp->mapeff.carrow.d[inum].time;
		}
		fwrite(&buf, sizeof(int), 198, fp);//キャラ向き切り替えタイミング
	}
	fwrite(&recfp->mapeff.viewT, sizeof(int), 198, fp);//ノーツ表示時間変換タイミング
#if SWITCH_NOTE_BOX_2
	fwrite(&recfp->mapdata.note, sizeof(note_box_2_t), recfp->allnum.notenum[0] + recfp->allnum.notenum[1] + recfp->allnum.notenum[2], fp); /* ノーツデータ */
#else
	fwrite(&recfp->mapdata.note[0], sizeof(struct note_box), recfp->allnum.notenum[0], fp); /* 上レーンノーツデータ */
	fwrite(&recfp->mapdata.note[1], sizeof(struct note_box), recfp->allnum.notenum[1], fp); /* 中レーンノーツデータ */
	fwrite(&recfp->mapdata.note[2], sizeof(struct note_box), recfp->allnum.notenum[2], fp); /* 下レーンノーツデータ */
#endif
	fwrite(&recfp->mapdata.notes, sizeof(short int), 1, fp);//ノーツ数
	fwrite(&recfp->time.end, sizeof(int), 1, fp);//曲終了時間
	fwrite(&recfp->mapdata.ddifG[0], sizeof(int), 1, fp);//最高難易度
	fwrite(&recfp->mapdata.ddifG[1], sizeof(int), 1, fp);//最終難易度
	fwrite(&recfp->mapdata.ddif, sizeof(int), 25, fp);//各区間難易度データ
	fwrite(&dummy, sizeof(int), 1, fp);//各区間難易度データ
	fwrite(&recfp->mapdata.ddifG[1], sizeof(int), 1, fp);//各区間難易度データ
	fwrite(&recfp->nameset.DifFN, 255, 1, fp);//難易度バー名
	fwrite(&recfp->mapeff.Movie, sizeof(item_box), recfp->allnum.movienum, fp);//動画データ
	fwrite(&recfp->mapeff.camera, sizeof(rec_camera_data_t), 255, fp);//カメラデータ
	fwrite(&recfp->mapeff.scrool, sizeof(rec_scrool_data_t), 99, fp);//スクロールデータ
	fwrite(&recfp->mapeff.v_BPM.data[0], sizeof(view_BPM_box), recfp->allnum.v_BPMnum, fp);//見た目のBPMデータ
	REC_LINE_WRITE(&recfp->mapeff.viewLine.d[0], sizeof(rec_viewline_data_t), 99, fp);//ラインガイドの表示/非表示
	fwrite(&recfp->outpoint, sizeof(int), 2, fp);//譜面エラー
	REC_LINE_WRITE(&recfp->mapdata.mpal, sizeof(rec_ddif_pal_t), 1, fp);//難易度分析データ

	fclose(fp);

	return 0;
}

/**
 * rrsファイルからddif計算に必要な情報を読み込む
 * @return 0: OK, -1: error
 */
int RecScoreReadForDdif(rec_score_file_row_t *recfp, const TCHAR *path) {
	FILE *fp;

	_wfopen_s(&fp, path, L"rb");

	if (recfp == NULL || fp == NULL) { return -1; }

	fread(&recfp->allnum, sizeof(playnum_box), 1, fp);//各データの個数
	fseek(fp, 255, SEEK_CUR);//音楽ファイル名
	fread(&recfp->mapdata.bpm, sizeof(double), 1, fp);//BPM
	fseek(fp, sizeof(int), SEEK_CUR);//offset
	fseek(fp, 255, SEEK_CUR);//空背景名
	fseek(fp, 255, SEEK_CUR);//地面画像名
	fseek(fp, 255, SEEK_CUR);//水中画像名
	fseek(fp, 255, SEEK_CUR);//曲名
	fseek(fp, 255, SEEK_CUR);//曲名(英語)
	fread(&recfp->mapdata.Lv, sizeof(short int), 1, fp);//レベル
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//落ち物背景切り替えタイミング
	fseek(fp, sizeof(double) * 990, SEEK_CUR);//レーン速度
	fseek(fp, sizeof(int) * 594, SEEK_CUR);//キャラグラ変換タイミング
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Ymovenum[0], SEEK_CUR);//上レーン縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Ymovenum[1], SEEK_CUR);//中レーン縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Ymovenum[2], SEEK_CUR);//下レーン縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Ymovenum[3], SEEK_CUR);//地面縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Ymovenum[4], SEEK_CUR);//水中縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Xmovenum[0], SEEK_CUR);//上レーン横位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Xmovenum[1], SEEK_CUR);//中レーン横位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * recfp->allnum.Xmovenum[2], SEEK_CUR);//下レーン横位置移動タイミング
	fseek(fp, sizeof(int) * 396, SEEK_CUR);//ノーツ固定切り替えタイミング
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//キャラ向き切り替えタイミング
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//ノーツ表示時間変換タイミング
#if SWITCH_NOTE_BOX_2 == 1
	fread(&recfp->mapdata.note, sizeof(note_box_2_t),
		recfp->allnum.notenum[0] + recfp->allnum.notenum[1] + recfp->allnum.notenum[2], fp); /* ノーツデータ */
#else
	fread(&recfp->mapdata.note2.up[0], sizeof(struct note_box), recfp->allnum.notenum[0], fp); /* 上レーンノーツデータ */
	fread(&recfp->mapdata.note2.mid[0], sizeof(struct note_box), recfp->allnum.notenum[1], fp); /* 中レーンノーツデータ */
	fread(&recfp->mapdata.note2.low[0], sizeof(struct note_box), recfp->allnum.notenum[2], fp); /* 下レーンノーツデータ */
#endif
	fread(&recfp->mapdata.notes, sizeof(short int), 1, fp);//ノーツ数
	fseek(fp, sizeof(int), SEEK_CUR);//曲終了時間
	{
		int buf[2];
		fread(buf, sizeof(int), 2, fp);
		recfp->mapdata.mdif = buf[0];//最高難易度
		recfp->mapdata.ldif = buf[1];//最終難易度
	}
	fread(&recfp->mapdata.ddif, sizeof(int), 25, fp);//各区間難易度データ
	fread(&recfp->mapdata.ddifG, sizeof(int), 2, fp);//各区間難易度データ
	// fseek(fp, 255, SEEK_CUR);//難易度バー名
	// fseek(fp, sizeof(item_box) * recfp->allnum.movienum, SEEK_CUR);//アイテムデータ
	// fseek(fp, sizeof(rec_camera_data_t) * 255, SEEK_CUR);//カメラデータ
	// fseek(fp, sizeof(rec_scrool_data_t) * 99, SEEK_CUR);//スクロールデータ
	// fseek(fp, sizeof(view_BPM_box) * recfp->allnum.v_BPMnum, SEEK_CUR);//スクロールデータ
	// REC_LINE_SEEK(&recfp->mapeff.viewLine.d[0], sizeof(rec_viewline_data_t), 99, fp);//ラインガイドの表示/非表示
	// fseek(fp, sizeof(int) * 2, SEEK_CUR);//エラーデータ

	fclose(fp);

	return 0;
}

/**
 * rrsファイルから曲名を読み込む
 * @return 0: OK, -1: error
 */
int RecScoreReadSongName(TCHAR *songName, const TCHAR *path) {
	FILE *fp;

	_wfopen_s(&fp, path, L"rb");

	if (songName == NULL || fp == NULL) { return -1; }

	REC_LINE_SEEK(&recfp->allnum, sizeof(playnum_box), 1, fp);//各データの個数
	REC_LINE_SEEK(&recfp->nameset.mp3FN, 255, 1, fp);//音楽ファイル名
	REC_LINE_SEEK(&recfp->mapdata.bpm, sizeof(double), 1, fp);//BPM
	REC_LINE_SEEK(&recfp->time.offset, sizeof(int), 1, fp);//offset
	REC_LINE_SEEK(&recfp->nameset.sky, 255, 1, fp);//空背景名
	REC_LINE_SEEK(&recfp->nameset.ground, 255, 1, fp);//地面画像名
	REC_LINE_SEEK(&recfp->nameset.water, 255, 1, fp);//水中画像名
	REC_LINE_READ(songName, 255, 1, fp);//曲名

	fclose(fp);

	return 0;
}

/**
 * rrsファイルからddifを読み込む
 * @return 0: OK, -1: error
 */
int RecScoreReadDdif(rec_ddif_pal_t *ddif, const TCHAR *path) {
	FILE *fp;

	playnum_box allnum;
	if (RecScoreGetAllnum(&allnum, path) != 0) { return -1; }

	_wfopen_s(&fp, path, L"rb");

	if (ddif == NULL || fp == NULL) { return -1; }

	REC_LINE_SEEK(&allnum, sizeof(playnum_box), 1, fp);//各データの個数
	fseek(fp, 255, SEEK_CUR);//音楽ファイル名
	REC_LINE_SEEK(&recfp->mapdata.bpm, sizeof(double), 1, fp);//BPM
	fseek(fp, sizeof(int), SEEK_CUR);//offset
	fseek(fp, 255, SEEK_CUR);//空背景名
	fseek(fp, 255, SEEK_CUR);//地面画像名
	fseek(fp, 255, SEEK_CUR);//水中画像名
	fseek(fp, 255, SEEK_CUR);//曲名
	fseek(fp, 255, SEEK_CUR);//曲名(英語)
	REC_LINE_SEEK(&recfp->mapdata.Lv, sizeof(short int), 1, fp);//レベル
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//落ち物背景切り替えタイミング
	fseek(fp, sizeof(double) * 990, SEEK_CUR);//レーン速度
	fseek(fp, sizeof(int) * 594, SEEK_CUR);//キャラグラ変換タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[0], SEEK_CUR);//上レーン縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[1], SEEK_CUR);//中レーン縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[2], SEEK_CUR);//下レーン縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[3], SEEK_CUR);//地面縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[4], SEEK_CUR);//水中縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Xmovenum[0], SEEK_CUR);//上レーン横位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Xmovenum[1], SEEK_CUR);//中レーン横位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Xmovenum[2], SEEK_CUR);//下レーン横位置移動タイミング
	fseek(fp, sizeof(int) * 396, SEEK_CUR);//ノーツ固定切り替えタイミング
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//キャラ向き切り替えタイミング
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//ノーツ表示時間変換タイミング
#if SWITCH_NOTE_BOX_2 == 1
	REC_LINE_SEEK(&recfp->mapdata.note, sizeof(note_box_2_t), allnum.notenum[0] + allnum.notenum[1] + allnum.notenum[2], fp); /* ノーツデータ */
#else
	REC_LINE_SEEK(&recfp->mapdata.note2.up[0], sizeof(struct note_box), allnum.notenum[0], fp); /* 上レーンノーツデータ */
	REC_LINE_SEEK(&recfp->mapdata.note2.mid[0], sizeof(struct note_box), allnum.notenum[1], fp); /* 中レーンノーツデータ */
	REC_LINE_SEEK(&recfp->mapdata.note2.low[0], sizeof(struct note_box), allnum.notenum[2], fp); /* 下レーンノーツデータ */
#endif
	REC_LINE_SEEK(&recfp->mapdata.notes, sizeof(short int), 1, fp);//ノーツ数
	fseek(fp, sizeof(int), SEEK_CUR);//曲終了時間
	REC_LINE_SEEK(buf, sizeof(int), 2, fp);
	REC_LINE_SEEK(&recfp->mapdata.ddif, sizeof(int), 25, fp);//各区間難易度データ
	REC_LINE_SEEK(&recfp->mapdata.ddifG, sizeof(int), 2, fp);//各区間難易度データ
	fseek(fp, 255, SEEK_CUR);//難易度バー名
	fseek(fp, sizeof(item_box) * allnum.movienum, SEEK_CUR);//アイテムデータ
	fseek(fp, sizeof(rec_camera_data_t) * 255, SEEK_CUR);//カメラデータ
	fseek(fp, sizeof(rec_scrool_data_t) * 99, SEEK_CUR);//スクロールデータ
	fseek(fp, sizeof(view_BPM_box) * allnum.v_BPMnum, SEEK_CUR);//スクロールデータ
	fseek(fp, sizeof(int) * 2, SEEK_CUR);//エラーデータ
	REC_LINE_SEEK(&recfp->mapeff.viewLine.d[0], sizeof(rec_viewline_data_t), 99, fp);//ラインガイドの表示/非表示
	REC_LINE_READ(ddif, sizeof(rec_ddif_pal_t), 1, fp);//難易度分析データ

	fclose(fp);

	return 0;
}

/**
 * rrsファイルからddifを書き込む
 * @return 0: OK, -1: error
 */
int RecScoreWriteDdif(rec_ddif_pal_t *ddif, const TCHAR *path) {
	FILE *fp;

	playnum_box allnum;
	if (RecScoreGetAllnum(&allnum, path) != 0) { return -1; }

	_wfopen_s(&fp, path, L"ab");

	if (ddif == NULL || fp == NULL) { return -1; }

	REC_LINE_SEEK(&allnum, sizeof(playnum_box), 1, fp);//各データの個数
	fseek(fp, 255, SEEK_CUR);//音楽ファイル名
	REC_LINE_SEEK(&recfp->mapdata.bpm, sizeof(double), 1, fp);//BPM
	fseek(fp, sizeof(int), SEEK_CUR);//offset
	fseek(fp, 255, SEEK_CUR);//空背景名
	fseek(fp, 255, SEEK_CUR);//地面画像名
	fseek(fp, 255, SEEK_CUR);//水中画像名
	fseek(fp, 255, SEEK_CUR);//曲名
	fseek(fp, 255, SEEK_CUR);//曲名(英語)
	REC_LINE_SEEK(&recfp->mapdata.Lv, sizeof(short int), 1, fp);//レベル
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//落ち物背景切り替えタイミング
	fseek(fp, sizeof(double) * 990, SEEK_CUR);//レーン速度
	fseek(fp, sizeof(int) * 594, SEEK_CUR);//キャラグラ変換タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[0], SEEK_CUR);//上レーン縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[1], SEEK_CUR);//中レーン縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[2], SEEK_CUR);//下レーン縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[3], SEEK_CUR);//地面縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Ymovenum[4], SEEK_CUR);//水中縦位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Xmovenum[0], SEEK_CUR);//上レーン横位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Xmovenum[1], SEEK_CUR);//中レーン横位置移動タイミング
	fseek(fp, sizeof(rec_move_data_t) * allnum.Xmovenum[2], SEEK_CUR);//下レーン横位置移動タイミング
	fseek(fp, sizeof(int) * 396, SEEK_CUR);//ノーツ固定切り替えタイミング
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//キャラ向き切り替えタイミング
	fseek(fp, sizeof(int) * 198, SEEK_CUR);//ノーツ表示時間変換タイミング
#if SWITCH_NOTE_BOX_2 == 1
	REC_LINE_SEEK(&recfp->mapdata.note, sizeof(note_box_2_t), allnum.notenum[0] + allnum.notenum[1] + allnum.notenum[2], fp); /* ノーツデータ */
#else
	REC_LINE_SEEK(&recfp->mapdata.note2.up[0], sizeof(struct note_box), allnum.notenum[0], fp); /* 上レーンノーツデータ */
	REC_LINE_SEEK(&recfp->mapdata.note2.mid[0], sizeof(struct note_box), allnum.notenum[1], fp); /* 中レーンノーツデータ */
	REC_LINE_SEEK(&recfp->mapdata.note2.low[0], sizeof(struct note_box), allnum.notenum[2], fp); /* 下レーンノーツデータ */
#endif
	REC_LINE_SEEK(&recfp->mapdata.notes, sizeof(short int), 1, fp);//ノーツ数
	fseek(fp, sizeof(int), SEEK_CUR);//曲終了時間
	REC_LINE_SEEK(buf, sizeof(int), 2, fp);
	REC_LINE_SEEK(&recfp->mapdata.ddif, sizeof(int), 25, fp);//各区間難易度データ
	REC_LINE_SEEK(&recfp->mapdata.ddifG, sizeof(int), 2, fp);//各区間難易度データ
	fseek(fp, 255, SEEK_CUR);//難易度バー名
	fseek(fp, sizeof(item_box) * allnum.movienum, SEEK_CUR);//アイテムデータ
	fseek(fp, sizeof(rec_camera_data_t) * 255, SEEK_CUR);//カメラデータ
	fseek(fp, sizeof(rec_scrool_data_t) * 99, SEEK_CUR);//スクロールデータ
	fseek(fp, sizeof(view_BPM_box) * allnum.v_BPMnum, SEEK_CUR);//スクロールデータ
	REC_LINE_SEEK(&recfp->mapeff.viewLine.d[0], sizeof(rec_viewline_data_t), 99, fp);//ラインガイドの表示/非表示
	fseek(fp, sizeof(int) * 2, SEEK_CUR);//エラーデータ
	REC_LINE_WRITE(ddif, sizeof(rec_ddif_pal_t), 1, fp);//難易度分析データ

	fclose(fp);

	return 0;
}
