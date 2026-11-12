#if VERSION <= 30
#define _spu_RXX header_spu_RXX
#define _spu_AllocBlockNum header_spu_AllocBlockNum
#define _spu_AllocLastNum header_spu_AllocLastNum
#define _spu_memList header_spu_memList
#define _SpuIsInAllocateArea_ header_SpuIsInAllocateArea_
#include "libspu_i.h"
#undef _spu_RXX
#undef _spu_AllocBlockNum
#undef _spu_AllocLastNum
#undef _spu_memList
#undef _SpuIsInAllocateArea_
#undef BLK
#define BLK(i) (&((SPU_MALLOC*)_SpuMemList)[i])

static union SpuUnion* _spu_RXX = (union SpuUnion*)0x1F801C00;
static s32 _spu_AllocBlockNum = 0;
static s32 _spu_AllocLastNum = 0;
static s32 spu_mem_mode;
static char* _SpuMemList;

static void _gcSPU(void);

void _gcSPU(void) {
    s32 i, j;

    for (i = 0; i <= _spu_AllocLastNum;) {
        if (BLK(i)->addr & 0x80000000) {
            for (j = i + 1;; j++) {
                u32 a = BLK(j)->addr;
                if (a != 0x2FFFFFFF) {
                    break;
                }
            }
            if ((BLK(j)->addr & 0x80000000) &&
                ((BLK(j)->addr & 0x0FFFFFFF) ==
                 ((BLK(i)->addr & 0x0FFFFFFF) + BLK(i)->size))) {
                BLK(j)->addr = 0x2FFFFFFF;
                BLK(i)->size += BLK(j)->size;
                continue;
            }
        }
        i++;
    }
    for (i = 0; i <= _spu_AllocLastNum; i++) {
        if (BLK(i)->size == 0) {
            BLK(i)->addr = 0x2FFFFFFF;
        }
    }
    for (i = 0; i <= _spu_AllocLastNum; i++) {
        if (BLK(i)->addr & 0x40000000) {
            break;
        }
        for (j = i + 1; j <= _spu_AllocLastNum; j++) {
            if (BLK(j)->addr & 0x40000000) {
                break;
            }
            if ((BLK(j)->addr & 0x0FFFFFFF) <
                (BLK(i)->addr & 0x0FFFFFFF)) {
                u32 swapAddr = BLK(i)->addr;
                u32 swapSize = BLK(i)->size;
                BLK(i)->addr = BLK(j)->addr;
                BLK(i)->size = BLK(j)->size;
                BLK(j)->addr = swapAddr;
                BLK(j)->size = swapSize;
            }
        }
    }
    for (i = 0; i <= _spu_AllocLastNum; i++) {
        if (BLK(i)->addr & 0x40000000) {
            break;
        }
        if (BLK(i)->addr == 0x2FFFFFFF) {
            BLK(i)->addr = BLK(_spu_AllocLastNum)->addr;
            BLK(i)->size = BLK(_spu_AllocLastNum)->size;
            _spu_AllocLastNum = i;
            break;
        }
    }
    for (i = _spu_AllocLastNum - 1; i >= 0; i--) {
        if (!(BLK(i)->addr & 0x80000000)) {
            break;
        }
        BLK(i)->addr &= 0x0FFFFFFF;
        BLK(i)->addr |= 0x40000000;
        BLK(i)->size += BLK(_spu_AllocLastNum)->size;
        _spu_AllocLastNum = i;
    }
}

long SpuInitMalloc(long num, char* top) {
    SPU_MALLOC* temp = (SPU_MALLOC*)top;

    if (num > 0) {
        temp->addr = 0x40001000;
        _SpuMemList = top;
        _spu_AllocLastNum = 0;
        _spu_AllocBlockNum = num;
        spu_mem_mode = ((_spu_RXX->rxx.data_trans & 0xE) >> 1) + 1;
        temp->size = (0x10000 << spu_mem_mode) - 0x1000;
        return num;
    }
    return 0;
}

