#ifndef LIBSND40_H
#define LIBSND40_H

#include <LIBSND.H>
#include <LIBSPU.H>
#include "../types.h"

#ifndef NULL
#define NULL 0
#endif
#define LEN(x) ((s32)(sizeof(x) / sizeof(*(x))))

#define NUM_VOICES 24
#define NUM_VAB 16
#define NUM_CC 13
#define NUM_DE 20

#define CC_NUMBER 0
#define CC_BANKCHANGE 1
#define CC_DATAENTRY 2
#define CC_MAINVOL 3
#define CC_PANPOT 4
#if VERSION >= 40
#define CC_EXPRESSION 5
#define CC_DAMPER 6
#define CC_NRPN1 7
#define CC_NRPN2 8
#define CC_RPN1 9
#define CC_RPN2 10
#define CC_EXTERNAL 11
#define CC_RESETALL 12
#else
#define CC_EXPRESSION 5
#define CC_DAMPER 5
#define CC_NRPN1 6
#define CC_NRPN2 7
#define CC_RPN1 8
#define CC_RPN2 9
#define CC_EXTERNAL 10
#define CC_RESETALL 11
#endif

#define DE_PRIORITY 0
#define DE_MODE 1
#define DE_LIMITL 2
#define DE_LIMITH 3
#define DE_ADSR_AR_L 4
#define DE_ADSR_AR_E 5
#define DE_ADSR_DR 6
#define DE_ADSR_SL 7
#define DE_ADSR_SR_L 8
#define DE_ADSR_SR_E 9
#define DE_ADSR_RR_L 10
#define DE_ADSR_RR_E 11
#define DE_ADSR_SR 12
#define DE_VIB_TIME 13
#define DE_PORTA_DEPTH 14
#define DE_REV_TYPE 15
#define DE_REV_DEPTH 16
#define DE_ECHO_FB 17
#define DE_ECHO_DELAY 18
#define DE_DELAY 19

#define SEQ_FLAG_1 1
#define SEQ_FLAG_2 2
#define SEQ_FLAG_4 4
#define SEQ_FLAG_8 8
#define SEQ_FLAG_10 0x10
#define SEQ_FLAG_20 0x20
#define SEQ_FLAG_100 0x100
#define SEQ_FLAG_200 0x200
#define SEQ_FLAG_400 0x400

typedef void (*SndSsMarkCallbackProc)(short seq_no, short sep_no, short data);
typedef void (*sCb)();

typedef struct _SsFCALL {
    sCb noteon;
    sCb programchange;
    sCb pitchbend;
    sCb metaevent;
    sCb control[NUM_CC];
    sCb ccentry[NUM_DE];
} _SsFCALL;

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

typedef struct {
    u16 ar;
    u16 dr;
    u16 sl;
    u16 sr;
    u16 rr;
    s16 a_mode;
    s16 s_mode;
    s16 r_mode;
    s16 s_dir;
} SsAdsr;

