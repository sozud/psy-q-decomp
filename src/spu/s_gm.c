#include "libspu_i.h"
long SpuGetMute(void) {
    if (_spu_RXX->rxx.spucnt & 0x4000) {
        return 0;
    } else {
        return 1;
    }
}

