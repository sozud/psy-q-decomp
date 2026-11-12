#if VERSION >= 36
s32 _spu_init(s32 arg0) {
    u32 wait_count;
    int i;
    static u8 _spu_dummy[16] = {
      7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7
    };

    *_spu_sys_pcr |= 0xB0000;
    _spu_RXX->rxx.main_vol.left = 0;
    _spu_RXX->rxx.main_vol.right = 0;
    _spu_RXX->rxx.spucnt = 0;
    _spu_transMode = 0;
    _spu_addrMode = 0;
    _spu_tsa = 0;
    _spu_Fw1ts();
    _spu_RXX->rxx.main_vol.left = 0;
    _spu_RXX->rxx.main_vol.right = 0;

    for (wait_count = 0; _spu_RXX->rxx.spustat & 0x7FF; wait_count++)
    {
        if (wait_count + 1 > 0xF00)
        {
            printf("SPU:T/O [%s]\n", "wait (reset)");
            break;
        }
    }

    _spu_mem_mode = 2;
    _spu_mem_mode_plus = 3;
    _spu_mem_mode_unit = 8;
    _spu_mem_mode_unitM = 7;
    _spu_RXX->rxx.data_trans = 4;
    _spu_RXX->rxx.rev_vol.left = 0;
    _spu_RXX->rxx.rev_vol.right = 0;
    _spu_RXX->rxx.key_off[0] = 0xFFFF;
    _spu_RXX->rxx.key_off[1] = 0xFFFF;
    _spu_RXX->rxx.rev_mode[0] = 0;
    _spu_RXX->rxx.rev_mode[1] = 0;

    for (i = 0; i < 10; i++)
    {
        _spu_RQ[i] = 0;
    }

    if (arg0 == 0)
    {
        _spu_RXX->rxx.chan_fm[0] = 0;
        _spu_RXX->rxx.chan_fm[1] = 0;
        _spu_RXX->rxx.noise_mode[0] = 0;
        _spu_RXX->rxx.noise_mode[1] = 0;
        _spu_RXX->rxx.cd_vol.left = 0;
        _spu_RXX->rxx.cd_vol.right = 0;
        _spu_RXX->rxx.ex_vol.left = 0;
        _spu_RXX->rxx.ex_vol.right = 0;
        _spu_tsa = 0x200;

        _spu_FwriteByIO(&_spu_dummy, ARRAY_COUNT(_spu_dummy));

        for(i = 0; i < 24; i++)
        {
            _spu_RXX->rxx.voice[i].volume.left = 0;
            _spu_RXX->rxx.voice[i].volume.right = 0;
            _spu_RXX->rxx.voice[i].pitch = 0x3fff;
            _spu_RXX->rxx.voice[i].addr = 0x200;
            _spu_RXX->rxx.voice[i].adsr[0] = 0;
            _spu_RXX->rxx.voice[i].adsr[1] = 0;
        }

        _spu_RXX->rxx.key_on[0] = 0xFFFF;
        _spu_RXX->rxx.key_on[1] = 0xFF;
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_RXX->rxx.key_off[0] = 0xFFFF;
        _spu_RXX->rxx.key_off[1] = 0xFF;
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
        _spu_Fw1ts();
    }

    _spu_inTransfer = 1;
    _spu_RXX->rxx.spucnt = 0xC000;
    _spu_transferCallback = NULL;
    _spu_IRQCallback = NULL;
    return 0;
}

#else
s32 _spu_reset(void) {
    volatile s32 sp0;
    volatile s32 sp4;
    u16 cnt;

    cnt = _spu_RXX->rxx.spucnt;
    _spu_RXX->rxx.spucnt = cnt & 0x7FCF;
    WASTE_TIME();
    cnt &= 0xFFCF;
    _spu_RXX->rxx.spucnt = cnt;
    return 0;
}

static u16 _spu_dummy[] = {
    0x0707, 0x0707, 0x0707, 0x0707, 0x0707, 0x0707, 0x0707, 0x0707};

extern s32 _spu_addrMode;
extern s32 _spu_mem_mode;
extern s32 _spu_mem_mode_unit;

#if VERSION == 33
typedef struct {
    u32 voice;
    u32 mask;
    u16 vol_l;
    u16 vol_r;
    u16 pad0[2];
    u16 pitch;
    u16 pad1;
    u32 addr;
    u16 loop_addr;
    u16 pad2;
    u32 adsr;
} SpuVoiceAttrInit;

