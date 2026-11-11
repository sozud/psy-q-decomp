#include "libgs_i.h"
#include "KERNEL.H"

long GsGetVcount(void) { return GetRCnt(RCntCNT1); }
