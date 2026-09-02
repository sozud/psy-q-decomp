#include "libspu_i.h"

#if VERSION == 40
unsigned long SpuRead(unsigned char *addr, unsigned long size) {
    if (size > 0x7EFF0) {
        size = 0x7EFF0;
    }

    _spu_Fr(addr, size);

    if (_spu_transferCallback == NULL) {
        _spu_inTransfer = 0;
    }

    return size;
}

#else

unsigned long SpuRead(unsigned char* addr, unsigned long size) {
    if (size > 0x7F000U) {
        size = 0x7F000;
    }
    _spu_read(addr, size);
    if (_spu_transferCallback == 0) {
        _spu_inTransfer = 0;
    }
    return size;
}
#endif
