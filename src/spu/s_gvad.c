#include "libspu_i.h"
void SpuGetVoiceADSR(s32 vNum, u16 *AR, u16 *DR, u16 *SR, u16 *RR, u16 *SL) {
    u16 temp_t0;
    u16 temp_v1;

    temp_t0 = _spu_RXX->raw[vNum * 8 + 4 + 0];
    temp_v1 = _spu_RXX->raw[vNum * 8 + 4 + 1];
    *AR = (temp_t0 >> 8) & 0x3F;
    *DR = (temp_t0 & 0xF0) >> 4;
    *SR = (temp_v1 >> 6) & 0x7F;
    *RR = temp_v1 & 0x1F;
    *SL = temp_t0 & 0xF;
}

