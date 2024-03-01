#include "libsnd_i.h"

/* previously seskoff.c in 3.5 */

long SsVoKeyOff(long vab_pro, long pitch) {
    SpuVmSeKeyOff((vab_pro << 8) >> 0x10, vab_pro, (pitch >> 8), pitch);
}
