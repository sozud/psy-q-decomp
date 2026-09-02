#include "libspu_i.h"
unsigned long SpuFlush(unsigned long ev) {
    s32 i;
    u32 var_a2;
    u32 var_a1;

    var_a2 = 0;
    var_a1 = ev;

    if (((var_a1 == 0) || (var_a1 & 2)) && (_spu_RQmask & 2)) {
        _spu_RQmask &= ~2;
        var_a2 = 2;
        _spu_RXX->rxx.chan_fm[0] = _spu_RQ[4];
        _spu_RXX->rxx.chan_fm[1] = _spu_RQ[5];
    }

    if (((var_a1 == 0) || (ev & 4)) && (_spu_RQmask & 4)) {
        _spu_RQmask &= ~4;
        var_a2 |= 4;
        _spu_RXX->rxx.noise_mode[0] = _spu_RQ[6];
        _spu_RXX->rxx.noise_mode[1] = _spu_RQ[7];
    }

    if (((var_a1 == 0) || (ev & 8)) && (_spu_RQmask & 8)) {
        _spu_RQmask &= ~8;
        var_a2 |= 8;
        _spu_RXX->rxx.rev_mode[0] = _spu_RQ[8];
        _spu_RXX->rxx.rev_mode[1] = _spu_RQ[9];
    }

    if (((var_a1 == 0) || (ev & 1)) && (_spu_RQmask & 1)) {
        _spu_RQmask &= ~1;
        _spu_RXX->rxx.key_on[0] = _spu_RQ[0];
        _spu_RXX->rxx.key_on[1] = _spu_RQ[1];
        _spu_RXX->rxx.key_off[0] = _spu_RQ[2];
        _spu_RXX->rxx.key_off[1] = _spu_RQ[3];

        for (i = 0; i < 4; i++) {
            _spu_RQ[i] = 0;
        }

        var_a2 |= 1;

        if (_spu_env & 1) {
            _spu_keystat = _spu_RQvoice;
        }
    }

    return var_a2;
}

