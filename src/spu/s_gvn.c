#include "libspu_i.h"
extern s32 _spu_pitch2note(u16 arg0, u16 arg1, u16 arg2);
void SpuGetVoiceNote(s32 vNum, u16 *note) {
    s32 temp_v0;
    u16 temp_a1;
    u16 b;

    b = _spu_RXX->raw[vNum * 8 + 2];
    temp_a1 = _spu_voice_centerNote[vNum];
    temp_v0 = _spu_pitch2note(temp_a1 >> 8, temp_a1 & 0xFF, b);
    if (temp_v0 >= 0) {
        *note = temp_v0;
    } else {
        *note = 0;
    }
}

