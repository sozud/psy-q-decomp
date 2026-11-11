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
#if VERSION == 40
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
     u8 unk23;
     u8 pad1[3];
     u8* seq_ptr;
     u8* read_pos;
     u8* next_sep_pos;
     u8 unk15;
     u8 running_status;
     u8 channel_idx;
     u8 unk18;
     u8 unk19;
     u8 fn_idx;
     u8 unk1B;
     u8 panpot[16];
     u8 unk1C;
     u8 unk1D;
     u8 unk1E;
     u8 unk1F;
     u8 play_mode;
     u8 programs[16];
     u8 unk22;
     u8 unk47;
     short vol_l;
     short vol_r;
     s16 unk4C;
     s16 unk4E;
     s16 l_count;
     s16 unk21;
     s16 resolution;
     s16 vab_id;
     short vol[16];
     s16 unk52;
     s16 unk54;
     s16 unk56;
     u16 voll;
     u16 volr;
     s16 unk5C;
     s16 unk5E;
     s32 unk84;
     s32 unk88;
     s32 tempo;
     s32 delta_value;
     u32 unk94;
     unsigned int flags;
     unsigned int v_time_l;
     unsigned int v_time_r;
     u32 unkA4;
     u32 unkA8;
     u32 unkAC;
#if VERSION == 36
     s16 unkA8_36;
     s16 channel_mute;
#else
     s16 channel_mute;
     s16 unkAA;
#endif
};
#else

struct SeqStruct {
     u8* seq_ptr;
     u8* read_pos;
     u8* next_sep_pos;
     u8* loop_pos;
     u32 unk10;
     u8 play_mode;
     u8 unk15;
     u8 running_status;
     u8 channel_idx;
     u8 unk18;
     u8 unk19;
     u8 fn_idx;
     u8 unk1B;
     u8 unk1C;
     u8 unk1D;
     u8 unk1E;
     u8 unk1F;
     u8 l_count;
     u8 unk21;
     u8 unk22;
     u8 unk23;
     u8 rhythm_n;
     u8 rhythm_d;
     u8 vab_id;
     u8 panpot[16];
     u8 programs[16];
     u8 unk47;
     short vol_l;
     short vol_r;
     s16 unk4C;
     s16 unk4E;
     s16 resolution;
     s16 unk52;
     s16 unk54;
     s16 unk56;
     u16 voll;
     u16 volr;
     s16 unk5C;
     s16 unk5E;
     short vol[16];
     s16 channel_mute;
     u8 unk82;
     u8 unk83;
     s32 unk84;
     s32 unk88;
     s32 tempo;
     s32 delta_value;
     u32 unk94;
     unsigned int flags;
     int v_time_l;
     int v_time_r;
     u32 unkA4;
     s32 unkA8;
     s32 unkAC;
};
#endif

struct SndSeqTickEnv {
     s32 tick_mode;
     s32 manual_tick;
     void (*tick_cb)(void);
     void (*vsync_cb)();
     u8 vsync_tick;
     u8 unk11;
     u8 alarm_tick;
     u8 unk13;
     int unk14;
};

struct SpuVoice {
    s16 vag_idx;
    s16 unk2;
    s16 unk04;
    u16 key_stat;
    s16 voll1;
    char pan;
    char unkb;
    s16 note;
    s16 seq_sep_no;
    s16 fake_program;
    s16 prog;
    s16 tone;
    s16 vabId;
    s16 priority;
    u8 pad4[1];
    u8 unk1b;
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
    unsigned char tone_shift;
    char tone_min;
    char tone_max;
    u8 tone_mode;
    u8 pad;
    short seq_sep_no;
    short tone_vag_idx;
    short voice;
    short voiceOffset;
    short field_0x1e;
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

extern short _svm_stereo_mono;
extern SpuReverbAttr _svm_rattr;
extern u8 _svm_vab_used[NUM_VAB];
extern char _SsVmMaxVoice;
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

extern s32 _svm_brr_start_addr[];
extern unsigned short _svm_okon1;
extern unsigned short _svm_okon2;
extern unsigned short _svm_okof1;
extern unsigned short _svm_okof2;
extern unsigned short _svm_orev1;
extern unsigned short _svm_orev2;
extern struct SpuVoice _svm_voice[NUM_VOICES];
extern int _svm_envx_hist[16];
#if VERSION != 36
extern u8 spuVmMaxVoice;
#endif
extern void (*_autovol)();
extern void (*_autopan)();
extern SPU_VOICE_REG _svm_sreg_buf[NUM_VOICES];
extern char _svm_sreg_dirty[NUM_VOICES];

void ResetCallback();
void SpuInit();
void SetAutoPan(short voice);
void SetAutoVol(short voice);
void SeAutoPan(short, short, short, short);
void SeAutoVol(short, short, short, short);
void Snd_SetPlayMode(s16, s16, u8, s16);
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
#if VERSION == 40
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
char _SsVmAlloc(short voice);
void vmNoiseOn(char voice);
void vmNoiseOff(char voice);
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
