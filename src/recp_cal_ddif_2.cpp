
#include <strcur.h>
#include <sancur.h>
#include <playbox.h>
#include <RecScoreFile.h>

#if 1 /* define */

/* numがtarget±gap以内であればtrueを返すdefine */
#define IS_NEAR_NUM(num, target, gap) (((target) - (gap)) <= (num) && (num) <= ((target) + (gap)))

/* いずれかのレーンに対象のノーツがあればtrueを返すdefine */
#define IS_NOTE_MAT_ANYLANE(note, mat) ((note)[0] == (mat) || (note)[1] == (mat) || (note)[2] == (mat))
#define IS_NOTE_UP_ANYLANE(note)    IS_NOTE_MAT_ANYLANE(note, NOTE_UP)
#define IS_NOTE_DOWN_ANYLANE(note)  IS_NOTE_MAT_ANYLANE(note, NOTE_DOWN)
#define IS_NOTE_LEFT_ANYLANE(note)  IS_NOTE_MAT_ANYLANE(note, NOTE_LEFT)
#define IS_NOTE_RIGHT_ANYLANE(note) IS_NOTE_MAT_ANYLANE(note, NOTE_RIGHT)

#define REC_DDIF_BUF_NUM 50 //ddif計算で使用するバッファの数
#define REC_DDIF_GROUP_TIME 40 //まとまりと見なす時間[ms]

#define REC_FREE 0 //何でもいい
#define REC_PUSH 1 //押す
#define REC_HOLD 2 //押しっぱなし
#define REC_NONE 3 //離しっぱなし
#define REC_RELA 4 //離す

#define REC_DDIF_BASE 990 // ddifの基準難易度

// それぞれ、Lv9以下の楽曲で9.9を上回らないように、うまい具合に設定している
// Lv10? 振り切れてなんぼよ
#define REC_DDIF_NOTES_BASE  354
#define REC_DDIF_ARROW_BASE 3757
#define REC_DDIF_CHORD_BASE 3319
#define REC_DDIF_CHAIN_BASE 2102
#define REC_DDIF_TRILL_BASE 3420
#define REC_DDIF_MELDY_BASE  289
#define REC_DDIF_ACTOR_BASE 2561
#define REC_DDIF_TRICK_BASE 2194

#define REC_DDIF_1ST_WAIGHT  50
#define REC_DDIF_2ND_WAIGHT  90
#define REC_DDIF_3RD_WAIGHT 100
#define REC_DDIF_4TH_WAIGHT  90
#define REC_DDIF_5TH_WAIGHT  75
#define REC_DDIF_6TH_WAIGHT  50
#define REC_DDIF_7TH_WAIGHT  25
#define REC_DDIF_8TH_WAIGHT  10

#endif /* define */

typedef struct ddef_box_2 {
	int maxdif = 0;
	int lastdif = 0;
	int nowdifsection = 1;
	int datanum = -1;
} ddef_box_2;

class rec_ddif_btn_c {
private:
	uint btn = REC_FREE;

public:
	uint GetVal() const {
		return this->btn;
	}

	void Reset() {
		this->btn = REC_FREE;
	}

	void SetPush() {
		switch (this->btn) {
		case REC_FREE:
		case REC_HOLD:
			this->btn = REC_PUSH;
			break;
		case REC_PUSH:
		case REC_NONE:
		case REC_RELA:
			break;
		}
	}

	void SetHold() {
		switch (this->btn) {
		case REC_FREE:
			this->btn = REC_HOLD;
			break;
		case REC_PUSH:
		case REC_HOLD:
		case REC_NONE:
		case REC_RELA:
			break;
		}
	}

	void SetNone() {
		switch (this->btn) {
		case REC_FREE:
			this->btn = REC_NONE;
			break;
		case REC_PUSH:
		case REC_HOLD:
		case REC_NONE:
		case REC_RELA:
			break;
		}
	}

	void SetRelease() {
		switch (this->btn) {
		case REC_FREE:
		case REC_NONE:
			this->btn = REC_RELA;
			break;
		case REC_PUSH:
		case REC_HOLD:
		case REC_RELA:
			break;
		}
	}

	bool CheckPushGroup() const {
		return this->btn == REC_PUSH || this->btn == REC_HOLD;
	}

	bool CheckReleaseGroup() const {
		return this->btn == REC_FREE || this->btn == REC_RELA || this->btn == REC_NONE;
	}
};

class rec_ddif_btn_set_c {
private:

public:
	rec_ddif_btn_c z;
	rec_ddif_btn_c x;
	rec_ddif_btn_c c;
	rec_ddif_btn_c u;
	rec_ddif_btn_c d;
	rec_ddif_btn_c l;
	rec_ddif_btn_c r;

