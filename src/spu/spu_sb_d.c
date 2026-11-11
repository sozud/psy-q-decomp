static inline void _spu_FsetDelayW(void) {
    *_spu_delay = (*_spu_delay & 0xF0FFFFFF) | 0x20000000;
}

static inline void _spu_FsetDelayR(void) {
    *_spu_delay = (*_spu_delay & 0xF0FFFFFF) | 0x22000000;
}

void _spu_r_(s32 arg0, u16 arg1, s32 arg2) {
    volatile s32 sp0;
    volatile s32 sp4;

    _spu_RXX->rxx.trans_addr = arg1;

    WASTE_TIME();
    WASTE_TIME();

    _spu_RXX->rxx.spucnt |= 0x30;

    WASTE_TIME();
    WASTE_TIME();

    _spu_FsetDelayR();
    *_spu_madr = arg0;
    *_spu_bcr = (arg2 << 0x10) | 0x10;
    _spu_dma_mode = 1;
    *_spu_chcr = 0x01000200;
}
