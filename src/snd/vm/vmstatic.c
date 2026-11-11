static s16 _svm_sreg_buf[NUM_SPU_CHANNELS * 8];
static char _svm_sreg_dirty[NUM_SPU_CHANNELS];
static struct SpuVoice _svm_voice[NUM_SPU_CHANNELS];
static s32 _svm_envx_ptr;
static s32 _svm_envx_hist[0xF];
static u32 _ss_spu_vm_rec[0x24];

static u16* D_80032F10 = (u16*)0x1F801C00;

static u8 SpuVmAlloc(s32 arg0) {
    u8 var_t2;
    u16 var_t0;
    u16 var_t3;
    u8 cur_voice;
    u8 channel;
    u8 var_t1;
    u16 prior;
#if VERSION == 33
    struct SpuVoice* voice;
#endif

    channel = 99;
    var_t3 = -1;
    var_t2 = 0;
    var_t0 = 0;
    prior = _svm_cur.field_F_prior;
    var_t1 = 99;
    for (cur_voice = 0; cur_voice < spuVmMaxVoice; cur_voice++) {
#if VERSION == 33
        voice = &_svm_voice[cur_voice];
        if (voice->unk1b == 0 &&
#else
        if (_svm_voice[cur_voice].unk1b == 0 &&
#endif
            _svm_voice[cur_voice].unk6 == 0) {
            channel = cur_voice;
            break;
        }
        if (_svm_voice[cur_voice].unk18 < prior) {
            prior = _svm_voice[cur_voice].unk18;
            var_t1 = cur_voice;
            var_t3 = _svm_voice[cur_voice].unk6;
            var_t0 = _svm_voice[cur_voice].unk2;
            var_t2 = 1;
        } else if (_svm_voice[cur_voice].unk18 == prior) {
            var_t2 += 1;
            if (_svm_voice[cur_voice].unk6 < var_t3) {
                var_t0 = _svm_voice[cur_voice].unk2;
                var_t3 = _svm_voice[cur_voice].unk6;
                var_t1 = cur_voice;
            } else if (_svm_voice[cur_voice].unk6 == var_t3) {
                if (var_t0 < _svm_voice[cur_voice].unk2) {
                    var_t0 = _svm_voice[cur_voice].unk2;
                    var_t1 = cur_voice;
                }
            }
        }
    }
    if (channel == 99) {
        if (var_t2 == 0) {
            channel = spuVmMaxVoice;
        } else {
            channel = var_t1;
        }
    }
    if (channel < spuVmMaxVoice) {
        for (cur_voice = 0; cur_voice < spuVmMaxVoice; cur_voice++) {
            _svm_voice[cur_voice].unk2++;
        }
        _svm_voice[channel].unk2 = 0;
        _svm_voice[channel].unk18 = _svm_cur.field_F_prior;
        if (_svm_voice[channel].unk1b == 2) {
            SpuSetNoiseVoice(0, 0xFFFFFF);
        }
    }
    return channel;
}

static void SpuVmKeyOnNow(u8 vagCount, u16 arg2) {
    struct SeqStruct* score =
        &_ss_score[_svm_cur.field_16_vag_idx & 0xFF]
                  [(_svm_cur.field_16_vag_idx & 0xFF00) >> 8];
    u16 pos;
    u16 bitsUpper;
    u16 bitsLower;
    u32 voll, volr;
    u32 voll_t, volr_t;
    s32 mvol_scaled;

    pos = _svm_cur.field_0x1a * 8;
    mvol_scaled = _svm_vh->mvol * 0x3FFF;
    voll_t = ((_svm_cur.field_4_voll * mvol_scaled) / 0x7F) / 0x7F;
    voll_t =
        ((voll_t * _svm_cur.field_A_mvol * _svm_cur.field_D_vol) / 0x7F) / 0x7F;
    volr_t = voll_t;

    if (_svm_cur.field_16_vag_idx != 0x21) {
        voll_t = (voll_t * score->unk74) / 0x7F;
        volr_t = (volr_t * score->unk76) / 0x7F;
    }

    if (_svm_cur.field_E_pan < 0x40) {
        voll = voll_t;
        volr = (volr_t * _svm_cur.field_E_pan) / 0x3F;
    } else {
        voll = (voll_t * (0x7F - _svm_cur.field_E_pan)) / 0x3F;
        volr = volr_t;
    }
    if (_svm_cur.field_B_mpan < 0x40) {
        volr = (volr * _svm_cur.field_B_mpan) / 0x3F;
    } else {
        voll = (voll * (0x7F - _svm_cur.field_B_mpan)) / 0x3F;
    }
    if (_svm_cur.field_0x5 < 0x40) {
        volr = (volr * _svm_cur.field_0x5) / 0x3F;
    } else {
        voll = (voll * (0x7F - _svm_cur.field_0x5)) / 0x3F;
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

    _svm_sreg_buf[pos + 2] = arg2;
    _svm_sreg_buf[pos + 0] = voll;
    _svm_sreg_buf[pos + 1] = volr;
    _svm_sreg_dirty[_svm_cur.field_0x1a] |= 7;

    _svm_voice[_svm_cur.field_0x1a].unk04 = arg2;
    _svm_voice[_svm_cur.field_0x1a].unk1b = 1;
    if (_svm_cur.field_0x1a < 0x10) {
        bitsLower = 1 << _svm_cur.field_0x1a;
        bitsUpper = 0;
    } else {
        bitsLower = 0;
        bitsUpper = 1 << (_svm_cur.field_0x1a - 0x10);
    }
    if (_svm_cur.field_14_seq_sep_no & 4) {
        _svm_orev1 |= bitsLower;
        _svm_orev2 |= bitsUpper;
    } else {
        _svm_orev1 &= ~bitsLower;
        _svm_orev2 &= ~bitsUpper;
    }
    _svm_okon1 |= bitsLower;
    _svm_okon2 |= bitsUpper;

    _svm_okof1 &= ~_svm_okon1;
    _svm_okof2 &= ~_svm_okon2;
}

static void SpuVmDoAllocate(void) {
    s32 i;

    _svm_cur.field_0x1c = _svm_cur.field_0x1a * 8;
    _svm_cur.field_0x1e =
        _svm_cur.field_C_vag_idx + (_svm_cur.field_7_fake_program * 0x10);
    _svm_voice[_svm_cur.field_0x1a].unk6 = 0x7FFF;
    for (i = 0; i < 0x10; i++) {
        _svm_envx_hist[i] &= ~(1 << _svm_cur.field_0x1a);
    }
    if ((_svm_cur.field_18_voice_idx & 1) > 0) {
        _svm_sreg_buf[_svm_cur.field_0x1c + 3] =
            _svm_pg[(_svm_cur.field_18_voice_idx - 1) / 2].reserved2;
        _svm_sreg_dirty[_svm_cur.field_0x1a] |= 8;
    } else {
        _svm_sreg_buf[_svm_cur.field_0x1c + 3] =
            _svm_pg[(_svm_cur.field_18_voice_idx - 1) / 2].reserved3;
        _svm_sreg_dirty[_svm_cur.field_0x1a] |= 8;
    }

    _svm_sreg_buf[_svm_cur.field_0x1c + 4] =
        _svm_tn[(_svm_cur.field_7_fake_program * 0x10) +
                _svm_cur.field_C_vag_idx]
            .adsr1;
    _svm_sreg_buf[_svm_cur.field_0x1c + 5] =
        _svm_tn[(_svm_cur.field_7_fake_program * 0x10) +
                _svm_cur.field_C_vag_idx]
            .adsr2 +
        _svm_damper;
    _svm_sreg_dirty[_svm_cur.field_0x1a] |= 0x30;
}

static void vmNoiseOn(u8 arg0) {
    struct SeqStruct* score =
        &_ss_score[_svm_cur.field_16_vag_idx & 0xFF]
                  [(_svm_cur.field_16_vag_idx & 0xFF00) >> 8];
    s16 voice;
    u16 bitsUpper;
    u16 bitsLower;
    u32 voll_t, volr_t;
    u32 voll, volr;
    u16 cnd;
    u32 temp;

    cnd = D_80032F10[0x1AA / 2];

    voll_t = score->unk74 * 0x81;
    volr_t = score->unk76 * 0x81;

    voll_t = (voll_t * _svm_cur.field_A_mvol) / 0x7F;
    volr_t = (volr_t * _svm_cur.field_A_mvol) / 0x7F;

    voll_t = (voll_t * _svm_cur.field_D_vol) / 0x7F;
    volr_t = (volr_t * _svm_cur.field_D_vol) / 0x7F;

    temp = _svm_cur.field_E_pan;
    if (temp < 0x40) {
        voll = voll_t;
        volr = (volr_t * temp) / 0x3F;
    } else {
        voll = (voll_t * (0x7F - temp)) / 0x3F;
        volr = volr_t;
    }
    temp = _svm_cur.field_B_mpan;
    if (temp < 0x40) {
        volr = (volr * temp) / 0x3F;
    } else {
        voll = (voll * (0x7F - temp)) / 0x3F;
    }
    temp = _svm_cur.field_0x5;
    if (temp < 0x40) {
        volr = (temp * volr) / 0x3F;
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

    cnd &= ~0x3F00;
    cnd |= ((_svm_cur.field_2_note - _svm_cur.field_10_centre) & 0x3F) << 8;
    D_80032F10[0x1AA / 2] = cnd;

    _svm_sreg_buf[arg0 * 8 + 0] = voll;
    _svm_sreg_buf[arg0 * 8 + 1] = volr;
    _svm_sreg_dirty[arg0] |= 3;
    if (arg0 < 0x10) {
        bitsLower = 1 << arg0;
        bitsUpper = 0;
    } else {
        bitsLower = 0;
        bitsUpper = 1 << (arg0 - 0x10);
    }
    _svm_voice[arg0].unk04 = 0xA;
    for (voice = 0; voice < spuVmMaxVoice; voice++) {
        _svm_voice[voice].unk1b &= 1;
    }
    _svm_voice[arg0].unk1b = 2;

    _svm_okon1 |= bitsLower;
    _svm_okon2 |= bitsUpper;

    _svm_okof1 &= ~_svm_okon1;
    _svm_okof2 &= ~_svm_okon2;

    if (_svm_cur.field_14_seq_sep_no & 4) {
        _svm_orev1 |= bitsLower;
        _svm_orev2 |= bitsUpper;
    } else {
        _svm_orev1 &= ~bitsLower;
        _svm_orev2 &= ~bitsUpper;
    }

    D_80032F10[0x194 / 2] = bitsLower;
    D_80032F10[0x196 / 2] = bitsUpper;
}

static void vmNoiseOn2(u8 voice, u16 arg1, u16 arg2, u16 arg3, u16 arg4) {
    u16 i;
    u16 bitsUpper;
    u16 bitsLower;

    _svm_sreg_buf[voice * 8 + 0] = arg1;
    _svm_sreg_buf[voice * 8 + 1] = arg2;
    _svm_sreg_dirty[voice] |= 3;

    if (voice < 0x10) {
        bitsLower = 1 << voice;
        bitsUpper = 0;
    } else {
        bitsLower = 0;
        bitsUpper = 1 << (voice - 0x10);
    }

    _svm_voice[voice].unk04 = 0xA;
    for (i = 0; i < spuVmMaxVoice; i++) {
        _svm_voice[i].unk1b &= 1;
    }
    _svm_voice[voice].unk1b = 2;
    _svm_voice[voice].unk2 = 0;

    _svm_okon1 |= bitsLower;
    _svm_okon2 |= bitsUpper;

    _svm_okof1 &= ~_svm_okon1;
    _svm_okof2 &= ~_svm_okon2;
    D_80032F10[0x194 / 2] = bitsLower;
    D_80032F10[0x196 / 2] = bitsUpper;
}
static u16 D_80032F14[] = {
    0x1000, 0x100E, 0x101D, 0x102C, 0x103B, 0x104A, 0x1059, 0x1068, 0x1078,
    0x1087, 0x1096, 0x10A5, 0x10B5, 0x10C4, 0x10D4, 0x10E3, 0x10F3, 0x1103,
    0x1113, 0x1122, 0x1132, 0x1142, 0x1152, 0x1162, 0x1172, 0x1182, 0x1193,
    0x11A3, 0x11B3, 0x11C4, 0x11D4, 0x11E5, 0x11F5, 0x1206, 0x1216, 0x1227,
    0x1238, 0x1249, 0x125A, 0x126B, 0x127C, 0x128D, 0x129E, 0x12AF, 0x12C1,
    0x12D2, 0x12E3, 0x12F5, 0x1306, 0x1318, 0x132A, 0x133C, 0x134D, 0x135F,
    0x1371, 0x1383, 0x1395, 0x13A7, 0x13BA, 0x13CC, 0x13DE, 0x13F1, 0x1403,
    0x1416, 0x1428, 0x143B, 0x144E, 0x1460, 0x1473, 0x1486, 0x1499, 0x14AC,
    0x14BF, 0x14D3, 0x14E6, 0x14F9, 0x150D, 0x1520, 0x1534, 0x1547, 0x155B,
    0x156F, 0x1583, 0x1597, 0x15AB, 0x15BF, 0x15D3, 0x15E7, 0x15FB, 0x1610,
    0x1624, 0x1638, 0x164D, 0x1662, 0x1676, 0x168B, 0x16A0, 0x16B5, 0x16CA,
    0x16DF, 0x16F4, 0x170A, 0x171F, 0x1734, 0x174A, 0x175F, 0x1775, 0x178B,
    0x17A1, 0x17B6, 0x17CC, 0x17E2, 0x17F9, 0x180F, 0x1825, 0x183B, 0x1852,
    0x1868, 0x187F, 0x1896, 0x18AC, 0x18C3, 0x18DA, 0x18F1, 0x1908, 0x191F,
    0x1937, 0x194E, 0x1965, 0x197D, 0x1995, 0x19AC, 0x19C4, 0x19DC, 0x19F4,
    0x1A0C, 0x1A24, 0x1A3C, 0x1A55, 0x1A6D, 0x1A85, 0x1A9E, 0x1AB7, 0x1ACF,
    0x1AE8, 0x1B01, 0x1B1A, 0x1B33, 0x1B4C, 0x1B66, 0x1B7F, 0x1B98, 0x1BB2,
    0x1BCC, 0x1BE5, 0x1BFF, 0x1C19, 0x1C33, 0x1C4D, 0x1C67, 0x1C82, 0x1C9C,
    0x1CB7, 0x1CD1, 0x1CEC, 0x1D07, 0x1D22, 0x1D3D, 0x1D58, 0x1D73, 0x1D8E,
    0x1DA9, 0x1DC5, 0x1DE0, 0x1DFC, 0x1E18, 0x1E34, 0x1E50, 0x1E6C, 0x1E88,
    0x1EA4, 0x1EC1, 0x1EDD, 0x1EFA, 0x1F16, 0x1F33, 0x1F50, 0x1F6D, 0x1F8A,
    0x1FA7, 0x1FC5, 0x1FE2, 0x2000};

static u16 note2pitch() {
    s16 new_var;
    s16 octave;
    s16 pos;

    u16 var_a2;
    u16 var_v1;

    new_var = (_svm_cur.field_2_note + 60) - _svm_cur.field_10_centre;
    octave = new_var / 12;
    pos = new_var % 12;

    var_a2 = _svm_cur.field_11_shift / 8;
    if (var_a2 > 15) {
        var_a2 = 15;
    }
    var_v1 = D_80032F14[pos * 16 + var_a2];
    octave -= 5;
    if (octave > 0) {
        var_v1 <<= octave;
    } else if (octave < 0) {
        var_v1 >>= -octave;
    }
    return var_v1;
}

static s32 note2pitch2(u16 arg0, u16 arg1) {
    s16 octave;
    s16 var_a2;
    s16 var_a3;
    short new_var;
    u16 var_v1;
    s32 pos;
    s32 tone;

    tone = _svm_cur.field_C_vag_idx + (_svm_cur.field_7_fake_program * 0x10);
    var_a3 = (arg1 + _svm_tn[tone].shift) / 8;
    var_a2 = 0;
    if (var_a3 >= 16) {
        var_a2 = 1;
        var_a3 -= 16;
    }
    new_var = arg0 + 60 - _svm_tn[tone].center + var_a2;
    octave = new_var / 12;
    pos = (new_var % 12) * 16;
    var_v1 = D_80032F14[pos + var_a3];

    octave -= 5;
    if (octave > 0) {
        var_v1 <<= octave;
    } else if (octave < 0) {
        var_v1 >>= -octave;
    }
    return var_v1;
}

static void SePitchBend(u8 voice, s16 arg1) {
    s32 tone;
    u16 note;
    u16 pbend;
    int pos;
    pos = voice * 8;

    if (voice >= 0 && voice < NUM_SPU_CHANNELS) {
        _svm_cur.field_7_fake_program = _svm_voice[voice].unk10;
        _svm_cur.field_C_vag_idx = _svm_voice[voice].tone;
        _svm_cur.field_0x1a = voice;
        tone =
            _svm_cur.field_C_vag_idx + (_svm_cur.field_7_fake_program * 0x10);
        if (arg1 >= 0) {
            note = _svm_voice[voice].note + (arg1 * _svm_tn[tone].pbmax) / 0x7F;
            pbend = (arg1 * _svm_tn[tone].pbmax) % 0x7F;
        } else {
            note =
                (_svm_voice[voice].note + (arg1 * _svm_tn[tone].pbmin) / 0x7F) -
                1;
            pbend = ((arg1 * _svm_tn[tone].pbmin) / 0x7F) + 0x7F;
        }
        _svm_sreg_buf[pos + 2] = note2pitch2(note, pbend);
        _svm_sreg_dirty[voice] |= 4;
    }
}

static inline u8 _SsVmSelectToneAndVag(u8* toneIdx, u8* vagIdx) {
    u8 i;
    u8 vagCount;
    VagAtr* vagAttr;

    vagCount = 0;
    for (i = 0; i < _svm_cur.field_0_sep_sep_no_tonecount; i++) {
        vagAttr = &_svm_tn[(_svm_cur.field_7_fake_program * 0x10) + i];
        if (vagAttr->min > _svm_cur.field_2_note ||
            _svm_cur.field_2_note > vagAttr->max) {
            continue;
        }
        vagIdx[vagCount] = vagAttr->vag;
        toneIdx[vagCount++] = i;
    }
    return vagCount;
}

void SsUtVibrateOn(void) {}

void SsUtVibrateOff(void) {}

#define klabs(A) (((A) >= 0) ? (A) : -(A))

static void SeAutoVol(s16 voice, s16 start_vol, s16 end_vol, s16 delta_time) {
    if (start_vol != end_vol) {
        _svm_voice[voice].auto_vol = 1;
        _svm_voice[voice].start_vol = start_vol;
        _svm_voice[voice].end_vol = end_vol;

        if (klabs(start_vol - end_vol) < delta_time) {
            _svm_voice[voice].unk1e = 1;
            _svm_voice[voice].unk20 = delta_time / (start_vol - end_vol);
            _svm_voice[voice].unk22 = delta_time / (start_vol - end_vol);
        } else {
            _svm_voice[voice].unk20 = 0;
            _svm_voice[voice].unk1e = (start_vol - end_vol) / delta_time;
        }
    }
}

static void SetAutoVol(s16 voice) {
    s16 temp_v1_3;
    u32 voll_t, volr_t;
    u16 voll, volr;
    s16 pos;
    s32 mvol_scaled;

    pos = voice * 8;
    if (_svm_voice[voice].unk20 != 0) {
        if (_svm_voice[voice].unk22-- > 0) {
            return;
        }
        _svm_voice[voice].unk22 = _svm_voice[voice].unk20;
    }
    _svm_voice[voice].start_vol += _svm_voice[voice].unk1e;
    if (_svm_voice[voice].unk1e > 0) {
        if (_svm_voice[voice].start_vol >= _svm_voice[voice].end_vol) {
            _svm_voice[voice].start_vol = _svm_voice[voice].end_vol;
            _svm_voice[voice].auto_vol = 0;
        }
    } else if (_svm_voice[voice].unk1e < 0) {
        if (_svm_voice[voice].start_vol <= _svm_voice[voice].end_vol) {
            _svm_voice[voice].start_vol = _svm_voice[voice].end_vol;
            _svm_voice[voice].auto_vol = 0;
        }
    }
    temp_v1_3 = _svm_voice[voice].start_vol;
    _svm_cur.field_4_voll = temp_v1_3;
    mvol_scaled = _svm_vh->mvol * 0x3FFF;

    voll_t = ((temp_v1_3 * mvol_scaled) / 0x7F) / 0x7F;
    volr_t = ((temp_v1_3 * mvol_scaled) / 0x7F) / 0x7F;

    voll_t =
        ((voll_t * _svm_cur.field_A_mvol * _svm_cur.field_D_vol) / 0x7F) / 0x7F;
    volr_t =
        ((volr_t * _svm_cur.field_A_mvol * _svm_cur.field_D_vol) / 0x7F) / 0x7F;

    if (_svm_cur.field_E_pan < 0x40) {
        voll = voll_t;
        volr = (volr_t * _svm_cur.field_E_pan) / 0x40;
    } else {
        voll = (voll_t * (0x7F - _svm_cur.field_E_pan)) / 0x40;
        volr = volr_t;
    }

    if (_svm_cur.field_B_mpan < 0x40) {
        volr = (volr * _svm_cur.field_B_mpan) / 0x40;
    } else {
        voll = (voll * (0x7F - _svm_cur.field_B_mpan)) / 0x40;
    }

    if (_svm_cur.field_0x5 < 0x40) {
        volr = (volr * _svm_cur.field_0x5) / 0x40;
    } else {
        voll = (voll * (0x7F - _svm_cur.field_0x5)) / 0x40;
    }

    if (_svm_stereo_mono == 1) {
        if (volr > voll) {
            voll = volr;
        } else {
            volr = voll;
        }
    }
    _svm_sreg_buf[pos + 0] = voll;
    _svm_sreg_buf[pos + 1] = volr;
    _svm_sreg_dirty[voice] |= 3;
}

static void SeAutoPan(s16 voice, s16 start_pan, s16 end_pan, s16 delta_time) {
    if (start_pan != end_pan) {
        _svm_voice[voice].auto_pan = 1;
        _svm_voice[voice].start_pan = start_pan;
        _svm_voice[voice].end_pan = end_pan;

        if (klabs(start_pan - end_pan) < delta_time) {
            _svm_voice[voice].unk2a = 1;
            _svm_voice[voice].unk2c = delta_time / (start_pan - end_pan);
            _svm_voice[voice].unk2e = delta_time / (start_pan - end_pan);
        } else {
            _svm_voice[voice].unk2c = 0;
            _svm_voice[voice].unk2a = (start_pan - end_pan) / delta_time;
        }
    }
}

static void SetAutoPan(s16 voice) {
    u32 voll_t, volr_t;
    u16 voll, volr;
    s16 pos;
    char new_var;
    u16 new_var2;
    s32 mvol_scaled;

    pos = voice * 8;
    if (_svm_voice[voice].unk2c != 0) {
        if (_svm_voice[voice].unk2e-- > 0) {
            return;
        }
        _svm_voice[voice].unk2e = _svm_voice[voice].unk2c;
    }
    _svm_voice[voice].start_pan += _svm_voice[voice].unk2a;
    if (_svm_voice[voice].unk2a > 0) {
        if (_svm_voice[voice].start_pan >= _svm_voice[voice].end_pan) {
            _svm_voice[voice].start_pan = _svm_voice[voice].end_pan;
            _svm_voice[voice].auto_pan = 0;
        }
    } else if (_svm_voice[voice].unk2a < 0) {
        if (_svm_voice[voice].start_pan <= _svm_voice[voice].end_pan) {
            _svm_voice[voice].start_pan = _svm_voice[voice].end_pan;
            _svm_voice[voice].auto_pan = 0;
        }
    }
    new_var = _svm_voice[voice].start_pan;
    _svm_cur.field_0x5 = new_var;
    mvol_scaled = _svm_vh->mvol * 0x3FFF;

    voll_t = ((_svm_cur.field_4_voll * mvol_scaled) / 0x7F) / 0x7F;
    volr_t = ((_svm_cur.field_4_voll * mvol_scaled) / 0x7F) / 0x7F;

    voll_t =
        ((voll_t * _svm_cur.field_A_mvol * _svm_cur.field_D_vol) / 0x7F) / 0x7F;
    volr_t =
        ((volr_t * _svm_cur.field_A_mvol * _svm_cur.field_D_vol) / 0x7F) / 0x7F;

    if (_svm_cur.field_E_pan < 0x40) {
        voll = voll_t;
        volr = (volr_t * _svm_cur.field_E_pan) / 0x40;
    } else {
        voll = (voll_t * (0x7F - _svm_cur.field_E_pan)) / 0x40;
        volr = volr_t;
    }

    if (_svm_cur.field_B_mpan < 0x40) {
        volr = (volr * _svm_cur.field_B_mpan) / 0x40;
    } else {
        voll = (voll * (0x7F - _svm_cur.field_B_mpan)) / 0x40;
    }
    new_var2 = new_var;
    if (new_var2 < 0x40) {
        volr = (volr * new_var2) / 0x40;
    } else {
        voll = (voll * (0x7F - new_var2)) / 0x40;
    }

    if (_svm_stereo_mono == 1) {
        if (volr > voll) {
            voll = volr;
        } else {
            volr = voll;
        }
    }
    _svm_sreg_buf[pos + 0] = voll;
    _svm_sreg_buf[pos + 1] = volr;
    _svm_sreg_dirty[voice] |= 3;
}
