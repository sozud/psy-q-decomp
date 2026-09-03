#include "libspu_i.h"
void SpuSetVoiceSR(s32 vNum, u16 SR) {
    u16 x;

    x = _spu_RXX->raw[vNum * 8 + 4 + 1] & 0x3F;
    x |= (SR | 0x100) << 6;

    _spu_RXX->raw[vNum * 8 + 4 + 1] = x;
    _spu_Fw1ts();
}

