#if VERSION >= 36
#include "libsnd_i.h"

#if VERSION == 40
#define ARG 0
#else
#define ARG 1
#endif

void _SsSndCrescendo(short seq_access_num, short arg1) {
    struct SeqStruct* score = &_ss_score[seq_access_num][arg1];
    unsigned short voll, volr;

    score->v_time_r--;
    if (score->v_time_r < 0) {
        _ss_score[seq_access_num][arg1].flags &= ~0x10;
    } else if (score->unk4C > 0) {
        if ((score->v_time_r % score->unk4C) == 0) {
            score->vol_r--;
            if (score->vol_r >= 0) {
                _SsVmGetSeqVol(seq_access_num | (arg1 << 8), &voll, &volr);
                if (voll + 1 <= voll + score->vol_r) {
                    _SsVmSetSeqVol(
                        seq_access_num | (arg1 << 8), voll + 1, volr + 1, ARG);
                }
            } else {
                _SsVmSetSeqVol(seq_access_num | (arg1 << 8), 127, 127, ARG);
                _ss_score[seq_access_num][arg1].flags &= ~0x10;
            }
            if (score->v_time_r == 0 || score->vol_r == 0) {
                _ss_score[seq_access_num][arg1].flags &= ~0x10;
            }
        }
    } else if (score->unk4C < 0) {
        score->vol_r += score->unk4C;
        if (score->vol_r >= 0) {
            _SsVmGetSeqVol(seq_access_num | (arg1 << 8), &voll, &volr);
            if (voll - score->unk4C >= 127 && volr - score->unk4C >= 127) {
                _SsVmSetSeqVol(seq_access_num | (arg1 << 8), 127, 127, ARG);
            }
            if ((score->v_time_l - score->v_time_r) * -score->unk4C <
                score->vol_l) {
                _SsVmSetSeqVol(seq_access_num | (arg1 << 8),
                               voll - score->unk4C, volr - score->unk4C, ARG);
            }
        } else {
            _SsVmSetSeqVol(seq_access_num | (arg1 << 8), 127, 127, ARG);
            _ss_score[seq_access_num][arg1].flags &= ~0x10;
        }
        if (score->v_time_r == 0 || score->vol_r == 0) {
            _ss_score[seq_access_num][arg1].flags &= ~0x10;
        }
    }
    _SsVmGetSeqVol(seq_access_num | (arg1 << 8), &score->unk5C, &score->unk5E);
}
#elif VERSION <= 33
#include "libsnd_i.h"

#if VERSION <= 30
#define SEQ_ID arg0
#else
#define SEQ_ID (arg0 | (arg1 << 8))
#endif

