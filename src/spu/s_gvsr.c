#include "libspu_i.h"
void SpuGetVoiceSR(s32 vNum, u16 *SR) {
    u16 t, b;

    t = _spu_RXX->raw[vNum * 8 + 4 + 0];
    b = _spu_RXX->raw[vNum * 8 + 4 + 1];
    *SR = (b >> 6) & 0x7F;
}

