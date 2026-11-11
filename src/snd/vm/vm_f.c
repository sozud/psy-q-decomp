void SpuVmFlush(void) {
    u32 env_mask;
    s32 i;

    _svm_envx_ptr = (_svm_envx_ptr + 1) & 0xF;
    _svm_envx_hist[_svm_envx_ptr] = 0;

    for (i = 0; i < spuVmMaxVoice; i++) {
        _svm_voice[i].unk6 = D_80032F10[i * 8 + 6];
        if (_svm_voice[i].unk6 == 0) {
            _svm_envx_hist[_svm_envx_ptr] |= 1 << i;
        }
    }
    if (_svm_auto_kof_mode == 0) {
        env_mask = 0xFFFFFFFF;
        for (i = 0; i < 0xF; i++) {
            env_mask &= _svm_envx_hist[i];
        }

        for (i = 0; i < spuVmMaxVoice; i++) {
            if (env_mask & (1 << i)) {
                if (_svm_voice[i].unk1b == 2) {
                    SpuSetNoiseVoice(0, 0xFFFFFF);
                }
                _svm_voice[i].unk1b = 0;
            }
        }
    }

    _svm_okon1 &= ~_svm_okof1;
    _svm_okon2 &= ~_svm_okof2;
    for (i = 0; i < NUM_SPU_CHANNELS; i++) {
        if (_svm_voice[i].auto_vol != 0) {
            SetAutoVol(i);
        }
        if (_svm_voice[i].auto_pan != 0) {
            SetAutoPan(i);
        }
    }

    for (i = 0; i < NUM_SPU_CHANNELS; i++) {
        if (_svm_sreg_dirty[i] & 1) {
            D_80032F10[i * 8 + 0] = _svm_sreg_buf[i * 8 + 0];
            D_80032F10[i * 8 + 1] = _svm_sreg_buf[i * 8 + 1];
        }
        if (_svm_sreg_dirty[i] & 4) {
            D_80032F10[i * 8 + 2] = _svm_sreg_buf[i * 8 + 2];
        }
        if (_svm_sreg_dirty[i] & 8) {
            D_80032F10[i * 8 + 3] = _svm_sreg_buf[i * 8 + 3];
        }
        if (_svm_sreg_dirty[i] & 0x10) {
            D_80032F10[i * 8 + 4] = _svm_sreg_buf[i * 8 + 4];
            D_80032F10[i * 8 + 5] = _svm_sreg_buf[i * 8 + 5];
        }
        _svm_sreg_dirty[i] = 0;
    }

    D_80032F10[0x18C / 2] = _svm_okof1;
    D_80032F10[0x18E / 2] = _svm_okof2;
    D_80032F10[0x188 / 2] = _svm_okon1;
    D_80032F10[0x18A / 2] = _svm_okon2;
    D_80032F10[0x198 / 2] = _svm_orev1;
    D_80032F10[0x19A / 2] = _svm_orev2;
    _svm_okof1 = 0;
    _svm_okof2 = 0;
    _svm_okon1 = 0;
    _svm_okon2 = 0;
}
