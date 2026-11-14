// SPDX-License-Identifier: MIT
#include "../types.h"

void EnterCriticalSection(void);
void ExitCriticalSection(void);
void ChangeClearPAD(long);
void _remove_ChgclrPAD(void);
void _patch_pad(void);
void _send_pad(void);
void PAD_init2(long, long*, long, long);
void InitPAD2(char*, long, char*, long);
void StartPAD2(void);
void StopPAD2(void);
void EnablePAD(void);
void DisablePAD(void);
void DesablePAD(void);
int SysDeqIntRP(int, void*);
int SysEnqIntRP(int, void*);

#if VERSION < 37
long PAD_init(long mode, long* tbl, long a2, long a3) {
    _patch_pad();
    ChangeClearPAD(0);
    PAD_init2(mode, tbl, a2, a3);
}

int InitPAD(char* bufA, long lenA, char* bufB, long lenB) {
    _patch_pad();
    ChangeClearPAD(0);
    InitPAD2(bufA, lenA, bufB, lenB);
}

int StartPAD(void) {
    StartPAD2();
    ChangeClearPAD(0);
}
#else
typedef struct {
    void* next;
    int (*func2)(void);
    int (*func1)(void);
    int dummy;
} INTRQ;

static int SetPatchPad(void);
static int RemovePatchPad(void);

static int init_pad_flag = 0;

void SetInitPadFlag(int flag) { init_pad_flag = flag; }

int ReadInitPadFlag(void) { return init_pad_flag; }

long PAD_init(long mode, long* tbl, long a2, long a3) {
#if VERSION >= 40
    _remove_ChgclrPAD();
#endif
    EnterCriticalSection();
    _patch_pad();
    ExitCriticalSection();
    ChangeClearPAD(0);
    SetPatchPad();
    PAD_init2(mode, tbl, a2, a3);
#if VERSION >= 40 && VERSION < 43
    _send_pad();
#endif
    init_pad_flag = 1;
    return 1;
}

int InitPAD(char* bufA, long lenA, char* bufB, long lenB) {
#if VERSION >= 40
    _remove_ChgclrPAD();
#endif
    EnterCriticalSection();
    _patch_pad();
    ExitCriticalSection();
    ChangeClearPAD(0);
    SetPatchPad();
    InitPAD2(bufA, lenA, bufB, lenB);
#if VERSION >= 40 && VERSION < 43
    _send_pad();
#endif
    init_pad_flag = 1;
    return 1;
}

int StartPAD(void) {
    StartPAD2();
    ChangeClearPAD(0);
    EnablePAD();
#if VERSION >= 41
    return 1;
#endif
}

void StopPAD(void) {
#if VERSION == 37
    DesablePAD();
    StopPAD2();
    RemovePatchPad();
#else
    DisablePAD();
    StopPAD2();
    RemovePatchPad();
    init_pad_flag = 0;
#endif
}

static volatile u16* sio = (u16*)0x1F801040;
static volatile s32* intc = (s32*)0x1F801070;

static INTRQ patch0;

static int _Pad1(void);
static int _IsVSync(void);

static inline void _SetVsync(void) {
    patch0.func2 = _Pad1;
    patch0.func1 = _IsVSync;
    patch0.next = 0;
    patch0.dummy = 0;
}

static int SetPatchPad(void) {
    EnterCriticalSection();
    _SetVsync();
    SysDeqIntRP(1, &patch0);
    SysEnqIntRP(1, &patch0);
    ExitCriticalSection();
    return 1;
}

static int RemovePatchPad(void) {
    EnterCriticalSection();
    SysDeqIntRP(1, &patch0);
    ExitCriticalSection();
    return 1;
}

static int _Pad1(void) {
    volatile int i[4];

    sio[5] = 0;
    i[0] = 10;
    while (--i[0] != -1) {
    }
    return 0;
}

static int _IsVSync(void) {
    if ((intc[1] & 1) == 0 || (intc[0] & 1) == 0) {
        return 0;
    }
    return 1;
}
#endif
