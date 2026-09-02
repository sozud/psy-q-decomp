#include "libspu_i.h"
void SpuGetVoiceSL(s32 vNum, u16 *SL) {
    u16 t, b;

    t = _spu_RXX->raw[vNum * 8 + 4 + 0];
    b = _spu_RXX->raw[vNum * 8 + 4 + 1];
    *SL = t & 0xF;
}

