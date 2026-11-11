#if VERSION >= 36
#include "libsnd_i.h"

void SsSeqPlay(short seq_access_num, char play_mode, short l_count) {
    Snd_SetPlayMode(seq_access_num, 0, play_mode, l_count);
}

void SsSepPlay(
    short sep_access_num, short seq_num, char play_mode, short l_count) {
    Snd_SetPlayMode(sep_access_num, seq_num, play_mode, l_count);
}
#elif VERSION <= 30
#include "libsnd_i.h"

void SsSeqPlay(short seq_access_num, char play_mode, short l_count) {
    Snd_SetPlayMode(seq_access_num, 0, play_mode, l_count);
}

void SsSepPlay(
    short sep_access_num, short seq_num, char play_mode, short l_count) {
    Snd_SetPlayMode(sep_access_num, seq_num, play_mode, l_count);
}

#if VERSION == 26
void SsPlayBack(short sep_no, short seq_no, short l_count) {
    _ss_score[sep_no][seq_no].unk90 &= ~0x200;
    _ss_score[sep_no][seq_no].unk90 &= ~0x4;
    _ss_score[sep_no][seq_no].unk90 |= 1;
    _ss_score[sep_no][seq_no].unk46 = l_count;
    _ss_score[sep_no][seq_no].unk48 = 0;
    _ss_score[sep_no][seq_no].read_pos = _ss_score[sep_no][seq_no].next_sep_pos;
    SpuVmSetSeqVol(sep_no, _ss_score[sep_no]->unk74, _ss_score[sep_no]->unk76, 0);
}

void Snd_SetPlayMode(short sep_no, short seq_no, u8 play_mode, short l_count) {
    _ss_score[sep_no][seq_no].unk90 &= ~0x200;
    _ss_score[sep_no][seq_no].unk90 &= ~4;
    _ss_score[sep_no][seq_no].unk46 = l_count;

    if (play_mode == 1) {
        _ss_score[sep_no][seq_no].unk90 |= 1;
        _ss_score[sep_no][seq_no].unk48 = 0;
        _ss_score[sep_no][seq_no].unk2b = 1;
        SpuVmSetSeqVol(sep_no, _ss_score[sep_no][seq_no].unk74,
            _ss_score[sep_no][seq_no].unk76, 0);
    } else if (play_mode == 0) {
        _ss_score[sep_no][seq_no].unk90 |= 2;
    }
}
#else
void SsPlayBack(s16 arg0, s16 arg1, s16 arg2) {
    struct SeqStruct* temp_t2;
    temp_t2 = &_ss_score[arg0][arg1];
    _ss_score[arg0][arg1].unk90 &= ~0x200;
    _ss_score[arg0][arg1].unk90 &= ~0x4;
    _ss_score[arg0][arg1].unk90 |= 1;
    temp_t2->unk46 = arg2;
    temp_t2->unk48 = 0;
    temp_t2->read_pos = temp_t2->next_sep_pos;
    SpuVmSetSeqVol((arg1 << 8) | arg0, temp_t2->unk74, temp_t2->unk76, 0);
}

void Snd_SetPlayMode(s16 arg0, s16 arg1, u8 arg2, s16 arg3) {
    struct SeqStruct* temp_t2;
    temp_t2 = &_ss_score[arg0][arg1];
    _ss_score[arg0][arg1].unk90 &= ~0x200;
    _ss_score[arg0][arg1].unk90 &= ~4;
    temp_t2->unk46 = arg3;

    if (arg2 == 1) {
        _ss_score[arg0][arg1].unk90 |= 1;
        temp_t2->unk48 = 0;
        temp_t2->unk2b = 1;
        SpuVmSetSeqVol((arg1 << 8) | arg0, temp_t2->unk74, temp_t2->unk76, 0);
    } else if (arg2 == 0) {
        _ss_score[arg0][arg1].unk90 |= 2;
    }
}
#endif
#else
#include "libsnd_i.h"

void Snd_SetPlayMode(s16 arg0, s16 arg1, u8 arg2, s16 arg3) {
    struct SeqStruct* temp_t2;
    temp_t2 = &_ss_score[arg0][arg1];
    _ss_score[arg0][arg1].unk90 &= ~0x200;
    _ss_score[arg0][arg1].unk90 &= ~4;
    temp_t2->unk46 = arg3;

    if (arg2 == 1) {
        _ss_score[arg0][arg1].unk90 |= 1;
        temp_t2->unk48 = 0;
        temp_t2->unk2b = 1;
        SpuVmSetSeqVol(arg0 | (arg1 << 8), temp_t2->unk74, temp_t2->unk76, 0);
    } else if (arg2 == 0) {
        _ss_score[arg0][arg1].unk90 |= 2;
    }
}

void SsSeqPlay(short seq_access_num, char play_mode, short l_count) {
    Snd_SetPlayMode(seq_access_num, 0, play_mode, l_count);
}

void SsSepPlay(
    short sep_access_num, short seq_num, char play_mode, short l_count) {
    Snd_SetPlayMode(sep_access_num, seq_num, play_mode, l_count);
}

#if VERSION == 33
void SsPlayBack(s16 arg0, s16 arg1, s16 arg2) {
    struct SeqStruct* temp_t2;
    temp_t2 = &_ss_score[arg0][arg1];
    _ss_score[arg0][arg1].unk90 &= ~0x200;
    _ss_score[arg0][arg1].unk90 &= ~0x4;
    _ss_score[arg0][arg1].unk90 |= 1;
    temp_t2->unk46 = arg2;
    temp_t2->unk48 = 0;
    temp_t2->read_pos = temp_t2->next_sep_pos;
    SpuVmSetSeqVol(arg0 | (arg1 << 8), temp_t2->unk74, temp_t2->unk76, 0);
}
#endif
#endif
