/* TODO: scoreって名前にするとプレイデータと勘違いしそう。ここで言うscoreは譜面データのこと。 */

/* base include */
#include <stdio.h>

#include <strcur.h>

/* rec system include */
#include <RecSystem.h>

/* own include */
#include <RecScoreFile.h>

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

bool RecScoreReadNameset(rec_play_nameset_t *dest, FILE *fp) {
	if (dest == nullptr) { return false; }
	if (  fp == nullptr) { return false; }
	ReadFileForTString(dest->songN,  fp); /* 曲名           */
	ReadFileForTString(dest->songNE, fp); /* 曲名(英語)     */
	ReadFileForTString(dest->DifFN,  fp); /* 難易度バー名   */
	ReadFileForTString(dest->mp3FN,  fp); /* 音楽ファイル名 */
	ReadFileForTString(dest->sky,    fp); /* 空背景名       */
	ReadFileForTString(dest->ground, fp); /* 地面画像名     */
	ReadFileForTString(dest->water,  fp); /* 水中画像名     */
	return true;
}

bool RecScoreWriteNameset(const rec_play_nameset_t *src, FILE *fp) {
	if (src == nullptr) { return false; }
	if ( fp == nullptr) { return false; }
	WriteFileForTString(src->songN,  fp); /* 曲名           */
	WriteFileForTString(src->songNE, fp); /* 曲名(英語)     */
	WriteFileForTString(src->DifFN,  fp); /* 難易度バー名   */
	WriteFileForTString(src->mp3FN,  fp); /* 音楽ファイル名 */
	WriteFileForTString(src->sky,    fp); /* 空背景名       */
	WriteFileForTString(src->ground, fp); /* 地面画像名     */
	WriteFileForTString(src->water,  fp); /* 水中画像名     */
	return true;
}

bool RecScoreReadMapdata(rec_map_detail_t *dest, FILE *fp, size_t notes_count) {
	if (dest == nullptr) { return false; }
	if (  fp == nullptr) { return false; }
	fread(&dest->note,  sizeof(note_box_2_t), notes_count, fp); /* ノーツデータ */
	fread(&dest->notes, sizeof(short),          1, fp); /* ノーツ数 */
	fread(&dest->bpm,   sizeof(double),         1, fp); /* BPM      */
	fread(&dest->Lv,    sizeof(short),          1, fp); /* レベル   */
	fread(&dest->mdif,  sizeof(int),            1, fp); /* 最高難易度 */
	fread(&dest->ldif,  sizeof(int),            1, fp); /* 最終難易度 */
	fread(&dest->ddif,  sizeof(int),           25, fp); /* 各区間難易度データ */
	fread(&dest->ddifG, sizeof(int),            2, fp); /* 各区間難易度データ */
	fread(&dest->mpal,  sizeof(rec_ddif_pal_t), 1, fp); /* 難易度分析データ */
	return true;
}

