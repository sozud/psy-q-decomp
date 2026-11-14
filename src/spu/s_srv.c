#if VERSION <= 30
#include "spu_old.h"

unsigned long SpuSetReverbVoice(long on_off, unsigned long voice_bit) {
    long v;

    switch (on_off) {
    case 1:
        _spu_ioctl(_spu_fd, 0x65, &voice_bit);
        break;
    case 0:
        _spu_ioctl(_spu_fd, 0x67, &voice_bit);
        break;
    }
    _spu_ioctl(_spu_fd, 0x66, &v);
    return v;
}
#else /* VERSION > 30 */
#include "libspu_i.h"
unsigned long SpuSetReverbVoice(long on_off, unsigned long voice_bit) {
    return _SpuSetAnyVoice(on_off, voice_bit, 0xCC, 0xCD);
}
#endif /* VERSION > 30 */