long SpuMalloc(long size) {
    s32 unit;
    s32 var_a0;
    s32 var_s2;
    s32 var_s3;
    int i;

    i = 0;
    var_s2 = -1;
    unit = 1 << spu_mem_mode;

    if (_spu_rev_reserve_wa == 0) {
        var_s3 = 0;
    } else {
        var_s3 = (0x10000 - _spu_rev_offsetaddr) << spu_mem_mode;
    }

    unit--;
    var_a0 = size;
    if (size & ~unit) {
        var_a0 += unit;
    }

    size = var_a0;
    size >>= spu_mem_mode;
    size <<= spu_mem_mode;

    if (BLK_IS_END(0)) {
        var_s2 = 0;
    } else {
        _gcSPU();

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


            BLK(next)->addr =
                ((*(volatile int*)&BLK(var_s2)->addr & SPU_MALLOC_ADDR_MASK) +
                 size) |
                SPU_MALLOC_END_MARKER;
            BLK(next)->size = BLK(var_s2)->size - size;

            BLK(var_s2)->addr &= SPU_MALLOC_ADDR_MASK;
            BLK(var_s2)->size = size;

            _spu_AllocLastNum = next;
            _gcSPU();

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
        _gcSPU();

        return BLK(var_s2)->addr;
    }

    return -1;
}

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
        var_v1 = (0x10000 - _spu_rev_offsetaddr) << spu_mem_mode;
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
        _gcSPU();
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

        _gcSPU();
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

        _gcSPU();
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
    s32 unit;

    unit = 1 << spu_mem_mode;
    if (_spu_rev_reserve_wa == 0) {
        var_s2 = 0;
    } else {
        var_s2 = (0x10000 - _spu_rev_offsetaddr) << spu_mem_mode;
    }

    unit--;
    size = size + ((size & ~unit) ? unit : 0);
    size >>= spu_mem_mode;
    size <<= spu_mem_mode;
    addr = _addr;
    addr = addr + ((addr & ~unit) ? unit : 0);
    addr >>= spu_mem_mode;
    addr <<= spu_mem_mode;

    if (addr < 0x1000) return -1;

    temp_a3_2 = addr + size;

    if (((0x10000 << spu_mem_mode) - var_s2) < temp_a3_2) return -1;


    if (BLK(0)->addr & SPU_MALLOC_END_MARKER) {
        temp_a2 = (BLK(0)->size + (0x1000 - addr)) - size;
        if ((addr - 0x1000) > 0) {
            if ((_spu_AllocBlockNum - 2) < _spu_AllocLastNum) return -1;

            BLK(0)->addr = SPU_MALLOC_FREE_ENTRY | 0x1000;
            BLK(0)->size = addr - 0x1000;
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

        _gcSPU();
        return addr;

    } else {

        _gcSPU();

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

        _gcSPU();
        return addr;
    }

    return -1;
}


s32 _SpuIsInAllocateArea(u32 arg0) {
    SPU_MALLOC* var_a1;
    u32 temp_v1;
    u32 bits1;
    u32 bits2;
    u32 mask;

    bits1 = 0x80000000;
    bits2 = 0x40000000;
    mask = 0x0FFFFFFF;

    var_a1 = (SPU_MALLOC*)_SpuMemList;
    while (true) {
        temp_v1 = var_a1->addr;
        if (temp_v1 & bits1) {
            var_a1 += 1;
            continue;
        }
        if (!(temp_v1 & bits2)) {
            if ((temp_v1 & mask) < arg0) {
                if (arg0 >= (u32)((temp_v1 & mask) + var_a1->size)) {
                    var_a1 += 1;
                    continue;
                }
                return 1;
            } else {
                return 1;
            }
        }
        break;
    }
    return 0;
}

s32 _SpuIsInAllocateArea_(u32 arg0) {
    return _SpuIsInAllocateArea(arg0 << spu_mem_mode);
}

void SpuFree(unsigned long arg0) {
    s32 temp_v1;
    s32 cur_block_num;
    SPU_MALLOC* cur_mem;
    s32 temp;
    s32 mask4;
    s32 block_num;
    u8 pad[2];

    cur_block_num = 0;
    if (temp_v1 = _spu_AllocBlockNum > 0) {
        mask4 = 0x40000000;
        temp = arg0 | 0x80000000;
        block_num = _spu_AllocBlockNum;
        cur_mem = (SPU_MALLOC*)_SpuMemList;
        do {
            temp_v1 = cur_mem->addr;
            if (temp_v1 & mask4) {
                break;
            }
            if (temp_v1 == arg0) {
                cur_mem->addr = temp;
                break;
            }
            cur_mem++;
            cur_block_num++;
        } while (cur_block_num < block_num);
    }
    _gcSPU();
}

void _print(void) {
    s32 i;

    for (i = 0; i <= _spu_AllocBlockNum; i++) {
        printf("[%d] %08x / %08x (%08d)\n", i, BLK(i)->addr, BLK(i)->size, BLK(i)->size);

        if (BLK_IS_END(i)) {
            break;
        }
    }
}

