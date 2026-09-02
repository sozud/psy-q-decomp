#include "libspu_i.h"

#if VERSION == 40

ulong SpuSetTransferStartAddr(ulong addr) {
    ulong offset;
    u16 base_addr;

    offset = addr - 0x1010;
    if (offset > 0x7efe8) {
        return 0;
    }

    base_addr = _spu_FsetRXXa(0xFFFFFFFF, addr);
    _spu_tsa = base_addr;
    return (ulong)base_addr << _spu_mem_mode_plus;
}

#else

unsigned long SpuSetTransferStartAddr(unsigned long addr) {
    _spu_tsa = _spu_FsetRXXa(-1, addr);
    return addr;
}
#endif
