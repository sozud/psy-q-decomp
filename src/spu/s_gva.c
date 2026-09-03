#include "libspu_i.h"
void SpuGetVoiceAttr(SpuVoiceAttr *arg) {
    s32 var_a0;
    s32 var_v1;

    var_a0 = -1;
    for (var_v1 = 0; var_v1 < 24; var_v1++) {
        if (arg->voice & (1 << var_v1)) {
            var_a0 = var_v1;
            break;
        }
    }

    if (var_a0 != -1) {
        SpuNGetVoiceAttr(var_a0, arg);
    }
}