#if VERSION == 26
void Snd_crescendo(s16 arg0, s16 arg1) {
    u16 voll, volr;

    _ss_score[arg0][arg1].unk98--;
    if (_ss_score[arg0][arg1].unk42 > 0) {
        if ((_ss_score[arg0][arg1].unk98 % _ss_score[arg0][arg1].unk42) == 0) {
            if (_ss_score[arg0][arg1].unk3E > 0) {
                _ss_score[arg0][arg1].unk40--;
                if (_ss_score[arg0][arg1].unk40 >= 0) {
                    SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
                    if (voll + 1 < 0x80 && volr + 1 < 0x80) {
                        SpuVmSetSeqVol(
                            SEQ_ID, voll + 1, volr + 1, 0);
                    } else {
                        SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                        _ss_score[arg0][arg1].unk90 &= ~0x10;
                    }
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x10;
                }
            } else if (_ss_score[arg0][arg1].unk3E < 0) {
                _ss_score[arg0][arg1].unk40++;
                if (_ss_score[arg0][arg1].unk40 <= 0) {
                    SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
                    if (voll - 1 >= 0 && volr - 1 >= 0) {
                        SpuVmSetSeqVol(
                            SEQ_ID, voll - 1, volr - 1, 0);
                    } else {
                        SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                        _ss_score[arg0][arg1].unk90 &= ~0x10;
                    }
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x10;
                }
            }
            if ((_ss_score[arg0][arg1].unk98 == 0) || (_ss_score[arg0][arg1].unk40 == 0)) {
                _ss_score[arg0][arg1].unk90 &= ~0x10;
            }
        }
    } else {
        if (_ss_score[arg0][arg1].unk3E > 0) {
            _ss_score[arg0][arg1].unk40 += _ss_score[arg0][arg1].unk42;
            SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
            if (_ss_score[arg0][arg1].unk40 >= 0) {
                if (voll - _ss_score[arg0][arg1].unk42 < 0x80 &&
                    volr - _ss_score[arg0][arg1].unk42 < 0x80) {
                    SpuVmSetSeqVol(SEQ_ID, voll - _ss_score[arg0][arg1].unk42,
                                   volr - _ss_score[arg0][arg1].unk42, 0);
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x10;
                }
            } else {
                SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                _ss_score[arg0][arg1].unk90 &= ~0x10;
            }
        } else if (_ss_score[arg0][arg1].unk3E < 0) {
            _ss_score[arg0][arg1].unk40 -= _ss_score[arg0][arg1].unk42;
            SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
            if (_ss_score[arg0][arg1].unk40 <= 0) {
                if (voll >= -_ss_score[arg0][arg1].unk42 && volr >= -_ss_score[arg0][arg1].unk42) {
                    SpuVmSetSeqVol(SEQ_ID, voll + _ss_score[arg0][arg1].unk42,
                                   volr + _ss_score[arg0][arg1].unk42, 0);
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x10;
                }
            } else {
                SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                _ss_score[arg0][arg1].unk90 &= ~0x10;
            }
        }
        if ((_ss_score[arg0][arg1].unk98 == 0) || (_ss_score[arg0][arg1].unk40 == 0)) {
            _ss_score[arg0][arg1].unk90 &= ~0x10;
        }
    }
    SpuVmGetSeqVol(SEQ_ID, &_ss_score[arg0][arg1].unk78, &_ss_score[arg0][arg1].unk7A);
}
#else
void Snd_crescendo(s16 arg0, s16 arg1) {
    struct SeqStruct* score = &_ss_score[arg0][arg1];
    u16 voll, volr;

    score->unk98--;
    if (score->unk42 > 0) {
        if ((score->unk98 % score->unk42) == 0) {
            if (score->unk3E > 0) {
                score->unk40--;
                if (score->unk40 >= 0) {
                    SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
                    if (voll + 1 < 0x80 && volr + 1 < 0x80) {
                        SpuVmSetSeqVol(
                            SEQ_ID, voll + 1, volr + 1, 0);
                    } else {
                        SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                        _ss_score[arg0][arg1].unk90 &= ~0x10;
                    }
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x10;
                }
            } else if (score->unk3E < 0) {
                score->unk40++;
                if (score->unk40 <= 0) {
                    SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
                    if (voll - 1 >= 0 && volr - 1 >= 0) {
                        SpuVmSetSeqVol(
                            SEQ_ID, voll - 1, volr - 1, 0);
                    } else {
                        SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                        _ss_score[arg0][arg1].unk90 &= ~0x10;
                    }
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x10;
                }
            }
            if ((score->unk98 == 0) || (score->unk40 == 0)) {
                _ss_score[arg0][arg1].unk90 &= ~0x10;
            }
        }
    } else {
        if (score->unk3E > 0) {
            score->unk40 += score->unk42;
            SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
            if (score->unk40 >= 0) {
                if (voll - score->unk42 < 0x80 &&
                    volr - score->unk42 < 0x80) {
                    SpuVmSetSeqVol(SEQ_ID, voll - score->unk42,
                                   volr - score->unk42, 0);
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x10;
                }
            } else {
                SpuVmSetSeqVol(SEQ_ID, 0x7F, 0x7F, 0);
                _ss_score[arg0][arg1].unk90 &= ~0x10;
            }
        } else if (score->unk3E < 0) {
            score->unk40 -= score->unk42;
            SpuVmGetSeqVol(SEQ_ID, &voll, &volr);
            if (score->unk40 <= 0) {
                if (voll >= -score->unk42 && volr >= -score->unk42) {
                    SpuVmSetSeqVol(SEQ_ID, voll + score->unk42,
                                   volr + score->unk42, 0);
                } else {
                    SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                    _ss_score[arg0][arg1].unk90 &= ~0x10;
                }
            } else {
                SpuVmSetSeqVol(SEQ_ID, 0, 0, 0);
                _ss_score[arg0][arg1].unk90 &= ~0x10;
            }
        }
        if ((score->unk98 == 0) || (score->unk40 == 0)) {
            _ss_score[arg0][arg1].unk90 &= ~0x10;
        }
    }
    SpuVmGetSeqVol(SEQ_ID, &score->unk78, &score->unk7A);
}
#endif
#else
#include "libsnd_i.h"

void _SsSndCrescendo(s16 arg0, s16 arg1) {
    struct SeqStruct* score = &_ss_score[arg0][arg1];
    u16 voll, volr;

    score->unk98--;

    if (score->unk42 > 0) {
        if ((score->unk98 % score->unk42) == 0) {
            score->unk40--;
            if (score->unk40 >= 0) {
                SpuVmGetSeqVol(arg0 | (arg1 << 8), &voll, &volr);
                if ((voll + 1) <= (voll + score->unk40)) {
                    SpuVmSetSeqVol(arg0 | (arg1 << 8), voll + 1, volr + 1, 0);
                }
            } else {
                SpuVmSetSeqVol(arg0 | (arg1 << 8), 0x7F, 0x7F, 0);
                _ss_score[arg0][arg1].unk90 &= ~0x10;
            }
            if ((score->unk98 == 0) || (score->unk40 == 0)) {
                _ss_score[arg0][arg1].unk90 &= ~0x10;
            }
        }
    } else if (score->unk42 < 0) {
        score->unk40 += score->unk42;
        if (score->unk40 >= 0) {
            SpuVmGetSeqVol(arg0 | (arg1 << 8), &voll, &volr);
            if (((voll - score->unk42) >= 0x7F) &&
                ((volr - score->unk42) >= 0x7F)) {
                SpuVmSetSeqVol(arg0 | (arg1 << 8), 0x7F, 0x7F, 0);
            }
            if (((score->unk94 - score->unk98) * -score->unk42) <
                score->unk3E) {
                SpuVmSetSeqVol(arg0 | (arg1 << 8), voll - score->unk42,
                               volr - score->unk42, 0);
            }
        } else {
            SpuVmSetSeqVol(arg0 | (arg1 << 8), 0x7F, 0x7F, 0);
            _ss_score[arg0][arg1].unk90 &= ~0x10;
        }
        if ((score->unk98 == 0) || (score->unk40 == 0)) {
            _ss_score[arg0][arg1].unk90 &= ~0x10;
        }
    }
    SpuVmGetSeqVol(arg0 | (arg1 << 8), &score->unk78, &score->unk7A);
}
#endif
