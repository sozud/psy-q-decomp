#include "libspu_i.h"
unsigned long SpuSetReverbVoice(long on_off, unsigned long voice_bit) {
    return _SpuSetAnyVoice(on_off, voice_bit, 0xCC, 0xCD);
}

