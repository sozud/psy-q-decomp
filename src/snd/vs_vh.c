#if VERSION >= 36
#include "libsnd_i.h"

static short SsVabOpenHeadWithMode(
    u_char* addr, short vabid, short mode, u_long sbaddr);

short SsVabOpenHead(u_char* addr, short vabid) {
    return SsVabOpenHeadWithMode(addr, vabid, 0, 0);
}

short SsVabOpenHeadSticky(u_char* addr, short vabid, u_long sbaddr) {
    return SsVabOpenHeadWithMode(addr, vabid, 1, sbaddr);
}

short SsVabFakeHead(u_char* addr, short vabid, u_long sbaddr) {
    return SsVabOpenHeadWithMode(addr, vabid, 1, sbaddr);
}

static short SsVabOpenHeadWithMode(
    u_char* addr, short vabid, short mode, u_long sbaddr) {
    int vagLens[256];
    int i;
    int size;
    short vab_id;
    unsigned short vag_header_len;
    unsigned short* ptr_vag_off_table;
    unsigned int magic;
    unsigned spuAllocMem;
    unsigned char num_vags;
    unsigned char* ptr;
    ProgAtr* progAtr;
    VabHdr* vab_header;

    vab_id = NUM_VAB;
    if (_spu_getInTransfer() == 1) {
        return -1;
    }
    _spu_setInTransfer(1);
    if (vabid >= NUM_VAB) {
        _spu_setInTransfer(0);
        return -1;
    }
    if (vabid == -1) {
        for (i = 0; i < NUM_VAB; i++) {
            if (_svm_vab_used[i] == 0) {
                _svm_vab_used[i] = 1;
                vab_id = i;
                _svm_vab_count++;
                break;
            }
        }
    } else {
        ptr = _svm_vab_used;
        if (ptr[vabid] == 0) {
            _svm_vab_used[vabid] = 1;
            vab_id = vabid;
            _svm_vab_count++;
        }
    }
    if (vab_id >= 0x10) {
        _spu_setInTransfer(0);
        return -1;
    }

    ptr = addr;
    _svm_vab_vh[vab_id] = (VabHdr*)ptr;
    ptr += sizeof(VabHdr);
    vab_header = (VabHdr*)addr;
    magic = vab_header->form;
    if ((magic >> 8) != ('V' << 0x10 | 'A' << 0x8 | 'B')) {
        _svm_vab_used[vab_id] = 0;
        _spu_setInTransfer(0);
        _svm_vab_count--;
        return -1;
    }
    if ((magic & 0xFF) == 'p') {
        if (vab_header->ver >= 5) {
            kMaxPrograms = 0x80;
        } else {
            kMaxPrograms = 0x40;
        }
    } else {
        kMaxPrograms = 0x40;
    }
    if (vab_header->ps > kMaxPrograms) {
        _svm_vab_used[vab_id] = 0;
        _spu_setInTransfer(0);
        _svm_vab_count--;
        return -1;
    }

    _svm_vab_pg[vab_id] = (ProgAtr*)ptr;
    progAtr = (ProgAtr*)ptr;
    ptr += kMaxPrograms * 0x10;
    size = 0;
    for (i = 0; i < kMaxPrograms; i++) {
        progAtr[i].reserved1 = size;
        if (progAtr[i].tones != 0) {
            size++;
        }
    }
    size = 0;
    _svm_vab_tn[vab_id] = (VagAtr*)ptr;
    ptr_vag_off_table = (u16*)(ptr + vab_header->ps * 512);
    num_vags = vab_header->vs;
    for (i = 0; i < LEN(vagLens); i++) {
        if (num_vags >= i) {
            vag_header_len = *ptr_vag_off_table;
            if (vab_header->ver >= 5) {
                vagLens[i] = vag_header_len * 8;
            } else {
                vagLens[i] = vag_header_len * 4;
            }
            size += vagLens[i];
        }
        ptr_vag_off_table++;
    }

    if (mode == 0) {
        spuAllocMem = (unsigned)SpuMalloc(size);
        if (spuAllocMem == -1) {
            _svm_vab_used[vab_id] = 0;
            _spu_setInTransfer(0);
            _svm_vab_count--;
            return -1;
        }
    } else {
        spuAllocMem = sbaddr;
    }
    if (spuAllocMem + size > 0x80000) {
        _svm_vab_used[vab_id] = 0;
        _spu_setInTransfer(0);
        _svm_vab_count--;
        return -1;
    }
    _svm_vab_start[vab_id] = spuAllocMem;
    size = 0;
    for (i = 0; i <= num_vags; i++) {
        size += vagLens[i];
        if (!(i & 1)) {
            ((short*)&progAtr[i / 2].reserved2)[0] = (spuAllocMem + size) / 8;
        } else {
            ((short*)&progAtr[i / 2].reserved2)[1] = (spuAllocMem + size) / 8;
        }
    }

    _svm_vab_total[vab_id] = size;
    _svm_vab_used[vab_id] = 2;
    return vab_id;
}
#elif VERSION <= 30
#include "libsnd_i.h"

