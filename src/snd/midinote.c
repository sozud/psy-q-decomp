#include "libsnd_i.h"

void _SsNoteOn(s16 seq_no, s16 sep_no, u8 note, u8 vol) {
    struct SeqStruct* score;
    u8 ch;
    u16 voll;
    u8 pan;

    score = &_ss_score[seq_no][sep_no];
    ch = score->channel_idx;
    voll = vol;
    pan = score->panpot[ch];
#if VERSION == 36
    if (((score->channel_mute >> ch) & 1) == 0 && score->voll != 0) {
        if (vol != 0) {
            _SsVmKeyOn(seq_no | (sep_no << 8), score->vab_id,
                       score->programs[ch], note, voll, pan);
            score->unkA8_36 = voll;
        } else {
            _SsVmKeyOff(seq_no | (sep_no << 8), score->vab_id,
                        score->programs[ch], note);
        }
    }
#elif VERSION >= 41
    if (vol != 0) {
        if (((score->channel_mute >> ch) & 1) == 0) {
            _SsVmKeyOn(seq_no | (sep_no << 8), score->vab_id,
                       score->programs[ch], note, voll, pan);
        }
    } else {
        _SsVmKeyOff(seq_no | (sep_no << 8), score->vab_id,
                    score->programs[ch], note);
    }
#else
    if (score->voll != 0) {
        if (vol != 0) {
            if (((score->channel_mute >> ch) & 1) == 0) {
                _SsVmKeyOn(seq_no | (sep_no << 8), score->vab_id,
                           score->programs[ch], note, voll, pan);
            }
        } else {
            _SsVmKeyOff(seq_no | (sep_no << 8), score->vab_id,
                        score->programs[ch], note);
        }
    }
#endif
}
