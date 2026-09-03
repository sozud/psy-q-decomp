#include "libspu_i.h"

#if VERSION == 40
void _spu_setInTransfer(s32 arg0) {
    if (arg0 == 1) {
        _spu_inTransfer = 0;
    } else {
        _spu_inTransfer = 1;
    }
}

s32 _spu_getInTransfer(void) {
    if (_spu_inTransfer == 1) {
        return 0;
    } else {
        return 1;
    }
}

#else

void _spu_setInTransfer(s32 arg0) {
    if (arg0 == 1) {
        _spu_inTransfer = 0;
        return;
    }
    _spu_inTransfer = 1;
}

s32 _spu_getInTransfer(void) { return _spu_inTransfer == 0; }
#endif
