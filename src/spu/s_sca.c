#if VERSION <= 33

#if VERSION <= 30
#define SPU_OLD_LOCAL_RXX
#endif
#include "spu_old.h"

int SpuCommonError;

void SpuSetCommonAttr(SpuCommonAttr* attr) {
    u16 mvol_mode_left;
    u16 mvol_mode_right;
    u16 vol_total_left;
    u16 vol_total_right;
    u32 mask;
    s32 bSetAll;
    u16 cnt;

    mask = attr->mask;
    bSetAll = attr->mask == 0;

    if (bSetAll || mask & SPU_COMMON_MVOLL) {
        if (bSetAll || mask & SPU_COMMON_MVOLMODEL) {
            switch ((s16)attr->mvolmode.left) {
            case 1:
                mvol_mode_left = 0x8000;
                break;
            case 2:
                mvol_mode_left = 0x9000;
                break;
            case 3:
                mvol_mode_left = 0xA000;
                break;
            case 4:
                mvol_mode_left = 0xB000;
                break;
            case 5:
                mvol_mode_left = 0xC000;
                break;
            case 6:
                mvol_mode_left = 0xD000;
                break;
            case 7:
                mvol_mode_left = 0xE000;
                break;
            case 0:
                vol_total_left = attr->mvol.left;
                mvol_mode_left = 0;
                break;
            default:
                vol_total_left = attr->mvol.left;
                mvol_mode_left = 0;
                break;
            }
        } else {
            vol_total_left = attr->mvol.left;
            mvol_mode_left = 0;
        }

        if (mvol_mode_left != 0) {
            if ((s16)attr->mvol.left >= 0x80) {
                vol_total_left = 0x7F;
            } else if ((s16)attr->mvol.left < 0) {
                vol_total_left = 0;
            } else {
                vol_total_left = attr->mvol.left;
            }
        }
        vol_total_left &= 0x7FFF;
        _spu_RXX->rxx.main_vol.left = vol_total_left | mvol_mode_left;
    }

    if (bSetAll || mask & SPU_COMMON_MVOLR) {
        if (bSetAll || mask & SPU_COMMON_MVOLMODER) {
            switch ((s16)attr->mvolmode.right) {
            case 1:
                mvol_mode_right = 0x8000;
                break;
            case 2:
                mvol_mode_right = 0x9000;
                break;
            case 3:
                mvol_mode_right = 0xA000;
                break;
            case 4:
                mvol_mode_right = 0xB000;
                break;
            case 5:
                mvol_mode_right = 0xC000;
                break;
            case 6:
                mvol_mode_right = 0xD000;
                break;
            case 7:
                mvol_mode_right = 0xE000;
                break;
            case 0:
                vol_total_right = attr->mvol.right;
                mvol_mode_right = 0;
                break;
            default:
                vol_total_right = attr->mvol.right;
                mvol_mode_right = 0;
                break;
            }
        } else {
            vol_total_right = attr->mvol.right;
            mvol_mode_right = 0;
        }

        if (mvol_mode_right != 0) {
            if ((s16)attr->mvol.right >= 0x80) {
                vol_total_right = 0x7F;
            } else if ((s16)attr->mvol.right < 0) {
                vol_total_right = 0;
            } else {
                vol_total_right = attr->mvol.right;
            }
        }
        vol_total_right &= 0x7FFF;
        _spu_RXX->rxx.main_vol.right = vol_total_right | mvol_mode_right;
    }

    if (bSetAll || mask & SPU_COMMON_CDVOLL) {
        _spu_RXX->rxx.cd_vol.left = attr->cd.volume.left;
    }

    if (bSetAll || mask & SPU_COMMON_CDVOLR) {
        _spu_RXX->rxx.cd_vol.right = attr->cd.volume.right;
    }

    if (bSetAll || mask & SPU_COMMON_CDREV) {
        if (attr->cd.reverb == 0) {
            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~4;
            _spu_RXX->rxx.spucnt = cnt;
        } else {
            cnt = _spu_RXX->rxx.spucnt;
            cnt |= 4;
            _spu_RXX->rxx.spucnt = cnt;
        }
    }

    if (bSetAll || mask & SPU_COMMON_CDMIX) {
        if (attr->cd.mix == 0) {
            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~1;
            _spu_RXX->rxx.spucnt = cnt;
        } else {
            cnt = _spu_RXX->rxx.spucnt;
            cnt |= 1;
            _spu_RXX->rxx.spucnt = cnt;
        }
    }

    if (bSetAll || mask & SPU_COMMON_EXTVOLL) {
        _spu_RXX->rxx.ex_vol.left = attr->ext.volume.left;
    }

    if (bSetAll || mask & SPU_COMMON_EXTVOLR) {
        _spu_RXX->rxx.ex_vol.right = attr->ext.volume.right;
    }

    if (bSetAll || mask & SPU_COMMON_EXTREV) {
        if (attr->ext.reverb == 0) {
            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~8;
            _spu_RXX->rxx.spucnt = cnt;
        } else {
            cnt = _spu_RXX->rxx.spucnt;
            cnt |= 8;
            _spu_RXX->rxx.spucnt = cnt;
        }
    }

    if (bSetAll || mask & SPU_COMMON_EXTMIX) {
        if (attr->ext.mix == 0) {
            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~2;
            _spu_RXX->rxx.spucnt = cnt;
        } else {
            cnt = _spu_RXX->rxx.spucnt;
            cnt |= 2;
            _spu_RXX->rxx.spucnt = cnt;
        }
    }
}
#else
// SPDX-License-Identifier: MIT
#if VERSION >= 36
#if VERSION == 36
#include "libspu_40.h"
#else
#include "libspu_i.h"
#endif
int SpuCommonError;
void SpuSetCommonAttr(SpuCommonAttr* attr) {
    u16 mvol_mode_left;
    u16 mvol_mode_right;
    u16 vol_total_left;
    u16 vol_total_right;
    u32 mask;
    s32 bSetAll;
    u16 cnt;

    vol_total_left = 0;
    vol_total_right = 0;
    mask = attr->mask;
    bSetAll = attr->mask == 0;

    if (bSetAll != 0 || mask & SPU_COMMON_MVOLL) {
        if (bSetAll != 0 || mask & SPU_COMMON_MVOLMODEL) {
            switch (attr->mvolmode.left) {

            case 1:
                mvol_mode_left = 0x8000;
                break;
            case 2:
                mvol_mode_left = 0x9000;
                break;
            case 3:
                mvol_mode_left = 0xA000;
                break;
            case 4:
                mvol_mode_left = 0xB000;
                break;
            case 5:
                mvol_mode_left = 0xC000;
                break;
            case 6:
                mvol_mode_left = 0xD000;
                break;
            case 7:
                mvol_mode_left = 0xE000;
                break;
            case 0:
                vol_total_left = attr->mvol.left;
                mvol_mode_left = 0;
                break;
            default:
                vol_total_left = attr->mvol.left;
                mvol_mode_left = 0;
                break;
            }
        }
        else {
            vol_total_left = attr->mvol.left;
            mvol_mode_left = 0;
        }

        if (mvol_mode_left != 0) {
            if (attr->mvol.left >= 0x80) {
                vol_total_left = 0x7F;
            } else if (attr->mvol.left < 0) {
                vol_total_left = 0;
            } else {
                vol_total_left = attr->mvol.left;
            }
        }
        vol_total_left &= 0x7FFF;
        _spu_RXX->rxx.main_vol.left = (vol_total_left) | mvol_mode_left;
    }

    if (bSetAll != 0 || mask & 2) {
        if (bSetAll != 0 || mask & 8) {
            switch (attr->mvolmode.right) {
            case 1:
                mvol_mode_right = 0x8000;
                break;
            case 2:
                mvol_mode_right = 0x9000;
                break;
            case 3:
                mvol_mode_right = 0xA000;
                break;
            case 4:
                mvol_mode_right = 0xB000;
                break;
            case 5:
                mvol_mode_right = 0xC000;
                break;
            case 6:
                mvol_mode_right = 0xD000;
                break;
            case 7:
                mvol_mode_right = 0xE000;
                break;
            case 0:
                vol_total_right = attr->mvol.right;
                mvol_mode_right = 0;
                break;
            default:
                vol_total_right = attr->mvol.right;
                mvol_mode_right = 0;
                break;
            }
        } else {
            vol_total_right = attr->mvol.right;
            mvol_mode_right = 0;
        }

        if (mvol_mode_right != 0) {
            if (attr->mvol.right >= 0x80) {
                vol_total_right = 0x7F;
            } else if (attr->mvol.right < 0) {
                vol_total_right = 0;
            } else {
                vol_total_right = attr->mvol.right;
            }
        }
        vol_total_right &= 0x7FFF;
        _spu_RXX->rxx.main_vol.right = vol_total_right | mvol_mode_right;
    }

    if ((bSetAll != 0) || (mask & 0x40)) {
        _spu_RXX->rxx.cd_vol.left = attr->cd.volume.left;
    }

    if ((bSetAll != 0) || (mask & 0x80)) {
        _spu_RXX->rxx.cd_vol.right = attr->cd.volume.right;
    }

    if ((bSetAll != 0) || (mask & 0x400)) {
        _spu_RXX->rxx.ex_vol.left = attr->ext.volume.left;
    }

    if ((bSetAll != 0) || (mask & 0x800)) {
        _spu_RXX->rxx.ex_vol.right = attr->ext.volume.right;
    }

    if ((bSetAll != 0) || (mask & 0x100)) {
        if (attr->cd.reverb == 0) {
            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~4;
            _spu_RXX->rxx.spucnt = cnt;
        } else {
            cnt = _spu_RXX->rxx.spucnt;
            cnt |= 4;
            _spu_RXX->rxx.spucnt = cnt;
        }
    }

    if ((bSetAll != 0) || (mask & 0x200)) {
        if (attr->cd.mix == 0) {
            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~1;
            _spu_RXX->rxx.spucnt = cnt;
        } else {
            cnt = _spu_RXX->rxx.spucnt;
            cnt |= 1;
            _spu_RXX->rxx.spucnt = cnt;
        }
    }

    if ((bSetAll != 0) || (mask & 0x1000)) {
        if (attr->ext.reverb == 0) {
            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~8;
            _spu_RXX->rxx.spucnt = cnt;
        } else {
            cnt = _spu_RXX->rxx.spucnt;
            cnt |= 8;
            _spu_RXX->rxx.spucnt = cnt;
        }
    }

    if ((bSetAll != 0) || (mask & 0x2000)) {
        if (attr->ext.mix == 0) {
            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~2;
            _spu_RXX->rxx.spucnt = cnt;
        } else {
            cnt = _spu_RXX->rxx.spucnt;
            cnt |= 2;
            _spu_RXX->rxx.spucnt = cnt;
        }
    }
}
#else
#include "libspu_i.h"

