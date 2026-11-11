#include "libsnd_i.h"

#if VERSION <= 33
#if VERSION <= 30
#define SEQ_ID arg0
#else
#define SEQ_ID (arg0 | (arg1 << 8))
#endif

#if VERSION == 26
void Snd_decrescendo(s16 arg0, s16 arg1) {
    u16 voll, volr;

    _ss_score[arg0][arg1].unk98--;
    if (_ss_score[arg0][arg1].unk42 > 0) {
        if ((_ss_score[arg0][arg1].unk98 % _ss_score[arg0][arg1].unk42) == 0) {
            if (_ss_score[arg0][arg1].unk3E > 0) {
                _ss_score[arg0][arg1].unk40--;
                if (_ss_score[arg0][arg1].unk40 >= 0) {
                    SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
                    if (voll != 0 && volr != 0) {
                        SpuVmSetSeqVol(
                            SEQ_ID, voll - 1, volr - 1, 0);
                    } else {
                        SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                        _ss_score[arg0][arg1].unk90 &= ~0x20;
                    }
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x20;
                }
            } else if (_ss_score[arg0][arg1].unk3E < 0) {
                _ss_score[arg0][arg1].unk40++;
                if (_ss_score[arg0][arg1].unk40 <= 0) {
                    SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
                    if (voll + 1 < 0x80 && volr + 1 < 0x80) {
                        SpuVmSetSeqVol(
                            SEQ_ID, voll + 1, volr + 1, 0);
                    } else {
                        SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                        _ss_score[arg0][arg1].unk90 &= ~0x20;
                    }
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x20;
                }
            }
            if ((_ss_score[arg0][arg1].unk98 == 0) || (_ss_score[arg0][arg1].unk40 == 0)) {
                _ss_score[arg0][arg1].unk90 &= ~0x20;
            }
        }
    } else {
        if (_ss_score[arg0][arg1].unk3E > 0) {
            _ss_score[arg0][arg1].unk40 += _ss_score[arg0][arg1].unk42;
            if (_ss_score[arg0][arg1].unk40 >= 0) {
                SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
                if (voll >= -_ss_score[arg0][arg1].unk42 && volr >= -_ss_score[arg0][arg1].unk42) {
                    SpuVmSetSeqVol(SEQ_ID, voll + _ss_score[arg0][arg1].unk42,
                                   volr + _ss_score[arg0][arg1].unk42, 0);
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x20;
                }
            } else {
                SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                _ss_score[arg0][arg1].unk90 &= ~0x20;
            }
        } else if (_ss_score[arg0][arg1].unk3E < 0) {
            _ss_score[arg0][arg1].unk40 -= _ss_score[arg0][arg1].unk42;
            if (_ss_score[arg0][arg1].unk40 <= 0) {
                SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
                if (voll - _ss_score[arg0][arg1].unk42 < 0x80 &&
                    volr - _ss_score[arg0][arg1].unk42 < 0x80) {
                    SpuVmSetSeqVol(SEQ_ID, voll - _ss_score[arg0][arg1].unk42,
                                   volr - _ss_score[arg0][arg1].unk42, 0);
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x20;
                }
            } else {
                SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                _ss_score[arg0][arg1].unk90 &= ~0x20;
            }
        }
        if ((_ss_score[arg0][arg1].unk98 == 0) || (_ss_score[arg0][arg1].unk40 == 0)) {
            _ss_score[arg0][arg1].unk90 &= ~0x20;
        }
    }
    SpuVmGetSeqVol(SEQ_ID, &_ss_score[arg0][arg1].unk78, &_ss_score[arg0][arg1].unk7A);
}
#else
void Snd_decrescendo(s16 arg0, s16 arg1) {
    struct SeqStruct* score = &_ss_score[arg0][arg1];
    u16 voll, volr;

    score->unk98--;
    if (score->unk42 > 0) {
        if ((score->unk98 % score->unk42) == 0) {
            if (score->unk3E > 0) {
                score->unk40--;
                if (score->unk40 >= 0) {
                    SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
                    if (voll != 0 && volr != 0) {
                        SpuVmSetSeqVol(
                            SEQ_ID, voll - 1, volr - 1, 0);
                    } else {
                        SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                        _ss_score[arg0][arg1].unk90 &= ~0x20;
                    }
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x20;
                }
            } else if (score->unk3E < 0) {
                score->unk40++;
                if (score->unk40 <= 0) {
                    SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
                    if (voll + 1 < 0x80 && volr + 1 < 0x80) {
                        SpuVmSetSeqVol(
                            SEQ_ID, voll + 1, volr + 1, 0);
                    } else {
                        SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                        _ss_score[arg0][arg1].unk90 &= ~0x20;
                    }
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x20;
                }
            }
            if ((score->unk98 == 0) || (score->unk40 == 0)) {
                _ss_score[arg0][arg1].unk90 &= ~0x20;
            }
        }
    } else {
        if (score->unk3E > 0) {
            score->unk40 += score->unk42;
            if (score->unk40 >= 0) {
                SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
                if (voll >= -score->unk42 && volr >= -score->unk42) {
                    SpuVmSetSeqVol(SEQ_ID, voll + score->unk42,
                                   volr + score->unk42, 0);
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x20;
                }
            } else {
                SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                _ss_score[arg0][arg1].unk90 &= ~0x20;
            }
        } else if (score->unk3E < 0) {
            score->unk40 -= score->unk42;
            if (score->unk40 <= 0) {
                SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
                if (voll - score->unk42 < 0x80 &&
                    volr - score->unk42 < 0x80) {
                    SpuVmSetSeqVol(SEQ_ID, voll - score->unk42,
                                   volr - score->unk42, 0);
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x20;
                }
            } else {
                SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                _ss_score[arg0][arg1].unk90 &= ~0x20;
            }
        }
        if ((score->unk98 == 0) || (score->unk40 == 0)) {
            _ss_score[arg0][arg1].unk90 &= ~0x20;
        }
    }
    SpuVmGetSeqVol(SEQ_ID, &score->unk78, &score->unk7A);
}
#endif
#else

