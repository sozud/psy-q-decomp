#include "libspu_i.h"
int SpuCommonError;
void SpuSetCommonCDVolume(s16 cd_left, s16 cd_right) {
    _spu_RXX->rxx.cd_vol.left = cd_left;
    _spu_RXX->rxx.cd_vol.right = cd_right;
}

