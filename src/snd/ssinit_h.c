#include "libsnd_i.h"

void SsInitHot(void) {
    ResetCallback();
    SpuInitHot();
    _SsInit();
}
