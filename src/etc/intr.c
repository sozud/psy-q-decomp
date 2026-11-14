#if VERSION == 26 || VERSION == 30
typedef void (*Callback)();

struct Callbacks {
    char* rcsid;
    Callback (*DMACallback)(int dma, Callback f);
    Callback (*InterruptCallback)(int irq, Callback f);
    void (*ResetCallback)(void);
    void (*StopCallback)(void);
    Callback (*VSyncCallback)(Callback f);
};

extern int setjmp(long*);
extern int printf(const char*, ...);
extern void HookEntryInt(long*);
extern void ReturnFromException(void);
extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void _96_remove();
extern void* VBLNKInit(void);
extern void* DMAInit(void);
#if VERSION == 30
extern void VBLNKStop(void);
extern void DMAStop(void);
#endif

static void intInit(void);
static Callback setIntCallback(int irq, Callback f);
static void stopInit(void);
#if VERSION == 26
static void my_96_remove(void);
#endif

static struct Callbacks jtb = {
#if VERSION == 26
    "$Id: intr.c,v 3.52 1994/12/26 12:49:57 suzu Exp suzu $",
#else
    "$Id: intr.c,v 1.52 1995/03/14 12:20:44 hakama Exp $",
#endif
    0,
    setIntCallback,
    intInit,
    stopInit,
    0,
};

static struct Callbacks* jtbp = &jtb;
static long initialized = 0;
static long inInterrupt = 0;

void ResetCallback(void) { jtbp->ResetCallback(); }

Callback InterruptCallback(int irq, Callback f) {
    return jtbp->InterruptCallback(irq, f);
}

Callback DMACallback(int dma, Callback f) {
    return jtbp->DMACallback(dma, f);
}

Callback VSyncCallback(Callback f) { return jtbp->VSyncCallback(f); }

void StopCallback(void) { jtbp->StopCallback(); }

int CheckCallback(void) { return inInterrupt; }

static volatile unsigned short* i_stat = (unsigned short*)0x1F801070;
static volatile unsigned short* i_mask = (unsigned short*)0x1F801074;
#if VERSION == 26
static volatile long cb_mask;
static long stack[1024];
static Callback IntCallback[11];
#else
static Callback IntCallback[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static volatile long cb_mask = 0;
static long stack[1024];
#endif

static void intInit(void) {
    static long rtc_point[12];
    int i;
    int count;
    unsigned long bit;

    if (initialized) {
        return;
    }
    EnterCriticalSection();
#if VERSION == 26
    my_96_remove();
#endif
    cb_mask = 0;
    for (i = 10; i >= 0; i--) {
        IntCallback[i] = 0;
    }
    if (setjmp(rtc_point)) {
        inInterrupt = 1;
        count = 0x100;
        while (*i_stat & cb_mask & *i_mask) {
            if (count-- == 0) {
                printf("Too many interrupts\n");
                break;
            }
            for (i = 0, bit = 1; i < 11; i++, bit <<= 1) {
                if (bit & cb_mask & *i_stat & *i_mask) {
                    *i_stat = ~bit;
                    *i_stat;
                    if (IntCallback[i] != 0) {
                        IntCallback[i]();
                    }
                }
            }
        }
        inInterrupt = 0;
        ReturnFromException();
    }
#if VERSION == 26
    rtc_point[1] = (long)&stack[1004];
    HookEntryInt(rtc_point);
    initialized = 1;
    jtbp->VSyncCallback = VBLNKInit();
    jtbp->DMACallback = DMAInit();
#else
    initialized = 1;
    jtbp->VSyncCallback = VBLNKInit();
    jtbp->DMACallback = DMAInit();
    _96_remove(jtbp);
    rtc_point[1] = (long)&stack[1004];
    HookEntryInt(rtc_point);
#endif
    ExitCriticalSection();
}

static Callback setIntCallback(int irq, Callback f) {
    Callback old;

#if VERSION == 30
    if (initialized) {
#endif
        old = IntCallback[irq];
        if (f != old) {
            IntCallback[irq] = 0;
            if (f != 0) {
                IntCallback[irq] = f;
                *i_mask |= 1 << irq;
                cb_mask |= 1 << irq;
            } else {
                *i_mask &= ~(1 << irq);
                cb_mask &= ~(1 << irq);
            }
        }
#if VERSION == 30
    }
#endif
    return old;
}

static void stopInit(void) {
    int i;
#if VERSION == 30
    volatile long timeout = 0x80000;
#endif

    EnterCriticalSection();
#if VERSION == 30
    initialized = 0;
    cb_mask = 0;
#endif
    *i_mask = *i_stat = 0;
    for (i = 10; i >= 0; i--) {
        IntCallback[i] = 0;
    }
#if VERSION == 26
    initialized = 0;
#else
    DMAStop();
    VBLNKStop();
    while (--timeout != -1) {
    }
#endif
    ExitCriticalSection();
}

#if VERSION == 26
static void my_96_remove(void) {
    volatile long* gp0 = (long*)0x1F801810;
    volatile long* gp1 = (long*)0x1F801814;
    long unused[2];

    *gp0 = (*gp1 & 0x3FFF) | 0xE1001000;
    *gp0 = (*gp1 & 0x3FFF) | 0xE1001000;
    *gp0 = (*gp1 & 0x3FFF) | 0xE1001000;
    if (!(*gp1 & 0x1000)) {
        printf("ResetCallback: _96_remove ..\n");
        _96_remove(jtbp);
    }
}
#endif

#else

#include "../types.h"
#include "etc.h"

typedef struct intrEnv_t intrEnv_t;
static Callback setIntr(s32 arg0, Callback arg1);
static void* startIntr();
static void* stopIntr();
static void* restartIntr();
static void memclr(void* ptr, s32 size);
static void trapIntr();
s32 setjmp(s32*);
void* startIntrVSync(); /* extern */
long long startIntrDMA();

#if VERSION >= 40
static char copyright[] = "Library Programs (c) 1993-1997 Sony Computer Entertainment Inc., All Rights Reserved.";
#endif

static struct intrEnv_t intrEnv = {
    0, // interruptsInitialized
    0, // inInterrupt
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0}, // handlers (explicit zeros for each element)
    0,   // enabledInterruptsMask
    0,   // savedMask
    0,
};

