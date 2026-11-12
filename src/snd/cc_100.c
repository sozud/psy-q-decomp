#include "libsnd_i.h"
void _SsContRpn1(s16 sep_no, s16 seq_no, u8 control_value) {
    struct SeqStruct* score_struct;

    score_struct = &_ss_score[sep_no][seq_no];
    score_struct->unk18 = control_value;
    score_struct->unk1E += 1;
    score_struct->delta_value = _SsReadDeltaValue(sep_no, seq_no);
}
