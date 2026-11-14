#include "libsnd_i.h"

#if VERSION >= 41
s32 _SsVmKeyOn(s16 seq_sep, s16 vabId, s16 prog, u16 note, u16 voll, u16 pan) {
    u8 vagIdx[0x80];
    u8 toneIdx[0x80];
    struct SeqStruct* score;
    u8 i;
    s32 ret;
    u16 tone;
    u8 vagCount;

    score = _ss_score[seq_sep & 0xFF] + ((seq_sep & 0xFF00) >> 8);
    ret = 0;
    if (_SsVmVSetUp(vabId, prog)) {
        return -1;
    }
    _svm_cur.seq_sep_no = seq_sep;
    _svm_cur.note = note;
    _svm_cur.fine = 0;
    if (seq_sep == 0x21) {
        _svm_cur.volume = voll;
    } else {
        _svm_cur.volume = (voll * score->vol[score->channel_idx]) / 127;
    }
    _svm_cur.pan = pan;
    _svm_cur.mvol = _svm_pg[prog].mvol;
    _svm_cur.mpan = _svm_pg[prog].mpan;
    _svm_cur.prog_tones = _svm_pg[prog].tones;
    if (_svm_cur.fake_program >= _svm_vh->ps) {
        return -1;
    }
    if (voll == 0) {
        ret = _SsVmKeyOff(seq_sep, vabId, prog, note);
    } else {
        vagCount = _SsVmSelectToneAndVag(toneIdx, vagIdx);
        for (i = 0; i < vagCount; i++) {
            _svm_cur.tone_vag_idx = vagIdx[i];
            _svm_cur.tone = toneIdx[i];
            tone = _svm_cur.tone + _svm_cur.fake_program * 16;
            _svm_cur.tone_prior = _svm_tn[tone].prior;
            _svm_cur.tone_vol = _svm_tn[tone].vol;
            _svm_cur.tone_pan = _svm_tn[tone].pan;
            _svm_cur.tone_center = _svm_tn[tone].center;
            _svm_cur.tone_shift = _svm_tn[tone].shift;
            _svm_cur.tone_mode = _svm_tn[tone].mode;
#if VERSION < 43
            _svm_cur.tone_min = _svm_tn[tone].min;
            _svm_cur.tone_max = _svm_tn[tone].max;
#endif
            _svm_cur.voice = _SsVmAlloc(0);
            if (_svm_cur.voice < _SsVmMaxVoice) {
                _svm_voice[_svm_cur.voice].unk1b = 1;
                _svm_voice[_svm_cur.voice].unk2 = 0;
                _svm_voice[_svm_cur.voice].seq_sep_no = seq_sep;
                _svm_voice[_svm_cur.voice].vabId = _svm_cur.vabId;
                _svm_voice[_svm_cur.voice].fake_program = _svm_cur.fake_program;
                _svm_voice[_svm_cur.voice].prog = prog;
                if (seq_sep != 0x21) {
                    _svm_voice[_svm_cur.voice].voll1 = voll;
                    _svm_voice[_svm_cur.voice].unkc = score->channel_idx;
                }
                _svm_voice[_svm_cur.voice].pan = pan;
                _svm_voice[_svm_cur.voice].tone = _svm_cur.tone;
                _svm_voice[_svm_cur.voice].note = note;
                _svm_voice[_svm_cur.voice].priority = _svm_cur.tone_prior;
                _svm_voice[_svm_cur.voice].vag_idx = _svm_cur.tone_vag_idx;
                _SsVmDoAllocate();
                if (_svm_cur.tone_vag_idx == 0xFF) {
                    vmNoiseOn(_svm_cur.voice);
                } else {
                    _SsVmKeyOnNow(vagCount, note2pitch() & 0xFFFF);
                }
                ret |= 1 << _svm_cur.voice;
            } else {
                ret = -1;
            }
        }
    }
    return ret;
}

int _SsVmKeyOff(short seq_sep_no, short vabId, short prog, unsigned short note) {
    u8 voice;
    s32 n;

    voice = 0;
    n = 0;
    if (_SsVmMaxVoice > 0) {
        do {
            if ((_snd_vmask & (1 << voice)) == 0 &&
                _svm_voice[voice].note == note &&
                _svm_voice[voice].prog == prog &&
                _svm_voice[voice].seq_sep_no == seq_sep_no &&
                _svm_voice[voice].vabId == vabId) {
                if (_svm_voice[voice].vag_idx == 0xFF) {
                    vmNoiseOff(voice);
                } else {
                    _svm_cur.voice = voice;
                    _SsVmKeyOffNow(0);
                }
                n++;
            }
            voice++;
        } while (voice < _SsVmMaxVoice);
    }
    return n;
}

int _SsVmSeKeyOn(short vabId, short prog, unsigned short note, long arg3,
                 unsigned short voll, unsigned short volr) {
    u16 pan;
    u16 vol;

    if (voll == volr) {
        pan = 64;
        vol = voll;
    } else if (volr < voll) {
        vol = voll;
        pan = (volr * 64) / voll;
    } else {
        vol = volr;
        pan = 127 - (voll * 64) / volr;
    }
    return _SsVmKeyOn(0x21, vabId, prog, note, vol, pan);
}

int _SsVmSeKeyOff(short vabId, short prog, unsigned short note, long arg3) {
    _SsVmKeyOff(0x21, vabId, prog, note);
}

