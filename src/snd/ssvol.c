#if VERSION >= 36
#include "libsnd_i.h"

#if VERSION == 40
#define ARG 0
#else
#define ARG 1
#endif

void _SsSndSetVol(short sep_access_num, short seq_num, short voll, short volr) {
    _SsVmSetSeqVol(sep_access_num | (seq_num << 8), voll, volr, ARG);
}

void SsSeqSetVol(short seq_access_num, short voll, short volr) {
    _SsVmSetSeqVol(seq_access_num, voll, volr, ARG);
}

void SsSepSetVol(short sep_access_num, short seq_num, short voll, short volr) {
    _SsVmSetSeqVol(sep_access_num | (seq_num << 8), voll, volr, ARG);
}

void SsSeqGetVol(short access_num, short seq_num, short* voll, short* volr) {
    _SsVmGetSeqVol(access_num | (seq_num << 8), voll, volr);
}
#elif VERSION <= 30
#include "libsnd_i.h"

void Snd_SetVol(short sep_no, short seq_no, short voll, short volr);

void SsSeqSetVol(short seq_no, short voll, short volr) {
    Snd_SetVol(seq_no, 0, voll, volr);
}

void SsSepSetVol(short sep_no, short seq_no, short voll, short volr) {
    Snd_SetVol(sep_no, seq_no, voll, volr);
}

void Snd_SetVol(short sep_no, short seq_no, short voll, short volr) {
#if VERSION == 30
    struct SeqStruct* score = &_ss_score[sep_no][seq_no];
    score->unk74 = voll;
    score->unk76 = volr;
#else
    _ss_score[sep_no][seq_no].unk74 = voll;
    _ss_score[sep_no][seq_no].unk76 = volr;
#endif
    SpuVmSetSeqVol(sep_no, voll, volr, 0);
}
#else
#include "libsnd_i.h"

#if VERSION == 35 || VERSION == 33
#define ARG 0
#else
#define ARG 1
#endif

#if VERSION == 33
#define _SsSndSetVol Snd_SetVol
#endif

void _SsSndSetVol(s32 arg0, s32 arg1, u16 arg2, u16 arg3) {
    SpuVmSetSeqVol(arg0 | (arg1 << 8), arg2, arg3, ARG);
}

void SsSeqSetVol(short arg0, short arg1, short arg2) {
    SpuVmSetSeqVol(arg0, arg1, arg2, ARG);
}

void SsSepSetVol(s16 sep_access_num, s16 seq_num, s16 voll, s16 volr) {
    SpuVmSetSeqVol(
        sep_access_num | (seq_num << 8), voll & 0xFFFF, volr & 0xFFFF, ARG);
}

#if VERSION != 33
void SsSeqGetVol(s16 access_num, s16 seq_num, s16* voll, s16* volr) {
    SpuVmGetSeqVol(access_num | (seq_num << 8), voll, volr);
}
#endif
#endif
