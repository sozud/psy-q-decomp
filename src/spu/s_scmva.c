#include "libspu_i.h"
int SpuCommonError;
void SpuSetCommonMasterVolumeAttr(s16 mvol_left, s16 mvol_right, s16 mvolmode_left, s16 mvolmode_right) {
    s16 var_t2;
    s16 var_t3;
    s32 var_t0;
    s32 var_t1;

    var_t2 = mvol_left;
    var_t3 = mvol_right;
    var_t0 = 0;

    var_t1 = 0;
    switch (mvolmode_left) {
        case 1:
            var_t0 = 0x8000;
            break;
        case 2:
            var_t0 = 0x9000;
            break;
        case 3:
            var_t0 = 0xA000;
            break;
        case 4:
            var_t0 = 0xB000;
            break;
        case 5:
            var_t0 = 0xC000;
            break;
        case 6:
            var_t0 = 0xD000;
            break;
        case 7:
            var_t0 = 0xE000;
            break;
    }
    if (var_t0 != 0) {
        if (mvol_left >= 0x80) {
            var_t2 = 0x7F;
        } else {
            var_t2 = mvol_left;
            if (mvol_left < 0) {
                var_t2 = 0;
            }
        }
    }
    _spu_RXX->rxx.main_vol.left = (var_t2 & 0x7FFF) | var_t0;
    switch (mvolmode_right) {
        case 1:
            var_t1 = 0x8000;
            break;
        case 2:
            var_t1 = 0x9000;
            break;
        case 3:
            var_t1 = 0xA000;
            break;
        case 4:
            var_t1 = 0xB000;
            break;
        case 5:
            var_t1 = 0xC000;
            break;
        case 6:
            var_t1 = 0xD000;
            break;
        case 7:
            var_t1 = 0xE000;
            break;
    }
    if (var_t1 != 0) {
        if (mvol_right >= 0x80) {
            var_t3 = 0x7F;
        } else {
            var_t3 = mvol_right;
            if (mvol_right < 0) {
                var_t3 = 0;
            }
        }
    }
    _spu_RXX->rxx.main_vol.right = (var_t3 & 0x7FFF) | var_t1;
}

