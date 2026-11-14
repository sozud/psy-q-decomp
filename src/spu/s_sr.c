#if VERSION <= 30
#include "spu_old.h"

long SpuSetReverb(long on_off) {
    switch (on_off) {
    case SPU_OFF:
        _spu_rev_flag = SPU_OFF;
        _spu_ioctl(_spu_fd, 0x60, &_spu_rev_flag);
        break;
    case SPU_ON:
        if ((_spu_rev_reserve_wa != on_off) &&
            (_SpuIsInAllocateArea_(_spu_rev_offsetaddr) != 0)) {
            _spu_rev_flag = SPU_OFF;
            _spu_ioctl(_spu_fd, 0x60, &_spu_rev_flag);
        } else {
            _spu_rev_flag = on_off;
            _spu_ioctl(_spu_fd, 0x60, &_spu_rev_flag);
        }
        break;
    }
    return _spu_rev_flag;
}
#elif VERSION >= 41
#include "libspu_i.h"

long SpuSetReverb(long on_off) {
    switch (on_off) {
    case 0:
        _spu_rev_flag = 0;
        _spu_RXX->rxx.spucnt &= ~0x80;
        break;
    case 1:
        if ((_spu_rev_reserve_wa != on_off) &&
            (_SpuIsInAllocateArea_(_spu_rev_offsetaddr) != 0)) {
            _spu_rev_flag = 0;
            _spu_RXX->rxx.spucnt &= ~0x80;
        } else {
            _spu_rev_flag = on_off;
            _spu_RXX->rxx.spucnt |= 0x80;
        }
        break;
    }
    return _spu_rev_flag;
}
#else
#include "libspu_i.h"

long SpuSetReverb(long on_off) {
    unsigned int new_var;
    u16 var_v1;
    switch (on_off) {
    case 0:
        _spu_rev_flag = 0;
        var_v1 = _spu_RXX->rxxnv.spucnt;
        var_v1 = var_v1 & 0xFF7F;
        _spu_RXX->rxxnv.spucnt = var_v1;
        break;

    case 1:
        if ((_spu_rev_reserve_wa != on_off) &&
            (_SpuIsInAllocateArea_(_spu_rev_offsetaddr) != 0)) {
            _spu_rev_flag = 0;
            var_v1 = _spu_RXX->rxxnv.spucnt;
            var_v1 = var_v1 & 0xFF7F;
            new_var = var_v1;
            _spu_RXX->rxxnv.spucnt = new_var;
        } else {
            _spu_rev_flag = on_off;
            var_v1 = (new_var = _spu_RXX->rxxnv.spucnt) | 0x80;
            _spu_RXX->rxxnv.spucnt = var_v1;
        }
        break;
    }

    return _spu_rev_flag;
}
#endif
