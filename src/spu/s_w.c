#include "libspu_i.h"

#if VERSION == 40
unsigned long SpuWrite(u8 *addr, unsigned long size) {
    if (size > 0x7EFF0) {
        size = 0x7EFF0;
    }

    _spu_Fw(addr, size);
    if (_spu_transferCallback == NULL) {
        _spu_inTransfer = 0;
    }

    return size;
}

#else

unsigned long SpuWrite(unsigned char* addr, unsigned long size) {
    u32 var_s0;

    var_s0 = size;
    if (var_s0 > 0x7F000U) {
        var_s0 = 0x7F000;
    }
    _spu_write(addr, var_s0);
    if (_spu_transferCallback == 0) {
        _spu_inTransfer = 0;
    }
    return var_s0;
}
#endif
