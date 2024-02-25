#include "libspu_i.h"

long SpuInitMalloc(long num, char* top) {
    SPU_MALLOC* temp = top;
    if (num > 0) {
        temp->addr = 0x40001010;
        _spu_memList = temp;
        _spu_AllocLastNum = 0;
        _spu_AllocBlockNum = num;
        temp->size = (s32)((0x10000 << _spu_mem_mode_plus) - 0x1010);
        return num;
    }
    return 0;
}