#if VERSION < 40
struct SeqStruct {
    /* 0x00 */ u8 unk23;
    /* 0x01 */ u8 pad1[3];
    /* 0x04 */ u8* seq_ptr;
    /* 0x08 */ u8* read_pos;
    /* 0x0C */ u8* next_sep_pos;
    /* 0x10 */ u8 unk15;
    /* 0x11 */ u8 running_status;
    /* 0x12 */ u8 channel_idx;
    /* 0x13 */ u8 unk18;
    /* 0x14 */ u8 unk19;
    /* 0x15 */ u8 fn_idx;
    /* 0x16 */ u8 unk1B;
    /* 0x17 */ u8 panpot[16];
    /* 0x27 */ u8 unk1C;
    /* 0x28 */ u8 unk1D;
    /* 0x29 */ u8 unk1E;
    /* 0x2A */ u8 unk1F;
    /* 0x2B */ u8 play_mode;
    /* 0x2C */ u8 programs[16];
    /* 0x3C */ u8 unk22;
    /* 0x3D */ u8 unk47;
    /* 0x3E */ short vol_l;
    /* 0x40 */ short vol_r;
    /* 0x42 */ s16 unk4C;
    /* 0x44 */ s16 unk4E;
    /* 0x46 */ s16 l_count;
    /* 0x48 */ s16 unk21;
    /* 0x4A */ s16 resolution;
    /* 0x4C */ s16 vab_id;
    /* 0x4E */ short vol[16];
    /* 0x6E */ s16 unk52;
    /* 0x70 */ s16 unk54;
    /* 0x72 */ s16 unk56;
    /* 0x74 */ u16 voll;
    /* 0x76 */ u16 volr;
    /* 0x78 */ s16 unk5C;
    /* 0x7A */ s16 unk5E;
    /* 0x7C */ s32 unk84;
    /* 0x80 */ s32 unk88;
    /* 0x84 */ s32 tempo;
    /* 0x88 */ s32 delta_value;
    /* 0x8C */ u32 unk94;
    /* 0x90 */ unsigned int flags;
    /* 0x94 */ unsigned int v_time_l;
    /* 0x98 */ unsigned int v_time_r;
    /* 0x9C */ u32 unkA4;
    /* 0xA0 */ u32 unkA8;
    /* 0xA4 */ u32 unkAC;
#if VERSION == 36
    /* 0xA8 */ s16 unkA8_36;
    /* 0xAA */ s16 channel_mute;
#else
    /* 0xA8 */ s16 channel_mute;
    /* 0xAA */ s16 unkAA;
#endif
};
#else
struct SeqStruct {
    /* 0x00 */ u8* seq_ptr;
    /* 0x04 */ u8* read_pos;
    /* 0x08 */ u8* next_sep_pos;
    /* 0x0C */ u8* loop_pos;
    /* 0x10 */ u32 unk10;
    /* 0x14 */ u8 play_mode;
    /* 0x15 */ u8 unk15;
    /* 0x16 */ u8 running_status;
    /* 0x17 */ u8 channel_idx;
    /* 0x18 */ u8 unk18;
    /* 0x19 */ u8 unk19;
    /* 0x1A */ u8 fn_idx;
    /* 0x1B */ u8 unk1B;
    /* 0x1C */ u8 unk1C;
    /* 0x1D */ u8 unk1D;
    /* 0x1E */ u8 unk1E;
    /* 0x1F */ u8 unk1F;
    /* 0x20 */ char l_count;
    /* 0x21 */ char unk21;
    /* 0x22 */ char unk22;
    /* 0x23 */ char unk23;
#if VERSION >= 43
    /* 0x24 */ char rhythm_n;
    /* 0x25 */ char rhythm_d;
#else
    /* 0x24 */ u8 rhythm_n;
    /* 0x25 */ u8 rhythm_d;
#endif
    /* 0x26 */ char vab_id;
    /* 0x27 */ u8 panpot[16];
    /* 0x37 */ u8 programs[16];
    /* 0x47 */ u8 unk47;
    /* 0x48 */ short vol_l;
    /* 0x4A */ short vol_r;
    /* 0x4C */ s16 unk4C;
    /* 0x4E */ s16 unk4E;
    /* 0x50 */ s16 resolution;
    /* 0x52 */ s16 unk52;
    /* 0x54 */ s16 unk54;
    /* 0x56 */ s16 unk56;
    /* 0x58 */ u16 voll;
    /* 0x5A */ u16 volr;
    /* 0x5C */ s16 unk5C;
    /* 0x5E */ s16 unk5E;
    /* 0x60 */ short vol[16];
    /* 0x80 */ s16 channel_mute;
    /* 0x82 */ u8 unk82;
    /* 0x83 */ u8 unk83;
    /* 0x84 */ s32 unk84;
    /* 0x88 */ s32 unk88;
    /* 0x8C */ s32 tempo;
    /* 0x90 */ s32 delta_value;
    /* 0x94 */ u32 unk94;
    /* 0x98 */ unsigned int flags;
    /* 0x9C */ int v_time_l;
    /* 0xA0 */ int v_time_r;
    /* 0xA4 */ u32 unkA4;
    /* 0xA8 */ s32 unkA8;
    /* 0xAC */ s32 unkAC;
};
#endif

struct SndSeqTickEnv {
    /* 0x00 */ s32 tick_mode;
    /* 0x04 */ s32 manual_tick;
    /* 0x08 */ void (*tick_cb)(void);
    /* 0x0C */ void (*vsync_cb)();
    /* 0x10 */ char vsync_tick;
    /* 0x11 */ char unk11;
    /* 0x12 */ char alarm_tick;
    /* 0x13 */ u8 unk13;
#if VERSION < 41
    /* 0x14 */ int unk14;
#endif
};

struct SpuVoice {
    s16 vag_idx;
    s16 unk2;
    s16 unk04;
    u16 key_stat;
    s16 voll1;
    char pan;
    char unkb;
#if VERSION >= 41
    s16 unkc;
#endif
    s16 note; /* 0xC */
    s16 seq_sep_no;
    s16 fake_program;
    s16 prog;  /* 0x12 */
    s16 tone;  /* 0x14 */
    s16 vabId; /* 0x16 */
    s16 priority;
    u8 pad4[1];
    char unk1b;
    s16 auto_vol; /* 0x1c */
    s16 unk1e;
    s16 unk20;
    s16 unk22;
    s16 start_vol; /* 0x24 */
    s16 end_vol;   /* 0x26 */
    s16 auto_pan;  /* 0x28 */
    s16 unk2a;
    s16 unk2c;
    s16 unk2e;
    s16 start_pan; /* 0x30 */
    s16 end_pan;   /* 0x32 */
};

