#include "libspu_i.h"
unsigned long SpuGetPitchLFOVoice(void) {
    return _SpuGetAnyVoice(0xC8, 0xC9);
}

