#define SsSepOpen header_SsSepOpen
#include "libsnd_i.h"
#undef SsSepOpen

#if VERSION <= 33
#define _SsInitSoundSep InitSoundSep
#define _SsReadDeltaValue ReadDeltaValue
#endif

s32 _SsReadDeltaValue(s16, s16);

#if VERSION >= 36
int _SsInitSoundSep(short sep_no, short seq_no, short vab_id, unsigned long* addr) {
    struct SeqStruct* score;
    s32 channel;
    s32 len;
    u8 b0;
    u8 b1;
    u8 b2;
    u8 b3;
    u8 b4;
    s32 data_len;
    s32 delta_value;
    u32 ticks;

    len = 0;
    score = &_ss_score[sep_no][seq_no];
#if VERSION >= 40
    score->l_count = 1;
#elif VERSION == 36
    score->unk52 = 1;
#endif
    score->unk15 = 0;
    score->running_status = 0;
    score->channel_idx = 0;
    score->unk18 = 0;
    score->unk19 = 0;
    score->fn_idx = 0;
    score->unk1B = 0;
    score->unk1C = 0;
    score->unk1D = 0;
    score->unk1E = 0;
    score->unk1F = 0;
    score->play_mode = 0;
    score->unk21 = 0;
#if VERSION >= 37
    score->unk52 = 1;
#endif
    score->resolution = 0;
    score->vab_id = vab_id;
    score->unk56 = 0;
    score->unk84 = 0;
    score->unk88 = 0;
    score->tempo = 0;
    score->delta_value = 0;
#if VERSION == 36
    score->unkA8_36 = 0x7F;
#endif
    score->channel_mute = 0;
#if VERSION >= 40
    score->rhythm_n = 0;
    score->rhythm_d = 0;
#endif
    for (channel = 0; channel < 16; channel++) {
        score->panpot[channel] = 0x40;
        score->programs[channel] = channel;
        score->vol[channel] = 0x7F;
    }

    score->seq_ptr = (u8*)addr;
    if (seq_no == 0) {
        if ((*score->seq_ptr == 'S') || (*score->seq_ptr == 'p')) {
            score->seq_ptr += 5;
            if (*score->seq_ptr++ != 0) {
                printf("This is not SEP Data.\n");
                return -1;
            }
            score->seq_ptr += 2;
            len += 8;
        }
    } else {
        score->seq_ptr += 2;
        len += 2;
    }
    b1 = *score->seq_ptr++;
    b0 = *score->seq_ptr++;
    score->resolution = b0 | (b1 << 8);
    b2 = *score->seq_ptr++;
    b3 = *score->seq_ptr++;
    b4 = *score->seq_ptr++;
    score->tempo = (b2 << 16) | (b3 << 8) | b4;
    len += 5;
    if ((score->tempo / 2) < (60000000 % score->tempo)) {
        score->tempo = (60000000 / score->tempo) + 1;
    } else {
        score->tempo = 60000000 / score->tempo;
    }
    score->unk94 = score->tempo;
#if VERSION >= 40
    score->rhythm_n = *score->seq_ptr++;
    score->rhythm_d = *score->seq_ptr++;
#else
    score->seq_ptr += 2;
#endif
    b1 = *score->seq_ptr++;
    b0 = *score->seq_ptr++;
    b2 = *score->seq_ptr++;
    b3 = *score->seq_ptr++;
    data_len = (b1 << 24) + (b0 << 16) + (b2 << 8) + b3;
    delta_value = _SsReadDeltaValue(sep_no, seq_no);
    ticks = score->resolution * score->tempo;
    score->unk84 = delta_value;
    score->delta_value = delta_value;
#if VERSION >= 40
    score->unk10 = 0;
    score->read_pos = score->seq_ptr;
    score->next_sep_pos = score->seq_ptr;
    score->loop_pos = score->seq_ptr;
#else
    score->read_pos = score->seq_ptr;
    score->next_sep_pos = score->seq_ptr;
#endif
    len += 6;
    if ((ticks * 10) < (VBLANK_MINUS * 60)) {
        score->unk54 = score->unk52 = (VBLANK_MINUS * 600) / ticks;
    } else {
        score->unk52 = -1;
        score->unk54 = (score->resolution * score->tempo * 10) / (VBLANK_MINUS * 60);
        if ((VBLANK_MINUS * 30) <
            (score->resolution * score->tempo * 10) % (VBLANK_MINUS * 60)) {
            score->unk54++;
        }
    }
    score->unk56 = score->unk54;
    return len + data_len;
}

