#include "../types.h"

typedef struct {
    u8 minute;
    u8 second;
    u8 sector;
    u8 track;
} CdlLOC;

typedef struct {
    u16 id;
    u16 type;
    u16 secCount;
    u16 nSectors;
    unsigned long frameCount;
    unsigned long frameSize;
    u16 width;
    u16 height;
    unsigned long dummy1;
    unsigned long dummy2;
    CdlLOC loc;
} StHEADER;

#if VERSION == 40
extern StHEADER* StRingAddr;
extern s32 StRingBase;
extern s32 StRgb24;
extern s32 StEmu_Addr;
extern s32 StEmu_Idx;
extern s32 StRingSize;
extern s32 StRingIdx1;
extern s32 StRingIdx2;
extern s32 StRingIdx3;
extern s32 StSTART_FLAG;
extern s32 StStartFrame;
extern s32 StEndFrame;
extern s32 StFinalSector;
void (*StFunc1)(void);
s32 StFunc2;
extern s32 Stframe_no;
extern s16 Stsector_offset;
extern s32 StCdIntrFlag;
extern s32 StCHANNEL;
extern s32 CChannel;
#else
StHEADER* StRingAddr;
s32 StRingBase;
s32 StRgb24;
s32 StEmu_Addr;
s32 StEmu_Idx;
s32 StRingSize;
s32 StRingIdx1;
s32 StRingIdx2;
s32 StRingIdx3;
s32 StSTART_FLAG;
s32 StStartFrame;
s32 StEndFrame;
s32 StFinalSector;
void (*StFunc1)(void);
s32 StFunc2;
s32 Stframe_no;
s16 Stsector_offset;
s32 StCdIntrFlag;
s32 StCHANNEL;
s32 CChannel;
#endif

static s32 HADR0 = 0x1F801800;
static s32 HADR1 = 0x1F801801;
static s32 HADR2 = 0x1F801802;
static s32 HADR3 = 0x1F801803;

#if VERSION != 33
extern s32 StMode;
static CdlLOC previous_frame_location;
static s32 next_frameid;
#endif

int CdPosToInt(CdlLOC*);
CdlLOC* CdIntToPos(int, CdlLOC*);

void data_ready_callback(void) {
    StHEADER* ptr = &StRingAddr[StRingIdx2];
#if VERSION != 33
    do {
    } while (0);
#endif

#if VERSION == 33
    *(u16*)ptr = 2;
#else
    ptr->id = 2;
#endif
#if VERSION != 33
    previous_frame_location = ptr->loc;
    next_frameid = ptr->frameCount;
#endif
    StRingIdx2 = StRingIdx1;
    if (StFunc1 != 0) {
        StFunc1();
    }
    StFinalSector = 0;
}

#if VERSION != 33
s32 StGetBackloc(CdlLOC* loc) {
    if (StMode) {
        return -1;
    }
    CdIntToPos(CdPosToInt(&previous_frame_location) + 1, loc);
    return next_frameid;
}
#endif
