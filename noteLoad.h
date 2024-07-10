#pragma once

#include <stdio.h>
#include "RecPlay/playbox.h"

extern void noteLoadOld(struct note_box *noteup, struct note_box *notemid,
	struct note_box *notelow, int allnotenum, FILE *fp);
void noteSaveOld(struct note_box *noteup, struct note_box *notemid,
	struct note_box *notelow, playnum_box *allnum, FILE *fp);
