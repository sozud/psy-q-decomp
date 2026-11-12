#include "libsnd_i.h"

SPU_RXX* _svm_sreg = (SPU_RXX*)0x1F801C00;
SPU_VOICE_REG _svm_sreg_buf[NUM_VOICES];
char _svm_sreg_dirty[NUM_VOICES];
unsigned short _svm_okon1;
unsigned short _svm_okon2;
unsigned short _svm_okof1;
unsigned short _svm_okof2;
unsigned short _svm_orev1;
unsigned short _svm_orev2;
struct SpuVoice _svm_voice[NUM_VOICES];
unsigned int _svm_envx_ptr;
int _svm_envx_hist[16];
void (*_autovol)();
void (*_autopan)();

#if VERSION == 36
void _SsVmFlush(void) {
    u32 env_mask;
    s32 i;

    _svm_envx_ptr = (_svm_envx_ptr + 1) & 0xF;
    _svm_envx_hist[_svm_envx_ptr] = 0;

    for (i = 0; i < _SsVmMaxVoice; i++) {
        _svm_voice[i].key_stat = ((u16*)_svm_sreg)[i * 8 + 6];
        if (_svm_voice[i].key_stat == 0) {
            _svm_envx_hist[_svm_envx_ptr] |= 1 << i;
        }
    }
    if (_svm_auto_kof_mode == 0) {
        env_mask = 0xFFFFFFFF;
        for (i = 0; i < 0xF; i++) {
            env_mask &= _svm_envx_hist[i];
        }

        for (i = 0; i < _SsVmMaxVoice; i++) {
            if (env_mask & (1 << i)) {
                if (_svm_voice[i].unk1b == 2) {
                    SpuSetNoiseVoice(0, 0xFFFFFF);
                }
                _svm_voice[i].unk1b = 0;
            }
        }
    }

    _svm_okon1 &= ~_svm_okof1;
    _svm_okon2 &= ~_svm_okof2;
    for (i = 0; i < NUM_VOICES; i++) {
        if (_svm_voice[i].auto_vol != 0) {
            _autovol(i);
        }
        if (_svm_voice[i].auto_pan != 0) {
            _autopan(i);
        }
    }

    for (i = 0; i < NUM_VOICES; i++) {
        if (_svm_sreg_dirty[i] & 1) {
            ((u16*)_svm_sreg)[i * 8 + 0] = ((short*)_svm_sreg_buf)[i * 8 + 0];
            ((u16*)_svm_sreg)[i * 8 + 1] = ((short*)_svm_sreg_buf)[i * 8 + 1];
        }
        if (_svm_sreg_dirty[i] & 4) {
            ((u16*)_svm_sreg)[i * 8 + 2] = ((short*)_svm_sreg_buf)[i * 8 + 2];
        }
        if (_svm_sreg_dirty[i] & 8) {
            ((u16*)_svm_sreg)[i * 8 + 3] = ((short*)_svm_sreg_buf)[i * 8 + 3];
        }
        if (_svm_sreg_dirty[i] & 0x10) {
            ((u16*)_svm_sreg)[i * 8 + 4] = ((short*)_svm_sreg_buf)[i * 8 + 4];
            ((u16*)_svm_sreg)[i * 8 + 5] = ((short*)_svm_sreg_buf)[i * 8 + 5];
        }
        _svm_sreg_dirty[i] = 0;
    }

    ((u16*)_svm_sreg)[0x18C / 2] = _svm_okof1;
    ((u16*)_svm_sreg)[0x18E / 2] = _svm_okof2;
    ((u16*)_svm_sreg)[0x188 / 2] = _svm_okon1;
    ((u16*)_svm_sreg)[0x18A / 2] = _svm_okon2;
    ((u16*)_svm_sreg)[0x198 / 2] = _svm_orev1;
    ((u16*)_svm_sreg)[0x19A / 2] = _svm_orev2;
    _svm_okof1 = 0;
    _svm_okof2 = 0;
    _svm_okon1 = 0;
    _svm_okon2 = 0;
}
#else
void _SsVmFlush(void) {
    s32 i;
    unsigned int var_s2;
    u16 temp_a0;
    u16 temp_a1;
    u16 temp_a2;
    u16 temp_v1;
    u16 rev1;
    u16 rev2;
    SPU_RXX* sreg;

    _svm_envx_ptr = (_svm_envx_ptr + 1) & 0xF;
    _svm_envx_hist[_svm_envx_ptr] = 0;
    for (i = 0; i < _SsVmMaxVoice; i++) {
        _svm_voice[i].key_stat = SPURV(i, volumex);
        if (!_svm_voice[i].key_stat) {
            _svm_envx_hist[_svm_envx_ptr] |= 1 << i;
        }
    }
    if (_svm_auto_kof_mode == 0) {
        var_s2 = -1;
        for (i = 0; i < LEN(_svm_envx_hist) - 1; i++) {
            var_s2 &= _svm_envx_hist[i];
        }
        for (i = 0; i < _SsVmMaxVoice; i++) {
            if (var_s2 & (1 << i)) {
                if (_svm_voice[i].unk1b == 2) {
                    SpuSetNoiseVoice(0, 0xFFFFFF);
                }
                _svm_voice[i].unk1b = 0;
            }
        }
    }
    _svm_okon1 &= ~_svm_okof1;
    _svm_okon2 &= ~_svm_okof2;
    for (i = 0; i < NUM_VOICES; i++) {
        if (_svm_voice[i].auto_vol) {
            _autovol(i);
        }
        if (_svm_voice[i].auto_pan) {
            _autopan(i);
        }
    }
    {
        char* dirty = _svm_sreg_dirty;
        u16* base = (u16*)_svm_sreg_buf;
        u16* adsr2 = base + 5;
        u16* adsr1 = base + 4;
        int offset = 0;
        u16* right = base + 1;
        u16* left = base;
        char* dirtyEnd = _svm_sreg_dirty + NUM_VOICES;

        do {
            if (*dirty & 1) {
                u16* spuVoice = (u16*)(offset + (u32)_svm_sreg);
                spuVoice[0] = *left;
                spuVoice[1] = *right;
            }
            if (*dirty & 4) {
                u16* spuVoice = (u16*)(offset + (u32)_svm_sreg);
                spuVoice[2] = *(u16*)((u8*)_svm_sreg_buf + offset + 4);
            }
            if (*dirty & 8) {
                u16* spuVoice = (u16*)(offset + (u32)_svm_sreg);
                spuVoice[3] = *(u16*)((u8*)_svm_sreg_buf + offset + 6);
            }
            if (*dirty & 0x10) {
                u16* spuVoice = (u16*)(offset + (u32)_svm_sreg);
                spuVoice[4] = *adsr1;
                spuVoice[5] = *adsr2;
            }
            *dirty++ = 0;
            adsr2 += 8;
            adsr1 += 8;
            offset += sizeof(SPU_VOICE_REG);
            right += 8;
            left += 8;
        } while ((s32)dirty < (s32)dirtyEnd);
    }
    sreg = _svm_sreg;
    temp_v1 = _svm_okof1;
    temp_a0 = _svm_okof2;
    temp_a1 = _svm_okon1;
    temp_a2 = _svm_okon2;
    rev1 = _svm_orev1;
    rev2 = _svm_orev2;
    _svm_okof1 = 0;
    _svm_okof2 = 0;
    _svm_okon1 = 0;
    _svm_okon2 = 0;
    sreg->key_off[0] = temp_v1;
    sreg->key_off[1] = temp_a0;
    sreg->key_on[0] = temp_a1;
    sreg->key_on[1] = temp_a2;
    sreg->rev_mode[0] = rev1;
    sreg->rev_mode[1] = rev2;
}
#endif
