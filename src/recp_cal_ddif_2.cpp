
#include <strcur.h>
#include <sancur.h>
#include <playbox.h>
#include <RecScoreFile.h>
#include <recp_cal_difkey.h>
#include <recp_cal_ddif.h>
#include <noteLoad.h>

typedef struct ddef_box_2 {
	int maxdif = 0;
	int lastdif = 0;
	int nowdifsection = 1;
	int datanum = -1;
} ddef_box_2;

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

#define REC_FREE 0 //���ł�����
#define REC_PUSH 1 //����
#define REC_HOLD 2 //�������ςȂ�
#define REC_NONE 3 //�������ςȂ�
#define REC_RELA 4 //����

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

typedef struct rec_ddif_pal_s {
	uint notes = 0; //HIT,ARROW�̖��x
	uint trill = 0; //�g�����̖��x
	uint arrow = 0; //ARROW�̖��x
	uint chord = 0; //���������̖��x
	uint chain = 0; //�c�A���x
	uint meldy = 0; //���Ŗ��x
	uint actor = 0; //CATCH,BOMB�̖��x
	uint trick = 0; //ARROW�Ђ������̖��x
} rec_ddif_pal_t;

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

void cal_ddif_3(const TCHAR *path) {
	TCHAR esc_path[255];
	strcopy_2(path, esc_path, 255);
	(void)path;

	int G[5];
	rec_score_file_row_t recfp;
	rec_ddif_data_t key[50];
	uint keyN = 0;
	rec_ddif_pal_t mdif;
	int objectN[3] = { 0,0,0 }; //���̔ԍ�

	RecScoreReadForDdif(&recfp, esc_path);
	//�e�����l
	for (uint iLane = 0; iLane < 3; iLane++) {
		for (uint iNum = 0; iNum < recfp.allnum.notenum[iLane]; iNum++) {
			if ((int)recfp.mapdata.note[iNum].lane == iLane) {
				objectN[iLane] = iNum;
				break;
			}
		}
	}

	//GHOST�m�[�c���X�L�b�v
	for (uint iLane = 0; iLane < 3; iLane++) {
		while (recfp.mapdata.note[objectN[iLane]].object == NOTE_GHOST &&
			0 <= recfp.mapdata.note[objectN[iLane]].hittime)
		{
			objectN[iLane] = recfp.mapdata.note[objectN[iLane]].next;
		}
	}

	while (recfp.mapdata.note[objectN[0]].next != -1 ||
		recfp.mapdata.note[objectN[1]].next != -1 ||
		recfp.mapdata.note[objectN[2]].next != -1)
	{
		note_material mattemp[3] = { NOTE_NONE,NOTE_NONE,NOTE_NONE };

		//���̃m�[�c�̎��Ԃ��擾
		G[0] = 0; //���̃m�[�c�̃��[���ԍ�
		if (recfp.mapdata.note[objectN[1]].hittime < recfp.mapdata.note[objectN[G[0]]].hittime) {
			G[0] = 1;
		}
		if (recfp.mapdata.note[objectN[2]].hittime < recfp.mapdata.note[objectN[G[0]]].hittime) {
			G[0] = 2;
		}
		key[keyN].time = recfp.mapdata.note[objectN[G[0]]].hittime;

		//���̃m�[�c�Q���擾
		if (recfp.mapdata.note[objectN[0]].hittime < key[keyN].time + 40) {
			key[keyN].note[0] = recfp.mapdata.note[objectN[0]].object;
		}
		if (recfp.mapdata.note[objectN[1]].hittime < key[keyN].time + 40) {
			key[keyN].note[1] = recfp.mapdata.note[objectN[1]].object;
		}
		if (recfp.mapdata.note[objectN[2]].hittime < key[keyN].time + 40) {
			key[keyN].note[2] = recfp.mapdata.note[objectN[2]].object;
		}

		//�����L�[�̔���,������
		key[keyN].btn.z.Reset();
		key[keyN].btn.x.Reset();
		key[keyN].btn.c.Reset();
		key[keyN].btn.u.Reset();
		key[keyN].btn.d.Reset();
		key[keyN].btn.l.Reset();
		key[keyN].btn.r.Reset();
		key[keyN].hitN = 0;
		key[keyN].arwN = 0;
		//�����L�[�̔���,HIT
		if (key[keyN].note[0] == NOTE_HIT) { key[keyN].hitN++; }
		if (key[keyN].note[1] == NOTE_HIT) { key[keyN].hitN++; }
		if (key[keyN].note[2] == NOTE_HIT) { key[keyN].hitN++; }
		switch (key[keyN].hitN) {
		case 1:
			G[1] = (keyN + 49) % 50; // �ЂƂO��keyN
			if (key[G[1]].btn.c.GetVal() == REC_PUSH) {
				key[keyN].btn.z.SetPush();
			}
			else {
				key[keyN].btn.c.SetPush();
			}
			break;
		case 2:
			G[1] = (keyN + 49) % 50; // �ЂƂO��keyN
			key[keyN].btn.x.SetPush();
			if (key[G[1]].btn.c.GetVal() == REC_PUSH) {
				key[keyN].btn.z.SetPush();
			}
			else {
				key[keyN].btn.c.SetPush();
			}
			break;
		case 3:
			key[keyN].btn.z.SetPush();
			key[keyN].btn.x.SetPush();
			key[keyN].btn.c.SetPush();
			break;
		}
		//�����L�[�̔���,�A���[
		if (key[keyN].note[0] == NOTE_UP ||
			key[keyN].note[1] == NOTE_UP ||
			key[keyN].note[2] == NOTE_UP)
		{
			key[keyN].btn.u.SetPush();
			key[keyN].arwN++;
		}
		if (key[keyN].note[0] == NOTE_DOWN ||
			key[keyN].note[1] == NOTE_DOWN ||
			key[keyN].note[2] == NOTE_DOWN)
		{
			key[keyN].btn.d.SetPush();
			key[keyN].arwN++;
		}
		if (key[keyN].note[0] == NOTE_LEFT ||
			key[keyN].note[1] == NOTE_LEFT ||
			key[keyN].note[2] == NOTE_LEFT)
		{
			key[keyN].btn.l.SetPush();
			key[keyN].arwN++;
		}
		if (key[keyN].note[0] == NOTE_RIGHT ||
			key[keyN].note[1] == NOTE_RIGHT ||
			key[keyN].note[2] == NOTE_RIGHT)
		{
			key[keyN].btn.r.SetPush();
			key[keyN].arwN++;
		}
		//�����L�[�̔���,�L���b�`
		G[0] = (keyN + 49) % 50; // �ЂƂO��keyN
		if (key[G[0]].btn.u.CheckPushGroup()) { // �O�ŏ�ɂ���
			if (key[keyN].note[2] == NOTE_CATCH) {
				key[keyN].btn.u.SetNone();
				key[keyN].btn.d.SetHold();
			}
			else if (key[keyN].note[1] == NOTE_CATCH) {
				key[keyN].btn.u.SetNone();
			}
			else if (key[keyN].note[0] == NOTE_CATCH) {
				key[keyN].btn.u.SetHold();
			}
		}
		else if (key[G[0]].btn.d.CheckPushGroup()) { // �O�ŉ��ɂ���
			if (key[keyN].note[0] == NOTE_CATCH) {
				key[keyN].btn.u.SetHold();
				key[keyN].btn.d.SetNone();
			}
			else if (key[keyN].note[1] == NOTE_CATCH) {
				key[keyN].btn.d.SetNone();
			}
			else if (key[keyN].note[2] == NOTE_CATCH) {
				key[keyN].btn.d.SetHold();
			}
		}
		else { // �O�Œ��ɂ���
			if (key[keyN].note[0] == NOTE_CATCH) {
				key[keyN].btn.u.SetHold();
			}
			if (key[keyN].note[2] == NOTE_CATCH) {
				key[keyN].btn.d.SetHold();
			}
		}
		//�����L�[�̔���,�{��
		G[0] = (keyN + 49) % 50; // �ЂƂO��keyN
		if (key[G[0]].btn.u.CheckPushGroup()) { // �O�ŏ�ɂ���
			if (key[keyN].note[0] == NOTE_BOMB) {
				if (key[keyN].note[1] == NOTE_BOMB) {
					key[keyN].btn.u.SetNone();
					key[keyN].btn.d.SetHold();
				}
				else if (key[G[0]].btn.u.CheckPushGroup()) {
					key[keyN].pal.trick = 1;
					key[keyN].btn.d.SetHold();
				}
				else {
					key[keyN].btn.u.SetNone();
				}
			}
		}
		else if (key[G[0]].btn.d.CheckPushGroup()) { // �O�ŉ��ɂ���
			if (key[keyN].note[2] == NOTE_BOMB) {
				if (key[keyN].note[1] == NOTE_BOMB) {
					key[keyN].btn.u.SetHold();
					key[keyN].btn.d.SetNone();
				}
				else if (key[G[0]].btn.d.CheckPushGroup()) {
					key[keyN].pal.trick = 1;
					key[keyN].btn.u.SetHold();
				}
				else {
					key[keyN].btn.d.SetNone();
				}
			}
		}
		else { // �O�Œ��ɂ���
			if (key[keyN].note[1] == NOTE_BOMB) {
				if (key[keyN].note[0] == NOTE_BOMB) {
					key[keyN].btn.d.SetHold();
				}
				else  {
					key[keyN].btn.u.SetHold();
				}
			}
		}

		// ���ʃp�����[�^�e��v�Z,������
		key[keyN].pal.notes = 0;
		key[keyN].pal.arrow = 0;
		key[keyN].pal.chord = 0;
		key[keyN].pal.chain = 0;
		key[keyN].pal.meldy = 0;
		key[keyN].pal.actor = 0;
		key[keyN].pal.trick = 0;
		// ���ʃp�����[�^�e��v�Z,�d�݌v�Z,notes,2000/�O�񂩂�̎���
		key[keyN].pal.notes = 10;
		
		// ���ʃp�����[�^�e��v�Z,�d�݌v�Z,arrow
		switch (key[keyN].arwN) {
		case 0:
			key[keyN].pal.arrow = 0;
			break;
		case 1:
			key[keyN].pal.arrow = 10;
			break;
		case 2:
			key[keyN].pal.arrow = 12;
			break;
		case 3:
			key[keyN].pal.arrow = 15;
			break;
		}

		// ���ʃp�����[�^�e��v�Z,�d�݌v�Z,chord
		// hit/arw
		//    0  1  2   3
		// 0  0  0 15 (18)
		// 1  0 12 17  --
		// 2 10 13 --  --
		// 3 12 -- --  --
		switch (key[keyN].arwN) {
		case 0:
			switch (key[keyN].hitN) {
			case 2:
				key[keyN].pal.chord = 10;
				break;
			case 3:
				key[keyN].pal.chord = 12;
				break;
			}
			break;
		case 1:
			switch (key[keyN].hitN) {
			case 1:
				key[keyN].pal.chord = 12;
				break;
			case 2:
				key[keyN].pal.chord = 13;
				break;
			}
			break;
		case 2:
			switch (key[keyN].hitN) {
			case 0:
				key[keyN].pal.chord = 15;
				break;
			case 1:
				key[keyN].pal.chord = 17;
				break;
			}
			break;
		case 3:
			key[keyN].pal.chord = 19;
			break;
		}

		// ���ʃp�����[�^�e��v�Z,�d�݌v�Z,chain
		G[0] = (keyN + 49) % 50; // �ЂƂO��keyN
		G[1] = (keyN + 48) % 50; // �ӂ��O��keyN
		G[2] = 0; // 2�A�J�E���g
		G[3] = 0; // �c�A�J�E���g
		if (key[keyN].btn.z.CheckPushGroup() &&
			key[G[0]].btn.z.CheckPushGroup())
		{
			G[2]++;
			if (key[G[1]].btn.z.CheckPushGroup()) { G[3]++; }
		}
		if (key[keyN].btn.x.CheckPushGroup() &&
			key[G[0]].btn.x.CheckPushGroup())
		{
			G[2]++;
			if (key[G[1]].btn.x.CheckPushGroup()) { G[3]++; }
		}
		if (key[keyN].btn.c.CheckPushGroup() &&
			key[G[0]].btn.c.CheckPushGroup())
		{
			G[2]++;
			if (key[G[1]].btn.c.CheckPushGroup()) { G[3]++; }
		}
		if (key[keyN].btn.u.CheckPushGroup() &&
			key[G[0]].btn.u.CheckPushGroup())
		{
			G[2]++;
			if (key[G[1]].btn.u.CheckPushGroup()) { G[3]++; }
		}
		if (key[keyN].btn.d.CheckPushGroup() &&
			key[G[0]].btn.d.CheckPushGroup())
		{
			G[2]++;
			if (key[G[1]].btn.d.CheckPushGroup()) { G[3]++; }
		}
		if (key[keyN].btn.l.CheckPushGroup() &&
			key[G[0]].btn.l.CheckPushGroup())
		{
			G[2]++;
			if (key[G[1]].btn.l.CheckPushGroup()) { G[3]++; }
		}
		if (key[keyN].btn.r.CheckPushGroup() &&
			key[G[0]].btn.r.CheckPushGroup())
		{
			G[2]++;
			if (key[G[1]].btn.r.CheckPushGroup()) { G[3]++; }
		}

		// ���ʃp�����[�^�e��v�Z,�d�݌v�Z,trill
		G[0] = (keyN + 49) % 50; // �ЂƂO��keyN
		G[1] = (keyN + 48) % 50; // �ӂ��O��keyN
		G[2] = 0; // �g�����J�E���g
		if (key[keyN].btn.z.CheckPushGroup() &&
			key[G[1]].btn.z.CheckPushGroup() &&
			key[G[0]].btn.z.CheckReleaseGroup())
		{
			G[2]++;
		}
		if (key[keyN].btn.x.CheckPushGroup() &&
			key[G[1]].btn.x.CheckPushGroup() &&
			key[G[0]].btn.x.CheckReleaseGroup())
		{
			G[2]++;
		}
		if (key[keyN].btn.c.CheckPushGroup() &&
			key[G[1]].btn.c.CheckPushGroup() &&
			key[G[0]].btn.c.CheckReleaseGroup())
		{
			G[2]++;
		}
		if (key[keyN].btn.u.CheckPushGroup() &&
			key[G[1]].btn.u.CheckPushGroup() &&
			key[G[0]].btn.u.CheckReleaseGroup())
		{
			G[2]++;
		}
		if (key[keyN].btn.d.CheckPushGroup() &&
			key[G[1]].btn.d.CheckPushGroup() &&
			key[G[0]].btn.d.CheckReleaseGroup())
		{
			G[2]++;
		}
		if (key[keyN].btn.l.CheckPushGroup() &&
			key[G[1]].btn.l.CheckPushGroup() &&
			key[G[0]].btn.l.CheckReleaseGroup())
		{
			G[2]++;
		}
		if (key[keyN].btn.r.CheckPushGroup() &&
			key[G[1]].btn.r.CheckPushGroup() &&
			key[G[0]].btn.r.CheckReleaseGroup())
		{
			G[2]++;
		}
		if (G[2] == 0) {
			key[keyN].pal.trill = 0;
		}
		else if (G[2] == 1) {
			key[keyN].pal.trill = 10;
		}
		else {
			key[keyN].pal.trill = 10 + G[2];
		}

		// ���ʃp�����[�^�e��v�Z,�d�݌v�Z,meldy
		// ���ʃp�����[�^�e��v�Z,�d�݌v�Z,actor
		// ���ʃp�����[�^�e��v�Z,�d�݌v�Z,trick
#if 0
		/**
		 * 
notes: �P��HIT�ƃA���[�̖��x
trill: �g�����̖��x
��1HIT�ŁA��O��1HIT�ŁA��O��1HIT�A�d�݂�1.0
��1HIT�ŁA��O��2HIT�ŁA��O��1HIT�A�d�݂�1.2
��2HIT�ŁA��O��1HIT�ŁA��O��2HIT�A�d�݂�1.2
��1�A���[�ŁA��O���Ⴄ1�A���[�ŁA��O������1�A���[�A�d�݂�1.2
��1�A���[�ŁA��O���Ⴄ2�A���[�ŁA��O������1�A���[�A�d�݂�1.4
��2�A���[�ŁA��O���Ⴄ1�A���[�ŁA��O������2�A���[�A�d�݂�1.4
��2�A���[�ŁA��O���Ⴄ2�A���[�ŁA��O������2�A���[�A�d�݂�1.5
arrow: �P�ɃA���[���x
chord: �P�ɓ����������x
��2HIT�̏d�݂�1.0
��3HIT�̏d�݂�1.2
��1HIT1�A���[�̏d�݂�1.2
��2HIT1�A���[�̏d�݂�1.3
��2�A���[�̏d�݂�1.4
��1HIT2�A���[�̏d�݂�1.6
��(3�A���[�͋֎~����Ă��邪�A�ꉞ�d�݂�1.8�Ƃ���)
chain: �c�A���x
��2HIT�ŁA��O��2HIT��3HIT�A�d�݂�1.0�A��O��2HIT��3HIT�Ȃ�d�݂�1.2
��3HIT�ŁA��O��1HIT��2HIT��3HIT�A�d�݂�1.0�A��O��1HIT��2HIT��3HIT�Ȃ�d�݂�1.2
���A���[�ŁA��O�������A���[�A�d�݂�1.0�A��O�������A���[�Ȃ�d�݂�1.2
meldy: ���Ŗ��x
�����̃m�[�c���A��O�ƈႤ�A�d�݂�1.0
�����̃m�[�c���A��O�ƈႤ�A�d�݂�1.1
�����̃m�[�c���A��O�Ƃ���O�Ƃ��Ⴄ�A�d�݂�1.3

actor: catch��bomb�ɂ�鑀��̖��x(�v����)
trick: �A���[�Ђ������̕p�x(�v����)

lanes: ���[���̈ړ��p�x
��lin�͏d��1.0�Aacc��dec�͏d��1.2
��easy�͍��v3�ȓ��Anormal�͍��v6�ȓ��Ahard�͍��v10�ȓ���ڈ��ɂ�����

		 */
#endif

		/*+++*/

		//GHOST�m�[�c���X�L�b�v
		for (uint iLane = 0; iLane < 3; iLane++) {
			while (recfp.mapdata.note[objectN[iLane]].object == NOTE_GHOST &&
				0 <= recfp.mapdata.note[objectN[iLane]].hittime)
			{
				objectN[iLane] = recfp.mapdata.note[objectN[iLane]].next;
			}
		}
	}

#if 0

	//�m�[�c���Ȃ��Ȃ�܂ŌJ��Ԃ�
	while (NULL) {
		DifkeyCalInsertNote(&note[G[0]][objectN[G[0]]], G[0]);
		//ddif�̌v�Z
		while (note[G[0]][objectN[G[0]]].hittime >=
			(Etime - noteoff) / 25 * ddif2.nowdifsection + noteoff) {
			ddif[ddif2.nowdifsection - 1] = cal_ddif_2(ddif2.datanum, difkey[0],
				Etime, noteoff, ddif2.nowdifsection, difkey[7][3]);
			ddif2.nowdifsection++;
		}
		difkey[difkey[1][3]][0] = note[G[0]][objectN[G[0]]].object;
		difkey[difkey[1][3]][1] = note[G[0]][objectN[G[0]]].hittime;

		switch (difkey[difkey[1][3]][0]) {
		case 1: //hit�m�[�c���
			if (difkey[difkey[2][3]][0] == 1 &&
				difkey[difkey[1][3]][1] - 20 < difkey[difkey[2][3]][1]) {
				difkey[difkey[2][3]][2] *= 1;
				objectN[G[0]]++;
				continue;
			}
			break;
		case 2: //catch�m�[�c���
			if (G[0] != 1) difkey[difkey[1][3]][0] = G[0] / 2 + 3;
			if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0]) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 3 &&
				(difkey[difkey[2][3]][0] == 8 ||
					difkey[difkey[2][3]][0] == 9)) {
				difkey[difkey[2][3]][0] = 3;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 2 && (difkey[difkey[2][3]][0] == 1 || difkey[difkey[2][3]][0] == 5 || difkey[difkey[2][3]][0] == 6)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 2 && (difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9)) {
				difkey[difkey[2][3]][0] = 2;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 4 && (difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 8)) {
				difkey[difkey[2][3]][0] = 4;
				objectN[G[0]]++;
				continue;
			}
			break;
		case 5: //left�m�[�c���
			if (difkey[difkey[2][3]][0] == 2 || difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9) {
				difkey[difkey[2][3]][0] = 5;
				continue;
			}
			break;
		case 6: //right�m�[�c���
			if (difkey[difkey[2][3]][0] == 2 || difkey[difkey[2][3]][0] == 7 || difkey[difkey[2][3]][0] == 9) {
				difkey[difkey[2][3]][0] = 6;
				continue;
			}
			break;
		case 7: //bomb�m�[�c���
			if (G[0] != 0)difkey[difkey[1][3]][0] = G[0] + 7;
			if (difkey[difkey[1][3]][0] == difkey[difkey[2][3]][0]) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && difkey[difkey[2][3]][0] == 8 || difkey[difkey[1][3]][0] == 8 && difkey[difkey[2][3]][0] == 7) {
				difkey[difkey[2][3]][0] = 4;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && difkey[difkey[2][3]][0] == 9 || difkey[difkey[1][3]][0] == 9 && difkey[difkey[2][3]][0] == 7) {
				difkey[difkey[2][3]][0] = 2;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 8 && difkey[difkey[2][3]][0] == 9 || difkey[difkey[1][3]][0] == 9 && difkey[difkey[2][3]][0] == 8) {
				difkey[difkey[2][3]][0] = 3;
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 7 && (difkey[difkey[2][3]][0] != 3)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 8 && (difkey[difkey[2][3]][0] == 3 || difkey[difkey[2][3]][0] == 4)) {
				objectN[G[0]]++;
				continue;
			}
			else if (difkey[difkey[1][3]][0] == 9 && (difkey[difkey[2][3]][0] != 4)) {
				objectN[G[0]]++;
				continue;
			}
			break;
		}
		/* calculate difkey */
		if (difkey[2][3] != -1 && difkey[3][3] != -1) {
			difkey[difkey[1][3]][2] = cal_difkey(difkey[difkey[1][3]][1],
				difkey[difkey[2][3]][1], difkey[difkey[1][3]][0],
				difkey[difkey[2][3]][0], difkey[difkey[3][3]][0],
				difkey[difkey[2][3]][2]);
		}
		for (i[0] = 0; i[0] < 3; i[0]++) {
			if (note[G[0]][objectN[G[0]]].object >= 3 && note[G[0]][objectN[G[0]]].object <= 6 &&
				G[0] != i[0] && note[G[0]][objectN[G[0]]].object == note[i[0]][objectN[i[0]]].object &&
				note[G[0]][objectN[G[0]]].hittime + 5 >= note[i[0]][objectN[i[0]]].hittime) {
				objectN[i[0]]++;
			}
		}
		objectN[G[0]]++;
		ddif2.datanum++;
		G[0] = 0;
		ddif2.maxdif = mins(ddif2.maxdif,
			cal_nowdif_m_2(difkey[0], difkey[0][3], difkey[1][3], difkey[7][3]));
		for (i[0] = 1; i[0] < 4; i[0]++) {
			difkey[i[0]][3]++;
			if (difkey[i[0]][3] > difkey[0][3]) { difkey[i[0]][3] = 0; }
		}
	}
	ddif2.datanum++;
	for (i[0] = 0; i[0] < 2; i[0]++) {
		if (difkey[i[0]][2] == 0 && difkey[2][2] > 0) { ddif2.datanum--; }
	}
	if (ddif2.datanum < 1) { ddif2.datanum = 1; }
	if (ddif2.datanum > 50) { ddif2.datanum = 50; }
	ddifG[1] = ddif2.maxdif;
	if (ddifG[1] <= 0) { ddifG[1] = 1; }
	ddif2.maxdif /= 50;
	ddif[ddif2.nowdifsection - 1] = 0;
	for (i[0] = 0; i[0] < ddif2.datanum; i[0]++) {
		if (difkey[i[0]][1] > Etime - difkey[7][3]) {
			ddif[ddif2.nowdifsection - 1] += difkey[i[0]][2];
		}
	}
	for (i[0] = ddif2.nowdifsection - 1; i[0] <= 24; i[0]++) {
		ddif[i[0]] = ddif[ddif2.nowdifsection - 1];
	}
	ddif2.lastdif = ddif[ddif2.nowdifsection - 1] / 50;
	//NEEDYOU:Lv.1.0->2713, Co-op katyohugetsu:Lv.8.0->34733
	ddif2.maxdif = lins(2713, 100, 34733, 800, ddif2.maxdif);
	ddif2.lastdif = lins(2713, 100, 34733, 800, ddif2.lastdif);

	//���ʃZ�[�u
	_wfopen_s(&fp, path, L"wb");
	if (fp == NULL) { return 0; }
	fwrite(&allnum, sizeof(playnum_box), 1, fp);//�e�f�[�^�̌�
	fwrite(&mp3FN, 255, 1, fp);//���y�t�@�C����
	fwrite(&bpm, sizeof(double), 1, fp);//BPM
	fwrite(&noteoff, sizeof(int), 1, fp);//offset
	fwrite(&skyFN, 255, 1, fp);//��w�i��
	fwrite(&groundFN, 255, 1, fp);//�n�ʉ摜��
	fwrite(&waterFN, 255, 1, fp);//�����摜��
	fwrite(&songN, 255, 1, fp);//�Ȗ�
	fwrite(&songNE, 255, 1, fp);//�Ȗ�(�p��)
	fwrite(&Lv, sizeof(short int), 1, fp);//���x��
	//fwrite(&item, sizeof(int), 99, fp);//�A�C�e���摜�f�[�^(���얢�m�F)
	fwrite(&fall, sizeof(int), 198, fp);//�������w�i�؂�ւ��^�C�~���O
	fwrite(&speedt, sizeof(double), 990, fp);//���[�����x
	fwrite(&chamo, sizeof(int), 594, fp);//�L�����O���ϊ��^�C�~���O
	fwrite(&Ymove[0], sizeof(int), allnum.Ymovenum[0] * 4, fp);//�ヌ�[���c�ʒu�ړ��^�C�~���O
	fwrite(&Ymove[1], sizeof(int), allnum.Ymovenum[1] * 4, fp);//�����[���c�ʒu�ړ��^�C�~���O
	fwrite(&Ymove[2], sizeof(int), allnum.Ymovenum[2] * 4, fp);//�����[���c�ʒu�ړ��^�C�~���O
	fwrite(&Ymove[3], sizeof(int), allnum.Ymovenum[3] * 4, fp);//�n�ʏc�ʒu�ړ��^�C�~���O
	fwrite(&Ymove[4], sizeof(int), allnum.Ymovenum[4] * 4, fp);//���ʏc�ʒu�ړ��^�C�~���O
	fwrite(&Xmove[0], sizeof(int), allnum.Xmovenum[0] * 4, fp);//�ヌ�[�����ʒu�ړ��^�C�~���O
	fwrite(&Xmove[1], sizeof(int), allnum.Xmovenum[1] * 4, fp);//�����[�����ʒu�ړ��^�C�~���O
	fwrite(&Xmove[2], sizeof(int), allnum.Xmovenum[2] * 4, fp);//�����[�����ʒu�ړ��^�C�~���O
	fwrite(&lock, sizeof(int), 396, fp);//�m�[�c�Œ�؂�ւ��^�C�~���O
	fwrite(&carrow, sizeof(int), 198, fp);//�L���������؂�ւ��^�C�~���O
	fwrite(&viewT, sizeof(int), 198, fp);//�m�[�c�\�����ԕϊ��^�C�~���O
