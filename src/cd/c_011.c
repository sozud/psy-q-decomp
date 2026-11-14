// SPDX-License-Identifier: MIT
#include "libcd.h"
#include "c_002.h"

static volatile u8* HADR0 = (u8*)0x1F801800;
static volatile u8* HADR1 = (u8*)0x1F801801;
static volatile u8* HADR2 = (u8*)0x1F801802;
static volatile u8* HADR3 = (u8*)0x1F801803;
static volatile u32* DEV5_DELAY = (u32*)0x1F801018;
static volatile u32* COMMON_DELAY = (u32*)0x1F801020;
static volatile u32* Dma_PCR = (u32*)0x1F8010F0;
static volatile u32* Dma_ICR = (u32*)0x1F8010F4;
static volatile u32* Dma_CHCR1 = (u32*)0x1F801098;
static volatile u32* Dma_MADR1 = (u32*)0x1F801090;
static volatile u32* Dma_CHCR2 = (u32*)0x1F8010A8;
static volatile u32* Dma_MADR2 = (u32*)0x1F8010A0;
static volatile u32* Dma_CHCR3 = (u32*)0x1F8010B8;
static volatile u32* Dma_MADR3 = (u32*)0x1F8010B0;
static volatile u32* Dma_CHCR5 = (u32*)0x1F8010D8;
static volatile u32* Dma_MADR5 = (u32*)0x1F8010D0;
static volatile u32* i_reg = (u32*)0x1F801070;
static volatile u32* i_mask = (u32*)0x1F801074;
#if VERSION >= 41
u32 debug_cause[1] = {0};
#else
u32 debug_cause[2] = {0, 0};
#endif

static void mem2mem(s32*, s32*, u32, u32);

#if VERSION >= 37
#define EMU_SECTOR_ADDR (StEmu_Addr + (StEmu_Idx << 0xB))
#else
#define EMU_SECTOR_ADDR ((StEmu_Idx << 0xB) + StEmu_Addr)
#endif
static void dma_execute(s32, void*, s32, s32, s32, u8, s32);
void data_ready_callback(void);
int printf(const char*, ...);

extern s32 StMode;

