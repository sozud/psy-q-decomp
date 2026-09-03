#include "libspu_i.h"
void SpuGetVoiceStartAddr(s32 vNum, unsigned long *startAddr) {
    *startAddr = _spu_RXX->raw[vNum * 8 + 3] << _spu_mem_mode_plus;
}

