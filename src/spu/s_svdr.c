#include "libspu_i.h"
void SpuSetVoiceDR(s32 vNum, u16 DR) {
    u16 x;

    x = _spu_RXX->raw[vNum * 8 + 4 + 0] & 0xFF0F;
    x |= DR << 4;
    _spu_RXX->raw[vNum * 8 + 4 + 0] = x;
    _spu_Fw1ts();
}

