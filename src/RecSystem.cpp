
/* base include */
#include <stdio.h>
#include <dirent.h>

/* curbine code include */
#include <dxcur.h>
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <option.h>
#include <RecWindowRescale.h>

/* own include */
#include <RecSystem.h>

int SmallFontData = 0;
int LargeFontData = 0;

int RecPlayDebug[3] = { 0,0,0 };

rec_sys_bgm rec_bgm_system_g;

#if 1 /* helpbar */

tstring rec_helpbar_c::GetStr(rec_helpbar_type_ec type) const {
	int page = (int)(GetNowCount() / 10000);
	tstring str;
#define cap(name) this->name[page % this->name.size()].get_str()
	switch (type) {
	case rec_helpbar_type_ec::MENU:
		str = cap(str_menu);
		break;
	case rec_helpbar_type_ec::SELECT:
		str = cap(str_select);
		break;
	case rec_helpbar_type_ec::COLLECT:
		str = cap(str_collect);
		break;
	case rec_helpbar_type_ec::COLLECT_ITEM:
		str = cap(str_collect_item);
		break;
	case rec_helpbar_type_ec::COLLECT_STORY:
		str = cap(str_collect_story);
		break;
	case rec_helpbar_type_ec::COLLECT_STORY_READ:
		str = cap(str_collect_story_read);
		break;
	case rec_helpbar_type_ec::OPTION:
		str = cap(str_option);
		break;
	}
#undef cap
	return str;
}

void rec_helpbar_c::DrawHelp(rec_helpbar_type_ec type) const {
	RecRescaleDrawGraph(0, 0, this->pic.handle(), TRUE);
	RecRescaleDrawString(this->draw_strX, this->draw_strY, this->GetStr(type).c_str(), COLOR_WHITE);
}

#endif /* helpbar */

#if 1 /* rec_sys_bgm */

void rec_sys_bgm::SetVolume(int val) {
	s_totalVolume = val * optiondata.BGMvolume / 10;
	ChangeVolumeSoundMem(s_totalVolume, s_bgm);
}

void rec_sys_bgm::SetMem(const tstring &sndPath) {
	if (s_bgmName == sndPath) { return; }
	s_bgmName = sndPath;
	StopSoundMem(s_bgm);
	DeleteSoundMem(s_bgm);
	s_bgm = LoadSoundMem(sndPath.c_str());
	this->SetVolume(255);
}

bool rec_sys_bgm::RecCheckSoundMem(void) const {
	return CheckSoundMem(s_bgm);
}

void rec_sys_bgm::Play(bool force, bool loop, bool top_position) {
	if (!(force) && CheckSoundMem(s_bgm) == 1) { return; }
	if (force) { StopSoundMem(s_bgm); }
	ChangeVolumeSoundMem(s_totalVolume, s_bgm);
	if (loop) {
		PlaySoundMem(s_bgm, DX_PLAYTYPE_LOOP, top_position);
	}
	else {
		PlaySoundMem(s_bgm, DX_PLAYTYPE_BACK, top_position);
	}
}

void rec_sys_bgm::Delete(void) {
	StopSoundMem(s_bgm);
	DeleteSoundMem(s_bgm);
	s_bgmName.clear();
}

void rec_sys_bgm::Stop(void) {
	StopSoundMem(s_bgm);
}

void rec_sys_bgm::SetCurrentPosition(int val) {
	SetCurrentPositionSoundMem(val, s_bgm);
}

#endif /* rec_sys_bgm */

#if 1 /* rec_system_langstr_c */

rec_system_langstr_c::rec_system_langstr_c(void) : jp(_T("")), en(_T("")) {}
rec_system_langstr_c::rec_system_langstr_c(tstring a_jp, tstring a_en) : jp(std::move(a_jp)), en(std::move(a_en)) {}

void rec_system_langstr_c::set_str_jp(const tstring &a_jp) {
	this->jp = std::move(a_jp);
}

void rec_system_langstr_c::set_str_en(const tstring &a_en) {
	this->en = std::move(a_en);
}

const tstring &rec_system_langstr_c::get_str(void) const {
	return (optiondata.lang == 0) ? this->jp : this->en;
}

#endif /* rec_system_langstr_c */

#if 1 /* dirent系 */

bool GetFolderList(std::vector<std::string> &list, const std::string &path) {
	struct dirent *dirs = NULL;
	DIR *dir = NULL;
	list.clear();

	dir = opendir(path.c_str());
	if (dir == NULL) { return false; }

	while (1) {
		dirs = readdir(dir);
		if (dirs == NULL) { break; }
		if (dirs->d_name[0] == '.') { continue; }
		list.push_back(dirs->d_name);
	}
	closedir(dir);

	return true;
}

