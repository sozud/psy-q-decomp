#include "libsnd_i.h"

void SsSetSerialVol(char s_num, short voll, short volr) {
    SpuCommonAttr attr;
    if (s_num == 0) {
        attr.mask = 0xC0;
        if (voll >= 128) {
            voll = 127;
        }
        if (volr >= 128) {
            volr = 127;
        }
#if VERSION == 33
        attr.cd.volume.left = (voll * 0x7FFF) / 127;
        attr.cd.volume.right = (volr * 0x7FFF) / 127;
#else
        attr.cd.volume.left = voll * 258;
        attr.cd.volume.right = volr * 258;
#endif
    }
    if (s_num == 1) {
        attr.mask = 0xC00;
        if (voll >= 128) {
            voll = 127;
        }
        if (volr >= 128) {
            volr = 127;
        }
#if VERSION == 33
        attr.ext.volume.left = (voll * 0x7FFF) / 127;
        attr.ext.volume.right = (volr * 0x7FFF) / 127;
#else
        attr.ext.volume.left = voll * 258;
        attr.ext.volume.right = volr * 258;
#endif
    }
    SpuSetCommonAttr(&attr);
}
