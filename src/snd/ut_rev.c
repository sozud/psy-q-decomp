#include "libsnd_i.h"

#if VERSION <= 33
void SsUtReverbOn(void) { SpuSetReverb(1); }

void SsUtReverbOff(void) { SpuSetReverb(0); }
#endif

#if VERSION <= 30
s16 SsUtSetReverbType(s16 type) {
    if ((u16)type < 10) {
        _svm_rattr.mask = 1;
        _svm_rattr.mode = type;
        SpuSetReverbModeParam(&_svm_rattr);
        return type;
    }
    return -1;
}

s16 SsUtGetReverbType(void) { return _svm_rattr.mode; }
#else

s16 SsUtSetReverbType(s16 arg0) {
    s16 var_v1;
    s32 var_a1;
    SpuReverbAttr* new_var;
    s32 var_v0;
    var_a1 = 0;
    var_v1 = arg0;
    if (arg0 < 0) {
        var_a1 = 1;
        var_v1 = -arg0;
    }
    if (((u32)(((u16)var_v1) & 0xFFFF)) < 0xAU) {
        _svm_rattr.mask = 1;
        if (var_a1 != 0) {
            var_v0 = (var_v1 | 0x100) << 0x10;
        } else {
            var_v0 = var_v1 << 0x10;
        }
        new_var = &_svm_rattr;
        _svm_rattr.mode = var_v0 >> 0x10;
        if (var_v1 == 0) {
            SpuSetReverb(0);
        }
        SpuSetReverbModeParam(new_var);
        return var_v1;
    }
    return -1;
}

s16 SsUtGetReverbType(void) { return _svm_rattr.mode; }
#endif

#if VERSION <= 33
void SsUtSetReverbDepth(s16 ldepth, s16 rdepth) {
    _svm_rattr.mask = 6;
    _svm_rattr.depth.left = (ldepth * 0x7FFF) / 127;
    _svm_rattr.depth.right = (rdepth * 0x7FFF) / 127;
    SpuSetReverbModeParam(&_svm_rattr);
}

void SsUtSetReverbFeedback(short feedback) {
    _svm_rattr.mask = SPU_REV_FEEDBACK;
    _svm_rattr.feedback = feedback;
    SpuSetReverbModeParam(&_svm_rattr);
}

void SsUtSetReverbDelay(short delay) {
    _svm_rattr.mask = SPU_REV_DELAYTIME;
    _svm_rattr.delay = delay;
    SpuSetReverbModeParam(&_svm_rattr);
}
#endif
