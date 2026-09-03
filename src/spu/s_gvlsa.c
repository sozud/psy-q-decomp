#include "libspu_i.h"
void SpuGetVoiceLoopStartAddr(s32 vNum, unsigned long *loopStartAddr) {
    *loopStartAddr = _spu_RXX->raw[vNum * 8 + 7] << _spu_mem_mode_plus;
}

