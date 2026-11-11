static s32 _spu_writeByIO(u8* addr, u32 size) {
    volatile s32 sp0;
    volatile s32 sp4;
    u16 spustat;
    s32 num_to_trans;
    u16* cur_pos;
    s32 spustat_cur;
    s32 i;
    u16 cnt;

    cur_pos = addr;
    spustat = _spu_RXX->rxx.spustat & 0x7FF;
    _spu_RXX->rxx.trans_addr = _spu_tsa;
    WASTE_TIME();
    while (size != 0) {
        num_to_trans = (size > 0x40) ? 0x40 : size;
        for (i = 0; i < num_to_trans; i += 2) {
            _spu_RXX->rxx.trans_fifo = *cur_pos++;
        }
        cnt = _spu_RXX->rxx.spucnt;
        cnt &= ~0x30;
        cnt |= 0x10;
        _spu_RXX->rxx.spucnt = cnt;
        WASTE_TIME();
        D_800334FC = 0;
        while (_spu_RXX->rxx.spustat & 0x400) {
            D_800334FC++;
            if (D_800334FC > 5000) {
                printf("SPU:T/O [%s]\n", "wait (wrdy H -> L)");
                break;
            }
        }
        WASTE_TIME();
        WASTE_TIME();
        size -= num_to_trans;
    }
    cnt = _spu_RXX->rxx.spucnt;
    cnt &= ~0x30;
    _spu_RXX->rxx.spucnt = cnt;
    D_800334FC = 0;
    spustat_cur = _spu_RXX->rxx.spustat & 0x7FF;

    while (spustat_cur != spustat) {
        D_800334FC++;
        if (D_800334FC > 5000) {
            printf("SPU:T/O [%s]\n", "wait (dmaf clear/W)");
            return;
        }
        spustat_cur = _spu_RXX->rxx.spustat & 0x7FF;
    }
    return spustat_cur;
}
