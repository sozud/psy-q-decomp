#include "../types.h"
#include <LIBETC.H>

static long _etc_vmode = 0;

long SetVideoMode(long mode) {
    long prev;

    prev = _etc_vmode;
    _etc_vmode = mode;
    return prev;
}

long GetVideoMode(void) { return _etc_vmode; }
