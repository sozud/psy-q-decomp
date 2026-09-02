#include "libspu_i.h"
void SpuGetReverbModeDelayTime(long *delay) {
    *delay = _spu_rev_attr.delay;
}

