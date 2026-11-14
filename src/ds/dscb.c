// SPDX-License-Identifier: MIT
#include "../types.h"

typedef void (*DslCB)(u8, u8*);

void* DMACallback(int dma, void (*func)());

struct DslCallbacks {
    DslCB sync;
    DslCB ready;
#if VERSION >= 41
    DslCB start;
#endif
};

#if !defined(PIECE) || PIECE == 0
struct DslCallbacks GlobalCallback;
#else
extern struct DslCallbacks GlobalCallback;
#endif

#if !defined(PIECE) || PIECE == 1
DslCB DsSyncCallback(DslCB func) {
    DslCB old = GlobalCallback.sync;
    GlobalCallback.sync = func;
    return old;
}
#endif

#if !defined(PIECE) || PIECE == 2
DslCB DsReadyCallback(DslCB func) {
    DslCB old = GlobalCallback.ready;
    GlobalCallback.ready = func;
    return old;
}
#endif

#if VERSION >= 41
#if !defined(PIECE) || PIECE == 3
DslCB DsStartCallback(DslCB func) {
    DslCB old = GlobalCallback.start;
    GlobalCallback.start = func;
    return old;
}
#endif
#endif

#if !defined(PIECE) || PIECE == 4
void (*DsDataCallback(void (*func)()))() { return DMACallback(3, func); }
#endif
