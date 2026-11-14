#include "libsnd_i.h"

#if VERSION >= 43
static u16 _svm_ntable[12] = {
    0x8000, 0x879C, 0x8FAC, 0x9837, 0xA145, 0xAADC,
    0xB504, 0xBFC8, 0xCB2F, 0xD744, 0xE411, 0xF1A1,
};
static u16 _svm_ftable[128] = {
    0x8000, 0x800E, 0x801D, 0x802C, 0x803B, 0x804A, 0x8058, 0x8067,
    0x8076, 0x8085, 0x8094, 0x80A3, 0x80B1, 0x80C0, 0x80CF, 0x80DE,
    0x80ED, 0x80FC, 0x810B, 0x811A, 0x8129, 0x8138, 0x8146, 0x8155,
    0x8164, 0x8173, 0x8182, 0x8191, 0x81A0, 0x81AF, 0x81BE, 0x81CD,
    0x81DC, 0x81EB, 0x81FA, 0x8209, 0x8218, 0x8227, 0x8236, 0x8245,
    0x8254, 0x8263, 0x8272, 0x8282, 0x8291, 0x82A0, 0x82AF, 0x82BE,
    0x82CD, 0x82DC, 0x82EB, 0x82FA, 0x830A, 0x8319, 0x8328, 0x8337,
    0x8346, 0x8355, 0x8364, 0x8374, 0x8383, 0x8392, 0x83A1, 0x83B0,
    0x83C0, 0x83CF, 0x83DE, 0x83ED, 0x83FD, 0x840C, 0x841B, 0x842A,
    0x843A, 0x8449, 0x8458, 0x8468, 0x8477, 0x8486, 0x8495, 0x84A5,
    0x84B4, 0x84C3, 0x84D3, 0x84E2, 0x84F1, 0x8501, 0x8510, 0x8520,
    0x852F, 0x853E, 0x854E, 0x855D, 0x856D, 0x857C, 0x858B, 0x859B,
    0x85AA, 0x85BA, 0x85C9, 0x85D9, 0x85E8, 0x85F8, 0x8607, 0x8617,
    0x8626, 0x8636, 0x8645, 0x8655, 0x8664, 0x8674, 0x8683, 0x8693,
    0x86A2, 0x86B2, 0x86C1, 0x86D1, 0x86E0, 0x86F0, 0x8700, 0x870F,
    0x871F, 0x872E, 0x873E, 0x874E, 0x875D, 0x876D, 0x877D, 0x878C,
};

unsigned short SsPitchFromNote(short note, short fine, unsigned char center, unsigned char shift);

unsigned short note2pitch(void) {
    return SsPitchFromNote(_svm_cur.note, 0, _svm_cur.tone_center, _svm_cur.tone_shift);
}

unsigned short note2pitch2(unsigned short note, unsigned short fine) {
    short i = _svm_cur.fake_program * 16 + _svm_cur.tone;

    return SsPitchFromNote(note, fine, _svm_tn[i].center, _svm_tn[i].shift);
}

unsigned short SsPitchFromNote(short note, short fine, unsigned char center, unsigned char shift) {
    u_long pitch;
    int s;
    int q;
    short f;
    short n;
    short octave;
    short semi;

    s = (short)(shift + fine);
    q = s / 128;
    note += q;
    note -= center;
    n = note;
    f = s - q * 128;
    if (f < 0) {
        f += 128;
        note--;
        n = note + f / 128;
    }
    octave = n / 12 - 2;
    semi = n % 12;
    if (semi < 0) {
        semi += 12;
        octave = n / 12 - 3;
    }
    pitch = (_svm_ntable[semi] * _svm_ftable[f]) >> 16;
    if (octave >= 0) {
        pitch = 0x3FFF;
    } else {
        pitch += 1 << (-octave - 1);
        pitch >>= -octave;
    }
    return pitch;
}
#else

