#if VERSION == 26 || VERSION == 30
#if VERSION == 26
static char rcsid[] = "$Id: vsync.c,v 1.2 1994/12/22 06:39:55 suzu Exp $";
#else
static char rcsid[] = "$Id: vsync.c,v 1.7 1995/03/14 04:44:17 suzu Exp $";
#endif

extern volatile long Vcount;
static void v_wait(long count);

static volatile long* gp1 = (long*)0x1F801814;
static volatile long* t1_count = (long*)0x1F801110;
static long Hcount = 0;
static long pre_vcount = 0;

long VSync(long mode) {
    long status;
    long ret;

    status = *gp1;
    ret = (*t1_count - Hcount) & 0xFFFF;
    if (mode < 0) {
        return Vcount;
    }
    if (mode == 1) {
        return ret;
    }
    v_wait(pre_vcount + mode - 1);
    status = *gp1;
    v_wait(Vcount + 1);
    if (status & 0x80000) {
        while (!((status ^ *gp1) & 0x80000000)) {
        }
    }
    pre_vcount = Vcount;
    Hcount = *t1_count;
    return ret;
}

static void v_wait(long count) {
    volatile long timeout;

    timeout = 0x8000;
    while (Vcount < count) {
        if (--timeout == -1) {
            printf("VSync: timeout\n");
            return;
        }
    }
}

#else

#include "../types.h"

void ChangeClearRCnt(s32, s32);
extern volatile s32 Vcount;
#if VERSION == 36
static s32 __ps_libinfo__[2] = {0x0A067350, 0x1036D51A};
#elif VERSION == 37
static s32 __ps_libinfo__[2] = {0x0C067350, 0x00379BDA};
#elif VERSION == 40
static s32 __ps_libinfo__[2] = {0x15067350, 0x0040889C};
#endif
static volatile s32* gp1 = (s32*)0x1F801814;
static volatile s32* t1_count = (s32*)0x1F801110;
volatile s32 Hcount = 0;

s32 VSync(s32 mode) {
    s32 temp_s0;
    s32 ret;
    s32 max;
    s32 waitCount;
    static volatile s32 pre_vcount = 0;

    temp_s0 = *gp1;

    ret = (*t1_count - Hcount) & 0xFFFF;
    if (mode < 0) {
        return Vcount;
    }
    if (mode == 1) {
        return ret;
    }
    if (mode > 0) {
        waitCount = 1;
        waitCount = (pre_vcount - waitCount) + mode;
    } else {
        waitCount = pre_vcount;
    }
    max = 0;
    if (mode > 0) {
        max = mode - 1;
    }
    v_wait(waitCount, max);
    temp_s0 = *gp1;
    v_wait(Vcount + 1, 1);
#if VERSION >= 37
    if ((temp_s0 & 0x400000) && ((temp_s0 ^ *gp1) >= 0)) {
#else
    if ((temp_s0 & 0x80000) && ((temp_s0 ^ *gp1) >= 0)) {
#endif
        do {

        } while (!((temp_s0 ^ *gp1) & 0x80000000));
    }
    pre_vcount = Vcount;
    Hcount = *t1_count;
    return ret;
}

static void v_wait(s32 count, s32 max) {
    volatile s32 timeout[2];

    timeout[0] = max << 0xF;
    while (Vcount < count) {
        if (timeout[0]-- == 0) {
            puts("VSync: timeout\n");
            ChangeClearPAD(0);
            ChangeClearRCnt(3, 0);
            return;
        }
    }
}

#endif