	bool operator ==(const rec_ddif_btn_set_c &r) const {
		if (this->z.CheckPushGroup() != r.z.CheckPushGroup()) { return false; }
		if (this->z.CheckReleaseGroup() != r.z.CheckReleaseGroup()) { return false; }
		if (this->x.CheckPushGroup() != r.x.CheckPushGroup()) { return false; }
		if (this->x.CheckReleaseGroup() != r.x.CheckReleaseGroup()) { return false; }
		if (this->c.CheckPushGroup() != r.c.CheckPushGroup()) { return false; }
		if (this->c.CheckReleaseGroup() != r.c.CheckReleaseGroup()) { return false; }
		if (this->u.CheckPushGroup() != r.u.CheckPushGroup()) { return false; }
		if (this->u.CheckReleaseGroup() != r.u.CheckReleaseGroup()) { return false; }
		if (this->d.CheckPushGroup() != r.d.CheckPushGroup()) { return false; }
		if (this->d.CheckReleaseGroup() != r.d.CheckReleaseGroup()) { return false; }
		if (this->l.CheckPushGroup() != r.l.CheckPushGroup()) { return false; }
		if (this->l.CheckReleaseGroup() != r.l.CheckReleaseGroup()) { return false; }
		if (this->r.CheckPushGroup() != r.r.CheckPushGroup()) { return false; }
		if (this->r.CheckReleaseGroup() != r.r.CheckReleaseGroup()) { return false; }
		return true;
	}

	bool operator !=(const rec_ddif_btn_set_c &r) const {
		return !(*this == r);
	}
};

typedef struct rec_ddif_data_s {
	DxTime_t time = 0;
	note_material note[3] = { NOTE_NONE,NOTE_NONE,NOTE_NONE };
	uint hitN = 0;
	uint arwN = 0;
	rec_ddif_btn_set_c btn;
	rec_ddif_pal_t pal;
} rec_ddif_data_t;

#define qsort_ease(type, mat) qsort((mat), sizeof(mat) / sizeof(type), sizeof(type), qsort_protocol<type>)

template<typename T>
static int qsort_protocol(const void *n1, const void *n2) {
	if (*(T *)n1 > *(T *)n2) { return -1; }
	else if (*(T *)n1 < *(T *)n2) { return 1; }
	else { return 0; }
}

#if 1 /* RecDdifGetKey */

static void RecDdifGetKeyHit(rec_ddif_data_t *Nowkey, rec_ddif_data_t *Befkey) {
	switch (Nowkey->hitN) {
	case 1:
		if (Befkey->btn.c.GetVal() == REC_PUSH) {
			Nowkey->btn.z.SetPush();
		}
		else {
			Nowkey->btn.c.SetPush();
		}
		break;
	case 2:
		Nowkey->btn.x.SetPush();
		if (Befkey->btn.c.GetVal() == REC_PUSH) {
			Nowkey->btn.z.SetPush();
		}
		else {
			Nowkey->btn.c.SetPush();
		}
		break;
	case 3:
		Nowkey->btn.z.SetPush();
		Nowkey->btn.x.SetPush();
		Nowkey->btn.c.SetPush();
		break;
	}
	return;
}

static void RecDdifGetKeyArrow(rec_ddif_data_t *Nowkey) {
	if (IS_NOTE_UP_ANYLANE(Nowkey->note)) { Nowkey->btn.u.SetPush(); }
	if (IS_NOTE_DOWN_ANYLANE(Nowkey->note)) { Nowkey->btn.d.SetPush(); }
	if (IS_NOTE_LEFT_ANYLANE(Nowkey->note)) { Nowkey->btn.l.SetPush(); }
	if (IS_NOTE_RIGHT_ANYLANE(Nowkey->note)) { Nowkey->btn.r.SetPush(); }
	return;
}

