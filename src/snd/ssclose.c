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
