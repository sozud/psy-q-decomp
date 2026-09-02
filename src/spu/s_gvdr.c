#include "libspu_i.h"
void SpuGetVoiceDR(s32 vNum, u16 *DR) {
    u16 t, b;
    t = _spu_RXX->raw[vNum * 8 + 4 + 0];
    b = _spu_RXX->raw[vNum * 8 + 4 + 1];
    *DR = (t & 0xF0) >> 4;
}