void StCdInterrupt(void) {
    static volatile u16* HeaderArea;
#if VERSION >= 41
    static int count = 0;
#endif
    volatile s16 subroutine_arg8[4];
    CdlLOC loc;
    u_char result[8];
    u32* var_a1;
    s32 var_t0;
    u32* var_a0;
    u32 var_v1_2;
    u32 var_v1_3;
    u8* var_v1;

    if (StFinalSector == 1) {
        return;
    }
    if ((StRgb24 != 0) && (*Dma_CHCR1 & 0x01000000)) {
        StCdIntrFlag = true;
        if (StEmu_Addr != 0) {
            StEmu_Idx++;
        }
        debug_cause[0] = 1;
        return;
    }
    if (CdReady(1, result) == CdlDiskError) {
        return;
    }
    subroutine_arg8[1] = result[0];
    subroutine_arg8[2] = result[1];
    if (subroutine_arg8[1] & 4) {
        debug_cause[0] = 3;
        return;
    }
    HeaderArea = (u16*)&((StHEADER*)StRingAddr)[StRingIdx1];
    if (HeaderArea[0] != 0) {
        if (StEmu_Addr != 0) {
            StEmu_Idx++;
        }
        debug_cause[0] = 4;
        return;
    }
    *HADR0 = 0;
    *HADR3 = 0;
    *HADR0 = 0;
    *HADR3 = 0x80;
    *DEV5_DELAY = 0x20943;
    *COMMON_DELAY = 0x1323;
    if (StMode == 0) {
#if VERSION >= 41
        var_v1 = (u8*)&subroutine_arg8[4];
        for (var_v1_2 = 0; var_v1_2 < 4; var_v1_2++) {
            var_v1[var_v1_2] = *HADR2;
        }
#else
        var_v1 = (u8*)&subroutine_arg8[4];
        do {
            *var_v1++ = *HADR2;
        } while (var_v1 < &subroutine_arg8[6]);
#endif
        for (var_v1_2 = 0; var_v1_2 < 8; var_v1_2++) {
            *HADR2;
        }
    }
    var_t0 = 0x11000000;
    if (StEmu_Addr != 0) {
        mem2mem(HeaderArea, EMU_SECTOR_ADDR, 8, 0);
    } else {
        dma_execute(3, HeaderArea, 0, 8, var_t0, 0, 0);
    }
    while (*Dma_CHCR3 & 0x01000000) {
    }
    ((StHEADER*)HeaderArea)->loc = loc;
    *DEV5_DELAY = 0x20843;
    *COMMON_DELAY = 0x1325;
    if ((StSTART_FLAG == 1) && (StStartFrame != 0)) {
        if (StStartFrame != HeaderArea[4]) {
            HeaderArea[0] = 0;
            if (StEmu_Addr != 0) {
                StEmu_Idx++;
            }
            return;
        }
        StSTART_FLAG = 0;
    }
    if ((HeaderArea[0] != 0x160) ||
        (((HeaderArea[1] >> 0xA) & 0x1F) != CChannel)) {
        if (StEmu_Addr != 0) {
            StEmu_Idx = 0;
        } else {
            HeaderArea[0];
        }
        debug_cause[0] = 5;
        HeaderArea[0] = 0;
        return;
    }
    if ((Stsector_offset != HeaderArea[2]) ||
        ((Stframe_no != 0) && (Stframe_no != HeaderArea[4]))) {
        Stframe_no = 0;
        Stsector_offset = 0;
        init_ring_status(StRingIdx2, StRingIdx1 - StRingIdx2);
        StRingIdx1 = StRingIdx2;
        HeaderArea[0] = 0;
        if (StEmu_Addr != 0) {
            StEmu_Idx++;
        }
        debug_cause[0] = 6;
        return;
    }
    if (HeaderArea[2] == 0) {
        Stsector_offset = 0;
        Stframe_no = HeaderArea[4];
        if ((StEndFrame != 0) && ((u32)Stframe_no >= (u32)StEndFrame)) {
            Stframe_no = 0;
            Stsector_offset = 0;
            init_ring_status(StRingIdx2, StRingIdx1 - StRingIdx2);
            StRingIdx1 = StRingIdx2;
            HeaderArea[0] = 0;
            StSTART_FLAG = 1;
            if (StFunc2 != NULL) {
                ((void (*)(void))StFunc2)();
            }
            if (StEmu_Addr != 0) {
                StEmu_Idx++;
            }
            debug_cause[0] = 7;
            return;
        }
        if ((u32)(StRingSize - StRingIdx1 - 1) < HeaderArea[3]) {
            if (StEndFrame == 0) {
                HeaderArea[0] = 1;
                StSTART_FLAG = 1;
                if (StFunc2 != NULL) {
                    ((void (*)(void))StFunc2)();
                }
                if (StEmu_Addr != 0) {
                    StEmu_Idx++;
                }
                debug_cause[0] = 8;
                return;
            }
            if ((s16)((StHEADER*)StRingAddr)->id != 0) {
                HeaderArea[0] = 0;
                if (StEmu_Addr != 0) {
                    StEmu_Idx++;
                }
                debug_cause[0] = 9;
                return;
            }
            HeaderArea[0] = 1;
            var_a1 = (u32*)StRingAddr;
            var_a0 = HeaderArea;
            StRingIdx1 = 0;
#if VERSION >= 41
            for (var_v1_2 = 0; var_v1_2 < 8; var_v1_2++) {
#else
            for (var_v1_3 = 0; var_v1_3 < 8; var_v1_3++) {
#endif
                *var_a1++ = *var_a0++;
            }
            HeaderArea = (u16*)StRingAddr;
        }
        StRingIdx2 = StRingIdx1;
    }
    debug_cause[0] = 10;
    Stsector_offset++;
    StRingBase = (s32)(&((StHEADER*)StRingAddr)[StRingSize] +
                       (StRingIdx1 * 0x3F));

    if (StRgb24 != 0) {
        var_t0 = 0x11000000;
        *DEV5_DELAY = 0x20943;
        *COMMON_DELAY = 0x1323;
    } else {
        *DEV5_DELAY = 0x21020843;
        var_t0 = 0x11400100;
    }
    if ((HeaderArea[3] - 1) == HeaderArea[2]) {
        StFinalSector = 1;
        if (StEmu_Addr != 0) {
            mem2mem(
                (s32*)StRingBase,
                EMU_SECTOR_ADDR + 0x20, 0x1F8, 1);
            StEmu_Idx++;
        } else {
            dma_execute(3, (void*)StRingBase, 0, 0x1F8, var_t0, 1, 0);
        }
        Stsector_offset = 0;
        Stframe_no = 0;
        CChannel = StCHANNEL;
    } else {
        if (StEmu_Addr != 0) {
            mem2mem(
                (s32*)StRingBase,
                EMU_SECTOR_ADDR + 0x20, 0x1F8, 0);
            StEmu_Idx++;
        } else {
            dma_execute(3, (void*)StRingBase, 0, 0x1F8, var_t0, 0, 0);
        }
    }
    *COMMON_DELAY = 0x1325;
    HeaderArea[0] = 3;
    StRingIdx1 += 1;
    if ((StEmu_Addr != 0) && (StFinalSector != 0)) {
        data_ready_callback();
    }
}

static void mem2mem(s32* dst, s32* src, u32 num, u32 arg3) {
    u32 i;
    for (i = 0; i < num; i++) {
        *dst++ = *src++;
    }
}

typedef struct {
    s32 madr;
    s32 bcr;
    s32 chcr;
    s32 unk;
} DMA_CONTROL;

static void dma_execute(
    s32 channel, void* address, s32 blocks, s32 words, s32 control,
    u8 cause, s32 unused) {
    s32 i;
    s32 temp;
    s32 temp2;
    u8 v;
    volatile s32 ret;
    volatile s32* ptr;

    for (i = 0; *(volatile u32*)(0x1F801088 + (channel << 4)) & 0x01000000;
         i++) {
        if (i == 0x10000) {
            printf(
                "DMA STATUS ERROR %x\n",
                *(volatile u32*)(0x1F801088 + (channel << 4)));
            break;
        }
    }
    if (cause == 1) {
        u8* q = (u8*)Dma_ICR;
        v = q[2];
        v |= 1 << channel;
        q[2] = v;
    } else {
        u8* q = (u8*)Dma_ICR;
        v = q[2];
        v &= ~(1 << channel);
        q[2] = v;
    }
    do {
        ret = *(volatile s32*)Dma_ICR;
    } while (0);
    temp2 = channel << 2;
    temp2 = temp2 + 3;
    temp = 1;
    temp = temp << temp2;
    ptr = &((volatile DMA_CONTROL*)0x1F801080)[channel].madr;
    temp2 = *Dma_PCR;
    temp2 |= temp;
    *Dma_PCR = temp2;
    *ptr++ = (s32)address;
    *ptr++ = (blocks << 16) | words;
#if VERSION != 33
    while (!(*HADR0 & 0x40)) {
    }
#endif
    *ptr = control;
    do {
        ret = *ptr;
    } while (0);
}
