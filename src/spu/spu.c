#if VERSION <= 30
#include "common.h"
#define _spu_writeByIO header_spu_writeByIO
#define _spu_write header_spu_write
#define _spu_read header_spu_read
#define _spu_init header_spu_init
#define _spu_ioctl header_spu_ioctl
#define SPU_OLD_LOCAL_RXX
#define SPU_OLD_DEFER_RXX
#include "spu_old.h"
#undef _spu_writeByIO
#undef _spu_write
#undef _spu_read
#undef _spu_init
#undef _spu_ioctl

extern int printf(char *fmt, ...);
extern void DeliverEvent(unsigned long ev, unsigned long spec);
extern void SpuDataCallback(void (*func)());
extern void SpuCallback(void (*func)());
extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);

typedef struct {
    unsigned long voice;
    unsigned long mask;
    unsigned short vol_l;
    unsigned short vol_r;
    unsigned short pad0[2];
    unsigned short pitch;
    unsigned short pad1;
    unsigned long addr;
    unsigned short loop_addr;
    unsigned short pad2;
    unsigned long adsr;
} SpuVoiceAttrInit;

typedef struct {
    unsigned long mask;
    unsigned short param[32];
} SpuIoReverbAttr;

typedef struct {
    unsigned long mask;
    short mvol_left;
    short mvol_right;
    short rvol_left;
    short rvol_right;
    short cd_left;
    short cd_right;
    long cd_reverb;
    long cd_mix;
    short ex_left;
    short ex_right;
    long ex_reverb;
    long ex_mix;
    short noise_clock;
    short reverb;
    short trans_mode;
    short trans_bit0;
    unsigned long endx;
    short stat;
} SpuIoCommonAttr;

static union SpuUnion *_spu_RXX = (union SpuUnion *)0x1F801C00;
static volatile long *_spu_madr = (volatile long *)0x1F8010C0;
static volatile long *_spu_bcr = (volatile long *)0x1F8010C4;
static volatile long *_spu_chcr = (volatile long *)0x1F8010C8;
static volatile long *_spu_sys_pcr = (volatile long *)0x1F8010F0;
static volatile long *_spu_dicr = (volatile long *)0x1F8010F4;
static volatile long *_spu_delay = (volatile long *)0x1F801014;

static long tcount;
static unsigned short spu_tsa;
static long spu_transMode;
static long spu_addrMode;
static long spu_dma_mode;
static long spu_transfer_startaddr;
static unsigned long spu_transfer_size;
static long spu_transfer_time;

#if VERSION >= 30
s32 _spu_inTransfer;
void (* volatile _spu_transferCallback)();
#endif

static void _spu_writeByIO(unsigned char *addr, unsigned long size);
static void _spu_setVoiceAttr(SpuVoiceAttrInit *attr);
static void _spu_interruptDMA(void);
static long _spu_setReverbAttr(SpuIoReverbAttr *attr);
static long _spu_setCommonAttr(SpuIoCommonAttr *attr);
static long _spu_getCommonAttr(SpuIoCommonAttr *attr);
void _spu_w(unsigned char *addr, unsigned long size);
void _spu_r(unsigned char *addr, unsigned long size);
#if VERSION >= 30
void _spu_r_(long addr, unsigned short tsa, long blocks);
#endif

#include "old/spu_io.c"
#include "old/spu_sb.c"

long SysSpu_setRegister(long reg, unsigned short value) {
    _spu_RXX->raw[reg] = value;
    return 0;
}

unsigned short SysSpu_getRegister(long reg) {
    return _spu_RXX->raw[reg];
}
#elif VERSION >= 36
#if VERSION == 36 || VERSION == 37
#define _spu_Fw1ts header_spu_Fw1ts
#define _spu_FwriteByIO _spu_writeByIO
#define _spu_Fr_ _spu_r_
#define _spu_Fw _spu_write
#define _spu_Fr _spu_read
#include "libspu_40.h"
#undef _spu_Fw1ts
#define _spu_Fw1ts _spu_FwaitFs
#define FW1TS_LINKAGE static
#else
#include "libspu_i.h"
#define FW1TS_LINKAGE
#endif
extern int printf(char *fmt, ...);
extern void DeliverEvent(unsigned long, unsigned long);

s32 _spu_init(s32 arg0);
static void _spu_FwriteByIO(void* _arg0, u32 arg1);
void _spu_FiDMA(void);
void _spu_Fr_(s32 arg0, u16 arg1, s32 arg2);
s32 _spu_t(s32 arg0, ...);
u32 _spu_Fw(void* arg0, u32 arg1);
u32 _spu_Fr(u32 arg0, u32 arg1);
void _spu_FsetRXX(u32 offset, u32 value, u32 mode);
u32 _spu_FsetRXXa(s32 arg0, u32 arg1);
s32 _spu_FgetRXXa(s32 arg0, s32 arg1);
void _spu_FsetPCR(s32 arg0);
static void _spu_FsetDelayW(void);
static void _spu_FsetDelayR(void);
FW1TS_LINKAGE void _spu_Fw1ts(void);

union SpuUnion* _spu_RXX = (union SpuUnion*)0x1F801C00;
static volatile int* _spu_madr = (vs32*)0x1F8010C0;
static volatile int* _spu_bcr = (vs32*)0x1F8010C4;
static volatile int* _spu_chcr = (vs32*)0x1F8010C8;
static volatile int* _spu_sys_pcr = (vs32*)0x1F8010F0;
static volatile int* _spu_delay = (vs32*)0x1F801014;
u16 _spu_tsa = 0;
s32 _spu_transMode = 0;
s32 _spu_addrMode = 0;
s32 _spu_mem_mode = 2;
s32 _spu_mem_mode_plus = 3;
s32 _spu_mem_mode_unit = 8;
s32 _spu_mem_mode_unitM = 7;
s32 _spu_inTransfer = 1;
volatile SpuIRQCallbackProc _spu_transferCallback = NULL;
volatile SpuTransferCallbackProc _spu_IRQCallback = NULL;

