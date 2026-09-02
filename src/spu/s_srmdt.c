#include "libspu_i.h"

void SpuSetReverbModeDelayTime(long delay) {
    ReverbPreset preset;
    s32 scaledDelay;
    s32 scaledDelay2;

    if (_spu_rev_attr.mode > SPU_REV_MODE_DELAY) {
        return;
    }

    if (_spu_rev_attr.mode < SPU_REV_MODE_ECHO) {
        return;
    }

    _memcpy(&preset, &_spu_rev_param[_spu_rev_attr.mode], sizeof(ReverbPreset));

    preset.m_Mask = SPU_REV_MASK_mLSAME
        | SPU_REV_MASK_mRSAME
        | SPU_REV_MASK_mLCOMB1
        | SPU_REV_MASK_dLSAME
        | SPU_REV_MASK_mLAPF1
        | SPU_REV_MASK_mRAPF1;

    scaledDelay  = (delay << 12) / 127;
    scaledDelay2 = (delay << 13) / 127;

    preset.m_Regs.m_mLSAME  = scaledDelay2 - preset.m_Regs.m_dAPF1;
    preset.m_Regs.m_mRSAME  = scaledDelay  - preset.m_Regs.m_dAPF2;
    preset.m_Regs.m_mLCOMB1 = preset.m_Regs.m_mRCOMB1 + scaledDelay;
    preset.m_Regs.m_dLSAME  = preset.m_Regs.m_dRSAME + scaledDelay;
    preset.m_Regs.m_mLAPF1  = preset.m_Regs.m_mLAPF2 + scaledDelay;
    preset.m_Regs.m_mRAPF1  = preset.m_Regs.m_mRAPF2 + scaledDelay;

    _spu_rev_attr.delay = delay;
    _spu_setReverbAttr(&preset);
}

