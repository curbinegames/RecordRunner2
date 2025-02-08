
#include <stdio.h>
#include <strcur.h>
#include <system.h>
#include <playbox.h>
#include <RecSave.h>

/* ìØÇ∂ã»ñºÇ™Ç†Ç¡ÇΩÇÁè„èëÇ´ÇµÇƒÇµÇ‹Ç§ */
int RecSaveReadScoreAllDif(rec_score_savefile_t dest[], const TCHAR *songname) {
	int	score[6]     = { 0,0,0,0,0,0 };
	int	dist[6]      = { 0,0,0,0,0,0 };
	int	scoreRate[6] = { 6,6,6,6,6,6 };
	int	clearRank[6] = { 0,0,0,0,0,0 };
	double acc[6]    = { 0,0,0,0,0,0 };
	TCHAR filename[255] = L"score/";
	FILE *fp;

	strcats(filename, songname); // save = score/<ã»ñº>
	strcats(filename, L".dat");  // save = score/<ã»ñº>.dat

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

int RecSaveWriteScoreAllDif(const rec_score_savefile_t src[], const TCHAR *songname) {
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

	strcats(filename, songname); // save = score/<ã»ñº>
	strcats(filename, L".dat");  // save = score/<ã»ñº>.dat

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
