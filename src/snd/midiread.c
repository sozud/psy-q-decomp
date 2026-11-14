#include "libsnd_i.h"

#if VERSION >= 41
void _SsSndNextSep(s16, s16);
int _SsGetSeqData(s16 seq_no, s16 sep_no);
void _SsSeqGetEof(s16 seq_no, s16 sep_no, u8 type);

void _SsSeqPlay(s16 seq_no, s16 sep_no) {
    struct SeqStruct* score;
    s32 delta;

    score = &_ss_score[seq_no][sep_no];
    if (score->delta_value - score->unk54 > 0) {
        if (score->unk52 > 0) {
            score->unk52--;
        } else if (score->unk52 == 0) {
            score->unk52 = score->unk54;
            score->delta_value--;
        } else {
            score->delta_value -= score->unk54;
        }
    } else if (score->delta_value <= score->unk54) {
        delta = score->delta_value;
        do {
            do {
                _SsGetSeqData(seq_no, sep_no);
            } while (score->delta_value == 0);
            delta += score->delta_value;
        } while (delta < score->unk54);
        score->delta_value = delta - score->unk54;
    }
}

void _SsSeqGetEof(s16 seq_no, s16 sep_no, u8 type) {
    struct SeqStruct* score;

    score = &_ss_score[seq_no][sep_no];
    score->unk21++;
    if (score->l_count == 0) {
        score->unk88 = 0;
        score->unk1C = 0;
        score->delta_value = 0;
        if (_ss_score[seq_no][sep_no].flags & 0x400) {
            score->seq_ptr = score->loop_pos;
        } else {
            score->seq_ptr = score->read_pos;
        }
    } else if (score->unk21 < score->l_count) {
        score->unk88 = 0;
        score->unk1C = 0;
        score->delta_value = 0;
        if (_ss_score[seq_no][sep_no].flags & 0x400) {
            score->seq_ptr = score->loop_pos;
            score->next_sep_pos = score->loop_pos;
        } else {
            score->seq_ptr = score->read_pos;
            score->next_sep_pos = score->read_pos;
        }
    } else {
        _ss_score[seq_no][sep_no].flags &= ~SEQ_FLAG_1;
        _ss_score[seq_no][sep_no].flags &= ~SEQ_FLAG_8;
        _ss_score[seq_no][sep_no].flags &= ~SEQ_FLAG_2;
        _ss_score[seq_no][sep_no].flags |= SEQ_FLAG_200;
        _ss_score[seq_no][sep_no].flags |= SEQ_FLAG_4;
        score->play_mode = 0;
        if (_ss_score[seq_no][sep_no].flags & 0x400) {
            score->next_sep_pos = score->loop_pos;
        } else {
            score->next_sep_pos = score->read_pos;
        }
        if (score->unk22 != -1) {
            score->play_mode = 0;
            _SsSndNextSep(score->unk22, score->unk23);
            _SsVmSeqKeyOff(seq_no | (sep_no << 8));
        }
        _SsVmSeqKeyOff(seq_no | (sep_no << 8));
        score->delta_value = score->unk54;
    }
}

int _SsGetSeqData(s16 seq_no, s16 sep_no) {
    struct SeqStruct* score;
    u8 data;
    u8 note;
    u8 vel;
    u8 arg;
    s32 ret;

    score = &_ss_score[seq_no][sep_no];
    data = *score->seq_ptr++;
    ret = 0;
    if ((_ss_score[seq_no][sep_no].flags & 0x401) == 0x401) {
        if (score->seq_ptr == (u8*)score->unk10 + 1) {
            _SsSeqGetEof(seq_no, sep_no, ((u8*)score->unk10)[1]);
            return -1;
        }
    }
    if (data & 0x80) {
        score->channel_idx = data & 0x0F;
        switch (data & 0xF0) {
        case 0x90:
            score->running_status = 0x90;
            note = *score->seq_ptr++;
            vel = *score->seq_ptr++;
            score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
            SsFCALL.noteon(seq_no, sep_no, note, vel);
            break;
        case 0xB0:
            score->running_status = 0xB0;
            arg = *score->seq_ptr++;
            SsFCALL.control[CC_NUMBER](seq_no, sep_no, arg);
            break;
        case 0xC0:
            score->running_status = 0xC0;
            arg = *score->seq_ptr++;
            SsFCALL.programchange(seq_no, sep_no, arg);
            break;
        case 0xE0:
            score->running_status = 0xE0;
            score->seq_ptr++;
            SsFCALL.pitchbend(seq_no, sep_no);
            break;
        case 0xF0:
            score->running_status = 0xFF;
            arg = *score->seq_ptr++;
            if (arg == 0x2F) {
                ret = 1;
                _SsSeqGetEof(seq_no, sep_no, arg);
            } else {
                SsFCALL.metaevent(seq_no, sep_no, arg);
            }
            break;
        }
    } else {
        switch (score->running_status) {
        case 0x90:
            vel = *score->seq_ptr++;
            score->delta_value = _SsReadDeltaValue(seq_no, sep_no);
            SsFCALL.noteon(seq_no, sep_no, data, vel);
            break;
        case 0xB0:
            SsFCALL.control[CC_NUMBER](seq_no, sep_no, data);
            break;
        case 0xC0:
            SsFCALL.programchange(seq_no, sep_no, data);
            break;
        case 0xE0:
            SsFCALL.pitchbend(seq_no, sep_no);
            break;
        case 0xFF:
            if (data == 0x2F) {
                ret = 1;
                _SsSeqGetEof(seq_no, sep_no, data);
            } else {
                SsFCALL.metaevent(seq_no, sep_no, data);
            }
            break;
        }
    }
    return ret;
}
#else


