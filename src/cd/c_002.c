#include "../types.h"
#include "c_002.h"

static s32 reg0 = 0x1F801800;
static s32 reg1 = 0x1F801801;
static s32 reg2 = 0x1F801802;
static s32 reg3 = 0x1F801803;

s32 Stframe_no;
s32 StFinalSector;
s16 Stsector_offset;

void StClearRing(void);

s32 StRgb24;
s32 StEmu_Idx;
s32 StFunc1;
s32 StCdIntrFlag;
s32 StFunc2;
s32 StRingIdx1;
s32 StRingIdx2;
s32 StRingIdx3;
s32 StStartFrame;
s32 StCHANNEL;

void init_ring_status(s32, s32);

s32 StEmu_Addr;
s32 StEndFrame;
s32 StSTART_FLAG;
s32 StRingAddr;
s32 StRingBase;
s32 StRingSize;
s32 CChannel;

void StClearRing(void) {
    StRingIdx3 = 0;
    StRingIdx2 = 0;
    StRingIdx1 = 0;
    StFinalSector = 0;
    init_ring_status(0, StRingSize);
    StCdIntrFlag = 0;
    Stsector_offset = 0;
    Stframe_no = 0;
}
