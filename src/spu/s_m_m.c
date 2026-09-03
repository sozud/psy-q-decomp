#include "libspu_i.h"
long SpuMalloc(long size) {
    s32 var_a0;
    s32 var_s2;
    s32 var_s3;
    int i;

    i = 0;
    var_s2 = -1;

    if (_spu_rev_reserve_wa == 0) {
        var_s3 = 0;
    } else {
        var_s3 = (0x10000 - _spu_rev_offsetaddr) << _spu_mem_mode_plus;
    }

    var_a0 = size;
    if (size & ~_spu_mem_mode_unitM) {
        var_a0 += _spu_mem_mode_unitM;
    }

    size = var_a0;
    size >>= _spu_mem_mode_plus;
    size <<= _spu_mem_mode_plus;

    if (BLK_IS_END(0)) {
        var_s2 = 0;
    } else {
        _spu_gcSPU();

        for (; i < _spu_AllocBlockNum; i++) {
            if (BLK_IS_END(i) || (BLK_IS_FREE(i) && BLK(i)->size >= size)) {
                var_s2 = i;
                break;
            }
        }
    }

    if (var_s2 == -1) return -1;

    if (BLK_IS_END(var_s2)) {
        if (var_s2 < _spu_AllocBlockNum && BLK(var_s2)->size - var_s3 >= size) {
            int next = var_s2 + 1;


            BLK(next)->addr = *(volatile int*)&BLK(var_s2)->addr & SPU_MALLOC_ADDR_MASK;
            BLK(next)->addr += size;
            BLK(next)->addr |= SPU_MALLOC_END_MARKER;
            BLK(next)->size = BLK(var_s2)->size - size;

            BLK(var_s2)->addr &= SPU_MALLOC_ADDR_MASK;
            BLK(var_s2)->size = size;

            _spu_AllocLastNum = next;
            _spu_gcSPU();

            return BLK(var_s2)->addr;
        }
    }
    else {
        if ((size < BLK(var_s2)->size) && (_spu_AllocLastNum < _spu_AllocBlockNum)) {
            long t_addr, t_size;
            long _addr = BLK(var_s2)->addr + size;
            long _size = BLK(var_s2)->size - size;

            t_addr = BLK(_spu_AllocLastNum)->addr;
            t_size = BLK(_spu_AllocLastNum)->size;

            BLK(_spu_AllocLastNum)->addr = _addr | SPU_MALLOC_FREE_ENTRY;
            BLK(_spu_AllocLastNum)->size = _size;

            BLK(_spu_AllocLastNum + 1)->addr = t_addr;
            BLK(_spu_AllocLastNum + 1)->size = t_size;

            _spu_AllocLastNum++;
        }

        BLK(var_s2)->size = size;
        BLK(var_s2)->addr = BLK_ADDR(var_s2);
        _spu_gcSPU();

        return BLK(var_s2)->addr;
    }

    return -1;
}

