#if VERSION >= 36
#include "libsnd_i.h"

s16 _snd_seq_s_max;
s16 _snd_seq_t_max;
s32 _snd_ev_flag;
_SsFCALL SsFCALL;
SndSsMarkCallbackProc _SsMarkCallback[32][16];
struct SeqStruct* _ss_score[32];
u32 VBLANK_MINUS;
s32 _snd_openflag;

#if VERSION == 36
static int __ps_libinfo__[] = {0x0A107350, 0x113699AB};
#elif VERSION == 37 && defined(LIBSND_37_RELEASE)
static int __ps_libinfo__[] = {0x0C107350, 0x0037C1DA};
#elif VERSION == 37
static int __ps_libinfo__[] = {0x12107350, 0x3037B53B};
#else
static int __ps_libinfo__[] = {0x15107350, 0x0040B19C};
#endif

static s16 voice_on[] = {0, 0, 0x1000, 0x3000, 0x00BF, 0, 0, 0};

static s16 volume_dat[] = {
    0x3FFF, 0x3FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

void _SsInit(void) {
    short* ptr;
    int i, j;

    ptr = (short*)0x1F801C00;
    for (i = 0; i < NUM_VOICES; i++) {
        for (j = 0; j < LEN(voice_on); j++) {
            *ptr++ = voice_on[j];
        }
    }

    ptr = (short*)0x1F801D80;
    for (i = 0; i < LEN(volume_dat); i++) {
        *ptr++ = volume_dat[i];
    }

    _SsVmInit(NUM_VOICES);
    for (j = 0; j < LEN(_SsMarkCallback); j++) {
        for (i = 0; i < LEN(*_SsMarkCallback); i++) {
            _SsMarkCallback[j][i] = NULL;
        }
    }

    VBLANK_MINUS = 60;
    _snd_openflag = 0;
    _snd_ev_flag = 0;
}
#elif VERSION <= 30
#include "libsnd_i.h"

extern long SetRCnt(unsigned long, unsigned short, long);
extern long ResetRCnt(unsigned long);
extern long StartRCnt(unsigned long);
extern long OpenEvent(unsigned long, long, long, long (*func)());
extern long CloseEvent(long);
extern long EnableEvent(long);
extern long DisableEvent(long);
void SpuInitHot(void);

#define _SsMarkCallback MarkCallback

s32 _snd_openflag;
s32 _snd_seq_tick_mode;
s32 _snd_seq_no_tick;
SndSsMarkCallbackProc _SsMarkCallback[32][16];
u32 VBLANK_MINUS;
struct SeqStruct* _ss_score[32];
s16 _snd_seq_s_max;
s16 _snd_seq_t_max;
s32 _snd_ev_flag;

static short voice_on[] = {
    0x0000, 0x0000, 0x1000, 0x3000, 0x00BF, 0x0000, 0x0000, 0x0000};

static short volume_dat[] = {
    0x3FFF, 0x3FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

static long EV;

#if VERSION == 26
void SsInit(void) {
#else
static void _SsInit(s32 hot) {
#endif
    u16* var_a2;
    int i, j;

    ResetCallback();
#if VERSION == 26
    SpuInit();
#else
    if (hot == 0) {
        SpuInit();
    } else {
        SpuInitHot();
    }
#endif

    var_a2 = (u16*)0x1F801C00;
    for (i = 0; i < 24; i++) {
        for (j = 0; j < 8; j++) {
            *var_a2++ = voice_on[j];
        }
    }

    var_a2 = (u16*)0x1F801D80;
    for (i = 0; i < 16; i++) {
        *var_a2++ = volume_dat[i];
    }

    SpuVmInit(24);

    for (j = 0; j < 32; j++) {
        for (i = 15; i >= 0; i--) {
            _SsMarkCallback[j][i] = 0;
        }
    }

    VBLANK_MINUS = 60;
    _snd_openflag = 0;
    _snd_ev_flag = 0;
}

#if VERSION == 30
void SsInit(void) { _SsInit(0); }

void SsInitHot(void) { _SsInit(1); }
#endif

void SsSetTableSize(char* table, short s_max, short t_max) {
    s32 i;
    s32 j;
    struct SeqStruct* pTypedTable;

    _snd_seq_s_max = s_max;
    _snd_seq_t_max = t_max;
    pTypedTable = (struct SeqStruct*)table;
    for (i = 0; i < s_max; i++) {
        _ss_score[i] = &pTypedTable[i * t_max];
    }

    for (i = 0; i < _snd_seq_s_max; i++) {
        for (j = 0; _snd_seq_t_max > j; j++) {
            _ss_score[i][j].unk90 = 0;
            _ss_score[i][j].unk3C = 0xFF;
            _ss_score[i][j].unk3E = 0;
            _ss_score[i][j].unk40 = 0;
            _ss_score[i][j].unk94 = 0;
            _ss_score[i][j].unk98 = 0;
            _ss_score[i][j].unk42 = 0;
            _ss_score[i][j].unkA4 = 0;
            _ss_score[i][j].unkA0 = 0;
            _ss_score[i][j].unk9C = 0;
            _ss_score[i][j].unk44 = 0;
            _ss_score[i][j].unk74 = 0x7F;
            _ss_score[i][j].unk76 = 0x7F;
        }
    }
}

void SsSetTickMode(long tick_mode) {
    if (tick_mode & 0x1000) {
        _snd_seq_no_tick = 1;
        _snd_seq_tick_mode = tick_mode & 0xFFF;
    } else {
        _snd_seq_no_tick = 0;
        _snd_seq_tick_mode = tick_mode;
    }
    if (_snd_seq_tick_mode < 4) {
        switch (_snd_seq_tick_mode) {
        case 1:
            VBLANK_MINUS = 60;
            break;
        case 3:
            VBLANK_MINUS = 120;
            break;
        case 2:
            VBLANK_MINUS = 240;
            break;
        case 0:
        default:
            VBLANK_MINUS = 60;
            break;
        }
    } else {
        VBLANK_MINUS = _snd_seq_tick_mode;
    }
}

static void _wait(void) {
    long i;

    for (i = 0; i < 6000; i++) {
        i += 2;
    }
}

static void _SsStart(s32 use_vsync) {
    long i;
    u32 rcnt_spec;
    u16 rcnt_target;

    for (i = 0; i < 3000; i++) {
        i += 2;
    }
    switch (_snd_seq_tick_mode) {
    case 0:
        return;
    case 1:
        if (use_vsync == 0) {
            VSyncCallback(SsSeqCalledTbyT);
            return;
        }
        EnterCriticalSection();
        ResetRCnt(0xF2000003);
        SetRCnt(0xF2000003, 1, 0x1000);
        _wait();
        _wait();
        StartRCnt(0xF2000003);
        EnableEvent(EV = OpenEvent(0xF2000003, 2, 0x1000, SsSeqCalledTbyT));
        break;
    case 3:
        EnterCriticalSection();
        ResetRCnt(0xF2000002);
        SetRCnt(0xF2000002, 35280, 0x1000);
        _wait();
        _wait();
        StartRCnt(0xF2000002);
        EnableEvent(EV = OpenEvent(0xF2000002, 2, 0x1000, SsSeqCalledTbyT));
        break;
    case 2:
        EnterCriticalSection();
        ResetRCnt(0xF2000002);
        SetRCnt(0xF2000002, 17640, 0x1000);
        _wait();
        _wait();
        StartRCnt(0xF2000002);
        EnableEvent(EV = OpenEvent(0xF2000002, 2, 0x1000, SsSeqCalledTbyT));
        break;
    default:
        if (_snd_seq_no_tick) {
            return;
        }
        if (_snd_seq_tick_mode < 70) {
            rcnt_target = 15750 / _snd_seq_tick_mode;
            rcnt_spec = 0xF2000001;
        } else {
            rcnt_target = 4233600 / _snd_seq_tick_mode;
            rcnt_spec = 0xF2000002;
        }
        EnterCriticalSection();
        ResetRCnt(rcnt_spec);
        SetRCnt(rcnt_spec, rcnt_target, 0x1000);
        _wait();
        _wait();
        StartRCnt(rcnt_spec);
        EnableEvent(EV = OpenEvent(rcnt_spec, 2, 0x1000, SsSeqCalledTbyT));
        break;
    }
    ExitCriticalSection();
}

void SsStart(void) { _SsStart(1); }

void SsStart2(void) { _SsStart(0); }

void SsEnd(void) {
    if (_snd_seq_no_tick == 0 || _snd_seq_tick_mode != 0) {
        EnterCriticalSection();
        DisableEvent(EV);
        CloseEvent(EV);
        ExitCriticalSection();
    }
}

void SsQuit(void) { SpuQuit(); }
#elif VERSION <= 33
#include "snd_defs.h"

#if VERSION == 33
#define _SsMarkCallback _ss_MarkCallback
#endif
extern SndSsMarkCallbackProc _SsMarkCallback[32][16];
extern struct SeqStruct* _ss_score[32];
extern u32 VBLANK_MINUS;
extern s32 _snd_openflag;
extern s16 _snd_seq_s_max;
extern s16 _snd_seq_t_max;
extern s32 _snd_ev_flag;

#define InterruptCallback header_InterruptCallback
#include "libsnd_i.h"
#undef InterruptCallback

void* InterruptCallback(int irq, void (*func)());

extern long SetRCnt(unsigned long, unsigned short, long);
extern long ResetRCnt(unsigned long);
extern long StartRCnt(unsigned long);
void SpuInitHot(void);

SndSsMarkCallbackProc _SsMarkCallback[32][16];
s32 _snd_openflag;
s32 _snd_seq_tick_mode;
s32 _snd_seq_no_tick;
u32 VBLANK_MINUS;
struct SeqStruct* _ss_score[32];
s16 _snd_seq_s_max;
s16 _snd_seq_t_max;
s32 _snd_ev_flag;

static short voice_on[] = {
    0x0000, 0x0000, 0x1000, 0x3000, 0x00BF, 0x0000, 0x0000, 0x0000};

static short volume_dat[] = {
    0x3FFF, 0x3FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

static s32 _snd_1per2_flag = 0;

static s32 _snd_use_vsync_cb;
static s32 _snd_use_event;
static s32 _snd_use_interrupt_id;
static s32 _snd_1per2;
static s32 _snd_video_mode;
static void (*_snd_vsync_cb)();

static void _SsInit(s32 hot) {
    u16* var_a2;
    int i, j;

    ResetCallback();
    if (hot == 0) {
        SpuInit();
    } else {
        SpuInitHot();
    }

    var_a2 = (u16*)0x1F801C00;
    for (i = 0; i < 24; i++) {
        for (j = 0; j < 8; j++) {
            *var_a2++ = voice_on[j];
        }
    }

    var_a2 = (u16*)0x1F801D80;
    for (i = 0; i < 16; i++) {
        *var_a2++ = volume_dat[i];
    }

    SpuVmInit(24);

    for (j = 0; j < 32; j++) {
        for (i = 15; i >= 0; i--) {
            _SsMarkCallback[j][i] = 0;
        }
    }

    VBLANK_MINUS = 60;
    _snd_openflag = 0;
    _snd_use_vsync_cb = 0;
    _snd_use_interrupt_id = -1;
    _snd_use_event = 0;
    _snd_1per2 = 0;
    _snd_vsync_cb = 0;
    _snd_video_mode = GetVideoMode();
    _snd_ev_flag = 0;
}

void SsInit(void) { _SsInit(0); }

void SsInitHot(void) { _SsInit(1); }

void SsSetTableSize(char* table, short s_max, short t_max) {
    s32 i;
    s32 j;
    struct SeqStruct* pTypedTable;

    _snd_seq_s_max = s_max;
    _snd_seq_t_max = t_max;
    pTypedTable = (struct SeqStruct*)table;
    for (i = 0; i < s_max; i++) {
        _ss_score[i] = &pTypedTable[i * t_max];
    }

    for (i = 0; i < _snd_seq_s_max; i++) {
        for (j = 0; _snd_seq_t_max > j; j++) {
            _ss_score[i][j].unk90 = 0;
            _ss_score[i][j].unk3C = 0xFF;
            _ss_score[i][j].unk0 = 0;
            _ss_score[i][j].unk3E = 0;
            _ss_score[i][j].unk40 = 0;
            _ss_score[i][j].unk94 = 0;
            _ss_score[i][j].unk98 = 0;
            _ss_score[i][j].unk42 = 0;
            _ss_score[i][j].unkA4 = 0;
            _ss_score[i][j].unkA0 = 0;
            _ss_score[i][j].unk9C = 0;
            _ss_score[i][j].unk44 = 0;
            _ss_score[i][j].unk74 = 0x7F;
            _ss_score[i][j].unk76 = 0x7F;
            _ss_score[i][j].unk78 = 0x7F;
            _ss_score[i][j].unk7A = 0x7F;
        }
    }
}

void SsSetTickMode(long tick_mode) {
    if (tick_mode & 0x1000) {
        _snd_seq_no_tick = 1;
        _snd_seq_tick_mode = tick_mode & 0xFFF;
    } else {
        _snd_seq_no_tick = 0;
        _snd_seq_tick_mode = tick_mode;
    }
    if (_snd_seq_tick_mode < 6) {
        switch (_snd_seq_tick_mode) {
        case 4:
            VBLANK_MINUS = 50;
            if (_snd_video_mode != 1) {
                _snd_seq_tick_mode = 50;
            } else {
                _snd_seq_tick_mode = 5;
            }
            break;
        case 1:
            VBLANK_MINUS = 60;
            if (_snd_video_mode == 0) {
                _snd_seq_tick_mode = 5;
            } else {
                _snd_seq_tick_mode = 60;
            }
            break;
        case 3:
            VBLANK_MINUS = 120;
            break;
        case 2:
            VBLANK_MINUS = 240;
            break;
        case 5:
            switch (_snd_video_mode) {
            case 1:
                VBLANK_MINUS = 50;
                break;
            default:
            case 0:
                VBLANK_MINUS = 60;
                break;
            }
            break;
        case 0:
            if (_snd_video_mode != 0 && _snd_video_mode == 1) {
                VBLANK_MINUS = 50;
            } else {
                VBLANK_MINUS = 60;
            }
            break;
        default:
            VBLANK_MINUS = 60;
            break;
        }
    } else {
        VBLANK_MINUS = _snd_seq_tick_mode;
    }
}

static void _SsTrapIntrVSync(void);
static void _SsSeqCalledTbyT_1per2(void);

static void _SsStart(s32 use_vsync) {
    s32 wait;
    u16 rcnt_target;
    u32 rcnt_spec;

    wait = 999;
    do {
        wait--;
    } while (wait >= 0);
    switch (_snd_seq_tick_mode) {
    case 0:
        return;
    case 5:
        if (use_vsync == 0) {
            _snd_use_vsync_cb++;
        } else {
            rcnt_spec = 0xF2000003;
            _snd_use_interrupt_id = 0;
            rcnt_target = 1;
        }
        break;
    case 3:
        rcnt_spec = 0xF2000002;
        rcnt_target = 35280;
        _snd_use_interrupt_id = 6;
        break;
    case 2:
        rcnt_spec = 0xF2000002;
        rcnt_target = 17640;
        _snd_use_interrupt_id = 6;
        break;
    default:
        if (_snd_seq_no_tick) {
            return;
        }
        rcnt_spec = 0xF2000002;
        _snd_use_interrupt_id = 6;
        if (_snd_seq_tick_mode < 70) {
            rcnt_target = 2116800 / _snd_seq_tick_mode;
            _snd_1per2++;
        } else {
            rcnt_target = 4233600 / _snd_seq_tick_mode;
        }
        break;
    }
    if (_snd_use_vsync_cb) {
        VSyncCallback(SsSeqCalledTbyT);
        return;
    }
    EnterCriticalSection();
    ResetRCnt(rcnt_spec);
    SetRCnt(rcnt_spec, rcnt_target, 0x1000);
    wait = 1999;
    do {
        wait--;
    } while (wait >= 0);
    wait = 1999;
    do {
        wait--;
    } while (wait >= 0);
    StartRCnt(rcnt_spec);
    if (_snd_use_interrupt_id == 0) {
        _snd_vsync_cb = InterruptCallback(0, 0);
        InterruptCallback(_snd_use_interrupt_id, _SsTrapIntrVSync);
    } else if (_snd_1per2 == 0) {
        InterruptCallback(_snd_use_interrupt_id, SsSeqCalledTbyT);
    } else {
        InterruptCallback(_snd_use_interrupt_id, _SsSeqCalledTbyT_1per2);
    }
    ExitCriticalSection();
}

void SsStart(void) { _SsStart(1); }

void SsStart2(void) { _SsStart(0); }

void SsEnd(void) {
    if (_snd_seq_no_tick == 0) {
        _snd_1per2 = 0;
        EnterCriticalSection();
        if (_snd_use_vsync_cb) {
            VSyncCallback(NULL);
            _snd_use_vsync_cb = 0;
        } else if (_snd_use_interrupt_id != -1) {
            if (_snd_use_interrupt_id == 0) {
                InterruptCallback(0, _snd_vsync_cb);
            } else {
                InterruptCallback(_snd_use_interrupt_id, NULL);
            }
            _snd_use_interrupt_id = -1;
        }
        ExitCriticalSection();
    }
}

void SsQuit(void) { SpuQuit(); }

static void _SsTrapIntrVSync(void) {
    if (_snd_vsync_cb) {
        _snd_vsync_cb();
    }
    SsSeqCalledTbyT();
}

static void _SsSeqCalledTbyT_1per2(void) {
    if (_snd_1per2_flag == 0) {
        _snd_1per2_flag = 1;
    } else {
        _snd_1per2_flag = 0;
        SsSeqCalledTbyT();
    }
}
#else
#include "ssinit.h"

s32 _snd_openflag;
u32 VBLANK_MINUS;
SndSsMarkCallbackProc _SsMarkCallback[32][16];
struct SeqStruct* _ss_score[32];
s16 _snd_seq_s_max;
s16 _snd_seq_t_max;
s32 _snd_ev_flag;

static short voice_on[] = {
    0x0000, 0x0000, 0x1000, 0x3000, 0x00BF, 0x0000, 0x0000, 0x0000};

static short volume_dat[] = {
    0x3FFF, 0x3FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

void _SsInit(void) {
    u16* var_a2;
    int i, j;

    var_a2 = (u16*)0x1F801C00;
    for (i = 0; i < 24; i++) {
        for (j = 0; j < 8; j++) {
            *var_a2++ = voice_on[j];
        }
    }

    var_a2 = (u16*)0x1F801D80;
    for (i = 0; i < 16; i++) {
        *var_a2++ = volume_dat[i];
    }

    SpuVmInit(24);

    for (j = 0; j < 32; j++) {
        for (i = 0; i < 16; i++) {
            _SsMarkCallback[j][i] = 0;
        }
    }

    VBLANK_MINUS = 0x3C;
    _snd_openflag = 0;
    _snd_ev_flag = 0;
}
#endif
