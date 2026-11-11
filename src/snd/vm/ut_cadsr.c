s16 SsUtChangeADSR(
    s16 voice, s16 vabId, s16 prog, s16 old_note, u16 adsr1, u16 adsr2) {
    if (voice >= 0 && voice < NUM_SPU_CHANNELS) {
        if ((_svm_voice[voice].vabId == vabId) &&
            (_svm_voice[voice].prog == prog) &&
            (_svm_voice[voice].note == old_note)) {
            _svm_sreg_buf[voice * 8 + 4] = adsr1;
            _svm_sreg_buf[voice * 8 + 5] = adsr2;
            _svm_sreg_dirty[voice] |= 0x30;
            return 0;
        }
    }
    return -1;
}
