#include "libspu_i.h"
void SpuGetReverbModeDepth(s16 *depth_left, s16 *depth_right) {
    *depth_left = _spu_rev_attr.depth.left;
    *depth_right = _spu_rev_attr.depth.right;
}

