#if VERSION == 36
#define SpuVmSetVol _hdr_SsVmSetVol
#else
#define _SsVmSetVol _hdr_SsVmSetVol
#endif
#include "libsnd_i.h"
#undef _SsVmSetVol
#undef SpuVmSetVol
#if VERSION == 36
#define _SsVmSetVol SpuVmSetVol
#endif


int _SsVmSetVol(short seq_sep_no, short vabId, short prog, u16 vol, u16 pan) {
    struct SeqStruct* score;
    int count;
    short i;
    int v;
    int vol_scaled;
    int tone;
    unsigned voll_t, volr_t;
    unsigned voll, volr;
    u8 p;

    count = 0;
    score = &_ss_score[seq_sep_no & 0xFF][(seq_sep_no & 0xFF00) >> 8];
    _SsVmVSetUp(vabId, prog);
    _svm_cur.seq_sep_no = seq_sep_no;
    if (pan == 0) {
        pan = 1;
    }
    if (vol == 0) {
        vol = 1;
    }
    for (i = 0; i < _SsVmMaxVoice; i++) {
        if (_svm_voice[i].seq_sep_no == seq_sep_no && _svm_voice[i].prog == prog &&
            _svm_voice[i].vabId == vabId) {
            if (score->vol[score->channel_idx] != vol &&
                score->vol[score->channel_idx] == 0) {
                score->vol[score->channel_idx] = 1;
            }
            v = _svm_voice[i].voll1 * vol / 127;
            vol_scaled = _svm_vh->mvol * 0x3FFF;
            voll_t = v * vol_scaled / 0x3F01;
            tone = _svm_voice[i].fake_program * 16 + _svm_voice[i].tone;
            volr_t = voll_t * _svm_pg[prog].mvol * _svm_tn[tone].vol / 0x3F01;
            voll_t = volr_t * score->voll / 127;
            volr_t = volr_t * score->volr / 127;
            p = _svm_tn[tone].pan;
            if (p < 64) {
                voll = voll_t;
                volr = (volr_t * p) / 63;
            } else {
                volr = volr_t;
                voll = (voll_t * (127 - p)) / 63;
            }
            do {
                do {
                    p = _svm_pg[_svm_voice[i].prog].mpan;
                } while (0);
            } while (0);
            if (p < 64) {
                volr = (volr * p) / 63;
            } else {
                voll = (voll * (127 - p)) / 63;
            }
            do {
                do {
                    p = pan;
                } while (0);
            } while (0);
            if (p < 64) {
                volr = (volr * p) / 63;
            } else {
                voll = (voll * (127 - p)) / 63;
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
            count++;
            _svm_sreg_dirty[i] |= 3;
        }
    }
    return count;
}
