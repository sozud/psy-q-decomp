#include "../types.h"
#include "c_002.h"

static s32 HADR0 = 0x1F801800;
static s32 HADR1 = 0x1F801801;
static s32 HADR2 = 0x1F801802;
static s32 HADR3 = 0x1F801803;

void EnterCriticalSection(void);
void ExitCriticalSection(void);
void CdDataCallback(void*);
void CdReadyCallback(void*);

#if VERSION >= 40
extern int DS_active;
void DsDataCallback(void*);
void DsReadyCallback(void*);
#endif

void StUnSetRing(void) {
    EnterCriticalSection();
#if VERSION >= 40
    if (DS_active == 1) {
        DsDataCallback(0);
        DsReadyCallback(0);
    } else {
        CdDataCallback(0);
        CdReadyCallback(0);
    }
#else
    CdDataCallback(0);
    CdReadyCallback(0);
#endif
    *(volatile u8*)HADR0 = 0;
    *(volatile u8*)HADR3 = 0;
    ExitCriticalSection();
}
