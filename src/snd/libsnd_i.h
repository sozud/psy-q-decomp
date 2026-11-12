#ifndef LIBSND_INTERNAL_H
#define LIBSND_INTERNAL_H

#if VERSION <= 36
#define _SsVmSetSeqVol SpuVmSetSeqVol
#define _SsVmGetSeqVol SpuVmGetSeqVol
#define _SsVmGetSeqLVol SpuVmGetSeqLVol
#define _SsVmGetSeqRVol SpuVmGetSeqRVol
#define _SsVmSeqKeyOff SpuVmSeqKeyOff
#define _SsVmSetVol SpuVmSetVol
#define _SsVmSetProgVol SpuVmSetProgVol
#define _SsVmGetProgVol SpuVmGetProgVol
#define _SsVmSetProgPan SpuVmSetProgPan
#define _SsVmGetProgPan SpuVmGetProgPan
#define _SsVmDamperOff SpuVmDamperOff
#define _SsVmDamperOn SpuVmDamperOn
#define _SsVmPitchBend SpuVmPitchBend
#define _SsVmPBVoice SpuVmPBVoice
#define _SsVmKeyOn SpuVmKeyOn
#define _SsVmKeyOff SpuVmKeyOff
#define _SsVmSeKeyOn SpuVmSeKeyOn
#define _SsVmSeKeyOff SpuVmSeKeyOff
#define _SsVmKeyOnNow SpuVmKeyOnNow
#define _SsVmKeyOffNow SpuVmKeyOffNow
#define _SsVmFlush SpuVmFlush
#define _SsVmInit SpuVmInit
#define _SsVmNoiseOn SpuVmNoiseOn
#define _SsVmNoiseOff SpuVmNoiseOff
#define _SsVmNoiseOnWithAdsr SpuVmNoiseOnWithAdsr
#define _SsVmVSetUp SpuVmVSetUp
#define _SsVmAlloc SpuVmAlloc
#define _SsVmDoAllocate SpuVmDoAllocate
#define _SsVmSelectToneAndVag SpuVmSelectToneAndVag
#define _SsVmMaxVoice spuVmMaxVoice
#endif

#if VERSION >= 36
#include "libsnd40.h"
#else

#include <LIBSND.H>
#include <LIBSPU.H>
#include "../types.h"
#include "snd_defs.h"

#if VERSION <= 30
typedef struct {
    unsigned short left;
    unsigned short right;
} SndSpuVolume;

typedef struct {
    SndSpuVolume volume;
    long reverb;
    long mix;
} SndSpuExtAttr;

typedef struct {
    unsigned long mask;
    SndSpuVolume mvol;
    SndSpuVolume depth;
    SndSpuExtAttr cd;
    SndSpuExtAttr ext;
    short noise_clock;
    char pad[0x12];
} SndSpuCommonAttr;

#define SND_SPU_SET_COMMON 0x100
#define SND_SPU_GET_COMMON 0x101
extern long _spu_ioctl(long fd, long cmd, ...);
#endif

struct Unk {
    u16 unk0;
    u16 unk2;
    u16 unk4;
    u16 unk6;
    u16 unk8;
    s16 unkA;
    s16 unkC;
    u16 unkE;
    s16 unk10;
};
void _SsUtResolveADSR(u16 arg0, u16 arg1, struct Unk* arg2);
void _SsSeqPlay(s16, s16);

extern s32 D_80032EF4;
extern u8 D_80032F00;
extern s8 D_80032F01;
extern u8 D_80032F02;
void EnterCriticalSection(void);
void VSyncCallback(void (*func)());
void ExitCriticalSection(void);
void* InterruptCallback(u8, void (*)());
void ResetCallback();
void SpuInit();
void _SsInit();
void Snd_SetPlayMode(s16, s16, u8, s16);
void SpuQuit();
void _SsStart(s32);
extern void (*D_80032EF8)();
extern void (*D_80032EFC)();
extern s32 D_80032F04;

extern s32 D_8003C74C;

#if VERSION <= 30
void SpuVmSeKeyOn(s16, u8, s16, u8, u16, u16);
#else
void SpuVmSeKeyOn(s32, u8, u16, u8, u16, u16);
#endif
void SpuVmSetSeqVol(s16, u16, u16, s32);
s32 SpuVmGetSeqVol(s16, s16*, s16*);

void _spu_setInTransfer(s32);
extern s32 D_800987CC[];
extern s32 D_80098810[];
extern u8 svm_vab_used[];

s16 SsVabOpenHead(u8*, s16);
s16 SsVabTransBody(u8*, s16);
extern s32 D_80098854[];

u8 SpuVmAlloc(u8);
s32 vmNoiseOn2(u8, u16, u16, u16, u16);

