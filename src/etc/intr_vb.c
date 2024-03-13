#include "../types.h"

typedef void (*Callback)();

void trapIntrVSync(void);
void setIntrVSync(int index, Callback callback);
void memclr(int*, int);

extern void InterruptCallback(int, void*);
static Callback D_8002D354[8] = {
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
static volatile u_long* D_8002D378 = 0x1F801114; /* 0x1F801114 - Timer 1 Counter Mode */

void* startIntrVSync(void) {
    *D_8002D378 = 0x107;
    Vcount = 0;
    memclr(&D_8002D354, 8);
    InterruptCallback(0, &trapIntrVSync);
    return &setIntrVSync;
}

static void trapIntrVSync(void) {
    int i;

    Vcount += 1;
    (void)Vcount;

    for (i = 0; i < 8; i++) {
        if (D_8002D354[i] != NULL) {
            D_8002D354[i]();
        }
    }
}

static void setIntrVSync(int index, Callback callback) {
    if (callback != D_8002D354[index]) {
        D_8002D354[index] = callback;
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
