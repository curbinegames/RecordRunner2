#pragma once

#include "playbox.h"

extern void AutoAution(int *keya, int *keyb, int *keyc, int *keyu, int *keyd, int *keyl, int *keyr,
#if SWITCH_NOTE_BOX_2 == 1
	note_box_2_t note[], short int objectNG[],
#else
	struct note_box *unote, struct note_box *mnote, struct note_box *lnote,
#endif
	int Ntime);