static s16 SsVabOpenHeadWithMode(u8* addr, s16 vabid, s16 mode,
                                 unsigned long sbaddr);

s16 SsVabOpenHead(u8* addr, s16 vabid) {
    return SsVabOpenHeadWithMode(addr, vabid, 0, 0);
}

s16 SsVabOpenHeadSticky(u8* addr, s16 vabid, unsigned long sbaddr) {
    return SsVabOpenHeadWithMode(addr, vabid, 1, sbaddr);
}

s16 SsVabFakeHead(u8* addr, s16 vabid, unsigned long sbaddr) {
    return SsVabOpenHeadWithMode(addr, vabid, 1, sbaddr);
}

static s16 SsVabOpenHeadWithMode(u8* addr, s16 vabid, s16 mode,
                                 unsigned long sbaddr) {
    int vagLens[256];
    s32 i;
    s32 size;
    s16 vab_id;
    u16 vag_len;
    u32 magic;
    u32 spuAllocMem;
    u8 num_vags;
    u8* ptr;
    VabHdr* vab_hdr;

    if (_spu_getInTransfer() == 1) {
        return -1;
    }
    _spu_setInTransfer(1);
    if (vabid >= 0x10) {
        _spu_setInTransfer(0);
        return -1;
    }
    if (vabid == -1) {
        for (i = 0; i < 16; i++) {
            if (_svm_vab_used[i] == 0) {
                _svm_vab_used[i] = 1;
                vab_id = i;
                _svm_vab_count++;
                break;
            }
        }
    } else {
        if (_svm_vab_used[vabid] == 0) {
            _svm_vab_used[vabid] = 1;
            vab_id = vabid;
            _svm_vab_count++;
        }
    }
    if (vab_id >= 0x10) {
        goto end;
    }
    ptr = addr;
    _svm_vab_vh[vab_id] = (VabHdr*)ptr;
    ptr += 0x20;
    _svm_vh = (VabHdr*)addr;
    vab_hdr = (VabHdr*)addr;
    magic = vab_hdr->form;
    if ((magic >> 8) != ('V' << 0x10 | 'A' << 0x8 | 'B')) {
        goto end;
    }
    if ((magic & 0xFF) == 'p' && vab_hdr->ver >= 5) {
        kMaxPrograms = 0x80;
    } else {
        kMaxPrograms = 0x40;
    }
    if (_svm_vh->ps > kMaxPrograms) {
        goto end;
    }
    _svm_vab_pg[vab_id] = (ProgAtr*)ptr;
    _svm_pg = (ProgAtr*)ptr;
    ptr += kMaxPrograms * 0x10;
    size = 0;
    for (i = 0; i < kMaxPrograms; i++) {
        _svm_pg[i].reserved1 = size;
        if (_svm_pg[i].tones != 0) {
            size++;
        }
    }
    _svm_vab_tn[vab_id] = (VagAtr*)ptr;
    _svm_tn = (VagAtr*)ptr;
    size = 0;
    ptr += _svm_vh->ps << 9;
    num_vags = _svm_vh->vs;
    for (i = 0; i < 256; i++) {
        if (num_vags >= i) {
            vag_len = *(u16*)ptr;
            if (_svm_vh->ver >= 5) {
                vagLens[i] = vag_len * 8;
            } else {
                vagLens[i] = vag_len * 4;
            }
            size += vagLens[i];
        }
        ptr += 2;
    }
    _svm_brr_start_addr[vab_id] = (s32)ptr;
    if (mode == 0) {
        spuAllocMem = SpuMalloc(size);
        if (spuAllocMem == -1) {
            _svm_vab_used[vabid] = 0;
            _spu_setInTransfer(0);
            _svm_vab_count--;
            return -1;
        }
    } else {
        spuAllocMem = sbaddr;
    }
    if (spuAllocMem + size > 0x80000U) {
    end:
        _svm_vab_used[vabid] = 0;
        _spu_setInTransfer(0);
        _svm_vab_count--;
        return -1;
    }
    _svm_vab_start[vab_id] = spuAllocMem;
    size = 0;
    for (i = 0; i <= num_vags; i++) {
        size += vagLens[i];
        if (!(i & 1)) {
            _svm_pg[i / 2].reserved2 = (spuAllocMem + size) >> 3;
        } else {
            _svm_pg[i / 2].reserved3 = (spuAllocMem + size) >> 3;
        }
    }
    _svm_vab_total[vab_id] = size;
    _svm_vab_used[vab_id] = 2;
    return vab_id;
}
#else
#include "libsnd_i.h"

static s16 SsVabOpenHeadWithMode(u8* addr, s16 vabid, s16 arg2,
                                 unsigned long sbaddr);

s16 SsVabOpenHead(u8* arg1, s16 vabid) {
    return SsVabOpenHeadWithMode(arg1, vabid, 0, 0);
}

