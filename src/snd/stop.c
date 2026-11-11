#include "libsnd_i.h"

#if VERSION <= 30
void Snd_stop(short sep_no, short seq_no);

void SsSeqStop(short seq_no) {
    _ss_score[seq_no]->unk90 &= ~1;
    _ss_score[seq_no]->unk90 &= ~2;
    _ss_score[seq_no]->unk90 &= ~8;
    _ss_score[seq_no]->unk90 |= 4;
    Snd_stop(seq_no, 0);
}

void SsSepStop(short sep_no, short seq_no) {
    _ss_score[sep_no][seq_no].unk90 &= ~1;
    _ss_score[sep_no][seq_no].unk90 &= ~2;
    _ss_score[sep_no][seq_no].unk90 &= ~8;
    _ss_score[sep_no][seq_no].unk90 |= 4;
    Snd_stop(sep_no, seq_no);
}

#if VERSION == 26
void Snd_stop(short sep_no, short seq_no) {
    int i;

    SpuVmSeqKeyOff(seq_no << 8 | sep_no);
    _ss_score[sep_no][seq_no].unk2b = 0;
    _ss_score[sep_no][seq_no].unk80 = 0;
    _ss_score[sep_no][seq_no].delta_value = _ss_score[sep_no][seq_no].unk7c;
    _ss_score[sep_no][seq_no].unk8c = _ss_score[sep_no][seq_no].unk84;
    _ss_score[sep_no][seq_no].unk70 = _ss_score[sep_no][seq_no].unk72;
    _ss_score[sep_no][seq_no].read_pos = _ss_score[sep_no][seq_no].next_sep_pos;
    _ss_score[sep_no][seq_no].loop_pos = _ss_score[sep_no][seq_no].next_sep_pos;
    _ss_score[sep_no][seq_no].unk27 = 0;
    _ss_score[sep_no][seq_no].unk13 = 0;
    _ss_score[sep_no][seq_no].unk14 = 0;
    _ss_score[sep_no][seq_no].unk29 = 0;
    _ss_score[sep_no][seq_no].unk15 = 0;
    _ss_score[sep_no][seq_no].unk16 = 0;
    _ss_score[sep_no][seq_no].unk2a = 0;
    _ss_score[sep_no][seq_no].channel = 0;
    _ss_score[sep_no][seq_no].unk48 = 0;
    _ss_score[sep_no][seq_no].unk27 = 0;
    _ss_score[sep_no][seq_no].unk28 = 0;
    _ss_score[sep_no][seq_no].unk10 = 0;
    _ss_score[sep_no][seq_no].unk11 = 0;
    for (i = 0; i < 16; i++) {
        _ss_score[sep_no][seq_no].programs[i] = i;
        _ss_score[sep_no][seq_no].panpot[i] = 0x40;
        _ss_score[sep_no][seq_no].vol[i] = 127;
    }
    _ss_score[sep_no][seq_no].unk78 = 0x7F;
    _ss_score[sep_no][seq_no].unk7A = 0x7F;
}
#else
void Snd_stop(short sep_no, short seq_no) {
    int i;
    struct SeqStruct* score;

    score = &_ss_score[sep_no][seq_no];
    SpuVmSeqKeyOff(seq_no << 8 | sep_no);
    score->unk2b = 0;
    score->unk80 = 0;
    score->unk27 = 0;
    score->unk13 = 0;
    score->unk14 = 0;
    score->unk29 = 0;
    score->unk15 = 0;
    score->unk16 = 0;
    score->unk2a = 0;
    score->channel = 0;
    score->unk48 = 0;
    score->unk27 = 0;
    score->unk28 = 0;
    score->unk10 = 0;
    score->unk11 = 0;
    score->delta_value = score->unk7c;
    score->unk8c = score->unk84;
    score->unk70 = score->unk72;
    score->read_pos = score->next_sep_pos;
    score->loop_pos = score->next_sep_pos;
    for (i = 0; i < 16; i++) {
        score->programs[i] = i;
        score->panpot[i] = 0x40;
        score->vol[i] = 127;
    }
    score->unk78 = 0x7F;
    score->unk7A = 0x7F;
}
#endif
#else

#if VERSION == 33
void Snd_stop(s16 arg0, s16 arg1)
#else
void _SsSndStop(s16 arg0, s16 arg1)
#endif
{
    int i;
    struct SeqStruct* score;
    score = &_ss_score[arg0][arg1];
    _ss_score[arg0][arg1].unk90 &= ~1;
    _ss_score[arg0][arg1].unk90 &= ~2;
    _ss_score[arg0][arg1].unk90 &= ~8;
    _ss_score[arg0][arg1].unk90 |= 4;
    SpuVmSeqKeyOff(arg1 << 8 | arg0);
    score->unk2b = 0;
    score->unk80 = 0;
    score->unk27 = 0;
    score->unk13 = 0;
    score->unk14 = 0;
    score->unk29 = 0;
    score->unk15 = 0;
    score->unk16 = 0;
    score->unk2a = 0;
    score->channel = 0;
    score->unk48 = 0;
    score->unk27 = 0;
    score->unk28 = 0;
    score->unk10 = 0;
    score->unk11 = 0;
    score->delta_value = score->unk7c;
    score->unk8c = score->unk84;
    score->unk70 = score->unk72;
    score->read_pos = score->next_sep_pos;
    score->loop_pos = score->next_sep_pos;
    for (i = 0; i < 16; i++) {
        score->programs[i] = i;
        score->panpot[i] = 0x40;
        score->vol[i] = 127;
    }

    score->unk78 = 0x7F;
    score->unk7A = 0x7F;
}

#if VERSION == 33
void SsSeqStop(s16 arg0) { Snd_stop(arg0, 0); }
#else
void SsSeqStop(s16 arg0) { _SsSndStop(arg0, 0); }
#endif

#if VERSION == 33
void SsSepStop(s16 arg0, s16 arg1) { Snd_stop(arg0, arg1); }
#else
void SsSepStop(s16 arg0, s16 arg1) { _SsSndStop(arg0, arg1); }
#endif
#endif