bool RecScoreWriteMapdata(const rec_map_detail_t *src, FILE *fp, size_t notes_count) {
	if (src == nullptr) { return false; }
	if ( fp == nullptr) { return false; }
	fwrite(&src->note,  sizeof(note_box_2_t), notes_count, fp); /* ノーツデータ */
	fwrite(&src->notes, sizeof(short),          1, fp); /* ノーツ数 */
	fwrite(&src->bpm,   sizeof(double),         1, fp); /* BPM      */
	fwrite(&src->Lv,    sizeof(short),          1, fp); /* レベル   */
	fwrite(&src->mdif,  sizeof(int),            1, fp); /* 最高難易度 */
	fwrite(&src->ldif,  sizeof(int),            1, fp); /* 最終難易度 */
	fwrite(&src->ddif,  sizeof(int),           25, fp); /* 各区間難易度データ */
	fwrite(&src->ddifG, sizeof(int),            2, fp); /* 各区間難易度データ */
	fwrite(&src->mpal,  sizeof(rec_ddif_pal_t), 1, fp); /* 難易度分析データ */
	return true;
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
	RecScoreReadNameset(&recfp->nameset, fp);
	RecScoreReadMapdata(&recfp->mapdata, fp, recfp->allnum.notenum[0] + recfp->allnum.notenum[1] + recfp->allnum.notenum[2]);
	fread(&recfp->time.offset, sizeof(int), 1, fp);//offset
	recfp->mapeff.fall.fread(fp); /* 落ち物背景切り替えタイミング */
	recfp->mapeff.speedt[0].fread(fp); /* レーン速度 */
	recfp->mapeff.speedt[1].fread(fp);
	recfp->mapeff.speedt[2].fread(fp);
	recfp->mapeff.speedt[3].fread(fp);
	recfp->mapeff.speedt[4].fread(fp);
	recfp->mapeff.chamo[0].fread(fp); /* キャラグラ変換タイミング */
	recfp->mapeff.chamo[1].fread(fp);
	recfp->mapeff.chamo[2].fread(fp);
	recfp->mapeff.move.y[0].fread(fp); /* 上レーン縦位置移動タイミング */
	recfp->mapeff.move.y[1].fread(fp); /* 中レーン縦位置移動タイミング */
	recfp->mapeff.move.y[2].fread(fp); /* 下レーン縦位置移動タイミング */
	recfp->mapeff.move.y[3].fread(fp); /*     地面縦位置移動タイミング */
	recfp->mapeff.move.y[4].fread(fp); /*     水面縦位置移動タイミング */
	recfp->mapeff.move.x[0].fread(fp); /* 上レーン横位置移動タイミング */
	recfp->mapeff.move.x[1].fread(fp); /* 中レーン横位置移動タイミング */
	recfp->mapeff.move.x[2].fread(fp); /* 下レーン横位置移動タイミング */
	recfp->mapeff.lock.x.fread(fp); /* ノーツ固定切り替えタイミング */
	recfp->mapeff.lock.y.fread(fp);
	recfp->mapeff.carrow.fread(fp); /* キャラ向き切り替えタイミング */
	recfp->mapeff.viewT.fread(fp); /* ノーツ表示時間変換タイミング */
	fread(&recfp->time.end, sizeof(int), 1, fp);//曲終了時間
	recfp->mapeff.Movie.fread(fp); /* アイテムデータ */
	recfp->mapeff.camera.fread(fp); /* カメラデータ */
	recfp->mapeff.scrool.fread(fp); /* スクロールデータ */
	recfp->mapeff.v_BPM.fread(fp); /* 見た目のBPMデータ */
	recfp->mapeff.viewLine.fread(fp); /* ラインガイドの表示/非表示 */
	fread(&recfp->outpoint, sizeof(int), 2, fp);//エラーデータ

	fclose(fp);

	return 0;
}

/**
 * rrsファイルから全ての情報を書き込む
 * @return 0: OK, -1: error
 */
