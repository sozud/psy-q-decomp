#if VERSION <= 30
#include "libcd.h"

void EnterCriticalSection(void);
void ExitCriticalSection(void);
long CdDataCallback(void (*)());
long CdCallback(void (*)());
int printf(const char*, ...);

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
static volatile u8* POST = (u8*)0x1F802041;
static volatile u32* i_reg = (u32*)0x1F801070;
static volatile u32* i_mask = (u32*)0x1F801074;
static int sdata19 = 0x10;
static int sdata20 = 0xF0;
#if VERSION <= 30
static int frame_no = 0;
static int CHANNEL = 0;
static int CChannel = 0;
#else
static int CHANNEL = 0;
static int CChannel = 0;
static int frame_no = 0;
#endif

u32 RingAddr;
u32 RingBase;
volatile u32 FinalSector;
u32 StCdIntrFlag;

static u32 Rgb24;
static u32 Emu_Addr;
static u32 Emu_Idx;
static u32 em_ack;
static u32 RingSize;
static u32 RingIdx1;
static u32 RingIdx2;
static u32 RingIdx3;
static u32 START_FLAG;
static u32 StartFlame;
static u32 EndFlame;
static void (*Func1)();
static void (*Func2)();
static u32 CDACK;

void StSetMask(u32 mask, u32 start, u32 end);
static int init_ring_status(int start, u32 num);
static void dma_execute(int ch, void* addr, int blocks, int words, int control, int cause, int arg6);
static void mem2mem(u32* dst, u32* src, u32 num, u32 arg3);

void StSetRing(u32* ring_addr, u32 ring_size) {
    RingAddr = (u32)ring_addr;
    RingSize = ring_size;
#if VERSION <= 30
    RingIdx3 = 0;
    RingIdx2 = 0;
    RingIdx1 = 0;
#else
    RingIdx1 = 0;
    RingIdx2 = 0;
    RingIdx3 = 0;
#endif
    FinalSector = 0;
    init_ring_status(0, ring_size);
    StCdIntrFlag = 0;
}

void StClearRing(void) {
#if VERSION <= 30
    RingIdx3 = 0;
    RingIdx2 = 0;
    RingIdx1 = 0;
#else
    RingIdx1 = 0;
    RingIdx2 = 0;
    RingIdx3 = 0;
#endif
    FinalSector = 0;
    init_ring_status(0, RingSize);
    StCdIntrFlag = 0;
}

void StUnSetRing(void) {
    EnterCriticalSection();
#if VERSION == 26
    CdCallback(NULL);
    CdDataCallback(NULL);
#else
    CdDataCallback(NULL);
    CdReadyCallback(NULL);
#endif
    ExitCriticalSection();
}

void data_ready_callback(void) {
    *(u16*)(RingAddr + RingIdx2 * 32) = 2;
    FinalSector = 0;
    RingIdx2 = RingIdx1;
    if (Func1 != NULL) {
        Func1();
    }
}

void StSetStream(u32 mode, u32 start_frame, u32 end_frame, void (*func1)(),
                 void (*func2)()) {
    StSetMask(1, start_frame, end_frame);
    Emu_Addr = 0;
    Func1 = func1;
    Rgb24 = mode & 1;
    Func2 = func2;
}

void StSetEmulate(u32* addr, u32 rgb24, u32 start_frame, u32 end_frame,
                  void (*func1)(), void (*func2)()) {
    StSetMask(1, start_frame, end_frame);
    Emu_Addr = (u32)addr;
    Emu_Idx = 0;
    Rgb24 = 0;
    Func1 = func1;
    Func2 = func2;
}

u32 StFreeRing(u32* base) {
    int idx;
    int i;
    s16 nSectors;
    s16 id;

    idx = (base - (u32*)(RingAddr + RingSize * 32)) / 504;
    id = *(s16*)(RingAddr + idx * 32);
    nSectors = *(s16*)(RingAddr + idx * 32 + 6);
    if (id != 4) {
        return 1;
    }
    for (i = 0; i < nSectors; i++) {
        *(s16*)(RingAddr + (i + idx) * 32) = 0;
    }
    RingIdx3 = i + idx;
    return 0;
}

static int init_ring_status(int start, u32 num) {
    u32 i;

    for (i = 0; i < num; i++) {
        *(u32*)&((StHEADER*)RingAddr)[i + start] = 0;
    }
}

