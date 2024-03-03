#include "libsnd_i.h"

#if VERSION == 33
void Snd_play(s16 arg0, s16 arg1);
void Snd_crescendo(s16 arg0, s16 arg1);
void Snd_decrescendo(s16 arg0, s16 arg1);
void Snd_tempo(s16 arg0, s16 arg1);
void Snd_pause(s16 arg0, s16 arg1);
void Snd_replay(s16 arg0, s16 arg1);
void Snd_stop(s32 arg0, s32 arg1);
#endif

void SsSeqCalledTbyT(void) {
    int i;
    int j;
    if (_snd_ev_flag != 1) {
        _snd_ev_flag = 1;

        SpuVmFlush();

        /* For each SEP block */
        for (i = 0; i < _snd_seq_s_max; i++) {
            /* Is it on? */
            if ((1 << i) & _snd_openflag) {
                /* Go through each SEQ track */
                for (j = 0; j < _snd_seq_t_max; j++) {
                    if (_ss_score[i][j].unk90 & 1) {
#if VERSION == 33
                        Snd_play(i, j);
#else
                        _SsSndPlay(i, j);
#endif

                        if (_ss_score[i][j].unk90 & 0x10) {
#if VERSION == 33
                            Snd_crescendo(i, j);
#else
                            _SsSndCrescendo(i, j);
#endif
                        }

                        if (_ss_score[i][j].unk90 & 0x20) {
#if VERSION == 33
                            Snd_decrescendo(i, j);
#else
                            _SsSndDecrescendo(i, j);
#endif
                        }

                        if (_ss_score[i][j].unk90 & 0x40) {
#if VERSION == 33
                            Snd_tempo(i, j);
#else
                            _SsSndTempo(i, j);
#endif
                        }

                        if (_ss_score[i][j].unk90 & 0x80) {
#if VERSION == 33
                            Snd_tempo(i, j);
#else
                            _SsSndTempo(i, j);
#endif
                        }
                    }

                    if (_ss_score[i][j].unk90 & 2) {
#if VERSION == 33
                        Snd_pause(i, j);
#else
                        _SsSndPause(i, j);
#endif
                    }

                    if (_ss_score[i][j].unk90 & 8) {
#if VERSION == 33
                        Snd_replay(i, j);
#else
                        _SsSndReplay(i, j);
#endif
                    }

                    if (_ss_score[i][j].unk90 & 4) {
#if VERSION == 33
                        Snd_stop(i, j);
#else
                        _SsSndStop(i, j);
#endif
                        _ss_score[i][j].unk90 = 0;
                    }
                }
            }
        }
        _snd_ev_flag = 0;
    }
}
