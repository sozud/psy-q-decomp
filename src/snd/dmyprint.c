#include "libsnd_i.h"


#define DMY(name, params)                                                    \
    void dmy_##name params {                                                 \
        static int called = 0;                                               \
        if (called == 0) {                                                   \
            printf("_" #name "\n");                                          \
            called = 1;                                                      \
        }                                                                    \
    }

#if VERSION >= 40
void dmy_nothing1(void) {}
#endif

DMY(SsNoteOn, (void))
DMY(SsSetProgramChange, (void))
DMY(SsGetMetaEvent, (void))
DMY(SsSetPitchBend, (void))
DMY(SsSetControlChange, (void))
DMY(SsContBankChange, (void))
DMY(SsContDataEntry, (void))
DMY(SsContMainVol, (void))
DMY(SsContPanpot, (void))
DMY(SsContExpression, (void))
DMY(SsContDamper, (void))
DMY(SsContExternal, (void))
DMY(SsContNrpn1, (void))
DMY(SsContNrpn2, (void))
DMY(SsContRpn1, (void))
DMY(SsContRpn2, (void))
DMY(SsContResetAll, (void))
DMY(SsSetNrpnVabAttr0, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr1, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr2, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr3, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr4, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr5, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr6, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr7, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr8, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr9, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr10, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr11, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr12, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr13, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr14, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr15, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr16, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr17, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr18, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
DMY(SsSetNrpnVabAttr19, (short vabId, short prog, short tone, VagAtr vagatr, short fn_idx, unsigned char data))
