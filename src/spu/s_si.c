#include "libspu_i.h"
long SpuSetIRQ(long on_off) {
    u_long dmaTimer;

    if ((on_off == SPU_OFF) || (on_off == SPU_RESET)) {
        _spu_RXX->rxx.spucnt &= ~0x40;
        dmaTimer = 0;
        while (_spu_RXX->rxx.spucnt & 0x40) {
            if (++dmaTimer > 0xF00u) {
                printf("SPU:T/O [%s]\n", "wait (IRQ/ON)");
                return SPU_ERROR;
            }
        }

    }

    if ((on_off == SPU_ON) || (on_off == SPU_RESET)) {
        _spu_RXX->rxx.spucnt |= 0x40;
        dmaTimer = 0;
        while ( !(_spu_RXX->rxx.spucnt & 0x40) ) {
            if (++dmaTimer > 0xF00u) {
                printf("SPU:T/O [%s]\n", "wait (IRQ/OFF)");
                return SPU_ERROR;
            }
        }
    }
    return on_off;
}

