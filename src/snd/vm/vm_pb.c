s16 SpuVmPBVoice(
    s16 voice, s16 seq_sep_no, s16 vabId, s16 program, u16 amount) {
    s16 temp_t1;
    u16 temp_v1;
    u16 pbend;
    u16 temp_a1;
    u16 note;

    temp_t1 = amount - 0x40;
    if ((_svm_voice[voice].unke == seq_sep_no) &&
        (_svm_voice[voice].vabId == vabId) &&
        (_svm_voice[voice].prog == program)) {
        temp_a1 = _svm_voice[voice].note;
        temp_v1 =
            _svm_voice[voice].tone + (_svm_cur.field_7_fake_program * 0x10);
        if (temp_t1 > 0) {
            do {
                note = _svm_voice[voice].note +
                       ((temp_t1 * _svm_tn[temp_v1].pbmax) / 0x3F);
            } while (0);
            pbend = ((temp_t1 * _svm_tn[temp_v1].pbmax) % 0x3F) * 2;
        } else {
            note = temp_a1;
            if (temp_t1 < 0) {
                note = ((temp_t1 * _svm_tn[temp_v1].pbmin) / 0x40);
                note = (temp_a1 + note) - 1;
                pbend =
                    (((temp_t1 * _svm_tn[temp_v1].pbmin) % 0x40) * 2) + 0x7F;
            } else {
                pbend = 0;
            }
        }
        _svm_cur.field_C_vag_idx = _svm_voice[voice].tone;
        _svm_cur.field_0x1a = voice;
        _svm_sreg_buf[voice * 8 + 2] = note2pitch2(note, pbend);
        _svm_sreg_dirty[voice] |= 4;
        return 1;
    } else {
        return 0;
    }
}

s32 SpuVmPitchBend(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    s16 voice;
    s32 var_s1;

    SpuVmVSetUp(arg1, arg2);
    _svm_cur.field_16_vag_idx = arg0;
    var_s1 = 0;

    for (voice = 0; voice < spuVmMaxVoice; voice++) {
        var_s1 += SpuVmPBVoice(voice, arg0, arg1, arg2, arg3);
    }
    return var_s1;
}
