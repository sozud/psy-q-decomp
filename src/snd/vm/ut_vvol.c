s16 SsUtGetDetVVol(s16 voice, s16* voll, s16* volr) {
    if (voice >= 0 && voice < NUM_SPU_CHANNELS) {
        *voll = D_80032F10[voice * 8 + 0];
        *volr = D_80032F10[voice * 8 + 1];
        return 0;
    }
    return -1;
}

s32 SsUtSetDetVVol(s16 voice, s16 arg1, s16 arg2) {
    if (voice >= 0 && voice < NUM_SPU_CHANNELS) {
        _svm_sreg_buf[voice * 8 + 0] = arg1;
        _svm_sreg_buf[voice * 8 + 1] = arg2;
        _svm_sreg_dirty[voice] |= 3;
        return 0;
    }

    return -1;
}

s16 SsUtGetVVol(s16 voice, s16* voll, s16* volr) {
    if (voice >= 0 && voice < NUM_SPU_CHANNELS) {
        s16 temp1 = D_80032F10[voice * 8 + 0];
        s16 temp2 = D_80032F10[voice * 8 + 1];
        *voll = temp1 / 0x81;
        *volr = temp2 / 0x81;
        return 0;
    }
    return -1;
}

s16 SsUtSetVVol(s16 voice, s16 voll, s16 volr) {
    if (voice >= 0 && voice < NUM_SPU_CHANNELS) {
        voll *= 0x81;
        volr *= 0x81;
        _svm_sreg_buf[voice * 8 + 1] = volr;
        _svm_sreg_buf[voice * 8 + 0] = voll;
        _svm_sreg_dirty[voice] |= 3;
        return 0;
    }

    return -1;
}
