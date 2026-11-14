#include "../libspu_i.h"

#if defined(SV_ALL)
void SpuSetVoiceAttr(SpuVoiceAttr *arg)
#elif defined(SV_N)
void SpuNSetVoiceAttr(int voice, SpuVoiceAttr *arg)
#elif defined(SV_L)
void SpuLSetVoiceAttr(int num, SpuLVoiceAttr *argList)
#else
long SpuRSetVoiceAttr(long min_, long max_, SpuVoiceAttr *arg)
#endif
{

#if !defined(SV_N)
    long voice;
#endif
#if defined(SV_L)
    SpuVoiceAttr *arg;
#endif
    long ch;
    unsigned long mask;
    long all;
    unsigned long vol;
    unsigned long mode;
    unsigned long amode;
    u16 rr;

#if defined(SV_L)
    for (; num--; argList++) {
        voice = argList->voiceNum;
        arg = &argList->attr;
        mask = arg->mask;
        all = mask == 0;
#else
    mask = arg->mask;
    all = mask == 0;
#endif
#if defined(SV_L)
#elif defined(SV_ALL)
    for (voice = 0; voice < 24; voice++) {
        if ((arg->voice & (1 << voice)) == 0) {
            continue;
        }
#elif defined(SV_N)
    {
#else
    if (min_ < 0) {
        min_ = 0;
    }
    if (min_ >= 24) {
        return -3;
    }
    if (max_ >= 24) {
        max_ = 23;
    }
    if (max_ < 0) {
        return -3;
    }
    if (max_ < min_) {
        return -3;
    }
    max_++;
    for (voice = min_; voice < max_; voice++) {
        if ((arg->voice & (1 << voice)) == 0) {
            continue;
        }
#endif
        ch = voice * 8;
        if (all || (mask & 0x10)) {
            _spu_RXX->raw[ch + 2] = arg->pitch;
        }
        if (all || (mask & 0x40)) {
            _spu_voice_centerNote[voice] = arg->sample_note;
        }
        if (all || (mask & 0x20)) {
            _spu_RXX->raw[ch + 2] = _spu_note2pitch(_spu_voice_centerNote[voice] >> 8, _spu_voice_centerNote[voice] & 0xFF,
                                               arg->note >> 8, arg->note & 0xFF);
        }
        if (all || (mask & 0x1)) {
            vol = arg->volume.left & 0x7FFF;
            mode = 0;
            if (all || (mask & 0x4)) {
                switch (arg->volmode.left) {
                case 1:
                    mode = 0x8000;
                    break;
                case 2:
                    mode = 0x9000;
                    break;
                case 3:
                    mode = 0xA000;
                    break;
                case 4:
                    mode = 0xB000;
                    break;
                case 5:
                    mode = 0xC000;
                    break;
                case 6:
                    mode = 0xD000;
                    break;
                case 7:
                    mode = 0xE000;
                    break;
                }
            }
            if (mode) {
                if (arg->volume.left >= 0x80) {
                    vol = 0x7F;
                } else if (arg->volume.left < 0) {
                    vol = 0;
                }
            }
            _spu_RXX->raw[ch] = vol | mode;
        }
        if (all || (mask & 0x2)) {
            vol = arg->volume.right & 0x7FFF;
            mode = 0;
            if (all || (mask & 0x8)) {
                switch (arg->volmode.right) {
                case 1:
                    mode = 0x8000;
                    break;
                case 2:
                    mode = 0x9000;
                    break;
                case 3:
                    mode = 0xA000;
                    break;
                case 4:
                    mode = 0xB000;
                    break;
                case 5:
                    mode = 0xC000;
                    break;
                case 6:
                    mode = 0xD000;
                    break;
                case 7:
                    mode = 0xE000;
                    break;
                }
            }
            if (mode) {
                if (arg->volume.right >= 0x80) {
                    vol = 0x7F;
                } else if (arg->volume.right < 0) {
                    vol = 0;
                }
            }
            _spu_RXX->raw[ch + 1] = vol | mode;
        }
        if (all || (mask & 0x80)) {
            _spu_FsetRXXa(ch | 3, arg->addr);
        }
        if (all || (mask & 0x10000)) {
            _spu_FsetRXXa(ch | 7, arg->loop_addr);
        }
        if (all || (mask & 0x20000)) {
            _spu_RXX->raw[ch + 4] = arg->adsr1;
        }
        if (all || (mask & 0x40000)) {
            _spu_RXX->raw[ch + 5] = arg->adsr2;
        }
        if (all || (mask & 0x800)) {
            vol = arg->ar;
            if (vol >= 0x80) {
                vol = 0x7F;
            }
            amode = 0;
            if (all || (mask & 0x100)) {
                if (arg->a_mode == 5) {
                    amode = 0x80;
                }
            }
            rr = _spu_RXX->raw[ch + 4];
            _spu_RXX->raw[ch + 4] = (rr & 0xFF) | ((vol | amode) << 8);
        }
        if (all || (mask & 0x1000)) {
            vol = arg->dr;
            if (vol >= 0x10) {
                vol = 0xF;
            }
            rr = _spu_RXX->raw[ch + 4];
            _spu_RXX->raw[ch + 4] = (rr & 0xFF0F) | (vol << 4);
        }
        if (all || (mask & 0x2000)) {
            vol = arg->sr;
            if (vol >= 0x80) {
                vol = 0x7F;
            }
            amode = 0x100;
            if (all || (mask & 0x200)) {
                switch (arg->s_mode) {
                case 1:
                    amode = 0;
                    break;
                case 5:
                    amode = 0x200;
                    break;
                case 7:
                    amode = 0x300;
                    break;
                }
            }
            rr = _spu_RXX->raw[ch + 5];
            _spu_RXX->raw[ch + 5] = (rr & 0x3F) | ((vol | amode) << 6);
        }
        if (all || (mask & 0x4000)) {
            vol = arg->rr;
            if (vol >= 0x20) {
                vol = 0x1F;
            }
            amode = 0;
            if (all || (mask & 0x400)) {
                switch (arg->r_mode) {
                case 3:
                    amode = 0;
                    break;
                case 7:
                    amode = 0x20;
                    break;
                }
            }
            rr = _spu_RXX->raw[ch + 5];
            _spu_RXX->raw[ch + 5] = (rr & 0xFFC0) | (vol | amode);
        }
        if (all || (mask & 0x8000)) {
            vol = arg->sl;
            if (vol >= 0x10) {
                vol = 0xF;
            }
            rr = _spu_RXX->raw[ch + 4];
            _spu_RXX->raw[ch + 4] = (rr & 0xFFF0) | vol;
        }
        }
    {
        volatile long i;
        volatile long j;

        j = 1;
        for (i = 0; i < 2; i = i + 1) {
            j = j * 13;
        }
    }
#if defined(SV_R)
    return 0;
#endif
}
