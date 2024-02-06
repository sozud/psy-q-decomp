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
