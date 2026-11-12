#include "libsnd_i.h"


s16 SsUtGetDetVVol(s16 voice, s16* voll, s16* volr) {
    if (voice >= 0 && voice < NUM_VOICES) {
        *voll = ((u16*)_svm_sreg)[voice * 8 + 0];
        *volr = ((u16*)_svm_sreg)[voice * 8 + 1];
        return 0;
    }
    return -1;
}

short SsUtSetDetVVol(short voice, short arg1, short arg2) {
    if (voice >= 0 && voice < NUM_VOICES) {
        ((short*)_svm_sreg_buf)[voice * 8 + 0] = arg1;
        ((short*)_svm_sreg_buf)[voice * 8 + 1] = arg2;
        _svm_sreg_dirty[voice] |= 3;
        return 0;
    }

    return -1;
}

s16 SsUtGetVVol(s16 voice, s16* voll, s16* volr) {
    if (voice >= 0 && voice < NUM_VOICES) {
        s16 temp1 = ((u16*)_svm_sreg)[voice * 8 + 0];
        s16 temp2 = ((u16*)_svm_sreg)[voice * 8 + 1];
        *voll = temp1 / 0x81;
        *volr = temp2 / 0x81;
        return 0;
    }
    return -1;
}

#if VERSION == 40
s16 SsUtSetVVol(s16 voice, s16 voll, s16 volr) {
    unsigned short l;
    unsigned short r;

    if (voice >= 0 && voice < NUM_VOICES) {
        l = voll * 0x81;
        r = volr * 0x81;
        l = (l * l) / 16383;
        r = (r * r) / 16383;
        ((short*)_svm_sreg_buf)[voice * 8 + 1] = r;
        ((short*)_svm_sreg_buf)[voice * 8 + 0] = l;
        _svm_sreg_dirty[voice] |= 3;
        return 0;
    }
    return -1;
}
#else
s16 SsUtSetVVol(s16 voice, s16 voll, s16 volr) {
    if (voice >= 0 && voice < NUM_VOICES) {
        voll *= 0x81;
        volr *= 0x81;
        ((short*)_svm_sreg_buf)[voice * 8 + 1] = volr;
        ((short*)_svm_sreg_buf)[voice * 8 + 0] = voll;
        _svm_sreg_dirty[voice] |= 3;
        return 0;
    }

    return -1;
}
#endif
