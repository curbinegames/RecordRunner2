
#include <strcur.h>
#include <sancur.h>
#include <playbox.h>
#include <RecScoreFile.h>
#include <recp_cal_difkey.h>
#include <recp_cal_ddif.h>
#include <noteLoad.h>

#define REC_DDIF_BUF_NUM 50 //ddif計算で使用するバッファの数
#define REC_DDIF_GROUP_TIME 40 //まとまりと見なす時間[ms]

#define REC_FREE 0 //何でもいい
#define REC_PUSH 1 //押す
#define REC_HOLD 2 //押しっぱなし
#define REC_NONE 3 //離しっぱなし
#define REC_RELA 4 //離す

#define REC_DDIF_NOTES_BASE    1115 // Co-op Taylorのnotesの値
#define REC_DDIF_ARROW_BASE    5360 // Co-op Taylorのarrowの値
#define REC_DDIF_CHORD_BASE    1288 // FURRY TIME  のchordの値
#define REC_DDIF_CHAIN_BASE 1000000 // Co-op Taylorのchainの値
#define REC_DDIF_TRILL_BASE    8930 // Co-op Taylorのtrillの値
#define REC_DDIF_MELDY_BASE   13395 // Co-op Taylorのmaldyの値
#define REC_DDIF_ACTOR_BASE    1710 // 花調風月    のactorの値 (Lv8)
#define REC_DDIF_TRICK_BASE       1 // xxxのtrickの値

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

typedef struct rec_ddif_data_s {
	DxTime_t time = 0;
	note_material note[3] = { NOTE_NONE,NOTE_NONE,NOTE_NONE };
	uint hitN = 0;
	uint arwN = 0;
	struct {
		rec_ddif_btn_c z;
		rec_ddif_btn_c x;
		rec_ddif_btn_c c;
		rec_ddif_btn_c u;
		rec_ddif_btn_c d;
		rec_ddif_btn_c l;
		rec_ddif_btn_c r;
	} btn;
	rec_ddif_pal_t pal;
} rec_ddif_data_t;

#if 0
static int cal_nowdif_m_2(int *difkey, int num, int now, int voidtime) {
	int ret = 0;
	int count = 0;
	for (int i = 0; i <= num; i++) {
		if (difkey[i * 4 + 2] < 0) {
			break;
		}
		if (difkey[i * 4 + 1] > difkey[now * 4 + 1] - voidtime) {
			ret += difkey[i * 4 + 2];
			count++;
		}
	}
	if (count == 0) {
		return 0;
	}
	else {
		return ret * 50 / count;
	}
}

static int cal_ddif_2(int num, int const *difkey, int Etime, int noteoff, int difsec, int voidtime) {
	int ret = 0;
	int count = 0;
	if (num >= 50) {
		num = 49;
	}
	for (int i = 0; i < num; i++) {
		if (difkey[i * 4 + 1] > (Etime - noteoff) / 25 *
			difsec - voidtime + noteoff) {
			count++;
			ret += difkey[i * 4 + 2];
		}
	}
	if (count == 0) {
		return 0;
	}
	else {
		return ret * 50 / count;
	}
}
#endif

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
	if (Nowkey->note[0] == NOTE_UP ||
		Nowkey->note[1] == NOTE_UP ||
		Nowkey->note[2] == NOTE_UP)
	{
		Nowkey->btn.u.SetPush();
	}
	if (Nowkey->note[0] == NOTE_DOWN ||
		Nowkey->note[1] == NOTE_DOWN ||
		Nowkey->note[2] == NOTE_DOWN)
	{
		Nowkey->btn.d.SetPush();
	}
	if (Nowkey->note[0] == NOTE_LEFT ||
		Nowkey->note[1] == NOTE_LEFT ||
		Nowkey->note[2] == NOTE_LEFT)
	{
		Nowkey->btn.l.SetPush();
	}
	if (Nowkey->note[0] == NOTE_RIGHT ||
		Nowkey->note[1] == NOTE_RIGHT ||
		Nowkey->note[2] == NOTE_RIGHT)
	{
		Nowkey->btn.r.SetPush();
	}
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

