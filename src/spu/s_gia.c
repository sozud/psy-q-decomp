#include "libspu_i.h"
unsigned long SpuGetIRQAddr(void) {
    return _spu_FgetRXXa(0xD2, -2);
}

