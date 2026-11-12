#include "libsnd_i.h"

void _SsContMainVol(short sep_no, short seq_no, unsigned char data) {
    struct SeqStruct* score;
    unsigned char channel;

    score = &_ss_score[sep_no][seq_no];
    channel = score->channel_idx;
    _SsVmSetVol(sep_no | (seq_no << 8), score->vab_id, score->programs[channel],
                data, score->panpot[channel]);
    score->vol[channel] = data;
    score->delta_value = _SsReadDeltaValue(sep_no, seq_no);
}
