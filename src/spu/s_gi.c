#include "libspu_i.h"
long SpuGetIRQ(void) {
    if (_spu_RXX->rxx.spucnt & 0x40) {
        return 1;
    } else {
        return 0;
    }
}

