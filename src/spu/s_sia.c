#include "libspu_i.h"
unsigned long SpuSetIRQAddr(unsigned long arg0) {
    if (arg0 > 0x7FFF8) {
        return 0;
    } else {
        return _spu_FsetRXXa(0xD2, arg0);
    }
}

