#if VERSION >= 36
#include "libsnd_i.h"

void _SsSndTempo(short seq_no, short sep_no) {
    struct SeqStruct* score;

    score = &_ss_score[seq_no][sep_no];
    score->unkA8--;
    if (score->unkA8 < 0) {
        _ss_score[seq_no][sep_no].flags &= ~0x40;
        _ss_score[seq_no][sep_no].flags &= ~0x80;
        return;
    }
    if (score->unk4E > 0) {
        if (score->unkA8 % score->unk4E == 0) {
            if (score->unk94 > score->unkAC) {
                score->unk94--;
            } else if (score->unk94 < score->unkAC) {
                score->unk94++;
            }
            score->unk54 =
                (score->resolution * score->unk94 * 10) / (VBLANK_MINUS * 60);
            if (score->unk54 <= 0) {
                score->unk54 = 1;
            }
            if (score->unkA8 == 0 || score->unk94 == score->unkAC) {
                _ss_score[seq_no][sep_no].flags &= ~0x40;
                _ss_score[seq_no][sep_no].flags &= ~0x80;
            }
        }
    } else {
        if (score->unk94 > score->unkAC) {
            score->unk94 += score->unk4E;
            if (score->unk94 < score->unkAC) {
                score->unk94 = score->unkAC;
            }
        } else if (score->unk94 < score->unkAC) {
            score->unk94 -= score->unk4E;
            if (score->unk94 > score->unkAC) {
                score->unk94 = score->unkAC;
            }
        }
        score->unk54 =
            (score->resolution * score->unk94 * 10) / (VBLANK_MINUS * 60);
        if (score->unk54 <= 0) {
            score->unk54 = 1;
        }
        if (score->unkA8 == 0 || score->unk94 == score->unkAC) {
            _ss_score[seq_no][sep_no].flags &= ~0x40;
            _ss_score[seq_no][sep_no].flags &= ~0x80;
        }
    }
}
#else
#include "libsnd_i.h"

#if VERSION <= 33
#define _SsSndTempo Snd_tempo
#endif

#if VERSION == 26
#define SC _ss_score[arg0][arg1]
void Snd_tempo(s16 arg0, s16 arg1) {
    SC.unkA0--;

    if (SC.unk44 > 0) {
        if ((SC.unkA0 % ((u32)SC.unk44)) == 0) {
            if (SC.unk8c > SC.unkA4) {
                SC.unk8c--;
            } else if (SC.unk8c < SC.unkA4) {
                SC.unk8c++;
            }
            SC.unk70 = ((u32)((SC.unk4a * SC.unk8c) * 0xA)) /
                       ((u32)(VBLANK_MINUS * 0x3C));
            if (SC.unk70 <= 0) {
                SC.unk70 = 1;
            }
            if ((SC.unkA0 == 0) || (SC.unk8c == SC.unkA4)) {
                SC.unk90 &= ~0x40;
                SC.unk90 &= ~0x80;
            }
        }
    } else {
        if (SC.unk8c > SC.unkA4) {
            SC.unk8c = SC.unk44 + SC.unk8c;
            if (SC.unk8c < SC.unkA4) {
                SC.unk8c = SC.unkA4;
            }
        } else if (SC.unk8c < SC.unkA4) {
            SC.unk8c = SC.unk8c - SC.unk44;
            if (SC.unk8c > SC.unkA4) {
                SC.unk8c = SC.unkA4;
            }
        }
        SC.unk70 = ((u32)((SC.unk4a * SC.unk8c) * 0xA)) /
                   ((u32)(VBLANK_MINUS * 0x3C));
        if (SC.unk70 <= 0) {
            SC.unk70 = 1;
        }
        if ((SC.unkA0 == 0) || (SC.unk8c == SC.unkA4)) {
            SC.unk90 &= ~0x40;
            SC.unk90 &= ~0x80;
        }
    }
}
#undef SC
#else

void _SsSndTempo(s16 arg0, s16 arg1) {
    s32 var_v0_2;
    s16 temp_lo;
    u32 temp_t0;

    struct SeqStruct* pSeq = &_ss_score[arg0][arg1];

    pSeq->unkA0--;

    if (pSeq->unk44 > 0) {
        if ((pSeq->unkA0 % ((u32)pSeq->unk44)) == 0) {
            if ((pSeq->unk8c > pSeq->unkA4)) {
                pSeq->unk8c--;
            } else if (pSeq->unk8c < pSeq->unkA4) {
                pSeq->unk8c++;
            }

            pSeq->unk70 = ((u32)((pSeq->unk4a * pSeq->unk8c) * 0xA)) /
                          ((u32)(VBLANK_MINUS * 0x3C));
            if (pSeq->unk70 <= 0) {
                pSeq->unk70 = 1;
            }
            if ((pSeq->unkA0 == 0) || (pSeq->unk8c == pSeq->unkA4)) {
                _ss_score[arg0][arg1].unk90 &= ~0x40;
                _ss_score[arg0][arg1].unk90 &= ~0x80;
            }
        }
    } else {
        if (pSeq->unk8c > pSeq->unkA4) {
            pSeq->unk8c = pSeq->unk44 + pSeq->unk8c;
            if (pSeq->unk8c < pSeq->unkA4) {
                pSeq->unk8c = pSeq->unkA4;
            }
        } else if (pSeq->unk8c < pSeq->unkA4) {
            pSeq->unk8c = pSeq->unk8c - pSeq->unk44;
            if (pSeq->unkA4 < (pSeq->unk8c)) {
                pSeq->unk8c = pSeq->unkA4;
            }
        }

        pSeq->unk70 = ((u32)((pSeq->unk4a * pSeq->unk8c) * 0xA)) /
                      ((u32)(VBLANK_MINUS * 0x3C));
        if (pSeq->unk70 <= 0) {
            pSeq->unk70 = 1;
        }
        if ((pSeq->unkA0 == 0) || (pSeq->unk8c == pSeq->unkA4)) {
            _ss_score[arg0][arg1].unk90 &= ~0x40;
            _ss_score[arg0][arg1].unk90 &= ~0x80;
        }
    }
}
#endif
#endif
