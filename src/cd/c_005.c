#include "../types.h"
#define CDROM_DEFINE_FUNC
#include "c_002.h"

static s32 HADR0 = 0x1F801800;
static s32 HADR1 = 0x1F801801;
static s32 HADR2 = 0x1F801802;
static s32 HADR3 = 0x1F801803;

void StSetMask(unsigned long, unsigned long, unsigned long);

void StSetStream(unsigned long mode, unsigned long start_frame,
                 unsigned long end_frame, void (*func1)(), void (*func2)()) {
    StSetMask(1U, start_frame, end_frame);
    StEmu_Addr = 0;
    StFunc1 = (s32)func1;
    StRgb24 = mode & 1;
    CChannel = 0;
    StCHANNEL = 0;
    Stsector_offset = 0;
    Stframe_no = 0;
    StFunc2 = (s32)func2;
}
