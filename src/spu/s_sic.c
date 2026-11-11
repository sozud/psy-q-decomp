#if VERSION <= 30
#include "spu_old.h"

SpuIRQCallbackProc SpuSetIRQCallback(SpuIRQCallbackProc func) {
    return (SpuIRQCallbackProc)_spu_ioctl(_spu_fd, 0x15, func);
}
#else
#include "libspu_i.h"

SpuIRQCallbackProc SpuSetIRQCallback(SpuIRQCallbackProc func) {
    void (*temp_s0)();

    temp_s0 = _spu_IRQCallback;
    if (func != temp_s0) {
        _spu_IRQCallback = func;
        _SpuCallback(func);
    }
    return temp_s0;
}
#endif
