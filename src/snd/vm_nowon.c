#include "libsnd_i.h"

#if VERSION >= 41
void _SsVmKeyOnNow(unsigned short vagCount, unsigned short pitch) {
    struct SeqStruct* score;
    u16 pos;
    u16 bitsUpper;
    u16 bitsLower;
    u32 voll, volr;
    u32 voll_t, volr_t;
    s32 mvol_scaled;
    u8 pv;

    pos = _svm_cur.voice * 8;
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
    pv = _svm_cur.tone_pan;
    if (pv < 64) {
        voll = voll_t;
        volr = (volr_t * pv) / 63;
    } else {
        voll = (voll_t * (127 - pv)) / 63;
        volr = volr_t;
    }
    pv = _svm_cur.mpan;
    if (pv < 64) {
        volr = (volr * pv) / 63;
    } else {
        voll = (voll * (127 - pv)) / 63;
    }
    pv = _svm_cur.pan;
    if (pv < 64) {
        volr = (volr * pv) / 63;
    } else {
        voll = (voll * (127 - pv)) / 63;
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
    ((u16*)_svm_sreg_buf)[pos + 2] = pitch;
    ((u16*)_svm_sreg_buf)[pos + 0] = voll;
    ((u16*)_svm_sreg_buf)[pos + 1] = volr;
    _svm_sreg_dirty[_svm_cur.voice] |= 7;
    _svm_voice[_svm_cur.voice].unk04 = pitch;
    if (_svm_cur.voice < 16) {
        bitsLower = 1 << _svm_cur.voice;
        bitsUpper = 0;
    } else {
        bitsLower = 0;
        bitsUpper = 1 << (_svm_cur.voice - 16);
    }
    if (_svm_cur.tone_mode & 4) {
        _svm_orev1 |= bitsLower;
        _svm_orev2 |= bitsUpper;
    } else {
        _svm_orev1 &= ~bitsLower;
        _svm_orev2 &= ~bitsUpper;
    }
    _svm_onos1 &= ~bitsLower;
    _svm_onos2 &= ~bitsUpper;
    _svm_okon1 |= bitsLower;
    _svm_okon2 |= bitsUpper;
    _svm_okof1 &= ~_svm_okon1;
    _svm_okof2 &= ~_svm_okon2;
}
#else

#if VERSION == 36
void _SsVmKeyOnNow(unsigned short vagCount, unsigned short pitch) {
    struct SeqStruct* score =
        &_ss_score[_svm_cur.seq_sep_no & 0xFF]
                  [(_svm_cur.seq_sep_no & 0xFF00) >> 8];
    u16 pos;
    u16 bitsUpper;
    u16 bitsLower;
    u32 voll, volr;
    u32 voll_t, volr_t;
    s32 mvol_scaled;

    pos = _svm_cur.voice * 8;
    mvol_scaled = _svm_vh->mvol * 0x3FFF;
    voll_t = ((_svm_cur.volume * mvol_scaled) / 0x7F) / 0x7F;
    voll_t =
        ((voll_t * _svm_cur.mvol * _svm_cur.tone_vol) / 0x7F) / 0x7F;
    volr_t = voll_t;

    if (_svm_cur.seq_sep_no != 0x21) {
        voll_t = (voll_t * score->voll) / 0x7F;
        volr_t = (volr_t * score->volr) / 0x7F;
    }

    if (_svm_cur.tone_pan < 0x40) {
        voll = voll_t;
        volr = (volr_t * _svm_cur.tone_pan) / 0x3F;
    } else {
        voll = (voll_t * (0x7F - _svm_cur.tone_pan)) / 0x3F;
        volr = volr_t;
    }
    if (_svm_cur.mpan < 0x40) {
        volr = (volr * _svm_cur.mpan) / 0x3F;
    } else {
        voll = (voll * (0x7F - _svm_cur.mpan)) / 0x3F;
    }
    if (_svm_cur.pan < 0x40) {
        volr = (volr * _svm_cur.pan) / 0x3F;
    } else {
        voll = (voll * (0x7F - _svm_cur.pan)) / 0x3F;
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

    ((short*)_svm_sreg_buf)[pos + 2] = pitch;
    ((short*)_svm_sreg_buf)[pos + 0] = voll;
    ((short*)_svm_sreg_buf)[pos + 1] = volr;
    _svm_sreg_dirty[_svm_cur.voice] |= 7;

    _svm_voice[_svm_cur.voice].unk04 = pitch;
    _svm_voice[_svm_cur.voice].unk1b = 1;
    if (_svm_cur.voice < 0x10) {
        bitsLower = 1 << _svm_cur.voice;
        bitsUpper = 0;
    } else {
        bitsLower = 0;
        bitsUpper = 1 << (_svm_cur.voice - 0x10);
    }
    if (_svm_cur.tone_mode & 4) {
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
#else
void _SsVmKeyOnNow(unsigned short vagCount, unsigned short pitch) {
    int pad[2];
    unsigned chL;
    unsigned chR;
    unsigned vol;
    short keyon1, keyon2;
    struct SeqStruct* ss;
    unsigned short voice;
    int volMul;

    volMul = _svm_vh->mvol * 16383;
    vol = (_svm_cur.volume * volMul) / 16129;
    chR = ((vol * _svm_cur.mvol) * _svm_cur.tone_vol) / 16129;
    voice = _svm_cur.voice * 8;
    vol = chR;
    ss = &_ss_score[_svm_cur.seq_sep_no & 0xFF]
                   [(_svm_cur.seq_sep_no >> 8) & 0xFF];
    if (_svm_cur.seq_sep_no != 0x21) {
        vol = (chR * ss->voll) / 127;
        chR = (chR * ss->volr) / 127;
    }
    if (_svm_cur.tone_pan < 64) {
        chR = (chR * _svm_cur.tone_pan) / 63;
        chL = vol;
    } else {
        chL = (vol * (127 - _svm_cur.tone_pan)) / 63;
    }
    if (_svm_cur.mpan < 64) {
        chR = (chR * _svm_cur.mpan) / 63;
    } else {
        chL = (chL * (127 - _svm_cur.mpan)) / 63;
    }
    if (_svm_cur.pan < 64) {
        chR = (chR * _svm_cur.pan) / 63;
    } else {
        chL = (chL * (127 - _svm_cur.pan)) / 63;
    }
    if (_svm_stereo_mono == 1) {
        if (chL < chR) {
            chL = chR;
        } else {
            chR = chL;
        }
    }
    chL = (chL * chL) / 16383;
    chR = (chR * chR) / 16383;
    ((short*)_svm_sreg_buf)[voice + 2] = pitch;
    ((short*)_svm_sreg_buf)[voice + 0] = chL;
    ((short*)_svm_sreg_buf)[voice + 1] = chR;
    _svm_sreg_dirty[_svm_cur.voice] |= 7;
    _svm_voice[_svm_cur.voice].unk04 = pitch;
#if VERSION < 40
    _svm_voice[_svm_cur.voice].unk1b = 1;
#endif
    if (_svm_cur.voice < 16) {
        keyon1 = 1 << _svm_cur.voice;
        keyon2 = 0;
    } else {
        keyon1 = 0;
        keyon2 = 1 << (_svm_cur.voice - 16);
    }
    if (_svm_cur.tone_mode & 4) {
        _svm_orev1 |= keyon1;
        _svm_orev2 |= keyon2;
    } else {
        _svm_orev1 &= ~keyon1;
        _svm_orev2 &= ~keyon2;
    }
    _svm_okon1 |= keyon1;
    _svm_okon2 |= keyon2;
    _svm_okof1 &= ~_svm_okon1;
    _svm_okof2 &= ~_svm_okon2;
}
#endif
#endif
