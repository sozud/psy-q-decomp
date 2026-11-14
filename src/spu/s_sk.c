#if VERSION <= 30
#include "spu_old.h"

void SpuSetKey(long on_off, unsigned long voice_bit) {
    switch (on_off) {
    case 1:
        _spu_ioctl(_spu_fd, 0x204, &voice_bit);
        _spu_keystat |= voice_bit;
        break;
    case 0:
        _spu_ioctl(_spu_fd, 0x203, &voice_bit);
        _spu_keystat &= ~voice_bit;
        break;
    }
}
#else /* VERSION > 30 */
#if VERSION >= 36
#define _spu_RQvoice header_spu_RQvoice
#if VERSION == 36
#include "libspu_40.h"
#else
#include "libspu_i.h"
#endif
#undef _spu_RQvoice
extern vs32 _spu_RQvoice;

void SpuSetKey(long on_off, unsigned long voice_bit) {
    u16 low;
    u16 high;
    voice_bit &= SPU_ALLCH;
    low = voice_bit;
    high = voice_bit >> 16;
    switch (on_off) {
    case SPU_ON:
        if (_spu_env & 1) {
            _spu_RQ[0] = low;
            _spu_RQ[1] = high;
            _spu_RQmask |= 1;
            _spu_RQvoice |= voice_bit;
            if (_spu_RQ[2] & voice_bit) {
                _spu_RQ[2] &= ~voice_bit;
            }
            if (_spu_RQ[3] & high) {
                _spu_RQ[3] &= ~high;
            }
        } else {
            _spu_RXX->rxx.key_on[0] = low;
            _spu_RXX->rxx.key_on[1] = high;
            _spu_keystat |= voice_bit;
        }
        break;
    case SPU_OFF:
        if (_spu_env & 1) {
            _spu_RQ[2] = low;
            _spu_RQ[3] = high;
            _spu_RQmask |= 1;
            _spu_RQvoice &= ~voice_bit;
            if (_spu_RQ[0] & voice_bit) {
                _spu_RQ[0] &= ~voice_bit;
            }
            if (_spu_RQ[1] & high) {
                _spu_RQ[1] &= ~high;
            }
        } else {
            _spu_RXX->rxx.key_off[0] = low;
            _spu_RXX->rxx.key_off[1] = high;
            _spu_keystat &= ~voice_bit;
        }
        break;
    }
}
#elif VERSION == 33
#include "libspu_i.h"

extern s32 _spu_keystat;

void SpuSetKey(long on_off, unsigned long voice_bit) {
    switch (on_off) {
    case 1:
        _spu_RXX->rxx.key_on[0] = voice_bit;
        _spu_RXX->rxx.key_on[1] = (voice_bit >> 16) & 0xFF;
        _spu_keystat |= voice_bit;
        break;
    case 0:
        _spu_RXX->rxx.key_off[0] = voice_bit;
        _spu_RXX->rxx.key_off[1] = (voice_bit >> 16) & 0xFF;
        _spu_keystat &= ~voice_bit;
        break;
    }
}
#else
#include "libspu_i.h"

extern s32 _spu_keystat;

void SpuSetKey(long on_off, unsigned long voice_bit) {
    u32 temp_a1;
    unsigned short new_var;
    u32 temp_a3;
    temp_a1 = voice_bit & 0xFFFFFF;
    temp_a3 = temp_a1 >> 0x10;
    new_var = temp_a1;
    if (on_off != 0) {
        if (on_off != 1) {
            return;
        }
        _spu_RXX->rxx.key_on[0] = new_var;
        _spu_RXX->rxx.key_on[1] = temp_a3;
        _spu_keystat = temp_a1 | _spu_keystat;
    } else {
        _spu_RXX->rxx.key_off[0] = temp_a1;
        _spu_RXX->rxx.key_off[1] = temp_a3;
        _spu_keystat = (~temp_a1) & _spu_keystat;
    }
}
#endif
#endif /* VERSION > 30 */
