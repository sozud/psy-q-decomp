#if VERSION >= 36
static void _spu_FwriteByIO(void* _arg0, u32 arg1) {
    u16 temp_a1_2;
    s32 var_v1;
    s32 var_s0;
    u16 cnt;
    u16* arg0 = _arg0;

    temp_a1_2 = _spu_RXX->rxx.spustat & 0x7FF;
    _spu_RXX->rxx.trans_addr = _spu_tsa;
    _spu_Fw1ts();

    for (; arg1; arg1 -= var_s0)
    {
#if VERSION == 36
        if (arg1 > 0x40)
        {
            var_s0 = 0x40;
        }
        else
        {
            var_s0 = arg1;
        }
#else
        if (arg1 <= 0x40)
        {
            var_s0 = arg1;
        }
        else
        {
            var_s0 = 0x40;
        }
#endif

        for (var_v1 = 0; var_v1 < var_s0; var_v1 += 2)
        {
            _spu_RXX->rxx.trans_fifo = *arg0++;
        }

        cnt = _spu_RXX->rxx.spucnt;
        cnt &= ~0x30;
        cnt |= 0x10;
        _spu_RXX->rxx.spucnt = cnt;
        _spu_Fw1ts();

        for (var_v1 = 0; _spu_RXX->rxx.spustat & 0x400; )
        {
            if (++var_v1 > 0xF00U)
            {
                printf("SPU:T/O [%s]\n", "wait (wrdy H -> L)");
                break;
            }
        }

        _spu_Fw1ts();
        _spu_Fw1ts();
    }

    cnt = _spu_RXX->rxx.spucnt;
    cnt &= ~0x30;
    cnt |= 0x0;
    _spu_RXX->rxx.spucnt = cnt;

    for (var_v1 = 0; (_spu_RXX->rxx.spustat & 0x7FF) != temp_a1_2;)
    {
        if (++var_v1 > 0xF00U)
        {
            printf("SPU:T/O [%s]\n", "wait (dmaf clear/W)");
            break;
        }
    }
}

static int _spu_dma_mode = 0;
static s32 _spu_transfer_startaddr = 0;
static s32 _spu_transfer_time = 0;

void _spu_FiDMA(void) {
    s32 i;

    if (_spu_dma_mode == 0)
    {
        _spu_Fw1ts();
#if VERSION == 36 || VERSION == 37
        _spu_Fw1ts();
        _spu_Fw1ts();
#endif
    }

    _spu_RXX->rxx.spucnt &= ~0x30;

    for(i = 0; _spu_RXX->rxx.spucnt & 0x30; )
    {
        if (++i > 0xF00U) {
            break;
        }
    }

    if (_spu_transferCallback)
    {
        _spu_transferCallback();
    }
    else
    {
        DeliverEvent(0xF0000009, 0x20);
    }
}

void _spu_Fr_(s32 arg0, u16 arg1, s32 arg2) {
    _spu_RXX->rxx.trans_addr = arg1;
    _spu_Fw1ts();
#if VERSION == 36 || VERSION == 37
    _spu_Fw1ts();
#endif
    _spu_RXX->rxx.spucnt |= 0x30;
    _spu_Fw1ts();
#if VERSION == 36 || VERSION == 37
    _spu_Fw1ts();
#endif
    _spu_FsetDelayR();
    *_spu_madr = arg0;
    *_spu_bcr = (arg2 << 0x10) | 0x10;
    _spu_dma_mode = 1;
    *_spu_chcr = 0x01000200;
}

s32 _spu_t(s32 arg0, ...) {
    u16 var_a0;
    s32 var_a2;
    u32 var_v1_3;
    va_list args;
    u32 count;
    u16 cnt;

    va_start(args, arg0);

    switch (arg0) {
        case 2:
            count = va_arg(args, u32);
            _spu_RXX->rxx.trans_addr = _spu_tsa = count >> _spu_mem_mode_plus;
        break;
        case 1:
            _spu_dma_mode = 0;

            for (var_v1_3 = 0; _spu_RXX->rxx.trans_addr != _spu_tsa;) {
                if (++var_v1_3 > 0xF00U) {
                    return -2;
                }
            }

            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~0x30;
            cnt |= 0x20;
            _spu_RXX->rxx.spucnt = cnt;
        break;
        case 0:
            _spu_dma_mode = 1;

            for (var_v1_3 = 0; _spu_RXX->rxx.trans_addr != _spu_tsa;) {
                if (++var_v1_3 > 0xF00U) {
                    return -2;
                }
            }

            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~0x30;
            cnt |= 0x30;
            _spu_RXX->rxx.spucnt = cnt;
        break;
        case 3:
            if (_spu_dma_mode == 1) {
                var_a0 = 0x30;
            } else {
                var_a0 = 0x20;
            }

            for (var_v1_3 = 0; (_spu_RXX->rxx.spucnt & 0x30) != var_a0;) {
                if (++var_v1_3 > 0xF00U) {
                    return -2;
                }
            }

            if (_spu_dma_mode == 1) {
                _spu_FsetDelayR();
            } else {
                _spu_FsetDelayW();
            }

            count = va_arg(args, u32);
            _spu_transfer_startaddr = count;
            count = va_arg(args, u32);
            _spu_transfer_time = (count / 64);
            _spu_transfer_time += ((count % 64) ? 1 : 0);
            *_spu_madr = _spu_transfer_startaddr;
            *_spu_bcr = (_spu_transfer_time << 0x10) | 0x10;
            if (_spu_dma_mode == 1) {
                var_a2 = 0x01000200;
            } else {
                var_a2 = 0x01000201;
            }
            *_spu_chcr = var_a2;
        break;
    }

    return 0;
}

u32 _spu_Fw(void* arg0, u32 arg1) {
    if (_spu_transMode == 0) {
        _spu_t(2, _spu_tsa << _spu_mem_mode_plus);
        _spu_t(1);
        _spu_t(3, arg0, arg1);
    } else {
        _spu_FwriteByIO(arg0, arg1);
    }

    return arg1;
}

u32 _spu_Fr(u32 arg0, u32 arg1) {
    _spu_t(2, _spu_tsa << _spu_mem_mode_plus);
    _spu_t(0);
    _spu_t(3, arg0, arg1);
    return arg1;
}

#else
static s32 _spu_dma_mode = 0;
static s32 _spu_transfer_startaddr = 0;
static s32 _spu_transfer_time = 0;

void _spu_FiDMA(void) {
    volatile s32 sp0;
    volatile s32 sp4;
    s32 i;

    if (_spu_dma_mode == 0) {
        WASTE_TIME();
        WASTE_TIME();
        WASTE_TIME();
    }

    _spu_RXX->rxx.spucnt &= 0xFFCF;

    i = 0;
    while (_spu_RXX->rxx.spucnt & 0x30) {
        if (++i > 0xF00) {
            break;
        }
    }

    if (_spu_transferCallback) {
        _spu_transferCallback();
        return;
    }
    DeliverEvent(HwSPU, EvSpCOMP);
}

#include "spu_sb_d.c"
#include "spu_t.c"

s32 _spu_write(u8* arg0, u32 size) {

    if (_spu_transMode != 0) {
        _spu_writeByIO(arg0, size);
        return size;
    }
    _spu_t(2, _spu_tsa << _spu_mem_mode_plus);
    _spu_t(1);
    _spu_t(3, arg0, size);
    return size;
}

s32 _spu_read(s32 arg0, u32 size) {
    _spu_t(2, _spu_tsa << _spu_mem_mode_plus);
    _spu_t(0);
    _spu_t(3, arg0, size);
    return size;
}
#endif
