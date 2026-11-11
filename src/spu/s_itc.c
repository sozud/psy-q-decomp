#if VERSION <= 30
#include "spu_old.h"

extern long TestEvent(long);

long SpuIsTransferCompleted(long flag) {
    long ret;

    if (_spu_trans_mode == 1 || _spu_inTransfer == 1) {
        return 1;
    }

    ret = TestEvent(_spu_EVdma);
    if (flag == 1) {
        while (ret == 0) {
            ret = TestEvent(_spu_EVdma);
        }
        _spu_inTransfer = 1;
        return ret;
    }

    if (ret == 1) {
        _spu_inTransfer = ret;
    }
    return ret;
}

void _spu_setInTransfer(long mode) {
    if (mode == 1) {
        _spu_inTransfer = 0;
    } else {
        _spu_inTransfer = 1;
    }
}

long _spu_getInTransfer(void) {
    return _spu_inTransfer == 0;
}
#else
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
#endif
