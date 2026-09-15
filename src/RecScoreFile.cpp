/* TODO: scoreって名前にするとプレイデータと勘違いしそう。ここで言うscoreは譜面データのこと。 */

/* base include */
#include <strcur.h>

/* rec system include */
#include <RecSystem.h>

/* own include */
#include <RecScoreFile.h>

#if 1 /* nameset */

static bool RecScoreReadNameset(rec_play_nameset_t &dest, FILE *fp) {
	if (fp == nullptr) { return false; }
	ReadFileForTString(dest.songN,  fp); /* 曲名           */
	ReadFileForTString(dest.songNE, fp); /* 曲名(英語)     */
	ReadFileForTString(dest.DifFN,  fp); /* 難易度バー名   */
	ReadFileForTString(dest.mp3FN,  fp); /* 音楽ファイル名 */
	ReadFileForTString(dest.sky,    fp); /* 空背景名       */
	ReadFileForTString(dest.ground, fp); /* 地面画像名     */
	ReadFileForTString(dest.water,  fp); /* 水中画像名     */
	return true;
}

static bool RecScoreWriteNameset(const rec_play_nameset_t &src, FILE *fp) {
	if (fp == nullptr) { return false; }
	WriteFileForTString(src.songN,  fp); /* 曲名           */
	WriteFileForTString(src.songNE, fp); /* 曲名(英語)     */
	WriteFileForTString(src.DifFN,  fp); /* 難易度バー名   */
	WriteFileForTString(src.mp3FN,  fp); /* 音楽ファイル名 */
	WriteFileForTString(src.sky,    fp); /* 空背景名       */
	WriteFileForTString(src.ground, fp); /* 地面画像名     */
	WriteFileForTString(src.water,  fp); /* 水中画像名     */
	return true;
}

static bool RecScoreSeekNameset(FILE *fp) {
	if (fp == nullptr) { return false; }
	tstring buf;
	ReadFileForTString(buf, fp); /* 曲名           */
	ReadFileForTString(buf, fp); /* 曲名(英語)     */
	ReadFileForTString(buf, fp); /* 難易度バー名   */
	ReadFileForTString(buf, fp); /* 音楽ファイル名 */
	ReadFileForTString(buf, fp); /* 空背景名       */
	ReadFileForTString(buf, fp); /* 地面画像名     */
	ReadFileForTString(buf, fp); /* 水中画像名     */
	return true;
}

#endif /* nameset */

#if 1 /* mapdata */

static bool RecScoreReadMapdata(rec_map_detail_t &dest, FILE *fp) {
	if (fp == nullptr) { return false; }
	dest.note[0].fread(fp); /* ノーツデータ */
	dest.note[1].fread(fp);
	dest.note[2].fread(fp);
	fread(&dest.notes, sizeof(short),          1, fp); /* ノーツ数 */
	fread(&dest.bpm,   sizeof(double),         1, fp); /* BPM */
	fread(&dest.Lv,    sizeof(short),          1, fp); /* レベル */
	fread(&dest.mdif,  sizeof(int),            1, fp); /* 最高難易度 */
	fread(&dest.ldif,  sizeof(int),            1, fp); /* 最終難易度 */
	fread(&dest.ddif,  sizeof(int),           25, fp); /* 各区間難易度データ */
	fread(&dest.ddifG, sizeof(int),            2, fp); /* 各区間難易度データ */
	fread(&dest.mpal,  sizeof(rec_ddif_pal_t), 1, fp); /* 難易度分析データ */
	return true;
}

static bool RecScoreWriteMapdata(const rec_map_detail_t &src, FILE *fp) {
	if (fp == nullptr) { return false; }
	src.note[0].fwrite(fp); /* ノーツデータ */
	src.note[1].fwrite(fp);
	src.note[2].fwrite(fp);
	fwrite(&src.notes, sizeof(short),          1, fp); /* ノーツ数 */
	fwrite(&src.bpm,   sizeof(double),         1, fp); /* BPM */
	fwrite(&src.Lv,    sizeof(short),          1, fp); /* レベル */
	fwrite(&src.mdif,  sizeof(int),            1, fp); /* 最高難易度 */
	fwrite(&src.ldif,  sizeof(int),            1, fp); /* 最終難易度 */
	fwrite(&src.ddif,  sizeof(int),           25, fp); /* 各区間難易度データ */
	fwrite(&src.ddifG, sizeof(int),            2, fp); /* 各区間難易度データ */
	fwrite(&src.mpal,  sizeof(rec_ddif_pal_t), 1, fp); /* 難易度分析データ */
	return true;
}

