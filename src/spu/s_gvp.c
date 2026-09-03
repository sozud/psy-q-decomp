#include "libspu_i.h"
void SpuGetVoicePitch(s32 vNum, u16 *pitch) {
    *pitch = _spu_RXX->raw[vNum * 8 + 2];
}

