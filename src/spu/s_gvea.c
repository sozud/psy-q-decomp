#include "libspu_i.h"
void SpuGetVoiceEnvelopeAttr(s32 vNum, long *keyStat, s16 *envx) {
    u16 temp_v1;

    temp_v1 = _spu_RXX->raw[vNum * 8 | 6];
    *envx = temp_v1;
    if (_spu_keystat & (1 << vNum)) {
        if ((temp_v1 << 16) > 0) {
            *keyStat = 1;
        } else {
            *keyStat = 3;
        }
    } else if ((temp_v1 << 16) > 0) {
        *keyStat = 2;
    } else {
        *keyStat = 0;
    }
}

