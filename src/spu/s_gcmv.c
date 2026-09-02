#include "libspu_i.h"
void SpuGetCommonMasterVolume(s16 *mvol_left, s16 *mvol_right) {
    u16 var_a2;
    u16 var_v1;

    var_v1 = _spu_RXX->rxx.main_vol.left;
    var_a2 = _spu_RXX->rxx.main_vol.right;

    if (var_v1 & 0x8000) {
        var_v1 &= 0xFFF;
    }

    if (var_a2 & 0x8000) {
        var_a2 &= 0xFFF;
    }

    CALC_VOL(*mvol_left, var_v1);

    CALC_VOL(*mvol_right, var_a2);
}