void _SsSndNextSep(s16, s16);
void _SsSeqGetEof();
#if VERSION >= 40
int _SsGetSeqData(s16 access_num, s16 seq_num);
#else
void _SsGetSeqData(s16 access_num, s16 seq_num);
#endif

void _SsSeqPlay(s16 arg0, s16 arg1) {
    struct SeqStruct* score = &_ss_score[arg0][arg1];
    s32 var_s0;

    if (score->delta_value - score->unk54 > 0) {
        if (score->unk52 > 0) {
            score->unk52--;
        } else if (score->unk52 == 0) {
            score->unk52 = score->unk54;
            score->delta_value--;
        } else {
            score->delta_value -= score->unk54;
        }
    } else if (score->delta_value <= score->unk54) {
        var_s0 = score->delta_value;
        do {
            do {
                _SsGetSeqData(arg0, arg1);
            } while (score->delta_value == 0);
            var_s0 += score->delta_value;
        } while (var_s0 < score->unk54);
        score->delta_value = var_s0 - score->unk54;
    }
}

#if VERSION >= 40
void _SsSeqGetEof(access_num, seq_num)
short access_num;
short seq_num;
{
    struct SeqStruct* score = &_ss_score[access_num][seq_num];

    score->unk21++;
    if (score->l_count == 0) {
        score->unk88 = 0;
        score->unk1C = 0;
        score->delta_value = 0;
        if (_ss_score[access_num][seq_num].flags & 0x400) {
            score->seq_ptr = score->loop_pos;
        } else {
            score->seq_ptr = score->read_pos;
        }
    } else if (score->unk21 < score->l_count) {
        score->unk88 = 0;
        score->unk1C = 0;
        score->delta_value = 0;
        if (_ss_score[access_num][seq_num].flags & 0x400) {
            score->seq_ptr = score->loop_pos;
            score->next_sep_pos = score->loop_pos;
        } else {
            score->seq_ptr = score->read_pos;
            score->next_sep_pos = score->read_pos;
        }
    } else {
        _ss_score[access_num][seq_num].flags &= ~1;
        _ss_score[access_num][seq_num].flags &= ~8;
        _ss_score[access_num][seq_num].flags &= ~2;
        _ss_score[access_num][seq_num].flags |= 0x200;
        _ss_score[access_num][seq_num].flags |= 4;
        score->play_mode = 0;
        if (_ss_score[access_num][seq_num].flags & 0x400) {
            score->next_sep_pos = score->loop_pos;
        } else {
            score->next_sep_pos = score->read_pos;
        }
        if (score->unk22 != 0xFF) {
            score->play_mode = 0;
            _SsSndNextSep(score->unk22, score->unk23);
            _SsVmSeqKeyOff(access_num | (seq_num << 8));
        }
        _SsVmSeqKeyOff(access_num | (seq_num << 8));
        score->delta_value = score->unk54;
    }
}
#else
void _SsSeqGetEof(access_num, seq_num)
short access_num;
short seq_num;
{
    struct SeqStruct* score = &_ss_score[access_num][seq_num];

    score->unk21++;
    if (score->l_count == 0) {
        score->unk88 = 0;
        score->unk1C = 0;
        score->delta_value = 0;
        score->seq_ptr = score->read_pos;
    } else if (score->unk21 < score->l_count) {
        score->unk88 = 0;
        score->unk1C = 0;
        score->delta_value = 0;
        score->seq_ptr = score->read_pos;
        score->next_sep_pos = score->read_pos;
    } else {
        _ss_score[access_num][seq_num].flags &= ~1;
        _ss_score[access_num][seq_num].flags &= ~8;
        _ss_score[access_num][seq_num].flags &= ~2;
        _ss_score[access_num][seq_num].flags |= 0x200;
        _ss_score[access_num][seq_num].flags |= 4;
        score->play_mode = 0;
        score->next_sep_pos = score->read_pos;
        if (score->unk22 != 0xFF) {
            score->play_mode = 0;
            _SsSndNextSep(score->unk22, score->unk23);
            _SsVmSeqKeyOff(access_num | (seq_num << 8));
        }
        _SsVmSeqKeyOff(access_num | (seq_num << 8));
        score->delta_value = score->unk54;
    }
}


