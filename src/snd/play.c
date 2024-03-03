#include "libsnd_i.h"

#if VERSION == 33
void Snd_play(s16 arg0, s16 arg1) { SeqPlay(arg0, arg1); }
#else
void _SsSndPlay(s16 arg0, s16 arg1) { _SsSeqPlay(arg0, arg1); }
#endif