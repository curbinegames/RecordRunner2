#pragma once

#include "playbox.h"

extern int cal_difkey(int n_time, int b_time, int n_note, int b_note, int
	bb_note, int b_dif);
extern void CalGhostSkip(note_box note1[], note_box note2[], note_box note3[],
	short objNo[]);
extern char CalFindNearNote(note_box note1[], note_box note2[],
	note_box note3[]);
extern int cal_ddif(int num, int const* difkey, int Etime, int noteoff,
	int difsec, int voidtime);
extern char DdifCatchFix(char lane, int* NdifCom, int* BdifCom);
extern int MapErrorCheck(int nownote, int nowtime, int befnote, int beftime, int dif,
	int wl);
extern char DdifBombFix(char lane, int* NdifCom, int* BdifCom);
extern void CheckDdif(note_box* note, int Etime, int noteoff, ddef_box* ddif2,
	int ddif[], int* Fdif, int voidtime);
#if 0
extern void DdifCal1(char* lane, note_box note1[], note_box note2[], note_box note3[],
	short objNo[], int Etime, int noteoff, ddef_box* ddif2, int ddif[],
	int *Fdif, int voidtime, int Ndif[], int Bdif[], char dif, int waningLv,
	int outpoint[]);
#endif
extern int cal_nowdif_m(int* difkey, int num, int now, int voidtime);
#if 0
extern void DdifCal2(int* NdifNo, int* BdifNo, int* BBdifNo, int Ndif[],
	int Bdif[], int BBdif[], int* lane, note_box note1[], note_box note2[],
	note_box note3[], short objNo[], ddef_box* ddif2, int* Fdif, int voidtime,
	int limitNo);
#endif
#if 0
extern void DdifCal3(note_box note1[], note_box note2[], note_box note3[],
	char* lane, short objNo[]);
#endif
extern void DdifCal4(short objNo[], char* lane, ddef_box* ddif2, int* Fdif,
	int limitNo, int* NdifNo, int* BdifNo, int* BBdifNo, int voidtime);
extern void DdifCal5(int Ndif[], int Bdif[], note_box* note, char dif,
	int waningLv, int outpoint[]);
extern char DdifNoteFix(int Ndif[], int Bdif[], short objNo[], char lane);
