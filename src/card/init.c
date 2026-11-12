#include "../types.h"

void ChangeClearPAD(long);
void EnterCriticalSection(void);
void ExitCriticalSection(void);
int ReadInitPadFlag(void);
void InitCARD2(long val);
void StartCARD2(void);
void StopCARD2(void);
void _ExitCard(void);
void _patch_card(void);
void _patch_card2(void);

#if VERSION >= 40
static u8 __ps_libinfo__[8] = {0x50, 0x73, 0x03, 0x15, 0x9C, 0x85, 0x40, 0x00};
#elif VERSION == 37
static u8 __ps_libinfo__[8] = {0x50, 0x73, 0x03, 0x0C, 0xDA, 0x99, 0x37, 0x00};
#else
static u8 __ps_libinfo__[8] = {0x50, 0x73, 0x03, 0x0A, 0x1A, 0xD3, 0x36, 0x10};
#endif

void InitCARD(long val) {
    ChangeClearPAD(0);
    EnterCriticalSection();
#if VERSION >= 40
    if (!ReadInitPadFlag()) {
        val = 0;
    }
    InitCARD2(val);
#else
    InitCARD2(val);
    _patch_card();
    _patch_card2();
#endif
    ExitCriticalSection();
}

long StartCARD(void) {
    EnterCriticalSection();
    StartCARD2();
    ChangeClearPAD(0);
    ExitCriticalSection();
}

long StopCARD(void) {
    StopCARD2();
#if VERSION < 40
    _patch_card2();
#endif
    _ExitCard();
}