int rec_score_fwrite(const rec_score_file_t *recfp, const TCHAR *path) {
	int dummy = 0;
	FILE *fp;

	_wfopen_s(&fp, path, L"wb");

	if (recfp == NULL || fp == NULL) { return -1; }

	fwrite(&recfp->allnum, sizeof(playnum_box), 1, fp);//各データの個数
	RecScoreWriteNameset(&recfp->nameset, fp);
	RecScoreWriteMapdata(&recfp->mapdata, fp, recfp->allnum.notenum[0] + recfp->allnum.notenum[1] + recfp->allnum.notenum[2]);
	fwrite(&recfp->time.offset, sizeof(int), 1, fp);//offset
	recfp->mapeff.fall.fwrite(fp); /* 落ち物背景切り替えタイミング */
	recfp->mapeff.speedt[0].fwrite(fp); /* レーン速度 */
	recfp->mapeff.speedt[1].fwrite(fp);
	recfp->mapeff.speedt[2].fwrite(fp);
	recfp->mapeff.speedt[3].fwrite(fp);
	recfp->mapeff.speedt[4].fwrite(fp);
	recfp->mapeff.chamo[0].fwrite(fp); /* キャラグラ変換タイミング */
	recfp->mapeff.chamo[1].fwrite(fp);
	recfp->mapeff.chamo[2].fwrite(fp);
	recfp->mapeff.move.y[0].fwrite(fp); /* 上レーン縦位置移動タイミング */
	recfp->mapeff.move.y[1].fwrite(fp); /* 中レーン縦位置移動タイミング */
	recfp->mapeff.move.y[2].fwrite(fp); /* 下レーン縦位置移動タイミング */
	recfp->mapeff.move.y[3].fwrite(fp); /*     地面縦位置移動タイミング */
	recfp->mapeff.move.y[4].fwrite(fp); /*     水面縦位置移動タイミング */
	recfp->mapeff.move.x[0].fwrite(fp); /* 上レーン横位置移動タイミング */
	recfp->mapeff.move.x[1].fwrite(fp); /* 中レーン横位置移動タイミング */
	recfp->mapeff.move.x[2].fwrite(fp); /* 下レーン横位置移動タイミング */
	recfp->mapeff.lock.x.fwrite(fp); /* ノーツ固定切り替えタイミング */
	recfp->mapeff.lock.y.fwrite(fp);
	recfp->mapeff.carrow.fwrite(fp); /* キャラ向き切り替えタイミング */
	recfp->mapeff.viewT.fwrite(fp); /* ノーツ表示時間変換タイミング */
	fwrite(&recfp->time.end, sizeof(int), 1, fp);//曲終了時間
	recfp->mapeff.Movie.fwrite(fp); /* アイテムデータ */
	recfp->mapeff.camera.fwrite(fp); /* カメラデータ */
	recfp->mapeff.scrool.fwrite(fp); /* スクロールデータ */
	recfp->mapeff.v_BPM.fwrite(fp); /* 見た目のBPMデータ */
	recfp->mapeff.viewLine.fwrite(fp); /* ラインガイドの表示/非表示 */
	fwrite(&recfp->outpoint, sizeof(int), 2, fp);//譜面エラー

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
	{
		rec_play_nameset_t buf;
		RecScoreReadNameset(&buf, fp); /* seekするだけ */
	}
	RecScoreReadMapdata(&recfp->mapdata, fp, recfp->allnum.notenum[0] + recfp->allnum.notenum[1] + recfp->allnum.notenum[2]);

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

	fseek(fp, sizeof(playnum_box), 1); //各データの個数
	{
		rec_play_nameset_t buf;
		RecScoreReadNameset(&buf, fp);
		strcopy_2(buf.songN.c_str(), songName, 255);
	}

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

	REC_LINE_READ(&allnum, sizeof(playnum_box), 1, fp);//各データの個数
	{
		rec_play_nameset_t buf;
		RecScoreReadNameset(&buf, fp); /* seekするだけ */
	}
	{
		rec_map_detail_t buf;
		RecScoreReadMapdata(&buf, fp, allnum.notenum[0] + allnum.notenum[1] + allnum.notenum[2]);
		*ddif = buf.mpal;
	}

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

	_wfopen_s(&fp, path, L"rb+");

	if (ddif == NULL || fp == NULL) { return -1; }

	REC_LINE_SEEK(&allnum, sizeof(playnum_box), 1, fp);//各データの個数
	{
		rec_play_nameset_t buf;
		RecScoreReadNameset(&buf, fp); /* seekするだけ */
	}
	{
		rec_map_detail_t buf;
		RecScoreReadMapdata(&buf, fp, allnum.notenum[0] + allnum.notenum[1] + allnum.notenum[2]); /* seekするだけ */
	}
	fseek(fp, sizeof(rec_ddif_pal_t), SEEK_CUR); /* seekし過ぎたので戻す */
	fwrite(ddif, sizeof(rec_ddif_pal_t), 1, fp); /* 書き込む */

	fclose(fp);

	return 0;
}