struct struct_svm {
    char field_0_sep_sep_no_tonecount;
    char field_1_vabId;
    char field_2_note;
    char field_0x3;
    char field_4_voll;
    char field_0x5;
    char field_6_program;
    char field_7_fake_program;
    char field_8_unknown;
    char field_0x9;
    char field_A_mvol;
    char field_B_mpan;
    char field_C_vag_idx;
    char field_D_vol;
    char field_E_pan;
    char field_F_prior;
    char field_10_centre;
    unsigned char field_11_shift;
    char field_12_mode;
    char field_0x13;
    u8 field_14_seq_sep_no;
    u8 pad;
    short field_16_vag_idx;
    short field_18_voice_idx;
    short field_0x1a;
    short field_0x1c;
    short field_0x1e;
};

/* vm/vm_g.c */
extern s32 _svm_brr_start_addr[16]; /* number: 10 */
extern s16 _svm_stereo_mono;        /* number: 11 */
extern SpuReverbAttr _svm_rattr;    /* number: 12 */
extern u8 _svm_vab_used[16];        /* number: 13 */
extern u8 spuVmMaxVoice;            /* number: 14 */
extern u16 _svm_vab_count;          /* number: 15 */
extern s16 kMaxPrograms;            /* number: 16 */
extern struct struct_svm _svm_cur;  /* number: 17 */
extern s16 _svm_damper;             /* number: 18 */
extern s16 _svm_vcf;                /* number: 19 */
extern u8 _svm_auto_kof_mode;       /* number: 20 */
extern VabHdr* _svm_vab_vh[16];     /* number: 21 */
extern ProgAtr* _svm_vab_pg[16];    /* number: 22 */
extern VagAtr* _svm_vab_tn[16];     /* number: 23 */
extern s32 _svm_vab_start[16];      /* number: 24 */
extern s32 _svm_vab_total[16];      /* number: 25 */
extern VabHdr* _svm_vh;             /* number: 26 */
extern ProgAtr* _svm_pg;            /* number: 27 */
extern VagAtr* _svm_tn;             /* number: 28 */
extern void* _svm_vg;               /* number: 29 */

void SeAutoVol(s16, s16, s16, s16);
void SeAutoPan(s16, s16, s16, s16);

extern struct SeqStruct* _ss_score[32];

#define SPU_COMMON_MVOLL (0x01 << 0) /* master volume (left) */
#define SPU_COMMON_MVOLR (0x01 << 1) /* master volume (right) */

extern s16 _snd_seq_s_max;
extern s16 _snd_seq_t_max;

extern u8 svm_vab_used[];

struct RegBufStruct {
    short field_0_vol_left;
    short field_2_vol_right;
    short field_4_pitch;
    unsigned short field_6_vagAddr;
    unsigned short field_8_adsr1;
    unsigned short field_A_adsr2;
    short field_0xc;
    short field_0xe;
};
extern struct RegBufStruct _svm_sreg_buf[24];
extern unsigned char _svm_sreg_dirty[24];

extern ProgAtr* D_8006C3B4;

extern volatile u16* D_80032F10;
extern u8 spuVmMaxVoice;

struct SpuVoice {
    s16 unk0;
    s16 unk2;
    s16 unk04;
    u16 unk6;
#if VERSION >= 35
    s16 unk8;
    s8 unka;
    s8 unkb;
#endif
    s16 note;
    s16 unke;
    s16 unk10;
    s16 prog;
    s16 tone;
    s16 vabId;
    s16 unk18;
    u8 pad4[1];
    u8 unk1b;
#if VERSION >= 33 || !defined(VERSION)
    s16 auto_vol;
    s16 unk1e;
    s16 unk20;
    s16 unk22;
    s16 start_vol;
    s16 end_vol;
    s16 auto_pan;
    s16 unk2a;
    s16 unk2c;
    s16 unk2e;
    s16 start_pan;
    s16 end_pan;
#endif
};

extern struct SpuVoice _svm_voice[24];
u32 SpuVmVSetUp(s16, s16);

extern VagAtr* _svm_tn;

void SpuVmFlush();
void _SsVmFlush();
void _SsSndCrescendo(s16, s16);
void _SsSndDecrescendo(s16, s16);
void _SsSndPause(s16, s16);
void _SsSndPlay(s16, s16);
void _SsSndReplay(s16, s16);
void _SsSndTempo(s16, s16);
extern s32 _snd_ev_flag;
extern s32 _snd_openflag;

short SsUtGetProgAtr(short vabId, short progNum, ProgAtr* progatrptr);
short SsUtGetVagAtr(
    short vabId, short progNum, short toneNum, VagAtr* vagatrptr);
short SsUtSetVagAtr(
    short vabId, short progNum, short toneNum, VagAtr* vagatrptr);

extern s32 D_80032F08;
extern s16 D_80032F0C;

s32 GetVideoMode();
extern u32 VBLANK_MINUS;
struct SndSeqTickEnv {
    s32 unk0;
    s32 unk4;
    void (*unk8)();
    void (*unk12)();
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19;
    u32 unk20;
};
extern struct SndSeqTickEnv _snd_seq_tick_env;
#if VERSION <= 30
s32 SpuVmSeqKeyOff();
#else
s32 SpuVmSeqKeyOff(s32);
#endif

#if VERSION <= 30
void SpuVmSeKeyOff(s16, u8, s16, u8);
#else
void SpuVmSeKeyOff(s32, u8, u16, u8);
#endif

#endif

#endif
