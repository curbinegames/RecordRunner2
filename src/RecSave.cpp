
/* base include */
#include <stdio.h>

/* curbine code include */
#include <strcur.h>

/* rec system include */
#include <option.h>
#include <playbox.h>
#include <RecSave.h>
#include <result.h>
#include <RecSystem.h>

static void RecSaveConvScToSc2(rec_save_score2_st &dest, const rec_save_score_t &src) {
	dest.score = src.score;
	dest.dist  = src.dist;
	dest.acc   = src.acc;
	switch (src.clearRank) {
	case REC_CLEAR_RANK_DROPED:
		dest.clearType = REC_CLEAR_TYPE2_DROPED;
		break;
	case REC_CLEAR_RANK_CLEARED:
		dest.clearType = REC_CLEAR_TYPE2_CLEARED;
		break;
	case REC_CLEAR_RANK_NOMISS:
		dest.clearType = REC_CLEAR_TYPE2_NOMISS;
		break;
	case REC_CLEAR_RANK_FULLCOMBO:
		dest.clearType = REC_CLEAR_TYPE2_FULLCOMBO;
		break;
	case REC_CLEAR_RANK_PERFECT:
		dest.clearType = REC_CLEAR_TYPE2_PERFECT;
		break;
	default:
		dest.clearType = REC_CLEAR_TYPE2_NO_PLAY;
		break;
	}
	switch (src.scoreRate) {
	case REC_SCORE_RATE_EX:
		dest.scoreRate = REC_SCORE_RATE2_X;
		break;
	case REC_SCORE_RATE_S:
		dest.scoreRate = REC_SCORE_RATE2_S;
		break;
	case REC_SCORE_RATE_A:
		dest.scoreRate = REC_SCORE_RATE2_A;
		break;
	case REC_SCORE_RATE_B:
		dest.scoreRate = REC_SCORE_RATE2_B;
		break;
	case REC_SCORE_RATE_C:
		dest.scoreRate = REC_SCORE_RATE2_C;
		break;
	case REC_SCORE_RATE_D:
		dest.scoreRate = REC_SCORE_RATE2_D;
		break;
	case REC_SCORE_RATE_F:
		dest.scoreRate = REC_SCORE_RATE2_F;
		break;
	default:
		dest.scoreRate = REC_SCORE_RATE2_NO_PLAY;
		break;
	}
}

static void RecSaveConvSc2ToSc(rec_save_score_t &dest, const rec_save_score2_st &src) {
	dest.score = src.score;
	dest.dist  = src.dist;
	dest.acc   = src.acc;
	switch (src.clearType) {
	case REC_CLEAR_TYPE2_DROPED:
		dest.clearRank = REC_CLEAR_RANK_DROPED;
		break;
	case REC_CLEAR_TYPE2_CLEARED:
	case REC_CLEAR_TYPE2_COZYCLEAR:
	case REC_CLEAR_TYPE2_MISSLESS:
		dest.clearRank = REC_CLEAR_RANK_CLEARED;
		break;
	case REC_CLEAR_TYPE2_NOMISS:
		dest.clearRank = REC_CLEAR_RANK_NOMISS;
		break;
	case REC_CLEAR_TYPE2_FULLCOMBO:
		dest.clearRank = REC_CLEAR_RANK_FULLCOMBO;
		break;
	case REC_CLEAR_TYPE2_PERFECT:
	case REC_CLEAR_TYPE2_FULLPERFECT:
		dest.clearRank = REC_CLEAR_RANK_PERFECT;
		break;
	default:
		dest.clearRank = REC_CLEAR_RANK_NO_PLAY;
		break;
	}
	switch (src.scoreRate) {
	case REC_SCORE_RATE2_X:
		dest.scoreRate = REC_SCORE_RATE_EX;
		break;
	case REC_SCORE_RATE2_S:
		dest.scoreRate = REC_SCORE_RATE_S;
		break;
	case REC_SCORE_RATE2_A:
		dest.scoreRate = REC_SCORE_RATE_A;
		break;
	case REC_SCORE_RATE2_B:
		dest.scoreRate = REC_SCORE_RATE_B;
		break;
	case REC_SCORE_RATE2_C:
		dest.scoreRate = REC_SCORE_RATE_C;
		break;
	case REC_SCORE_RATE2_D:
		dest.scoreRate = REC_SCORE_RATE_D;
		break;
	case REC_SCORE_RATE2_F:
		dest.scoreRate = REC_SCORE_RATE_F;
		break;
	default:
		dest.scoreRate = REC_SCORE_RATE_NO_PLAY;
		break;
	}
}

