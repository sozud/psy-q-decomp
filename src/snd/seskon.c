#include "libsnd_i.h"

long SsVoKeyOn(
    long vab_pro, long pitch, unsigned short volL, unsigned short volR) {
#if VERSION <= 30
    SpuVmSeKeyOn((short)(vab_pro >> 8), (unsigned char)vab_pro,
        (short)(pitch >> 8), (unsigned char)pitch, volL, volR);
#else
    SpuVmSeKeyOn(
        (vab_pro << 8) >> 0x10, vab_pro, pitch >> 8, pitch, volL, volR);
#endif
}
