#include "libspu_i.h"
void SpuSetVoiceStartAddr(s32 vNum, unsigned long startAddr) {
    _spu_FsetRXXa(vNum * 8 + 3, startAddr);
    _spu_Fw1ts();
}

