#include "libspu_i.h"
void SpuGetVoiceSRAttr(s32 vNum, u16 *SR, long *SRmode) {
    u16 t, b;

    t = _spu_RXX->raw[vNum * 8 + 4 + 0];
    b = _spu_RXX->raw[vNum * 8 + 4 + 1];

    *SR = (b >> 6) & 0x7F;

    *SRmode = 1;
    if ((b & 0xE000) == 0xC000) {
        *SRmode = 7;
    }
    else if ((b & 0xE000) == 0x8000) {
        *SRmode = 5;
    }
    else if ((b & 0xE000) == 0x4000) {
        *SRmode = 3;
    }
}