int SpuCommonError;

void SpuSetCommonAttr(SpuCommonAttr* attr) {
    u16 mvol_mode_left;
    u16 mvol_mode_right;
    u16 vol_total_left;
    u16 vol_total_right;
    u32 mask;
    s32 bSetAll;
    u16 cnt;

    vol_total_left = 0;
    vol_total_right = 0;
    mask = attr->mask;
    bSetAll = attr->mask == 0;

    if (bSetAll || mask & SPU_COMMON_MVOLL) {
        if (bSetAll || mask & SPU_COMMON_MVOLMODEL) {
            switch ((s16)attr->mvolmode.left) {
            case 1:
                mvol_mode_left = 0x8000;
                break;
            case 2:
                mvol_mode_left = 0x9000;
                break;
            case 3:
                mvol_mode_left = 0xA000;
                break;
            case 4:
                mvol_mode_left = 0xB000;
                break;
            case 5:
                mvol_mode_left = 0xC000;
                break;
            case 6:
                mvol_mode_left = 0xD000;
                break;
            case 7:
                mvol_mode_left = 0xE000;
                break;
            case 0:
                vol_total_left = attr->mvol.left;
                mvol_mode_left = 0;
                break;
            default:
                vol_total_left = attr->mvol.left;
                mvol_mode_left = 0;
                break;
            }
        } else {
            vol_total_left = attr->mvol.left;
            mvol_mode_left = 0;
        }

        if (mvol_mode_left != 0) {
            if ((s16)attr->mvol.left >= 0x80) {
                vol_total_left = 0x7F;
            } else if ((s16)attr->mvol.left < 0) {
                vol_total_left = 0;
            } else {
                vol_total_left = attr->mvol.left;
            }
        }
        vol_total_left &= 0x7FFF;
        _spu_RXX->rxx.main_vol.left = vol_total_left | mvol_mode_left;
    }

    if (bSetAll || mask & SPU_COMMON_MVOLR) {
        if (bSetAll || mask & SPU_COMMON_MVOLMODER) {
            switch ((s16)attr->mvolmode.right) {
            case 1:
                mvol_mode_right = 0x8000;
                break;
            case 2:
                mvol_mode_right = 0x9000;
                break;
            case 3:
                mvol_mode_right = 0xA000;
                break;
            case 4:
                mvol_mode_right = 0xB000;
                break;
            case 5:
                mvol_mode_right = 0xC000;
                break;
            case 6:
                mvol_mode_right = 0xD000;
                break;
            case 7:
                mvol_mode_right = 0xE000;
                break;
            case 0:
                vol_total_right = attr->mvol.right;
                mvol_mode_right = 0;
                break;
            default:
                vol_total_right = attr->mvol.right;
                mvol_mode_right = 0;
                break;
            }
        } else {
            vol_total_right = attr->mvol.right;
            mvol_mode_right = 0;
        }

        if (mvol_mode_right != 0) {
            if ((s16)attr->mvol.right >= 0x80) {
                vol_total_right = 0x7F;
            } else if ((s16)attr->mvol.right < 0) {
                vol_total_right = 0;
            } else {
                vol_total_right = attr->mvol.right;
            }
        }
        vol_total_right &= 0x7FFF;
        _spu_RXX->rxx.main_vol.right = vol_total_right | mvol_mode_right;
    }

    if (bSetAll || mask & SPU_COMMON_CDVOLL) {
        _spu_RXX->rxx.cd_vol.left = attr->cd.volume.left;
    }

    if (bSetAll || mask & SPU_COMMON_CDVOLR) {
        _spu_RXX->rxx.cd_vol.right = attr->cd.volume.right;
    }

    if (bSetAll || mask & SPU_COMMON_CDREV) {
        if (attr->cd.reverb == 0) {
            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~4;
            _spu_RXX->rxx.spucnt = cnt;
        } else {
            cnt = _spu_RXX->rxx.spucnt;
            cnt |= 4;
            _spu_RXX->rxx.spucnt = cnt;
        }
    }

    if (bSetAll || mask & SPU_COMMON_CDMIX) {
        if (attr->cd.mix == 0) {
            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~1;
            _spu_RXX->rxx.spucnt = cnt;
        } else {
            cnt = _spu_RXX->rxx.spucnt;
            cnt |= 1;
            _spu_RXX->rxx.spucnt = cnt;
        }
    }

    if (bSetAll || mask & SPU_COMMON_EXTVOLL) {
        _spu_RXX->rxx.ex_vol.left = attr->ext.volume.left;
    }

    if (bSetAll || mask & SPU_COMMON_EXTVOLR) {
        _spu_RXX->rxx.ex_vol.right = attr->ext.volume.right;
    }

    if (bSetAll || mask & SPU_COMMON_EXTREV) {
        if (attr->ext.reverb == 0) {
            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~8;
            _spu_RXX->rxx.spucnt = cnt;
        } else {
            cnt = _spu_RXX->rxx.spucnt;
            cnt |= 8;
            _spu_RXX->rxx.spucnt = cnt;
        }
    }

    if (bSetAll || mask & SPU_COMMON_EXTMIX) {
        if (attr->ext.mix == 0) {
            cnt = _spu_RXX->rxx.spucnt;
            cnt &= ~2;
            _spu_RXX->rxx.spucnt = cnt;
        } else {
            cnt = _spu_RXX->rxx.spucnt;
            cnt |= 2;
            _spu_RXX->rxx.spucnt = cnt;
        }
    }
}
#endif
#endif
