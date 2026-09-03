#include "libspu_i.h"
long SpuGetKeyStatus(unsigned long voice_bit) {
    s32 var_a1;
    s32 var_v1;
    u16 temp_a0;

    var_a1 = -1;
    for (var_v1 = 0; var_v1 < 24; var_v1++) {
        if (voice_bit & (1 << var_v1)) {
            var_a1 = var_v1;
            break;
        }
    }

    if (var_a1 == -1) {
        return -1;
    }

    temp_a0 = _spu_RXX->raw[var_a1 * 8 + 6];
    if (_spu_keystat & (1 << var_a1)) {
        if (temp_a0 != 0) {
            return 1;
        } else {
            return 3;
        }
    } else {
        return (temp_a0 != 0) * 2;
    }
}

