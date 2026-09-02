#include "libspu_i.h"
s32 _spu_AllocBlockNum = 0;
s32 _spu_AllocLastNum = 0;
s8* _spu_memList = NULL;
extern void _spu_gcSPU(void);