static void RecDdifGetKeyCatch(rec_ddif_data_t *Nowkey, rec_ddif_data_t *Befkey) {
	if (Befkey->btn.u.CheckPushGroup()) { // 前で上にいる
		if (Nowkey->note[2] == NOTE_CATCH) {
			Nowkey->btn.u.SetNone();
			Nowkey->btn.d.SetHold();
		}
		else if (Nowkey->note[1] == NOTE_CATCH) { Nowkey->btn.u.SetNone(); }
		else if (Nowkey->note[0] == NOTE_CATCH) { Nowkey->btn.u.SetHold(); }
	}
	else if (Befkey->btn.d.CheckPushGroup()) { // 前で下にいる
		if (Nowkey->note[0] == NOTE_CATCH) {
			Nowkey->btn.u.SetHold();
			Nowkey->btn.d.SetNone();
		}
		else if (Nowkey->note[1] == NOTE_CATCH) { Nowkey->btn.d.SetNone(); }
		else if (Nowkey->note[2] == NOTE_CATCH) { Nowkey->btn.d.SetHold(); }
	}
	else { // 前で中にいる
		if (Nowkey->note[0] == NOTE_CATCH) { Nowkey->btn.u.SetHold(); }
		if (Nowkey->note[2] == NOTE_CATCH) { Nowkey->btn.d.SetHold(); }
	}
	return;
}

static void RecDdifGetKeyBomb(rec_ddif_data_t *Nowkey, rec_ddif_data_t *Befkey) {
	Nowkey->pal.trick = 0;
	if (Befkey->btn.u.CheckPushGroup()) { // 前で上にいる
		if (Nowkey->note[0] == NOTE_BOMB) {
			if (Nowkey->note[1] == NOTE_BOMB) {
				Nowkey->btn.u.SetNone();
				Nowkey->btn.d.SetHold();
			}
			else if (Befkey->btn.u.CheckPushGroup()) {
				Nowkey->btn.d.SetHold();
			}
			else {
				Nowkey->btn.u.SetNone();
			}
		}
	}
	else if (Befkey->btn.d.CheckPushGroup()) { // 前で下にいる
		if (Nowkey->note[2] == NOTE_BOMB) {
			if (Nowkey->note[1] == NOTE_BOMB) {
				Nowkey->btn.u.SetHold();
				Nowkey->btn.d.SetNone();
			}
			else if (Befkey->btn.d.CheckPushGroup()) {
				Nowkey->btn.u.SetHold();
			}
			else {
				Nowkey->btn.d.SetNone();
			}
		}
	}
	else { // 前で中にいる
		if (Nowkey->note[1] == NOTE_BOMB) {
			if (Nowkey->note[0] == NOTE_BOMB) {
				Nowkey->btn.d.SetHold();
			}
			else {
				Nowkey->btn.u.SetHold();
			}
		}
	}
	return;
}

#endif /* RecDdifGetKey */

#if 1 /* RecDdifGetPal */

/**
 * hit/arw
 *    0  1  2   3
 * 0  0  0 15 (18)
 * 1  0 12 17  --
 * 2 10 13 --  --
 * 3 12 -- --  --
 */
static void RecDdifGetPalChord(rec_ddif_data_t *Nowkey) {
	Nowkey->pal.chord = 0;
	switch (Nowkey->arwN) {
	case 0:
		switch (Nowkey->hitN) {
		case 2:
			Nowkey->pal.chord = 10;
			break;
		case 3:
			Nowkey->pal.chord = 12;
			break;
		}
		break;
	case 1:
		switch (Nowkey->hitN) {
		case 1:
			Nowkey->pal.chord = 12;
			break;
		case 2:
			Nowkey->pal.chord = 13;
			break;
		}
		break;
	case 2:
		switch (Nowkey->hitN) {
		case 0:
			Nowkey->pal.chord = 15;
			break;
		case 1:
			Nowkey->pal.chord = 17;
			break;
		}
		break;
	case 3:
		Nowkey->pal.chord = 19;
		break;
	}
	return;
}

static void RecDdifGetPalChain(rec_ddif_data_t *Nowkey, rec_ddif_data_t *Befkey, rec_ddif_data_t *BBefkey) {
	uint divN = 0; // 2連カウント
	uint chainN = 0; // 縦連カウント
	Nowkey->pal.chain = 0;
	if (Nowkey->hitN == 0 && Nowkey->arwN == 0) { return; }
	if (Nowkey->btn.z.CheckPushGroup() &&
		Befkey->btn.z.CheckPushGroup())
	{
		divN++;
		if (BBefkey->btn.z.CheckPushGroup()) { chainN++; }
	}
	if (Nowkey->btn.x.CheckPushGroup() &&
		Befkey->btn.x.CheckPushGroup())
	{
		divN++;
		if (BBefkey->btn.x.CheckPushGroup()) { chainN++; }
	}
	if (Nowkey->btn.c.CheckPushGroup() &&
		Befkey->btn.c.CheckPushGroup())
	{
		divN++;
		if (BBefkey->btn.c.CheckPushGroup()) { chainN++; }
	}
	if (Nowkey->btn.u.CheckPushGroup() &&
		Befkey->btn.u.CheckPushGroup())
	{
		divN++;
		if (BBefkey->btn.u.CheckPushGroup()) { chainN++; }
	}
	if (Nowkey->btn.d.CheckPushGroup() &&
		Befkey->btn.d.CheckPushGroup())
	{
		divN++;
		if (BBefkey->btn.d.CheckPushGroup()) { chainN++; }
	}
	if (Nowkey->btn.l.CheckPushGroup() &&
		Befkey->btn.l.CheckPushGroup())
	{
		divN++;
		if (BBefkey->btn.l.CheckPushGroup()) { chainN++; }
	}
	if (Nowkey->btn.r.CheckPushGroup() &&
		Befkey->btn.r.CheckPushGroup())
	{
		divN++;
		if (BBefkey->btn.r.CheckPushGroup()) { chainN++; }
	}

	if (0 < chainN) {
		Nowkey->pal.chain += 12 + max(chainN * 1.5, divN);
	}
	else if (0 < divN) {
		Nowkey->pal.chain = 9 + divN;
	}
	else {
		Nowkey->pal.chain = 0;
	}

	return;
}

