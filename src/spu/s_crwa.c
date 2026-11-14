#if VERSION <= 30
#if VERSION == 26
static char rcsid[] = "$Id: s_crwa.c,v 1.7 1995/01/23 03:35:08 kaol Exp $ : \tCopyright (C) by 1993, 1994 Sony Computer Entertainment Inc.";
#else
static char rcsid[] = "$Id: s_crwa.c,v 1.8 1995/03/11 08:24:16 kaol Exp $ : \tCopyright (C) by 1994-95 Sony Computer Entertainment Inc.";
#endif

#define SPU_OLD_LOCAL_RXX
#include "spu_old.h"

long SpuClearReverbWorkArea(long mode) {
    unsigned long n;
    unsigned long size;
    unsigned long addr;
    long more;
    long restore;
    long trans;
    long io;

    more = 1;
    restore = 0;
    n = (_spu_RXX->rxx.data_trans & 0xE) >> 1;
    n++;
    if ((unsigned long)mode >= 10 || _SpuIsInAllocateArea_(_spu_rev_startaddr[mode])) {
        return -1;
    }
    size = (0x10000 - _spu_rev_startaddr[mode]) << n;
    addr = _spu_rev_startaddr[mode] << n;
    _spu_ioctl(_spu_fd, 0x2F, &trans);
    if (trans == 0) {
        io = 1;
        _spu_ioctl(_spu_fd, 0x2E, &io);
        restore = 1;
    }
    do {
        if (size > 0x400) {
            n = 0x400;
        } else {
            n = size;
            more = 0;
        }
        SpuSetTransferStartAddr(addr);
        SpuWrite(_spu_zerobuf, n);
        SpuIsTransferCompleted(1);
        size -= 0x400;
        addr += 0x400;
    } while (more == 1);
    if (restore) {
        _spu_ioctl(_spu_fd, 0x2E, &trans);
    }
    return 0;
}
#elif VERSION >= 41
#include "libspu_i.h"

long SpuClearReverbWorkArea(long rev_mode) {
    volatile SpuTransferCallbackProc callback;
    s32 oldTransmode;
    s32 dstAddr;
    s32 more;
    s32 transmodeCleared;
    u32 size;
    u32 length;

    callback = 0;
    transmodeCleared = 0;
    if (rev_mode < 0 || rev_mode >= 10 ||
        _SpuIsInAllocateArea_(_spu_rev_startaddr[rev_mode])) {
        return -1;
    }
    if (rev_mode == 0) {
        length = 0x10 << _spu_mem_mode_plus;
        dstAddr = 0xFFF0 << _spu_mem_mode_plus;
    } else {
        length = (0x10000 - _spu_rev_startaddr[rev_mode]) << _spu_mem_mode_plus;
        dstAddr = _spu_rev_startaddr[rev_mode] << _spu_mem_mode_plus;
    }
    oldTransmode = _spu_transMode;
    if (_spu_transMode == 1) {
        _spu_transMode = 0;
        transmodeCleared = 1;
    }
    more = 1;
    if (_spu_transferCallback != 0) {
        callback = _spu_transferCallback;
        _spu_transferCallback = 0;
    }
    while (more != 0) {
        if (length > 0x400) {
            size = 0x400;
        } else {
            size = length;
            more = 0;
        }
        _spu_t(2, dstAddr);
        _spu_t(1);
        _spu_t(3, _spu_zerobuf, size);
        WaitEvent(_spu_EVdma);
        length -= 0x400;
        dstAddr += 0x400;
    }
    if (transmodeCleared != 0) {
        _spu_transMode = oldTransmode;
    }
    if (callback != 0) {
        _spu_transferCallback = callback;
    }
    return 0;
}
#else /* VERSION > 30 */
#include "libspu_i.h"

long SpuClearReverbWorkArea(long mode) {
    volatile s32 callback;
    s32 oldTransmode;
    s32 var_s2;
    s32 var_s3;
    s32 transmodeCleared;
    u32 var_s0;
    u32 var_s1;

    callback = 0;
    transmodeCleared = 0;
    if ((mode >= 0xAU) ||
        (_SpuIsInAllocateArea_(_spu_rev_startaddr[mode]) != 0)) {
        return -1;
    }
    if (mode == 0) {
        var_s1 = 0x10 << _spu_mem_mode_plus;
        var_s2 = 0xFFF0 << _spu_mem_mode_plus;
    } else {
        var_s1 = (0x10000 - _spu_rev_startaddr[mode]) << _spu_mem_mode_plus;
        var_s2 = _spu_rev_startaddr[mode] << _spu_mem_mode_plus;
    }
    oldTransmode = _spu_transMode;
    if (_spu_transMode == 1) {
        _spu_transMode = 0;
        transmodeCleared = 1;
    }
    var_s3 = 1;
    if (_spu_transferCallback != 0) {
        callback = _spu_transferCallback;
        _spu_transferCallback = 0;
    }
    do {
        var_s0 = var_s1;
        if (!(var_s1 < 0x401U)) {
            var_s0 = 0x400;
        } else {
            var_s3 = 0;
        }
        _spu_t(2, var_s2);
        _spu_t(1);
        _spu_t(3, &_spu_zerobuf, var_s0);
        WaitEvent(_spu_EVdma);
        var_s1 -= 0x400;
        var_s2 += 0x400;
    } while (var_s3 != 0);
    if (transmodeCleared != 0) {
        _spu_transMode = oldTransmode;
    }
    if (callback != 0) {
        _spu_transferCallback = callback;
    }
    return 0;
}
#endif /* VERSION > 30 */