static bool RecScoreSeekMapdata(FILE *fp) {
	if (fp == nullptr) { return false; }
	SeekFileForVector<note_box_2_t>(fp); /* ノーツデータ上 */
	SeekFileForVector<note_box_2_t>(fp); /* ノーツデータ中 */
	SeekFileForVector<note_box_2_t>(fp); /* ノーツデータ下 */
	fseek(fp, sizeof(short),          SEEK_CUR); /* ノーツ数 */
	fseek(fp, sizeof(double),         SEEK_CUR); /* BPM */
	fseek(fp, sizeof(short),          SEEK_CUR); /* レベル */
	fseek(fp, sizeof(int),            SEEK_CUR); /* 最高難易度 */
	fseek(fp, sizeof(int),            SEEK_CUR); /* 最終難易度 */
	fseek(fp, sizeof(int) * 25,       SEEK_CUR); /* 各区間難易度データ */
	fseek(fp, sizeof(int) *  2,       SEEK_CUR); /* 各区間難易度データ */
	fseek(fp, sizeof(rec_ddif_pal_t), SEEK_CUR); /* 難易度分析データ */
	return true;
}

#endif /* mapdata */

#if 1 /* mapeff */

static bool RecScoreReadMapeff(rec_map_eff_data_t &dest, FILE *fp) {
	if (fp == nullptr) { return false; }
	dest.fall.fread(fp); /* 落ち物背景切り替えタイミング */
	dest.speedt[0].fread(fp); /* レーン速度 */
	dest.speedt[1].fread(fp);
	dest.speedt[2].fread(fp);
	dest.speedt[3].fread(fp);
	dest.speedt[4].fread(fp);
	dest.chamo[0].fread(fp); /* キャラグラ変換タイミング */
	dest.chamo[1].fread(fp);
	dest.chamo[2].fread(fp);
	dest.move.y[0].fread(fp); /* 上レーン縦位置移動タイミング */
	dest.move.y[1].fread(fp); /* 中レーン縦位置移動タイミング */
	dest.move.y[2].fread(fp); /* 下レーン縦位置移動タイミング */
	dest.move.y[3].fread(fp); /*     地面縦位置移動タイミング */
	dest.move.y[4].fread(fp); /*     水面縦位置移動タイミング */
	dest.move.x[0].fread(fp); /* 上レーン横位置移動タイミング */
	dest.move.x[1].fread(fp); /* 中レーン横位置移動タイミング */
	dest.move.x[2].fread(fp); /* 下レーン横位置移動タイミング */
	dest.lock.x.fread(fp); /* ノーツ固定切り替えタイミング */
	dest.lock.y.fread(fp);
	dest.carrow.fread(fp); /* キャラ向き切り替えタイミング */
	dest.viewT.fread(fp); /* ノーツ表示時間変換タイミング */
	dest.Movie.fread(fp); /* アイテムデータ */
	dest.camera.fread(fp); /* カメラデータ */
	dest.scrool.fread(fp); /* スクロールデータ */
	dest.v_BPM.fread(fp); /* 見た目のBPMデータ */
	dest.viewLine.fread(fp); /* ラインガイドの表示/非表示 */
	dest.gnote.fread(fp); /* ゴーストノート */
	return true;
}

static bool RecScoreWriteMapeff(const rec_map_eff_data_t &src, FILE *fp) {
	if (fp == nullptr) { return false; }
	src.fall.fwrite(fp); /* 落ち物背景切り替えタイミング */
	src.speedt[0].fwrite(fp); /* レーン速度 */
	src.speedt[1].fwrite(fp);
	src.speedt[2].fwrite(fp);
	src.speedt[3].fwrite(fp);
	src.speedt[4].fwrite(fp);
	src.chamo[0].fwrite(fp); /* キャラグラ変換タイミング */
	src.chamo[1].fwrite(fp);
	src.chamo[2].fwrite(fp);
	src.move.y[0].fwrite(fp); /* 上レーン縦位置移動タイミング */
	src.move.y[1].fwrite(fp); /* 中レーン縦位置移動タイミング */
	src.move.y[2].fwrite(fp); /* 下レーン縦位置移動タイミング */
	src.move.y[3].fwrite(fp); /*     地面縦位置移動タイミング */
	src.move.y[4].fwrite(fp); /*     水面縦位置移動タイミング */
	src.move.x[0].fwrite(fp); /* 上レーン横位置移動タイミング */
	src.move.x[1].fwrite(fp); /* 中レーン横位置移動タイミング */
	src.move.x[2].fwrite(fp); /* 下レーン横位置移動タイミング */
	src.lock.x.fwrite(fp); /* ノーツ固定切り替えタイミング */
	src.lock.y.fwrite(fp);
	src.carrow.fwrite(fp); /* キャラ向き切り替えタイミング */
	src.viewT.fwrite(fp); /* ノーツ表示時間変換タイミング */
	src.Movie.fwrite(fp); /* アイテムデータ */
	src.camera.fwrite(fp); /* カメラデータ */
	src.scrool.fwrite(fp); /* スクロールデータ */
	src.v_BPM.fwrite(fp); /* 見た目のBPMデータ */
	src.viewLine.fwrite(fp); /* ラインガイドの表示/非表示 */
	src.gnote.fwrite(fp); /* ゴーストノート */
	return true;
}

