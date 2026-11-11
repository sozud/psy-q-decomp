#include "libsnd_i.h"

void SsInitHot(void) {
    ResetCallback();
#if VERSION == 31

    SpuInit();
#else
    SpuInitHot();
#endif
    _SsInit();
}
