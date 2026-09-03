#include "libspu_i.h"
void SpuSetVoiceADSR(s32 vNum, u16 AR, u16 DR, u16 SR, u16 RR, u16 SL) {
    u16 v;

    v = ((AR & 0x7F) << 8) | ((DR & 0xF) * 0x10) | (SL & 0xF);
    _spu_RXX->raw[vNum * 8 + 4 + 0] = v;

    v = ((SR & 0x7F) << 6) | (RR & 0x1F) | 0x4000;
    _spu_RXX->raw[vNum * 8 + 4 + 1] = v;

    _spu_Fw1ts();
}

