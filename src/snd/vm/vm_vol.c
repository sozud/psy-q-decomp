#if VERSION == 33
s32 SpuVmSetVol(s16 seq_sep_no, s16 vabId, s16 prog, u16 arg3, u16 arg4) {
    struct SeqStruct* score =
        &_ss_score[seq_sep_no & 0xFF][(seq_sep_no & 0xFF00) >> 8];
    s32 var_s2;
    u8 voice;
    u32 voll, volr;
    u32 voll_t, volr_t;
    u8 temp;
    s32 mvol_scaled;
    s32 new_var;

    var_s2 = 0;
    SpuVmVSetUp(vabId, prog);
    _svm_cur.field_16_vag_idx = seq_sep_no;
    for (voice = 0; voice < spuVmMaxVoice; voice++) {
        if ((_svm_voice[voice].unke == seq_sep_no) &&
            (_svm_voice[voice].vabId == vabId) &&
            (_svm_voice[voice].prog == prog)) {
            new_var = arg3;
            mvol_scaled = _svm_vh->mvol * 0x3FFF;
            voll_t = (new_var * mvol_scaled) / 0x3F01;
            voll_t = (voll_t * _svm_cur.field_A_mvol * _svm_cur.field_D_vol) /
                     0x3F01;
            volr_t = voll_t;
            voll_t = (voll_t * score->unk74) / 0x7F;
            volr_t = (volr_t * score->unk76) / 0x7F;
            _svm_cur.field_C_vag_idx = _svm_voice[voice].tone;
            _svm_cur.field_0x1a = voice;

            temp = _svm_cur.field_E_pan;
            if (temp < 0x40) {
                voll = voll_t;
                volr = (volr_t * temp) / 0x40;
            } else {
                voll = (voll_t * (0x7F - temp)) / 0x40;
                volr = volr_t;
            }
            temp = _svm_cur.field_B_mpan;
            if (temp < 0x40) {
                volr = (volr * temp) / 0x40;
            } else {
                voll = (voll * (0x7F - temp)) / 0x40;
            }
            temp = arg4;
            if (temp < 0x40) {
                volr = (volr * temp) / 0x40;
            } else {
                voll = (voll * (0x7F - temp)) / 0x40;
            }
            if (_svm_stereo_mono == 1) {
                if (voll < volr) {
                    voll = volr;
                } else {
                    volr = voll;
                }
            }
            voll = (voll * voll) / 0x3FFF;
            volr = (volr * volr) / 0x3FFF;
            _svm_sreg_buf[voice * 8 + 0] = voll;
            _svm_sreg_buf[voice * 8 + 1] = volr;
            _svm_sreg_dirty[voice] |= 3;
            var_s2++;
        }
    }
    return var_s2;
}
#else
s32 SpuVmSetVol(s16 seq_sep_no, s16 vabId, s16 prog, u16 arg3, u16 arg4) {
    struct SeqStruct* score =
        &_ss_score[seq_sep_no & 0xFF][(seq_sep_no & 0xFF00) >> 8];
    s32 var_s2;
    u8 voice;
    s32 new_var;
    u32 voll, volr;
    u32 voll_t, volr_t;
    s32 mvol_scaled;
    u8 temp;
    s16 new_var2;

    var_s2 = 0;
    SpuVmVSetUp(vabId, prog);
    _svm_cur.field_16_vag_idx = seq_sep_no;
    for (voice = 0; voice < spuVmMaxVoice; voice++) {
        new_var2 = prog;
        if ((_svm_voice[voice].unke == seq_sep_no) &&
            (_svm_voice[voice].prog == new_var2) &&
            (_svm_voice[voice].vabId == vabId)) {
            if (score->vol[score->channel] != arg3 &&
                score->vol[score->channel] == 0) {
                score->vol[score->channel] = 1;
            }
            new_var = (_svm_voice[voice].unk8 * arg3) / 0x7F;
            mvol_scaled = _svm_vh->mvol * 0x3FFF;
            voll_t = ((new_var * mvol_scaled) / 0x7F) / 0x7F;
            voll_t =
                ((voll_t * _svm_pg[new_var2].mvol *
                  _svm_tn[_svm_voice[voice].tone + (new_var2 * 0x10)].vol) /
                 0x7F) /
                0x7F;
            volr_t = voll_t;

            voll_t = (voll_t * score->unk74) / 0x7F;
            volr_t = (volr_t * score->unk76) / 0x7F;

            temp = _svm_tn[_svm_voice[voice].tone].pan;
            if (temp < 0x40) {
                voll = voll_t;
                volr = (volr_t * temp) / 0x3F;
            } else {
                voll = (voll_t * (0x7F - temp)) / 0x3F;
                volr = volr_t;
            }
            temp = _svm_pg[_svm_voice[voice].unk10].mpan;
            if (temp < 0x40) {
                volr = (volr * temp) / 0x3F;
            } else {
                voll = (voll * (0x7F - temp)) / 0x3F;
            }
            do {
                do {
                    temp = arg4;
                } while (0);
            } while (0);
            if (temp < 0x40) {
                volr = (volr * temp) / 0x3F;
            } else {
                voll = (voll * (0x7F - temp)) / 0x3F;
            }
            if (_svm_stereo_mono == 1) {
                if (voll < volr) {
                    voll = volr;
                } else {
                    volr = voll;
                }
            }
            voll = (voll * voll) / 0x3FFF;
            volr = (volr * volr) / 0x3FFF;
            _svm_sreg_buf[voice * 8 + 0] = voll;
            _svm_sreg_buf[voice * 8 + 1] = volr;
            _svm_sreg_dirty[voice] |= 3;
            var_s2++;
        }
    }
    return var_s2;
}
#endif