volatile u16 _spu_RQ[10];


#include "spu_i.c"
#include "spu_sb.c"

void _spu_FsetRXX(u32 offset, u32 value, u32 mode)
{
    if (mode == 0) {
        _spu_RXX->raw[offset] = value;
    }
    else {
        _spu_RXX->raw[offset] = value >> _spu_mem_mode_plus;
    }
}

u32 _spu_FsetRXXa(s32 arg0, u32 arg1) {
#if VERSION >= 41
    u16 temp_a3;
#else
    u32 temp_a3;
#endif

    if ((_spu_mem_mode != 0) && ((arg1 % _spu_mem_mode_unit) != 0)) {
        arg1 += _spu_mem_mode_unit;
        arg1 &= ~_spu_mem_mode_unitM;
    }

    temp_a3 = arg1 >> _spu_mem_mode_plus;
    switch (arg0) {
        case -1:
            return temp_a3 & 0xFFFF;
        case -2:
            return arg1;
        default:
            _spu_RXX->raw[arg0] = temp_a3;
    }

    return arg1;
}

s32 _spu_FgetRXXa(s32 arg0, s32 arg1) {
    u16 temp_a0;

    temp_a0 = _spu_RXX->raw[arg0];
    switch (arg1) {
        case -1:
            return temp_a0;
        default:
            return temp_a0 << _spu_mem_mode_plus;
    }
}

void _spu_FsetPCR(s32 arg0) {

    *_spu_sys_pcr &= ~0x70000;
    if (arg0 != 0) {
        *_spu_sys_pcr |= 0x30000;
    } else {
        *_spu_sys_pcr |= 0x50000;
    }
}

static void _spu_FsetDelayW(void) {
    *_spu_delay = (*_spu_delay & 0xF0FFFFFF) | 0x20000000;
}

static void _spu_FsetDelayR(void) {
    *_spu_delay = (*_spu_delay & 0xF0FFFFFF) | 0x22000000;
}

FW1TS_LINKAGE void _spu_Fw1ts(void) {
    volatile s32 counter;
    volatile s32 value;

    value = 13;
    counter = 0;

#if VERSION == 36 || VERSION == 37
    while (counter < 240) {
        value = value * 3;
        counter++;
    }
#else
    while (counter < 60) {
        value = value * 13;
        counter++;
    }
#endif
}
#else
#include "common.h"
#define _spu_writeByIO header_spu_writeByIO
#define _spu_IRQCallback header_spu_IRQCallback
#include "libspu_i.h"
#undef _spu_writeByIO
#undef _spu_IRQCallback
#include <psxsdk/kernel.h>
#include <psxsdk/stdarg.h>

#define WASTE_TIME()                                                           \
    sp4 = 0xD;                                                                 \
    for (sp0 = 0; sp0 < 0xF0; sp0++) {                                         \
        sp4 *= 3;                                                              \
    }

static s32 D_800334FC = 0;
u16 _spu_tsa = 0;
union SpuUnion* _spu_RXX = (union SpuUnion*)0x1F801C00;
static s32* _spu_madr = (s32*)0x1F8010C0;
static s32* _spu_bcr = (s32*)0x1F8010C4;
static s32* _spu_chcr = (s32*)0x1F8010C8;
static s32* _spu_sys_pcr = (s32*)0x1F8010F0;
static s32* _spu_delay = (s32*)0x1F801014;
long _spu_transMode = 0;
s32 _spu_addrMode = 0;
s32 _spu_mem_mode = 2;
s32 _spu_mem_mode_plus = 3;
s32 _spu_mem_mode_unit = 8;
s32 _spu_mem_mode_unitM = 7;
s32 _spu_inTransfer = 1;
void (* volatile _spu_transferCallback)() = NULL;
void (* volatile _spu_IRQCallback)() = NULL;

static s32 _spu_dma_mode;

#include "spu_i.c"
#include "spu_sb_i.c"
#include "spu_sb.c"

void _spu_FsetRXX(s32 arg0, u32 arg1, s32 arg2) {
    if (arg2 == 0) {
#ifdef VERSION_PC
        write_16(0x1F801C00 + arg0 * 2, arg1, __FILE__, __LINE__);
#else
        _spu_RXX->raw[arg0] = arg1;
#endif
        return;
    }

    _spu_RXX->raw[arg0] = arg1 >> _spu_mem_mode_plus;
}

u32 _spu_FsetRXXa(s32 arg0, u32 arg1) {
    u32 temp_a3;
    u32 var_a1;

    var_a1 = arg1;
    if ((_spu_mem_mode != 0) && ((var_a1 % _spu_mem_mode_unit) != 0)) {
        var_a1 += _spu_mem_mode_unit;
        var_a1 &= ~_spu_mem_mode_unitM;
    }
    temp_a3 = var_a1 >> _spu_mem_mode_plus;

    switch (arg0) {
    case -1:
        return temp_a3 & 0xFFFF;
    case -2:
        return var_a1;
    default:
        _spu_RXX->raw[arg0] = temp_a3;
        return var_a1;
    }
}

u32 _spu_FgetRXXa(s32 arg0, s32 arg1) {
    u16 temp = _spu_RXX->raw[arg0];
    if (arg1 == -1) {
        return temp;
    } else {
        return temp << _spu_mem_mode_plus;
    }
}
#endif
