#include "libspu_i.h"
void SpuGetVoiceADSRAttr(s32 vNum, u16 *AR, u16 *DR, u16 *SR, u16 *RR, u16 *SL, long *ARmode, long *SRmode, long *RRmode) {
    u16 temp_a0_2;
    u16 temp_t0;

    temp_t0 = _spu_RXX->raw[vNum * 8 + 4 + 0];
    temp_a0_2 = _spu_RXX->raw[vNum * 8 + 4 + 1];
    *AR = (temp_t0 >> 8) & 0x3F;

    *ARmode = 1;
    if (temp_t0 & 0x8000) {
        *ARmode = 5;
    }

    *DR = (temp_t0 & 0xF0) >> 4;
    *SR = (temp_a0_2 >> 6) & 0x7F;

    *SRmode = 1;
    if ((temp_a0_2 & 0xE000) == 0xC000) {
        *SRmode = 7;
    }
    else if ((temp_a0_2 & 0xE000) == 0x8000) {
        *SRmode = 5;
    }
    else if ((temp_a0_2 & 0xE000) == 0x4000) {
        *SRmode = 3;
    }

    *RR = temp_a0_2 & 0x1F;
    *RRmode = 3;
    if (temp_a0_2 & 0x20) {
        *RRmode = 7;
    }

    *SL = temp_t0 & 0xF;
}