void _SsSndDecrescendo(s16 arg0, s16 arg1) {
    struct SeqStruct* score = &_ss_score[arg0][arg1];
    u16 voll, volr;

    score->unk98--;

    if (score->unk42 > 0) {
        if ((score->unk98 % score->unk42) == 0) {
            score->unk40--;
            if (score->unk40 > 0) {
                SpuVmGetSeqVol(arg0 | (arg1 << 8), &voll, &volr);
                if (((voll - score->unk40) > 0) &&
                    ((volr - score->unk40) > 0) && voll != 1) {
                    SpuVmSetSeqVol(arg0 | (arg1 << 8), voll - 1, volr - 1, 0);
                } else {
                    SpuVmSetSeqVol(arg0 | (arg1 << 8), 1, 1, 0);
                }
            } else {
                _ss_score[arg0][arg1].unk90 &= ~0x20;
            }
            if ((score->unk98 == 0) || (score->unk40 == 0)) {
                _ss_score[arg0][arg1].unk90 &= ~0x20;
            }
        }
    } else {
        score->unk40 += score->unk42;
        if (score->unk40 > 0) {
            SpuVmGetSeqVol(arg0 | (arg1 << 8), &voll, &volr);
            if ((score->unk3E >=
                 ((score->unk94 - score->unk98) * -score->unk42)) &&
                -score->unk42 < voll) {
                SpuVmSetSeqVol(arg0 | (arg1 << 8), voll + score->unk42,
                               volr + score->unk42, 0);
            } else {
                SpuVmSetSeqVol(arg0 | (arg1 << 8), 1, 1, 0);
            }
        } else {
            _ss_score[arg0][arg1].unk90 &= ~0x20;
        }
        if ((score->unk98 == 0) || (score->unk40 == 0)) {
            _ss_score[arg0][arg1].unk90 &= ~0x20;
        }
    }
    SpuVmGetSeqVol(arg0 | (arg1 << 8), &score->unk78, &score->unk7A);
}
#endif