struct struct_svm {
    char prog_tones;
    char vabId;
    char note;
    char fine;
    char volume;
    char pan;
    char prog;
    char fake_program;
    char field_8_unknown;
    char field_0x9;
    char mvol;
    char mpan;
    char tone;
    char tone_vol;
    char tone_pan;
    char tone_prior;
    char tone_center;
    char tone_shift;
#if VERSION < 43
    char tone_min;
    char tone_max;
#endif
    u8 tone_mode;
    u8 pad;
    short seq_sep_no;
    short tone_vag_idx;
    short voice;
#if VERSION < 43
    short voiceOffset;
    short field_0x1e;
#endif
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
    u16 rev_regs[32];
} SPU_RXX;

#define SPUR(field) (_svm_sreg->field)
#define SPUW(field, val) _svm_sreg->field = (val)
#define SPURV(n, field) (((SPU_VOICE_REG*)_svm_sreg)[n].field)
#define SPUWV(n, field, val) ((SPU_VOICE_REG*)_svm_sreg)[n].field = (val)

#if VERSION == 36
extern s32 _svm_brr_start_addr[16];
#endif
extern short _svm_stereo_mono;
extern SpuReverbAttr _svm_rattr;
extern u8 _svm_vab_used[NUM_VAB];
extern char _SsVmMaxVoice;
#if VERSION >= 41
extern unsigned long _snd_vmask;
#endif
extern u16 _svm_vab_count;
extern short kMaxPrograms;
extern struct struct_svm _svm_cur;
extern s16 _svm_damper;
extern char _svm_auto_kof_mode;
extern VabHdr* _svm_vab_vh[NUM_VAB];
extern ProgAtr* _svm_vab_pg[NUM_VAB];
extern VagAtr* _svm_vab_tn[NUM_VAB];
extern unsigned _svm_vab_start[NUM_VAB];
extern s32 _svm_vab_total[NUM_VAB];
extern VabHdr* _svm_vh;
extern ProgAtr* _svm_pg;
extern VagAtr* _svm_tn;
extern u_long* _svm_vg;

extern _SsFCALL SsFCALL;
extern SndSsMarkCallbackProc _SsMarkCallback[32][16];
extern struct SeqStruct* _ss_score[32];
extern u32 VBLANK_MINUS;
extern s32 _snd_openflag;
extern s16 _snd_seq_s_max;
extern s16 _snd_seq_t_max;
extern s32 _snd_ev_flag;
extern struct SndSeqTickEnv _snd_seq_tick_env;
extern SPU_RXX* _svm_sreg;
extern SPU_VOICE_REG _svm_sreg_buf[NUM_VOICES];
extern unsigned char _svm_sreg_dirty[NUM_VOICES];
extern unsigned short _svm_okon1;
extern unsigned short _svm_okon2;
extern unsigned short _svm_okof1;
#if VERSION >= 41
extern unsigned short _svm_onos1;
extern unsigned short _svm_onos2;
#endif
extern unsigned short _svm_okof2;
extern unsigned short _svm_orev1;
extern unsigned short _svm_orev2;
extern struct SpuVoice _svm_voice[NUM_VOICES];
extern unsigned int _svm_envx_ptr;
extern int _svm_envx_hist[16];
extern void (*_autovol)();
extern void (*_autopan)();

extern s32 _svm_brr_start_addr[];
#if VERSION != 36
extern u8 spuVmMaxVoice;
#endif

void ResetCallback();
void SpuInit();
void SetAutoPan(short voice);
void SetAutoVol(short voice);
void SeAutoPan(short, short, short, short);
void SeAutoVol(short, short, short, short);
void Snd_SetPlayMode(s16, s16, char, s16);
long SpuIsTransferCompleted(long);
void SpuQuit(void);
void SpuSetCommonAttr(SpuCommonAttr* attr);
void _SsInit(void);
void _SsVmInit(char numVoices);
void _SsVmFlush(void);
void _SsSeqPlay(short seq_access_num, short arg1);
void _SsSndCrescendo(short seq_access_num, short arg1);
void _SsSndDecrescendo(short seq_access_num, short arg1);
void _SsSndPause(short seq_access_num, short arg1);
void _SsSndPlay(short seq_access_num, short arg1);
void _SsSndReplay(short seq_access_num, short arg1);
void _SsSndTempo(short seq_access_num, short arg1);
void _SsSndStop(short seq_access_num, short arg1);
void _SsUtResolveADSR(u16 adsr1, u16 adsr2, SsAdsr* adsr);
void _SsUtBuildADSR(SsAdsr* adsr, u16* adsr1, u16* adsr2);
void _SsVmSeqKeyOff(s16 seq_sep_num);
short _SsVmSetSeqVol(
    short seq_sep_no, unsigned short voll, unsigned short volr, short arg3);
