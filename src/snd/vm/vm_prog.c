s32 SpuVmSetProgVol(s16 vabId, s16 prog, u8 arg2) {
    if (SpuVmVSetUp(vabId, prog)) {
        return -1;
    }
    _svm_pg[prog].mvol = arg2;
    return _svm_pg[prog].mvol;
}

s32 SpuVmGetProgVol(s16 vabId, s16 prog) {
    if (SpuVmVSetUp(vabId, prog)) {
        return -1;
    }
    return _svm_pg[prog].mvol;
}

s32 SpuVmSetProgPan(s16 vabId, s16 prog, u8 pan) {
    if (SpuVmVSetUp(vabId, prog)) {
        return -1;
    }
    _svm_pg[prog].mpan = pan;
    return _svm_pg[prog].mpan;
}

s32 SpuVmGetProgPan(s16 vabId, s16 prog) {
    if (SpuVmVSetUp(vabId, prog)) {
        return -1;
    }
    return _svm_pg[prog].mpan;
}
