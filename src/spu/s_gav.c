#include "libspu_i.h"
u32 _SpuGetAnyVoice(s32 arg0, s32 arg1) {
    return ((_spu_RXX->raw[arg1] & 0xFF) << 16) | _spu_RXX->raw[arg0];
}

