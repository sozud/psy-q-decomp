#include "libspu_i.h"
long SpuSetReverbModeType(long mode) {
    ReverbPreset preset;
    s32 bClearWorkArea;
    u32 bMasterReverbEnableFlag;
    u16 spucnt;

    bClearWorkArea = 0;

    if (mode & SPU_REV_MODE_CLEAR_WA) {
        mode &= ~SPU_REV_MODE_CLEAR_WA;
        bClearWorkArea = 1;
    }

    if (mode < 0 || mode >= SPU_REV_MODE_MAX) {
        return SPU_ERROR;
    }

    if (_SpuIsInAllocateArea_(_spu_rev_startaddr[mode])) {
        return SPU_ERROR;
    }

    _spu_rev_attr.mode = mode;
    _spu_rev_offsetaddr = _spu_rev_startaddr[mode];

    _memcpy(&preset, &_spu_rev_param[mode], sizeof(ReverbPreset));

    preset.m_Mask = 0;
    switch (mode) {
        case SPU_REV_MODE_ECHO:
            _spu_rev_attr.feedback = 0x7F;
            _spu_rev_attr.delay = 0x7F;
            break;
        case SPU_REV_MODE_DELAY:
            _spu_rev_attr.feedback = 0;
            _spu_rev_attr.delay = 0x7F;
            break;
        default:
            _spu_rev_attr.feedback = 0;
            _spu_rev_attr.delay = 0;
            break;
    }

    bMasterReverbEnableFlag = (_spu_RXX->rxx.spucnt & SPU_CONTROL_FLAG_MASTER_REVERB) != 0;
    if (bMasterReverbEnableFlag) {
        spucnt = _spu_RXX->rxx.spucnt;
        spucnt &= ~SPU_CONTROL_FLAG_MASTER_REVERB;
        _spu_RXX->rxx.spucnt = spucnt;
    }

    _spu_RXX->rxx.rev_vol.left = 0;
    _spu_RXX->rxx.rev_vol.right = 0;
    _spu_rev_attr.depth.left = 0;
    _spu_rev_attr.depth.right = 0;

    _spu_setReverbAttr(&preset);

    if (bClearWorkArea) {
        SpuClearReverbWorkArea(mode);
    }

    _spu_FsetRXX(0xD1, _spu_rev_offsetaddr, 0);

    if (bMasterReverbEnableFlag) {
        spucnt = _spu_RXX->rxx.spucnt;
        spucnt |= SPU_CONTROL_FLAG_MASTER_REVERB;
        _spu_RXX->rxx.spucnt = spucnt;
    }

    return SPU_SUCCESS;
}