#if SWITCH_NOTE_BOX_2 == 1
	noteSaveOld(&note[0][0], &note[1][0], &note[2][0], &allnum, fp);
#else
	fwrite(&note[0], sizeof(struct note_box), allnum.notenum[0], fp); /* �ヌ�[���m�[�c�f�[�^ */
	fwrite(&note[1], sizeof(struct note_box), allnum.notenum[1], fp); /* �����[���m�[�c�f�[�^ */
	fwrite(&note[2], sizeof(struct note_box), allnum.notenum[2], fp); /* �����[���m�[�c�f�[�^ */
#endif
	fwrite(&notes, sizeof(short int), 1, fp);//�m�[�c��
	fwrite(&Etime, sizeof(int), 1, fp);//�ȏI������
	G[0] = ddif2.maxdif;//�ō���Փx
	G[1] = ddif2.lastdif;//�ŏI��Փx
	fwrite(&G, sizeof(int), 2, fp);
	fwrite(&ddif, sizeof(int), 25, fp);//�e��ԓ�Փx�f�[�^
	fwrite(&ddif2.nowdifsection, sizeof(int), 1, fp);//�e��ԓ�Փx�f�[�^
	fwrite(&ddifG[1], sizeof(int), 1, fp);//�e��ԓ�Փx�f�[�^
	fwrite(&DifFN, 255, 1, fp);//��Փx�o�[��
	fwrite(&Movie, sizeof(item_box), allnum.movienum, fp);//����f�[�^
	fwrite(&camera, sizeof(struct camera_box), 255, fp);//�J�����f�[�^
	fwrite(&scrool, sizeof(struct scrool_box), 99, fp);//�X�N���[���f�[�^
	fwrite(&v_bpm, sizeof(view_BPM_box), allnum.v_BPMnum, fp);//�����ڂ�BPM�f�[�^
	fwrite(&outpoint, sizeof(int), 2, fp);//���ʃG���[
	fclose(fp);

#endif

	return;
}
