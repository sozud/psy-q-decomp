#if VERSION <= 30
#include "spu_old.h"

#if VERSION == 26
unsigned long SpuSetTransferStartAddr(unsigned long addr) {
    if (addr > 0x7F000) {
        return 0;
    }
    if (addr < 0x1000) {
        return 0;
    }
    if (addr & 7) {
        addr = (addr + 8) & ~4;
    }
    _spu_ioctl(_spu_fd, 0x2B, &addr);
    return addr;
}
#else
unsigned long SpuSetTransferStartAddr(unsigned long addr) {
    if (addr > 0x7F000) {
        return 0;
    }
    _spu_ioctl(_spu_fd, 0x2B, &addr);
    return addr;
}
#endif
#else
#if VERSION == 36 || VERSION == 37
#include "libspu_40.h"
#else
#include "libspu_i.h"
#endif

#if VERSION >= 36

ulong SpuSetTransferStartAddr(ulong addr) {
    ulong offset;
    u16 base_addr;

    offset = addr - 0x1010;
    if (offset > 0x7efe8) {
        return 0;
    }

#if VERSION == 36
    _spu_tsa = _spu_FsetRXXa(0xFFFFFFFF, addr);
    return _spu_tsa;
#else
    base_addr = _spu_FsetRXXa(0xFFFFFFFF, addr);
    _spu_tsa = base_addr;
    return (ulong)base_addr << _spu_mem_mode_plus;
#endif
}

#else

unsigned long SpuSetTransferStartAddr(unsigned long addr) {
    _spu_tsa = _spu_FsetRXXa(-1, addr);
    return addr;
}
#endif
#endif
