#include "libsnd_i.h"

long SsVoKeyOff(long vab_pro, long pitch) {
#if VERSION <= 30
    SpuVmSeKeyOff((short)(vab_pro >> 8), (unsigned char)vab_pro,
        (short)(pitch >> 8), (unsigned char)pitch);
#else
    SpuVmSeKeyOff((vab_pro << 8) >> 0x10, vab_pro, (pitch >> 8), pitch);
#endif
}
