#if VERSION == 26 || VERSION == 30

typedef void (*Callback)();

extern Callback InterruptCallback(int irq, Callback f);
extern void ChangeClearPAD(long);
extern void ChangeClearRCnt(long, long);

static void intVBLNK(void);
static Callback setVBLNKCallback(Callback f);
extern volatile long Vcount;
static Callback VBLNKCallback;

static volatile unsigned long* rcnt1_count = (unsigned long*)0x1F801110;
static volatile unsigned long* rcnt1_mode = (unsigned long*)0x1F801114;
static volatile unsigned long* rcnt1_target = (unsigned long*)0x1F801118;
#if VERSION == 30
static long initialized = 0;
#endif

void* VBLNKInit(void) {
#if VERSION == 26
    if (InterruptCallback(0, intVBLNK) != intVBLNK) {
        *rcnt1_target = -1;
        *rcnt1_mode = 0x148;
        ChangeClearPAD(0);
        ChangeClearRCnt(3, 0);
        VBLNKCallback = 0;
    }
#else
    *rcnt1_target = -1;
    *rcnt1_mode = 0x148;
    ChangeClearPAD(0);
    ChangeClearRCnt(3, 0);
    VBLNKCallback = 0;
    InterruptCallback(0, intVBLNK);
    initialized = 1;
#endif
    return setVBLNKCallback;
}

#if VERSION == 30
void VBLNKStop(void) {
    initialized = 0;
    InterruptCallback(0, 0);
    VBLNKCallback = 0;
}
#endif

volatile long Vcount = 0;

static void intVBLNK(void) {
    Vcount++;
    if (VBLNKCallback != 0) {
        VBLNKCallback();
    }
}

#if VERSION == 26
static Callback setVBLNKCallback(Callback f) {
    VBLNKInit();
    if (f != VBLNKCallback) {
        VBLNKCallback = f;
    }
}
#else
static Callback setVBLNKCallback(Callback f) {
    Callback old;

    VBLNKInit();
    if (initialized) {
        old = VBLNKCallback;
        if (f != old) {
            VBLNKCallback = f;
        }
    }
    return old;
}
#endif

#else

#include "../types.h"

typedef void (*Callback)();

void trapIntrVSync(void);
void setIntrVSync(int index, Callback callback);
void memclr(int*, int);

extern void InterruptCallback(int, void*);
static Callback cbIntrVSync[8] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
volatile int Vcount = 0;

void* startIntrVSync(void) {
    static volatile u_long* t1_mode = 0x1F801114;

    *t1_mode = 0x107;
    Vcount = 0;
    memclr(&cbIntrVSync, 8);
    InterruptCallback(0, &trapIntrVSync);
    return &setIntrVSync;
}

static void trapIntrVSync(void) {
    int i;

    Vcount += 1;
    (void)Vcount;

    for (i = 0; i < 8; i++) {
        if (cbIntrVSync[i] != NULL) {
            cbIntrVSync[i]();
        }
    }
}

static void setIntrVSync(int index, Callback callback) {
    if (callback != cbIntrVSync[index]) {
        cbIntrVSync[index] = callback;
    }
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
