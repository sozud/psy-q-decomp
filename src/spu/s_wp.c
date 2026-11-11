#if VERSION <= 30
#include "spu_old.h"

unsigned long SpuWritePartly(unsigned char *addr, unsigned long size) {
    unsigned long start;

    if (size > 0x7F000) {
        size = 0x7F000;
    }
    start = SpuGetTransferStartAddr();
    _spu_write(addr, size);
    start += size;
    _spu_ioctl(_spu_fd, 0x2B, &start);
#if VERSION == 26
    _spu_inTransfer = 0;
#else
    if (_spu_transferCallback == NULL) {
        _spu_inTransfer = 0;
    }
#endif
    return size;
}
#else
#if VERSION == 40
#include "libspu_i.h"
unsigned long SpuWritePartly(unsigned char *addr, unsigned long size) {
    s32 temp_s0;

    if (size > 0x7EFF0) {
        size = 0x7EFF0;
    }

    temp_s0 = _spu_tsa << _spu_mem_mode_plus;
    _spu_Fw(addr, size);

    _spu_tsa = _spu_FsetRXXa(-1, temp_s0 + size);

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

#include "libspu_i.h"

unsigned long SpuWritePartly(unsigned char* addr, unsigned long size) {
    s32 temp_s0;
    u32 var_s1;

    var_s1 = size;
    if (var_s1 > SPU_XFER_MAX) {
        var_s1 = SPU_XFER_MAX;
    }
    size = var_s1;
    temp_s0 = _spu_tsa << _spu_mem_mode_plus;
    _spu_write(addr, size);
    _spu_tsa = _spu_FsetRXXa(-1, temp_s0 + var_s1);
    if (_spu_transferCallback == 0) {
        _spu_inTransfer = 0;
    }
    return var_s1;
}
#endif
#endif
