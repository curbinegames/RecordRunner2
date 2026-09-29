#pragma once

#include <stdcur.h>

typedef struct rec_mapenc_basedata_s {
    int note_offset = 0;
    int level = -1;
    int preview[2] = { 44100 * 10, 44100 * 60 };
    double bpm = 120;
    rec_system_langstr_c music_name;
    rec_system_langstr_c artist_name;
    tstring music_path  = _T("");
    tstring sky_path    = _T("");
    tstring jacket_path = _T("");
    tstring field_path  = _T("");
    tstring water_path  = _T("");
    tstring difbar_path = _T("");
} rec_mapenc_basedata_st;

extern rec_error_t RecMapencGetBaseData(rec_mapenc_basedata_st &dest, const tstring &path);

extern rec_error_t RecordLoad2(int packNo, int songNo, int difNo);
