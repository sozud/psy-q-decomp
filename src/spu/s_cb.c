#include "libspu_i.h"

void _SpuCallback(s32 arg0) { InterruptCallback(9, arg0); }