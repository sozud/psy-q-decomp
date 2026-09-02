#include "libspu_i.h"
long SpuIsReverbWorkAreaReserved(long on_off) {
    if (on_off == -1) {
        return _spu_rev_reserve_wa;
    }

    if (_SpuIsInAllocateArea_(_spu_rev_offsetaddr) == 0) {
        return 1;
    } else {
        return 0;
    }
}

