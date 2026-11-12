#include "libsnd_i.h"

void SsInit(void) {
    ResetCallback();
    SpuInit();
    _SsInit();
}