#else
#include "libspu_i.h"
#if VERSION < 40
void _spu_gcSPU(void) {
    s32 i, j;

    for (i = 0; i <= _spu_AllocLastNum;) {
        if (BLK(i)->addr & 0x80000000) {
            for (j = i + 1; 1; j++) {
                if (BLK(j)->addr != 0x2FFFFFFF) {
                    break;
                }
            }
            if ((BLK(j)->addr & 0x80000000) &&
                ((BLK(j)->addr & 0x0FFFFFFF) ==
                 ((BLK(i)->addr & 0x0FFFFFFF) + BLK(i)->size))) {
                BLK(j)->addr = 0x2FFFFFFF;
                BLK(i)->size += BLK(j)->size;
                continue;
            }
        }
        i++;
    }
    for (i = 0; i <= _spu_AllocLastNum; i++) {
        if (BLK(i)->size == 0) {
            BLK(i)->addr = 0x2FFFFFFF;
        }
    }
    for (i = 0; i <= _spu_AllocLastNum; i++) {
        if (BLK(i)->addr & 0x40000000) {
            break;
        }
        for (j = i + 1; j <= _spu_AllocLastNum; j++) {
            if (BLK(j)->addr & 0x40000000) {
                break;
            }
            if ((BLK(j)->addr & 0x0FFFFFFF) <
                (BLK(i)->addr & 0x0FFFFFFF)) {
                u32 swapAddr = BLK(i)->addr;
                u32 swapSize = BLK(i)->size;
                BLK(i)->addr = BLK(j)->addr;
                BLK(i)->size = BLK(j)->size;
                BLK(j)->addr = swapAddr;
                BLK(j)->size = swapSize;
            }
        }
    }
    for (i = 0; i <= _spu_AllocLastNum; i++) {
        if (BLK(i)->addr & 0x40000000) {
            break;
        }
        if (BLK(i)->addr == 0x2FFFFFFF) {
            BLK(i)->addr = BLK(_spu_AllocLastNum)->addr;
            BLK(i)->size = BLK(_spu_AllocLastNum)->size;
            _spu_AllocLastNum = i;
            break;
        }
    }
    for (i = _spu_AllocLastNum - 1; i >= 0; i--) {
        if (!(BLK(i)->addr & 0x80000000)) {
            break;
        }
        BLK(i)->addr &= 0x0FFFFFFF;
        BLK(i)->addr |= 0x40000000;
        BLK(i)->size += BLK(_spu_AllocLastNum)->size;
        _spu_AllocLastNum = i;
    }
}
#else
void _spu_gcSPU(void) {
    int i, j;


    for (i = 0; i <= _spu_AllocLastNum;) {
        if (BLK_IS_FREE(i)) {

            for (j = i + 1; 1; j++) {
                if (!BLK_IS_DEAD_MARK(j)) {
                    break;
                }
            }

            if (BLK_IS_FREE(j) && BLK_ADDR(j) == BLK_ADDR_END(i)) {
                BLK(j)->addr = SPU_MALLOC_DEAD_MARKER;
                BLK(i)->size += BLK(j)->size;
                continue;
            }
        }

        i++;
    }

    for (i = 0; i <= _spu_AllocLastNum; i++) {
        if (BLK(i)->size == 0) {
            BLK(i)->addr = SPU_MALLOC_DEAD_MARKER;
        }
    }

    for (i = 0; i <= _spu_AllocLastNum; i++) {
        if (BLK_IS_END(i)) break;

        for (j = i + 1; j <= _spu_AllocLastNum; j++) {

            if (BLK_IS_END(j)) break;

            if (BLK_ADDR(j) < BLK_ADDR(i)) {
                BLK_SWAP(BLK(j), BLK(i));
            }
        }
    }

    for (i = 0; i <= _spu_AllocLastNum; i++) {
        if (BLK_IS_END(i)) break;

        if (BLK_IS_DEAD_MARK(i)) {
            BLK(i)->addr = BLK(_spu_AllocLastNum)->addr;
            BLK(i)->size = BLK(_spu_AllocLastNum)->size;
            _spu_AllocLastNum = i;
            break;
        }
    }

    for (i = _spu_AllocLastNum - 1; i >= 0; i--) {
        if ((BLK_IS_FREE(i)) == 0) break;

        BLK(i)->addr = BLK_ADDR(i) | SPU_MALLOC_END_MARKER;
        BLK(i)->size += BLK(_spu_AllocLastNum)->size;
        _spu_AllocLastNum = i;
    }
}
#endif
#endif
