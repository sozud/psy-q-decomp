#if VERSION <= 30
#define SPU_OLD_LOCAL_RXX
#define SPU_OLD_DEFER_RXX
#include "spu_old.h"

s32 _spu_rev_startaddr[0xA] = {
    0xFFF0, 0xFB28, 0xFC18, 0xF6F8, 0xF204,
    0xEA44, 0xE128, 0xCFF8, 0xCFF8, 0xF880,
};
s32 _spu_rev_workareasize[0xA] = {
    0x10, 0x4D8, 0x3D8, 0x908, 0xDFC, 0x15BC, 0x1ED8, 0x3008, 0x3008, 0x780
};
static ReverbPreset _spu_rev_param[0xA] = {
    {
        0,
        {
            0,      0,      0,      0,      0,      0,      0,      0,
            0,      0,      0,      0,      0,      0,      0,      0,
            0,      0,      0,      0,      0,      0,      0,      0,
            0,      0,      0,      0,      0,      0,      0,      0,
        },
    },
    {
        0,
        {
            0x7D,   0x5B,   0x6D80, 0x54B8, 0xBED0, 0,      0,      0xBA80,
            0x5800, 0x5300, 0x4D6,  0x333,  0x3F0,  0x227,  0x374,  0x1EF,
            0x334,  0x1B5,  0,      0,      0,      0,      0,      0,
            0,      0,      0x1B4,  0x136,  0xB8,   0x5C,   0x8000, 0x8000,
        },
    },
    {
        0,
        {
            0x33,   0x25,   0x70F0, 0x4FA8, 0xBCE0, 0x4410, -0x3F10, 0x9C00,
            0x5280, 0x4EC0, 0x3E4,  0x31B,  0x3A4,  0x2AF,  0x372,  0x266,
            0x31C,  0x25D,  0x25C,  0x18E,  0x22F,  0x135,  0x1D2,  0xB7,
            0x18F,  0xB5,   0xB4,   0x80,   0x4C,   0x26,   0x8000, 0x8000,
        },
    },
    {
        0,
        {
            0xB1,   0x7F,   0x70F0, 0x4FA8, 0xBCE0, 0x4510, 0xBEF0, 0xB4C0,
            0x5280, 0x4EC0, 0x904,  0x76B,  0x824,  0x65F,  0x7A2,  0x616,
            0x76C,  0x5ED,  0x5EC,  0x42E,  0x50F,  0x305,  0x462,  0x2B7,
            0x42F,  0x265,  0x264,  0x1B2,  0x100,  0x80,   0x8000, 0x8000,
        },
    },
    {
        0,
        {
            0xE3,   0xA9,   0x6F60, 0x4FA8, 0xBCE0, 0x4510, 0xBEF0, 0xA680,
            0x5680, 0x52C0, 0xDFB,  0xB58,  0xD09,  0xA3C,  0xBD9,  0x973,
            0xB59,  0x8DA,  0x8D9,  0x5E9,  0x7EC,  0x4B0,  0x6EF,  0x3D2,
            0x5EA,  0x31D,  0x31C,  0x238,  0x154,  0xAA,   0x8000, 0x8000,
        },
    },
    {
        0,
        {
            0x1A5,  0x139,  0x6000, 0x5000, 0x4C00, 0xB800, 0xBC00, 0xC000,
            0x6000, 0x5C00, 0x15BA, 0x11BB, 0x14C2, 0x10BD, 0x11BC, 0xDC1,
            0x11C0, 0xDC3,  0xDC0,  0x9C1,  0xBC4,  0x7C1,  0xA00,  0x6CD,
            0x9C2,  0x5C1,  0x5C0,  0x41A,  0x274,  0x13A,  0x8000, 0x8000,
        },
    },
    {
        0,
        {
            0x33D,  0x231,  0x7E00, 0x5000, 0xB400, 0xB000, 0x4C00, 0xB000,
            0x6000, 0x5400, 0x1ED6, 0x1A31, 0x1D14, 0x183B, 0x1BC2, 0x16B2,
            0x1A32, 0x15EF, 0x15EE, 0x1055, 0x1334, 0xF2D,  0x11F6, 0xC5D,
            0x1056, 0xAE1,  0xAE0,  0x7A2,  0x464,  0x232,  0x8000, 0x8000,
        },
    },
    {
        0,
        {
            1,      1,      0x7FFF, 0x7FFF, 0,      0,      0,      0x8100,
            0,      0,      0x1FFF, 0xFFF,  0x1005, 5,      0,      0,
            0x1005, 5,      0,      0,      0,      0,      0,      0,
            0,      0,      0x1004, 0x1002, 4,      2,      0x8000, 0x8000,
        },
    },
    {
        0,
        {
            1,      1,      0x7FFF, 0x7FFF, 0,      0,      0,      0,
            0,      0,      0x1FFF, 0xFFF,  0x1005, 5,      0,      0,
            0x1005, 5,      0,      0,      0,      0,      0,      0,
            0,      0,      0x1004, 0x1002, 4,      2,      0x8000, 0x8000,
        },
    },
    {
        0,
        {
            0x17,   0x13,   0x70F0, 0x4FA8, 0xBCE0, 0x4510, 0xBEF0, 0x8500,
            0x5F80, 0x54C0, 0x371,  0x2AF,  0x2E5,  0x1DF,  0x2B0,  0x1D7,
            0x358,  0x26A,  0x1D6,  0x11E,  0x12D,  0xB1,   0x11F,  0x59,
            0x1A0,  0xE3,   0x58,   0x40,   0x28,   0x14,   0x8000, 0x8000,
        },
    },
};
SPU_OLD_RXX_DEF;

