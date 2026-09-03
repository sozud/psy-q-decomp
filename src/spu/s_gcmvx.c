#include "libspu_i.h"
void SpuGetCommonMasterVolumeX(s16 *mvolx_left, s16 *mvolx_right) {
    *mvolx_left = _spu_RXX->rxx.main_volx.left;
    *mvolx_right = _spu_RXX->rxx.main_volx.right;
}

