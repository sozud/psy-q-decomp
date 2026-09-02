#include "libspu_i.h"
long SpuSetMute(long on_off) {
    switch (on_off) {
        case 0:
            _spu_RXX->rxx.spucnt |= 0x4000;
            break;
        case 1:
            _spu_RXX->rxx.spucnt &= ~0x4000;
            break;
    }

    return on_off;
}

