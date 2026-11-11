#if VERSION <= 30

#include "libcd.h"

extern volatile int Vcount;
extern int printf();
extern int CheckCallback();
extern void ResetCallback();
extern void *InterruptCallback();

#if VERSION == 26
int CD_status = 0;
int CD_debug = 0;
#else
int CD_debug = 0;
int CD_isopen = 0;
#endif

char *CD_comstr[] = {
    "CdlSync",     "CdlNop",      "CdlSetloc",   "CdlPlay",     "CdlForward",
    "CdlBackword", "CdlReadN",    "CdlStandby",  "CdlStop",     "CdlPause",
    "CdlReset",    "CdlMute",     "CdlDemute",   "CdlSetfilter", "CdlSetmode",
    "CdlGetparam", "CdlGetlocL",  "CdlGetlocP",  "CdlReadT",    "CdlGetTN",
    "CdlGetTD",    "CdlSeekL",    "CdlSeekP",    "CdlSetclock", "CdlGetclock",
    "CdlTest",     "CdlId",       "CdlReadS",    "CdlInit",
};

char *CD_intstr[] = {
    "NoIntr", "DataReady", "Complete", "Acknowledge",
    "DataEnd", "DiskError", "Unkown", "Unkown",
};

#if VERSION == 26
static volatile int nsec = 0;
static u_char com = 0;

static volatile u_char *reg0 = (u_char *)0x1F801800;
static volatile u_char *reg1 = (u_char *)0x1F801801;
static volatile u_char *reg2 = (u_char *)0x1F801802;
static volatile u_char *reg3 = (u_char *)0x1F801803;
static volatile u_short *spu = (u_short *)0x1F801C00;
#endif

