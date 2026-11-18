#include <playbox.h>

#pragma region rec_play_score_c

void rec_play_score_c::SetScoreFlip(int Ntime) {
    this->before = pals_scale(500, this->GetAllScore(), 0, this->before, Ntime - this->time);
    this->time = Ntime;
}

void rec_play_score_c::SetJudgeScore(const rec_play_judge_t &judge, int note_count) {
    this->normal = (judge.just * 90000 + judge.good * 86667 + judge.safe * 45000) / note_count;
}

void rec_play_score_c::SetComboScore(int max_combo, int note_count) {
    this->combo = max_combo * 10000 / note_count;
}

void rec_play_score_c::SetPjustScore(int pjust_count, int note_count) {
    if (note_count == 0) {
        this->pjust = 0;
        return;
    }
    int gap_point = 100 - (note_count - pjust_count);
    int acc_point = (pjust_count * 100 / (double)note_count) - 65;
    this->pjust = maxs_2(maxs_2(gap_point, acc_point), 0);
}

void rec_play_score_c::AddLoss(int val) {
    this->loss = mins_2(this->loss + val, this->normal + this->combo);
}

int rec_play_score_c::GetJudgeScore(void) const {
    return this->normal;
}

int rec_play_score_c::GetAllScore(void) const {
    int ret = this->normal + this->combo - this->loss;
    if (ret == 100000) { ret += this->pjust; }
    return ret;
}

int rec_play_score_c::GetViewScore(int Ntime) const {
    return pals_scale(500, this->GetAllScore(), 0, this->before, Ntime - this->time);
}

#pragma endregion rec_play_score_c

#pragma region distance_score_c

void distance_score_c::AddDist(int val) {
    if (this->droped) { return; }
    this->add += val;
}

void distance_score_c::SetDist(int val) {
    if (this->droped) { return; }
    this->now_dis = val;
}

int distance_score_c::GetDist(void) const {
    return this->now_dis;
}

double distance_score_c::GetAllPoint(void) const {
    return (this->now_dis + 100 * this->add) / 100000.0;
}

void distance_score_c::SetDrop(void) {
    this->droped = true;
}

#pragma endregion distance_score_c

#pragma region rec_play_key_hold_c

int rec_play_key_hold_c::GetHitPushCount(void) const {
    int ret = 0;
    if (this->z == 1) { ret++; }
    if (this->x == 1) { ret++; }
    if (this->c == 1) { ret++; }
    return ret;
}

bool rec_play_key_hold_c::IsPushAnyHit(void) const {
    return this->z > 0 || this->x > 0 || this->c > 0;
}

bool rec_play_key_hold_c::IsJustPushAnyHit(void) const {
    return this->z == 1 || this->x == 1 || this->c == 1;
}

bool rec_play_key_hold_c::IsUpAction(void) const {
    return this->up > 0 && this->down == 0;
}

bool rec_play_key_hold_c::IsDownAction(void) const {
    return this->up == 0 && this->down > 0;
}

bool rec_play_key_hold_c::IsMidAction(void) const {
    return (this->up > 0 && this->down > 0) || this->left > 0 || this->right > 0;
}

bool rec_play_key_hold_c::IsNoneAction(void) const {
    return this->up == 0 && this->down == 0 && this->left == 0 && this->right == 0;
}

bool rec_play_key_hold_c::IsPushAnyArrow(void) const {
    return !this->IsNoneAction();
}

bool rec_play_key_hold_c::IsJustPushAnyArrow(void) const {
    return this->up == 1 || this->down == 1 || this->left == 1 || this->right == 1;
}

#pragma endregion rec_play_key_hold_c
