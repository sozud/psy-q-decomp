#include "libsnd_i.h"


u8 _SsVmSelectToneAndVag(u8* toneIdx, u8* vagIdx) {
    u8 i;
    u8 vagCount;

    vagCount = 0;
    for (i = 0; i < _svm_cur.prog_tones; i++) {
        if (_svm_tn[_svm_cur.fake_program * 16 + i].min <= _svm_cur.note &&
            _svm_cur.note <= _svm_tn[_svm_cur.fake_program * 16 + i].max) {
            vagIdx[vagCount] = _svm_tn[_svm_cur.fake_program * 16 + i].vag;
            toneIdx[vagCount++] = i;
        }
    }
    return vagCount;
}