static void _spu_setVoiceAttr(SpuVoiceAttrInit* attr);

static inline void _spu_wait(void) {
    volatile s32 count;
    volatile s32 value;

    value = 13;
    for (count = 0; count < 0xF0; count++) {
        value *= 3;
    }
}

s32 _spu_init(s32 arg0) {
    SpuVoiceAttrInit attr;

    _spu_transMode = 0;
    _spu_addrMode = 0;
    _spu_tsa = 0;
    *_spu_sys_pcr |= 0xB0000;
    _spu_RXX->rxx.spucnt = 0;
    _spu_wait();
    D_800334FC = 0;
    while (_spu_RXX->rxx.spustat & 0x7FF) {
        if (++D_800334FC > 5000) {
            printf("SPU:T/O [%s]\n", "wait (reset)");
            break;
        }
    }
    _spu_mem_mode = 2;
    _spu_mem_mode_plus = 3;
    _spu_mem_mode_unit = 8;
    _spu_mem_mode_unitM = 7;
    _spu_RXX->rxx.data_trans = 4;
    _spu_RXX->rxx.main_vol.left = 0;
    _spu_RXX->rxx.main_vol.right = 0;
    _spu_RXX->rxx.rev_vol.left = 0;
    _spu_RXX->rxx.rev_vol.right = 0;
    _spu_RXX->rxx.key_off[0] = 0xFFFF;
    _spu_RXX->rxx.key_off[1] = 0xFFFF;
    _spu_RXX->rxx.rev_mode[0] = 0;
    _spu_RXX->rxx.rev_mode[1] = 0;
    if (arg0 == 0) {
        _spu_RXX->rxx.chan_fm[0] = 0;
        _spu_RXX->rxx.chan_fm[1] = 0;
        _spu_RXX->rxx.noise_mode[0] = 0;
        _spu_RXX->rxx.noise_mode[1] = 0;
        _spu_RXX->rxx.cd_vol.left = 0;
        _spu_RXX->rxx.cd_vol.right = 0;
        _spu_RXX->rxx.ex_vol.left = 0;
        _spu_RXX->rxx.ex_vol.right = 0;
        _spu_tsa = 0x200;
        _spu_writeByIO((u8*)&_spu_dummy[0], 0x10);
        attr.voice = 0xFFFFFFFF;
        attr.mask = 0x1F;
        attr.pitch = 0x3FFF;
        attr.vol_l = 0;
        attr.vol_r = 0;
        attr.addr = 0x200;
        attr.adsr = 0;
        _spu_addrMode = 0;
        _spu_setVoiceAttr(&attr);
        _spu_RXX->rxx.key_on[0] |= 0xFFFF;
        _spu_RXX->rxx.key_on[1] |= 0xFF;
        _spu_wait();
        _spu_wait();
        _spu_wait();
        _spu_wait();
        _spu_RXX->rxx.key_off[0] |= 0xFFFF;
        _spu_RXX->rxx.key_off[1] |= 0xFF;
        _spu_wait();
        _spu_wait();
        _spu_wait();
        _spu_wait();
    }
    _spu_inTransfer = 1;
    _spu_RXX->rxx.spucnt = 0xC000;
    _spu_transferCallback = NULL;
    _spu_IRQCallback = NULL;
    return 0;
}

static inline u16 _spu_addr(u32 addr) {
    if (_spu_mem_mode != 0 && addr % _spu_mem_mode_unit != 0) {
        addr += _spu_mem_mode_unit;
    }
    return addr >> _spu_mem_mode_plus;
}

