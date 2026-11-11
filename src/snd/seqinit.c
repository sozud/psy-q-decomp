#if VERSION == 40
#include "libsnd_i.h"

short _SsInitSoundSeq(short seq_no, short vab_id, u8* addr) {
    struct SeqStruct* score = _ss_score[seq_no];
    int channel;
    u8 hi;
    u8 mid;

    score->vab_id = vab_id;
    score->resolution = 0;
    score->unk18 = 0;
    score->unk19 = 0;
    score->unk1E = 0;
    score->fn_idx = 0;
    score->unk1B = 0;
    score->unk1F = 0;
    score->channel_idx = 0;
    score->unk84 = 0;
    score->unk88 = 0;
    score->tempo = 0;
    score->unk56 = 0;
    score->unk21 = 0;
    score->l_count = 1;
    score->play_mode = 0;
    score->delta_value = 0;
    score->unk1C = 0;
    score->unk1D = 0;
    score->unk15 = 0;
    score->running_status = 0;
    score->channel_mute = 0;
    score->rhythm_n = 0;
    score->rhythm_d = 0;
    for (channel = 0; channel < 16; channel++) {
        score->programs[channel] = channel;
        score->panpot[channel] = 0x40;
        score->vol[channel] = 0x7F;
    }
    score->unk52 = 1;
    score->seq_ptr = addr;
    if (*score->seq_ptr == 'S' || *score->seq_ptr == 'p') {
        score->seq_ptr += 8;
        if (addr[7] != 1) {
            printf("This is not SEQ Data.\n");
            return -1;
        }
    } else {
        printf("This is an old SEQ Data Format.\n");
        return 0;
    }
    hi = *score->seq_ptr++;
    score->resolution = (hi << 8) | *score->seq_ptr++;
    hi = *score->seq_ptr++;
    mid = *score->seq_ptr++;
    score->tempo = (hi << 16) | (mid << 8) | *score->seq_ptr++;
    if (score->tempo / 2 < 60000000 % score->tempo) {
        score->tempo = 60000000 / score->tempo + 1;
    } else {
        score->tempo = 60000000 / score->tempo;
    }
    score->unk94 = score->tempo;
    score->rhythm_n = *score->seq_ptr++;
    score->rhythm_d = *score->seq_ptr++;
    score->delta_value = score->unk84 = _SsReadDeltaValue(seq_no, 0);
    score->unk10 = 0;
    score->read_pos = score->seq_ptr;
    score->next_sep_pos = score->seq_ptr;
    score->loop_pos = score->seq_ptr;
    if (score->resolution * score->tempo * 10 < VBLANK_MINUS * 60) {
        score->unk54 = score->unk52 =
            VBLANK_MINUS * 600 / (score->resolution * score->tempo);
    } else {
        score->unk52 = -1;
        score->unk54 =
            score->resolution * score->tempo * 10 / (VBLANK_MINUS * 60);
        if (VBLANK_MINUS * 30 <
            score->resolution * score->tempo * 10 % (VBLANK_MINUS * 60)) {
            score->unk54++;
        }
    }
    score->unk56 = score->unk54;
    return 0;
}
#elif VERSION >= 36
#include "libsnd_i.h"

