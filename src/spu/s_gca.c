#include "libspu_i.h"
void SpuGetCommonAttr(SpuCommonAttr *attr) {
    u16 var_a1;
    u16 var_a2;
    u16 var_a3;
    u16 var_t0;

    var_a3 = _spu_RXX->rxx.main_vol.left;
    var_t0 = _spu_RXX->rxx.main_vol.right;
    var_a1 = var_a2 = 0;

    if ((var_a3 & 0x8000) == 0) {
        var_a2 = 0;
    } else {
        switch(var_a3 & 0xF000) {
            case 0x8000:
                var_a2 = 1;
            break;
            case 0x9000:
                var_a2 = 2;
            break;
            case 0xA000:
                var_a2 = 3;
            break;
            case 0xB000:
                var_a2 = 4;
            break;
            case 0xC000:
                var_a2 = 5;
            break;
            case 0xD000:
                var_a2 = 6;
            break;
            case 0xE000:
            case 0xF000:
                var_a2 = 7;
            break;
        }

        var_a3 &= 0xFFF;
    }

    if ((var_t0 & 0x8000) == 0) {
        var_a1 = 0;
    } else {
        switch(var_t0 & 0xF000) {
            case 0x8000:
                var_a1 = 1;
            break;
            case 0x9000:
                var_a1 = 2;
            break;
            case 0xA000:
                var_a1 = 3;
            break;
            case 0xB000:
                var_a1 = 4;
            break;
            case 0xC000:
                var_a1 = 5;
            break;
            case 0xD000:
                var_a1 = 6;
            break;
            case 0xE000:
            case 0xF000:
                var_a1 = 7;
            break;
        }

        var_t0 &= 0xFFF;
    }




    {
        u32 x = 0x8000;
        u32 y = var_a3;

        if (y >= 0x4000) {
            attr->mvol.left = y - x;
        } else {
            attr->mvol.left = var_a3;
        }
    }

    {
        u32 x = 0x8000;
        u32 y = var_t0;

        if (y >= 0x4000) {
            attr->mvol.right = y - x;
        } else {
            attr->mvol.right = var_t0;
        }
    }

    attr->mvolmode.left = var_a2;
    attr->mvolmode.right = var_a1;
    attr->mvolx.left = _spu_RXX->rxx.main_volx.left;
    attr->mvolx.right = _spu_RXX->rxx.main_volx.right;
    attr->cd.volume.left = _spu_RXX->rxx.cd_vol.left;
    attr->cd.volume.right = _spu_RXX->rxx.cd_vol.right;

    if (_spu_RXX->rxx.spucnt & 4) {
        attr->cd.reverb = 1;
    } else {
        attr->cd.reverb = 0;
    }

    if (_spu_RXX->rxx.spucnt & 1) {
        attr->cd.mix = 1;
    } else {
        attr->cd.mix = 0;
    }

    attr->ext.volume.left = _spu_RXX->rxx.ex_vol.left;
    attr->ext.volume.right = _spu_RXX->rxx.ex_vol.right;

    if (_spu_RXX->rxx.spucnt & 8) {
        attr->ext.reverb = 1;
    } else {
        attr->ext.reverb = 0;
    }

    if (_spu_RXX->rxx.spucnt & 2) {
        attr->ext.mix = 1;
    } else {
        attr->ext.mix = 0;
    }
}