static void _spu_setVoiceAttr(SpuVoiceAttrInit* attr) {
    u32 mask;
    s32 bSetAll;
    s32 voice;
    s32 pos;
    u32 addr;

    mask = attr->mask;
    bSetAll = mask == 0;
    for (voice = 0; voice < NUM_SPU_CHANNELS; voice++) {
        if ((attr->voice & (1 << voice)) == 0) {
            continue;
        }
        pos = voice * 8;
        if (bSetAll || (mask & 1)) {
            _spu_RXX->raw[pos + 0] = attr->vol_l;
        }
        if (bSetAll || (mask & 2)) {
            _spu_RXX->raw[pos + 1] = attr->vol_r;
        }
        if (bSetAll || (mask & 4)) {
            _spu_RXX->raw[pos + 2] = attr->pitch;
        }
        if (bSetAll || (mask & 8)) {
            if (_spu_addrMode == 0) {
                _spu_RXX->raw[pos + 3] = attr->addr;
            } else {
                _spu_RXX->raw[pos + 3] = _spu_addr(attr->addr);
            }
        }
        if (bSetAll || (mask & 0x10)) {
            _spu_RXX->raw[pos + 4] = attr->adsr >> 16;
        }
        if (bSetAll || (mask & 0x10)) {
            _spu_RXX->raw[pos + 5] = attr->adsr;
        }
        if (bSetAll || (mask & 0x100)) {
            _spu_RXX->raw[pos + 7] = attr->loop_addr;
        }
    }
}
#else
s32 _spu_init(s32 arg0) {
    volatile s32 sp0;
    volatile s32 sp4;
    s32 channel;

    *_spu_sys_pcr |= 0xB0000;
    _spu_RXX->rxx.main_vol.left = 0;
    _spu_RXX->rxx.main_vol.right = 0;
    _spu_RXX->rxx.spucnt = 0;
    _spu_transMode = 0;
    _spu_addrMode = 0;
    _spu_tsa = 0;
    WASTE_TIME();
    _spu_RXX->rxx.main_vol.left = 0;
    _spu_RXX->rxx.main_vol.right = 0;
    D_800334FC = 0;
    while (_spu_RXX->rxx.spustat & 0x7FF) {
        if (++D_800334FC > 5000) {
            printf("SPU:T/O [%s]\n", "wait (reset)");
            break;
        }
    }
    _spu_mem_mode = 2;
    _spu_mem_mode_plus = 3;
    _spu_mem_mode_unit = 8;
    _spu_mem_mode_unitM = 7;
    _spu_RXX->rxx.data_trans = 4;
    _spu_RXX->rxx.rev_vol.left = 0;
    _spu_RXX->rxx.rev_vol.right = 0;
    _spu_RXX->rxx.key_off[0] = 0xFFFF;
    _spu_RXX->rxx.key_off[1] = 0xFFFF;
    _spu_RXX->rxx.rev_mode[0] = 0;
    _spu_RXX->rxx.rev_mode[1] = 0;
    if (arg0 == 0) {
        _spu_RXX->rxx.chan_fm[0] = 0;
        _spu_RXX->rxx.chan_fm[1] = 0;
        _spu_RXX->rxx.noise_mode[0] = 0;
        _spu_RXX->rxx.noise_mode[1] = 0;
        _spu_RXX->rxx.cd_vol.left = 0;
        _spu_RXX->rxx.cd_vol.right = 0;
        _spu_RXX->rxx.ex_vol.left = 0;
        _spu_RXX->rxx.ex_vol.right = 0;
        _spu_tsa = 0x200;
        _spu_writeByIO((u8*)&_spu_dummy[0], 0x10);
        for (channel = 0; channel < NUM_SPU_CHANNELS; channel++) {
            _spu_RXX->rxx.voice[channel].volume.left = 0;
            _spu_RXX->rxx.voice[channel].volume.right = 0;
            _spu_RXX->rxx.voice[channel].pitch = 0x3fff;
            _spu_RXX->rxx.voice[channel].addr = 0x200;
            _spu_RXX->rxx.voice[channel].adsr[0] = 0;
            _spu_RXX->rxx.voice[channel].adsr[1] = 0;
        }
        _spu_RXX->rxx.key_on[0] |= 0xFFFF;
        _spu_RXX->rxx.key_on[1] |= 0xFF;
        WASTE_TIME();
        WASTE_TIME();
        WASTE_TIME();
        WASTE_TIME();
        _spu_RXX->rxx.key_off[0] |= 0xFFFF;
        _spu_RXX->rxx.key_off[1] |= 0xFF;
        WASTE_TIME();
        WASTE_TIME();
        WASTE_TIME();
        WASTE_TIME();
    }
    _spu_inTransfer = 1;
    _spu_RXX->rxx.spucnt = 0xC000;
    _spu_transferCallback = NULL;
    _spu_IRQCallback = NULL;
    return 0;
}
#endif
#endif
