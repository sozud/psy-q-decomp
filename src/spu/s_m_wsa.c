#include "libspu_i.h"
static s32 _SpuMallocSeparateTo3(s32 arg0, u32 arg1, s32 arg2) {
    s32 temp_t1;
    s32 temp_t3;
    s32 temp_t4;
    s32 var_a3;
    s32 var_v1;
    int a1 = arg1;

    temp_t4 = BLK(arg0)->size;
    temp_t3 = BLK(arg0)->addr;
    temp_t1 = a1 - BLK_ADDR(arg0);

    if (_spu_rev_reserve_wa == 0) {
        var_v1 = 0;
    } else {
        var_v1 = (0x10000 - _spu_rev_offsetaddr) << _spu_mem_mode_plus;
    }

    if (temp_t3 & SPU_MALLOC_END_MARKER) {
        var_a3 = (temp_t4 - temp_t1) - var_v1;
    } else {
        var_a3 = temp_t4 - temp_t1;
    }

    if (var_a3 < arg2) return -1;

    if (temp_t3 & SPU_MALLOC_END_MARKER) {
        if (temp_t1 > 0) {
            if ((_spu_AllocBlockNum - 2) < _spu_AllocLastNum) return -1;

            BLK(_spu_AllocLastNum)->size = temp_t1;
            BLK(_spu_AllocLastNum)->addr = BLK_ADDR(_spu_AllocLastNum) | SPU_MALLOC_FREE_ENTRY;
            _spu_AllocLastNum++;
            BLK(_spu_AllocLastNum)->addr = a1;
            BLK(_spu_AllocLastNum)->size = arg2;
        } else {
            if ((_spu_AllocBlockNum - 1) < _spu_AllocLastNum) return -1;

            BLK(_spu_AllocLastNum)->size = arg2;
            BLK(_spu_AllocLastNum)->addr = BLK_ADDR(_spu_AllocLastNum);

        }

        _spu_AllocLastNum++;
        BLK(_spu_AllocLastNum)->addr = ((temp_t3 & SPU_MALLOC_ADDR_MASK) + temp_t1 + arg2) | SPU_MALLOC_END_MARKER;
        BLK(_spu_AllocLastNum)->size = (temp_t4 - temp_t1) - arg2;
        _spu_gcSPU();
        return a1;
    } else if (temp_t1 > 0) {
        int temp_v1_3;
        int var_a0;

        if (var_a3 == arg2) {
            if ((_spu_AllocBlockNum - 2) < _spu_AllocLastNum) return -1;
        } else {
            if ((_spu_AllocBlockNum - 1) < _spu_AllocLastNum) return -1;
        }

        BLK(arg0)->size = temp_t1;

        temp_v1_3 = BLK(_spu_AllocLastNum)->addr;
        var_a0 = BLK(_spu_AllocLastNum)->size;
        BLK(_spu_AllocLastNum)->addr = a1;
        BLK(_spu_AllocLastNum)->size = arg2;
        BLK(_spu_AllocLastNum + 1)->addr = temp_v1_3;
        BLK(_spu_AllocLastNum + 1)->size = var_a0;
        _spu_AllocLastNum++;

        if (var_a3 >= arg2) {
            int var_a1 = BLK(_spu_AllocLastNum)->addr;
            int var_a0 = BLK(_spu_AllocLastNum)->size;

            BLK(_spu_AllocLastNum)->addr = ((temp_t3 & SPU_MALLOC_ADDR_MASK) + temp_t1 + arg2) | SPU_MALLOC_FREE_ENTRY;
            BLK(_spu_AllocLastNum)->size = var_a3 - arg2;
            BLK(_spu_AllocLastNum + 1)->addr = var_a1;
            BLK(_spu_AllocLastNum + 1)->size = var_a0;
            _spu_AllocLastNum++;
        }

        _spu_gcSPU();
        return a1;
    } else {
        if ((arg2 < var_a3) && ((_spu_AllocBlockNum - 1) < _spu_AllocLastNum)) {
            return -1;
        }

        BLK(arg0)->size = arg2;
        BLK(arg0)->addr = BLK_ADDR(arg0);
        if (arg2 < var_a3) {
            int var_a2 = BLK(_spu_AllocLastNum)->addr;
            int var_a0 = BLK(_spu_AllocLastNum)->size;

            BLK(_spu_AllocLastNum)->addr = ((temp_t3 & SPU_MALLOC_ADDR_MASK) + arg2) | SPU_MALLOC_FREE_ENTRY;
            BLK(_spu_AllocLastNum)->size = var_a3 - arg2;
            BLK(_spu_AllocLastNum + 1)->addr = var_a2;
            BLK(_spu_AllocLastNum + 1)->size = var_a0;
            _spu_AllocLastNum++;
        }

        _spu_gcSPU();
        return a1;
    }

    return -1;
}