#endif /* mapeff */

/**
 * rrsファイルから全ての情報を読み込む
 * @return bool
 */
bool rec_score_fread(rec_score_file_t &recfp, const tstring &path) {
	FILE *fp;

	_wfopen_s(&fp, path.c_str(), L"rb");

	if (fp == nullptr) { return false; }

	fread(&recfp.allnum, sizeof(playnum_box), 1, fp);//各データの個数
	RecScoreReadNameset(recfp.nameset, fp);
	RecScoreReadMapdata(recfp.mapdata, fp);
	RecScoreReadMapeff(recfp.mapeff, fp);
	fread(&recfp.time.offset, sizeof(int), 1, fp);//offset
	fread(&recfp.time.end, sizeof(int), 1, fp);//曲終了時間
	/* TODO: 譜面エラーはログにしたい */
	fread(&recfp.outpoint, sizeof(int), 2, fp);//エラーデータ

	fclose(fp);

	return true;
}

/**
 * rrsファイルから全ての情報を書き込む
 * @return bool
 */
bool rec_score_fwrite(const rec_score_file_t &recfp, const tstring &path) {
	FILE *fp;

	_wfopen_s(&fp, path.c_str(), L"wb");

	if (fp == nullptr) { return false; }

	fwrite(&recfp.allnum, sizeof(playnum_box), 1, fp);//各データの個数
	RecScoreWriteNameset(recfp.nameset, fp);
	RecScoreWriteMapdata(recfp.mapdata, fp);
	RecScoreWriteMapeff(recfp.mapeff, fp);
	fwrite(&recfp.time.offset, sizeof(int), 1, fp);//offset
	fwrite(&recfp.time.end, sizeof(int), 1, fp);//曲終了時間
	fwrite(&recfp.outpoint, sizeof(int), 2, fp);//譜面エラー

	fclose(fp);

	return true;
}

/**
 * rrsファイルからddif計算に必要な情報を読み込む
 * @return bool
 */
bool RecScoreReadForDdif(rec_score_file_row_t &recfp, const tstring &path) {
	FILE *fp;

	_wfopen_s(&fp, path.c_str(), L"rb");

	if (fp == nullptr) { return false; }

	fread(&recfp.allnum, sizeof(playnum_box), 1, fp);//各データの個数
	RecScoreSeekNameset(fp);
	RecScoreReadMapdata(recfp.mapdata, fp);

	fclose(fp);

	return true;
}

/**
 * rrsファイルからddifを読み込む
 * @return bool
 */
bool RecScoreReadDdif(rec_ddif_pal_t &ddif, const tstring &path) {
	FILE *fp;
	rec_map_detail_t buf;

	_wfopen_s(&fp, path.c_str(), L"rb");

	if (fp == nullptr) { return false; }

	fseek(fp, sizeof(playnum_box), SEEK_CUR);
	RecScoreSeekNameset(fp);
	RecScoreReadMapdata(buf, fp);
	ddif = buf.mpal;

	fclose(fp);

	return true;
}

/**
 * rrsファイルからddifを書き込む
 * @return bool
 */
bool RecScoreWriteDdif(const rec_ddif_pal_t &ddif, const tstring &path) {
	FILE *fp;

	playnum_box allnum;

	_wfopen_s(&fp, path.c_str(), L"rb+");

	if (fp == nullptr) { return false; }

	fseek(fp, sizeof(playnum_box), SEEK_CUR);
	RecScoreSeekNameset(fp);
	RecScoreSeekMapdata(fp);
	fseek(fp, -(int)sizeof(rec_ddif_pal_t), SEEK_CUR); /* seekし過ぎたので戻す */
	fwrite(&ddif, sizeof(rec_ddif_pal_t), 1, fp); /* 書き込む */

	fclose(fp);

	return true;
}
