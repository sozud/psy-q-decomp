#include "libsnd_i.h"

#if VERSION >= 43
void _SsVmDoAllocate(void) {
    int i;
    short voiceOffset;
    short damper;
    VagAtr* tn;
    int idx;
    unsigned short adsr2;

    voiceOffset = _svm_cur.voice << 3;
    _svm_voice[_svm_cur.voice].key_stat = 0x7FFF;
    for (i = 0; i < NUM_VAB; i++) {
        _svm_envx_hist[i] &= ~(1 << _svm_cur.voice);
    }
    if ((_svm_cur.tone_vag_idx & 1) > 0) {
        ((short*)_svm_sreg_buf)[voiceOffset + 3] =
            ((u16*)&_svm_pg[(_svm_cur.tone_vag_idx - 1) / 2].reserved2)[0];
        _svm_sreg_dirty[_svm_cur.voice] |= 8;
    } else {
        ((short*)_svm_sreg_buf)[voiceOffset + 3] =
            ((u16*)&_svm_pg[(_svm_cur.tone_vag_idx - 1) / 2].reserved2)[1];
        _svm_sreg_dirty[_svm_cur.voice] |= 8;
    }
    idx = _svm_cur.fake_program * 16 + _svm_cur.tone;
    ((short*)_svm_sreg_buf)[voiceOffset + 4] = _svm_tn[idx].adsr1;
    tn = &_svm_tn[idx];
    adsr2 = tn->adsr2 & ~0x1F;
    damper = _svm_damper + (tn->adsr2 & 0x1F);
    if (damper >= 32) {
        damper = 31;
    }
    damper |= adsr2;
    ((short*)_svm_sreg_buf)[voiceOffset + 5] = damper;
    _svm_sreg_dirty[_svm_cur.voice] |= 0x30;
}
#else

void _SsVmDoAllocate(void) {
    int i;

    _svm_cur.voiceOffset = _svm_cur.voice * sizeof(SPU_VOICE_REG) / 2;
    _svm_cur.field_0x1e = _svm_cur.fake_program * 16 + _svm_cur.tone;
    _svm_voice[_svm_cur.voice].key_stat = 0x7FFF;
    for (i = 0; i < NUM_VAB; i++) {
        _svm_envx_hist[i] &= ~(1 << _svm_cur.voice);
    }
    if ((_svm_cur.tone_vag_idx & 1) > 0) {
        ((short*)_svm_sreg_buf)[_svm_cur.voiceOffset + 3] =
            ((u16*)&_svm_pg[(_svm_cur.tone_vag_idx - 1) / 2].reserved2)[0];
        _svm_sreg_dirty[_svm_cur.voice] |= 8;
    } else {
        ((short*)_svm_sreg_buf)[_svm_cur.voiceOffset + 3] =
            ((u16*)&_svm_pg[(_svm_cur.tone_vag_idx - 1) / 2].reserved2)[1];
        _svm_sreg_dirty[_svm_cur.voice] |= 8;
    }
    ((short*)_svm_sreg_buf)[_svm_cur.voiceOffset + 4] =
        _svm_tn[_svm_cur.fake_program * 16 + _svm_cur.tone].adsr1;
    ((short*)_svm_sreg_buf)[_svm_cur.voiceOffset + 5] =
        _svm_tn[_svm_cur.fake_program * 16 + _svm_cur.tone].adsr2 + _svm_damper;
    _svm_sreg_dirty[_svm_cur.voice] |= 0x30;
}
#endif
