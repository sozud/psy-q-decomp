#include "libgs_i.h"
#if VERSION <= 33

#define RCntCNT1 0xF2000001
#define RCntMdNOINTR 0x2000
extern long SetRCnt(unsigned long spec, long target, long mode);
extern long StartRCnt(unsigned long spec);
#else
#include "KERNEL.H"
#endif

void GsInitVcount(void) {
#if VERSION <= 33
    SetRCnt(RCntCNT1, -1, RCntMdNOINTR);
#else
    SetRCnt(RCntCNT1, 0xFFFF, RCntMdNOINTR);
#endif
    StartRCnt(RCntCNT1);
}