short _SsVmGetSeqVol(short seq_sep_no, short* voll, short* volr);
void _spu_setInTransfer(s32);
void vmNoiseOn2(u8 arg0, u16 arg1, u16 arg2, u16 arg3, u16 arg4);
#if VERSION >= 40
void _SsSndSetVolData(
    short sep_access_num, short seq_num, short vol, int v_time);
#else
void _SsSndSetVolData(
    short sep_access_num, short seq_num, short vol, unsigned long v_time);
#endif
void _SsVmDamperOff(void);
int _SsInitSoundSep(short flag, short i, short vab_id, unsigned long* addr);
short _SsInitSoundSeq(short seq_no, short vab_id, u8* addr);
int _SsReadDeltaValue(short seq_access_num, short seq_num);
unsigned char _SsVmAlloc(short voice);
void vmNoiseOn(unsigned char voice);
void vmNoiseOff(unsigned char voice);
unsigned short note2pitch2(unsigned short note, unsigned short fine);
void _SsVmKeyOnNow(unsigned short vagCount, unsigned short pitch);
int _SsVmVSetUp(short vabId, short prog);
void _SsVmDoAllocate(void);

void _SsNoteOn(short a0, short a1, unsigned char a2, unsigned char a3);
void _SsSetProgramChange(short a0, short a1, unsigned char a2);
void _SsGetMetaEvent(short a0, short a1, unsigned char a2);
void _SsSetPitchBend(short a0, short a1);
void _SsSetControlChange(short a0, short a1, unsigned char a2);
void _SsContBankChange(short a0, short a1);
void _SsContDataEntry(short a0, short a1, unsigned char a2);
void _SsContMainVol(short a0, short a1, unsigned char a2);
void _SsContPanpot(short a0, short a1, unsigned char a2);
void _SsContExpression(short a0, short a1, unsigned char a2);
void _SsContDamper(short a0, short a1, unsigned char a2);
void _SsContExternal(short a0, short a1, unsigned char a2);
void _SsContNrpn1(short a0, short a1, unsigned char a2);
void _SsContNrpn2(short a0, short a1, unsigned char a2);
void _SsContRpn1(short a0, short a1, unsigned char a2);
void _SsContRpn2(short a0, short a1, unsigned char a2);
void _SsContResetAll(short a0, short a1);

void _SsVmNoiseOn(unsigned short voll, unsigned short volr);
void _SsVmNoiseOff(void);
short _SsVmSetVol(short seq_sep_no, short vabId, short prog, unsigned short voll, unsigned short volr);
int _SsVmKeyOn(short seq_sep_no, short vabId, short prog, unsigned short note, unsigned short voll, unsigned short volr);
int _SsVmKeyOff(short seq_sep_no, short vabId, short prog, unsigned short note);
int _SsVmSeKeyOn(short vabId, short prog, unsigned short note, long pitch, unsigned short voll, unsigned short volr);
int _SsVmSeKeyOff(short vabId, short prog, unsigned short note, long pitch);
int _SsVmPitchBend(short seq_sep_no, short vabId, short prog, short pitch);
int _SsVmSetProgVol(short vabId, short prog, unsigned char vol);
int _SsVmGetProgVol(short vabId, short prog);
int _SsVmSetProgPan(short vabId, short prog, unsigned char pan);
int _SsVmGetProgPan(short vabId, short prog);
short _SsVmGetSeqLVol(short seq_sep_no);
short _SsVmGetSeqRVol(short seq_sep_no);
void _SsVmDamperOn(void);
short _SsVmPBVoice(short voice, short seq_sep_no, short vabId, short prog, unsigned short pbend);

void _SsSetNrpnVabAttr0(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr1(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr2(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr3(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr4(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr5(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr6(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr7(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr8(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr9(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr10(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr11(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr12(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr13(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr14(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr15(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr16(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr17(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr18(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);
void _SsSetNrpnVabAttr19(
    short a0, short a1, short a2, VagAtr a3, short a4, unsigned char a5);

#endif