#elif VERSION == 26
#define SC (&_ss_score[sep_no][seq_no])
s32 _SsInitSoundSep(s16 sep_no, s16 seq_no, s16 vab_id, u8* addr) {
    s32 channel;
    s32 len;
    s32 data_len;
    u32 ticks;
    u8 b0;
    u8 b1;
    s32 b2;
    s32 b3;
    u8* p;

    SC->unk4a = 0;
    SC->unk13 = 0;
    SC->unk14 = 0;
    SC->unk29 = 0;
    SC->unk15 = 0;
    SC->unk16 = 0;
    SC->unk2a = 0;
    SC->channel = 0;
    SC->unk7c = 0;
    SC->unk80 = 0;
    SC->unk84 = 0;
    SC->unk72 = 0;
    SC->unk48 = 0;
    SC->unk2b = 0;
    SC->delta_value = 0;
    SC->unk27 = 0;
    SC->unk28 = 0;
    SC->unk10 = 0;
    SC->unk11 = 0;
    len = 0;
    SC->unk4c = vab_id;
    for (channel = 0; channel < 16; channel++) {
        SC->programs[channel] = channel;
        SC->panpot[channel] = 0x40;
        SC->vol[channel] = 0x7F;
    }
    SC->unk6E = 1;
    SC->unk78 = 0x7F;
    SC->unk7A = 0x7F;
    SC->read_pos = addr;
    if (seq_no == 0) {
        SC->read_pos += 8;
        len += 8;
    } else {
        SC->read_pos += 2;
        len += 2;
    }
    b2 = *SC->read_pos++;
    p = SC->read_pos++;
    SC->unk4a = *p | (b2 << 8);
    b1 = *SC->read_pos++;
    b3 = *SC->read_pos++;
    p = SC->read_pos++;
    SC->unk84 = (b1 << 16) | (b3 << 8) | *p;
    len += 5;
    {
        s32 t = SC->unk84;
        if ((t / 2) < (60000000 % t)) {
            SC->unk84 = (60000000 / t) + 1;
        } else {
            SC->unk84 = 60000000 / t;
        }
    }
    SC->unk8c = SC->unk84;
    SC->read_pos += 2;
    b2 = *SC->read_pos++;
    b3 = *SC->read_pos++;
    b1 = *SC->read_pos++;
    b0 = *SC->read_pos++;
    data_len = (b2 << 24) + (b3 << 16) + (b1 << 8) + b0;
    SC->unk7c = _SsReadDeltaValue(sep_no, seq_no);
    SC->delta_value = SC->unk7c;
    SC->next_sep_pos = SC->read_pos;
    SC->loop_pos = SC->read_pos;
    ticks = SC->unk4a * SC->unk84;
    len += 6;
    if ((ticks * 10) < (VBLANK_MINUS * 60)) {
        SC->unk6E = (VBLANK_MINUS * 600) / ticks;
        SC->unk70 = SC->unk6E;
    } else {
        SC->unk6E = -1;
        SC->unk70 = (SC->unk4a * SC->unk84 * 10) / (VBLANK_MINUS * 60);
        if ((VBLANK_MINUS * 30) < (SC->unk4a * SC->unk84 * 10) % (VBLANK_MINUS * 60)) {
            SC->unk70++;
        }
    }
    SC->unk72 = SC->unk70;
    return len + data_len;
}
#undef SC

