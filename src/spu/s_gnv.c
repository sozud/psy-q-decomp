#include "libspu_i.h"
unsigned long SpuGetNoiseVoice(void) {
    return _SpuGetAnyVoice(0xCA, 0xCB);
}