u32 StGetNext(u32** addr, u32** header) {
    volatile u16* ptr = (volatile u16*)&((StHEADER*)RingAddr)[RingIdx3];

    if (*ptr == 1) {
        RingIdx3 = 0;
        *ptr = 0;
        ptr = (volatile u16*)&((StHEADER*)RingAddr)[RingIdx3];
    }
    if (*ptr == 2) {
        *ptr = 4;
        *addr = (u32*)(RingAddr + RingSize * 32 + RingIdx3 * 2016);
        *header = (u32*)ptr;
        return 0;
    }
    return 1;
}

void StSetMask(u32 mask, u32 start, u32 end) {
    START_FLAG = mask;
    StartFlame = start;
    EndFlame = end;
}

void StCdInterrupt(void) {
    volatile s16 st[3];
    u_char result[8];
    static int cause = 0;
    static volatile u16* HeaderArea;
    static s16 sector_offset = 0;
    static int sdata26 = 0;
    int control;
    u32 i;
    u32* src;
    u32* dst;

    st[2] = 0x5555;
    st[1] = st[2];
    if (Rgb24 != 0 && (*Dma_CHCR1 & 0x01000000)) {
        StCdIntrFlag = 1;
        if (Emu_Addr != 0) {
            Emu_Idx++;
        }
        cause = 1;
        return;
    }
    *HADR0 = 0;
    *HADR3 = 0;
    CdReady(1, result);
    st[1] = result[0];
    st[2] = result[1];
    st[0] = *HADR0 & 0xFF;
    if (st[1] & 4) {
        st[2] = *HADR1 & 0xFF;
        cause = 3;
        return;
    }
    if (FinalSector == 1) {
        if (Emu_Addr != 0) {
            Emu_Idx++;
        }
        cause = 0xB;
#if VERSION <= 30
        printf("Final Sector is on %08x %08x %08x %04x %04x\n", *Dma_CHCR3,
               *Dma_PCR, *Dma_ICR, *(u16*)i_reg, *(u16*)i_mask);
#else
        printf("Final Sector is on %08x %08x %08x %04x %04x\n", *i_reg,
               *i_mask, *Dma_ICR, *(u16*)Dma_CHCR1, *(u16*)Dma_MADR1);
#endif
        return;
    }
    HeaderArea = (u16*)&((StHEADER*)RingAddr)[RingIdx1];
    if (HeaderArea[0] != 0) {
        if (Emu_Addr != 0) {
            Emu_Idx++;
        }
        cause = 4;
        return;
    }
    *HADR0 = 0;
    *HADR3 = 0x80;
    *DEV5_DELAY = 0x20943;
    *COMMON_DELAY = 0x1323;
    control = 0x11000000;
    if (Emu_Addr != 0) {
        mem2mem((u32*)HeaderArea, (u32*)((Emu_Idx << 11) + Emu_Addr), 8, 0);
    } else {
        dma_execute(3, (void*)HeaderArea, 0, 8, control, 0, 0);
    }
    while (*Dma_CHCR3 & 0x01000000) {
    }
    *DEV5_DELAY = 0x20843;
    *COMMON_DELAY = 0x1325;
    if (START_FLAG == 1 && StartFlame != 0) {
        if (StartFlame != HeaderArea[4]) {
            HeaderArea[0] = 0;
            if (Emu_Addr != 0) {
                Emu_Idx++;
            }
            return;
        }
        START_FLAG = 0;
    }
    if (HeaderArea[0] != 0x160 || ((HeaderArea[1] >> 10) & 0x1F) != CChannel) {
        if (Emu_Addr != 0) {
            Emu_Idx = 0;
        } else {
            HeaderArea[0];
        }
        cause = 5;
        HeaderArea[0] = 0;
        return;
    }
    if (sector_offset != HeaderArea[2] ||
        (frame_no != 0 && frame_no != HeaderArea[4])) {
        frame_no = 0;
        sector_offset = 0;
        init_ring_status(RingIdx2, RingIdx1 - RingIdx2);
        RingIdx1 = RingIdx2;
        HeaderArea[0] = 0;
        if (Emu_Addr != 0) {
            Emu_Idx++;
        }
        cause = 6;
        return;
    }
    if (HeaderArea[2] == 0) {
        sector_offset = 0;
        frame_no = HeaderArea[4];
        if (EndFlame != 0 && (u32)frame_no >= EndFlame) {
            frame_no = 0;
            sector_offset = 0;
            init_ring_status(RingIdx2, RingIdx1 - RingIdx2);
            RingIdx1 = RingIdx2;
            HeaderArea[0] = 0;
            START_FLAG = 1;
            if (Func2 != NULL) {
                Func2();
            }
            if (Emu_Addr != 0) {
                Emu_Idx++;
            }
            cause = 7;
            return;
        }
        if ((u32)(RingSize - RingIdx1 - 1) < HeaderArea[3]) {
            if (EndFlame == 0) {
                HeaderArea[0] = 1;
                START_FLAG = 1;
                if (Func2 != NULL) {
                    Func2();
                }
                if (Emu_Addr != 0) {
                    Emu_Idx++;
                }
                cause = 8;
                return;
            }
            if (*(s16*)RingAddr != 0) {
                HeaderArea[0] = 0;
                if (Emu_Addr != 0) {
                    Emu_Idx++;
                }
                cause = 9;
                return;
            }
            HeaderArea[0] = 1;
            RingIdx1 = 0;
            dst = (u32*)RingAddr;
            src = (u32*)HeaderArea;
            for (i = 0; i < 8; i++) {
                *dst++ = *src++;
            }
            HeaderArea = (u16*)RingAddr;
        }
        RingIdx2 = RingIdx1;
    }
    cause = 10;
    sector_offset++;
    RingBase = (u32)&((StHEADER*)RingAddr)[RingSize] + RingIdx1 * 0x3F * 32;
    if (Rgb24 != 0) {
        *DEV5_DELAY = 0x20943;
        control = 0x11000000;
        *COMMON_DELAY = 0x1323;
    } else {
        *DEV5_DELAY = 0x21020843;
        control = 0x11400100;
    }
    if (HeaderArea[3] - 1 == HeaderArea[2]) {
        if (Emu_Addr != 0) {
            mem2mem((u32*)RingBase, (u32*)((Emu_Idx << 11) + Emu_Addr + 0x20),
                    0x1F8, 1);
            Emu_Idx++;
        } else {
            dma_execute(3, (void*)RingBase, 0, 0x1F8, control, 1, 0);
        }
        *COMMON_DELAY = 0x1325;
        sector_offset = 0;
        frame_no = 0;
        FinalSector = 1;
        CChannel = CHANNEL;
    } else {
        if (Emu_Addr != 0) {
            mem2mem((u32*)RingBase, (u32*)((Emu_Idx << 11) + Emu_Addr + 0x20),
                    0x1F8, 0);
            Emu_Idx++;
        } else {
            dma_execute(3, (void*)RingBase, 0, 0x1F8, control, 0, 0);
        }
    }
    HeaderArea[0] = 3;
    RingIdx1++;
    if (Emu_Addr != 0 && FinalSector != 0) {
        data_ready_callback();
    }
}