short _SsInitSoundSeq(short seq_no, short vab_id, u8* addr) {
    struct SeqStruct* score = &_ss_score[seq_no][0];
    int channel;

    score->vab_id = vab_id;
    score->resolution = 0;
    score->unk18 = 0;
    score->unk19 = 0;
    score->unk1E = 0;
    score->fn_idx = 0;
    score->unk1B = 0;
    score->unk1F = 0;
    score->channel_idx = 0;
    score->unk84 = 0;
    score->unk88 = 0;
    score->tempo = 0;
    score->unk56 = 0;
    score->unk21 = 0;
    score->play_mode = 0;
    score->delta_value = 0;
    score->unk1C = 0;
    score->unk1D = 0;
    score->unk15 = 0;
    score->running_status = 0;
#if VERSION == 36
    score->unkA8_36 = 0x7F;
#endif
    score->channel_mute = 0;
    for (channel = 0; channel < 16; channel++) {
        score->programs[channel] = channel;
        score->panpot[channel] = 0x40;
        score->vol[channel] = 0x7F;
    }
    score->unk52 = 1;
    score->seq_ptr = addr;
    if (*score->seq_ptr == 'S' || *score->seq_ptr == 'p') {
        score->seq_ptr += 8;
        if (addr[7] != 1) {
            printf("This is not SEQ Data.\n");
            return -1;
        }
    } else {
        printf("This is an old SEQ Data Format.\n");
        return 0;
    }
    score->resolution = (*score->seq_ptr++) << 8 | *score->seq_ptr++;
    score->tempo = (*score->seq_ptr++) << 16 | (*score->seq_ptr++) << 8 |
                   *score->seq_ptr++;
    if (score->tempo / 2 < 60000000 % score->tempo) {
        score->tempo = 60000000 / score->tempo + 1;
    } else {
        score->tempo = 60000000 / score->tempo;
    }
    score->unk94 = score->tempo;
    score->seq_ptr += 2;
    score->delta_value = score->unk84 = _SsReadDeltaValue(seq_no, 0);
    score->read_pos = score->seq_ptr;
    score->next_sep_pos = score->seq_ptr;

    if (score->resolution * (u32)score->tempo * 10 < VBLANK_MINUS * 60) {
        score->unk54 = score->unk52 =
            VBLANK_MINUS * 600 / (score->resolution * (u32)score->tempo);
    } else {
        score->unk52 = -1;
        score->unk54 =
            score->resolution * (u32)score->tempo * 10 / (VBLANK_MINUS * 60);
        if (VBLANK_MINUS * 30 <
            score->resolution * (u32)score->tempo * 10 % (VBLANK_MINUS * 60)) {
            score->unk54++;
        }
    }
    score->unk56 = score->unk54;
    return 0;
}
#else
#include "libsnd_i.h"

#if VERSION <= 33
#define _SsInitSoundSeq InitSoundSeq
#define _SsReadDeltaValue ReadDeltaValue
#endif

