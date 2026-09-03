#include "libspu_i.h"
void SpuGetVoiceVolumeX(s32 vNum, s16 *volXL, s16 *volXR) {
    *volXL = _spu_RXX->raw[0x100 + vNum * 2];
    *volXR = _spu_RXX->raw[0x101 + vNum * 2];
}

