#include "libsnd_i.h"

s16 SsVabFakeBody(s16 vabid) {
    if ((vabid < 0x11U) && (_svm_vab_used[vabid] == 2)) {
#if VERSION > 30
        _spu_setInTransfer(0);
#endif
        _svm_vab_used[vabid] = 1;
        return vabid;
    }
    return -1;
}