#if 1 /* play score 2 */

rec_save_error_et RecSaveReadScore2AllDif(rec_save_score2_st (&dest)[6], const tstring &songname) {
	tstring filename = _T("score2/");
	FILE *fp;

	filename += songname;
	filename += _T(".dat");

	if (_wfopen_s(&fp, filename.c_str(), _T("rb")) != 0) { return REC_SAVE_ERROR_OPEN; }
	if (fp == NULL) { return REC_SAVE_ERROR_OPEN; }
	if (fread(dest, sizeof(rec_save_score2_st), 6, fp) != 6) {
		fclose(fp);
		return REC_SAVE_ERROR_READ;
	}
	if (fclose(fp) != 0) {
		return REC_SAVE_ERROR_CLOSE;
	}

	return REC_SAVE_ERROR_NONE;
}

rec_save_error_et RecSaveWriteScore2AllDif(const rec_save_score2_st (&src)[6], const tstring &songname) {
	tstring filename = _T("score2/");
	FILE *fp;

	filename += songname;
	filename += _T(".dat");

	if (_wfopen_s(&fp, filename.c_str(), L"wb") != 0) { return REC_SAVE_ERROR_OPEN; }
	if (fp == NULL) { return REC_SAVE_ERROR_OPEN; }
	if (fwrite(src, sizeof(rec_save_score2_st), 6, fp)) {
		fclose(fp);
		return REC_SAVE_ERROR_WRITE;
	}
	if (fclose(fp) != 0) {
		return REC_SAVE_ERROR_CLOSE;
	}

	return REC_SAVE_ERROR_NONE;
}

rec_save_error_et RecSaveReadScore2OneDif(rec_save_score2_st &dest, const tstring &songname, rec_dif_t dif) {
	rec_save_score2_st buf[6];
	rec_save_error_et err;
	err = RecSaveReadScore2AllDif(buf, songname);
	dest = buf[dif];
	return err;
}

rec_save_error_et RecSaveWriteScore2OneDif(const rec_save_score2_st &src, const tstring &songname, rec_dif_t dif) {
	rec_save_score2_st buf[6];
	rec_save_error_et err;
	err = RecSaveReadScore2AllDif(buf, songname);
	if (err != REC_SAVE_ERROR_NONE && err != REC_SAVE_ERROR_OPEN) {
		return err;
	}
	buf[dif] = src;
	return RecSaveWriteScore2AllDif(buf, songname);
}

rec_save_error_et RecSaveUpdateScore2OneDif(const rec_save_score2_st &src, const tstring &songname, rec_dif_t dif) {
	rec_save_score2_st buf;
	rec_save_error_et err;
	err = RecSaveReadScore2OneDif(buf, songname, dif);
	if (err != REC_SAVE_ERROR_NONE && err != REC_SAVE_ERROR_OPEN) {
		return err;
	}

#define cap(name) if (buf.name < src.name) { buf.name = src.name; }
#define dat(name) if (buf.name > src.name) { buf.name = src.name; }
	cap(score);
	cap(dist);
	cap(acc);
	cap(clearType);
	cap(scoreRate);
	dat(good);
	dat(safe);
	dat(miss);
#undef dat
#undef cap

	return RecSaveWriteScore2OneDif(buf, songname, dif);
}

#endif /* play score 2 */

#if 1 /* play score */

int RecSaveReadScoreAllDif(rec_save_score_t dest[], const TCHAR *songname) {
	/* 新方式でトライ */
	rec_save_score2_st data[6];
	if (RecSaveReadScore2AllDif(data, songname) == REC_SAVE_ERROR_NONE) {
		for (size_t idif = 0; idif < 6; idif++) {
			RecSaveConvSc2ToSc(dest[idif], data[idif]);
		}
		return 0;
	}

	/* 旧方式でトライ */
	int	score[6]     = { 0,0,0,0,0,0 };
	int	dist[6]      = { 0,0,0,0,0,0 };
	int	scoreRate[6] = { 6,6,6,6,6,6 };
	int	clearRank[6] = { 0,0,0,0,0,0 };
	double acc[6]    = { 0,0,0,0,0,0 };
	TCHAR filename[255] = L"score/";
	FILE *fp;

	strcats(filename, songname); // save = score/<曲名>
	strcats(filename, L".dat");  // save = score/<曲名>.dat

	_wfopen_s(&fp, filename, L"rb");
	if (fp == NULL) { return -1; }
	fread(&score,     sizeof(int),    6, fp);
	fread(&acc,       sizeof(double), 6, fp);
	fread(&dist,      sizeof(int),    6, fp);
	fread(&scoreRate, sizeof(int),    6, fp);
	fread(&clearRank, sizeof(int),    6, fp);
	fclose(fp);

	for (uint iDif = 0; iDif < 6; iDif++) {
		dest[iDif].score     = score[iDif];
		dest[iDif].acc       = acc[iDif];
		dest[iDif].dist      = dist[iDif];
		dest[iDif].scoreRate = (rec_score_rate_t)scoreRate[iDif];
		dest[iDif].clearRank = (rec_clear_rank_t)clearRank[iDif];
	}

	return 0;
}