void KeyOnCheck(void) {}
#else

s32 _SsVmKeyOn(s16 arg0, s16 vabId, s16 prog, u16 note, u16 voll, u16 volr) {
    struct SeqStruct* score = &_ss_score[arg0 & 0xFF][(arg0 & 0xFF00) >> 8];
    u8 vagIdx[0x80];
    u8 toneIdx[0x80];
    u8 i;
    s32 var_s2;
    s32 tone;
    u8 vagCount;

    var_s2 = 0;
    if (_SsVmVSetUp(vabId, prog)) {
        return -1;
    }
    _svm_cur.seq_sep_no = arg0;
    _svm_cur.note = note;
    _svm_cur.fine = 0;
    if (arg0 == 0x21) {
        _svm_cur.volume = voll;
    } else {
        _svm_cur.volume = (voll * score->vol[score->channel_idx]) / 0x7F;
    }

    _svm_cur.pan = volr;
    _svm_cur.mvol = _svm_pg[prog].mvol;
    _svm_cur.mpan = _svm_pg[prog].mpan;
    _svm_cur.prog_tones = _svm_pg[prog].tones;
    if (_svm_cur.fake_program >= _svm_vh->ps) {
        return -1;
    }
    if (voll == 0) {
        var_s2 = _SsVmKeyOff(arg0, vabId, prog, note);
    } else {
        vagCount = _SsVmSelectToneAndVag(toneIdx, vagIdx);
        for (i = 0; i < vagCount; i++) {
            _svm_cur.tone_vag_idx = vagIdx[i];
            _svm_cur.tone = toneIdx[i];

            tone = _svm_cur.tone +
                   (_svm_cur.fake_program * 0x10);
            _svm_cur.tone_prior = _svm_tn[tone].prior;
            _svm_cur.tone_vol = _svm_tn[tone].vol;
            _svm_cur.tone_pan = _svm_tn[tone].pan;
            _svm_cur.tone_center = _svm_tn[tone].center;
            _svm_cur.tone_shift = _svm_tn[tone].shift;
            _svm_cur.tone_mode = _svm_tn[tone].mode;
#if VERSION < 43
            _svm_cur.tone_min = _svm_tn[tone].min;
            _svm_cur.tone_max = _svm_tn[tone].max;
#endif
            _svm_cur.voice = _SsVmAlloc(0);
            if (_svm_cur.voice < _SsVmMaxVoice) {
                _svm_voice[_svm_cur.voice].unk1b = 1;
                _svm_voice[_svm_cur.voice].unk2 = 0;
                _svm_voice[_svm_cur.voice].seq_sep_no = arg0;
                _svm_voice[_svm_cur.voice].vabId = _svm_cur.vabId;
                _svm_voice[_svm_cur.voice].fake_program =
                    _svm_cur.fake_program;
                _svm_voice[_svm_cur.voice].prog = prog;
                if (arg0 != 0x21) {
                    _svm_voice[_svm_cur.voice].voll1 = voll;
                }
                _svm_voice[_svm_cur.voice].pan = volr;
                _svm_voice[_svm_cur.voice].tone = _svm_cur.tone;
                _svm_voice[_svm_cur.voice].note = note;
                _svm_voice[_svm_cur.voice].priority = _svm_cur.tone_prior;
                _svm_voice[_svm_cur.voice].vag_idx =
                    _svm_cur.tone_vag_idx;
                _SsVmDoAllocate();
                if (_svm_cur.tone_vag_idx == 0xFF) {
                    vmNoiseOn(_svm_cur.voice);
                } else {
                    _SsVmKeyOnNow(vagCount, note2pitch() & 0xFFFF);
                }
                var_s2 |= 1 << _svm_cur.voice;
            } else {
                var_s2 = -1;
            }
        }
    }
    return var_s2;
}

int _SsVmKeyOff(short seq_sep_no, short vabId, short prog, unsigned short note) {
    u8 voice;
    int count;

    count = 0;
    for (voice = 0; voice < _SsVmMaxVoice; voice++) {
        if (_svm_voice[voice].note == note && _svm_voice[voice].prog == prog &&
            _svm_voice[voice].seq_sep_no == seq_sep_no &&
            _svm_voice[voice].vabId == vabId) {
            if (_svm_voice[voice].vag_idx == 0xFF) {
                vmNoiseOff(voice);
            } else {
                _svm_cur.voice = voice;
                _SsVmKeyOffNow(0);
            }
            count++;
        }
    }
    return count;
}

int _SsVmSeKeyOn(short vabId, short prog, unsigned short note, long arg3, unsigned short voll, unsigned short volr) {
    u16 var_v1;
    u16 var_a1;

    if (voll == volr) {
        var_v1 = 0x40;
        var_a1 = voll;
    } else if (volr < voll) {
        var_a1 = voll;
        var_v1 = (volr * 0x40) / voll;
    } else {
        var_a1 = volr;
        var_v1 = 0x7F - ((voll * 0x40) / volr);
    }
    _SsVmKeyOn(0x21, vabId, prog, note, var_a1, var_v1);
}

int _SsVmSeKeyOff(short vabId, short prog, unsigned short note, long arg3) {
    _SsVmKeyOff(0x21, vabId, prog, note);
}

void KeyOnCheck(void) {}
#endif
