#include "libspu_i.h"
unsigned long SpuSetPitchLFOVoice(long on_off, unsigned long voice_bit) {
    return _SpuSetAnyVoice(on_off, voice_bit, 0xC8, 0xC9);
}