static void _memcpy(char *dst, char *src, long size) {
    while (size--) {
        *dst++ = *src++;
    }
}

long SpuSetReverbModeParam(SpuReverbAttr *attr) {
    struct rev_param_entry entry;
    long mode;
    long rev_on;
    long off;
    long clear_wa;
    long mode_set;
    long delay_set;
    long feedback_set;
    unsigned long mask;
    long all;

    clear_wa = 0;
    mode_set = 0;
    mask = attr->mask;
    delay_set = 0;
    feedback_set = 0;
    all = mask == 0;
    entry.flags = 0;
    if (all || (mask & SPU_REV_MODE)) {
        mode = attr->mode;
        if (mode & SPU_REV_MODE_CLEAR_WA) {
            mode &= ~SPU_REV_MODE_CLEAR_WA;
            clear_wa = 1;
        }
        if ((unsigned long)mode >= 10 || _SpuIsInAllocateArea_(_spu_rev_startaddr[mode])) {
            return -1;
        }
        _spu_rev_attr.mode = mode;
        _spu_rev_offsetaddr = _spu_rev_startaddr[_spu_rev_attr.mode];
        mode_set = 1;
        if (clear_wa) {
            SpuClearReverbWorkArea(mode);
        }
        _memcpy((char *)&entry, (char *)&_spu_rev_param[_spu_rev_attr.mode], sizeof(struct rev_param_entry));
        switch (_spu_rev_attr.mode) {
        case SPU_REV_MODE_ECHO:
            _spu_rev_attr.feedback = 0xFF;
            _spu_rev_attr.delay = 0xFF;
            break;
        case SPU_REV_MODE_DELAY:
            _spu_rev_attr.feedback = 0;
            _spu_rev_attr.delay = 0xFF;
            break;
        default:
            _spu_rev_attr.feedback = 0;
            _spu_rev_attr.delay = 0;
            break;
        }
    }
    if (all || (mask & SPU_REV_DELAYTIME)) {
        switch (_spu_rev_attr.mode) {
        case SPU_REV_MODE_ECHO:
        case SPU_REV_MODE_DELAY:
            delay_set = 1;
            if (!mode_set) {
                _memcpy((char *)&entry, (char *)&_spu_rev_param[_spu_rev_attr.mode], sizeof(struct rev_param_entry));
                entry.flags = 0x0C011C00;
            }
            _spu_rev_attr.delay = attr->delay;
            entry.mLSAME = ((_spu_rev_attr.delay << 13) / 0x7F) - entry.dAPF1;
            entry.mRSAME = ((_spu_rev_attr.delay << 12) / 0x7F) - entry.dAPF2;
            entry.mLCOMB1 = ((_spu_rev_attr.delay << 12) / 0x7F) + entry.mRCOMB1;
            entry.dLSAME = ((_spu_rev_attr.delay << 12) / 0x7F) + entry.dRSAME;
            entry.mLAPF1 = ((_spu_rev_attr.delay << 12) / 0x7F) + entry.mLAPF2;
            entry.mRAPF1 = ((_spu_rev_attr.delay << 12) / 0x7F) + entry.mRAPF2;
            break;
        default:
            _spu_rev_attr.delay = 0;
            break;
        }
    }
    if (all || (mask & SPU_REV_FEEDBACK)) {
        switch (_spu_rev_attr.mode) {
        case SPU_REV_MODE_ECHO:
        case SPU_REV_MODE_DELAY:
            feedback_set = 1;
            if (!mode_set) {
                if (!delay_set) {
                    _memcpy((char *)&entry, (char *)&_spu_rev_param[_spu_rev_attr.mode], sizeof(struct rev_param_entry));
                    entry.flags = 0x80;
                } else {
                    entry.flags |= 0x80;
                }
            }
            _spu_rev_attr.feedback = attr->feedback;
            entry.vWALL = (_spu_rev_attr.feedback * 0x8100) / 0x7F;
            break;
        default:
            _spu_rev_attr.feedback = 0;
            break;
        }
    }
    if (mode_set) {
        _spu_ioctl(_spu_fd, 0x6A, &rev_on);
        if (rev_on) {
            off = 0;
            _spu_ioctl(_spu_fd, 0x60, &off);
        }
    }
    if (!mode_set) {
        if (all || (mask & SPU_REV_DEPTHL)) {
            _spu_RXX->rxx.rev_vol.left = attr->depth.left;
            _spu_rev_attr.depth.left = attr->depth.left;
        }
        if (all || (mask & SPU_REV_DEPTHR)) {
            _spu_RXX->rxx.rev_vol.right = attr->depth.right;
            _spu_rev_attr.depth.right = attr->depth.right;
        }
    } else {
        _spu_RXX->rxx.rev_vol.left = 0;
        _spu_RXX->rxx.rev_vol.right = 0;
        _spu_rev_attr.depth.left = 0;
        _spu_rev_attr.depth.right = 0;
    }
    if (mode_set || delay_set || feedback_set) {
        _spu_ioctl(_spu_fd, 0x63, &entry);
    }
    if (mode_set) {
        _spu_ioctl(_spu_fd, 0x61, &_spu_rev_offsetaddr);
        if (rev_on) {
            _spu_ioctl(_spu_fd, 0x60, &rev_on);
        }
    }
    return 0;
}
#else
#if VERSION < 40
// SPDX-License-Identifier: MIT
#include "libspu_i.h"
typedef unsigned int size_t;
extern SpuReverbAttr _spu_rev_attr;

