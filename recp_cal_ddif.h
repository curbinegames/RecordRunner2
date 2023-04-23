
#ifndef RECP_CAL_DDIF_H
#define RECP_CAL_DDIF_H

extern int cal_difkey(int n_time, int b_time, int n_note, int b_note, int
	bb_note, int b_dif);
extern void CalGhostSkip(note_box note1[], note_box note2[], note_box note3[],
	short objNo[]);
extern char CalFindNearNote(note_box note1[], note_box note2[],
	note_box note3[]);
extern int cal_ddif(int num, int const* difkey, int Etime, int noteoff,
	int difsec, int voidtime);

#endif