static void RecDdifGetPalTrill(rec_ddif_data_t *Nowkey, rec_ddif_data_t *Befkey, rec_ddif_data_t *BBefkey) {
	uint count = 0; // トリルカウント
	Nowkey->pal.trill = 0;
	if (Nowkey->hitN == 0 && Nowkey->arwN == 0) { return; }
	if (Nowkey->btn.z.CheckPushGroup() &&
		BBefkey->btn.z.CheckPushGroup() &&
		Befkey->btn.z.CheckReleaseGroup())
	{
		count++;
	}
	if (Nowkey->btn.x.CheckPushGroup() &&
		BBefkey->btn.x.CheckPushGroup() &&
		Befkey->btn.x.CheckReleaseGroup())
	{
		count++;
	}
	if (Nowkey->btn.c.CheckPushGroup() &&
		BBefkey->btn.c.CheckPushGroup() &&
		Befkey->btn.c.CheckReleaseGroup())
	{
		count++;
	}
	if (Nowkey->btn.u.CheckPushGroup() &&
		BBefkey->btn.u.CheckPushGroup() &&
		Befkey->btn.u.CheckReleaseGroup())
	{
		count++;
	}
	if (Nowkey->btn.d.CheckPushGroup() &&
		BBefkey->btn.d.CheckPushGroup() &&
		Befkey->btn.d.CheckReleaseGroup())
	{
		count++;
	}
	if (Nowkey->btn.l.CheckPushGroup() &&
		BBefkey->btn.l.CheckPushGroup() &&
		Befkey->btn.l.CheckReleaseGroup())
	{
		count++;
	}
	if (Nowkey->btn.r.CheckPushGroup() &&
		BBefkey->btn.r.CheckPushGroup() &&
		Befkey->btn.r.CheckReleaseGroup())
	{
		count++;
	}

	if (count == 0) { Nowkey->pal.trill = 0; }
	else if (count == 1) { Nowkey->pal.trill = 10; }
	else { Nowkey->pal.trill = 10 + count; }

	return;
}

static void RecDdifGetPalMeldy(rec_ddif_data_t *Nowkey, rec_ddif_data_t *Befkey, rec_ddif_data_t *BBefkey) {
	uint count = 0; // 乱打カウント
	Nowkey->pal.meldy = 0;
	if (Nowkey->hitN == 0 && Nowkey->arwN == 0) { return; }
	if (Nowkey->btn != Befkey->btn && Nowkey->btn != BBefkey->btn) { Nowkey->pal.meldy = 1; }
	return;
}

/**
 * cth/bom
 *    0  1  2  3
 * 0  0 10 12 (0)
 * 1 10 12 14 --
 * 2 12 13 -- --
 * 3 12 -- -- --
 */
