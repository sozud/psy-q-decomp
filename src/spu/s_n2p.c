#if VERSION <= 30
#include "common.h"

static s16 D_80033860[] = {
    0x1000, 0x100E, 0x101D, 0x102C, 0x103B, 0x104A, 0x1059, 0x1068, 0x1078,
    0x1087, 0x1096, 0x10A5, 0x10B5, 0x10C4, 0x10D4, 0x10E3, 0x10F3, 0x1103,
    0x1113, 0x1122, 0x1132, 0x1142, 0x1152, 0x1162, 0x1172, 0x1182, 0x1193,
    0x11A3, 0x11B3, 0x11C4, 0x11D4, 0x11E5, 0x11F5, 0x1206, 0x1216, 0x1227,
    0x1238, 0x1249, 0x125A, 0x126B, 0x127C, 0x128D, 0x129E, 0x12AF, 0x12C1,
    0x12D2, 0x12E3, 0x12F5, 0x1306, 0x1318, 0x132A, 0x133C, 0x134D, 0x135F,
    0x1371, 0x1383, 0x1395, 0x13A7, 0x13BA, 0x13CC, 0x13DE, 0x13F1, 0x1403,
    0x1416, 0x1428, 0x143B, 0x144E, 0x1460, 0x1473, 0x1486, 0x1499, 0x14AC,
    0x14BF, 0x14D3, 0x14E6, 0x14F9, 0x150D, 0x1520, 0x1534, 0x1547, 0x155B,
    0x156F, 0x1583, 0x1597, 0x15AB, 0x15BF, 0x15D3, 0x15E7, 0x15FB, 0x1610,
    0x1624, 0x1638, 0x164D, 0x1662, 0x1676, 0x168B, 0x16A0, 0x16B5, 0x16CA,
    0x16DF, 0x16F4, 0x170A, 0x171F, 0x1734, 0x174A, 0x175F, 0x1775, 0x178B,
    0x17A1, 0x17B6, 0x17CC, 0x17E2, 0x17F9, 0x180F, 0x1825, 0x183B, 0x1852,
    0x1868, 0x187F, 0x1896, 0x18AC, 0x18C3, 0x18DA, 0x18F1, 0x1908, 0x191F,
    0x1937, 0x194E, 0x1965, 0x197D, 0x1995, 0x19AC, 0x19C4, 0x19DC, 0x19F4,
    0x1A0C, 0x1A24, 0x1A3C, 0x1A55, 0x1A6D, 0x1A85, 0x1A9E, 0x1AB7, 0x1ACF,
    0x1AE8, 0x1B01, 0x1B1A, 0x1B33, 0x1B4C, 0x1B66, 0x1B7F, 0x1B98, 0x1BB2,
    0x1BCC, 0x1BE5, 0x1BFF, 0x1C19, 0x1C33, 0x1C4D, 0x1C67, 0x1C82, 0x1C9C,
    0x1CB7, 0x1CD1, 0x1CEC, 0x1D07, 0x1D22, 0x1D3D, 0x1D58, 0x1D73, 0x1D8E,
    0x1DA9, 0x1DC5, 0x1DE0, 0x1DFC, 0x1E18, 0x1E34, 0x1E50, 0x1E6C, 0x1E88,
    0x1EA4, 0x1EC1, 0x1EDD, 0x1EFA, 0x1F16, 0x1F33, 0x1F50, 0x1F6D, 0x1F8A,
    0x1FA7, 0x1FC5, 0x1FE2, 0x2000,
};

s32 _spu_note2pitch(s32 center_note, u16 center_note_frac, s32 offset_note,
                    u16 offset_note_frac) {
    s16 octave;
    s16 var_a1;
    s16 var_v1_2;
    u16 sample_rate;
    s32 pos;

    var_a1 = (offset_note_frac + center_note_frac) / 8;
    for (var_v1_2 = 0; var_a1 >= 16; var_v1_2++) {
        var_a1 -= 16;
    }
    if (var_a1 < 0) {
        var_a1 = 0;
    }
    var_v1_2 += offset_note - (center_note - 60);
    pos = ((var_v1_2 % 12) * 16) + var_a1;
    sample_rate = D_80033860[pos];
    octave = (var_v1_2 / 12) - 5;
    if (octave > 0) {
        sample_rate <<= octave;
    } else if (octave < 0) {
        sample_rate >>= -octave;
    }
    return sample_rate;
}

s32 _spu_pitch2note(u16 arg0, u16 arg1, u16 arg2) {
    s32 temp_a0;
    s32 note_high;
    s32 lsb_mask;
    s32 temp_v1;
    s32 lsb;
    s32 note_low;
    s32 i;

    lsb = 0;
    for (i = 0; i < 16; i++) {
        if ((arg2 >> i) & 1) {
            lsb = i;
        }
    }

    lsb_mask = (1 << (lsb + 1)) - (1 << lsb);

    temp_a0 = arg2 - (1 << lsb);

    note_high = arg0 + ((lsb - 12) * 12) + (temp_a0 * 12) / lsb_mask;
    temp_v1 =
        arg1 +
        ((temp_a0 - (lsb_mask * ((temp_a0 * 12) / lsb_mask)) / 12) * 0x600) /
            lsb_mask;

    note_low = temp_v1 % 0x80;
    note_high += (temp_v1 < 0x80) ? 0 : 1;

    if (note_high < 0) {
        return -1;
    }
    if (note_high >= 0x80) {
        return -1;
    }

    return (note_high << 8) | note_low;
}
#else
// SPDX-License-Identifier: MIT
#if VERSION == 40
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
#else
#include "common.h"

