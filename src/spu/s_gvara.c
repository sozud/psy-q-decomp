#include "libspu_i.h"
void SpuGetVoiceARAttr(s32 vNum, u16 *AR, long *ARmode) {
    u16 temp_v1;
    u16 temp_v0;

    temp_v1 = _spu_RXX->raw[vNum * 8 + 4 + 0];
    temp_v0 = _spu_RXX->raw[vNum * 8 + 4 + 1];
    *AR = (temp_v1 >> 8) & 0x3F;
    *ARmode = 1;
    if (temp_v1 & 0x8000) {
        *ARmode = 5;
    }
}

