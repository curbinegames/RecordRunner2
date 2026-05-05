#include <playbox.h>

#if 1 /* rec_play_key_hold_c */

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

#endif /* rec_play_key_hold_c */
