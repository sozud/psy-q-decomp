#ifndef LIBSPU_INTERNAL_H
#define LIBSPU_INTERNAL_H

#include "../types.h"
#include <LIBSPU.H>

void InterruptCallback(s32, s32);
void _SpuInit(s32);
s32 _spu_read(s32 arg0, s32 arg1);
void _spu_FiDMA();

extern s32 D_80033098;
extern s32 _spu_EVdma;
extern s32 _spu_isCalled;
extern s32 _spu_inTransfer;

extern s32 D_8003355C;
extern s32 D_80033560;
extern s8* D_80033564;
extern s32 _spu_mem_mode_plus;

typedef struct tagSpuMalloc {
    u32 addr;
    u32 size;
} SPU_MALLOC;

extern s32 D_80033550;
extern void (* volatile _spu_transferCallback)();
extern s32 _spu_inTransfer;

void _SpuCallback(s32 arg0);
extern void (*_spu_IRQCallback)();

s32 _SpuSetAnyVoice(s32 on_off, u32 bits, s32 addr1, s32 addr2);

s32 _spu_t(s32, ...);
s32 _spu_writeByIO(s32, s32);
extern u16 _spu_tsa;

u16 _spu_FsetRXXa(s32, s32);
s32 _spu_write(u32, u32);
extern s32 _spu_inTransfer;

void _SpuSetVoiceAttr(SpuVoiceAttr* arg, s32, s32, s32);

struct rev_param_entry {
    u32 flags;
    u16 dAPF1;
    u16 dAPF2;
    u16 vIIR;
    u16 vCOMB1;
    u16 vCOMB2;
    u16 vCOMB3;
    u16 vCOMB4;
    u16 vWALL;
    u16 vAPF1;
    u16 vAPF2;
    u16 mLSAME;
    u16 mRSAME;
    u16 mLCOMB1;
    u16 mRCOMB1;
    u16 mLCOMB2;
    u16 mRCOMB2;
    u16 dLSAME;
    u16 dRSAME;
    u16 mLDIFF;
    u16 mRDIFF;
    u16 mLCOMB3;
    u16 mRCOMB3;
    u16 mLCOMB4;
    u16 mRCOMB4;
    u16 dLDIFF;
    u16 dRDIFF;
    u16 mLAPF1;
    u16 mRAPF1;
    u16 mLAPF2;
    u16 mRAPF2;
    u16 vLIN;
    u16 vRIN;
};

typedef struct tagSpuVoiceRegister {
    SpuVolume volume;
    u16 pitch;
    u16 addr;
    u16 adsr[2];
    u16 volumex;
    u16 loop_addr;
} SPU_VOICE_REG;

typedef struct tagSpuControl {
    SPU_VOICE_REG voice[24];
    SpuVolume main_vol;
    SpuVolume rev_vol;

    u16 key_on[2];
    u16 key_off[2];
    u16 chan_fm[2];
    u16 noise_mode[2];
    u16 rev_mode[2];
    u32 chan_on;
    u16 unk;
    u16 rev_work_addr;
    u16 irq_addr;
    u16 trans_addr;
    u16 trans_fifo;
    u16 spucnt;
    u16 data_trans;
    u16 spustat;
    SpuVolume cd_vol;
    SpuVolume ex_vol;
    SpuVolume main_volx;
    SpuVolume unk_vol;

    u16 dAPF1;
    u16 dAPF2;
    u16 vIIR;
    u16 vCOMB1;
    u16 vCOMB2;
    u16 vCOMB3;
    u16 vCOMB4;
    u16 vWALL;
    u16 vAPF1;
    u16 vAPF2;
    u16 mLSAME;
    u16 mRSAME;
    u16 mLCOMB1;
    u16 mRCOMB1;
    u16 mLCOMB2;
    u16 mRCOMB2;
    u16 dLSAME;
    u16 dRSAME;
    u16 mLDIFF;
    u16 mRDIFF;
    u16 mLCOMB3;
    u16 mRCOMB3;
    u16 mLCOMB4;
    u16 mRCOMB4;
    u16 dLDIFF;
    u16 dRDIFF;
    u16 mLAPF1;
    u16 mRAPF1;
    u16 mLAPF2;
    u16 mRAPF2;
    u16 vLIN;
    u16 vRIN;
} SPU_RXX;

union SpuUnion {
    SPU_RXX rxxnv;
    volatile SPU_RXX rxx;
    volatile u16 raw[0x100];
};

extern union SpuUnion* _spu_RXX;

s32 _SpuIsInAllocateArea_(u32);
extern s32 D_800330A0;
extern s32 D_800330A4;
extern s32 D_800330A8;

extern s32 D_8003309C;

#define SPU_TRANSFER_BY_DMA 0
#define SPU_TRANSFER_BY_IO 1

void WaitEvent(s32);
s32 _SpuIsInAllocateArea_(u32);
extern s32 _spu_rev_startaddr[];
extern s32 _spu_EVdma;
extern s32 _spu_mem_mode_plus;

extern s32 _spu_AllocBlockNum;
extern s32 _spu_AllocLastNum;
extern s8* _spu_memList;
extern s32 _spu_mem_mode_plus;

extern s32 _spu_rev_flag;
extern s32 _spu_rev_reserve_wa;
extern s32 _spu_rev_offsetaddr;
extern long _spu_trans_mode;
extern long _spu_transMode;

extern s8 _spu_zerobuf[1024];

#endif