static struct Callbacks jtb = {
#if VERSION >= 43
    "$Id: intr.c,v 1.75 1997/02/07 09:00:36 makoto Exp $",
#elif VERSION >= 40
    "$Id: intr.c,v 1.76 1997/02/12 12:45:05 makoto Exp $",
#elif VERSION == 37
    "$Id: intr.c,v 1.74 1996/12/04 07:30:16 makoto Exp $",
#elif VERSION == 33
    "$Id: intr.c,v 1.71 1995/08/29 11:01:44 suzu Exp $",
#else
    "$Id: intr.c,v 1.73 1995/11/10 05:29:40 suzu Exp $",
#endif
    0,
    setIntr,
    startIntr,
    stopIntr,
    0,
    restartIntr,
    &intrEnv};

static struct Callbacks* jtbp = &jtb;

void* ResetCallback(void) { return jtbp->ResetCallback(); }

void InterruptCallback(int irq, void (*f)()) {
    jtbp->InterruptCallback(irq, f);
}

void* DMACallback(int dma, void (*func)()) {
    return jtbp->DMACallback(dma, func);
}

#if VERSION >= 37
void VSyncCallback(void (*f)()) { jtbp->VSyncCallbacks(4, f); }
#else
void VSyncCallback(void (*f)()) { jtbp->VSyncCallbacks(0, f); }
#endif

int VSyncCallbacks(int ch, void (*f)()) {
    return jtbp->VSyncCallbacks(ch, f);
}

void* StopCallback(void) { return jtbp->StopCallback(); }

void* RestartCallback(void) { return jtbp->RestartCallback(); }

int CheckCallback(void) { return intrEnv.inInterrupt; }

static volatile u16* i_reg = (u16*)0x1F801070;
static volatile u16* i_mask = (u16*)0x1F801074;
static volatile s32* d_pcr = (s32*)0x1F8010F0;

u16 GetIntrMask(void) { return *i_mask; }

u16 SetIntrMask(u16 arg0) {
    u16 mask;

    mask = *i_mask;
    *i_mask = arg0;
    return mask;
}

