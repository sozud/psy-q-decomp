#include "libspu_i.h"
void SpuGetVoiceVolumeAttr(s32 vNum, s16 *volL, s16 *volR, s16 *volModeL, s16 *volModeR) {
    s16 var_a0;
    s16 var_t0;
    u16 var_t1;
    u16 var_t2;

    var_t1 = _spu_RXX->raw[vNum * 8 + 0];
    var_t2 = _spu_RXX->raw[vNum * 8 + 1];

    var_t0 = 0;
    if (var_t1 & 0x8000) {
        switch(var_t1 & 0xF000) {
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

        var_t1 &= 0xFFF;
    }

    var_a0 = 0;
    if (var_t2 & 0x8000) {
        switch(var_t2 & 0xF000) {
            case 0x8000:
                var_a0 = 1;
            break;
            case 0x9000:
                var_a0 = 2;
            break;
            case 0xA000:
                var_a0 = 3;
            break;
            case 0xB000:
                var_a0 = 4;
            break;
            case 0xC000:
                var_a0 = 5;
            break;
            case 0xD000:
                var_a0 = 6;
            break;
            case 0xE000:
            case 0xF000:
                var_a0 = 7;
            break;
        }

        var_t2 &= 0xFFF;
    }

    *volModeL = var_t0;
    *volModeR = var_a0;

    CALC_VOL(*volL, var_t1);

    CALC_VOL(*volR, var_t2);

}

