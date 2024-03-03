#include "libsnd_i.h"

#if VERSION == 33
void Snd_replay(s16 arg0, s16 arg1) {
#else
void _SsSndReplay(s16 arg0, s16 arg1) {
#endif
    _ss_score[arg0][arg1].unk2b = 1;
    _ss_score[arg0][arg1].unk90 &= ~8;
}
