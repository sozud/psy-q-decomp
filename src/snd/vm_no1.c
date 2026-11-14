#include "libsnd_i.h"

#if VERSION >= 41
void vmNoiseOn(unsigned char voice) {
    struct SeqStruct* score;
    s16 i;
    u16 bitsUpper;
    u16 bitsLower;
    u32 voll, volr;
    u32 voll_t, volr_t;
    u32 temp;
    s32 mvol_scaled;
    s32 pos;

    score = &_ss_score[_svm_cur.seq_sep_no & 0xFF]
                      [(_svm_cur.seq_sep_no & 0xFF00) >> 8];
    mvol_scaled = _svm_vh->mvol * 0x3FFF;
    voll_t = (_svm_cur.volume * mvol_scaled) / 16129;
    voll_t = (voll_t * _svm_cur.mvol * _svm_cur.tone_vol) / 16129;
    volr_t = voll_t;
    if (_svm_cur.seq_sep_no != 0x21) {
        voll_t = (voll_t * score->voll) / 127;
        volr_t = (volr_t * score->volr) / 127;
    }
    temp = _svm_cur.tone_pan;
    if (temp < 64) {
        voll = voll_t;
        volr = (volr_t * temp) / 63;
    } else {
        voll = (voll_t * (127 - temp)) / 63;
        volr = volr_t;
    }
    temp = _svm_cur.mpan;
    if (temp < 64) {
        volr = (volr * temp) / 63;
    } else {
        voll = (voll * (127 - temp)) / 63;
    }
    temp = _svm_cur.pan;
    if (temp < 64) {
        volr = (temp * volr) / 63;
    } else {
        voll = (voll * (127 - temp)) / 63;
    }
    if (_svm_stereo_mono == 1) {
        if (voll < volr) {
            voll = volr;
        } else {
            volr = voll;
        }
    }
    if (_svm_cur.seq_sep_no != 0x21) {
        voll = (voll * voll) / 16383;
        volr = (volr * volr) / 16383;
    }
    pos = voice * 8;
    SpuSetNoiseClock((_svm_cur.note - _svm_cur.tone_center) & 0x3F);
    ((u16*)_svm_sreg_buf)[pos + 0] = voll;
    ((u16*)_svm_sreg_buf)[pos + 1] = volr;
    _svm_sreg_dirty[voice] |= 3;
    if (voice < 16) {
        bitsLower = 1 << voice;
        bitsUpper = 0;
    } else {
        bitsLower = 0;
        bitsUpper = 1 << (voice - 16);
    }
    _svm_voice[voice].unk04 = 10;
    for (i = 0; i < _SsVmMaxVoice; i++) {
        if ((_snd_vmask & (1 << i)) == 0) {
            _svm_voice[i].unk1b &= 1;
        }
    }
    _svm_voice[voice].unk1b = 2;
    _svm_okon1 |= bitsLower;
    _svm_okon2 |= bitsUpper;
    _svm_okof1 &= ~_svm_okon1;
    _svm_okof2 &= ~_svm_okon2;
    if (_svm_cur.tone_mode & 4) {
        _svm_orev1 |= bitsLower;
        _svm_orev2 |= bitsUpper;
    } else {
        _svm_orev1 &= ~bitsLower;
        _svm_orev2 &= ~bitsUpper;
    }
    _svm_onos1 = bitsLower;
    _svm_onos2 = bitsUpper;
}
#else

void vmNoiseOn(unsigned char arg0) {
    struct SeqStruct* score =
        &_ss_score[_svm_cur.seq_sep_no & 0xFF]
                  [(_svm_cur.seq_sep_no & 0xFF00) >> 8];
    s16 voice;
    u16 bitsUpper;
    u16 bitsLower;
    u32 voll_t, volr_t;
    u32 voll, volr;
    u16 cnd;
    u32 temp;

    cnd = ((u16*)_svm_sreg)[0x1AA / 2];

    voll_t = score->voll * 0x81;
    volr_t = score->volr * 0x81;

    voll_t = (voll_t * _svm_cur.mvol) / 0x7F;
    volr_t = (volr_t * _svm_cur.mvol) / 0x7F;

    voll_t = (voll_t * _svm_cur.tone_vol) / 0x7F;
    volr_t = (volr_t * _svm_cur.tone_vol) / 0x7F;

    temp = _svm_cur.tone_pan;
    if (temp < 0x40) {
        voll = voll_t;
        volr = (volr_t * temp) / 0x3F;
    } else {
        voll = (voll_t * (0x7F - temp)) / 0x3F;
        volr = volr_t;
    }
    temp = _svm_cur.mpan;
    if (temp < 0x40) {
        volr = (volr * temp) / 0x3F;
    } else {
        voll = (voll * (0x7F - temp)) / 0x3F;
    }
    temp = _svm_cur.pan;
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
    cnd |= ((_svm_cur.note - _svm_cur.tone_center) & 0x3F) << 8;
    ((u16*)_svm_sreg)[0x1AA / 2] = cnd;

    ((short*)_svm_sreg_buf)[arg0 * 8 + 0] = voll;
    ((short*)_svm_sreg_buf)[arg0 * 8 + 1] = volr;
    _svm_sreg_dirty[arg0] |= 3;
    if (arg0 < 0x10) {
        bitsLower = 1 << arg0;
        bitsUpper = 0;
    } else {
        bitsLower = 0;
        bitsUpper = 1 << (arg0 - 0x10);
    }
    _svm_voice[arg0].unk04 = 0xA;
    for (voice = 0; voice < _SsVmMaxVoice; voice++) {
        _svm_voice[voice].unk1b &= 1;
    }
    _svm_voice[arg0].unk1b = 2;

    _svm_okon1 |= bitsLower;
    _svm_okon2 |= bitsUpper;

    _svm_okof1 &= ~_svm_okon1;
    _svm_okof2 &= ~_svm_okon2;

    if (_svm_cur.tone_mode & 4) {
        _svm_orev1 |= bitsLower;
        _svm_orev2 |= bitsUpper;
    } else {
        _svm_orev1 &= ~bitsLower;
        _svm_orev2 &= ~bitsUpper;
    }

    ((u16*)_svm_sreg)[0x194 / 2] = bitsLower;
    ((u16*)_svm_sreg)[0x196 / 2] = bitsUpper;
}
#endif
