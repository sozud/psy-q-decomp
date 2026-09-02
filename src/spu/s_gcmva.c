#include "libspu_i.h"
static inline void test(s16 *vol, u16 var_t2) {
        *vol = var_t2;
    if (var_t2 >= 0x4000) {
        *vol -= 0x8000u;
    } else {
    }
}

void SpuGetCommonMasterVolumeAttr(s16 *mvol_left, s16 *mvol_right, s16 *mvolmode_left, s16 *mvolmode_right) {
    s16 var_t0;
    s16 var_t1;
    u16 var_t2;
    u16 var_t3;

    var_t2 = _spu_RXX->rxx.main_vol.left;
    var_t3 = _spu_RXX->rxx.main_vol.right;
    var_t0 = var_t1 = 0;

    if ((var_t2 & 0x8000) != 0) {
        switch(var_t2 & 0xF000) {
            case 0x8000:
                var_t1 = 1;
            break;
            case 0x9000:
                var_t1 = 2;
            break;
            case 0xA000:
                var_t1 = 3;
            break;
            case 0xB000:
                var_t1 = 4;
            break;
            case 0xC000:
                var_t1 = 5;
            break;
            case 0xD000:
                var_t1 = 6;
            break;
            case 0xE000:
            case 0xF000:
                var_t1 = 7;
            break;
        }

        var_t2 &= 0xFFF;
    } else {
        var_t1 = 0;
    }

    if ((var_t3 & 0x8000) != 0) {
        switch(var_t3 & 0xF000) {
            case 0x8000:
                var_t0 = 1;
            break;
            case 0x9000:
                var_t0 = 2;
            break;
            case 0xA000:
                var_t0 = 3;
            break;
            case 0xB000:
                var_t0 = 4;
            break;
            case 0xC000:
                var_t0 = 5;
            break;
            case 0xD000:
                var_t0 = 6;
            break;
            case 0xE000:
            case 0xF000:
                var_t0 = 7;
            break;
        }

        var_t3 &= 0xFFF;
    } else {
        var_t0 = 0;
    }

    *mvolmode_left = var_t1;
    *mvolmode_right = var_t0;

    CALC_VOL(*mvol_left, var_t2);

    CALC_VOL(*mvol_right, var_t3);
}

