#if VERSION <= 30
#include "spu_old.h"

long SpuSetNoiseClock(long n_clock) {
    long v;

    if (n_clock < 0) {
        v = 0;
    } else if (n_clock >= 0x40) {
        v = 0x3F;
    } else {
        v = n_clock;
    }
    _spu_ioctl(_spu_fd, 0x50, &v);
    return v;
}
#else /* VERSION > 30 */
#include "libspu_i.h"

long SpuSetNoiseClock(long n_clock) {
    long clamped;
    u16 controlRegister;

    if (n_clock < 0) {
        clamped = 0;
    } else if (n_clock > 0x3F) {
        clamped = 0x3F;
    } else {
        clamped = n_clock;
    }

    controlRegister = _spu_RXX->rxx.spucnt;
    controlRegister &= ~0x3F00;
    controlRegister |= ((clamped & 0x3F) << 8);
    _spu_RXX->rxx.spucnt = controlRegister;

    return clamped;
}
#endif /* VERSION > 30 */
