#include "libspu_i.h"
unsigned long SpuGetTransferStartAddr(void) {
    return _spu_tsa << _spu_mem_mode_plus;
}