/* trickもこの関数でやってる */
static void RecDdifGetKeyBomb(rec_ddif_data_t *Nowkey, rec_ddif_data_t *Befkey) {
	Nowkey->pal.trick = 0;
	if (Befkey->btn.u.CheckPushGroup()) { // 前で上にいる
		if (Nowkey->note[0] == NOTE_BOMB) {
			if (Nowkey->note[1] == NOTE_BOMB) {
				Nowkey->btn.u.SetNone();
				Nowkey->btn.d.SetHold();
			}
			else if (Befkey->btn.u.CheckPushGroup()) {
				Nowkey->pal.trick = 1;
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
				Nowkey->pal.trick = 1;
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
			else  {
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
		Nowkey->pal.chain = 13;
	}
	else {
		Nowkey->pal.chain = 10;
	}
	Nowkey->pal.chain += divN;

	return;
}

static void RecDdifGetPalTrill(rec_ddif_data_t *Nowkey, rec_ddif_data_t *Befkey, rec_ddif_data_t *BBefkey) {
	uint count = 0; // トリルカウント
	Nowkey->pal.trill = 0;
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
	if (Nowkey->btn.z.CheckPushGroup()) {
		if (Befkey->btn.z.CheckReleaseGroup() &&
			BBefkey->btn.z.CheckPushGroup())
		{
			count++;
			Nowkey->pal.meldy += 15;
		}
		else if (Befkey->btn.z.CheckPushGroup() &&
			BBefkey->btn.z.CheckReleaseGroup())
		{
			count++;
			Nowkey->pal.meldy += 40;
		}
	}
	if (Nowkey->btn.x.CheckPushGroup()) {
		if (Befkey->btn.x.CheckReleaseGroup() &&
			BBefkey->btn.x.CheckPushGroup())
		{
			count++;
			Nowkey->pal.meldy += 15;
		}
		else if (Befkey->btn.x.CheckPushGroup() &&
			BBefkey->btn.x.CheckReleaseGroup())
		{
			count++;
			Nowkey->pal.meldy += 40;
		}
	}
	if (Nowkey->btn.c.CheckPushGroup()) {
		if (Befkey->btn.c.CheckReleaseGroup() &&
			BBefkey->btn.c.CheckPushGroup())
		{
			count++;
			Nowkey->pal.meldy += 15;
		}
		else if (Befkey->btn.c.CheckPushGroup() &&
			BBefkey->btn.c.CheckReleaseGroup())
		{
			count++;
			Nowkey->pal.meldy += 40;
		}
	}
	if (Nowkey->btn.u.CheckPushGroup()) {
		if (Befkey->btn.u.CheckReleaseGroup() &&
			BBefkey->btn.u.CheckPushGroup())
		{
			count++;
			Nowkey->pal.meldy += 15;
		}
		else if (Befkey->btn.u.CheckPushGroup() &&
			BBefkey->btn.u.CheckReleaseGroup())
		{
			count++;
			Nowkey->pal.meldy += 40;
		}
	}
	if (Nowkey->btn.d.CheckPushGroup()) {
		if (Befkey->btn.d.CheckReleaseGroup() &&
			BBefkey->btn.d.CheckPushGroup())
		{
			count++;
			Nowkey->pal.meldy += 15;
		}
		else if (Befkey->btn.d.CheckPushGroup() &&
			BBefkey->btn.d.CheckReleaseGroup())
		{
			count++;
			Nowkey->pal.meldy += 40;
		}
	}
	if (Nowkey->btn.l.CheckPushGroup()) {
		if (Befkey->btn.l.CheckReleaseGroup() &&
			BBefkey->btn.l.CheckPushGroup())
		{
			count++;
			Nowkey->pal.meldy += 15;
		}
		else if (Befkey->btn.l.CheckPushGroup() &&
			BBefkey->btn.l.CheckReleaseGroup())
		{
			count++;
			Nowkey->pal.meldy += 40;
		}
	}
	if (Nowkey->btn.r.CheckPushGroup()) {
		if (Befkey->btn.r.CheckReleaseGroup() &&
			BBefkey->btn.r.CheckPushGroup())
		{
			count++;
			Nowkey->pal.meldy += 15;
		}
		else if (Befkey->btn.r.CheckPushGroup() &&
			BBefkey->btn.r.CheckReleaseGroup())
		{
			count++;
			Nowkey->pal.meldy += 40;
		}
	}

	switch (count) {
	case 0: /* 0 -> 0 */
		Nowkey->pal.meldy = 0;
		break;
	case 1: /* 1 -> 1.0 */
		// Nowkey->pal.meldy = Nowkey->pal.meldy * 1;
		break;
	case 2: /* 2 -> 1.3 */
		Nowkey->pal.meldy = Nowkey->pal.meldy * 13 / 20;
		break;
	case 3: /* 3 -> 1.5 */
		Nowkey->pal.meldy = Nowkey->pal.meldy * 15 / 30;
		break;
	case 4: /* 4 -> 1.7 */
		Nowkey->pal.meldy = Nowkey->pal.meldy * 17 / 40;
		break;
	case 5: /* 5 -> 1.8 */
		Nowkey->pal.meldy = Nowkey->pal.meldy * 18 / 50;
		break;
	case 6: /* 6 -> 1.9 */
		Nowkey->pal.meldy = Nowkey->pal.meldy * 19 / 60;
		break;
	case 7: /* 7 -> 2.0 */
		Nowkey->pal.meldy = Nowkey->pal.meldy * 20 / 70;
		break;
	}

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
	Nowkey->pal.actor = 0;
	if (Nowkey->note[0] == NOTE_CATCH) {
		catchN++;
	}
	if (Nowkey->note[1] == NOTE_CATCH) {
		catchN++;
	}
	if (Nowkey->note[2] == NOTE_CATCH) {
		catchN++;
	}
	if (Nowkey->note[0] == NOTE_BOMB) {
		bombN++;
	}
	if (Nowkey->note[1] == NOTE_BOMB) {
		bombN++;
	}
	if (Nowkey->note[2] == NOTE_BOMB) {
		bombN++;
	}
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
	return;
}

#endif /* RecDdifGetPal */

template<typename T>
static int qsort_protocol(const void *n1, const void *n2) {
	if (*(T *)n1 > *(T *)n2) { return -1; }
	else if (*(T *)n1 < *(T *)n2) { return 1; }
	else { return 0; }
}

#define qsort_ease(type, mat) qsort((mat), sizeof(mat) / sizeof(type), sizeof(type), qsort_protocol<type>)

static void cal_ddif_4(rec_ddif_pal_t *mpal, const TCHAR *path) {
	TCHAR esc_path[255];
	strcopy_2(path, esc_path, 255);
	(void)path;

	int G[5];
	rec_score_file_row_t recfp;
	rec_ddif_data_t key[REC_DDIF_BUF_NUM];
	uint keyN = 0;
	int objectN[3] = { 0,0,0 }; //↑の番号

	RecScoreReadForDdif(&recfp, esc_path);
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

		//次のノーツの時間を取得
		G[0] = 0; //次のノーツのレーン番号
		if (recfp.mapdata.note[objectN[1]].hittime < recfp.mapdata.note[objectN[G[0]]].hittime) { G[0] = 1; }
		if (recfp.mapdata.note[objectN[2]].hittime < recfp.mapdata.note[objectN[G[0]]].hittime) { G[0] = 2; }
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

		//押しキーの初期化
		Nowkey->btn.z.Reset();
		Nowkey->btn.x.Reset();
		Nowkey->btn.c.Reset();
		Nowkey->btn.u.Reset();
		Nowkey->btn.d.Reset();
		Nowkey->btn.l.Reset();
		Nowkey->btn.r.Reset();

		//押しキーの判定
		RecDdifGetKeyHit(Nowkey, Befkey);
		RecDdifGetKeyArrow(Nowkey);
		RecDdifGetKeyCatch(Nowkey, Befkey);
		RecDdifGetKeyBomb(Nowkey, Befkey);

		// 譜面パラメータ各種計算,重み計算,notes
		Nowkey->pal.notes = 0;
		if (Nowkey->note[0] == NOTE_HIT || IS_NOTE_ARROW_GROUP(Nowkey->note[0]) ||
			Nowkey->note[1] == NOTE_HIT || IS_NOTE_ARROW_GROUP(Nowkey->note[1]) ||
			Nowkey->note[2] == NOTE_HIT || IS_NOTE_ARROW_GROUP(Nowkey->note[2]))
		{
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
		// trickはRecDdifGetKeyBomb()で計算済み

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
			for (uint iNum = 0; iNum < REC_DDIF_BUF_NUM; iNum++) {
				buf.notes += key[iNum].pal.notes;
				buf.trill += key[iNum].pal.trill;
				buf.arrow += key[iNum].pal.arrow;
				buf.chord += key[iNum].pal.chord;
				buf.chain += key[iNum].pal.chain;
				buf.meldy += key[iNum].pal.meldy;
				buf.actor += key[iNum].pal.actor;
				buf.trick += key[iNum].pal.trick;
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
		if (recfp.mapdata.note[objectN[0]].next != -1) {
			objectN[0] = recfp.mapdata.note[objectN[0]].next;
		}
		if (recfp.mapdata.note[objectN[1]].next != -1) {
			objectN[1] = recfp.mapdata.note[objectN[1]].next;
		}
		if (recfp.mapdata.note[objectN[2]].next != -1) {
			objectN[2] = recfp.mapdata.note[objectN[2]].next;
		}

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
	return;
}

void cal_ddif_3(const TCHAR *path) {
	rec_ddif_pal_t mpal;

	cal_ddif_4(&mpal, path);

	// 各項目をLv9の基準に合わせる
	mpal.notes = mpal.notes * 990 / REC_DDIF_NOTES_BASE;
	mpal.trill = mpal.trill * 990 / REC_DDIF_TRILL_BASE;
	mpal.arrow = mpal.arrow * 990 / REC_DDIF_ARROW_BASE;
	mpal.chord = mpal.chord * 990 / REC_DDIF_CHORD_BASE;
	mpal.chain = mpal.chain * 990 / REC_DDIF_CHAIN_BASE;
	mpal.meldy = mpal.meldy * 990 / REC_DDIF_MELDY_BASE;
	mpal.actor = mpal.actor * 990 / REC_DDIF_ACTOR_BASE;
	mpal.trick = mpal.trick * 990 / REC_DDIF_TRICK_BASE;

	/**
	 * 内部レート計算
	 * A = 一番高い項目
	 * B = 上位2位～上位4位の平均
	 * C = 全ての項目の平均
	 * D = B + 2 * C / B
	 * レート = D + bitween(0, A - D, 2) / 3
	 */
	{
		uint pal[8] = {
			mpal.notes, mpal.trill, mpal.arrow, mpal.chord,
			mpal.chain, mpal.meldy, mpal.actor, mpal.trick
		};
		qsort_ease(uint, pal);

		mpal.mdif = (pal[1] + pal[2] + pal[3]) / 3; // 上位2位～上位4位の平均
		mpal.mdif = mpal.mdif + 2 * (pal[0] + pal[1] + pal[2] + pal[3] + pal[4] + pal[5] + pal[6] + pal[7]) / 8 / maxs_2(mpal.mdif, 1); // D
		mpal.mdif = mpal.mdif + betweens(0, pal[0] - mpal.mdif, 2) / 3; // レート
	}

	// mpal と ddifRate を何らかの方法で保存する
	RecScoreWriteDdif(&mpal, path);

	return;
}
