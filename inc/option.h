#pragma once

#include "RecSystem.h"

#define LANG_JP 0
#define LANG_EN 1

#define REC_STR_LANG(jp, en) ( (optiondata.lang == LANG_JP) ? (jp) : (en) )

typedef struct rec_option_s {
	int empty_val  = 0;
	int chara      = 0; /* enum‚É‚µ‚½‚¢‚ªA‘€ìƒ‹[ƒv‚ª–Ê“| */
	int offset     = 0;
	int SEenable   = 0;
	int backbright = 2;
	int lang       = 0;
	int keydetail  = 0;
	int combopos   = 0;
	int BGMvolume  = 7; /* 0~10 */
	int SEvolume   = 7; /* 0~10 */
	int lineThick  = 3;
	int barThick   = 3;
} rec_option_t;

extern rec_option_t optiondata;

/* TODO: ŒÄ‚Ñ‚ğ‚Ç‚¤‚É‚©‚µ‚½‚¢ */
extern int RecOpenOptionFile(int *data);
extern int RecOpenOptionFile2(rec_option_t *data);
extern int RecWriteOptineFile(int *data);
extern int RecWriteOptineFile2(rec_option_t *data);

extern void RecOpenOptionFileSystem();

extern now_scene_t option();
