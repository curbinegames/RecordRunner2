#pragma once

#include "playbox.h"

extern void CalGhostSkip(note_box note1[], note_box note2[], note_box note3[],
	short objNo[]);
extern char DdifNoteFix(int Ndif[], int Bdif[], short objNo[], char lane);
extern void DdifCal1(char* lane, note_box note1[], note_box note2[],
	note_box note3[], int Etime, int noteoff, ddef_box* ddif2, int ddif[],
	int* Fdif, int voidtime, int Ndif[], int Bdif[], char dif, int waningLv,
	int outpoint[]);
extern void DdifCal2(int* NdifNo, int* BdifNo, int* BBdifNo, int Ndif[],
	int Bdif[], int BBdif[], int* lane, note_box note1[], note_box note2[],
	note_box note3[], short objNo[], ddef_box* ddif2, int* Fdif, int voidtime,
	int limitNo);
