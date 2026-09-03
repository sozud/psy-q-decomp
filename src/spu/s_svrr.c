#include "libspu_i.h"
void SpuSetVoiceRR(s32 vNum, u16 RR) {
    u16 x;

    x = _spu_RXX->raw[vNum * 8 + 4 + 1] & 0xFFC0;
    x |= RR;

    _spu_RXX->raw[vNum * 8 + 4 + 1] = x;
    _spu_Fw1ts();
}