extern struct rev_param_entry _spu_rev_param[];
static void _spu_setReverbAttr(struct rev_param_entry* arg0);

static inline void _memcpy(char* dst, char* src, size_t size) {
    while (size--) {
        *dst++ = *src++;
    }
}

#define SPU_REV_ATTR_DAPF1 (0x01 << 0)
#define SPU_REV_ATTR_DAPF2 (0x01 << 1)
#define SPU_REV_ATTR_VIIR (0x01 << 2)
#define SPU_REV_ATTR_VCOMB1 (0x01 << 3)
#define SPU_REV_ATTR_VCOMB2 (0x01 << 4)
#define SPU_REV_ATTR_VCOMB3 (0x01 << 5)
#define SPU_REV_ATTR_VCOMB4 (0x01 << 6)
#define SPU_REV_ATTR_VWALL (0x01 << 7)
#define SPU_REV_ATTR_VAPF1 (0x01 << 8)
#define SPU_REV_ATTR_VAPF2 (0x01 << 9)
#define SPU_REV_ATTR_MLSAME (0x01 << 10)
#define SPU_REV_ATTR_MRSAME (0x01 << 11)
#define SPU_REV_ATTR_MLCOMB1 (0x01 << 12)
#define SPU_REV_ATTR_MRCOMB1 (0x01 << 13)
#define SPU_REV_ATTR_MLCOMB2 (0x01 << 14)
#define SPU_REV_ATTR_MRCOMB2 (0x01 << 15)
#define SPU_REV_ATTR_DLSAME (0x01 << 16)
#define SPU_REV_ATTR_DRSAME (0x01 << 17)
#define SPU_REV_ATTR_MLDIFF (0x01 << 18)
#define SPU_REV_ATTR_MRDIFF (0x01 << 19)
#define SPU_REV_ATTR_MLCOMB3 (0x01 << 20)
#define SPU_REV_ATTR_MRCOMB3 (0x01 << 21)
#define SPU_REV_ATTR_MLCOMB4 (0x01 << 22)
#define SPU_REV_ATTR_MRCOMB4 (0x01 << 23)
#define SPU_REV_ATTR_DLDIFF (0x01 << 24)
#define SPU_REV_ATTR_DRDIFF (0x01 << 25)
#define SPU_REV_ATTR_MLAPF1 (0x01 << 26)
#define SPU_REV_ATTR_MRAPF1 (0x01 << 27)
#define SPU_REV_ATTR_MLAPF2 (0x01 << 28)
#define SPU_REV_ATTR_MRAPF2 (0x01 << 29)
#define SPU_REV_ATTR_VLIN (0x01 << 30)
#define SPU_REV_ATTR_VRIN (0x01 << 31)

