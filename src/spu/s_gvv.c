#include "libspu_i.h"
void SpuGetVoiceVolume(s32 vNum, s16 *volL, s16 *volR) {
    u16 temp_a0_2;
    u16 temp_v1;

    temp_v1 = _spu_RXX->raw[vNum * 8 + 0];
    temp_a0_2 = _spu_RXX->raw[vNum * 8 + 1];

    CALC_VOL(*volL, temp_v1);

    CALC_VOL(*volR, temp_a0_2);
}

