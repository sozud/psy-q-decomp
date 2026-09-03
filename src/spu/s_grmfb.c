#include "libspu_i.h"
void SpuGetReverbModeFeedback(long *feedback) {
    *feedback = _spu_rev_attr.feedback;
}

