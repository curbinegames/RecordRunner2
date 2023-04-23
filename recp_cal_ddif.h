#pragma once

typedef struct ddif_box_s ddif_box_t;
struct ddif_box_s {
	int time = 0;
	int note = 0;
	int dif = 0;
	ddif_box_t* bef = NULL;
	ddif_box_t* bbef = NULL;
};
typedef ddif_box_t difkeyData_t;
typedef struct difkey_s {
	difkeyData_t difkey[50];
	int datalim;
	int nowNo;
	int befNo;
	int bbefNo;
	int maxdif;
	int datanum;
	int lastsum;
	int avoidtime;
} difkey_t;

extern int cal_difkey(int n_time, int b_time, int n_note, int b_note, int
	bb_note, int b_dif);

