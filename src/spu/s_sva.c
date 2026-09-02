#include "libspu_i.h"
void SpuSetVoiceAttr(SpuVoiceAttr *arg) {
    _SpuRSetVoiceAttr(arg, NULL, 0x17, 0);
}