bool GetFolderListWchar(std::vector<std::wstring> &list, const std::wstring &path) {
	std::vector<std::string> buf;

	int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, path.c_str(), path.size(), NULL, 0, NULL, NULL);
	std::string u8path;
	u8path.resize(sizeNeeded);
	WideCharToMultiByte(CP_UTF8, 0, path.c_str(), path.size(), &u8path[0], sizeNeeded, NULL, NULL);

	if (!GetFolderList(buf, u8path)) { return false; }

	for (size_t i = 0; i < buf.size(); i++) {
		sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, buf[i].c_str(), buf[i].size(), NULL, 0);
		std::wstring wname;
		wname.resize(sizeNeeded);
		MultiByteToWideChar(CP_UTF8, 0, buf[i].c_str(), buf[i].size(), &wname[0], sizeNeeded);
		if (sizeNeeded <= wname.size()) {
			wname[sizeNeeded] = L'\0';
		}
		list.push_back(wname);
	}

	return true;
}

#endif /* dirent系 */

#if 1 /* enum操作系 */

rec_score_rate2_et RecSRateToSRate2(rec_score_rate_t rate) {
	switch (rate) {
	case REC_SCORE_RATE_EX:
		return REC_SCORE_RATE2_X;
	case REC_SCORE_RATE_S:
		return REC_SCORE_RATE2_S;
	case REC_SCORE_RATE_A:
		return REC_SCORE_RATE2_A;
	case REC_SCORE_RATE_B:
		return REC_SCORE_RATE2_B;
	case REC_SCORE_RATE_C:
		return REC_SCORE_RATE2_C;
	case REC_SCORE_RATE_D:
		return REC_SCORE_RATE2_D;
	case REC_SCORE_RATE_F:
		return REC_SCORE_RATE2_F;
	case REC_SCORE_RATE_NO_PLAY:
		return REC_SCORE_RATE2_NO_PLAY;
	default:
		return REC_SCORE_RATE2_NO_PLAY;
	}
}

rec_score_rate_t RecSRate2ToSRate(rec_score_rate2_et rate) {
	switch (rate) {
	case REC_SCORE_RATE2_X:
		return REC_SCORE_RATE_EX;
	case REC_SCORE_RATE2_S:
		return REC_SCORE_RATE_S;
	case REC_SCORE_RATE2_A:
		return REC_SCORE_RATE_A;
	case REC_SCORE_RATE2_B:
		return REC_SCORE_RATE_B;
	case REC_SCORE_RATE2_C:
		return REC_SCORE_RATE_C;
	case REC_SCORE_RATE2_D:
		return REC_SCORE_RATE_D;
	case REC_SCORE_RATE2_F:
		return REC_SCORE_RATE_F;
	case REC_SCORE_RATE2_NO_PLAY:
		return REC_SCORE_RATE_NO_PLAY;
	default:
		return REC_SCORE_RATE_NO_PLAY;
	}
}

rec_clear_type2_et RecCRankToCRank2(rec_clear_rank_t type) {
	switch (type) {
	case REC_CLEAR_RANK_NO_PLAY:
		return REC_CLEAR_TYPE2_NO_PLAY;
	case REC_CLEAR_RANK_DROPED:
		return REC_CLEAR_TYPE2_DROPED;
	case REC_CLEAR_RANK_CLEARED:
		return REC_CLEAR_TYPE2_CLEARED;
	case REC_CLEAR_RANK_NOMISS:
		return REC_CLEAR_TYPE2_NOMISS;
	case REC_CLEAR_RANK_FULLCOMBO:
		return REC_CLEAR_TYPE2_FULLCOMBO;
	case REC_CLEAR_RANK_PERFECT:
		return REC_CLEAR_TYPE2_PERFECT;
	default:
		return REC_CLEAR_TYPE2_NO_PLAY;
	}
}

