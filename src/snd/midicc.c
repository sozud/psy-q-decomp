#include "libsnd_i.h"


void _SsSetControlChange(s16 seq_no, s16 sep_no, u8 control) {
    struct SeqStruct* score;
    u8 data;

    score = &_ss_score[seq_no][sep_no];
    data = *score->seq_ptr++;
    switch (control) {
    case 0:
        score->vab_id = data;
        score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
        break;
    case 6:
        SsFCALL.control[CC_DATAENTRY](seq_no, sep_no, data);
        break;
    case 7:
        SsFCALL.control[CC_MAINVOL](seq_no, sep_no, data);
        break;
    case 10:
        SsFCALL.control[CC_PANPOT](seq_no, sep_no, data);
        break;
    case 11:
        SsFCALL.control[CC_EXPRESSION](seq_no, sep_no, data);
        break;
    case 64:
        SsFCALL.control[CC_DAMPER](seq_no, sep_no, data);
        break;
    case 91:
        SsFCALL.control[CC_EXTERNAL](seq_no, sep_no, data);
        break;
    case 98:
        SsFCALL.control[CC_NRPN1](seq_no, sep_no, data);
        break;
    case 99:
        SsFCALL.control[CC_NRPN2](seq_no, sep_no, data);
        break;
    case 100:
        SsFCALL.control[CC_RPN1](seq_no, sep_no, data);
        break;
    case 101:
        SsFCALL.control[CC_RPN2](seq_no, sep_no, data);
        break;
    case 121:
        SsFCALL.control[CC_RESETALL](seq_no, sep_no);
        break;
    default:
        score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
        break;
    }
}
