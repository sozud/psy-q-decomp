#include "libspu_i.h"

unsigned long SpuSetTransferStartAddr(unsigned long addr) {
    _spu_tsa = _spu_FsetRXXa(-1, addr);
    return addr;
}