rec_clear_rank_t RecCRank2ToCRank(rec_clear_type2_et type) {
	switch (type) {
	case REC_CLEAR_TYPE2_NO_PLAY:
		return REC_CLEAR_RANK_NO_PLAY;
	case REC_CLEAR_TYPE2_CRASHED:
	case REC_CLEAR_TYPE2_DROPED:
		return REC_CLEAR_RANK_DROPED;
	case REC_CLEAR_TYPE2_CLEARED:
	case REC_CLEAR_TYPE2_STABLECLEAR:
	case REC_CLEAR_TYPE2_MISSLESS:
		return REC_CLEAR_RANK_CLEARED;
	case REC_CLEAR_TYPE2_NOMISS:
		return REC_CLEAR_RANK_NOMISS;
	case REC_CLEAR_TYPE2_FULLCOMBO:
		return REC_CLEAR_RANK_FULLCOMBO;
	case REC_CLEAR_TYPE2_PERFECT:
	case REC_CLEAR_TYPE2_FULLPERFECT:
		return REC_CLEAR_RANK_PERFECT;
	default:
		return REC_CLEAR_RANK_NO_PLAY;
	}
}

#endif /* enum操作系 */

bool RecGetPackList(std::vector<tstring> &list) {
	list.clear();
	DxFile_t file = FileRead_open(_T("RecordPack.txt"));
	if (file == 0) { return false; }
	while (FileRead_eof(file) == 0) {
		TCHAR buf[256];
		FileRead_gets(buf, 256, file);
		list.push_back(buf);
	}
	FileRead_close(file);
	return true;
}

bool RecGetMusicList(std::vector<tstring> &list, const tstring &packName) {
	list.clear();
	tstring path = _T("record/") + packName + _T("/list.txt");
	DxFile_t file = FileRead_open(path.c_str());
	if (file == 0) { return false; }
	while (FileRead_eof(file) == 0) {
		TCHAR buf[256];
		FileRead_gets(buf, 256, file);
		list.push_back(buf);
	}
	FileRead_close(file);
	return true;
}

/* TODO: 各呼び元での戻り値チェック */
/**
* packNoとsongNoから曲フォルダパスを取得する
* @param[out] ret 曲フォルダパスの格納先
* @param[in] packNo パックナンバー
* @param[in] songNo 曲ナンバー
* @return rec_error_t
*/
rec_error_t RecGetMusicFolderPath(tstring &ret, uint packNo, uint songNo) {
	tstring packName;
	std::vector<tstring> list;

	if (!RecGetPackList(list)) { return REC_ERROR_FILE_EXIST; }
	packName = list[packNo];
	if (!RecGetMusicList(list, packName)) { return REC_ERROR_FILE_EXIST; }

	ret = _T("record/") + packName + _T("/") + list[songNo] + _T("/");

	return REC_ERROR_NONE;
}

/**
* packNoとsongNoから曲フォルダ名を取得する
* @param[out] ret 曲フォルダ名の格納先
* @param[in] packNo パックナンバー
* @param[in] songNo 曲ナンバー
* @return rec_error_t
*/
rec_error_t RecGetMusicFolderName(tstring &ret, uint packNo, uint songNo) {
	std::vector<tstring> list;
	tstring packName;

	if (!RecGetPackList(list)) { return REC_ERROR_FILE_EXIST; }
	packName = list[packNo];
	if (!RecGetMusicList(list, packName)) { return REC_ERROR_FILE_EXIST; }
	ret = list[songNo];

	return REC_ERROR_NONE;
}

/**
* packNoとsongNoとdifNoからマップファイルパスを取得する
* @param[out] ret マップファイルパスの格納先
* @param[in] packNo パックナンバー
* @param[in] songNo 曲ナンバー
* @param[in] difNo 難易度ナンバー
* @return rec_error_t
*/
rec_error_t RecGetMusicMapRrsPath(tstring &ret, uint packNo, uint songNo, rec_dif_t difNo) {
	rec_error_t status = REC_ERROR_NONE;
	status = RecGetMusicFolderPath(ret, packNo, songNo);
	if (status != REC_ERROR_NONE) { return status; }
	ret += (TCHAR)((int)_T('0') + (int)difNo);
	ret += _T(".rrs");
	return REC_ERROR_NONE;
}

/**
* packNoとsongNoとdifNoからマップテキストファイルパスを取得する
* @param[out] ret マップテキストファイルパスの格納先
* @param[in] packNo パックナンバー
* @param[in] songNo 曲ナンバー
* @param[in] difNo 難易度ナンバー
* @return rec_error_t
*/
rec_error_t RecGetMusicMapTxtPath(tstring &ret, uint packNo, uint songNo, rec_dif_t difNo) {
	rec_error_t status = REC_ERROR_NONE;
	status = RecGetMusicFolderPath(ret, packNo, songNo);
	if (status != REC_ERROR_NONE) { return status; }
	ret += (TCHAR)((int)_T('0') + (int)difNo);
	ret += _T(".txt");
	return REC_ERROR_NONE;
}
