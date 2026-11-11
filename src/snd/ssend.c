#if VERSION >= 36
#include "libsnd_i.h"

void SsEnd(void) {
    if (_snd_seq_tick_env.manual_tick == 0) {
        _snd_seq_tick_env.unk11 = 0;
        if (_snd_seq_tick_env.alarm_tick == 0x7F) {
            return;
        }
        EnterCriticalSection();
        if (_snd_seq_tick_env.vsync_tick) {
            VSyncCallback(NULL);
            _snd_seq_tick_env.vsync_tick = 0;
        } else if (_snd_seq_tick_env.alarm_tick == 0) {
            InterruptCallback(0, _snd_seq_tick_env.vsync_cb);
            _snd_seq_tick_env.vsync_cb = NULL;
        } else {
            InterruptCallback(6, NULL);
        }
        ExitCriticalSection();
        _snd_seq_tick_env.alarm_tick = 0x7F;
    }
}
#else
#include "libsnd_i.h"

#if VERSION == 35
void SsEnd(void) {
    if (_snd_seq_tick_env.unk4 == 0) {
        _snd_seq_tick_env.unk17 = 0;
        EnterCriticalSection();
        if (_snd_seq_tick_env.unk16 != 0) {
            VSyncCallback(NULL);
            _snd_seq_tick_env.unk16 = 0;
        } else if (_snd_seq_tick_env.unk18 == 0) {
            InterruptCallback(0, _snd_seq_tick_env.unk12);
            _snd_seq_tick_env.unk12 = 0;
        } else {
            InterruptCallback(6, NULL);
        }
        ExitCriticalSection();
        _snd_seq_tick_env.unk18 = 0xFF;
    }
}
#else
void SsEnd(void) {
    if (_snd_seq_tick_env.unk4 == 0) {
        _snd_seq_tick_env.unk17 = 0;
        if (_snd_seq_tick_env.unk18 != 0x7F) {
            EnterCriticalSection();
            if (_snd_seq_tick_env.unk16 != 0) {
                VSyncCallback(NULL);
                _snd_seq_tick_env.unk16 = 0;
            } else if (_snd_seq_tick_env.unk18 == 0) {
                InterruptCallback(0, _snd_seq_tick_env.unk12);
                _snd_seq_tick_env.unk12 = 0;
            } else {
                InterruptCallback(6, NULL);
            }
            ExitCriticalSection();
            _snd_seq_tick_env.unk18 = 0x7F;
        }
    }
}
#endif
#endif
