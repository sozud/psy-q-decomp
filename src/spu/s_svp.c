#include "libspu_i.h"
void SpuSetVoicePitch(s32 vNum, u16 pitch) {
    _spu_RXX->raw[vNum * 8 + 2] = pitch;
    _spu_Fw1ts();
}

