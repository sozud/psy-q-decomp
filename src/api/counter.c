#include "../types.h"

typedef struct {
    u16 rootCounter;
    s16 unk2;
    s16 mode;
    s16 : 16;
    s16 target;
    s32 : 32;
} Counter;

// https://psx-spx.consoledev.net/timers/
static volatile s32 *_interrupt_status_register = 0x1F801070;

static volatile Counter* _counters = 0x1F801100;

static volatile s32 _interrupt_status_masks[4] = {
    0x00000010,
    0x00000020,
    0x00000040,
    0x00000001
};

s32 SetRCnt(s32 spec, s16 target, s32 flags) {
    s32 i = spec & 0xFFFF;
    s32 final_mode = 0x48;
    if (i >= 3) {
        return 0;
    }
    _counters[i].mode = 0;
    _counters[i].target = target;

    if (i < 2u) {
        if (flags & 0x10) {
            final_mode = 0x49;
        }
        if (!(flags & 1)) {
            final_mode |= 0x100;
        }
    } else if (i == 2u) { // nb: `else` is redundant here
        if (!(flags & 1)) {
            final_mode = 0x248;
        }
    }
    if ((flags & 0x1000) != 0) {
        final_mode |= 0x10;
    }

    _counters[i].mode = final_mode;
    return 1;
}

long GetRCnt(long spec) {
    s32 i = spec & 0xFFFF;
    if (i >= 3) {
        return 0;
    }
    return _counters[i].rootCounter;
}

s32 StartRCnt(s32 spec) {
    s32 i = spec & 0xFFFF;
    long* mask = _interrupt_status_masks;
    _interrupt_status_register[1] |= mask[i];
    return i < 3;
}

long StopRCnt(long spec) {
    s32 i = spec & 0xFFFF;
    long* mask = _interrupt_status_masks;
    _interrupt_status_register[1] &= ~mask[i];
    return 1;
}

long ResetRCnt(long spec) {
    s32 i = spec & 0xFFFF;
    if (i >= 3) {
        return 0;
    }
    _counters[i].rootCounter = 0;
    return 1;
}