#include "libspu_i.h"
long SpuGetTransferMode(void) {
    switch (_spu_transMode) {
        case 1:
            _spu_trans_mode = _spu_transMode;
        break;
        case 0:
            _spu_trans_mode = 0;
        break;
        default:
            _spu_trans_mode = 0;
        break;
    }

    return _spu_trans_mode;
}

