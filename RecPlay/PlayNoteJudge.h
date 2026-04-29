#pragma once

#include <queue>
#include <RecScoreFile.h>
#include <playbox.h>

typedef struct rec_judge_event_s {
	note_material mat = NOTE_NONE;
	int gaptime = 0;
	note_box_2_t noteinfo;
	note_judge judge;
	int lineNo;
} rec_judge_event_st;

class rec_play_sound_c {
private:
	dxcur_snd_c att{_T("sound/attack.wav")};
	dxcur_snd_c cat{_T("sound/catch.wav")};
	dxcur_snd_c arw{_T("sound/arrow.wav")};
	dxcur_snd_c bom{_T("sound/bomb.wav")};
	dxcur_snd_c swi{_T("sound/swing.wav")};

public:
	rec_play_sound_c(void);
	void PlayNoteSound(note_material mat);
	void PlaySwing(void);
};

class rec_play_snditem_def_c {
private:
	dxcur_snd_c snd[24] = {
		dxcur_snd_c(_T("sound/melody/lowF.wav")),
		dxcur_snd_c(_T("sound/melody/lowF#.wav")),
		dxcur_snd_c(_T("sound/melody/lowG.wav")),
		dxcur_snd_c(_T("sound/melody/lowG#.wav")),
		dxcur_snd_c(_T("sound/melody/lowA.wav")),
		dxcur_snd_c(_T("sound/melody/lowA#.wav")),
		dxcur_snd_c(_T("sound/melody/lowB.wav")),
		dxcur_snd_c(_T("sound/melody/lowC.wav")),
		dxcur_snd_c(_T("sound/melody/lowC#.wav")),
		dxcur_snd_c(_T("sound/melody/lowD.wav")),
		dxcur_snd_c(_T("sound/melody/lowD#.wav")),
		dxcur_snd_c(_T("sound/melody/lowE.wav")),
		dxcur_snd_c(_T("sound/melody/highF.wav")),
		dxcur_snd_c(_T("sound/melody/highF#.wav")),
		dxcur_snd_c(_T("sound/melody/highG.wav")),
		dxcur_snd_c(_T("sound/melody/highG#.wav")),
		dxcur_snd_c(_T("sound/melody/highA.wav")),
		dxcur_snd_c(_T("sound/melody/highA#.wav")),
		dxcur_snd_c(_T("sound/melody/highB.wav")),
		dxcur_snd_c(_T("sound/melody/highC.wav")),
		dxcur_snd_c(_T("sound/melody/highC#.wav")),
		dxcur_snd_c(_T("sound/melody/highD.wav")),
		dxcur_snd_c(_T("sound/melody/highD#.wav")),
		dxcur_snd_c(_T("sound/melody/highE.wav"))
	};

public:
	rec_play_snditem_def_c(void);

	void PlaySoundDataBase(
		const note_box_2_t &note, rec_play_sound_c &p_sound, const std::vector<DxSnd_t> &soundItem
	);

	void PlaySoundData(
		note_judge judge, const note_box_2_t &note, rec_play_sound_c &p_sound,
		const std::vector<DxSnd_t> &soundItem
	);
};

class rec_play_sitem_c {
private:
	std::vector<DxSnd_t> vec;

public:
	rec_play_sitem_c(void) = delete;
	rec_play_sitem_c(const tstring &folderPath);
	~rec_play_sitem_c();
	void Getxxxwav(tstring &str, int num);
	const std::vector<DxSnd_t> &GetSitemList(void) const;
};

class rec_play_snditem_all_c {
private:
	rec_play_sound_c main;
	rec_play_snditem_def_c sub;
	rec_play_sitem_c user;

public:
	rec_play_snditem_all_c(const tstring &folderPath) : user(folderPath) {}

	void PlaySwing(void) {
		this->main.PlaySwing();
	}

	void PlaySoundDataBase(const note_box_2_t &noteinfo) {
		this->sub.PlaySoundDataBase(noteinfo, this->main, this->user.GetSitemList());
	}

	void PlaySoundData(note_judge judge, const note_box_2_t &noteinfo) {
		this->sub.PlaySoundData(judge, noteinfo, this->main, this->user.GetSitemList());
	}
};

/* DOTO: Œø‰Ê‰¹Œn‚ð•Êƒtƒ@ƒCƒ‹‚ÉˆÚ“®‚·‚é */
extern void RecPlayInitMelodySnd();
extern void RecJudgeAllNotes(
	std::queue<rec_judge_event_st> &event_queue, cvec<note_box_2_t> note[], int Ntime,
	rec_play_key_hold_t *keyhold, rec_hitatk_event_ec &hitatk, int LaneTrack[], int *charahit,
	short charaput, rec_play_userpal_t *userpal, rec_play_snditem_all_c &rec_snd
);
