#ifndef LIBSND_INTERNAL_H
#define LIBSND_INTERNAL_H

#include <libspu.h>
#include <libsnd.h>
#include <types.h>

struct Unk {
    u16 unk0;
    u16 unk2;
    u16 unk4;
    u16 unk6;
    u16 unk8;
    s16 unkA;
    u16 unkC;
    u16 unkE;
    u16 unk10;
};

void _SsUtResolveADSR(u16 arg0, u16 arg1, struct Unk* arg2);
void _SsSeqPlay(s16, s16);
void _SsClose(s16);

extern s32 D_80032EF4;
extern u8 D_80032F00;
extern s8 D_80032F01;
extern u8 D_80032F02;
void EnterCriticalSection(void);
void VSyncCallback(void (*func)());
void ExitCriticalSection(void);
void InterruptCallback(s32, s32);
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
extern SpuReverbAttr _svm_rattr;

void SpuVmSeKeyOn(s32, u8, u16, u8, u16, u16);
void SpuVmSetSeqVol(s16, u16, u16, s32);
s32 SpuVmGetSeqVol(s16, s16*, s16*);

void _spu_setInTransfer(s32);
extern s32 D_800987CC[];
extern s32 D_80098810[];
extern u8 svm_vab_used[];

s16 SsVabOpenHeadWithMode(u8* addr, s16 vabid, s32 arg2, u32 sbaddr);

extern u16 _svm_vab_count;

s16 SsVabOpenHead(u8*, s16);
s16 SsVabTransBody(u8*, s16);
extern s32 D_80098854[];

extern u8 spuVmMaxVoice;

extern s16 _svm_stereo_mono;

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
    short field_14_seq_sep_no;
    short field_16_vag_idx;
    short field_18_voice_idx;
    short field_0x1a;
};

extern struct struct_svm svm_cur;

extern u8 spuVmMaxVoice;
void SeAutoVol(s16, s16, s16, s16);
void SeAutoPan(s16, s16, s16, s16);

struct SeqStruct {
    u8 unk0;
    u8 pad1[3];
    u8* read_pos;
    s32 unk8;
    s32 unkc;
    u8 unk10;
    u8 unk11;
    u8 channel;
    u8 unk13;
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 panpot[16];
    u8 unk27;
    u8 unk28;
    u8 unk29;
    u8 unk2a;
    u8 unk2b;
    u8 programs[16];
    u8 unk3C;
    u8 pad3D;
    s16 unk3E;
    s16 unk40;
    s16 unk42;
    s16 unk44;
    s16 unk46;
    s16 unk48;
    u8 pad8[2];
    s16 unk4c;
    s16 vol[16];
    u8 pad10[1];
    s16 unk70;
    s16 unk72;
    u16 unk74;
    s16 unk76;
    s16 unk78;
    s16 unk7A;
    s32 unk7c;
    u32 unk80;
    s32 unk84;
    s32 delta_value;
    s32 unk8c;
    s32 unk90;
    s32 unk94;
    s32 unk98;
    s32 unk9C;
    s32 unkA0;
    s32 unkA4;
    s16 padA6;
    s16 padaa;
};

extern struct SeqStruct* _ss_score[32];

#define SPU_COMMON_MVOLL (0x01 << 0) /* master volume (left) */
#define SPU_COMMON_MVOLR (0x01 << 1) /* master volume (right) */

extern s16 _snd_seq_s_max;
extern s16 _snd_seq_t_max;

extern ProgAtr* D_8006C3B4;
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
    u8 pad[4];
    s16 unk04;
    u8 pad2[21];
    u8 unk1b;
    u8 pad3[24];
};

extern struct SpuVoice _svm_voice[24];
u32 SpuVmVSetUp(s16, s16);

extern VagAtr* _svm_tn;

void SpuVmFlush();
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

#endif