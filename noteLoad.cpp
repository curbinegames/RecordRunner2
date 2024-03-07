
#include <stdio.h>
#include "playbox.h"

void noteLoadOld(struct note_box *noteup, struct note_box *notemid,
	struct note_box *notelow, int allnotenum, FILE *fp) {
	note_box_2_t buf;
	struct note_box *insert = &notemid[0];
	int notenum[3] = { 0,0,0 };

	for (int i = 0; i < allnotenum; i++) {
		fread(&buf, sizeof(note_box_2_t), 1, fp);
		switch (buf.lane) {
		case NOTE_LANE_UP:
			insert = &noteup[notenum[0]];
			break;
		case NOTE_LANE_MID:
			insert = &notemid[notenum[1]];
			break;
		case NOTE_LANE_LOW:
			insert = &notelow[notenum[2]];
			break;
		}
		insert->hittime = buf.hittime;
		insert->viewtime = buf.viewtime;
		insert->object = buf.object;
		insert->xpos = buf.xpos;
		insert->ypos = buf.ypos;
		insert->sound = buf.sound;
		insert->melody = buf.melody;
		insert->color = buf.color;
		switch (buf.lane) {
		case NOTE_LANE_UP:
			notenum[0]++;
			break;
		case NOTE_LANE_MID:
			notenum[1]++;
			break;
		case NOTE_LANE_LOW:
			notenum[2]++;
			break;
		}
	}
	return;
}

void noteSaveOld(struct note_box *noteup, struct note_box *notemid,
	struct note_box *notelow, playnum_box *allnum, FILE *fp) {
	note_box_2_t buf;
	struct note_box *insert = &noteup[0];
	int nextnum = 1;

	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < allnum->notenum[j]; i++) {
			switch (j) {
			case 0:
				insert = &noteup[i];
				buf.lane = NOTE_LANE_UP;
				break;
			case 1:
				insert = &notemid[i];
				buf.lane = NOTE_LANE_MID;
				break;
			case 2:
				insert = &notelow[i];
				buf.lane = NOTE_LANE_LOW;
				break;
			}
			buf.hittime = insert->hittime;
			buf.viewtime = insert->viewtime;
			buf.object = insert->object;
			buf.xpos = insert->xpos;
			buf.ypos = insert->ypos;
			buf.sound = insert->sound;
			buf.melody = insert->melody;
			buf.color = insert->color;
			if (i + 1 < allnum->notenum[j]) {
				buf.next = nextnum;
			}
			else {
				buf.next = 5999;
			}
			nextnum++;
			fwrite(&buf, sizeof(note_box_2_t), 1, fp);
		}
	}
	return;
}
