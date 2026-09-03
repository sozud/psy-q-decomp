#include "libspu_i.h"
void SpuGetReverbModeType(long *mode) {
    *mode = _spu_rev_attr.mode;
}

