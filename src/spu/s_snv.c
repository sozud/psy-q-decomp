#if VERSION <= 30
#include "spu_old.h"

unsigned long SpuSetNoiseVoice(long on_off, unsigned long voice_bit) {
    long v;

    switch (on_off) {
    case 1:
        _spu_ioctl(_spu_fd, 0x52, &voice_bit);
        break;
    case 0:
        _spu_ioctl(_spu_fd, 0x54, &voice_bit);
        break;
    }
    _spu_ioctl(_spu_fd, 0x53, &v);
    return v;
}
#else /* VERSION > 30 */
#include "libspu_i.h"

unsigned long SpuSetNoiseVoice(long on_off, unsigned long voice_bit) {
    _SpuSetAnyVoice(on_off, voice_bit, 0xCA, 0xCB);
}
#endif /* VERSION > 30 */
