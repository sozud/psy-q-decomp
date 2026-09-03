#include "libspu_i.h"
unsigned long SpuGetReverbVoice(void) {
    return _SpuGetAnyVoice(0xCC, 0xCD);
}

