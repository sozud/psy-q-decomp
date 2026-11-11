#if VERSION >= 36
#include "libsnd_i.h"
void _SsVmDamperOn(void) { _svm_damper = 2; }
#else
#include "libsnd_i.h"

void SpuVmDamperOn(void) { _svm_damper = 2; }
#endif
