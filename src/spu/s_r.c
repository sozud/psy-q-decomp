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
#if VERSION >= 36
#define SPU_XFER_MAX 0x7EFF0
#else
#define SPU_XFER_MAX 0x7F000
#endif

unsigned long SpuRead(unsigned char* addr, unsigned long size) {
    if (size > SPU_XFER_MAX) {
        size = SPU_XFER_MAX;
    }
    _spu_read(addr, size);
#if VERSION == 26
    _spu_inTransfer = 0;
#else
    if (_spu_transferCallback == 0) {
        _spu_inTransfer = 0;
    }
#endif
    return size;
}
#endif
