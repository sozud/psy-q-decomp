#include "libspu_i.h"
void SpuSetVoiceLoopStartAddr(s32 vNum, unsigned long lsa) {
    _spu_FsetRXXa(vNum * 8 + 7, lsa);
    _spu_Fw1ts();
}

