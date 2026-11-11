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
#if VERSION >= 36
#define SPU_XFER_MAX 0x7EFF0
#else
#define SPU_XFER_MAX 0x7F000
#endif

unsigned long SpuWrite(unsigned char* addr, unsigned long size) {
    u32 var_s0;

    var_s0 = size;
    if (var_s0 > SPU_XFER_MAX) {
        var_s0 = SPU_XFER_MAX;
    }
    _spu_write(addr, var_s0);
#if VERSION == 26
    _spu_inTransfer = 0;
#else
    if (_spu_transferCallback == 0) {
        _spu_inTransfer = 0;
    }
#endif
    return var_s0;
}
#endif
