#include "libsnd_i.h"

#if VERSION <= 30
void SsSetSerialAttr(char s_num, char attr, char mode) {
    SndSpuCommonAttr c_attr;

    if (s_num == SS_SERIAL_A) {
        if (attr == SS_MIX) {
            c_attr.mask = 0x400;
            c_attr.cd.mix = mode;
        }
        if (attr == SS_REV) {
            c_attr.mask = 0x100;
            c_attr.cd.reverb = mode;
        }
    }
    if (s_num == SS_SERIAL_B) {
        if (attr == SS_MIX) {
            c_attr.mask = 0x800;
            c_attr.ext.mix = mode;
        }
        if (attr == SS_REV) {
            c_attr.mask = 0x200;
            c_attr.ext.reverb = mode;
        }
    }
    _spu_ioctl(0, SND_SPU_SET_COMMON, &c_attr);
}
#else

void SsSetSerialAttr(char arg0, char arg1, char arg2) {
    SpuCommonAttr attr;
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_v1;

    if (!(arg0 & 0xFF)) {
        temp_v1 = arg1 & 0xFF;
        if (temp_v1 == 0) {
            attr.mask = 0x200;
            attr.cd.mix = arg2 & 0xFF;
        }
        if (temp_v1 == 1) {
            attr.mask = 0x100;
            attr.cd.reverb = arg2 & 0xFF;
        }
    }
    temp_a0 = arg0 & 0xFF;
    temp_a1 = arg1 & 0xFF;
    if (temp_a0 == 1) {
        if (temp_a1 == 0) {
            attr.mask = 0x2000;
            attr.ext.mix = arg2 & 0xFF;
        }
        if (temp_a1 == temp_a0) {
            attr.mask = 0x1000;
            attr.ext.reverb = arg2 & 0xFF;
        }
    }
    SpuSetCommonAttr(&attr);
}
#endif
