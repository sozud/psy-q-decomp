#include "libspu_i.h"
void SpuGetVoiceAR(s32 vNum, u16 *AR) {
    u16 t, b;
    t = _spu_RXX->raw[vNum * 8 + 4 + 0];
    b = _spu_RXX->raw[vNum * 8 + 4 + 1];
    *AR = (t >> 8) & 0x3F;
}

