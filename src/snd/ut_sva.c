#if VERSION >= 36
#include "libsnd_i.h"

s16 SsUtSetVagAtr(s16 vabId, s16 progNum, s16 toneNum, VagAtr* vagatrptr) {
    s16 tone;

    if (_svm_vab_used[vabId] == 1) {
        _SsVmVSetUp(vabId, progNum);
        tone = toneNum + _svm_cur.fake_program * 16;
        _svm_tn[tone].prior = vagatrptr->prior;
        _svm_tn[tone].mode = vagatrptr->mode;
        _svm_tn[tone].vol = vagatrptr->vol;
        _svm_tn[tone].pan = vagatrptr->pan;
        _svm_tn[tone].center = vagatrptr->center;
        _svm_tn[tone].shift = vagatrptr->shift;
        _svm_tn[tone].max = vagatrptr->max;
        _svm_tn[tone].min = vagatrptr->min;
        _svm_tn[tone].vibW = vagatrptr->vibW;
        _svm_tn[tone].vibT = vagatrptr->vibT;
        _svm_tn[tone].porW = vagatrptr->porW;
        _svm_tn[tone].porT = vagatrptr->porT;
        _svm_tn[tone].pbmin = vagatrptr->pbmin;
        _svm_tn[tone].pbmax = vagatrptr->pbmax;
        _svm_tn[tone].adsr1 = vagatrptr->adsr1;
        _svm_tn[tone].adsr2 = vagatrptr->adsr2;
        _svm_tn[tone].prog = vagatrptr->prog;
        _svm_tn[tone].vag = vagatrptr->vag;
        return 0;
    }
    return -1;
}
#else
#include "libsnd_i.h"

short SsUtSetVagAtr(
    short vabId, short progNum, short toneNum, VagAtr* pVagAttr) {
    s16 toneIndex;
    if (_svm_vab_used[vabId] == 1) {
        SpuVmVSetUp(vabId, progNum);
        toneIndex = toneNum + _svm_cur.field_7_fake_program * 0x10;
        _svm_tn[toneIndex].prior = pVagAttr->prior;
        _svm_tn[toneIndex].mode = pVagAttr->mode;
        _svm_tn[toneIndex].vol = pVagAttr->vol;
        _svm_tn[toneIndex].pan = pVagAttr->pan;
        _svm_tn[toneIndex].center = pVagAttr->center;
        _svm_tn[toneIndex].shift = pVagAttr->shift;
        _svm_tn[toneIndex].max = pVagAttr->max;
        _svm_tn[toneIndex].min = pVagAttr->min;
        _svm_tn[toneIndex].vibW = pVagAttr->vibW;
        _svm_tn[toneIndex].vibT = pVagAttr->vibT;
        _svm_tn[toneIndex].porW = pVagAttr->porW;
        _svm_tn[toneIndex].porT = pVagAttr->porT;
        _svm_tn[toneIndex].pbmin = pVagAttr->pbmin;
        _svm_tn[toneIndex].pbmax = pVagAttr->pbmax;
        _svm_tn[toneIndex].adsr1 = pVagAttr->adsr1;
        _svm_tn[toneIndex].adsr2 = pVagAttr->adsr2;
        _svm_tn[toneIndex].prog = pVagAttr->prog;
        _svm_tn[toneIndex].vag = pVagAttr->vag;
        return 0;
    }
    return -1;
}
#endif
