#include "libspu_i.h"
void SpuSetVoiceAR(s32 vNum, u16 AR) {
    u8 x;

    x = _spu_RXX->raw[vNum * 8 + 4 + 0];
    _spu_RXX->raw[vNum * 8 + 4 + 0] = x | (AR << 8);
    _spu_Fw1ts();
}

