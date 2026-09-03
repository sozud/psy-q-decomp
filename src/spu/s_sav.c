#include "libspu_i.h"

#if VERSION == 40
u32 _SpuSetAnyVoice(long on_off, unsigned long voice_bit, s32 arg2, s32 arg3) {
    s32 var_t2;

    if (_spu_env & 1) {
        var_t2 = ((_spu_RQ[arg3 - 0xC4] & 0xFF) << 0x10) | _spu_RQ[arg2 - 0xC4];
    } else {
        var_t2 = ((_spu_RXX->raw[arg3] & 0xFF) << 0x10) | _spu_RXX->raw[arg2];
    }

    switch (on_off) {
        case 1:
            if (_spu_env & 1) {
                _spu_RQ[arg2 - 0xC4] |= voice_bit;
                _spu_RQ[arg3 - 0xC4] |= (voice_bit >> 0x10) & 0xFF;
                _spu_RQmask |= 1 << ((arg2 - 0xC6) >> 1);
            } else {
                _spu_RXX->raw[arg2] |= voice_bit;
                _spu_RXX->raw[arg3] |= (voice_bit >> 0x10) & 0xFF;
            }
            var_t2 |= voice_bit & 0xFFFFFF;
        break;
        case 0:
            if (_spu_env & 1) {
                _spu_RQ[arg2 - 0xC4] &= ~voice_bit;
                _spu_RQ[arg3 - 0xC4] &= ~((voice_bit >> 0x10) & 0xFF);
                _spu_RQmask |= 1 << ((arg2 - 0xC6) >> 1);
            } else {
                _spu_RXX->raw[arg2] &= ~voice_bit;
                _spu_RXX->raw[arg3] &= ~((voice_bit >> 0x10) & 0xFF);
            }
            var_t2 &= ~(voice_bit & 0xFFFFFF);
        break;
    }

    return var_t2 & 0xFFFFFF;
}

#else

s32 _SpuSetAnyVoice(s32 on_off, u32 bits, s32 addr1, s32 addr2) {
    s32 var_t0;
    u16 var_v1;
    u16* temp_a2;
    u16* temp_a3;
    u16 temp;

    temp_a3 = &_spu_RXX->raw[addr2];
    temp_a2 = &_spu_RXX->raw[addr1];
    var_t0 = *temp_a2 | ((_spu_RXX->raw[addr2] & 0xff) << 0x10);
    if (on_off != 0) {
        if (on_off == 1) {
            var_t0 |= bits & 0xFFFFFF;
            *temp_a2 |= bits;
            var_v1 = *temp_a3 | ((bits >> 0x10) & 0xFF);
            *temp_a3 = var_v1;
        }
    } else {
        var_t0 &= ~(bits & 0xFFFFFF);
        *temp_a2 &= ~bits;
        var_v1 = *temp_a3 & ~((bits >> 0x10) & 0xFF);
        *temp_a3 = var_v1;
    }
    return var_t0 & 0xFFFFFF;
}

#endif