/* 同じ曲名があったら上書きしてしまう */
int RecSaveWriteScoreAllDif(const rec_save_score_t src[], const TCHAR *songname) {
	/* 新方式でトライ */
	rec_save_score2_st data[6];
	for (size_t idif = 0; idif < 6; idif++) {
		RecSaveConvScToSc2(data[idif], src[idif]);
	}
	if (RecSaveWriteScore2AllDif(data, songname) == REC_SAVE_ERROR_NONE) { return 0; }

	/* 旧方式でトライ */
	int	score[6]     = { 0,0,0,0,0,0 };
	int	dist[6]      = { 0,0,0,0,0,0 };
	int	scoreRate[6] = { 6,6,6,6,6,6 };
	int	clearRank[6] = { 0,0,0,0,0,0 };
	double acc[6]    = { 0,0,0,0,0,0 };
	TCHAR filename[255] = L"score/";
	FILE *fp;

	for (uint iDif = 0; iDif < 6; iDif++) {
		score[iDif]     = src[iDif].score;
		acc[iDif]       = src[iDif].acc;
		dist[iDif]      = src[iDif].dist;
		scoreRate[iDif] = src[iDif].scoreRate;
		clearRank[iDif] = src[iDif].clearRank;
	}

	strcats(filename, songname); // save = score/<曲名>
	strcats(filename, L".dat");  // save = score/<曲名>.dat

	_wfopen_s(&fp, filename, L"wb");
	if (fp == NULL) { return -1; }
	fwrite(&score,     sizeof(int),    6, fp);
	fwrite(&acc,       sizeof(double), 6, fp);
	fwrite(&dist,      sizeof(int),    6, fp);
	fwrite(&scoreRate, sizeof(int),    6, fp);
	fwrite(&clearRank, sizeof(int),    6, fp);
	fclose(fp);

	return 0;
}

int RecSaveReadScoreOneDif(rec_save_score_t *dest, const TCHAR *songname, rec_dif_t dif) {
	rec_save_score_t buf[6];
	RecSaveReadScoreAllDif(buf, songname);
	*dest = buf[dif];
	return 0;
}

int RecSaveWriteScoreOneDif(const rec_save_score_t *src, const TCHAR *songname, rec_dif_t dif) {
	rec_save_score_t buf[6];
	RecSaveReadScoreAllDif(buf, songname);
	buf[dif] = *src;
	return RecSaveWriteScoreAllDif(buf, songname);
}

int RecSaveUpdateScoreOneDif(const rec_save_score_t *src, const TCHAR *songname, rec_dif_t dif) {
	rec_save_score_t buf;
	RecSaveReadScoreOneDif(&buf, songname, dif);

	if (buf.score     < src->score)     { buf.score     = src->score; }
	if (buf.acc       < src->acc)       { buf.acc       = src->acc; }
	if (buf.dist      < src->dist)      { buf.dist      = src->dist; }
	if (buf.clearRank < src->clearRank) { buf.clearRank = src->clearRank; }
	if (buf.scoreRate > src->scoreRate || buf.scoreRate < 0) { buf.scoreRate = src->scoreRate; } /* ナンバリングの都合上、これだけ他と違う実装になってる */

	RecSaveWriteScoreOneDif(&buf, songname, dif);
	return 0;
}

#endif /* play score */

#if 1 /* user play */

int RecSaveReadUserPlay(rec_user_data_t *dest) {
	FILE *fp;

	_wfopen_s(&fp, L"save/data.dat", L"rb");
	if (fp == NULL) { return -1; }
	fread(dest, sizeof(rec_user_data_t), 1, fp);
	fclose(fp);

	return 0;
}

int RecSaveWriteUserPlay(const rec_user_data_t *src) {
	FILE *fp;

	_wfopen_s(&fp, L"save/data.dat", L"wb");
	if (fp == NULL) { return -1; }
	fwrite(src, sizeof(rec_user_data_t), 1, fp);
	fclose(fp);

	return 0;
}

