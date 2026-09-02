#include "libspu_i.h"
void SpuSetVoiceSampleNote(s32 vNum, u16 sampleNote) {
    _spu_voice_centerNote[vNum] = sampleNote;
    _spu_Fw1ts();
}

