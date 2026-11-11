#include "libsnd_i.h"

#if VERSION <= 30
void SsSetMVol(short voll, short volr) {
    SndSpuCommonAttr attr;
    attr.mask = 3;
    attr.mvol.left = 129 * voll;
    attr.mvol.right = 129 * volr;
    _spu_ioctl(0, SND_SPU_SET_COMMON, &attr);
}
#else

void SsSetMVol(short voll, short volr) {
    SpuCommonAttr attr;
    attr.mask = SPU_COMMON_MVOLL | SPU_COMMON_MVOLR;
    attr.mvol.left = 129 * voll;
    attr.mvol.right = 129 * volr;
    SpuSetCommonAttr(&attr);
}
#endif
