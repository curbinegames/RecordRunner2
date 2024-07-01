#pragma once

typedef struct rec_option_s {
	int chara      = 0;
	int offset     = 0;
	int SEenable   = 0;
	int backbright = 2;
	int lang       = 0;
	int keydetail  = 0;
	int combopos   = 0;
} rec_option_t;

extern int RecOpenOptionFile(int *data);
extern int RecOpenOptionFile2(rec_option_t *data);
extern int RecWriteOptineFile(int *data);
extern int RecWriteOptineFile2(rec_option_t *data);

extern now_scene_t option();
