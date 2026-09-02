#include "libspu_i.h"
void SpuGetVoiceEnvelope(s32 vNum, s16 *envx) {
    *envx = _spu_RXX->raw[vNum * 8 + 6];
}

