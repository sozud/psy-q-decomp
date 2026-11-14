#include "libsnd_i.h"

#if VERSION >= 41
short _SsVmSetSeqVol(short seq_sep_num, u16 voll, u16 volr, short arg3) {
    struct SeqStruct* score;
    short voice;
    int vol;
    int vm;
    unsigned int vl;
    unsigned int vr;
    u16 l;
    u16 r;
    u8 pv;

    score = &_ss_score[seq_sep_num & 0xFF][(seq_sep_num & 0xFF00) >> 8];
    score->voll = voll;
    score->volr = volr;
    if (score->voll >= 127) {
        score->voll = 127;
    }
    if (score->volr >= 127) {
        score->volr = 127;
    }
    for (voice = 0; voice < _SsVmMaxVoice; voice++) {
        if ((_snd_vmask & (1 << voice)) == 0 &&
            _svm_voice[voice].seq_sep_no == seq_sep_num &&
            _svm_voice[voice].vabId == score->vab_id) {
            _SsVmVSetUp(_svm_voice[voice].vabId, _svm_voice[voice].fake_program);
            vol = (_svm_voice[voice].voll1 *
                   score->vol[_svm_voice[voice].unkc]) / 127;
            vm = vol * 0x3FFF;
            vl = (_svm_vh->mvol * vm) / 16129;
            vl = (vl * _svm_pg[_svm_voice[voice].prog].mvol *
                  _svm_tn[_svm_voice[voice].fake_program * 16 +
                          _svm_voice[voice].tone].vol) / 16129;
            vr = vl;
            vl = (vl * score->voll) / 127;
            vr = (vr * score->volr) / 127;
            pv = _svm_tn[_svm_voice[voice].fake_program * 16 +
                         _svm_voice[voice].tone].pan;
            if (pv < 64) {
                l = vl;
                r = (vr * pv) / 63;
            } else {
                l = (vl * (127 - pv)) / 63;
                r = vr;
            }
            pv = _svm_pg[_svm_voice[voice].prog].mpan;
            if (pv < 64) {
                r = (r * pv) / 63;
            } else {
                l = (l * (127 - pv)) / 63;
            }
            pv = _svm_voice[voice].pan;
            if (pv < 64) {
                r = (r * pv) / 63;
            } else {
                l = (l * (127 - pv)) / 63;
            }
            if (_svm_stereo_mono == 1) {
                if (l < r) {
                    l = r;
                } else {
                    r = l;
                }
            }
            l = (l * l) / 16383;
            r = (r * r) / 16383;
            ((u16*)_svm_sreg_buf)[voice * 8 + 0] = l;
            ((u16*)_svm_sreg_buf)[voice * 8 + 1] = r;
            _svm_sreg_dirty[voice] |= 3;
        }
    }
    return seq_sep_num;
}
#else
short _SsVmSetSeqVol(short seq_sep_no, u16 voll, u16 volr, short arg3) {
    struct SeqStruct* score;
    int vol;
    int vol_scaled;
    int tone;
    unsigned voll_t, volr_t;
    u8 pan;
    short i;

    score = &_ss_score[seq_sep_no & 0xFF][(seq_sep_no & 0xFF00) >> 8];
    _svm_cur.seq_sep_no = seq_sep_no;
    if (voll == 0) {
        voll = 1;
    }
    if (volr == 0) {
        volr = 1;
    }
    score->voll = voll;
    score->volr = volr;
    if (score->voll > 127) {
        score->voll = 127;
    }
    if (score->volr > 127) {
        score->volr = 127;
    }
    if (arg3 == 1) {
        for (i = 0; i < _SsVmMaxVoice; i++) {
            if ((u16)_svm_voice[i].seq_sep_no == (u16)seq_sep_no) {
                vol =
                    _svm_voice[i].voll1 * score->vol[score->channel_idx] / 127;
                vol_scaled = vol * 0x3FFF;
                voll_t = _svm_vh->mvol * vol_scaled / 0x3F01;
                tone = _svm_voice[i].fake_program * 16 + _svm_voice[i].tone;
                volr_t = voll_t * _svm_pg[_svm_voice[i].prog].mvol *
                         _svm_tn[tone].vol / 0x3F01;
                voll_t = volr_t * score->voll / 127;
                volr_t = volr_t * score->volr / 127;
                pan = _svm_tn[tone].pan;
                if (pan < 64) {
                    voll = voll_t;
                    volr = (volr_t * pan) / 63;
                } else {
                    volr = volr_t;
                    voll = (voll_t * (127 - pan)) / 63;
                }
                pan = _svm_pg[_svm_voice[i].prog].mpan;
                if (pan < 64) {
                    volr = (volr * pan) / 63;
                } else {
                    voll = (voll * (127 - pan)) / 63;
                }
                pan = _svm_voice[i].pan;
                if (pan < 64) {
                    volr = (volr * pan) / 63;
                } else {
                    voll = (voll * (127 - pan)) / 63;
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
                ((short*)_svm_sreg_buf)[i * 8 + 0] = voll;
                ((short*)_svm_sreg_buf)[i * 8 + 1] = volr;
                _svm_sreg_dirty[i] |= 3;
            }
        }
    }
    return _svm_cur.seq_sep_no;
}

#endif

short _SsVmGetSeqVol(short seq_sep_no, short* voll, short* volr) {
    struct SeqStruct* score =
        &_ss_score[seq_sep_no & 0xFF][(seq_sep_no & 0xFF00) >> 8];
    _svm_cur.seq_sep_no = seq_sep_no;
    *voll = score->voll;
    *volr = score->volr;
    return _svm_cur.seq_sep_no;
}

s16 _SsVmGetSeqLVol(s16 arg0) {
    struct SeqStruct* score = &_ss_score[arg0 & 0xFF][(arg0 & 0xFF00) >> 8];
    _svm_cur.seq_sep_no = arg0;
    return score->voll;
}

s16 _SsVmGetSeqRVol(s16 arg0) {
    struct SeqStruct* score = &_ss_score[arg0 & 0xFF][(arg0 & 0xFF00) >> 8];
    _svm_cur.seq_sep_no = arg0;
    return score->volr;
}

void _SsVmSeqKeyOff(s16 seq_sep_num) {
    u8 i;

    for (i = 0; i < _SsVmMaxVoice; i++) {
#if VERSION >= 41
        if ((_snd_vmask & (1 << i)) == 0 && _svm_voice[i].seq_sep_no == seq_sep_num) {
#else
        if (_svm_voice[i].seq_sep_no == seq_sep_num) {
#endif
            _svm_cur.voice = i;
            _SsVmKeyOffNow(0);
        }
    }
}
