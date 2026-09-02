#include "libspu_i.h"
void SpuSetReverbModeFeedback(long feedback) {
    ReverbPreset preset;

    if (_spu_rev_attr.mode > SPU_REV_MODE_DELAY) {
        return;
    }

    if (_spu_rev_attr.mode < SPU_REV_MODE_ECHO) {
        return;
    }

    _memcpy(&preset, &_spu_rev_param[_spu_rev_attr.mode], sizeof(ReverbPreset));

    _spu_rev_attr.feedback = feedback;
    preset.m_Mask = SPU_REV_MASK_vWALL;
    preset.m_Regs.m_vWALL = (feedback * 0x8100) / 127;
    _spu_setReverbAttr(&preset);
}

