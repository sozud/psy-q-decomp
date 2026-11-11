#include "../types.h"
#include "c_002.h"

static s32 HADR0 = 0x1F801800;
static s32 HADR1 = 0x1F801801;
static s32 HADR2 = 0x1F801802;
static s32 HADR3 = 0x1F801803;

int init_ring_status(s32 arg0, u32 arg1) {
    s32 i;

    for (i = 0; i < arg1; i++) {
#if VERSION >= 37
        *(s32*)((u8*)StRingAddr + ((i + arg0) << 5)) = 0;
#else
        *(s32*)(StRingAddr + ((i + arg0) * 32)) = 0;
#endif
    }
}
