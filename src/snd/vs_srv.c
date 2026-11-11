#if VERSION >= 36
#include "libsnd_i.h"

char SsSetReservedVoice(char voices) {
    if (voices > NUM_VOICES || voices == 0) {
        return -1;
    }
    _SsVmMaxVoice = voices;
    return _SsVmMaxVoice;
}
#else
#include "libsnd_i.h"

char SsSetReservedVoice(char voices) {
    if ((voices >= 0x19) || (voices == 0)) {
        return -1;
    }
    spuVmMaxVoice = voices;
    return spuVmMaxVoice;
}
#endif
