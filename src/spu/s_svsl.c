#include "libspu_i.h"
void SpuSetVoiceSL(s32 vNum, u16 SL) {
    u16 x;

    x = _spu_RXX->raw[vNum * 8 + 4 + 0] & 0xFFF0;
    x |= SL;
    _spu_RXX->raw[vNum * 8 + 4 + 0] = x;
    _spu_Fw1ts();
}