long SpuMallocWithStartAddr(unsigned long _addr, long _size) {
    s32 temp_a2;
    s32 temp_a3_2;
    s32 size = _size;
    s32 addr;
    s32 var_s2;
    int i;

    if (_spu_rev_reserve_wa == 0) {
        var_s2 = 0;
    } else {
        var_s2 = (0x10000 - _spu_rev_offsetaddr) << _spu_mem_mode_plus;
    }

    size = size + ((size & ~_spu_mem_mode_unitM) ? _spu_mem_mode_unitM : 0);
    size >>= _spu_mem_mode_plus;
    size <<= _spu_mem_mode_plus;
    addr = _addr;
    addr = addr + ((addr & ~_spu_mem_mode_unitM) ? _spu_mem_mode_unitM : 0);
    addr >>= _spu_mem_mode_plus;
    addr <<= _spu_mem_mode_plus;

    if (addr < 0x1010) return -1;

    temp_a3_2 = addr + size;

    if (((0x10000 << _spu_mem_mode_plus) - var_s2) < temp_a3_2) return -1;


    if (BLK(0)->addr & SPU_MALLOC_END_MARKER) {
        temp_a2 = (BLK(0)->size + (0x1010 - addr)) - size;
        if ((addr - 0x1010) > 0) {
            if ((_spu_AllocBlockNum - 2) < _spu_AllocLastNum) return -1;

            BLK(0)->addr = SPU_MALLOC_FREE_ENTRY | 0x1010;
            BLK(0)->size = addr - 0x1010;
            BLK(1)->addr = addr;
            BLK(1)->size = size;
            BLK(2)->addr = temp_a3_2 | SPU_MALLOC_END_MARKER;
            BLK(2)->size = temp_a2;

            _spu_AllocLastNum = 2;
        } else {
            if ((_spu_AllocBlockNum - 1) < _spu_AllocLastNum) return -1;

            BLK(0)->addr = addr;
            BLK(0)->size = size;
            BLK(1)->addr = temp_a3_2 | SPU_MALLOC_END_MARKER;
            BLK(1)->size = temp_a2;

            _spu_AllocLastNum = 1;
        }

        _spu_gcSPU();
        return addr;

    } else {

        _spu_gcSPU();

        for (i = 0; 1; ) {
            if (BLK_IS_FREE(i)) {
                if ((u32)addr >= BLK_ADDR(i)) {
                    if ((u32)addr >= BLK_ADDR_END(i)
                      && !(BLK_IS_END(i + 1))) {
                        i++;
                        continue;
                    } else {
                        temp_a2 = BLK(i)->size - (addr - BLK_ADDR(i));
                        if ((temp_a2 >= size))  {
                            addr = _SpuMallocSeparateTo3(i, addr, size);
                            break;
                        }
                    }
                } else {
                    temp_a2 = BLK(i)->size;
                    if (temp_a2 >= size) {
                        addr = _SpuMallocSeparateTo3(i, BLK_ADDR(i), size);
                        break;
                    }
                }

                i++;
            }
            else if (!(BLK_IS_END(i))) {
                i++;
            } else {
                temp_a2 = (BLK(i)->size - var_s2);
                if (temp_a2 < size) return -1;
                addr = _SpuMallocSeparateTo3(i, BLK_ADDR(i), size);
                break;
            }
        }

        _spu_gcSPU();
        return addr;
    }

    return -1;
}

