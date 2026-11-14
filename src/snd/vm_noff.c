#include "libsnd_i.h"

void vmNoiseOff(unsigned char voice) {
    _svm_voice[voice].unk1b = 0;
#if VERSION >= 41
    _svm_voice[voice].vag_idx = 0;
    _svm_voice[voice].unk04 = 0;
#else
    _svm_voice[voice].unk04 = 0;
    SPUW(noise_mode[0], 0);
    SPUW(noise_mode[1], 0);
#endif
}
