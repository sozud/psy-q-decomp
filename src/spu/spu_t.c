s32 _spu_t(s32 arg0, ...) {
    s32 var_a2;
    s32 i;
    u32 addr;
    va_list args;
    u32 count;
    u16 ck2;
    u16 cnt;

    va_start(args, arg0);

    switch (arg0) {
    case 2:
        count = va_arg(args, u32);
        _spu_tsa = count >> _spu_mem_mode_plus;
        _spu_RXX->rxx.trans_addr = _spu_tsa;
        break;

    case 1:
        _spu_dma_mode = 0;
        i = 0;
        while (_spu_RXX->rxx.trans_addr != _spu_tsa) {
            if (++i > 0xF00) {
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

        i = 0;
        while (_spu_RXX->rxx.trans_addr != _spu_tsa) {
            if (++i > 0xF00) {
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
            ck2 = 0x30;
        } else {
            ck2 = 0x20;
        }

        i = 0;
        while ((_spu_RXX->rxx.spucnt & 0x30) != ck2) {
            if (++i > 0xF00) {
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
