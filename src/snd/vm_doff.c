#if VERSION >= 36
#include "libsnd_i.h"

void _SsVmDamperOff(void) { _svm_damper = 0; }
#else
#include "libsnd_i.h"

void SpuVmDamperOff(void) { _svm_damper = 0; }
#endif