static void RecDdifGetPalActor(rec_ddif_data_t *Nowkey) {
	uint catchN = 0; // catchの数
	uint bombN = 0; // bombの数
	if (Nowkey->note[0] == NOTE_CATCH) { catchN++; }
	if (Nowkey->note[1] == NOTE_CATCH) { catchN++; }
	if (Nowkey->note[2] == NOTE_CATCH) { catchN++; }
	if (Nowkey->note[0] == NOTE_BOMB) { bombN++; }
	if (Nowkey->note[1] == NOTE_BOMB) { bombN++; }
	if (Nowkey->note[2] == NOTE_BOMB) { bombN++; }
	Nowkey->pal.actor = 0;
	switch (catchN) {
	case 0:
		switch (bombN) {
		case 1:
			Nowkey->pal.actor = 10;
			break;
		case 2:
			Nowkey->pal.actor = 12;
			break;
		case 3:
			Nowkey->pal.actor = 12;
			break;
		}
		break;
	case 1:
		switch (bombN) {
		case 0:
			Nowkey->pal.actor = 10;
			break;
		case 1:
			Nowkey->pal.actor = 12;
			break;
		case 2:
			Nowkey->pal.actor = 13;
			break;
		}
		break;
	case 2:
		switch (bombN) {
		case 0:
			Nowkey->pal.actor = 12;
			break;
		case 1:
			Nowkey->pal.actor = 14;
			break;
		}
		break;
	}
	//arrowひっかけ
	if (Nowkey->note[0] == NOTE_BOMB) {
		if (Nowkey->note[1] == NOTE_UP && Nowkey->note[2] == NOTE_BOMB) {
			Nowkey->pal.actor *= 4;
		}
		else if (Nowkey->note[1] == NOTE_UP || Nowkey->note[2] == NOTE_UP) {
			Nowkey->pal.actor *= 2;
		}
	}
	else if (Nowkey->note[2] == NOTE_BOMB) {
		if (Nowkey->note[0] == NOTE_BOMB && Nowkey->note[1] == NOTE_DOWN) {
			Nowkey->pal.actor *= 4;
		}
		else if (Nowkey->note[0] == NOTE_DOWN || Nowkey->note[1] == NOTE_DOWN) {
			Nowkey->pal.actor *= 2;
		}
	}
	return;
}

/**
 * 1:1(1.000) -> 0  o...o...o カウントする必要なし
 * 2:1(2.000) -> 10 o...o.o.o 10の基準
 * 3:2(1.500) -> 12 o..o..o.o 12の基準
 * 6:1(6.000) -> 12 o.....ooo
 * 3:1(3.000) -> 13 o..oo..oo.13の基準
 * 7:1(7.000) -> 13 o......oo
 * 4:1(4.000) -> 13 o...oo..o
 * 4:3(1.333) -> 13 o...o..oo
 * 5:1(5.000) -> 15 o....oo.o 15の基準
 * 5:2(2.500) -> 15 o....o.oo 15の基準
 * 5:3(1.667) -> 15 o....o..o
 * others -> 18
 */
static void RecDdifGetPalTrick(rec_ddif_data_t *Nowkey, rec_ddif_data_t *Befkey, rec_ddif_data_t *BBefkey) {
	int FirstTime = Nowkey->time - Befkey->time;
	int SecondTime = Befkey->time - BBefkey->time;
	float Gap = 0.0;

	Nowkey->pal.trick = 0;

	if (Nowkey->hitN == 0 && Nowkey->arwN == 0) { return; }

	if (FirstTime < SecondTime) {
		int temp = FirstTime;
		FirstTime = SecondTime;
		SecondTime = temp;
	}

	Gap = (float)FirstTime / (float)SecondTime;

	if (7 < Gap) {
		Nowkey->pal.trick = 0;
	}
	else if (IS_NEAR_NUM(Gap, 1.000, 0.1)) {
		Nowkey->pal.trick = 0;
	}
	else if (IS_NEAR_NUM(Gap, 2.000, 0.1)) {
		Nowkey->pal.trick = 10;
	}
	else if (IS_NEAR_NUM(Gap, 1.500, 0.1)) {
		Nowkey->pal.trick = 12;
	}
	else if (IS_NEAR_NUM(Gap, 6.000, 0.1)) {
		Nowkey->pal.trick = 12;
	}
	else if (IS_NEAR_NUM(Gap, 3.000, 0.1)) {
		Nowkey->pal.trick = 13;
	}
	else if (IS_NEAR_NUM(Gap, 7.000, 0.1)) {
		Nowkey->pal.trick = 13;
	}
	else if (IS_NEAR_NUM(Gap, 4.000, 0.1)) {
		Nowkey->pal.trick = 13;
	}
	else if (IS_NEAR_NUM(Gap, 1.333, 0.1)) {
		Nowkey->pal.trick = 13;
	}
	else if (IS_NEAR_NUM(Gap, 5.000, 0.1)) {
		Nowkey->pal.trick = 15;
	}
	else if (IS_NEAR_NUM(Gap, 2.500, 0.1)) {
		Nowkey->pal.trick = 15;
	}
	else if (IS_NEAR_NUM(Gap, 1.667, 0.1)) {
		Nowkey->pal.trick = 15;
	}
	else {
		Nowkey->pal.trick = 18;
	}
	return;
}

