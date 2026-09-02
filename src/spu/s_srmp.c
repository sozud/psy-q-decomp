#include "libspu_i.h"
long SpuSetReverbModeParam(SpuReverbAttr *attr) {
    struct rev_param_entry param;
    long clear_wa;
    long bSetMode;
    long bSetDelay;
    long bSetFeedback;
    long bReverbWasEnabled;
    long mode;
    u32 mask;
    long no_mask;
    long temp;
    long delay;
    long feedback;
    u16 spucnt;

    clear_wa = 0;
    mask = attr->mask;
    bReverbWasEnabled = 0;
    bSetMode = 0;
    bSetDelay = 0;
    bSetFeedback = 0;
    no_mask = (mask == 0);
    param.flags = 0;

    if (no_mask || (mask & SPU_REV_MODE)) {
        mode = attr->mode;
        if (mode & SPU_REV_MODE_CLEAR_WA) {
            mode &= ~SPU_REV_MODE_CLEAR_WA;
            clear_wa = 1;
        }

        if ((u32)mode >= SPU_REV_MODE_MAX || _SpuIsInAllocateArea_(_spu_rev_startaddr[mode])) {
            return -1;
        }

        bSetMode = 1;
        _spu_rev_attr.mode = mode;
        _spu_rev_offsetaddr = _spu_rev_startaddr[_spu_rev_attr.mode];
        _memcpy(&param, &_spu_rev_param[_spu_rev_attr.mode], sizeof(ReverbPreset));

        switch (_spu_rev_attr.mode) {
        case SPU_REV_MODE_ECHO:
            _spu_rev_attr.feedback = 127;
            _spu_rev_attr.delay = 127;
            break;
        case SPU_REV_MODE_DELAY:
            _spu_rev_attr.feedback = 0;
            _spu_rev_attr.delay = 127;
            break;
        default:
            _spu_rev_attr.feedback = 0;
            _spu_rev_attr.delay = 0;
            break;
        }
    }

    if (no_mask || (mask & SPU_REV_DELAYTIME)) {
        if (_spu_rev_attr.mode < 9) {
            if (_spu_rev_attr.mode >= 7) {
                bSetDelay = 1;
                if (!bSetMode) {
                    _memcpy(&param, &_spu_rev_param[_spu_rev_attr.mode], sizeof(ReverbPreset));
                    param.flags = 0x0C011C00;
                }
                delay = attr->delay;
                _spu_rev_attr.delay = delay;
                param.mLSAME = ((delay << 13) / 127) - param.dAPF1;
                temp = (delay << 12) / 127;
                param.mRSAME = temp - param.dAPF2;
                param.mLCOMB1 = param.mRCOMB1 + temp;
                param.dLSAME = param.dRSAME + temp;
                param.mLAPF1 = param.mLAPF2 + temp;
                param.mRAPF1 = param.mRAPF2 + temp;
            }
        }
    }

    if (no_mask || (mask & SPU_REV_FEEDBACK)) {
        if (_spu_rev_attr.mode < 9) {
            if (_spu_rev_attr.mode >= 7) {
                bSetFeedback = 1;
                if (!bSetMode) {
                    if (!bSetDelay) {
                        _memcpy(&param, &_spu_rev_param[_spu_rev_attr.mode], sizeof(ReverbPreset));
                        param.flags = 0x80;
                    } else {
                        param.flags |= 0x80;
                    }
                }
                feedback = attr->feedback;
                _spu_rev_attr.feedback = feedback;
                param.vWALL = (feedback * 0x8100) / 127;
            }
        }
    }

    if (bSetMode) {
        bReverbWasEnabled = (_spu_RXX->rxx.spucnt) >> 7 & 1;
        if (bReverbWasEnabled) {
            spucnt = _spu_RXX->rxx.spucnt;
            spucnt &= 0xFF7F;
            _spu_RXX->rxx.spucnt = spucnt;
        }
    }

    if (!bSetMode) {
        if (no_mask || (mask & SPU_REV_DEPTHL)) {
            _spu_RXX->rxx.rev_vol.left = attr->depth.left;
            _spu_rev_attr.depth.left = attr->depth.left;
        }
        if (no_mask || (mask & SPU_REV_DEPTHR)) {
            _spu_RXX->rxx.rev_vol.right = attr->depth.right;
            _spu_rev_attr.depth.right = attr->depth.right;
        }
    } else {
        _spu_RXX->rxx.rev_vol.left = 0;
        _spu_RXX->rxx.rev_vol.right = 0;
        _spu_rev_attr.depth.left = 0;
        _spu_rev_attr.depth.right = 0;
    }

    if (bSetMode || bSetDelay || bSetFeedback) {
        _spu_setReverbAttr(&param);
    }

    if (clear_wa) {
        SpuClearReverbWorkArea(_spu_rev_attr.mode);
    }

    if (bSetMode) {
        _spu_FsetRXX(SPU_RXX_REV_WA_START_ADDR, _spu_rev_offsetaddr, 0);
        if (bReverbWasEnabled) {
            spucnt = _spu_RXX->rxx.spucnt;
            spucnt |= 0x80;
            _spu_RXX->rxx.spucnt = spucnt;
        }
    }

    return 0;
}

