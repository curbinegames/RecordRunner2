
/* base include */
#include <stdio.h>
#include <dirent.h>

/* curbine code include */
#include <dxcur.h>
#include <sancur.h>
#include <strcur.h>

/* rec system include */
#include <option.h>

/* own include */
#include <RecSystem.h>

int SmallFontData = 0;
int LargeFontData = 0;

int RecPlayDebug[3] = { 0,0,0 };

rec_sys_bgm rec_bgm_system_g;

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

rec_system_langstr_c::rec_system_langstr_c(tstring a_jp, tstring a_en) : jp(std::move(a_jp)), en(std::move(a_en)) {}

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

/* TODO: 各呼び元での戻り値チェック */
/**
* packNoからパックフォルダパスを取得する
* @param[out] ret パックフォルダパスの格納先
* @param[in] size retの長さ、配列数で指定
* @param[in] packNo パックナンバー
* @return rec_error_t
*/
rec_error_t RecGetPackFolderPath(TCHAR *ret, size_t size, uint packNo) {
	std::vector<tstring> list;

	strcopy_2(_T("record/"), ret, size); // ret = "record/"

	if (!GetFolderListWchar(list, _T("record"))) { return REC_ERROR_FILE_EXIST; }

	strcats_2(ret, size, list[packNo].c_str()); // ret = "record/<パック名>"
	stradds_2(ret, size, _T('/'));              // ret = "record/<パック名>/"

	return REC_ERROR_NONE;
}

/**
* packNoとsongNoから曲フォルダパスを取得する
* @param[out] ret 曲フォルダパスの格納先
* @param[in] size retの長さ、配列数で指定
* @param[in] packNo パックナンバー
* @param[in] songNo 曲ナンバー
* @return rec_error_t
*/
rec_error_t RecGetMusicFolderPath(TCHAR *ret, size_t size, uint packNo, uint songNo) {
	rec_error_t status = REC_ERROR_NONE;
	tstring packPath;
	std::vector<tstring> list;

	status = RecGetPackFolderPath(ret, size, packNo); // ret = "record/<パック名>/"
	if (status != REC_ERROR_NONE) { return status; }

	packPath = ret;      // packPath = "record/<パック名>/"
	packPath.pop_back(); // packPath = "record/<パック名>"
	if (!GetFolderListWchar(list, packPath)) { return REC_ERROR_FILE_EXIST; }

	strcats_2(ret, size, list[songNo].c_str()); // ret = "record/<パック名>/<曲名>"
	stradds_2(ret, size, _T('/'));              // ret = "record/<パック名>/<曲名>/"

	return REC_ERROR_NONE;
}

/**
* packNoとsongNoから曲フォルダ名を取得する
* @param[out] ret 曲フォルダ名の格納先
* @param[in] size retの長さ、配列数で指定
* @param[in] packNo パックナンバー
* @param[in] songNo 曲ナンバー
* @return rec_error_t
*/
rec_error_t RecGetMusicFolderName(TCHAR *ret, size_t size, uint packNo, uint songNo) {
	rec_error_t status = REC_ERROR_NONE;
	TCHAR packPath[256];
	tstring packPathStr;
	std::vector<tstring> list;

	status = RecGetPackFolderPath(packPath, 256, packNo); // ret = "record/<パック名>/"
	if (status != REC_ERROR_NONE) { return status; }

	packPathStr = packPath; // packPathStr = "record/<パック名>/"
	packPathStr.pop_back(); // packPathStr = "record/<パック名>"
	if (!GetFolderListWchar(list, packPathStr)) { return REC_ERROR_FILE_EXIST; }

	strcopy_2(list[songNo].c_str(), ret, size);

	return REC_ERROR_NONE;
}

/**
* packNoとsongNoとdifNoからマップファイルパスを取得する
* @param[out] ret マップファイルパスの格納先
* @param[in] size retの長さ、配列数で指定
* @param[in] packNo パックナンバー
* @param[in] songNo 曲ナンバー
* @param[in] difNo 難易度ナンバー
* @return rec_error_t
*/
rec_error_t RecGetMusicMapRrsPath(TCHAR *ret, size_t size, uint packNo, uint songNo, rec_dif_t difNo) {
	rec_error_t status = REC_ERROR_NONE;
	status = RecGetMusicFolderPath(ret, size, packNo, songNo);
	if (status != REC_ERROR_NONE) { return status; }
	stradds_2(ret, size, (TCHAR)((int)_T('0') + (int)difNo));
	strcats_2(ret, size, _T(".rrs"));
	return REC_ERROR_NONE;
}

/**
* packNoとsongNoとdifNoからマップテキストファイルパスを取得する
* @param[out] ret マップテキストファイルパスの格納先
* @param[in] size retの長さ、配列数で指定
* @param[in] packNo パックナンバー
* @param[in] songNo 曲ナンバー
* @param[in] difNo 難易度ナンバー
* @return rec_error_t
*/
rec_error_t RecGetMusicMapTxtPath(TCHAR *ret, size_t size, uint packNo, uint songNo, rec_dif_t difNo) {
	rec_error_t status = REC_ERROR_NONE;
	status = RecGetMusicFolderPath(ret, size, packNo, songNo);
	if (status != REC_ERROR_NONE) { return status; }
	stradds_2(ret, size, (TCHAR)((int)_T('0') + (int)difNo));
	strcats_2(ret, size, _T(".txt"));
	return REC_ERROR_NONE;
}
