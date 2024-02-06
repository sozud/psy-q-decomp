#include "libspu_i.h"

void _SpuDataCallback(void (*arg0)()) { DMACallback(4, arg0); }