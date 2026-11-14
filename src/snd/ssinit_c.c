#include "libsnd_i.h"

void SsInit(void) {
    ResetCallback();
    SpuInit();
#if VERSION >= 43
    SpuClearReverbWorkArea(7);
#endif
    _SsInit();
}
