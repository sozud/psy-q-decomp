#include "libspu_i.h"
long SpuReadDecodedData(SpuDecodedData *d_data, long flag) {
    switch (flag) {
        case 5:
            _spu_Fr_(d_data->cd_left, 0, 0x20);
        break;
        case 6:
            _spu_Fr_(d_data->voice1, 0x100, 0x20);
        break;
        default:
            _spu_Fr_(d_data->cd_left, 0, 0x40);
        break;
    }

    if (_spu_RXX->rxx.spustat & 0x800) {
        return 1;
    } else {
        return 0;
    }
}

