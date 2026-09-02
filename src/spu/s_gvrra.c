#include "libspu_i.h"
void SpuGetVoiceRRAttr(s32 vNum, u16 *RR, long *RRmode) {
    u16 t, b;

    t = _spu_RXX->raw[vNum * 8 + 4 + 0];
    b = _spu_RXX->raw[vNum * 8 + 4 + 1];

    *RR = b & 0x1F;
    *RRmode = 3;
    if (b & 0x20) {
        *RRmode = 7;
    }
}

