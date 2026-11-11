#include "../types.h"
#include "c_002.h"

static s32 HADR0 = 0x1F801800;
static s32 HADR1 = 0x1F801801;
static s32 HADR2 = 0x1F801802;
static s32 HADR3 = 0x1F801803;

void StSetMask(unsigned long mask, unsigned long start, unsigned long end) {
    StSTART_FLAG = mask;
    StStartFrame = start;
    StEndFrame = end;
}
