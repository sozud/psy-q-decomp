#if VERSION == 26 || VERSION == 30

typedef void (*Callback)();

extern Callback InterruptCallback(int irq, Callback f);
extern int printf(const char*, ...);

static void intDMA(void);
static Callback setDMACallback(int index, Callback f);

static volatile unsigned long* d_icr = (unsigned long*)0x1F8010F4;
#if VERSION == 26
static Callback DMACallback[8];
static void intDMAError(void);
#else
static Callback DMACallback[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static long initialized = 0;
#endif

void* DMAInit(void) {
    int i;

#if VERSION == 26
    if (InterruptCallback(3, intDMA) != intDMA) {
        *d_icr = 0;
        for (i = 6; i >= 0; i--) {
            DMACallback[i] = 0;
        }
        DMACallback[7] = intDMAError;
    }
#else
    *d_icr = 0;
    for (i = 6; i >= 0; i--) {
        DMACallback[i] = 0;
    }
    InterruptCallback(3, intDMA);
    initialized = 1;
#endif
    return setDMACallback;
}

#if VERSION == 26
static void intDMAError(void) {
    printf("DMA Bus Error: code=%08x\n", *d_icr);
}
#else
void DMAStop(void) {
    int i;

    initialized = 0;
    *d_icr = 0;
    InterruptCallback(3, 0);
    for (i = 6; i >= 0; i--) {
        DMACallback[i] = 0;
    }
}
#endif

static void intDMA(void) {
    int i;
    unsigned long mask;

    while (*d_icr & 0x7F000000) {
        for (i = 0, mask = 0x1000000; i < 7; i++, mask <<= 1) {
            if (*d_icr & mask) {
                *d_icr &= mask | 0xFFFFFF;
                *d_icr;
                if (DMACallback[i] != 0) {
                    DMACallback[i]();
                }
            }
        }
    }
    if ((*d_icr & 0xFF000000) == 0x80000000 || (*d_icr & 0x8000)) {
        printf("DMA bus error: code=%08x\n", *d_icr);
    }
}

static Callback setDMACallback(int index, Callback f) {
    Callback old;

#if VERSION == 30
    if (initialized) {
#endif
        old = DMACallback[index];
        if (f != old) {
            DMACallback[index] = 0;
            if (f != 0) {
                DMACallback[index] = f;
                *d_icr |= 0x800000 | (0x1010000 << index);
            } else {
                *d_icr &= ~(0xFF000000 | (0x10000 << index));
            }
        }
#if VERSION == 30
    }
#endif
    return old;
}

#else

#include "../types.h"

typedef void (*Callback)();

Callback setIntrDMA(int index, Callback callback);
void trapIntrDMA(void);
void memclr(int* ptr, int size);

extern void InterruptCallback(int, void*);
static volatile u_long* d_icr = 0x1F8010F4;
static Callback cbIntrDMA[8] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
static unsigned long* madr_top = 0x1f801080;

void* startIntrDMA(void) {
    memclr(&cbIntrDMA, 8);
    *d_icr = 0;
    InterruptCallback(3, &trapIntrDMA);
    return &setIntrDMA;
}

static void trapIntrDMA(void) {
    u32 mask;
    int i;

    while ((mask = (*d_icr >> 24) & 0x7f) != 0) {
        for (i = 0; mask != 0 && i < 7; i++, mask >>= 1) {
            if (mask & 1) {
                *d_icr &= (0xffffff | (1 << (i + 24)));
                if (cbIntrDMA[i] != NULL) {
                    cbIntrDMA[i]();
                }
            }
        }
    }

    if ((*d_icr & 0xFF000000) == 0x80000000 || *d_icr & 0x8000) {
        printf("DMA bus error: code=%08x\n", *d_icr);
        for (i = 0; i < 7; i++) {
            printf("MADR[%d]=%08x\n", i, madr_top[4 * i]);
        }
    }
}

static Callback setIntrDMA(int index, Callback callback) {
    Callback prev = cbIntrDMA[index];
    if (callback != prev) {
        if (callback != NULL) {
            cbIntrDMA[index] = callback;
            *d_icr =
                (*d_icr & 0xFFFFFF) | 0x800000 | ((1 << (index + 16)));
        } else {
            cbIntrDMA[index] = 0;
            *d_icr =
                ((*d_icr & 0xFFFFFF) | 0x800000) & ~(1 << (index + 16));
        }
    }
    return prev;
}

static void memclr(int* ptr, int size) {
    int i;
    int* e = ptr;

    for (i = size - 1; i != -1; i--) {
        *e = 0;
        e++;
    }
}

#endif
