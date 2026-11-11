#if VERSION == 37
#include "libspu_40.h"
#elif VERSION <= 30
#define SPU_OLD_LOCAL_RXX
#include "spu_old.h"
#else
#include "libspu_i.h"
#endif

#if VERSION <= 36
#if VERSION <= 30

static inline void SpuNGetVoiceAttr(s32 vNum, SpuVoiceAttr* arg, long shift) {
#define ADDR_SHIFT (shift + 1)
#else
static inline void SpuNGetVoiceAttr(s32 vNum, SpuVoiceAttr* arg) {
#define ADDR_SHIFT _spu_mem_mode_plus
#endif
    s16 var_a0;
    s16 var_a1;
    s32 temp_s2;
    s32 note;
    u16 adsr0;
    u16 adsr1;
    u16 voll;
    u16 volr;
    s32 temp_t0;

#if VERSION > 33
    var_a1 = 0;
#endif
#if VERSION <= 30
    temp_s2 = vNum * 8;
    voll = _spu_RXX->raw[temp_s2];
    volr = _spu_RXX->raw[temp_s2 + 1];
    temp_t0 = vNum * 2;
#else
    voll = _spu_RXX->raw[vNum * 8];
    volr = _spu_RXX->raw[vNum * 8 + 1];
    temp_s2 = vNum * 8;
    temp_t0 = vNum * 2;
#endif

#if VERSION <= 33
    if ((voll & 0x8000) == 0) {
        var_a1 = 0;
    } else {
#else
    if (voll & 0x8000) {
#endif
        switch (voll & 0xF000) {
        case 0x8000: var_a1 = 1; break;
        case 0x9000: var_a1 = 2; break;
        case 0xA000: var_a1 = 3; break;
        case 0xB000: var_a1 = 4; break;
        case 0xC000: var_a1 = 5; break;
        case 0xD000: var_a1 = 6; break;
        case 0xE000: var_a1 = 7; break;
        case 0xF000: var_a1 = 7; break;
        }
        voll &= 0xFFF;
    }

#if VERSION <= 33
    if ((volr & 0x8000) == 0) {
        var_a0 = 0;
    } else {
#else
    var_a0 = 0;
    if (volr & 0x8000) {
#endif
        switch (volr & 0xF000) {
        case 0x8000: var_a0 = 1; break;
        case 0x9000: var_a0 = 2; break;
        case 0xA000: var_a0 = 3; break;
        case 0xB000: var_a0 = 4; break;
        case 0xC000: var_a0 = 5; break;
        case 0xD000: var_a0 = 6; break;
        case 0xE000: var_a0 = 7; break;
        case 0xF000: var_a0 = 7; break;
        }
        volr &= 0xFFF;
    }

    {
        u32 x = 0x8000;
        u32 y = voll;
        if (voll < 0x4000) {
            arg->volume.left = voll;
        } else {
            arg->volume.left = y - x;
        }
    }
    {
        u32 x = 0x8000;
        u32 y = volr;
        if (volr < 0x4000) {
            arg->volume.right = volr;
        } else {
            arg->volume.right = y - x;
        }
    }
#if VERSION <= 33
    arg->volmode.left = var_a1;
    arg->volmode.right = var_a0;
#else
    arg->volmode.right = var_a0;
    arg->volmode.left = var_a1;
#endif
    arg->volumex.left = _spu_RXX->raw[temp_t0 + 0x100];
    arg->volumex.right = _spu_RXX->raw[temp_t0 + 0x101];
    arg->pitch = _spu_RXX->raw[temp_s2 + 2];
    note = _spu_pitch2note(_spu_voice_centerNote[vNum] >> 8,
                           _spu_voice_centerNote[vNum] & 0xFF, arg->pitch);
    if (note >= 0) {
        arg->note = note;
    } else {
        arg->note = 0;
    }
    arg->sample_note = _spu_voice_centerNote[vNum];
    arg->envx = _spu_RXX->raw[temp_s2 + 6];
    arg->addr = _spu_RXX->raw[temp_s2 + 3] << ADDR_SHIFT;
    arg->loop_addr = _spu_RXX->raw[temp_s2 + 7] << ADDR_SHIFT;
    adsr0 = _spu_RXX->raw[temp_s2 + 4];
    adsr1 = _spu_RXX->raw[temp_s2 + 5];
#if VERSION == 36
    if (adsr0 & 0x8000) {
        arg->a_mode = 5;
    } else {
        arg->a_mode = 1;
    }
    if (adsr1 & 0xC000) {
        arg->s_mode = 7;
    } else if (adsr1 & 0x8000) {
        arg->s_mode = 5;
    } else if (adsr1 & 0x4000) {
#else
    if (adsr0 & 0x80) {
        arg->a_mode = 5;
    } else {
        arg->a_mode = 1;
    }
    if (adsr1 & 0x300) {
        arg->s_mode = 7;
    } else if (adsr1 & 0x200) {
        arg->s_mode = 5;
    } else if (adsr1 & 0x100) {
#endif
        arg->s_mode = 3;
    } else {
        arg->s_mode = 1;
    }
    if (adsr1 & 0x20) {
        arg->r_mode = 7;
    } else {
        arg->r_mode = 3;
    }
#if VERSION == 36
    arg->ar = (adsr0 >> 8) & ~0x80;
#else
    arg->ar = adsr0 >> 8;
#endif
    arg->dr = (adsr0 & 0xF0) >> 4;
    arg->sr = (adsr1 >> 6) & ~0x300;
    arg->rr = adsr1 & 0x1F;
    arg->sl = adsr0 & 0xF;
    arg->adsr1 = adsr0;
    arg->adsr2 = adsr1;
}

void SpuGetVoiceAttr(SpuVoiceAttr* arg) {
    s32 voice;
    s32 i;
#if VERSION <= 30
    long shift = (_spu_RXX->rxx.data_trans & 0xE) >> 1;
#endif

    voice = -1;
    for (i = 0; i < 24; i++) {
        if (arg->voice & (1 << i)) {
            voice = i;
            break;
        }
    }
    if (voice != -1) {
#if VERSION <= 30
        SpuNGetVoiceAttr(voice, arg, shift);
#else
        SpuNGetVoiceAttr(voice, arg);
#endif
    }
}
#undef ADDR_SHIFT

#elif VERSION == 37

static inline void SpuNGetVoiceAttr(s32 vNum, SpuVoiceAttr *arg) {
    s16 var_a0;
    s16 var_a1;
    s32 temp_s2;
    s32 note;
    u16 adsr0;
    u16 adsr1;
    u16 voll;
    u16 volr;
    int temp_t0;

    var_a1 = 0;

    voll = _spu_RXX->raw[vNum * 8 + 0];
    volr = _spu_RXX->raw[vNum * 8 + 1];
    temp_s2 = vNum * 8;
    temp_t0 = vNum * 2;

    if (voll & 0x8000) {
        switch (voll & 0xF000) {
            case 0x8000:
                var_a1 = 1;
            break;
            case 0x9000:
                var_a1 = 2;
            break;
            case 0xA000:
                var_a1 = 3;
            break;
            case 0xB000:
                var_a1 = 4;
            break;
            case 0xC000:
                var_a1 = 5;
            break;
            case 0xD000:
                var_a1 = 6;
            break;
            case 0xE000:
                var_a1 = 7;
            break;
            case 0xF000:
                var_a1 = 7;
            break;
        }
        voll &= 0xFFF;
    }

    var_a0 = 0;
    if (volr & 0x8000) {
        switch (volr & 0xF000) {
            case 0x8000:
                var_a0 = 1;
            break;
            case 0x9000:
                var_a0 = 2;
            break;
            case 0xA000:
                var_a0 = 3;
            break;
            case 0xB000:
                var_a0 = 4;
            break;
            case 0xC000:
                var_a0 = 5;
            break;
            case 0xD000:
                var_a0 = 6;
            break;
            case 0xE000:
                var_a0 = 7;
            break;
            case 0xF000:
                var_a0 = 7;
            break;
        }
        volr &= 0xFFF;
    }

    voll++; voll--;

    CALC_VOL(arg->volume.left, voll);
    CALC_VOL(arg->volume.right, volr);

    arg->volmode.right = var_a0;
    arg->volmode.left = var_a1;

    arg->volumex.left = _spu_RXX->raw[temp_t0 + 0x100];
    arg->volumex.right = _spu_RXX->raw[temp_t0 + 0x101];

    arg->pitch = _spu_RXX->raw[temp_s2 + 2];

    note = _spu_pitch2note(
        _spu_voice_centerNote[vNum] >> 8,
        _spu_voice_centerNote[vNum] & 0xFF,
        arg->pitch);

    if (note >= 0) {
        arg->note = note;
    } else {
        arg->note = 0;
    }

    arg->sample_note = _spu_voice_centerNote[vNum];
    arg->envx = _spu_RXX->raw[temp_s2 + 6];
    arg->addr = _spu_RXX->raw[temp_s2 + 3] << _spu_mem_mode_plus;
    arg->loop_addr = _spu_RXX->raw[temp_s2 + 7] << _spu_mem_mode_plus;
    adsr0 = _spu_RXX->raw[temp_s2 + 4];
    adsr1 = _spu_RXX->raw[temp_s2 + 5];

    if (adsr0 & 0x8000) {
        arg->a_mode = 5;
    } else {
        arg->a_mode = 1;
    }

    if ((adsr1 & 0xE000) == 0xC000) {
        arg->s_mode = 7;
    } else if ((adsr1 & 0xE000) == 0x8000) {
        arg->s_mode = 5;
    } else if ((adsr1 & 0xE000) == 0x4000) {
        arg->s_mode = 3;
    } else {
        arg->s_mode = 1;
    }

    if (adsr1 & 0x20) {
        arg->r_mode = 7;
    } else {
        arg->r_mode = 3;
    }

    arg->ar = (adsr0 >> 8) & 0x3F;
    arg->dr = (adsr0 & 0xF0) >> 4;
    arg->sr = (adsr1 >> 6) & ~0xFF80;
    arg->rr = adsr1 & 0x1F;
    arg->sl = adsr0 & 0xF;
    arg->adsr1 = adsr0;
    arg->adsr2 = adsr1;
}

void SpuGetVoiceAttr(SpuVoiceAttr* arg) {
    s32 voice;
    s32 i;

    voice = -1;
    for (i = 0; i < 24; i++) {
        if (arg->voice & (1 << i)) {
            voice = i;
            break;
        }
    }
    if (voice != -1) {
        SpuNGetVoiceAttr(voice, arg);
    }
}

#else

void SpuGetVoiceAttr(SpuVoiceAttr *arg) {
    s32 var_a0;
    s32 var_v1;

    var_a0 = -1;
    for (var_v1 = 0; var_v1 < 24; var_v1++) {
        if (arg->voice & (1 << var_v1)) {
            var_a0 = var_v1;
            break;
        }
    }

    if (var_a0 != -1) {
        SpuNGetVoiceAttr(var_a0, arg);
    }
}

#endif