static void dma_execute(int ch, void* addr, int blocks, int words, int control,
                        int cause, int arg6) {
    volatile u32* chcr;
    volatile u32* dma;
    volatile u32* dpcr;
    volatile u32* p;
    u32 base;
    u32 x;
    u32 b;
    int i;

    chcr = (volatile u32*)(0x1F801080 + (ch << 4));
    chcr = (volatile u32*)((u32)chcr | 8);
    for (i = 0; *chcr & 0x01000000; i++) {
        if (i == 0x10000) {
            printf("DMA STATUS ERROR %x\n", *chcr);
            break;
        }
    }
    if (cause == 1) {
        cause = 0x10000;
        cause <<= ch;
        arg6 <<= ch;
        cause |= arg6;
        p = Dma_ICR;
        x = *p;
        x &= 0xFFFFFF;
        x |= cause;
        *p = x;
        *Dma_ICR;
    } else {
        cause = 1 << (ch + 16);
        p = Dma_ICR;
        x = *p;
        x &= 0xFFFFFF;
        x &= ~cause;
        *p = x;
        *Dma_ICR;
    }
    x = ch << 2;
    x += 3;
    b = 1;
    b <<= x;
    chcr = Dma_PCR;
    base = 0x1F801080;
    x = *chcr;
    dma = (volatile u32*)((ch << 4) + base);
    x |= b;
    *chcr = x;
    *dma++ = (u32)addr;
    dma[0] = (blocks << 16) | words;
    dma[1] = control;
}

static void mem2mem(u32* dst, u32* src, u32 num, u32 arg3) {
    u32 i;

    for (i = 0; i < num; i++) {
        *dst++ = *src++;
    }
}

u32 StSetChannel(u32 channel) {
    if (channel < 32) {
        CHANNEL = channel;
        return 0;
    }
    return 1;
}
#else
#include "../types.h"
#define CDROM_DEFINE_ALL
#include "c_002.h"

static s32 HADR0 = 0x1F801800;
static s32 HADR1 = 0x1F801801;
static s32 HADR2 = 0x1F801802;
static s32 HADR3 = 0x1F801803;

void StClearRing(void);

void StSetRing(unsigned long* ring_addr, unsigned long ring_size) {
    StRingAddr = (s32)ring_addr;
    StRingSize = ring_size;
    StClearRing();
}
#endif
