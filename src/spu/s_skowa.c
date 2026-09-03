#include "libspu_i.h"
void SpuSetKeyOnWithAttr(SpuVoiceAttr *attr) {
    SpuSetVoiceAttr(attr);
    SpuSetKey(1, attr->voice);
}

