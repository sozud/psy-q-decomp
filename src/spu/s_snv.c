#include "libspu_i.h"

unsigned long SpuSetNoiseVoice(long on_off, unsigned long voice_bit) {
    _SpuSetAnyVoice(on_off, voice_bit, 0xCA, 0xCB);
}