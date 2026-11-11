s16 SsUtPitchBend(s16 voice, s16 vabId, s16 prog, s16 note, s16 pbend) {
    SpuVmVSetUp(vabId, prog);
    _svm_cur.field_16_vag_idx = 0x21;
    return -((SpuVmPBVoice(voice, 0x21, vabId, prog, pbend) << 0x10) == 0);
}
