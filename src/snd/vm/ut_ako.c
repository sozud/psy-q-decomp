void SsUtAllKeyOff(void) {
    s16 voice;
    u16 bitsUpper;
    u16 bitsLower;
    u16 temp2;
    s16 pos;

    for (voice = 0; voice < spuVmMaxVoice; voice++) {
        _svm_voice[voice].unk2 = 0x18;
        _svm_voice[voice].unk0 = 0xFF;
        _svm_voice[voice].unk1b = 0;
        _svm_voice[voice].unk04 = 0;
        _svm_voice[voice].unk6 = 0;
        _svm_voice[voice].unke = 0xFF;
        _svm_voice[voice].unk10 = 0;
        _svm_voice[voice].prog = 0;
        _svm_voice[voice].tone = 0xFF;

        pos = voice * 8;
        D_80032F10[pos + 0x6 / 2] = 0x200;
        D_80032F10[pos + 0x4 / 2] = 0x1000;
        D_80032F10[pos + 0x8 / 2] = 0x80ff;
        D_80032F10[pos + 0x0] = 0;
        D_80032F10[pos + 0x2 / 2] = 0;
        D_80032F10[pos + 0xa / 2] = 0x4000;

        _svm_cur.field_0x1a = voice;
        temp2 = get_field_0x1a();
        if (temp2 < 0x10) {
            bitsLower = 1 << temp2;
            bitsUpper = 0;
        } else {
            bitsLower = 0;
            bitsUpper = 1 << (temp2 - 0x10);
        }

        _svm_voice[temp2].unk1b = 0;
        _svm_voice[temp2].unk04 = 0;
        _svm_voice[temp2].unk0 = 0;

        _svm_okof1 |= bitsLower;
        _svm_okof2 |= bitsUpper;

        _svm_okon1 &= ~_svm_okof1;
        _svm_okon2 &= ~_svm_okof2;
    }
}
