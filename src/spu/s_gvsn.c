#include "libspu_i.h"
void SpuGetVoiceSampleNote(s32 vNum, u16 *sampleNote) {
    *sampleNote = _spu_voice_centerNote[vNum];
}