#endif /* RecDdifGetPal */

static int cal_ddif_4(rec_ddif_pal_t *mpal, const TCHAR *path) {
	TCHAR esc_path[255];
	strcopy_2(path, esc_path, 255);
	(void)path;

	int G[5];
	rec_score_file_row_t recfp;
	rec_ddif_data_t key[REC_DDIF_BUF_NUM];
	uint keyN = 0;
	int objectN[3] = { 5999,5999,5999 }; //↑の番号

	if (RecScoreReadForDdif(&recfp, esc_path) != 0) { return -1; }

	//各初期値
	for (uint iLane = 0; iLane < 3; iLane++) {
		for (uint iNum = 0; iNum < recfp.allnum.notenum[iLane]; iNum++) {
			if ((int)recfp.mapdata.note[iNum].lane == iLane) {
				objectN[iLane] = iNum;
				break;
			}
		}
	}

	//GHOSTノーツをスキップ
	for (uint iLane = 0; iLane < 3; iLane++) {
		while (recfp.mapdata.note[objectN[iLane]].object == NOTE_GHOST &&
			0 <= recfp.mapdata.note[objectN[iLane]].hittime)
		{
			objectN[iLane] = recfp.mapdata.note[objectN[iLane]].next;
		}
	}

	while (objectN[0] != 5999 || objectN[1] != 5999 || objectN[2] != 5999) {
		rec_ddif_data_t *Nowkey = &key[keyN];
		rec_ddif_data_t *Befkey = &key[(keyN + REC_DDIF_BUF_NUM - 1) % REC_DDIF_BUF_NUM];
		rec_ddif_data_t *BBefkey = &key[(keyN + REC_DDIF_BUF_NUM - 2) % REC_DDIF_BUF_NUM];

		//Nowkeyの初期化
		Nowkey->time = 0;
		Nowkey->note[0] = NOTE_NONE;
		Nowkey->note[1] = NOTE_NONE;
		Nowkey->note[2] = NOTE_NONE;
		Nowkey->hitN = 0;
		Nowkey->arwN = 0;
		Nowkey->btn.z.Reset();
		Nowkey->btn.x.Reset();
		Nowkey->btn.c.Reset();
		Nowkey->btn.u.Reset();
		Nowkey->btn.d.Reset();
		Nowkey->btn.l.Reset();
		Nowkey->btn.r.Reset();
		Nowkey->pal.notes = 0;
		Nowkey->pal.trill = 0;
		Nowkey->pal.arrow = 0;
		Nowkey->pal.chord = 0;
		Nowkey->pal.chain = 0;
		Nowkey->pal.meldy = 0;
		Nowkey->pal.actor = 0;
		Nowkey->pal.trick = 0;

		//次のノーツの時間を取得
		G[0] = -1; //次のノーツのレーン番号
		if (0 <= recfp.mapdata.note[objectN[0]].hittime) { G[0] = 0; }
		else if (0 <= recfp.mapdata.note[objectN[1]].hittime) { G[0] = 1; }
		else if (0 <= recfp.mapdata.note[objectN[2]].hittime) { G[0] = 2; }
		if (G[0] == -1) { break; }

		if (IS_BETWEEN_RIGHT_LESS(0, recfp.mapdata.note[objectN[0]].hittime, recfp.mapdata.note[objectN[G[0]]].hittime)) { G[0] = 0; }
		if (IS_BETWEEN_RIGHT_LESS(0, recfp.mapdata.note[objectN[1]].hittime, recfp.mapdata.note[objectN[G[0]]].hittime)) { G[0] = 1; }
		if (IS_BETWEEN_RIGHT_LESS(0, recfp.mapdata.note[objectN[2]].hittime, recfp.mapdata.note[objectN[G[0]]].hittime)) { G[0] = 2; }
		Nowkey->time = recfp.mapdata.note[objectN[G[0]]].hittime;

		//次のノーツ群を取得
		Nowkey->note[0] = NOTE_NONE;
		if (recfp.mapdata.note[objectN[0]].hittime < Nowkey->time + REC_DDIF_GROUP_TIME) {
			Nowkey->note[0] = recfp.mapdata.note[objectN[0]].object;
		}
		Nowkey->note[1] = NOTE_NONE;
		if (recfp.mapdata.note[objectN[1]].hittime < Nowkey->time + REC_DDIF_GROUP_TIME) {
			Nowkey->note[1] = recfp.mapdata.note[objectN[1]].object;
		}
		Nowkey->note[2] = NOTE_NONE;
		if (recfp.mapdata.note[objectN[2]].hittime < Nowkey->time + REC_DDIF_GROUP_TIME) {
			Nowkey->note[2] = recfp.mapdata.note[objectN[2]].object;
		}

		//hitノーツ数取得
		Nowkey->hitN = 0;
		if (Nowkey->note[0] == NOTE_HIT) { Nowkey->hitN++; }
		if (Nowkey->note[1] == NOTE_HIT) { Nowkey->hitN++; }
		if (Nowkey->note[2] == NOTE_HIT) { Nowkey->hitN++; }

		//arwノーツ数取得
		Nowkey->arwN = 0;
		if (IS_NOTE_ARROW_GROUP(Nowkey->note[0])) { Nowkey->arwN++; }
		if (IS_NOTE_ARROW_GROUP(Nowkey->note[1])) { Nowkey->arwN++; }
		if (IS_NOTE_ARROW_GROUP(Nowkey->note[2])) { Nowkey->arwN++; }

		//押しキーの判定
		RecDdifGetKeyHit(Nowkey, Befkey);
		RecDdifGetKeyArrow(Nowkey);
		RecDdifGetKeyCatch(Nowkey, Befkey);
		RecDdifGetKeyBomb(Nowkey, Befkey);

		// 譜面パラメータ各種計算,重み計算,notes
		Nowkey->pal.notes = 0;
		if (0 < Nowkey->hitN || 0 < Nowkey->arwN) {
			Nowkey->pal.notes = 1;
		}

		// 譜面パラメータ各種計算,重み計算,arrow
		Nowkey->pal.arrow = 0;
		switch (Nowkey->arwN) {
		case 0:
			Nowkey->pal.arrow = 0;
			break;
		case 1:
			Nowkey->pal.arrow = 10;
			break;
		case 2:
			Nowkey->pal.arrow = 12;
			break;
		case 3:
			Nowkey->pal.arrow = 15;
			break;
		}

		// 譜面パラメータ各種計算,重み計算,Chord~Actor
		RecDdifGetPalChord(Nowkey);
		RecDdifGetPalChain(Nowkey, Befkey, BBefkey);
		RecDdifGetPalTrill(Nowkey, Befkey, BBefkey);
		RecDdifGetPalMeldy(Nowkey, Befkey, BBefkey);
		RecDdifGetPalActor(Nowkey);
		RecDdifGetPalTrick(Nowkey, Befkey, BBefkey);

		// 時間でかける(2000/前回からの時間)
		G[0] = 2000 / maxs_2(1, Nowkey->time - Befkey->time);
		Nowkey->pal.notes *= G[0];
		Nowkey->pal.trill *= G[0];
		Nowkey->pal.arrow *= G[0];
		Nowkey->pal.chord *= G[0];
		Nowkey->pal.chain *= G[0];
		Nowkey->pal.meldy *= G[0];
		Nowkey->pal.actor *= G[0];
		Nowkey->pal.trick *= G[0];

		// maxの計算
		{
			rec_ddif_pal_t buf;
			uint mlp = 100;
			for (uint iNum = 0; iNum < REC_DDIF_BUF_NUM; iNum++) {
				uint iView = (keyN + 50 - iNum) % 50;
				buf.notes += key[iView].pal.notes * mlp / 100;
				buf.trill += key[iView].pal.trill * mlp / 100;
				buf.arrow += key[iView].pal.arrow * mlp / 100;
				buf.chord += key[iView].pal.chord * mlp / 100;
				buf.chain += key[iView].pal.chain * mlp / 100;
				buf.meldy += key[iView].pal.meldy * mlp / 100;
				buf.actor += key[iView].pal.actor * mlp / 100;
				buf.trick += key[iView].pal.trick * mlp / 100;
				mlp = mlp * 95 / 100;
			}
			if (mpal->notes < buf.notes) { mpal->notes = buf.notes; }
			if (mpal->trill < buf.trill) { mpal->trill = buf.trill; }
			if (mpal->arrow < buf.arrow) { mpal->arrow = buf.arrow; }
			if (mpal->chord < buf.chord) { mpal->chord = buf.chord; }
			if (mpal->chain < buf.chain) { mpal->chain = buf.chain; }
			if (mpal->meldy < buf.meldy) { mpal->meldy = buf.meldy; }
			if (mpal->actor < buf.actor) { mpal->actor = buf.actor; }
			if (mpal->trick < buf.trick) { mpal->trick = buf.trick; }
		}

		//次のノートへ
		G[0] = 1; // ノーツなしフラグ
		if (Nowkey->note[0] != NOTE_NONE && recfp.mapdata.note[objectN[0]].next != -1) {
			objectN[0] = recfp.mapdata.note[objectN[0]].next;
			G[0] = 0;
		}
		if (Nowkey->note[1] != NOTE_NONE && recfp.mapdata.note[objectN[1]].next != -1) {
			objectN[1] = recfp.mapdata.note[objectN[1]].next;
			G[0] = 0;
		}
		if (Nowkey->note[2] != NOTE_NONE && recfp.mapdata.note[objectN[2]].next != -1) {
			objectN[2] = recfp.mapdata.note[objectN[2]].next;
			G[0] = 0;
		}

		if (G[0] == 1) { break; }

		//GHOSTノーツ,連actをスキップ
		for (uint iLane = 0; iLane < 3; iLane++) {
			if (Nowkey->note[iLane] == NOTE_CATCH) {
				while ((recfp.mapdata.note[objectN[iLane]].object == NOTE_GHOST ||
					recfp.mapdata.note[objectN[iLane]].object == NOTE_CATCH) &&
					recfp.mapdata.note[objectN[iLane]].next != -1)
				{
					objectN[iLane] = recfp.mapdata.note[objectN[iLane]].next;
				}
			}
			else if (Nowkey->note[iLane] == NOTE_BOMB) {
				while ((recfp.mapdata.note[objectN[iLane]].object == NOTE_GHOST ||
					recfp.mapdata.note[objectN[iLane]].object == NOTE_BOMB) &&
					recfp.mapdata.note[objectN[iLane]].next != -1)
				{
					objectN[iLane] = recfp.mapdata.note[objectN[iLane]].next;
				}
			}
			else {
				while (recfp.mapdata.note[objectN[iLane]].object == NOTE_GHOST &&
					recfp.mapdata.note[objectN[iLane]].next != -1)
				{
					objectN[iLane] = recfp.mapdata.note[objectN[iLane]].next;
				}
			}
		}

		// keyNを進める
		keyN = (keyN + 1) % REC_DDIF_BUF_NUM;
	}
	return 0;
}

