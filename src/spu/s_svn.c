#include "libspu_i.h"
void SpuSetVoiceNote(s32 vNum, u16 note) {
    u16 temp_a1;

    temp_a1 = _spu_voice_centerNote[vNum];
    _spu_RXX->raw[vNum * 8 + 2] = _spu_note2pitch(temp_a1 >> 8, temp_a1 & 0xFF, (note & 0xFF00) >> 8, note & 0xFF);
    _spu_Fw1ts();
}