static s16 D_80033860[] = {
    0x1000, 0x100E, 0x101D, 0x102C, 0x103B, 0x104A, 0x1059, 0x1068, 0x1078,
    0x1087, 0x1096, 0x10A5, 0x10B5, 0x10C4, 0x10D4, 0x10E3, 0x10F3, 0x1103,
    0x1113, 0x1122, 0x1132, 0x1142, 0x1152, 0x1162, 0x1172, 0x1182, 0x1193,
    0x11A3, 0x11B3, 0x11C4, 0x11D4, 0x11E5, 0x11F5, 0x1206, 0x1216, 0x1227,
    0x1238, 0x1249, 0x125A, 0x126B, 0x127C, 0x128D, 0x129E, 0x12AF, 0x12C1,
    0x12D2, 0x12E3, 0x12F5, 0x1306, 0x1318, 0x132A, 0x133C, 0x134D, 0x135F,
    0x1371, 0x1383, 0x1395, 0x13A7, 0x13BA, 0x13CC, 0x13DE, 0x13F1, 0x1403,
    0x1416, 0x1428, 0x143B, 0x144E, 0x1460, 0x1473, 0x1486, 0x1499, 0x14AC,
    0x14BF, 0x14D3, 0x14E6, 0x14F9, 0x150D, 0x1520, 0x1534, 0x1547, 0x155B,
    0x156F, 0x1583, 0x1597, 0x15AB, 0x15BF, 0x15D3, 0x15E7, 0x15FB, 0x1610,
    0x1624, 0x1638, 0x164D, 0x1662, 0x1676, 0x168B, 0x16A0, 0x16B5, 0x16CA,
    0x16DF, 0x16F4, 0x170A, 0x171F, 0x1734, 0x174A, 0x175F, 0x1775, 0x178B,
    0x17A1, 0x17B6, 0x17CC, 0x17E2, 0x17F9, 0x180F, 0x1825, 0x183B, 0x1852,
    0x1868, 0x187F, 0x1896, 0x18AC, 0x18C3, 0x18DA, 0x18F1, 0x1908, 0x191F,
    0x1937, 0x194E, 0x1965, 0x197D, 0x1995, 0x19AC, 0x19C4, 0x19DC, 0x19F4,
    0x1A0C, 0x1A24, 0x1A3C, 0x1A55, 0x1A6D, 0x1A85, 0x1A9E, 0x1AB7, 0x1ACF,
    0x1AE8, 0x1B01, 0x1B1A, 0x1B33, 0x1B4C, 0x1B66, 0x1B7F, 0x1B98, 0x1BB2,
    0x1BCC, 0x1BE5, 0x1BFF, 0x1C19, 0x1C33, 0x1C4D, 0x1C67, 0x1C82, 0x1C9C,
    0x1CB7, 0x1CD1, 0x1CEC, 0x1D07, 0x1D22, 0x1D3D, 0x1D58, 0x1D73, 0x1D8E,
    0x1DA9, 0x1DC5, 0x1DE0, 0x1DFC, 0x1E18, 0x1E34, 0x1E50, 0x1E6C, 0x1E88,
    0x1EA4, 0x1EC1, 0x1EDD, 0x1EFA, 0x1F16, 0x1F33, 0x1F50, 0x1F6D, 0x1F8A,
    0x1FA7, 0x1FC5, 0x1FE2, 0x2000,
};

s32 _spu_note2pitch(s32 center_note, u16 center_note_frac, s32 offset_note,
                    u16 offset_note_frac) {
    s16 octave;
    s16 var_a1;
    s16 var_v1_2;
    u16 sample_rate;
    s32 pos;

    var_a1 = (offset_note_frac + center_note_frac) / 8;
    for (var_v1_2 = 0; var_a1 >= 16; var_v1_2++) {
        var_a1 -= 16;
    }
    if (var_a1 < 0) {
        var_a1 = 0;
    }
    var_v1_2 += offset_note - (center_note - 60);
    pos = ((var_v1_2 % 12) * 16) + var_a1;
    sample_rate = D_80033860[pos];
    octave = (var_v1_2 / 12) - 5;
    if (octave > 0) {
        sample_rate <<= octave;
    } else if (octave < 0) {
        sample_rate >>= -octave;
    }
    return sample_rate;
}

s32 _spu_pitch2note(u16 arg0, u16 arg1, u16 arg2) {
    s32 temp_a0;
    s32 note_high;
    s32 lsb_mask;
    s32 temp_v1;
    s32 lsb;
    s32 note_low;
    s32 i;

    lsb = 0;
    for (i = 0; i < 16; i++) {
        if ((arg2 >> i) & 1) {
            lsb = i;
        }
    }

    lsb_mask = (1 << (lsb + 1)) - (1 << lsb);

    temp_a0 = arg2 - (1 << lsb);

    temp_v1 =
        arg1 +
        ((temp_a0 - (lsb_mask * ((temp_a0 * 12) / lsb_mask)) / 12) * 0x600) /
            lsb_mask;
    note_high = arg0 + ((lsb - 12) * 12) + (temp_a0 * 12) / lsb_mask;

    note_low = temp_v1 % 0x80;
    note_high += (temp_v1 < 0x80) ? 0 : 1;

    if (note_high < 0) {
        return -1;
    }
    if (note_high >= 0x80) {
        return -1;
    }

    return (note_high << 8) | note_low;
}
#endif
#endif
