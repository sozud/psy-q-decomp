#include "libspu_i.h"
extern long TestEvent(long);
long SpuIsTransferCompleted(long flag) {
    s32 var_v0;

    if ((_spu_trans_mode == 1) || (_spu_inTransfer == 1)) {
        return 1;
    }

    var_v0 = TestEvent(_spu_EVdma);
    if (flag == 1) {
        while (var_v0 == 0) {
            var_v0 = TestEvent(_spu_EVdma);
        }

        _spu_inTransfer = 1;
        return 1;
    }

    if (var_v0 == 1) {
        _spu_inTransfer = 1;
    }

    return var_v0;
}

