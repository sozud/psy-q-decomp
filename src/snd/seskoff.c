#include "libsnd_i.h"

long SsVoKeyOff(long vab_pro, long pitch) {
    SpuVmSeKeyOff((vab_pro << 8) >> 0x10, vab_pro, (pitch >> 8), pitch);
}