static int ComSync[32] = {
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static int ComReady[32] = {
    0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
#if VERSION != 26

static int ComStat[32] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};
#endif

static int ComParm[32] = {
    0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0,
    0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

#if VERSION == 26

extern void CdCallback();
extern int putchar();

static volatile u_char Intr[8];
static int cmpintr;
static int rdyintr;
static u_long *bufp;
static int size;
static void (*func)();
static u_char Result[8][8];

void (*CD_cbsync)();
void (*CD_cbready)();
void (*CD_cbread)();

static int getintr();
static void cb_read(int intr, u_char *result);
int CD_cw(u_char c, u_char *param, u_char *result, int async);
int CD_sync(int mode, u_char *result);
int CD_read(u_long *addr, int size);

static void rescpy(u_char *dst, u_char *src)
{
    int i;

    if (dst) {
        for (i = 7; i != -1; i--) {
            *dst++ = *src++;
        }
    }
}

static void timeout(u_char c)
{
    int i;

    printf("%s: timeout\n", CD_comstr[c]);
    for (i = 0; i < 6; i++) {
        printf("%s: %d\n", CD_intstr[i], Intr[i]);
        Intr[i] = 0;
    }
    CD_cw(CdlReset, 0, 0, 0);
    CD_sync(0, 0);
    putchar('\n');
}

static void pollintr(void)
{
    if (CheckCallback()) {
        while (getintr())
            ;
    }
}

static int getintr(void)
{
    int intr;
    int i;

    *reg0 = 1;
    if ((intr = *reg3 & 7) == 0) {
        return 0;
    }
    for (i = 0; i < 8 && (*reg0 & 0x20); i++) {
        Result[intr][i] = *reg1;
    }
    Intr[intr] = 1;
    rdyintr = 0;
    cmpintr = 0;
    switch (intr) {
    case 2:
        Intr[3] = 1;
        cmpintr = 2;
        break;
    case 3:
        if (ComSync[com] == 0) {
            Intr[2] = 1;
            cmpintr = 2;
            rescpy(Result[2], Result[intr]);
        }
        if (Result[intr][0] & 1) {
            Intr[2] = Intr[5] = 1;
            cmpintr = 5;
            rescpy(Result[2], Result[intr]);
            rescpy(Result[5], Result[intr]);
        }
        break;
    case 5:
        if (Result[intr][0] & 1) {
            Intr[3] = Intr[2] = 1;
            cmpintr = intr;
            rescpy(Result[3], Result[intr]);
            rescpy(Result[2], Result[intr]);
            break;
        }
    case 1:
    case 4:
        rdyintr = intr;
        break;
    default:
        printf("bios: unknown intr(%d)\n", intr);
        break;
    }
    *reg0 = 1;
    *reg3 = 7;
    *reg2 = 7;
    return intr;
}

static void callback(void)
{
    int mode;

    mode = *reg0 & 3;
    while (getintr()) {
        if (rdyintr) {
            if (CD_cbready) {
                CD_cbready(rdyintr, Result[rdyintr]);
            }
        }
        if (cmpintr) {
            if (CD_cbsync) {
                CD_cbsync(cmpintr, Result[cmpintr]);
            }
        }
    }
    *reg0 = mode;
}

int CD_init(void)
{
    u_char vol[4];

    Intr[0] = Intr[1] = 0;
    Intr[4] = Intr[5] = 0;
    Intr[2] = Intr[3] = 1;
    CD_cbready = 0;
    CD_cbsync = 0;
    nsec = 0;
    ResetCallback();
    CdCallback(callback);
    CD_cw(CdlReset, 0, 0, 0);
    CD_sync(0, 0);
    if (spu[0x1B8 / 2] == 0 && spu[0x1BA / 2] == 0) {
        spu[0x180 / 2] = 0x3FFF;
        spu[0x182 / 2] = 0x3FFF;
    }
    spu[0x1B0 / 2] = 0x3FFF;
    spu[0x1B2 / 2] = 0x3FFF;
    spu[0x1AA / 2] = 0xC001;
    vol[0] = vol[2] = 0x80;
    vol[1] = vol[3] = 0;
    return CD_vol(vol);
}

int CD_sync(int mode, u_char *result)
{
    int cnt;

    cnt = 0x800000;
    while (cnt--) {
        pollintr();
        if (Intr[5]) {
            if (CD_debug) {
                printf("CD_sync: %s: DiskError:\n", CD_comstr[com]);
            }
            Intr[5] = 0;
            rescpy(result, Result[5]);
            return 5;
        }
        if (Intr[2]) {
            rescpy(result, Result[2]);
            return 2;
        }
        if (mode) {
            return 0;
        }
    }
    timeout(com);
    return -1;
}

int CD_ready(int mode, u_char *result)
{
    int ret;
    int cnt;

    ret = 0;
    cnt = 0x8000000;
    while (cnt--) {
        pollintr();
        if (Intr[5]) {
            ret = 5;
        }
        if (Intr[4]) {
            ret = 4;
        }
        if (Intr[1]) {
            ret = 1;
        }
        if (ret) {
            if (CD_debug) {
                printf("CD_ready: %s: %s\n", CD_comstr[com], CD_intstr[ret]);
            }
            Intr[ret] = 0;
            rescpy(result, Result[ret]);
            return ret;
        }
        if (mode) {
            return 0;
        }
    }
    timeout(com);
    return -1;
}

int CD_readsync(int mode, u_char *result)
{
    int ret;
    int cnt;

    ret = 0;
    cnt = 0x800000;
    while (cnt--) {
        if (Intr[4]) {
            ret = 4;
        }
        if (Intr[5]) {
            ret = 5;
        }
        if (ret) {
            if (CD_debug) {
                printf("CD_ready: %s (%d)\n", CD_intstr[ret], nsec);
            }
            Intr[ret] = 0;
            rescpy(result, Result[ret]);
            return -1;
        }
        if (mode || nsec == 0) {
            return nsec;
        }
    }
    timeout(com);
    return -1;
}

int CD_cw(u_char c, u_char *param, u_char *result, int async)
{
    int i;
    int cnt;

    if (CD_sync(0, 0) != 2) {
        if (CD_debug) {
            printf("%s: DiskError\n", CD_comstr[com]);
        }
    }
    if (CD_debug) {
        printf("%s...\n", CD_comstr[c]);
    }
    if (ComParm[c] && param == 0) {
        if (CD_debug) {
            printf("%s: bad param\n", CD_comstr[c]);
        }
        return 2;
    }
    Intr[3] = Intr[2] = Intr[5] = 0;
    if (ComReady[c]) {
        Intr[1] = 0;
    }
    *reg0 = 0;
    for (i = 0; i < ComParm[c]; i++) {
        *reg2 = *param++;
    }
    com = c;
    *reg1 = c;
    if (async) {
        return 0;
    }
    cnt = 0x800000;
    while (Intr[3] == 0) {
        if (cnt-- == 0) {
            timeout(c);
            return -1;
        }
        pollintr();
    }
    rescpy(result, Result[3]);
    if (Intr[5]) {
        if (CD_debug) {
            printf("%s: DiskError\n", CD_comstr[c]);
        }
        Intr[5] = 0;
        return -1;
    }
    return 0;
}

int CD_vol(u_char *vol)
{
    if (CD_sync(0, 0) != 2) {
        if (CD_debug) {
            printf("%s: DiskError\n", CD_comstr[com]);
        }
    }
    *reg0 = 2;
    *reg2 = vol[0];
    *reg3 = vol[1];
    *reg0 = 3;
    *reg1 = vol[2];
    *reg2 = vol[3];
    *reg3 = 0x20;
    return 0;
}

static volatile int *cd_delay = (int *)0x1F801018;
static volatile int *com_delay = (int *)0x1F801020;
static volatile int *dpcr = (int *)0x1F8010F0;
static volatile int *madr = (int *)0x1F8010B0;
static volatile int *bcr = (int *)0x1F8010B4;
static volatile int *chcr = (int *)0x1F8010B8;

int CD_read(u_long *addr, int size)
{
    int cnt;

    cnt = 0x800000;
    while (*chcr & 0x01000000) {
        if (cnt-- == 0) {
            timeout(0);
            return -1;
        }
    }
    *reg0 = 0;
    *reg3 = 0x80;
    *cd_delay = 0x20943;
    *com_delay = 0x132C;
    *dpcr |= 0x8000;
    *chcr = 0;
    *madr = (int)addr;
    *bcr = size | 0x10000;
    *chcr = 0x11000000;
    return 0;
}

int CD_readm(u_long *addr, int n, int sz, u_char *result)
{
    int ret;

    nsec = n;
    size = sz;
    bufp = addr;
    while ((ret = CD_ready(1, 0))) {
        if (CD_debug) {
            printf("CD_readm: skipping %s...\n", CD_intstr[ret]);
        }
    }
    func = CD_cbready;
    CD_cbready = cb_read;
    return CD_cw(CdlReadN, 0, result, 0);
}

static void cb_read(int intr, u_char *result)
{
    u_char unused[8];

    if (intr == 1) {
        if (nsec > 0) {
            CD_read(bufp, size);
            bufp += size;
            nsec--;
        }
        CD_status = intr;
    } else {
        if (CD_debug) {
            printf("CD_readm: error (sector=%d)\n", nsec);
        }
        nsec = -1;
    }
    if (nsec == 0) {
        CD_cw(CdlPause, 0, 0, 0);
        CD_cbready = func;
        if (CD_cbread) {
            CD_cbread(intr, result);
        }
    }
}

void CD_set_test_parmnum(int n)
{
    ComParm[25] = n;
}

#else

static volatile u_char *reg0 = (u_char *)0x1F801800;
static volatile u_char *reg1 = (u_char *)0x1F801801;
static volatile u_char *reg2 = (u_char *)0x1F801802;
static volatile u_char *reg3 = (u_char *)0x1F801803;
static volatile u_short *spu = (u_short *)0x1F801C00;

static volatile u_char com = 0;
static volatile int Sync = 0;
static volatile int Ready = 0;

static volatile u_char SyncResult[8];
static volatile u_char ReadyResult[8];
static int target;

void (*CD_cbsync)();
void (*CD_cbready)();
void (*CD_cbread)();

static int getintr();

static void rescpy(u_char *dst, u_char *src)
{
    int i;

    if (dst) {
        for (i = 7; i != -1; i--) {
            *dst++ = *src++;
        }
    }
}

static void set_alarm(void)
{
    target = Vcount + 480;
}

static int get_alarm(void)
{
    if (target < Vcount) {
        printf("%s: timeout Sync=%s, Ready=%s\n", CD_comstr[com],
               CD_intstr[Sync], CD_intstr[Ready]);
        return -1;
    }
    return 0;
}

static void pollintr(void)
{
    if (CheckCallback()) {
        getintr();
    }
    if (Sync == 5 && CD_debug > 0) {
        printf("%s: DiskError(%02x)\n", CD_comstr[com], SyncResult[0]);
    }
    if (Ready == 5 && CD_debug > 0) {
        printf("Ready=DiskError(%02x)\n", ReadyResult[0]);
    }
}

static void callback(void)
{
    int mode;
    int intr;

    mode = *reg0 & 3;
    while ((intr = getintr())) {
        if (intr & 4) {
            if (CD_cbready) {
                CD_cbready(Ready, ReadyResult);
            }
        }
        if (intr & 2) {
            if (CD_cbsync) {
                CD_cbsync(Sync, SyncResult);
            }
        }
    }
    *reg0 = mode;
}

static int getintr(void)
{
    static u_char result[8];
    static u_char last_intr;
    static int intr;
    static int i;
    static int iserror;

    *reg0 = 1;
    if ((intr = *reg3 & 7) == 0) {
        return 0;
    }
    for (i = 0; *reg0 & 0x20;) {
        result[i] = *reg1;
        i++;
        if (i >= 8) {
            goto done;
        }
    }
    for (; i < 8; i++) {
        result[i] = 0;
    }
done:
    *reg0 = 1;
    *reg3 = 7;
    *reg2 = 7;

    iserror = ComStat[com] && (result[0] & 0x1C);
    CD_isopen = ComStat[com] && (result[0] & 0x10);

    switch (last_intr = intr) {
    case 3:
        if (iserror) {
            Sync = 5;
            rescpy(SyncResult, result);
            return 2;
        }
        if (Sync == 0 && ComSync[com]) {
            Sync = 3;
            rescpy(SyncResult, result);
            return 1;
        }
        Sync = 2;
        rescpy(SyncResult, result);
        return 2;
    case 2:
        Sync = iserror ? 5 : 2;
        rescpy(SyncResult, result);
        return 2;
    case 1:
        Ready = iserror ? 5 : 1;
        rescpy(ReadyResult, result);
        return 4;
    case 4:
        Ready = iserror ? 5 : 4;
        rescpy(ReadyResult, result);
        return 4;
    case 5:
        Ready = 5;
        Sync = Ready;
        rescpy(SyncResult, result);
        rescpy(ReadyResult, result);
        return 6;
    default:
        printf("CDROM: unknown intr(%d)\n", intr);
        return -1;
    }
}

int CD_sync(int mode, u_char *result)
{
    set_alarm();
    do {
        if (get_alarm()) {
            return -1;
        }
        pollintr();
        if (Sync == 2 || Sync == 5) {
            rescpy(result, SyncResult);
            return Sync;
        }
    } while (mode == 0);
    return 0;
}

int CD_ready(int mode, u_char *result)
{
    int ret;

    set_alarm();
    do {
        if (get_alarm()) {
            return -1;
        }
        pollintr();
        ret = Ready;
        if (ret) {
            Ready = 0;
            rescpy(result, ReadyResult);
        }
        if (ret) {
            return ret;
        }
    } while (mode == 0);
    return 0;
}

int CD_cw(u_char c, u_char *param, u_char *result, int async)
{
    int i;

    set_alarm();
    while (Sync != 2 && Sync != 5) {
        if (get_alarm()) {
            return -1;
        }
        pollintr();
    }
    if (CD_debug > 1) {
        printf("%s...\n", CD_comstr[c]);
    }
    if (ComParm[c] && param == 0) {
        if (CD_debug > 0) {
            printf("%s: no param\n", CD_comstr[c]);
        }
        return 2;
    }
    Sync = 0;
    if (ComReady[c]) {
        Ready = 0;
    }
    *reg0 = 0;
    for (i = 0; i < ComParm[c]; i++) {
        *reg2 = *param++;
    }
    com = c;
    *reg1 = com;
    if (async) {
        return 0;
    }
    set_alarm();
    while (Sync == 0) {
        if (get_alarm()) {
            return -1;
        }
        pollintr();
    }
    rescpy(result, SyncResult);
    return -(Sync == 5);
}

int CD_vol(u_char *vol)
{
    CD_sync(0, 0);
    *reg0 = 2;
    *reg2 = vol[0];
    *reg3 = vol[1];
    *reg0 = 3;
    *reg1 = vol[2];
    *reg2 = vol[3];
    *reg3 = 0x20;
    return 0;
}

int CD_init(void)
{
    u_char vol[4];

    Sync = 2;
    Ready = 0;
    CD_cbready = 0;
    CD_cbsync = 0;
    ResetCallback();
    InterruptCallback(2, callback);
    if (spu[0x1B8 / 2] == 0 && spu[0x1BA / 2] == 0) {
        spu[0x180 / 2] = 0x3FFF;
        spu[0x182 / 2] = 0x3FFF;
    }
    spu[0x1B0 / 2] = 0x3FFF;
    spu[0x1B2 / 2] = 0x3FFF;
    spu[0x1AA / 2] = 0xC001;
    vol[0] = vol[2] = 0x80;
    vol[1] = vol[3] = 0;
    if (CD_vol(vol)) {
        return -1;
    }
    if (CD_cw(CdlReset, 0, 0, 0)) {
        return -1;
    }
    return -(CD_sync(0, 0) != 2);
}

static struct {
    u_long *addr;
    int size;
    volatile int nsec;
    volatile int timeout;
    void (*volatile cbready)();
    int pad;
} ReadAttr;

static void cb_read(int intr, u_char *result);
int CD_read(u_long *addr, int size);

int CD_readm(u_long *addr, int nsec, int size)
{
    u_char result[8];

    ReadAttr.addr = addr;
    ReadAttr.nsec = nsec;
    ReadAttr.size = size;
    set_alarm();
    while (CD_ready(1, result)) {
        if (get_alarm()) {
            goto error;
        }
    }
    ReadAttr.cbready = CD_cbready;
    CD_cbready = cb_read;
    if (CD_cw(CdlReadN, 0, result, 0) == 0) {
        ReadAttr.timeout = Vcount + 480 + nsec;
        return 0;
    }
error:
    if (CD_cbread) {
        CD_cbread(5, result);
    }
    CD_cbready = ReadAttr.cbready;
    ReadAttr.nsec = -1;
    CD_cw(8, 0, 0, 0);
    return -1;
}

int CD_readsync(int mode, u_char *result)
{
    for (;;) {
        rescpy(result, ReadyResult);
        if (Vcount > ReadAttr.timeout || Ready == 5 || Ready == 4) {
            ReadAttr.nsec = -1;
        }
        if (mode || ReadAttr.nsec <= 0) {
            return ReadAttr.nsec;
        }
    }
}

static void cb_read(int intr, u_char *result)
{
    if (intr == 1) {
        if (ReadAttr.nsec > 0) {
            CD_read(ReadAttr.addr, ReadAttr.size);
            ReadAttr.addr += ReadAttr.size;
            ReadAttr.nsec--;
        }
    } else {
        ReadAttr.nsec = -1;
    }
    if (ReadAttr.nsec <= 0) {
        if (CD_cbread) {
            CD_cbread(5, result);
        }
        CD_cw(ReadAttr.nsec == 0 ? 9 : 8, 0, 0, 0);
        CD_cbready = ReadAttr.cbready;
    }
}

static volatile int *cd_delay = (int *)0x1F801018;
static volatile int *com_delay = (int *)0x1F801020;
static volatile int *dpcr = (int *)0x1F8010F0;
static volatile int *madr = (int *)0x1F8010B0;
static volatile int *bcr = (int *)0x1F8010B4;
static volatile int *chcr = (int *)0x1F8010B8;

int CD_read(u_long *addr, int size)
{
    set_alarm();
    while (*chcr & 0x01000000) {
        if (get_alarm()) {
            return -1;
        }
    }
    *reg0 = 0;
    *reg3 = 0x80;
    *cd_delay = 0x20943;
    *com_delay = 0x132C;
    *dpcr |= 0x8000;
    *chcr = 0;
    *madr = (int)addr;
    *bcr = size | 0x10000;
    *chcr = 0x11000000;
    return 0;
}

void CD_set_test_parmnum(int n)
{
    ComParm[25] = n;
}

#endif
#else
// SPDX-License-Identifier: MIT
#include "libcd.h"
#if VERSION == 40

typedef struct {
    short left;
    short right;
} SpuVolume;
typedef struct {
    u8 voice_regs[0x180];
    SpuVolume main_vol;
    u8 pad184[0x1AA - 0x184];
    u16 spucnt;
    u8 pad1AC[0x1B0 - 0x1AC];
    SpuVolume cd_vol;
    SpuVolume ex_vol;
    SpuVolume main_volx;
} SPU_RXX;
#else
#include "../spu/libspu_i.h"
#endif

typedef unsigned int size_t;

typedef char Result_t[8];

typedef struct {
#if VERSION == 33
    unsigned char pending;
#endif
    unsigned char sync;
    unsigned char ready;
    unsigned char c;
} CD_intr;

typedef struct {
    CD_intr* intr;
    Result_t* result;
    unsigned char* cd_com;
    int* cd_status;
    unsigned char** cd_pos;
#if VERSION != 33
    const char* rcsid;
#endif
} CD_init_struct;

typedef struct Alarm_t {
    int unk0;
    int unk4;
    char* unk8;
} Alarm_t;

static Result_t Result[3];
static volatile Alarm_t Alarm;

#if VERSION == 36
static u8 __ps_libinfo__[] = {0x50, 0x73, 0x04, 0x0A, 0x1A, 0xD3, 0x36, 0x10};
#elif VERSION == 37
static u8 __ps_libinfo__[] = {0x50, 0x73, 0x04, 0x0C, 0xDA, 0x99, 0x37};
#elif VERSION == 40
static u8 __ps_libinfo__[] = {0x50, 0x73, 0x04, 0x15, 0x9C, 0x85, 0x40};
#endif
CdlCB CD_cbsync = NULL;
CdlCB CD_cbready = NULL;
CdlCB CD_cbread = NULL;
int CD_debug = 0;
int CD_status = 0;
int CD_status1 = 0;
int CD_nopen = 0;
unsigned char CD_pos[] = {2, 0, 0, 0};
#if VERSION != 33
unsigned char CD_mode = 0;
#endif
unsigned char CD_com = 0;
#if VERSION == 40
int DS_active = 0;
int StMode;
#endif
char* CD_comstr[] = {
    "CdlSync",    "CdlNop",
    "CdlSetloc",  "CdlPlay",
#if VERSION == 40
    "CdlForward", "CdlBackward",
#else
    "CdlForward", "CdlBackword",
#endif
    "CdlReadN",   "CdlStandby",
    "CdlStop",    "CdlPause",
    "CdlReset",   "CdlMute",
    "CdlDemute",  "CdlSetfilter",
    "CdlSetmode", "?",
    "CdlGetlocL", "CdlGetlocP",
    "?",          "CdlGetTN",
    "CdlGetTD",   "CdlSeekL",
    "CdlSeekP",   "?",
    "?",          "?",
    "?",          "CdlReadS",
    "?",          "?",
    "?",          "?",
};
char* CD_intstr[] = {
    "NoIntr",  "DataReady", "Complete", "Acknowledge",
    "DataEnd", "DiskError", "?",        "?",
};
static int ComAttr[4][32] = {

    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
#if VERSION == 33
     0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#else
     0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
#endif

    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},

    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

    {0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0,
     0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static volatile u8* reg0 = (u8*)0x1F801800;
static volatile u8* reg1 = (u8*)0x1F801801;
static volatile u8* reg2 = (u8*)0x1F801802;
static volatile u8* reg3 = (u8*)0x1F801803;

static volatile int* com_delay = (int*)0x1F801020;
static volatile SPU_RXX* spu = (SPU_RXX*)0x1F801C00;
#if VERSION == 33
static volatile CD_intr Intr = {0, 0, 0, 0};

static struct {
    int retry;
    int nsect;
    u_long* volatile addr;
    u_long* volatile ptr;
    volatile int mode;
    int size;
    volatile int count;
    volatile int time;
    CdlCB cbsync;
    CdlCB cbready;
} Read = {0, 0, NULL, NULL, 0, 0, 0, 0, NULL, NULL};
#else
static volatile CD_intr Intr = {0, 0, 0};
#endif

static inline void _memcpy(void* _dst, void* _src, size_t _size) {
    char* pDst = (char*)_dst;
    char* pSrc = (char*)_src;

    if (pDst == NULL) {
        return;
    }

    while (_size--) {
        *pDst++ = *pSrc++;
    }
}

#if VERSION == 33
#define ALARM_VSYNC 480
#define ALARM_COUNT 0x1E0000
#else
#define ALARM_VSYNC 960
#define ALARM_COUNT 0x3C0000
#endif

static inline void set_alarm(char* name) {

    ((Alarm_t*)&Alarm)->unk0 = VSync(-1) + ALARM_VSYNC;
    ((Alarm_t*)&Alarm)->unk4 = 0;
    ((Alarm_t*)&Alarm)->unk8 = name;
}

static inline int get_alarm(void) {
    if (((Alarm_t*)&Alarm)->unk0 < VSync(-1) ||
        ((Alarm_t*)&Alarm)->unk4++ > ALARM_COUNT) {
#if VERSION == 33
        printf("%s:(%s) timeout Sync=%s, Ready=%s\n",
               ((Alarm_t*)&Alarm)->unk8,
#else
        puts("CD timeout: ");
        printf("%s:(%s) Sync=%s, Ready=%s\n", ((Alarm_t*)&Alarm)->unk8,
#endif
               CD_comstr[CD_com], CD_intstr[Intr.sync],
               CD_intstr[Intr.ready]);
        CD_flush();
        return -1;
    }
    return 0;
}

#if VERSION == 33
static int getintr(void) {
    u_char nReg;
    u_char buf[8];
    int i;
    int bHasError = 0;

    if (Intr.pending) {
        Intr.pending = 0;
        *reg0 = 1;
        *reg3 = 7;
        *reg2 = 7;
    }
    *reg0 = 1;
    if ((*reg3 & 7) == 0) {
        return 0;
    }
    nReg = *reg3 & 7;
    if (nReg == 1) {
        Intr.pending = 1;
    } else {
        *reg0 = 1;
        *reg3 = 7;
        *reg2 = 7;
    }

    for (i = 0; i < 8; i++) {
        if ((*reg0 & 0x20) == 0) {
            break;
        }
        buf[i] = *reg1;
    }
    for (; i < 8; i++) {
        buf[i] = 0;
    }

    if (nReg != 3 || ComAttr[2][CD_com]) {
        if (!(CD_status & CdlStatShellOpen) && (buf[0] & CdlStatShellOpen)) {
            CD_nopen++;
        }
        CD_status = buf[0];
        CD_status1 = buf[1];
        bHasError = CD_status & (CdlStatError | CdlStatSeekError |
                                 CdlStatIdError | CdlStatShellOpen);
    }
    if (nReg == 5) {
        printf("%s: DiskError(%02x:%02x)\n", CD_comstr[CD_com], CD_status,
               CD_status1);
    }
    switch (nReg) {
    case 3:
        if (bHasError) {
            Intr.sync = CdlDiskError;
            _memcpy(&Result[0], &buf, sizeof(Result_t));
            return 2;
        }
        if (ComAttr[0][CD_com]) {
            Intr.sync = CdlAcknowledge;
            _memcpy(&Result[0], &buf, sizeof(Result_t));
            return 1;
        }
        Intr.sync = CdlComplete;
        _memcpy(&Result[0], &buf, sizeof(Result_t));
        return 2;
    case 2:
        Intr.sync = bHasError ? CdlDiskError : CdlComplete;
        _memcpy(&Result[0], &buf, sizeof(Result_t));
        return 2;
    case 1:
        Intr.ready = bHasError ? CdlDiskError : CdlDataReady;
        _memcpy(&Result[1], &buf, sizeof(Result_t));
        return 4;
    case 4:
        Intr.ready = Intr.c = CdlDataEnd;
        _memcpy(&Result[2], &buf, sizeof(Result_t));
        _memcpy(&Result[1], &buf, sizeof(Result_t));
        return 4;
    case 5:
        Intr.sync = Intr.ready = CdlDiskError;
        _memcpy(&Result[0], &buf, sizeof(Result_t));
        _memcpy(&Result[1], &buf, sizeof(Result_t));
        return 6;
    default:
        printf("CDROM: unknown intr(%d)\n", nReg);
        return -1;
    }
}
#else
static int getintr(void) {
    volatile char nReg;
    volatile Result_t buf;
    int i, j;
    int bHasError;

    *reg0 = 1;

    nReg = *reg3 & 0x7;

    if (nReg == 0) {
        return 0;
    }

    bHasError = 0;

    while (nReg != (*reg3 & 7)) {
        nReg = *reg3 & 0x7;
    }

    for (i = 0; i < 8; i++) {
        if ((*reg0 & 0x20) == 0) {
            break;
        }
        buf[i] = *reg1;
    }
    for (j = i; j < 8; j++) {
        buf[j] = 0;
    }

    *reg0 = 1;
    *reg3 = 7;
    *reg2 = 7;
    if (nReg != 3 || ComAttr[2][CD_com]) {
        if (!(CD_status & CdlStatShellOpen) && (buf[0] & CdlStatShellOpen)) {
            CD_nopen++;
        }
        CD_status = buf[0];
        CD_status1 = buf[1];
        bHasError = CD_status;
        bHasError &= (CdlStatError | CdlStatSeekError | CdlStatIdError |
                      CdlStatShellOpen);
    }
    if (nReg == 5) {
#if VERSION >= 37
        if (CD_debug > 0) {
            printf("DiskError: ");
        }
#else
        puts("DiskError: ");
#endif
        if (CD_debug > 0) {
            printf("com=%s,code=(%02x:%02x)\n", CD_comstr[CD_com], CD_status,
                   CD_status1);
        }
    }
    switch (nReg) {
    case 3:
        if (bHasError) {
            Intr.sync = CdlDiskError;
            _memcpy(&Result[0], &buf, sizeof(Result_t));
            return 2;
        }
        if (ComAttr[0][CD_com]) {
            Intr.sync = CdlAcknowledge;
            _memcpy(&Result[0], &buf, sizeof(Result_t));
            return 1;
        }
        Intr.sync = CdlComplete;
        _memcpy(&Result[0], &buf, sizeof(Result_t));
        return 2;
    case 2:
        Intr.sync = bHasError ? CdlDiskError : CdlComplete;
        _memcpy(&Result[0], &buf, sizeof(Result_t));
        return 2;
    case 1:
        if (bHasError && i == 1) {
            bHasError = 0;
        }
        Intr.ready = bHasError ? CdlDiskError : CdlDataReady;
        _memcpy(&Result[1], &buf, sizeof(Result_t));
        *reg0 = 0;
        *reg3 = 0;
        return 4;
    case 4:
        Intr.ready = Intr.c = CdlDataEnd;
        _memcpy(&Result[2], &buf, sizeof(Result_t));
        _memcpy(&Result[1], &buf, sizeof(Result_t));
        return 4;
    case 5:
        Intr.sync = Intr.ready = CdlDiskError;
        _memcpy(&Result[0], &buf, sizeof(Result_t));
        _memcpy(&Result[1], &buf, sizeof(Result_t));
        return 6;
    default:
        puts("CDROM: unknown intr");
        printf("(%d)\n", nReg);
        return 0;
    }
}

#endif

static inline void callback(void) {
    int interrupt;
    unsigned char temp_s1;

    temp_s1 = *reg0 & 3;

    while (true) {
        interrupt = getintr();
        if (interrupt == 0) {
            break;
        }

        if ((interrupt & 4) && CD_cbready != NULL) {
            CD_cbready(Intr.ready, &Result[1]);
        }
        if ((interrupt & 2) && CD_cbsync != NULL) {
            CD_cbsync(Intr.sync, &Result[0]);
        }
    }
    *reg0 = temp_s1;
}

int CD_sync(int mode, u_char* result) {
    int i;
    int sync;

    set_alarm("CD_sync");

    while (true) {
        if (get_alarm()) {
            return -1;
        }

        if (CheckCallback()) {
            callback();
        }

        sync = Intr.sync;
        if (sync == CdlComplete || sync == CdlDiskError) {
            Intr.sync = CdlComplete;
            _memcpy(result, &Result[0], sizeof(Result_t));
            return sync;
        }

        if (mode != 0) {
            return 0;
        }
    }
}

int CD_ready(int mode, u_char* result) {
    int i;
    int c;
    int ready;

    set_alarm("CD_ready");
    while (true) {
        if (get_alarm()) {
            return -1;
        }
        if (CheckCallback()) {
            callback();
        }
        c = Intr.c;
        if (c != 0) {
            Intr.c = 0;
            _memcpy(result, &Result[2], sizeof(Result_t));
            return c;
        }
        ready = Intr.ready;
        if (ready != 0) {
            Intr.ready = 0;
            _memcpy(result, &Result[1], sizeof(Result_t));
            return ready;
        }
        if (mode != 0) {
            return 0;
        }
    }
}

int CD_cw(u8 com, u8* param, u_char* result, s32 arg3) {
    int i;

    if (CD_debug > 1) {
        printf("%s...\n", CD_comstr[com]);
    }
    if ((ComAttr[3][com] != 0) && (param == NULL)) {
        if (CD_debug > 0) {
            printf("%s: no param\n", CD_comstr[com]);
        }
        return -2;
    }
    CD_sync(CdlSync, NULL);
#if VERSION == 33
    if (Intr.pending) {
        Intr.pending = 0;
        *reg0 = 1;
        *reg3 = 7;
        *reg2 = 7;
    }
#endif
    if (com == CdlSetloc) {
        for (i = 0; i < 4; i++) {
            CD_pos[i] = param[i];
        }
    }
#if VERSION >= 36
    if (com == CdlSetmode) {
        CD_mode = param[0];
    }
#endif
    Intr.sync = CdlNoIntr;
    if (ComAttr[1][com]) {
        Intr.ready = CdlNoIntr;
    }
    *reg0 = 0;
    for (i = 0; i < ComAttr[3][com]; i++) {
#if VERSION == 33
        *reg2 = *param++;
#else
        *reg2 = param[i];
#endif
    }
    CD_com = com;
    *reg1 = com;
    if (arg3 != 0) {
        return 0;
    }

    set_alarm("CD_cw");

    while (Intr.sync == CdlNoIntr) {
        if (get_alarm()) {
            return -1;
        }
        if (CheckCallback()) {
            callback();
        }
    }
#if VERSION < 36 && VERSION != 33
    if (Intr.sync == CdlComplete && com == CdlSetmode) {
        CD_mode = *param;
    }
#endif

    _memcpy(result, &Result[0], sizeof(Result_t));

    return -(Intr.sync == CdlDiskError);
}

int CD_vol(CdlATV* vol) {
    *reg0 = 2;
    *reg2 = vol->val0;
    *reg3 = vol->val1;
    *reg0 = 3;
    *reg1 = vol->val2;
    *reg2 = vol->val3;
    *reg3 = 0x20;
    return 0;
}

#if VERSION == 33
inline int CD_shell(void) {
    CdlCB old = CD_cbsync;
    int n = 0;

    while (CD_status & CdlStatShellOpen) {
        CD_cbsync = NULL;
        printf("CD open(%02x)...\n", CD_status);
        n++;
        CD_cw(CdlNop, NULL, NULL, 0);
        VSync(60);
        CD_cbsync = old;
    }
    return n;
}

inline
#endif
int CD_flush(void) {
    *reg0 = 1;
    while (*reg3 & 7) {
        *reg0 = 1;
        *reg3 = 7;
        *reg2 = 7;
    }

    Intr.ready = Intr.c = CdlNoIntr;
    Intr.sync = CdlComplete;
    *reg0 = 0;
    *reg3 = 0;
    *com_delay = 0x1325;
}

int CD_initvol(void) {
    CdlATV vol;

    if (spu->main_volx.left == 0 && spu->main_volx.right == 0) {
        spu->main_vol.left = 0x3FFF;
        spu->main_vol.right = 0x3FFF;
    }

    spu->cd_vol.left = 0x3FFF;
    spu->cd_vol.right = 0x3FFF;
    spu->spucnt = 0xC001;
    vol.val0 = vol.val2 = 0x80;
    vol.val1 = vol.val3 = 0;
    *reg0 = 2;
    *reg2 = vol.val0;
    *reg3 = vol.val1;
    *reg0 = 3;
    *reg1 = vol.val2;
    *reg2 = vol.val3;
    *reg3 = 0x20;

    return 0;
}

#if VERSION == 33
inline void CD_initintr(void) {
    int* p;
    int n;

    CD_cbready = NULL;
    CD_cbsync = NULL;
    CD_status = 0;
    p = (int*)&Read;
    n = sizeof(Read) / sizeof(int);
    while (n--) {
        *p++ = 0;
    }
    ResetCallback();
    InterruptCallback(2, callback);
}
#else
void CD_initintr(void) {
    CD_cbready = NULL;
    CD_cbsync = NULL;
    CD_status1 = 0;
    CD_status = 0;
    ResetCallback();
    InterruptCallback(2, callback);
}
#endif

#if VERSION == 33
int CD_init(void) {
    static CD_init_struct tab = {
        .intr = &Intr,
        .result = &Result[0],
        .cd_com = &CD_com,
        .cd_status = &CD_status,
        .cd_pos = &CD_pos,
    };

    printf("CD_init:addr=%08x\n", &tab);
    CD_initintr();
    CD_flush();
    CD_cw(CdlNop, NULL, NULL, 0);
    if (CD_status & CdlStatShellOpen) {
        CD_cw(CdlNop, NULL, NULL, 0);
    }
    CD_shell();
    if (CD_cw(CdlReset, NULL, NULL, 0)) {
        return -1;
    }
    if (CD_cw(CdlDemute, NULL, NULL, 0)) {
        return -1;
    }
    if (CD_sync(CdlSync, NULL) != CdlComplete) {
        return -1;
    }
    return 0;
}

static int cd_read_retry(void);
int CD_getsector(void* buffer, size_t size);

static inline void cb_read(int intr, u_char* result) {
    if (intr == CdlDataReady) {
        if (Read.count > 0) {
            CD_getsector(Read.ptr, Read.size);
            Read.ptr += Read.size;
            Read.count--;
        }
    } else {
        Read.count = -1;
    }
    Read.time = VSync(-1);
    if (Read.count < 0 && Read.retry > 0) {
        cd_read_retry();
    }
    if (Read.count <= 0) {
        CD_cbsync = Read.cbsync;
        CD_cbready = Read.cbready;
        CD_cw(CdlPause, NULL, NULL, 0);
        if (CD_cbread != NULL) {
            CD_cbread(Read.count == 0 ? CdlComplete : CdlDiskError, result);
        }
    }
}

static int cd_read_retry(void) {
    u_char param[4];
    int i;

    CD_cbready = NULL;
    CD_cbsync = NULL;
    while (Read.retry-- > 0) {
        if (Read.retry < 7) {
            printf("CD read retry %2d(%02x:%02x:%02x)\n", Read.retry,
                   CD_pos[0], CD_pos[1], CD_pos[2]);
            if (CD_shell()) {
                i = 8;
                while (i--) {
                    VSync(60);
                }
            }
        }
        CD_flush();
        param[0] = Read.mode;
        if (CD_cw(CdlSetmode, param, NULL, 0)) {
            continue;
        }
        if (CD_cw(CdlSetloc, CD_pos, NULL, 0)) {
            continue;
        }
        CD_cbready = (CdlCB)cb_read;
        Read.ptr = Read.addr;
        CD_cw(CdlReadN, NULL, NULL, 1);
        Read.count = Read.nsect;
        Read.time = VSync(-1) + 480;
        return Read.count;
    }
    return Read.count = -1;
}

int CD_readm(u_long* addr, int nsect, int mode) {
    Read.mode = mode;
    switch (Read.mode & 0x30) {
    case 0:
        Read.size = 0x200;
        break;
    case 0x20:
        Read.size = 0x249;
        break;
    default:
        Read.size = 0x246;
        break;
    }
    Read.addr = addr;
    Read.nsect = nsect;
    Read.retry = 8;
    Read.cbsync = CD_cbsync;
    Read.cbready = CD_cbready;
    if (CD_status & 0xE0) {
        CD_cw(CdlPause, NULL, NULL, 0);
    }
    CD_sync(CdlSync, NULL);
    if (cd_read_retry() > 0) {
        return 0;
    }
    return -1;
}

int CD_datasync(int mode);

int CD_readsync(int mode, u_char* result) {
    int ret;

    set_alarm("CD_read");
    while (true) {
        if ((ret = get_alarm()) != 0) {
            return ret;
        }
        if (CheckCallback()) {
            callback();
        }
        _memcpy(result, &Result[1], sizeof(Result_t));
        if (VSync(-1) > Read.time + 60) {
            cd_read_retry();
        }
        if (Read.count == 0) {
            CD_datasync(0);
        }
        if (mode != 0 || Read.count <= 0) {
            return Read.count;
        }
    }
}

#else
int CD_init(void) {
    static CD_init_struct tab = {
        .intr = &Intr,
        .result = &Result[0],
        .cd_com = &CD_com,
        .cd_status = &CD_status,
        .cd_pos = &CD_pos,
#if VERSION == 40
        .rcsid = "$Id: bios.c,v 1.86 1997/03/28 07:42:42 makoto Exp $"
#elif VERSION == 37
        .rcsid = "$Id: bios.c,v 1.81 1996/12/16 06:24:14 makoto Exp $"
#elif VERSION == 36
        .rcsid = "$Id: bios.c,v 1.80 1996/09/11 07:02:15 ayako Exp $"
#else
        .rcsid = "$Id: bios.c,v 1.77 1996/05/13 06:58:16 suzu Exp $"
#endif
    };

    puts("CD_init:");
    printf("addr=%08x\n", &tab);
    CD_com = 0;
    CD_mode = 0;
    CD_cbready = NULL;
    CD_cbsync = NULL;
    CD_status1 = 0;
    CD_status = 0;
    ResetCallback();
    InterruptCallback(2, callback);

    *reg0 = 1;
    while (*reg3 & 7) {
        *reg0 = 1;
        *reg3 = 7;
        *reg2 = 7;
    }

    Intr.ready = Intr.c = CdlNoIntr;
    Intr.sync = CdlComplete;

    *reg0 = 0;
    *reg3 = 0;
    *com_delay = 0x1325;

    CD_cw(CdlNop, NULL, NULL, 0);
    if (CD_status & CdlStatShellOpen) {
        CD_cw(CdlNop, NULL, NULL, 0);
    }

    if (CD_cw(CdlReset, NULL, NULL, 0)) {
        return -1;
    }

    if (CD_cw(CdlDemute, NULL, NULL, 0)) {
        return -1;
    }

    if (CD_sync(CdlSync, NULL) != CdlComplete) {
        return -1;
    }
    return 0;
}
#endif

static volatile int* dv5_delay = (int*)0x1F801018;
static volatile int* d_pcr = (int*)0x1F8010F0;
static volatile int* d3_madr = (int*)0x1F8010B0;
static volatile int* d3_bcr = (int*)0x1F8010B4;
static volatile int* d3_chcr = (int*)0x1F8010B8;

int CD_datasync(int mode) {
    int ret;

    set_alarm("CD_datasync");
    while (true) {
        if (get_alarm()) {
            ret = -1;
            break;
        }
        if (!(*d3_chcr & 0x01000000)) {
            ret = 0;
            break;
        }
        if (mode != 0) {
            ret = 1;
            break;
        }
    }
    return ret;
}

int CD_getsector(void* buffer, size_t size) {
    *reg0 = 0;
    *reg3 = 0x80;
    *dv5_delay = 0x20943;
    *com_delay = 0x1323;
    *d_pcr |= 0x8000;
    *d3_madr = buffer;
    *d3_bcr = size | 0x10000;
#if VERSION != 33
    while (!(*reg0 & 0x40)) {
    };
#endif
    *d3_chcr = 0x11000000;
    while (*d3_chcr & 0x01000000) {
    };
    *com_delay = 0x1325;
    return 0;
}

#if VERSION == 40
int CD_getsector2(void* buffer, size_t size) {
    volatile int readback;

    *reg0 = 0;
    *reg3 = 0x80;
    *dv5_delay = 0x21020843;
    *com_delay = 0x1325;
    *d_pcr |= 0x8000;
    *d3_madr = buffer;
    *d3_bcr = size | 0x10000;
    while (!(*reg0 & 0x40)) {
    };
    *d3_chcr = 0x11400100;
    readback = *d3_chcr;
    return 0;
}
#endif

void CD_set_test_parmnum(int parmNum) { ComAttr[3][25] = parmNum; }
#endif
