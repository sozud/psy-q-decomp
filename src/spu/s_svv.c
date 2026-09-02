#include "libspu_i.h"
void SpuSetVoiceVolume(s32 vNum, s16 volL, s16 volR) {
    volL &= 0x7FFF;
    volR &= 0x7FFF;

    _spu_RXX->raw[vNum * 8 + 0] = volL;
    _spu_RXX->raw[vNum * 8 + 1] = volR;
    _spu_Fw1ts();
}

