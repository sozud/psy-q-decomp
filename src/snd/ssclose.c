#if VERSION >= 36
#include "libsnd_i.h"

static void _SsClose(s16 seq_sep_num) {
    s32 seq_num;
    _SsVmSetSeqVol(seq_sep_num, 0, 0, 1);
    _SsVmSeqKeyOff(seq_sep_num);
    _snd_openflag &= ~(1 << seq_sep_num);
    for (seq_num = 0; seq_num < _snd_seq_t_max; seq_num++) {
        _ss_score[seq_sep_num][seq_num].flags = 0;
        _ss_score[seq_sep_num][seq_num].unk22 = 0xFF;
        _ss_score[seq_sep_num][seq_num].unk23 = 0;
        _ss_score[seq_sep_num][seq_num].vol_l = 0;
        _ss_score[seq_sep_num][seq_num].vol_r = 0;
        _ss_score[seq_sep_num][seq_num].v_time_l = 0;
        _ss_score[seq_sep_num][seq_num].v_time_r = 0;
        _ss_score[seq_sep_num][seq_num].unk4C = 0;
        _ss_score[seq_sep_num][seq_num].unkAC = 0;
        _ss_score[seq_sep_num][seq_num].unkA8 = 0;
        _ss_score[seq_sep_num][seq_num].unkA4 = 0;
        _ss_score[seq_sep_num][seq_num].unk4E = 0;
        _ss_score[seq_sep_num][seq_num].voll = 127;
        _ss_score[seq_sep_num][seq_num].volr = 127;
    }
}

void SsSeqClose(short seq_access_num) { _SsClose(seq_access_num); }

void SsSepClose(short sep_access_num) { _SsClose(sep_access_num); }
#elif VERSION <= 30
#include "libsnd_i.h"

void SsSeqClose(short seq_access_num) {
    s32 seq_num;
    SpuVmSetSeqVol(seq_access_num, 0, 0, 1);
    SpuVmSeqKeyOff(seq_access_num);
    _snd_openflag &= ~(1 << seq_access_num);
    for (seq_num = 0; seq_num < _snd_seq_t_max; seq_num++) {
        _ss_score[seq_access_num][seq_num].unk90 = 0;
        _ss_score[seq_access_num][seq_num].unk3C = 0xFF;
        _ss_score[seq_access_num][seq_num].unk3E = 0;
        _ss_score[seq_access_num][seq_num].unk40 = 0;
        _ss_score[seq_access_num][seq_num].unk94 = 0;
        _ss_score[seq_access_num][seq_num].unk98 = 0;
        _ss_score[seq_access_num][seq_num].unk42 = 0;
        _ss_score[seq_access_num][seq_num].unkA4 = 0;
        _ss_score[seq_access_num][seq_num].unkA0 = 0;
        _ss_score[seq_access_num][seq_num].unk9C = 0;
        _ss_score[seq_access_num][seq_num].unk44 = 0;
        _ss_score[seq_access_num][seq_num].unk74 = 0x7f;
        _ss_score[seq_access_num][seq_num].unk76 = 0x7f;
    }
}

void SsSepClose(short sep_access_num) {
    s32 seq_num;
    SpuVmSetSeqVol(sep_access_num, 0, 0, 1);
    SpuVmSeqKeyOff(sep_access_num);
    _snd_openflag &= ~(1 << sep_access_num);
    for (seq_num = 0; seq_num < _snd_seq_t_max; seq_num++) {
        _ss_score[sep_access_num][seq_num].unk90 = 0;
        _ss_score[sep_access_num][seq_num].unk3C = 0xFF;
        _ss_score[sep_access_num][seq_num].unk3E = 0;
        _ss_score[sep_access_num][seq_num].unk40 = 0;
        _ss_score[sep_access_num][seq_num].unk94 = 0;
        _ss_score[sep_access_num][seq_num].unk98 = 0;
        _ss_score[sep_access_num][seq_num].unk42 = 0;
        _ss_score[sep_access_num][seq_num].unkA4 = 0;
        _ss_score[sep_access_num][seq_num].unkA0 = 0;
        _ss_score[sep_access_num][seq_num].unk9C = 0;
        _ss_score[sep_access_num][seq_num].unk44 = 0;
        _ss_score[sep_access_num][seq_num].unk74 = 0x7f;
        _ss_score[sep_access_num][seq_num].unk76 = 0x7f;
    }
}
#else
#include "libsnd_i.h"

#if VERSION == 33
void seq_close(s16 seq_sep_num) {
#else
static void _SsClose(s16 seq_sep_num) {
#endif
    s32 seq_num;
    SpuVmSetSeqVol(seq_sep_num, 0, 0, 1);
    SpuVmSeqKeyOff(seq_sep_num);
    _snd_openflag &= ~(1 << seq_sep_num);
    for (seq_num = 0; seq_num < _snd_seq_t_max; seq_num++) {
        _ss_score[seq_sep_num][seq_num].unk90 = 0;
        _ss_score[seq_sep_num][seq_num].unk3C = 0xFF;
        _ss_score[seq_sep_num][seq_num].unk0 = 0;
        _ss_score[seq_sep_num][seq_num].unk3E = 0;
        _ss_score[seq_sep_num][seq_num].unk40 = 0;
        _ss_score[seq_sep_num][seq_num].unk94 = 0;
        _ss_score[seq_sep_num][seq_num].unk98 = 0;
        _ss_score[seq_sep_num][seq_num].unk42 = 0;
        _ss_score[seq_sep_num][seq_num].unkA4 = 0;
        _ss_score[seq_sep_num][seq_num].unkA0 = 0;
        _ss_score[seq_sep_num][seq_num].unk9C = 0;
        _ss_score[seq_sep_num][seq_num].unk44 = 0;
        _ss_score[seq_sep_num][seq_num].unk74 = 0x7f;
        _ss_score[seq_sep_num][seq_num].unk76 = 0x7f;
    }
}

#if VERSION == 33
void SsSeqClose(short seq_access_num) { seq_close(seq_access_num); }

void SsSepClose(s16 sep_access_num) { seq_close(sep_access_num); }
#else

void SsSeqClose(short seq_access_num) { _SsClose(seq_access_num); }

void SsSepClose(s16 sep_access_num) { _SsClose(sep_access_num); }
#endif
#endif