s16 SsVabOpenHeadSticky(u8* addr, s16 vabid, unsigned long sbaddr) {
    return SsVabOpenHeadWithMode(addr, vabid, 1, sbaddr);
}

s16 SsVabFakeHead(u8* addr, s16 vabid, unsigned long sbaddr) {
    return SsVabOpenHeadWithMode(addr, vabid, 1, sbaddr);
}

static s16 SsVabOpenHeadWithMode(u8* addr, s16 vabid, s16 arg2,
                                 unsigned long sbaddr) {
    int vagLens[256];
    s32 i;
    s32 var_s0;
    s16 vabId_2;
    u16 temp_v1;
    u16* ptr_vag_off_table;
    u32 magic;
    u32 spuAllocMem;
    u8 num_vags;
    ProgAtr* pProgTable;
    u8* var_a2;
    VabHdr* vab_hdr_2;

    vabId_2 = 0x10;
    if (_spu_getInTransfer() == 1) {
        return -1;
    }
    _spu_setInTransfer(1);
    if (vabid >= 0x10) {
        _spu_setInTransfer(0);
        return -1;
    }
    if (vabid == -1) {
        for (i = 0; i < 16; i++) {
            if (_svm_vab_used[i] == 0) {
                _svm_vab_used[i] = 1;
                vabId_2 = i;
                _svm_vab_count++;
                break;
            }
        }
    } else {
        var_a2 = _svm_vab_used;
        if (var_a2[vabid] == 0) {
            _svm_vab_used[vabid] = 1;
            vabId_2 = vabid;
            _svm_vab_count++;
        }
    }
    if (vabId_2 >= 0x10) {
        _spu_setInTransfer(0);
#if VERSION <= 33
        _svm_vab_count -= 1;
#endif
        return -1;
    }
    var_a2 = addr;
    _svm_vab_vh[vabId_2] = (VabHdr*)var_a2;

    var_a2 = var_a2 + 0x20;
    vab_hdr_2 = (VabHdr*)addr;
    magic = vab_hdr_2->form;
    if ((magic >> 8) != ('V' << 0x10 | 'A' << 0x8 | 'B')) {
        _svm_vab_used[vabId_2] = 0;
        _spu_setInTransfer(0);
        _svm_vab_count -= 1;
        return -1;
    }
    if ((magic & 0xFF) == 'p') {
        if (vab_hdr_2->ver >= 5) {
            kMaxPrograms = 0x80;
        } else {
            kMaxPrograms = 0x40;
        }
    } else {
        kMaxPrograms = 0x40;
    }
    if (vab_hdr_2->ps <= kMaxPrograms) {
        _svm_vab_pg[vabId_2] = (ProgAtr*)var_a2;
        pProgTable = (ProgAtr*)var_a2;
        var_a2 = var_a2 + (kMaxPrograms * 0x10);
        var_s0 = 0;
        for (i = 0; i < kMaxPrograms; i++) {
            pProgTable[i].reserved1 = var_s0;
            if (pProgTable[i].tones != 0) {
                var_s0++;
            }
        }
        var_s0 = 0;
        _svm_vab_tn[vabId_2] = (VagAtr*)var_a2;
        ptr_vag_off_table = (u16*)(var_a2 + (vab_hdr_2->ps << 9));
        num_vags = vab_hdr_2->vs;
        for (i = 0; i < 256; i++) {
            if (num_vags >= i) {
                temp_v1 = *ptr_vag_off_table;
                if (vab_hdr_2->ver >= 5) {
                    vagLens[i] = temp_v1 * 8;
                } else {
                    vagLens[i] = temp_v1 * 4;
                }
                var_s0 += vagLens[i];
            }
            ptr_vag_off_table++;
        }

        _svm_brr_start_addr[vabId_2] = (s32)ptr_vag_off_table;
        spuAllocMem = sbaddr;
        if (arg2 == 0) {
            spuAllocMem = SpuMalloc(var_s0);
            if (spuAllocMem == -1) {
                _svm_vab_used[vabId_2] = 0;
                _spu_setInTransfer(0);
                _svm_vab_count -= 1;
                return -1;
            }
        }
        if ((spuAllocMem + var_s0) > 0x80000U) {
        end:
            _svm_vab_used[vabId_2] = 0;
            _spu_setInTransfer(0);
            _svm_vab_count -= 1;
            return -1;
        }
        _svm_vab_start[vabId_2] = spuAllocMem;
        var_s0 = 0;
        for (i = 0; i <= num_vags; i++) {
            var_s0 += vagLens[i];
            if (!(i & 1)) {
                pProgTable[i / 2].reserved2 = (spuAllocMem + var_s0) >> 3;
            } else {
                pProgTable[i / 2].reserved3 = (spuAllocMem + var_s0) >> 3;
            }
        }

        _svm_vab_total[vabId_2] = var_s0;
        _svm_vab_used[vabId_2] = 2;
    } else {
        goto end;
    }
    return vabId_2;
}
#endif
