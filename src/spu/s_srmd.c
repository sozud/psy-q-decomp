#include "libspu_i.h"
void SpuSetReverbModeDepth(s16 depth_left, s16 depth_right) {
    _spu_RXX->rxx.rev_vol.left = depth_left;
    _spu_RXX->rxx.rev_vol.right = depth_right;
    _spu_rev_attr.depth.left = depth_left;
    _spu_rev_attr.depth.right = depth_right;
}

