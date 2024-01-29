#include "libsnd_i.h"

long SsVoKeyOn(
    long vab_pro, long pitch, unsigned short volL, unsigned short volR) {
    SpuVmSeKeyOn(
        (vab_pro << 8) >> 0x10, vab_pro, pitch >> 8, pitch, volL, volR);
}
