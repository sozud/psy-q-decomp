#include "libspu_i.h"
long SpuGetReverb(void) {
    if (_spu_RXX->rxx.spucnt & 0x80) {
        return 1;
    } else {
        return 0;
    }
}

