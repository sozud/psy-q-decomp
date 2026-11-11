#include "../types.h"
#include "c_002.h"

static s32 HADR0 = 0x1F801800;
static s32 HADR1 = 0x1F801801;
static s32 HADR2 = 0x1F801802;
static s32 HADR3 = 0x1F801803;

unsigned long StFreeRing(unsigned long* base) {
    s32 temp_a1;
    s32 i;
    s16 nSectors;
    s16 id;

    temp_a1 =
        (base - (unsigned long*)(StRingAddr + (StRingSize * 32))) / 504;
#if VERSION >= 37
    id = *(s16*)((u8*)StRingAddr + (temp_a1 << 5));
    nSectors = *(s16*)((u8*)StRingAddr + (temp_a1 << 5) + 6);
#else
    id = *(s16*)(StRingAddr + (temp_a1 * 32));
    nSectors = *(s16*)(StRingAddr + (temp_a1 * 32) + 6);
#endif
    if (id != 4) {
        return 1;
    }
    for (i = 0; i < nSectors; i++) {
#if VERSION >= 37
        *(s16*)((u8*)StRingAddr + ((i + temp_a1) << 5)) = 0;
#else
        *(s16*)(StRingAddr + ((i + temp_a1) * 32)) = 0;
#endif
    }
    StRingIdx3 = i + temp_a1;
    return 0;
}
