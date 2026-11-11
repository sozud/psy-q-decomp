#include "../types.h"

void SetDQA(long);
void SetDQB(long);

#if VERSION < 40
asm(".text");
#endif
#if VERSION == 37
asm(".text");
#endif

void SetFogNear(long a, long h) {
    SetDQA(-(a * 0x140) / h);
    SetDQB(0x01400000);
}