long SpuSetReverbModeParam(SpuReverbAttr* attr) {
    struct rev_param_entry entry;
    u32 var_s0;
    u16 cnt;

#if VERSION == 33
    bool sp58 = false;
    bool var_s7;
    bool var_s4 = false;
    bool var_s6 = false;
    bool var_fp = false;
#else
    bool var_s7 = false;
    bool var_s4 = false;
    bool var_s6 = false;
    bool sp58 = false;
    bool var_fp = false;
#endif

    u32 mask = attr->mask;
    bool bSetAll = attr->mask == 0;

    entry.flags = 0;
    if (bSetAll || mask & SPU_REV_MODE) {
        var_s0 = attr->mode;
        if (attr->mode & SPU_REV_MODE_CLEAR_WA) {
            var_s0 &= ~SPU_REV_MODE_CLEAR_WA;
            sp58 = true;
        }
        if (var_s0 >= 0xA ||
            _SpuIsInAllocateArea_(_spu_rev_startaddr[var_s0])) {
            return -1;
        }
        var_s4 = true;
        _spu_rev_attr.mode = var_s0;
        _spu_rev_offsetaddr = _spu_rev_startaddr[_spu_rev_attr.mode];
        _memcpy(&entry, &_spu_rev_param[_spu_rev_attr.mode],
                sizeof(struct rev_param_entry));
        switch (_spu_rev_attr.mode) {
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
    }
    if (bSetAll || mask & SPU_REV_DELAYTIME) {
        switch (_spu_rev_attr.mode) {
        case SPU_REV_MODE_ECHO:
        case SPU_REV_MODE_DELAY:
            var_s6 = true;
            if (!var_s4) {
                _memcpy(&entry, &_spu_rev_param[_spu_rev_attr.mode],
                        sizeof(struct rev_param_entry));
                entry.flags = SPU_REV_ATTR_MLSAME | SPU_REV_ATTR_MRSAME |
                              SPU_REV_ATTR_MLCOMB1 | SPU_REV_ATTR_DLSAME |
                              SPU_REV_ATTR_MLAPF1 | SPU_REV_ATTR_MRAPF1;
            }
            _spu_rev_attr.delay = attr->delay;
            entry.mLSAME = ((_spu_rev_attr.delay << 0xD) / 0x7F) - entry.dAPF1;
            entry.mRSAME = ((_spu_rev_attr.delay << 0xC) / 0x7F) - entry.dAPF2;
            entry.mLCOMB1 =
                ((_spu_rev_attr.delay << 0xC) / 0x7F) + entry.mRCOMB1;
            entry.dLSAME = ((_spu_rev_attr.delay << 0xC) / 0x7F) + entry.dRSAME;
            entry.mLAPF1 = ((_spu_rev_attr.delay << 0xC) / 0x7F) + entry.mLAPF2;
            entry.mRAPF1 = ((_spu_rev_attr.delay << 0xC) / 0x7F) + entry.mRAPF2;
            break;
        default:
            _spu_rev_attr.delay = 0;
            break;
        }
    }
    if (bSetAll || mask & SPU_REV_FEEDBACK) {
        switch (_spu_rev_attr.mode) {
        case SPU_REV_MODE_ECHO:
        case SPU_REV_MODE_DELAY:
            var_fp = true;
            if (!var_s4) {
                if (!var_s6) {
                    _memcpy(&entry, &_spu_rev_param[_spu_rev_attr.mode],
                            sizeof(struct rev_param_entry));
                    entry.flags = SPU_REV_ATTR_VWALL;
                } else {
                    entry.flags |= SPU_REV_ATTR_VWALL;
                }
            }
            _spu_rev_attr.feedback = attr->feedback;
            entry.vWALL = (_spu_rev_attr.feedback * 0x8100) / 0x7F;
            break;
        default:
            _spu_rev_attr.feedback = 0;
            break;
        }
    }
    if (var_s4) {
        var_s7 = (_spu_RXX->rxx.spucnt >> 7) & 1;
        if (var_s7) {
            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~0x80;
            _spu_RXX->rxx.spucnt = cnt;
        }
    }
    if (!var_s4) {
        if (bSetAll || mask & SPU_REV_DEPTHL) {
            _spu_RXX->rxx.rev_vol.left = attr->depth.left;
            _spu_rev_attr.depth.left = attr->depth.left;
        }
        if (bSetAll || mask & SPU_REV_DEPTHR) {
            _spu_RXX->rxx.rev_vol.right = attr->depth.right;
            _spu_rev_attr.depth.right = attr->depth.right;
        }
    } else {
        _spu_RXX->rxx.rev_vol.left = 0;
        _spu_RXX->rxx.rev_vol.right = 0;
        _spu_rev_attr.depth.left = 0;
        _spu_rev_attr.depth.right = 0;
    }
    if (var_s4 || var_s6 || var_fp) {
        _spu_setReverbAttr(&entry);
    }
    if (sp58) {
        SpuClearReverbWorkArea(_spu_rev_attr.mode);
    }
    if (var_s4) {
        _spu_FsetRXX(0xD1, _spu_rev_offsetaddr, 0);
        if (var_s7) {
            cnt = _spu_RXX->rxx.spucnt;
            cnt |= 0x80;
            _spu_RXX->rxx.spucnt = cnt;
        }
    }
    return 0;
}

static void _spu_setReverbAttr(struct rev_param_entry* arg0) {
    u32 mask = arg0->flags;
    bool bSetAll = arg0->flags == 0;

    if (bSetAll || mask & SPU_REV_ATTR_DAPF1) {
        _spu_RXX->rxx.dAPF1 = arg0->dAPF1;
    }
    if (bSetAll || mask & SPU_REV_ATTR_DAPF2) {
        _spu_RXX->rxx.dAPF2 = arg0->dAPF2;
    }
    if (bSetAll || mask & SPU_REV_ATTR_VIIR) {
        _spu_RXX->rxx.vIIR = arg0->vIIR;
    }
    if (bSetAll || mask & SPU_REV_ATTR_VCOMB1) {
        _spu_RXX->rxx.vCOMB1 = arg0->vCOMB1;
    }
    if (bSetAll || mask & SPU_REV_ATTR_VCOMB2) {
        _spu_RXX->rxx.vCOMB2 = arg0->vCOMB2;
    }
    if (bSetAll || mask & SPU_REV_ATTR_VCOMB3) {
        _spu_RXX->rxx.vCOMB3 = arg0->vCOMB3;
    }
    if (bSetAll || mask & SPU_REV_ATTR_VCOMB4) {
        _spu_RXX->rxx.vCOMB4 = arg0->vCOMB4;
    }
    if (bSetAll || mask & SPU_REV_ATTR_VWALL) {
        _spu_RXX->rxx.vWALL = arg0->vWALL;
    }
    if (bSetAll || mask & SPU_REV_ATTR_VAPF1) {
        _spu_RXX->rxx.vAPF1 = arg0->vAPF1;
    }
    if (bSetAll || mask & SPU_REV_ATTR_VAPF2) {
        _spu_RXX->rxx.vAPF2 = arg0->vAPF2;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MLSAME) {
        _spu_RXX->rxx.mLSAME = arg0->mLSAME;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MRSAME) {
        _spu_RXX->rxx.mRSAME = arg0->mRSAME;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MLCOMB1) {
        _spu_RXX->rxx.mLCOMB1 = arg0->mLCOMB1;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MRCOMB1) {
        _spu_RXX->rxx.mRCOMB1 = arg0->mRCOMB1;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MLCOMB2) {
        _spu_RXX->rxx.mLCOMB2 = arg0->mLCOMB2;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MRCOMB2) {
        _spu_RXX->rxx.mRCOMB2 = arg0->mRCOMB2;
    }
    if (bSetAll || mask & SPU_REV_ATTR_DLSAME) {
        _spu_RXX->rxx.dLSAME = arg0->dLSAME;
    }
    if (bSetAll || mask & SPU_REV_ATTR_DRSAME) {
        _spu_RXX->rxx.dRSAME = arg0->dRSAME;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MLDIFF) {
        _spu_RXX->rxx.mLDIFF = arg0->mLDIFF;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MRDIFF) {
        _spu_RXX->rxx.mRDIFF = arg0->mRDIFF;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MLCOMB3) {
        _spu_RXX->rxx.mLCOMB3 = arg0->mLCOMB3;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MRCOMB3) {
        _spu_RXX->rxx.mRCOMB3 = arg0->mRCOMB3;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MLCOMB4) {
        _spu_RXX->rxx.mLCOMB4 = arg0->mLCOMB4;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MRCOMB4) {
        _spu_RXX->rxx.mRCOMB4 = arg0->mRCOMB4;
    }
    if (bSetAll || mask & SPU_REV_ATTR_DLDIFF) {
        _spu_RXX->rxx.dLDIFF = arg0->dLDIFF;
    }
    if (bSetAll || mask & SPU_REV_ATTR_DRDIFF) {
        _spu_RXX->rxx.dRDIFF = arg0->dRDIFF;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MLAPF1) {
        _spu_RXX->rxx.mLAPF1 = arg0->mLAPF1;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MRAPF1) {
        _spu_RXX->rxx.mRAPF1 = arg0->mRAPF1;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MLAPF2) {
        _spu_RXX->rxx.mLAPF2 = arg0->mLAPF2;
    }
    if (bSetAll || mask & SPU_REV_ATTR_MRAPF2) {
        _spu_RXX->rxx.mRAPF2 = arg0->mRAPF2;
    }
    if (bSetAll || mask & SPU_REV_ATTR_VLIN) {
        _spu_RXX->rxx.vLIN = arg0->vLIN;
    }
    if (bSetAll || mask & SPU_REV_ATTR_VRIN) {
        _spu_RXX->rxx.vRIN = arg0->vRIN;
    }
}

#else
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


#endif


#endif