int RecSaveUpdateUserPlay(const rec_play_userpal_t *userpal) {
	rec_user_data_t data;

	RecSaveReadUserPlay(&data);

	data.playCount++;

	switch (JudgeClearRank(userpal)) {
	case REC_CLEAR_RANK_DROPED:
		data.dropCount++;
		break;
	case REC_CLEAR_RANK_CLEARED:
		data.clearCount++;
		break;
	case REC_CLEAR_RANK_NOMISS:
		data.clearCount++;
		data.NMCount++;
		break;
	case REC_CLEAR_RANK_FULLCOMBO:
		data.clearCount++;
		data.NMCount++;
		data.FCCount++;
		break;
	case REC_CLEAR_RANK_PERFECT:
		data.clearCount++;
		data.NMCount++;
		data.FCCount++;
		data.PFcount++;
		break;
	}

	data.mileage += userpal->Dscore.point;

	return RecSaveWriteUserPlay(&data);
}

#endif /* user play */

#if 1 /* chara play */

int RecSaveReadCharaPlay(rec_save_charaplay_t *dest) {
	int chap[3] = { 0,0,0 };
	FILE *fp;

	_wfopen_s(&fp, L"save/chap.dat", L"rb");
	if (fp == NULL) { return -1; }
	fread(chap, sizeof(int), 3, fp);
	fclose(fp);

	dest->picker   = chap[0];
	dest->mapgator = chap[1];
	dest->taylor   = chap[2];

	return 0;
}

int RecSaveWriteCharaPlay(const rec_save_charaplay_t *src) {
	int chap[3] = { 0,0,0 };
	FILE *fp;

	chap[0] = src->picker;
	chap[1] = src->mapgator;
	chap[2] = src->taylor;

	_wfopen_s(&fp, L"save/chap.dat", L"wb");
	if (fp == NULL) { return -1; }
	fwrite(chap, sizeof(int), 3, fp);
	fclose(fp);

	return 0;
}

int RecSaveUpdateCharaPlay(rec_nowchara_t nowChara) {
	rec_save_charaplay_t data;

	RecSaveReadCharaPlay(&data);

	switch (nowChara) {
	case REC_CHARA_PICKER:
		data.picker++;
		break;
	case REC_CHARA_MAPGATOR:
		data.mapgator++;
		break;
	case REC_CHARA_TAYLOR:
		data.taylor++;
		break;
	}

	return RecSaveWriteCharaPlay(&data);
}

#endif /* chara play */

#if 1 /* runner rate */

int RecSaveReadRunnerRate(play_rate_t *dest) {
	FILE *fp;

	_wfopen_s(&fp, RATE_FILE_NAME, L"rb");
	if (fp == NULL) { return -1; }
	fread(dest, sizeof(play_rate_t), RATE_NUM, fp);
	fclose(fp);

	return 0;
}

int RecSaveWriteRunnerRate(const play_rate_t *src) {
	FILE *fp;

	_wfopen_s(&fp, RATE_FILE_NAME, L"wb");
	if (fp == NULL) { return -1; }
	fwrite(src, sizeof(play_rate_t), RATE_NUM, fp);
	fclose(fp);

	return 0;
}

int RecSaveUpdateRunnerRate(const TCHAR *songname, double rate) {
	play_rate_t data[RATE_NUM];
	char num = -1;

	RecSaveReadRunnerRate(data);

	// 同じ曲、または未収録を探す
	for (uint i = 0; i < RATE_NUM; i++) {
		if (strands(songname, data[i].name) ||
			(data[i].name[0] == L'\0' && data[i].num <= 0))
		{
			num = i;
			break;
		}
	}

	// なかったら、一番低いレートを探す
	if (num == -1) {
		num = 0;
		for (uint i = 1; i < RATE_NUM; i++) {
			if (data[num].num > data[i].num) { num = i; }
		}
	}

	if (rate <= data[num].num) { return 0; }

	// レートを更新する
	data[num].num = rate;
	strcopy_2(songname, data[num].name, 64);

	return RecSaveWriteRunnerRate(data);
}

double RecSaveGetFullRunnerRate(void) {
	double ret = 0;
	play_rate_t data[RATE_NUM];

	RecSaveReadRunnerRate(data);

	for (int i = 0; i < RATE_NUM; i++) {
		if (0 <= data[i].num && data[i].num <= RATE_NUM) {
			ret += data[i].num;
		}
	}

	return ret * 10 / RATE_NUM;
}

#endif /* runner rate */