void cal_ddif_3(const TCHAR *path) {
	rec_ddif_pal_t mpal;

	if (cal_ddif_4(&mpal, path) != 0) { return; }

	// 各項目をREC_DDIF_BASEの基準に合わせる
	mpal.notes = mpal.notes * REC_DDIF_BASE / REC_DDIF_NOTES_BASE;
	mpal.arrow = mpal.arrow * REC_DDIF_BASE / REC_DDIF_ARROW_BASE;
	mpal.chord = mpal.chord * REC_DDIF_BASE / REC_DDIF_CHORD_BASE;
	mpal.chain = mpal.chain * REC_DDIF_BASE / REC_DDIF_CHAIN_BASE;
	mpal.trill = mpal.trill * REC_DDIF_BASE / REC_DDIF_TRILL_BASE;
	mpal.meldy = mpal.meldy * REC_DDIF_BASE / REC_DDIF_MELDY_BASE;
	mpal.actor = mpal.actor * REC_DDIF_BASE / REC_DDIF_ACTOR_BASE;
	mpal.trick = mpal.trick * REC_DDIF_BASE / REC_DDIF_TRICK_BASE;

	// mdifを計算する
	{
		uint pal[8] = {
			mpal.notes, mpal.trill, mpal.arrow, mpal.chord,
			mpal.chain, mpal.meldy, mpal.actor, mpal.trick
		};
		qsort_ease(uint, pal);

		mpal.mdif =
			pal[0] * REC_DDIF_1ST_WAIGHT +
			pal[1] * REC_DDIF_2ND_WAIGHT +
			pal[2] * REC_DDIF_3RD_WAIGHT +
			pal[3] * REC_DDIF_4TH_WAIGHT +
			pal[4] * REC_DDIF_5TH_WAIGHT +
			pal[5] * REC_DDIF_6TH_WAIGHT +
			pal[6] * REC_DDIF_7TH_WAIGHT +
			pal[7] * REC_DDIF_8TH_WAIGHT;
		mpal.mdif = lins(0, 0, 414885, REC_DDIF_BASE, mpal.mdif); // リスケール
	}

#if 0
	// 各項目をmdifで持ち上げる
	{
		double sum = lins(0, 0, 10, 3, mpal.mdif);
		mpal.notes += sum;
		mpal.arrow += sum;
		mpal.chord += sum;
		mpal.chain += sum;
		mpal.trill += sum;
		mpal.meldy += sum;
		mpal.actor += sum;
		mpal.trick += sum;
	}
#endif

	// mpalを保存する
	RecScoreWriteDdif(&mpal, path);

	return;
}
