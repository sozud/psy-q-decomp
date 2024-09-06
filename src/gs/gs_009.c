#include "../types.h"
#include "KERNEL.H"
#include "gs_007.h"

s32 PSDOFSX;
s32 PSDOFSY;
s32 CLIP2[2]; 
s32 PSDBASEX;
s32 PSDBASEY;
s32 POSITION;
s16 GsDISPENV[10];
s16 PSDGPU;
s16 GsORGOFSX;
s16 GsORGOFSY;
s16 PSDIDX;
s32 PSDCNT;
u8 GsDRAWENV[92];

void GsClearVcount() { return ResetRCnt(RCntCNT1); }