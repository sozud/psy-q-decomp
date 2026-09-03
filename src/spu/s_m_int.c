#include "libspu_i.h"
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