void* startIntr() {
    if (intrEnv.interruptsInitialized != 0) {
        return NULL;
    }
    *i_reg = *i_mask = 0;
    *d_pcr = 0x33333333;
    memclr(&intrEnv, sizeof(intrEnv) / sizeof(s32));
    if (setjmp(intrEnv.buf) != 0) {
        trapIntr();
    }
    intrEnv.buf[JB_SP] = (s32)&intrEnv.stack[1004];
    HookEntryInt((u16*)intrEnv.buf);
    intrEnv.interruptsInitialized = 1;
    jtbp->VSyncCallbacks = startIntrVSync();
    jtbp->DMACallback = startIntrDMA();
    _96_remove();
    ExitCriticalSection();
    return &intrEnv;
}

void trapIntr() {
    s32 i;
    u16 mask;
    static s32 nintr = 0;

    if (intrEnv.interruptsInitialized == 0) {
        printf("unexpected interrupt(%04x)\n", *i_reg);
        ReturnFromException();
    }
    intrEnv.inInterrupt = 1;
    while ((mask = (intrEnv.enabledInterruptsMask & *i_reg) & *i_mask) != 0) {
        for (i = 0; mask && i < 11; ++i, mask >>= 1) {
            if (mask & 1) {
                *i_reg = ~(1 << i);
                if (intrEnv.handlers[i] != NULL) {
                    intrEnv.handlers[i]();
                }
            }
        }
    }
    if (*i_reg & *i_mask) {
        if (nintr++ > 0x800) {
            printf("intr timeout(%04x:%04x)\n", *i_reg, *i_mask);
            nintr = 0;
            *i_reg = 0;
        }
    } else {
        nintr = 0;
    }
    intrEnv.inInterrupt = 0;
    ReturnFromException();
}

Callback setIntr(s32 arg0, Callback arg1) {
    Callback temp_s4;
    u16 temp_v1;
    s32 var_s3;

    temp_s4 = intrEnv.handlers[arg0];
    if ((arg1 != temp_s4) && (intrEnv.interruptsInitialized != 0)) {
        temp_v1 = *i_mask;
        *i_mask = 0;
        var_s3 = temp_v1 & 0xFFFF;
        if (arg1 != 0) {
            intrEnv.handlers[arg0] = arg1;
            var_s3 = var_s3 | (1 << arg0);
            intrEnv.enabledInterruptsMask |= (1 << arg0);
        } else {
            intrEnv.handlers[arg0] = 0;
            var_s3 = var_s3 & ~(1 << arg0);
            intrEnv.enabledInterruptsMask &= ~(1 << arg0);
        }
        if (arg0 == 0) {
            ChangeClearPAD(arg1 == 0);
            ChangeClearRCnt(3, arg1 == 0);
        }
        if (arg0 == 4) {
            ChangeClearRCnt(0, arg1 == 0);
        }
        if (arg0 == 5) {
            ChangeClearRCnt(1, arg1 == 0);
        }
        if (arg0 == 6) {
            ChangeClearRCnt(2, arg1 == 0);
        }
        *i_mask = var_s3;
    }
    return temp_s4;
}

void* stopIntr() {
    if (intrEnv.interruptsInitialized == 0) {
        return NULL;
    }
    EnterCriticalSection();
    intrEnv.savedMask = *i_mask;
    intrEnv.savedPcr = *d_pcr;
    *i_reg = *i_mask = 0;
    *d_pcr &= 0x77777777;
    ResetEntryInt();
    intrEnv.interruptsInitialized = 0;
    return &intrEnv;
}

void* restartIntr() {
    if (intrEnv.interruptsInitialized != 0) {
        return 0;
    }

    HookEntryInt((u16*)intrEnv.buf);
    intrEnv.interruptsInitialized = 1;
    *i_mask = intrEnv.savedMask;
    *d_pcr = intrEnv.savedPcr;
    ExitCriticalSection();
    return &intrEnv;
}

void memclr(void* ptr, s32 size) {
    s32 i;
    s32* e = (s32*)ptr;

    for (i = size - 1; i != -1; i--) {
        *e = 0;
        e++;
    }
}

#endif
