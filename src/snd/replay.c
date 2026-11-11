#if VERSION >= 36
#include "libsnd_i.h"

void _SsSndReplay(short seq_access_num, short arg1) {
    struct SeqStruct* score = &_ss_score[seq_access_num][arg1];
    score->play_mode = 1;
    _ss_score[seq_access_num][arg1].flags &= ~8;
}
#else
#include "libsnd_i.h"

#if VERSION <= 33
void Snd_replay(s16 arg0, s16 arg1) {
#else
void _SsSndReplay(s16 arg0, s16 arg1) {
#endif
    _ss_score[arg0][arg1].unk2b = 1;
    _ss_score[arg0][arg1].unk90 &= ~8;
}
#endif