static unsigned short _svm_ptable[] = {
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

#if VERSION >= 41
unsigned short note2pitch(void) {
    short note;
    unsigned short step;
    short octave;
    short pos;
    short shift;
    unsigned short pitch;

    note = _svm_cur.note - (short)(_svm_cur.tone_center - 60);
    step = _svm_cur.tone_shift / 8;
    octave = note / 12;
    pos = note % 12;
    if (step >= 16) {
        step = 15;
    }
    pitch = _svm_ptable[(pos << 4) + step];
    shift = octave - 5;
    if (shift > 0) {
        pitch <<= shift;
    } else if (shift < 0) {
        pitch >>= -shift;
    }
    return pitch;
}

unsigned short note2pitch2(unsigned short note, unsigned short fine) {
    short tone;
    short step;
    short octaveBase;
    short semitones;
    short octave;
    short shift;
    unsigned short pitch;

    tone = _svm_cur.fake_program * 16 + _svm_cur.tone;
    step = (fine + _svm_tn[tone].shift) / 8;
    octaveBase = 0;
    if (step >= 16) {
        octaveBase = 1;
        step -= 16;
    }
    semitones = octaveBase + (note + 60 - _svm_tn[tone].center);
    octave = semitones / 12;
    semitones = semitones % 12;
    pitch = _svm_ptable[(semitones << 4) + step];
    shift = octave - 5;
    if (shift > 0) {
        pitch <<= shift;
    } else if (shift < 0) {
        pitch >>= -shift;
    }
    return pitch;
}

unsigned short SsPitchFromNote(short note, short fine, unsigned char center,
                               unsigned char shift_) {
    short step;
    short octaveBase;
    short semitones;
    short octave;
    short shift;
    unsigned short pitch;

    step = (fine + shift_) / 8;
    octaveBase = 0;
    if (step >= 16) {
        octaveBase = 1;
        step -= 16;
    }
    semitones = octaveBase + (note - (center - 60));
    octave = semitones / 12;
    semitones = semitones % 12;
    pitch = _svm_ptable[(semitones << 4) + step];
    shift = octave - 5;
    if (shift > 0) {
        pitch <<= shift;
    } else if (shift < 0) {
        pitch >>= -shift;
    }
    return pitch;
}
#else
#if VERSION == 36
unsigned short note2pitch(void) {
    short note;
    short octave;
    short pos;
    unsigned short step;
    unsigned short pitch;

    note = (_svm_cur.note + 60) - _svm_cur.tone_center;
    octave = note / 12;
    pos = note % 12;

    step = _svm_cur.tone_shift / 8;
    if (step > 15) {
        step = 15;
    }
    pitch = _svm_ptable[(pos << 4) + step];
    octave -= 5;
    if (octave > 0) {
        pitch <<= octave;
    } else if (octave < 0) {
        pitch >>= -octave;
    }
    return pitch;
}
#else
unsigned short note2pitch(void) {
    int octave;
    int note;
    int shiftVal;
    int semitones;
    short semitone;
    unsigned int tableIndex;
    unsigned short step;
    short shift;
    unsigned short pitch;
    int noteIndex;

    note = _svm_cur.note + (60 - _svm_cur.tone_center);
    shiftVal = _svm_cur.tone_shift;
    step = shiftVal / 8;
    semitones = (short)note;
    octave = semitones / 12;
    semitone = semitones - (octave * 12);
    if (step >= 16) {
        step = 15;
    }
    noteIndex = semitone * 16;
    tableIndex = noteIndex + step;
    pitch = _svm_ptable[tableIndex];
    shift = octave - 5;
    if (shift > 0) {
        pitch <<= shift;
    } else if (shift < 0) {
        pitch >>= -shift;
    }
    return pitch;
}

#endif

unsigned short note2pitch2(unsigned short note, unsigned short fine) {
    VagAtr* tn;
    int octaveBase;
    int octave;
    short step;
    short shift;
    int semitones;
    int noteIndex;
    unsigned short pitch;
    int tone;

    tone = _svm_cur.tone + (_svm_cur.fake_program * 16);
    step = (fine + _svm_tn[tone].shift) / 8;
    octaveBase = 0;
    if (step >= 16) {
        octaveBase = 1;
        step -= 16;
    }
    semitones = (short)(octaveBase + (note + 60 - _svm_tn[tone].center));
    octave = semitones / 12;
    semitones = (short)(semitones - octave * 12);
    noteIndex = semitones * 16;
    pitch = _svm_ptable[noteIndex + step];
    shift = octave - 5;
    if (shift > 0) {
        pitch <<= shift;
    } else if (shift < 0) {
        pitch >>= -shift;
    }
    return pitch;
}
#endif
#endif
