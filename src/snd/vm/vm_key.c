s32 SpuVmKeyOn(s16 arg0, s16 vabId, s16 prog, u16 note, u16 voll, u16 volr) {
#if VERSION >= 35
    struct SeqStruct* score = &_ss_score[arg0 & 0xFF][(arg0 & 0xFF00) >> 8];
#endif
    u8 vagIdx[0x80];
    u8 toneIdx[0x80];
    u8 i;
    s32 var_s2;
    s32 tone;
    u8 vagCount;

#if VERSION >= 35
    var_s2 = 0;
#endif
    if (SpuVmVSetUp(vabId, prog)) {
        return -1;
    }
    _svm_cur.field_16_vag_idx = arg0;
    _svm_cur.field_2_note = note;
    _svm_cur.field_0x3 = 0;
#if VERSION >= 35
    if (arg0 == 0x21) {
        _svm_cur.field_4_voll = voll;
    } else {
        _svm_cur.field_4_voll = (voll * score->vol[score->channel]) / 0x7F;
    }
#else
    _svm_cur.field_4_voll = voll;
#endif

    _svm_cur.field_0x5 = volr;
    _svm_cur.field_A_mvol = _svm_pg[prog].mvol;
    _svm_cur.field_B_mpan = _svm_pg[prog].mpan;
    _svm_cur.field_0_sep_sep_no_tonecount = _svm_pg[prog].tones;
#if VERSION < 35
    var_s2 = 0;
#endif
    if (_svm_cur.field_7_fake_program >= _svm_vh->ps) {
        return -1;
    }
    if (voll == 0) {
#if VERSION >= 35
        var_s2 = SpuVmKeyOff(arg0, vabId, prog, note);
#else
        SpuVmKeyOff(arg0, vabId, prog, note);
#endif
    } else {
        vagCount = _SsVmSelectToneAndVag(toneIdx, vagIdx);
        for (i = 0; i < vagCount; i++) {
            _svm_cur.field_18_voice_idx = vagIdx[i];
            _svm_cur.field_C_vag_idx = toneIdx[i];

            tone = _svm_cur.field_C_vag_idx +
                   (_svm_cur.field_7_fake_program * 0x10);
            _svm_cur.field_F_prior = _svm_tn[tone].prior;
            _svm_cur.field_D_vol = _svm_tn[tone].vol;
            _svm_cur.field_E_pan = _svm_tn[tone].pan;
            _svm_cur.field_10_centre = _svm_tn[tone].center;
            _svm_cur.field_11_shift = _svm_tn[tone].shift;
            _svm_cur.field_14_seq_sep_no = _svm_tn[tone].mode;
            _svm_cur.field_12_mode = _svm_tn[tone].min;
            _svm_cur.field_0x13 = _svm_tn[tone].max;
            _svm_cur.field_0x1a = SpuVmAlloc(0);
            if (_svm_cur.field_0x1a < spuVmMaxVoice) {
                _svm_voice[_svm_cur.field_0x1a].unk1b = 1;
                _svm_voice[_svm_cur.field_0x1a].unk2 = 0;
                _svm_voice[_svm_cur.field_0x1a].unke = arg0;
                _svm_voice[_svm_cur.field_0x1a].vabId = _svm_cur.field_1_vabId;
                _svm_voice[_svm_cur.field_0x1a].unk10 =
                    _svm_cur.field_7_fake_program;
                _svm_voice[_svm_cur.field_0x1a].prog = prog;
#if VERSION >= 35
                if (arg0 != 0x21) {
                    _svm_voice[_svm_cur.field_0x1a].unk8 = voll;
                }
                _svm_voice[_svm_cur.field_0x1a].unka = volr;
#endif
                _svm_voice[_svm_cur.field_0x1a].tone = _svm_cur.field_C_vag_idx;
                _svm_voice[_svm_cur.field_0x1a].note = note;
                _svm_voice[_svm_cur.field_0x1a].unk18 = _svm_cur.field_F_prior;
                _svm_voice[_svm_cur.field_0x1a].unk0 =
                    _svm_cur.field_18_voice_idx;
                SpuVmDoAllocate();
                if (_svm_cur.field_18_voice_idx == 0xFF) {
                    vmNoiseOn(_svm_cur.field_0x1a);
                } else {
                    SpuVmKeyOnNow(vagCount, note2pitch() & 0xFFFF);
                }
#if VERSION >= 35
                var_s2 |= 1 << _svm_cur.field_0x1a;
            } else {
                var_s2 = -1;
#else
                var_s2 = (var_s2 << 4) | _svm_cur.field_0x1a;
#endif
            }
        }
    }
    return var_s2;
}

s32 SpuVmKeyOff(s16 arg0, s16 vabId, s16 prog, u16 note) {
    u16 bitsUpper;
    u16 bitsLower;
    u8 voice;
#if VERSION == 33
    u8 var_t1;
#else
    s32 var_t1;
#endif
    u16 temp_v1;

    var_t1 = 0;
    for (voice = 0; voice < spuVmMaxVoice; voice++) {
        if ((_svm_voice[voice].note == note) &&
            (_svm_voice[voice].prog == prog) &&
            (_svm_voice[voice].unke == arg0) &&
            (_svm_voice[voice].vabId == vabId)) {
            if (_svm_voice[voice].unk0 == 0xFF) {
                _svm_voice[voice].unk1b = 0;
                _svm_voice[voice].unk04 = 0;
                D_80032F10[202] = 0;
                D_80032F10[203] = 0;
            } else {
                _svm_cur.field_0x1a = voice;
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
            var_t1++;
        }
    }
    return var_t1;
}

void SpuVmSeKeyOn(s16 vabId, s16 prog, u16 note, s32 arg3, u16 voll, u16 volr) {
    u16 var_v1;
    u16 var_a1;

    if (voll == volr) {
        var_v1 = 0x40;
        var_a1 = voll;
    } else if (volr < voll) {
        var_a1 = voll;
        var_v1 = (volr * 0x40) / voll;
    } else {
        var_a1 = volr;
        var_v1 = 0x7F - ((voll * 0x40) / volr);
    }
    SpuVmKeyOn(0x21, vabId, prog, note, var_a1, var_v1);
}

void SpuVmSeKeyOff(s16 vabId, s16 prog, u16 note) {
    SpuVmKeyOff(0x21, vabId, prog, note);
}

void KeyOnCheck(void) {}
