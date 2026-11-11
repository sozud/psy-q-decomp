#if VERSION == 26 || VERSION == 30
#if VERSION == 26

static char rcsid[] = "$Id: pad.c,v 1.29 1994/12/22 06:39:54 suzu Exp $";
#else

static char rcsid[] = "$Id: pad.c,v 1.33 1995/03/14 04:44:13 suzu Exp $";
#endif
extern void ResetCallback(void);
extern void PAD_init(long, long *);
extern void PAD_dr(void);
extern void ChangeClearPAD(long);
extern void StopPAD(void);

int PadIdentifier;
static long pad_buf;
static long pad_status;

void PadInit(int mode) {
    PadIdentifier = mode;
    pad_status = 0;
    pad_buf = -1;
    ResetCallback();
    PAD_init(0x20000001, &pad_buf);
    ChangeClearPAD(0);
}

unsigned long PadRead(int id) {
    PAD_dr();
    return ~pad_buf;
}

void PadStop(void) { StopPAD(); }

#else

#include "../types.h"
#if VERSION == 33
#define PadInit header_PadInit
#endif
#include <LIBETC.H>
#if VERSION == 33
#undef PadInit
#endif

int PadIdentifier;
static int pad_buf;

#if VERSION == 33
int PadInit(int mode) {
    int ret;

    PadIdentifier = mode;
    pad_buf = -1;
    ResetCallback();
    ret = PAD_init(0x20000001, &pad_buf);
    ChangeClearPAD(0);
    return ret;
}
#else
void PadInit(int mode) {
    PadIdentifier = mode;
    pad_buf = -1;
    ResetCallback();
    PAD_init(0x20000001, &pad_buf);
    ChangeClearPAD(0);
}
#endif

u_long PadRead(int id) {
    PAD_dr();
    return ~pad_buf;
}

void PadStop(void) { StopPAD(); }

#endif
