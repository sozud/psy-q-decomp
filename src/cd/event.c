#include "../types.h"
#include <KERNEL.H>

int printf(const char*, ...);
#if VERSION == 26
int CdReset();
#else
int CdReset(int);
#endif
void* CdSyncCallback(void (*)());
void* CdReadyCallback(void (*)());
void* CdReadCallback(void (*)());

static inline void def_cbsync(u8 intr, u8* result) {
    DeliverEvent(HwCdRom, EvSpCOMP);
}

static inline void def_cbready(u8 intr, u8* result) {
    DeliverEvent(HwCdRom, EvSpDR);
}

static inline void def_cbread(u8 intr, u8* result) {
    DeliverEvent(HwCdRom, EvSpDR);
}

#if VERSION == 26
void _96_vec(void);
int VSync(int);

void CdInit(void) {
    CdReset();
    CdSyncCallback(def_cbsync);
    CdReadyCallback(def_cbready);
    CdReadCallback(def_cbread);
}

void CdInitFileSystem(void) {
    _96_vec();
    VSync(0);
    CdReset();
    VSync(0);
    CdSyncCallback(def_cbsync);
    VSync(0);
    CdReadyCallback(def_cbready);
    VSync(0);
    CdReadCallback(def_cbread);
}
#elif VERSION == 30
int CD_init(void);
void _96_vec(void);
int VSync(int);

void CdInit(void) {
    int i = 4;

    while (CD_init()) {
        if (i-- == 0) {
            printf("CdInit: Init failed\n");
            return;
        }
    }
    CdSyncCallback(def_cbsync);
    CdReadyCallback(def_cbready);
    CdReadCallback(def_cbread);
}

void CdInitFileSystem(void) {
    _96_vec();
    VSync(0);
    CD_init();
    VSync(0);
    CdSyncCallback(def_cbsync);
    VSync(0);
    CdReadyCallback(def_cbready);
    VSync(0);
    CdReadCallback(def_cbread);
}
#elif VERSION == 37
int CdInit(void) {
    int i = 4;

retry:
    if (CdReset(1) != 1) {
        if (i-- != 0) {
            goto retry;
        }
        printf("CdInit: Init failed\n");
        return 0;
    }
    CdSyncCallback(def_cbsync);
    CdReadyCallback(def_cbready);
    CdReadCallback(def_cbread);
    return 1;
}
#elif VERSION >= 43
extern void (*CD_cbsync)();
extern void (*CD_cbready)();
int CD_init(void);
int CD_initvol(void);

void (*CD_cbread)() = 0;
int CD_read_dma_mode = 0;

static int CdReset2(void);

int CdInit(void) {
    int i = 4;

    while (CdReset2() != 1) {
        if (!i--) {
            printf("CdInit: Init failed\n");
            return 0;
        }
    }
    CD_cbsync = def_cbsync;
    CD_cbready = def_cbready;
    CD_cbread = def_cbread;
    CD_read_dma_mode = 0;
    return 1;
}

static int CdReset2(void) {
    if (CD_init()) {
        return 0;
    }
    if (CD_initvol()) {
        return 0;
    }
    return 1;
}
#elif VERSION >= 40
int CdReadMode(int);
int CdInit(void) {
    int i = 4;

    while (CdReset(1) != 1) {
        if (!i--) {
            printf("CdInit: Init failed\n");
            return 0;
        }
    }
    CdSyncCallback(def_cbsync);
    CdReadyCallback(def_cbready);
    CdReadCallback(def_cbread);
    CdReadMode(0);
    return 1;
}
#else
int CdInit(void) {
    unsigned int count = 5;

    while (count--) {
        if (CdReset(1) == 1) {
            CdSyncCallback(def_cbsync);
            CdReadyCallback(def_cbready);
            CdReadCallback(def_cbread);
            count = 1;
            return (float)count;
        }
    }

    printf("CdInit: Init failed\n");
    return 0;
}
#endif
