#if VERSION >= 36
#include "libsnd_i.h"

void _SsSndNextSep(short sep_no, short seq_no) {
    struct SeqStruct* score;
    score = &_ss_score[sep_no][seq_no];
    _ss_score[sep_no][seq_no].l_count = 1;
    score->unk21 = 0;
    _ss_score[sep_no][seq_no].flags &= ~0x100;
    _ss_score[sep_no][seq_no].flags &= ~0x8;
    _ss_score[sep_no][seq_no].flags &= ~0x2;
    _ss_score[sep_no][seq_no].flags &= ~0x4;
    _ss_score[sep_no][seq_no].flags &= ~0x200;
    score->play_mode = 1;
    score->seq_ptr = score->read_pos;
    _ss_score[sep_no][seq_no].flags |= 1;
}
#else
#include "libsnd_i.h"

#if VERSION <= 30
void Snd_nextseq(s16 arg0, s16 arg1) {
#if VERSION == 30
    struct SeqStruct* temp_a0;
    temp_a0 = &_ss_score[arg0][arg1];
    temp_a0->unk46 = 1;
    temp_a0->unk48 = 0;
#else
    _ss_score[arg0][arg1].unk46 = 1;
    _ss_score[arg0][arg1].unk48 = 0;
#endif
    SpuVmSetSeqVol(arg0, 0x7F, 0x7F, arg1);
    _ss_score[arg0][arg1].unk90 &= ~0x100;
    _ss_score[arg0][arg1].unk90 &= ~0x8;
    _ss_score[arg0][arg1].unk90 &= ~0x2;
    _ss_score[arg0][arg1].unk90 &= ~0x4;
    _ss_score[arg0][arg1].unk90 &= ~0x200;
#if VERSION == 30
    temp_a0->unk2b = 1;
#else
    _ss_score[arg0][arg1].unk2b = 1;
#endif
    _ss_score[arg0][arg1].unk90 |= 1;
}
#elif VERSION == 33
void Snd_nextsep(s16 arg0, s16 arg1) {
    struct SeqStruct* temp_a0;
    temp_a0 = &_ss_score[arg0][arg1];
    _ss_score[arg0][arg1].unk46 = 1;
    temp_a0->unk48 = 0;
    _ss_score[arg0][arg1].unk90 &= ~0x100;
    _ss_score[arg0][arg1].unk90 &= ~0x8;
    _ss_score[arg0][arg1].unk90 &= ~0x2;
    _ss_score[arg0][arg1].unk90 &= ~0x4;
    _ss_score[arg0][arg1].unk90 &= ~0x200;
    temp_a0->unk2b = 1;
    _ss_score[arg0][arg1].unk90 |= 1;
}
#else
void _SsSndNextSep(s16 arg0, s16 arg1) {
    struct SeqStruct* temp_a0;
    temp_a0 = &_ss_score[arg0][arg1];
    _ss_score[arg0][arg1].unk46 = 1;
    temp_a0->unk48 = 0;
    _ss_score[arg0][arg1].unk90 &= ~0x100;
    _ss_score[arg0][arg1].unk90 &= ~0x8;
    _ss_score[arg0][arg1].unk90 &= ~0x2;
    _ss_score[arg0][arg1].unk90 &= ~0x4;
    _ss_score[arg0][arg1].unk90 &= ~0x200;
    temp_a0->unk2b = 1;
    temp_a0->read_pos = temp_a0->next_sep_pos;
    _ss_score[arg0][arg1].unk90 |= 1;
}
#endif
#endif
