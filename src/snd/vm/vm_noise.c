void SpuVmNoiseOnWithAdsr(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s16 temp_v0;

    _svm_cur.field_F_prior = 0x7F;
    temp_v0 = SpuVmAlloc(0xFF) & 0xFF;
    _svm_cur.field_0x1a = temp_v0;
    if (temp_v0 < (s32)spuVmMaxVoice) {
        vmNoiseOn2((u8)_svm_cur.field_0x1a, arg0, arg1, arg2, arg3);
    }
}

void SpuVmNoiseOff(void) {
    s16 i;
    for (i = 0; i < spuVmMaxVoice; i++) {
        if (_svm_voice[i].unk1b == 2) {
            _svm_voice[i & 0xFF].unk1b = 0;
            _svm_voice[i & 0xFF].unk04 = 0;

            D_80032F10[0x194 / 2] = 0;
            D_80032F10[0x196 / 2] = 0;
        }
    }
}

void SpuVmNoiseOn(s32 arg0, s32 arg1) {
    _svm_cur.field_F_prior = 0x7F;
    _svm_cur.field_0x1a = SpuVmAlloc(0xFF);
    if (_svm_cur.field_0x1a < spuVmMaxVoice) {
        vmNoiseOn2(_svm_cur.field_0x1a, arg0, arg1, 0x80FF, 0x5FC8);
    }
}
