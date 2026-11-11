#include "../types.h"
#include "c_002.h"

static s32 HADR0 = 0x1F801800;
static s32 HADR1 = 0x1F801801;
static s32 HADR2 = 0x1F801802;
static s32 HADR3 = 0x1F801803;

unsigned long StGetNext(unsigned long** addr, unsigned long** header) {
    volatile u16* ptr = (volatile u16*)(StRingAddr + (StRingIdx3 * 32));

    if (*ptr == 1) {
        StRingIdx3 = 0;
        if (StEndFrame != 0) {
            *ptr = 0;
        }
        ptr = (volatile u16*)(StRingAddr + (StRingIdx3 * 32));
    }
    if (*ptr == 2) {
        *ptr = 4;
        *addr = (unsigned long*)(StRingAddr + (StRingSize * 32) +
                                (StRingIdx3 * 2016));
        *header = (unsigned long*)ptr;
        return 0;
    } else {
        return 1;
    }
}
