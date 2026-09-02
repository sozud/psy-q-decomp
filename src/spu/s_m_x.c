#include "libspu_i.h"
void _spu_print(void) {
    s32 i;

    for (i = 0; i <= _spu_AllocBlockNum; i++) {
        printf("[%d] %08lx / %08lx (%08ld)\n", i, BLK(i)->addr, BLK(i)->size, BLK(i)->size);

        if (BLK_IS_END(i)) {
            break;
        }
    }
}

