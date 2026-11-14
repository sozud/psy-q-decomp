#include "libsnd_i.h"

#if VERSION <= 30
short SsVabTransBodyPartly(
    unsigned char* addr, unsigned long bufsize, short vabid) {
    static s32 not_send_size = 0;
    static s16 vabid_old = 0xffff;
    s16 new_vabid;
    u32 new_bufsize;
    u32 written;

    if (vabid < 0 || vabid > 16 || _svm_vab_used[vabid] != 2) {
        return -1;
    }
    if (not_send_size == 0) {
        vabid_old = vabid;
        not_send_size = _svm_vab_total[vabid];
        SpuSetTransferMode(0);
        SpuSetTransferStartAddr(_svm_vab_start[vabid]);
    }
    new_vabid = vabid_old;
    if (new_vabid != vabid) {
        return -1;
    }
    new_bufsize = bufsize;
    if (not_send_size < new_bufsize) {
        new_bufsize = not_send_size;
    }
    written = SpuWritePartly(addr, new_bufsize);
    _spu_setInTransfer(1);
    if (written != new_bufsize) {
        return -1;
    }
    not_send_size -= new_bufsize;
    if (not_send_size == 0) {
        not_send_size = 0;
        vabid_old = -1;
        _svm_vab_used[new_vabid] = 1;
        return new_vabid;
    }
    return -2;
}
#else
short SsVabTransBodyPartly(
    unsigned char* addr, unsigned long bufsize, short vabid) {
    static s32 not_send_size = 0;
    static s16 vabid_old = 0xffff;
    s16 new_vabid;
    s32 buff_diff;
    u32 new_bufsize;
    if (vabid < 0 || vabid > 16 || _svm_vab_used[vabid] != 2) {
        _spu_setInTransfer(0);
        return -1;
    }
    if (not_send_size == 0) {
        vabid_old = vabid;
        not_send_size = _svm_vab_total[vabid];
        SpuSetTransferMode(0);
#if VERSION >= 43
        if (SpuSetTransferStartAddr(_svm_vab_start[vabid]) == 0) {
            not_send_size = 0;
            vabid_old = -1;
            _spu_setInTransfer(0);
            return -1;
        }
#else
        SpuSetTransferStartAddr(_svm_vab_start[vabid]);
#endif
    }
    new_vabid = vabid_old;
    if (new_vabid != vabid) {
        _spu_setInTransfer(0);
        return -1;
    }
    new_bufsize = bufsize;
    if (not_send_size < new_bufsize) {
        new_bufsize = not_send_size;
    }
    _spu_setInTransfer(1);
    SpuWritePartly(addr, new_bufsize);
    buff_diff = not_send_size - new_bufsize;
    not_send_size = buff_diff;
    if (buff_diff == 0) {
        vabid_old = -1;
        not_send_size = 0;
        _svm_vab_used[new_vabid] = 1;
        return new_vabid;
    }
    return -2;
}
#endif