#elif VERSION <= 30
s32 _SsInitSoundSep(s16 sep_no, s16 seq_no, s16 vab_id, u8* addr) {
    struct SeqStruct* score;
    s32 channel;
    s32 len;
    u8 b0;
    u8 b1;
    s32 b2;
    s32 b3;
    s32 data_len;
    s32 delta_value;
    u32 ticks;

    score = &_ss_score[sep_no][seq_no];
    len = 0;
    score->unk4a = 0;
    score->unk13 = 0;
    score->unk14 = 0;
    score->unk29 = 0;
    score->unk15 = 0;
    score->unk16 = 0;
    score->unk2a = 0;
    score->channel = 0;
    score->unk7c = 0;
    score->unk80 = 0;
    score->unk84 = 0;
    score->unk72 = 0;
    score->unk48 = 0;
    score->unk2b = 0;
    score->delta_value = 0;
    score->unk27 = 0;
    score->unk28 = 0;
    score->unk10 = 0;
    score->unk11 = 0;
    score->unk4c = vab_id;
    for (channel = 0; channel < 16; channel++) {
        score->programs[channel] = channel;
        score->panpot[channel] = 0x40;
        score->vol[channel] = 0x7F;
    }
    score->unk6E = 1;
    score->unk78 = 0x7F;
    score->unk7A = 0x7F;

    score->read_pos = addr;
    if (seq_no == 0) {
        score->read_pos += 8;
        len += 8;
    } else {
        score->read_pos += 2;
        len += 2;
    }
    b2 = *score->read_pos++;
    b0 = *score->read_pos++;
    score->unk4a = b0 | (b2 << 8);
    b1 = *score->read_pos++;
    b3 = *score->read_pos++;
    score->unk84 = (b1 << 16) | (b3 << 8) | *score->read_pos++;
    len += 5;
    if ((score->unk84 / 2) < (60000000 % score->unk84)) {
        score->unk84 = (60000000 / score->unk84) + 1;
    } else {
        score->unk84 = 60000000 / score->unk84;
    }
    score->unk8c = score->unk84;
    score->read_pos += 2;
    b2 = *score->read_pos++;
    b3 = *score->read_pos++;
    b1 = *score->read_pos++;
    b0 = *score->read_pos++;
    data_len = (b2 << 24) + (b3 << 16) + (b1 << 8) + b0;
    delta_value = _SsReadDeltaValue(sep_no, seq_no);
    ticks = score->unk4a * score->unk84;
    score->unk7c = delta_value;
    score->delta_value = delta_value;
    score->next_sep_pos = score->read_pos;
    score->loop_pos = score->read_pos;
    len += 6;
    if ((ticks * 10) < (VBLANK_MINUS * 60)) {
        score->unk70 = score->unk6E = (VBLANK_MINUS * 600) / ticks;
    } else {
        score->unk6E = -1;
        score->unk70 = (score->unk4a * score->unk84 * 10) / (VBLANK_MINUS * 60);
        if ((VBLANK_MINUS * 30) <
            (score->unk4a * score->unk84 * 10) % (VBLANK_MINUS * 60)) {
            score->unk70++;
        }
    }
    score->unk72 = score->unk70;
    return len + data_len;
}

