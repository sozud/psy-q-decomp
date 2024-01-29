#include "libsnd_i.h"

short SsVabTransBodyPartly(unsigned char* addr, unsigned long bufsize, short vabid) {
    static s32 D_00000000 = 0;
    static s16 D_00000004 = 0xffff;
    s16 new_vabid;
    s32 buff_diff;
    u32 new_bufsize;
    if (vabid < 0 || vabid > 16 || _svm_vab_used[vabid] != 2) {
        _spu_setInTransfer(0);
        return -1;
    }
    if (D_00000000 == 0) {
        D_00000004 = vabid;
        D_00000000 = _svm_vab_total[vabid];
        SpuSetTransferMode(0);
        SpuSetTransferStartAddr(_svm_vab_start[vabid]);
    }
    new_vabid = D_00000004;
    if (new_vabid != vabid) {
        _spu_setInTransfer(0);
        return -1;
    }
    new_bufsize = bufsize;
    if (D_00000000 < new_bufsize) {
        new_bufsize = D_00000000;
    }
    _spu_setInTransfer(1);
    SpuWritePartly(addr, new_bufsize);
    buff_diff = D_00000000 - new_bufsize;
    D_00000000 = buff_diff;
    if (buff_diff == 0) {
        D_00000004 = -1;
        D_00000000 = 0;
        _svm_vab_used[new_vabid] = 1;
        return new_vabid;
    }
    return -2;
}
