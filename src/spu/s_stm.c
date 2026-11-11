#if VERSION <= 30
#include "spu_old.h"

long SpuSetTransferMode(long mode) {
    long v;

    switch (mode) {
    case 0:
        v = 1;
        break;
    case 1:
        v = 0;
        break;
    default:
        v = 1;
        break;
    }
    _spu_trans_mode = mode;
    _spu_ioctl(_spu_fd, 0x21, &v);
}
#else
#include "libspu_i.h"

long SpuSetTransferMode(long mode) {
    int temp;

    switch (mode) {
    case SPU_TRANSFER_BY_DMA:
        temp = 0;
        break;
    case SPU_TRANSFER_BY_IO:
        temp = 1;
        break;
    default:
        temp = 0;
    }
    _spu_trans_mode = mode;
    _spu_transMode = temp;
    return temp;
}
#endif