#else
s32 _SsInitSoundSep(s16 sep_no, s16 seq_no, s16 vab_id, u8* addr) {
    struct SeqStruct* score;
    s32 channel;
    s32 len;
    u8 b0;
    u8 b1;
    u8 b2;
    u8 b3;
    u8 b4;
    s32 data_len;
    s32 delta_value;
    u32 ticks;

    len = 0;
    score = &_ss_score[sep_no][seq_no];
    score->unk6E = 1;
    score->unk10 = 0;
    score->unk11 = 0;
    score->channel = 0;
    score->unk13 = 0;
    score->unk14 = 0;
    score->unk15 = 0;
    score->unk16 = 0;
    score->unk27 = 0;
    score->unk28 = 0;
    score->unk29 = 0;
    score->unk2a = 0;
    score->unk2b = 0;
    score->unk48 = 0;
    score->unk4a = 0;
    score->unk4c = vab_id;
    score->unk72 = 0;
    score->unk7c = 0;
    score->unk80 = 0;
    score->unk84 = 0;
    score->delta_value = 0;
    score->padA6 = 0x7F;
#if VERSION != 33
    score->padaa = 0;
#endif
    for (channel = 0; channel < 16; channel++) {
        score->panpot[channel] = 0x40;
        score->programs[channel] = channel;
        score->vol[channel] = 0x7F;
    }

    score->read_pos = addr;
    if (seq_no == 0) {
        if ((*score->read_pos == 'S') || (*score->read_pos == 'p')) {
            score->read_pos += 5;
            if (*score->read_pos++ != 0) {
                printf("This is not SEP Data.\n");
                return -1;
            }
            score->read_pos += 2;
            len += 8;
        }
    } else {
        score->read_pos += 2;
        len += 2;
    }
    b1 = *score->read_pos++;
    b0 = *score->read_pos++;
    score->unk4a = b0 | (b1 << 8);
    b2 = *score->read_pos++;
    b3 = *score->read_pos++;
    b4 = *score->read_pos++;
    score->unk84 = (b2 << 16) | (b3 << 8) | b4;
    len += 5;
    if ((score->unk84 / 2) < (60000000 % score->unk84)) {
        score->unk84 = (60000000 / score->unk84) + 1;
    } else {
        score->unk84 = 60000000 / score->unk84;
    }
    score->unk8c = score->unk84;
    score->read_pos += 2;
    b1 = *score->read_pos++;
    b0 = *score->read_pos++;
    b2 = *score->read_pos++;
    b3 = *score->read_pos++;
    data_len = (b1 << 24) + (b0 << 16) + (b2 << 8) + b3;
    delta_value = _SsReadDeltaValue(sep_no, seq_no);
    ticks = score->unk4a * score->unk84;
    score->unk7c = delta_value;
    score->delta_value = delta_value;
    score->next_sep_pos = score->read_pos;
    score->loop_pos = score->read_pos;
    len += 6;
    if ((ticks * 10) < (VBLANK_MINUS * 60)) {
        score->unk70 = score->unk6E = (VBLANK_MINUS * 600) / ticks;
    } else {
        score->unk6E = -1;
        score->unk70 = (score->unk4a * score->unk84 * 10) / (VBLANK_MINUS * 60);
        if ((VBLANK_MINUS * 30) <
            (score->unk4a * score->unk84 * 10) % (VBLANK_MINUS * 60)) {
            score->unk70++;
        }
    }
    score->unk72 = score->unk70;
    return len + data_len;
}

#endif

#if VERSION <= 35 && VERSION > 30
s16 SsSepOpen(unsigned long* addr, long vab_id, s16 seq_cnt) {
    s16 i;
    s16 seq_sep_no;
    u32 bit_pos;
    u8 found;
    s32 len;
    u8* p = (u8*)addr;

    if (_snd_openflag == -1) {
        printf("Can't Open Sequence data any more\n\n");
        return -1;
    }
    bit_pos = 0;
    found = 0;
    while (!found) {
        if (!((1 << bit_pos) & _snd_openflag)) {
            seq_sep_no = bit_pos;
            found = 1;
        }
        bit_pos++;
    }
    _snd_openflag |= 1 << seq_sep_no;
    for (i = 0; i < seq_cnt; i++) {
        len = _SsInitSoundSep(seq_sep_no, i, vab_id, p);
        if (len == -1) {
            return -1;
        }
        p += len;
    }
    return seq_sep_no;
}
#endif
