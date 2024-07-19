
#include <stdio.h>
#include "RecScoreFile.h"

/* 0: OK, -1: error */
int rec_score_fread(rec_score_file_t *recfp, FILE *fp) {
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
	fread(&recfp->mapeff.speedt, sizeof(double), 990, fp);//レーン速度
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
	{
#if 0
		int buf[999][4];
		fread(buf, sizeof(int), recfp->allnum.Ymovenum[0] * 4, fp);//上レーン縦位置移動タイミング
		for (int i = 0; i < recfp->allnum.Ymovenum[0]; i++) {
			recfp->mapeff.move.y[0].d[i].Stime = buf[i][0];
			recfp->mapeff.move.y[0].d[i].pos = buf[i][1];
			recfp->mapeff.move.y[0].d[i].Etime = buf[i][2];
			recfp->mapeff.move.y[0].d[i].mode = buf[i][3];
		}
		fread(buf, sizeof(int), recfp->allnum.Ymovenum[1] * 4, fp);//中レーン縦位置移動タイミング
		for (int i = 0; i < recfp->allnum.Ymovenum[1]; i++) {
			recfp->mapeff.move.y[1].d[i].Stime = buf[i][0];
			recfp->mapeff.move.y[1].d[i].pos = buf[i][1];
			recfp->mapeff.move.y[1].d[i].Etime = buf[i][2];
			recfp->mapeff.move.y[1].d[i].mode = buf[i][3];
		}
		fread(buf, sizeof(int), recfp->allnum.Ymovenum[2] * 4, fp);//下レーン縦位置移動タイミング
		for (int i = 0; i < recfp->allnum.Ymovenum[2]; i++) {
			recfp->mapeff.move.y[2].d[i].Stime = buf[i][0];
			recfp->mapeff.move.y[2].d[i].pos = buf[i][1];
			recfp->mapeff.move.y[2].d[i].Etime = buf[i][2];
			recfp->mapeff.move.y[2].d[i].mode = buf[i][3];
		}
		fread(buf, sizeof(int), recfp->allnum.Ymovenum[3] * 4, fp);//地面縦位置移動タイミング
		for (int i = 0; i < recfp->allnum.Ymovenum[3]; i++) {
			recfp->mapeff.move.y[3].d[i].Stime = buf[i][0];
			recfp->mapeff.move.y[3].d[i].pos = buf[i][1];
			recfp->mapeff.move.y[3].d[i].Etime = buf[i][2];
			recfp->mapeff.move.y[3].d[i].mode = buf[i][3];
		}
		fread(buf, sizeof(int), recfp->allnum.Ymovenum[4] * 4, fp);//水面縦位置移動タイミング
		for (int i = 0; i < recfp->allnum.Ymovenum[4]; i++) {
			recfp->mapeff.move.y[4].d[i].Stime = buf[i][0];
			recfp->mapeff.move.y[4].d[i].pos = buf[i][1];
			recfp->mapeff.move.y[4].d[i].Etime = buf[i][2];
			recfp->mapeff.move.y[4].d[i].mode = buf[i][3];
		}
		fread(buf, sizeof(int), recfp->allnum.Xmovenum[0] * 4, fp);//上レーン横位置移動タイミング
		for (int i = 0; i < recfp->allnum.Xmovenum[0]; i++) {
			recfp->mapeff.move.x[0].d[i].Stime = buf[i][0];
			recfp->mapeff.move.x[0].d[i].pos = buf[i][1];
			recfp->mapeff.move.x[0].d[i].Etime = buf[i][2];
			recfp->mapeff.move.x[0].d[i].mode = buf[i][3];
		}
		fread(buf, sizeof(int), recfp->allnum.Xmovenum[1] * 4, fp);//中レーン横位置移動タイミング
		for (int i = 0; i < recfp->allnum.Xmovenum[1]; i++) {
			recfp->mapeff.move.x[1].d[i].Stime = buf[i][0];
			recfp->mapeff.move.x[1].d[i].pos = buf[i][1];
			recfp->mapeff.move.x[1].d[i].Etime = buf[i][2];
			recfp->mapeff.move.x[1].d[i].mode = buf[i][3];
		}
		fread(buf, sizeof(int), recfp->allnum.Xmovenum[2] * 4, fp);//下レーン横位置移動タイミング
		for (int i = 0; i < recfp->allnum.Xmovenum[2]; i++) {
			recfp->mapeff.move.x[2].d[i].Stime = buf[i][0];
			recfp->mapeff.move.x[2].d[i].pos = buf[i][1];
			recfp->mapeff.move.x[2].d[i].Etime = buf[i][2];
			recfp->mapeff.move.x[2].d[i].mode = buf[i][3];
		}
#else
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
#endif
	}
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
	fread(&recfp->mapeff.camera, sizeof(struct camera_box), 255, fp);//カメラデータ
	fread(&recfp->mapeff.scrool, sizeof(struct scrool_box), 99, fp);//スクロールデータ
	fread(&recfp->mapeff.v_BPM.data[0], sizeof(view_BPM_box), recfp->allnum.v_BPMnum, fp);//見た目のBPMデータ
	fread(&recfp->outpoint, sizeof(int), 2, fp);//エラーデータ

	return 0;
}
