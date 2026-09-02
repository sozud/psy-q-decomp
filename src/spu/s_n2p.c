#include "libspu_i.h"
inline u32 _spu_2pitch(s32 arg0, u32 arg1) {
    u32 sp8;
    s32 var_t0;
    s32 temp_a3;
    u32 var_v1;
    u32 _arg1;
    int i;

    var_t0 = arg0 << 0xC;
    var_v1 = 0x103B;
    temp_a3 = arg1 / 32;
    _arg1 = arg1 % 32;
    sp8 = arg0 * var_v1;
    for (i = 0; i < temp_a3; i++) {
        var_t0 = arg0 * var_v1;
        var_v1 *= 0x103B;
        var_v1 >>= 0xC;
        sp8 = arg0 * var_v1;
    }

    return (var_t0 + (((sp8 - var_t0) >> 5) * _arg1)) >> 0xC;
}

u16 _spu_note2pitch(u16 arg0, u16 arg1, u16 arg2, u16 arg3) {
    s32 temp_a2;
    s32 var_a0;
    s32 var_a1;
    u16 var_v0_2;
    s32 var_v1;
    u32 var_v0;
    s32 _arg0;
    s32 _arg2;

    _arg0 = (arg0 << 7) + arg1;
    _arg2 = (arg2 << 7) + arg3;
    temp_a2 = _arg2 - _arg0;
    var_a1 = ABS(temp_a2);

    var_a0 = var_a1 / 1536;
    var_v1 = var_a1 % 1536;

    if (temp_a2 >= 0) {
        var_v0_2 = 0x1000 << var_a0;
    } else {
        if (var_v1 != 0) {
            var_a0 += 1;
            var_v1 = 0x600 - var_v1;
        }
        var_v0_2 = 0x1000 >> var_a0;
    }

    var_v0 = _spu_2pitch(var_v0_2, ABS(var_v1));

    if (var_v0 >= 0x4000) {
        var_v0 = 0x3FFF;
    }

    return var_v0;
}

s32 _spu_pitch2note(u16 arg0, u16 arg1, u16 arg2) {
    s32 lsb;
    s32 i;
    u16 inv;
    s32 octave;
    u32 mask;
    u32 t0;
    u32 val1, val2;
    u32 step;
    s32 j;
    u32 base;
    u32 acc, acc_step;
    s32 val;
    s32 s, f;
    s32 note, fine;

    inv = ~arg2;
    lsb = 0;
    i = 15;
    while (i >= 0) {
        if (!((inv >> i) & 1)) {
            lsb = i;
            break;
        }
        i--;
    }

    octave = lsb - 12;
    mask = 1 << lsb;
    t0 = 0x1000;
    for (i = 0; i < 48; i++) {
        val1 = mask * t0;
        t0 *= 0x103B;
        t0 >>= 12;
        val2 = mask * t0;
        j = 0;
        base = i << 5;
        acc = 0;
        step = (val2 - val1) >> 5;
        acc_step = step;
        for (; j < 32; j++) {
            u32 p0 = val1 + acc;
            u32 p1 = val1 + acc_step;
            p0 >>= 12;
            p1 >>= 12;
            if (arg2 >= p0 && arg2 < p1) {
                val = base + j;
                goto epilogue;
            }
            acc_step += step;
            acc += step;
        }
    }
    val = 0x600;

epilogue:
    s = val / 128;
    f = val % 128;
    note = arg0 + s + octave * 12;
    fine = arg1 + f;
    return (note << 8) | fine;
}

