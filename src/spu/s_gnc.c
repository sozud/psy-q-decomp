#include "libspu_i.h"
long SpuGetNoiseClock(void) {
    return (_spu_RXX->rxx.spucnt & 0x3F00) >> 8;
}

