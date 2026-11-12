#include "libsnd_i.h"
void _SsSetProgramChange(s16 sep_no, s16 seq_no, u8 prog) {
    struct SeqStruct* score_struct;

    score_struct = &_ss_score[sep_no][seq_no];
    score_struct->programs[score_struct->channel_idx] = prog;
    score_struct->delta_value = _SsReadDeltaValue(sep_no, seq_no);
}
