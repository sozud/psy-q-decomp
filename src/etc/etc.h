#ifndef LIBETC_H
#define LIBETC_H

#define JB_PC 0
#define JB_SP 1
#define JB_FP 2
#define JB_S0 3
#define JB_S1 4
#define JB_S2 5
#define JB_S3 6
#define JB_S4 7
#define JB_S5 8
#define JB_S6 9
#define JB_S7 10
#define JB_GP 11
#define JB_SIZE 12

typedef int jmp_buf[JB_SIZE];
typedef void (*Callback)();
struct intrEnv_t {
    u16 interruptsInitialized; // 2
    u16 inInterrupt;           // 2
    Callback handlers[11];     // 44
    u16 enabledInterruptsMask; // 2
    u16 savedMask;             // 2
    struct Temp {
        int savedPcr; // 4
        jmp_buf buf;  // 48
        s32 stack[1024];
    } temp;
};

struct Callbacks {
    const char* rcsid; /* "$Id: intr.c,v 1.73 1995/11/10 05:29:40 suzu Exp $" */
    void* (*DMACallback)(int dma, void (*func)());
    Callback (*InterruptCallback)(int irq, Callback f);
    void* (*ResetCallback)(void);
    void* (*StopCallback)(void);
    int (*VSyncCallbacks)(int ch, void (*f)());
    void* (*RestartCallback)(void);
    void* D_8002C2B8;
};

extern struct Callbacks* D_8002D340;

void VSyncCallback(void (*f)()); /* 13 */

extern void ChangeClearPAD(long); /* 28 */

int VSync(int mode);
int VSyncCallbacks(int ch, void (*f)());

void* DMACallback(int dma, void (*func)());
void* ResetCallback(void);
void* StopCallback(void);
void* RestartCallback(void);
int CheckCallback(void);

long SetVideoMode(long mode);

void PadInit(int mode);
u_long PadRead(int id);
void PadStop(void);

#endif
