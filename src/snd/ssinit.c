#include "ssinit.h"

s32 _snd_openflag;
s32 VBLANK_MINUS;
SndSsMarkCallbackProc _SsMarkCallback[32][16];
struct SeqStruct* _ss_score[32];
s16 _snd_seq_s_max;
s16 _snd_seq_t_max;
s32 _snd_ev_flag;

static short D_80032EC0[] = {
    0x0000, 0x0000, 0x1000, 0x3000, 0x00BF, 0x0000, 0x0000, 0x0000};

static short D_80032ED0[] = {
    0x3FFF, 0x3FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

void _SsInit(void) {
    u16* var_a2;
    int i, j;

    var_a2 = (u16*)0x1F801C00;
    for (i = 0; i < 24; i++) {
        for (j = 0; j < 8; j++) {
            *var_a2++ = D_80032EC0[j];
        }
    }

    var_a2 = (u16*)0x1F801D80;
    for (i = 0; i < 16; i++) {
        *var_a2++ = D_80032ED0[i];
    }

    SpuVmInit(24);

    for (j = 0; j < 32; j++) {
        for (i = 0; i < 16; i++) {
            _SsMarkCallback[j][i] = 0;
        }
    }

    VBLANK_MINUS = 0x3C;
    _snd_openflag = 0;
    _snd_ev_flag = 0;
}