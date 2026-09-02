#include "libspu_i.h"
long SpuSetReverbDepth(SpuReverbAttr *attr) {
    s32 temp_a2;
    u32 temp_a1;

    temp_a1 = attr->mask;
    temp_a2 = temp_a1 == 0;
    if ((temp_a2 != 0) || (temp_a1 & 2)) {
        _spu_RXX->rxx.rev_vol.left = attr->depth.left;
        _spu_rev_attr.depth.left = attr->depth.left;
    }
    if ((temp_a2 != 0) || (temp_a1 & 4)) {
        _spu_RXX->rxx.rev_vol.right = attr->depth.right;
        _spu_rev_attr.depth.right = attr->depth.right;
    }
    return 0;
}

