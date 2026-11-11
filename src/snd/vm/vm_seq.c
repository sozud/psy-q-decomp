s32 SpuVmSetSeqVol(s16 seq_sep_no, u16 voll, u16 volr, s16 arg3) {
    struct SeqStruct* score =
        &_ss_score[seq_sep_no & 0xFF][(seq_sep_no >> 8) & 0xFF];
    s16 voice;
    int voll_mul;
    int volr_mul;
    u8 pad[4];
    int temp;
    s16 pos;

    _svm_cur.field_16_vag_idx = seq_sep_no;
    score->unk74 = voll;
    score->unk76 = volr;
    temp = seq_sep_no;
    if (score->unk74 >= 0x80) {
        score->unk74 = 0x7F;
    }
    if (score->unk76 >= 0x80) {
        score->unk76 = 0x7F;
    }
    voll_mul = voll * 0x81;
    volr_mul = volr * 0x81;

    if (arg3 == 1) {
        for (voice = 0; voice < spuVmMaxVoice; voice++) {
#if VERSION == 33
            if (_svm_voice[voice].unke == temp) {
#else
            if ((u16)_svm_voice[voice].unke == (u16)temp) {
#endif
                pos = voice * 8;
                _svm_sreg_buf[pos + 0] = voll_mul;
                _svm_sreg_buf[pos + 1] = volr_mul;
                _svm_sreg_dirty[voice] |= 3;
            }
        }
    }
    return _svm_cur.field_16_vag_idx;
}

s32 SpuVmGetSeqVol(s16 arg0, s16* arg1, s16* arg2) {
    struct SeqStruct* score = &_ss_score[arg0 & 0xFF][(arg0 & 0xFF00) >> 8];
    _svm_cur.field_16_vag_idx = arg0;
    *arg1 = score->unk74;
    *arg2 = score->unk76;
    return _svm_cur.field_16_vag_idx;
}

s16 SpuVmGetSeqLVol(s16 arg0) {
    struct SeqStruct* score = &_ss_score[arg0 & 0xFF][(arg0 & 0xFF00) >> 8];
#if VERSION == 33
    _svm_cur.field_16_vag_idx = arg0 & 0xFF;
#else
    _svm_cur.field_16_vag_idx = arg0;
#endif
    return score->unk74;
}

s16 SpuVmGetSeqRVol(s16 arg0) {
    struct SeqStruct* score = &_ss_score[arg0 & 0xFF][(arg0 & 0xFF00) >> 8];
    _svm_cur.field_16_vag_idx = arg0;
    return score->unk76;
}

void SpuVmSeqKeyOff(s16 arg0) {
    u8 voice;
    u16 bitsUpper;
    u16 bitsLower;
    u16 temp2;

    for (voice = 0; voice < spuVmMaxVoice; voice++) {
        if (_svm_voice[voice].unke == arg0) {
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
}
