#include "libspu_i.h"
void SpuGetVoiceRR(s32 vNum, u16 *RR) {
    u16 t, b;
    t = _spu_RXX->raw[vNum * 8 + 4 + 0];
    b = _spu_RXX->raw[vNum * 8 + 4 + 1];
    *RR = b & 0x1F;
}