#endif

#if VERSION < 40
void _SsGetSeqData(s16 access_num, s16 seq_num) {
    struct SeqStruct* score = &_ss_score[access_num][seq_num];
    u8 data;
    u8 channel;
    u8 note;
    u8 velocity;

    data = *score->seq_ptr++;
    channel = data & 0xF;
    if (data & 0x80) {
        score->channel_idx = channel;
        switch (data & 0xF0) {
        case 0x90:
            score->running_status = 0x90;
            note = *score->seq_ptr++;
            velocity = *score->seq_ptr++;
            score->delta_value = _SsReadDeltaValue(access_num, seq_num);
            SsFCALL.noteon(access_num, seq_num, note, velocity);
            break;
        case 0xB0:
            score->running_status = 0xB0;
            data = *score->seq_ptr++;
            SsFCALL.control[0](access_num, seq_num, data);
            break;
        case 0xC0:
            score->running_status = 0xC0;
            data = *score->seq_ptr++;
            SsFCALL.programchange(access_num, seq_num, data);
            break;
        case 0xE0:
            score->running_status = 0xE0;
            score->seq_ptr++;
            SsFCALL.pitchbend(access_num, seq_num);
            break;
        case 0xF0:
            score->running_status = 0xFF;
            score->channel_idx = channel;
            data = *score->seq_ptr++;
            if (data == 0x2F) {
                _SsSeqGetEof(access_num, seq_num, data);
            } else {
                SsFCALL.metaevent(access_num, seq_num, data);
            }
            break;
        }
    } else {
        switch (score->running_status) {
        case 0x90:
            velocity = *score->seq_ptr++;
            score->delta_value = _SsReadDeltaValue(access_num, seq_num);
            SsFCALL.noteon(access_num, seq_num, data, velocity);
            break;
        case 0xB0:
            SsFCALL.control[0](access_num, seq_num, data);
            break;
        case 0xC0:
            SsFCALL.programchange(access_num, seq_num, data);
            break;
        case 0xE0:
            SsFCALL.pitchbend(access_num, seq_num);
            break;
        case 0xFF:
            if (data == 0x2F) {
                _SsSeqGetEof(access_num, seq_num, data);
            } else {
                SsFCALL.metaevent(access_num, seq_num, data);
            }
            break;
        }
    }
}
#else
int _SsGetSeqData(s16 access_num, s16 seq_num) {
    struct SeqStruct* score = &_ss_score[access_num][seq_num];
    u8 data;
    u8 channel;
    u8 note;
    u8 velocity;
    int ret;

    ret = 0;
    data = *score->seq_ptr++;
    if ((_ss_score[access_num][seq_num].flags & 0x401) == 0x401 &&
        score->seq_ptr == (u8*)score->unk10 + 1) {
        _SsSeqGetEof(access_num, seq_num, ((u8*)score->unk10)[1]);
        return -1;
    }
    channel = data & 0xF;
    if (data & 0x80) {
        score->channel_idx = channel;
        switch (data & 0xF0) {
        case 0x90:
            score->running_status = 0x90;
            note = *score->seq_ptr++;
            velocity = *score->seq_ptr++;
            score->delta_value = _SsReadDeltaValue(access_num, seq_num);
            SsFCALL.noteon(access_num, seq_num, note, velocity);
            break;
        case 0xB0:
            score->running_status = 0xB0;
            data = *score->seq_ptr++;
            SsFCALL.control[0](access_num, seq_num, data);
            break;
        case 0xC0:
            score->running_status = 0xC0;
            data = *score->seq_ptr++;
            SsFCALL.programchange(access_num, seq_num, data);
            break;
        case 0xE0:
            score->running_status = 0xE0;
            score->seq_ptr++;
            SsFCALL.pitchbend(access_num, seq_num);
            break;
        case 0xF0:
            score->running_status = 0xFF;
            score->channel_idx = channel;
            data = *score->seq_ptr++;
            if (data == 0x2F) {
                ret = 1;
                _SsSeqGetEof(access_num, seq_num, data);
            } else {
                SsFCALL.metaevent(access_num, seq_num, data);
            }
            break;
        }
    } else {
        switch (score->running_status) {
        case 0x90:
            velocity = *score->seq_ptr++;
            score->delta_value = _SsReadDeltaValue(access_num, seq_num);
            SsFCALL.noteon(access_num, seq_num, data, velocity);
            break;
        case 0xB0:
            SsFCALL.control[0](access_num, seq_num, data);
            break;
        case 0xC0:
            SsFCALL.programchange(access_num, seq_num, data);
            break;
        case 0xE0:
            SsFCALL.pitchbend(access_num, seq_num);
            break;
        case 0xFF:
            if (data == 0x2F) {
                ret = 1;
                _SsSeqGetEof(access_num, seq_num, data);
            } else {
                SsFCALL.metaevent(access_num, seq_num, data);
            }
            break;
        }
    }
    return ret;
}
#endif
#endif