#if VERSION == 26
#define SC _ss_score[arg0]
void InitSoundSeq(s16 arg0, s16 vab_id, u8* addr) {
    s32 channel;
    s32 tmp;
    u16 tmp2;

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
    SC->unk4c = vab_id;
    for (channel = 0; channel < 16; channel++) {
        SC->programs[channel] = channel;
        SC->panpot[channel] = 0x40;
        SC->vol[channel] = 0x7f;
    }
    SC->unk6E = 1;
    SC->unk78 = 0x7F;
    SC->unk7A = 0x7F;
    SC->read_pos = addr;
    if ((*SC->read_pos == 'S') || (*SC->read_pos == 'p')) {
        SC->read_pos += 7;
        if (*SC->read_pos++ != 1) {
            printf("This is not SEQ Data.\n");
            return;
        }
    } else {
        printf("This is an old SEQ Data Format.\n");
    }
    tmp2 = (*SC->read_pos++) << 8 | *SC->read_pos++;
    SC->unk4a = tmp2;
    tmp = (*SC->read_pos++) << 0x10 | (*SC->read_pos++) << 8 |
          *SC->read_pos++;
    SC->unk84 = tmp;
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
    SC->unk7c = ReadDeltaValue(arg0, 0);
    SC->delta_value = SC->unk7c;
    SC->next_sep_pos = SC->read_pos;
    SC->loop_pos = SC->read_pos;
    if ((SC->unk4a * (u32)SC->unk84 * 10) < (VBLANK_MINUS * 60)) {
        SC->unk6E = (VBLANK_MINUS * 600) / (SC->unk4a * (u32)SC->unk84);
        SC->unk70 = SC->unk6E;
    } else {
        SC->unk6E = -1;
        SC->unk70 = (SC->unk4a * (u32)SC->unk84 * 10) / (VBLANK_MINUS * 60);
        if ((VBLANK_MINUS * 30) <
            (SC->unk4a * (u32)SC->unk84 * 10) % (VBLANK_MINUS * 60)) {
            SC->unk70++;
        }
    }
    SC->unk72 = SC->unk70;
}
#undef SC
#else
#if VERSION <= 30
void _SsInitSoundSeq(s16 arg0, s16 vab_id, u8* addr) {
#else
s16 _SsInitSoundSeq(s16 arg0, s16 vab_id, u8* addr) {
#endif
    struct SeqStruct* score = &_ss_score[arg0][0];
    s32 channel;

#if VERSION > 30
    score->unk4c = vab_id;
#endif
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
    score->unk72 = 0U;
    score->unk48 = 0;
    score->unk2b = 0;
    score->delta_value = 0;
    score->unk27 = 0;
    score->unk28 = 0;
    score->unk10 = 0;
    score->unk11 = 0;
#if VERSION <= 30
    score->unk4c = vab_id;
#else
    score->padA6 = 0x7F;
#endif
#if VERSION > 33
    score->padaa = 0;
#endif
    for (channel = 0; channel < 16; channel++) {
        score->programs[channel] = channel;
        score->panpot[channel] = 0x40;
        score->vol[channel] = 0x7f;
    }
    score->unk6E = 1;
#if VERSION <= 30
    score->unk78 = 0x7F;
    score->unk7A = 0x7F;
#endif
    score->read_pos = addr;
    if ((*score->read_pos == 'S') || (*score->read_pos == 'p')) {
        score->read_pos += 8;
        if (addr[7] != 1) {
            printf("This is not SEQ Data.\n");
#if VERSION <= 30
            return;
#else
            return -1;
#endif
        }
    } else {
        printf("This is an old SEQ Data Format.\n");
#if VERSION > 30
        return 0;
#endif
    }
    score->unk4a = (*score->read_pos++) << 8 | *score->read_pos++;
    score->unk84 = (*score->read_pos++) << 0x10 | (*score->read_pos++) << 8 |
                   *score->read_pos++;
    if ((score->unk84 / 2) < (60000000 % score->unk84)) {
        score->unk84 = (60000000 / score->unk84) + 1;
    } else {
        score->unk84 = 60000000 / score->unk84;
    }
    score->unk8c = score->unk84;
    score->read_pos += 2;
    score->delta_value = score->unk7c = _SsReadDeltaValue(arg0, 0);
    score->next_sep_pos = score->read_pos;
    score->loop_pos = score->read_pos;

    if ((score->unk4a * (u32)score->unk84 * 10) <
        (VBLANK_MINUS * 60)) {
        score->unk70 = score->unk6E =
            (VBLANK_MINUS * 600) / (score->unk4a * (u32)score->unk84);
    } else {
        score->unk6E = -1;
        score->unk70 = (score->unk4a * (u32)score->unk84 * 10) /
                       (VBLANK_MINUS * 60);
        if ((VBLANK_MINUS * 30) <
            (score->unk4a * (u32)score->unk84 * 10) %
                (VBLANK_MINUS * 60)) {
            score->unk70++;
        }
    }
    score->unk72 = score->unk70;
#if VERSION > 30
    return 0;
#endif
}

#endif

#if VERSION > 30
s16 SsSeqOpen(unsigned long* addr, s16 vab_id) {
    s16 seq_sep_no;
    u32 bit_pos;
    u8 found;

    if (_snd_openflag == 0xFFFFFFFF) {
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
    if (_SsInitSoundSeq(seq_sep_no, vab_id, (u8*)addr) == -1) {
        return -1;
    }
    return seq_sep_no;
}
#endif
#endif
