#if VERSION <= 30
#include "spu_old.h"

#if VERSION == 26
static void _wait(void) {
    volatile long i;
    volatile long v;

    v = 1;
    for (i = 0; i < 2; i++) {
        v *= 13;
    }
}

long SpuSetIRQ(long on_off) {
    switch (on_off) {
    case 1:
        _spu_ioctl(_spu_fd, 0x10, 1);
        _wait();
        break;
    case 0:
        _spu_ioctl(_spu_fd, 0x10, 0);
        _wait();
        break;
    case 3:
        _spu_ioctl(_spu_fd, 0x10, 0);
        _wait();
        _spu_ioctl(_spu_fd, 0x10, 1);
        break;
    }
    return on_off;
}
#else
long SpuSetIRQ(long on_off) {
    switch (on_off) {
    case 0:
        _spu_ioctl(_spu_fd, 0x10, 0);
        break;
    case 3:
        _spu_ioctl(_spu_fd, 0x10, 0);

    case 1:
        _spu_ioctl(_spu_fd, 0x10, 1);
        break;
    }
    return on_off;
}
#endif
#else
// SPDX-License-Identifier: MIT
#if VERSION >= 36
#if VERSION == 36
#include "libspu_40.h"
#else
#include "libspu_i.h"
#endif
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
#else
#include "libspu_i.h"

long SpuSetIRQ(long arg0) {
    u32 var_v1;

    if ((arg0 == 0) || (arg0 == 3)) {
        _spu_RXX->rxx.spucnt &= ~0x40;
        for (var_v1 = 0; _spu_RXX->rxx.spucnt & 0x40; var_v1++) {
            if (var_v1 + 1 > 0xF00) {
                printf("SPU: time out: wait (IRQ/ON)");
                return -1;
            }
        }
    }
    if ((arg0 == 1) || (arg0 == 3)) {
        _spu_RXX->rxx.spucnt |= 0x40;
        for (var_v1 = 0; !(_spu_RXX->rxx.spucnt & 0x40); var_v1++) {
            if (var_v1 + 1 > 0xF00) {
                printf("SPU: time out: wait (IRQ/OFF)");
                return -1;
            }
        }
    }
    return arg0;
}
#endif
#endif
