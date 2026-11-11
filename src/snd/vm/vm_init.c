void SpuVmInit(u8 arg0) {
    u16 temp_v1;
    u16 var_a1;
    u16* temp_a0;
    s32 var_a0;
    u16 bitsUpper;
    u16 bitsLower;
    u16 temp;
    _spu_setInTransfer(0);
    _svm_vcf = 0;
    _svm_damper = 0;
    SpuInitMalloc(0x20, &_ss_spu_vm_rec[2]);
    for (var_a1 = 0; var_a1 < 8 * NUM_SPU_CHANNELS; var_a1++) {
        _svm_sreg_buf[var_a1] = 0;
    }
    for (var_a1 = 0; var_a1 < NUM_SPU_CHANNELS; var_a1++) {
        _svm_sreg_dirty[var_a1] = 0;
    }

    _svm_vab_count = 0;

    for (var_a1 = 0; var_a1 < 16; var_a1++) {
        _svm_vab_used[var_a1] = 0;
    }

    var_a0 = arg0;
    if (arg0 >= NUM_SPU_CHANNELS) {
        spuVmMaxVoice = NUM_SPU_CHANNELS;
    } else {
        spuVmMaxVoice = var_a0;
    }
    for (var_a1 = 0; var_a1 < spuVmMaxVoice; var_a1++) {
        temp = var_a1 * 8;
        _svm_voice[var_a1].unk2 = 0x18;
#if VERSION >= 35
        _svm_voice[var_a1].unke = -1;
#endif
        _svm_voice[var_a1].unk0 = 0xFF;
        _svm_voice[var_a1].unk1b = 0;
        _svm_voice[var_a1].unk04 = 0;
        _svm_voice[var_a1].unk6 = 0;
#if VERSION < 35
        _svm_voice[var_a1].unke = -1;
#endif
        _svm_voice[var_a1].unk10 = 0;
        _svm_voice[var_a1].prog = 0;
        _svm_voice[var_a1].tone = 0xff;
#if VERSION >= 35
        _svm_voice[var_a1].unk8 = 0;
        _svm_voice[var_a1].unka = 0x40;
#endif
        _svm_voice[var_a1].auto_vol = 0;
        _svm_voice[var_a1].unk1e = 0;
        _svm_voice[var_a1].unk20 = 0;
        _svm_voice[var_a1].unk22 = 0;
        _svm_voice[var_a1].auto_pan = 0;
        _svm_voice[var_a1].unk2a = 0;
        _svm_voice[var_a1].unk2c = 0;
        _svm_voice[var_a1].unk2e = 0;
        _svm_voice[var_a1].start_pan = 0;
        _svm_voice[var_a1].start_vol = 0;

        temp_a0 = &D_80032F10[temp];
        temp_a0[3] = 0x200;
        temp_a0[2] = 0x1000;
        temp_a0[4] = 0x80FF;
        temp_a0[0] = 0;
        temp_a0[1] = 0;
        temp_a0[5] = 0x4000;

        _svm_cur.field_0x1a = var_a1;
        temp_v1 = get_field_0x1a();
        if (temp_v1 < 0x10) {
            bitsLower = 1 << temp_v1;
            bitsUpper = 0;
        } else {
            bitsLower = 0;
            bitsUpper = 1 << (temp_v1 - 0x10);
        }

        _svm_voice[temp_v1].unk1b = 0;
        _svm_voice[temp_v1].unk04 = 0;
        _svm_voice[temp_v1].unk0 = 0;

        _svm_okof1 |= bitsLower;
        _svm_okof2 |= bitsUpper;

        _svm_okon1 &= ~_svm_okof1;
        _svm_okon2 &= ~_svm_okof2;
    }
    _svm_rattr.depth.left = 0x3FFF;
    _svm_rattr.depth.right = 0x3FFF;
    _svm_okon1 = 0;
    _svm_okon2 = 0;
    _svm_okof1 = 0;
    _svm_orev1 = 0;
    _svm_orev2 = 0;
    _svm_rattr.mask = 0;
    _svm_rattr.mode = 0;
    _svm_auto_kof_mode = 0;
    _svm_stereo_mono = 0;
    kMaxPrograms = 0x80;
    SpuVmFlush();
}
