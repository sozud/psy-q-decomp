#include "libsnd_i.h"

#if VERSION == 33
void Snd_nextpause(s16 arg0, s16 arg1) {
    _ss_score[arg0][arg1].unk2b = 0;
    _ss_score[arg0][arg1].unk90 &= ~0x100;
}
#endif

#if VERSION == 33
void Snd_pause(s16 arg0, s16 arg1) {
    struct SeqStruct* temp_s2;
    temp_s2 = &_ss_score[arg0][arg1];
    SpuVmSeqKeyOff(arg1 << 8 | arg0);
    temp_s2->unk2b = 0;
    _ss_score[arg0][arg1].unk90 &= ~2;
}
#else
void _SsSndPause(s16 arg0, s16 arg1) {
    struct SeqStruct* temp_s2;
    temp_s2 = &_ss_score[arg0][arg1];
    SpuVmSeqKeyOff(arg1 << 8 | arg0);
    temp_s2->unk2b = 0;
    _ss_score[arg0][arg1].unk90 &= ~2;
}
#endif