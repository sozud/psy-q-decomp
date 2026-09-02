typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;
typedef	unsigned short	ushort;
typedef	unsigned int	uint;
typedef	unsigned long	ulong;
typedef	struct	_physadr { int r[1]; } *physadr;
typedef	struct	label_t	{
	int	val[12];
} label_t;
typedef	struct	_quad { long val[2]; } quad;
typedef	long	daddr_t;
typedef	char *	caddr_t;
typedef	long *	qaddr_t;
typedef	u_long	ino_t;
typedef	long	swblk_t;
typedef	unsigned int size_t;
typedef	long	time_t;
typedef	short	dev_t;
typedef	long	off_t;
typedef	u_short	uid_t;
typedef	u_short	gid_t;
#define	NBBY	8
#define NULL 0
typedef signed char 	   byte;
typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
typedef signed long long   s64;

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;

#define USED __attribute__((unused))

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define ABS_ALT(x) ((x) >= 0 ? (x) : -(x))

#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#define va_start(AP, LASTARG) 						\
 (AP = ((char *)&(LASTARG) + __va_rounded_size(LASTARG)))

#define va_end(AP) AP = (char *)NULL

#define va_arg(AP, TYPE)						\
 (AP = ((char *) (AP)) += __va_rounded_size (TYPE),			\
  *((TYPE *) ((char *) (AP) - __va_rounded_size (TYPE))))

typedef char *va_list;

#define ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))

#ifndef _LIBSPU_H_
#define _LIBSPU_H_






#define SPU_SUCCESS	0
#define SPU_INVALID_ARGS (-3)
#define SPU_DIAG	(-2)
#define SPU_CHECK	(-1)
#define SPU_OFF		0
#define SPU_ON		1
#define SPU_CLEAR	2
#define SPU_RESET	3
#define SPU_DONT_CARE	4
#define SPU_ALL		0
#define SPU_CDONLY	5
#define SPU_VOICEONLY	6
#define SPU_CONT	7
#define SPU_NULL	0

#define SpuDiag		SPU_DIAG
#define SpuCheck	SPU_CHECK
#define SpuOff		SPU_OFF
#define SpuOn		SPU_ON
#define SpuClear	SPU_CLEAR
#define SpuReset	SPU_RESET
#define SpuDontCare	SPU_DONT_CARE
#define SpuALL		SPU_ALL
#define SpuCDOnly	SPU_CDONLY
#define SpuVoiceOnly	SPU_VOICEONLY
#define SpuCont		SPU_CONT
#define SpuNull		SPU_NULL

#define SPU_OFF_ENV_ON	2
#define SPU_ON_ENV_OFF	3

#define SpuOffEnvOn	SPU_OFF_ENV_ON
#define SpuOnEnvOff	SPU_ON_ENV_OFF

#define SPU_ERROR	(-1)

#define SpuError	SPU_ERROR

#define SPU_TRANSFER_BY_DMA 0L
#define SPU_TRANSFER_BY_IO 1L

#define SpuTransferByDMA SPU_TRANSFER_BY_DMA
#define SpuTransferByIO  SPU_TRANSFER_BY_IO
#define SpuTransByDMA SpuTransferByDMA
#define SpuTransByIO SpuTransferByIO

#define SPU_TRANSFER_WAIT 1
#define SPU_TRANSFER_PEEK 0
#define SPU_TRANSFER_GLANCE SPU_TRANSFER_PEEK



#ifndef __SPU_VOICE
#define __SPU_VOICE

#define SPU_00CH (0x1L<< 0)
#define SPU_01CH (0x1L<< 1)
#define SPU_02CH (0x1L<< 2)
#define SPU_03CH (0x1L<< 3)
#define SPU_04CH (0x1L<< 4)
#define SPU_05CH (0x1L<< 5)
#define SPU_06CH (0x1L<< 6)
#define SPU_07CH (0x1L<< 7)
#define SPU_08CH (0x1L<< 8)
#define SPU_09CH (0x1L<< 9)
#define SPU_10CH (0x1L<<10)
#define SPU_11CH (0x1L<<11)
#define SPU_12CH (0x1L<<12)
#define SPU_13CH (0x1L<<13)
#define SPU_14CH (0x1L<<14)
#define SPU_15CH (0x1L<<15)
#define SPU_16CH (0x1L<<16)
#define SPU_17CH (0x1L<<17)
#define SPU_18CH (0x1L<<18)
#define SPU_19CH (0x1L<<19)

#define SPU_20CH (0x1L<<20)
#define SPU_21CH (0x1L<<21)
#define SPU_22CH (0x1L<<22)
#define SPU_23CH (0x1L<<23)

#define SPU_0CH SPU_00CH
#define SPU_1CH SPU_01CH
#define SPU_2CH SPU_02CH
#define SPU_3CH SPU_03CH
#define SPU_4CH SPU_04CH
#define SPU_5CH SPU_05CH
#define SPU_6CH SPU_06CH
#define SPU_7CH SPU_07CH
#define SPU_8CH SPU_08CH
#define SPU_9CH SPU_09CH

#define SPU_ALLCH (SPU_00CH | SPU_01CH | SPU_02CH | SPU_03CH | SPU_04CH | \
		   SPU_05CH | SPU_06CH | SPU_07CH | SPU_08CH | SPU_09CH | \
		   SPU_10CH | SPU_11CH | SPU_12CH | SPU_13CH | SPU_14CH | \
		   SPU_15CH | SPU_16CH | SPU_17CH | SPU_18CH | SPU_19CH | \
		   SPU_20CH | SPU_21CH | SPU_22CH | SPU_23CH)

#define SPU_KEYCH(x) (0x1L<<(x))
#define SPU_VOICECH(x) SPU_KEYCH(x)

#endif



#define	SPU_VOICE_VOLL		(0x01 <<  0)
#define	SPU_VOICE_VOLR		(0x01 <<  1)
#define	SPU_VOICE_VOLMODEL	(0x01 <<  2)
#define	SPU_VOICE_VOLMODER	(0x01 <<  3)
#define	SPU_VOICE_PITCH		(0x01 <<  4)
#define	SPU_VOICE_NOTE		(0x01 <<  5)
#define	SPU_VOICE_SAMPLE_NOTE	(0x01 <<  6)
#define	SPU_VOICE_WDSA		(0x01 <<  7)
#define	SPU_VOICE_ADSR_AMODE	(0x01 <<  8)
#define	SPU_VOICE_ADSR_SMODE	(0x01 <<  9)
#define	SPU_VOICE_ADSR_RMODE	(0x01 << 10)
#define	SPU_VOICE_ADSR_AR	(0x01 << 11)
#define	SPU_VOICE_ADSR_DR	(0x01 << 12)
#define	SPU_VOICE_ADSR_SR	(0x01 << 13)
#define	SPU_VOICE_ADSR_RR	(0x01 << 14)
#define	SPU_VOICE_ADSR_SL	(0x01 << 15)
#define	SPU_VOICE_LSAX		(0x01 << 16)
#define	SPU_VOICE_ADSR_ADSR1	(0x01 << 17)
#define	SPU_VOICE_ADSR_ADSR2	(0x01 << 18)

#define	SPU_VOICE_DIRECT	0
#define	SPU_VOICE_LINEARIncN	1
#define	SPU_VOICE_LINEARIncR	2
#define	SPU_VOICE_LINEARDecN	3
#define	SPU_VOICE_LINEARDecR	4
#define	SPU_VOICE_EXPIncN	5
#define	SPU_VOICE_EXPIncR	6
#define	SPU_VOICE_EXPDec	7
#define	SPU_VOICE_EXPDecN	SPU_VOICE_EXPDec
#define	SPU_VOICE_EXPDecR	SPU_VOICE_EXPDec

#define	SPU_DECODED_FIRSTHALF	0
#define	SPU_DECODED_SECONDHALF	1
#define	SPU_DECODE_FIRSTHALF	SPU_DECODED_FIRSTHALF
#define	SPU_DECODE_SECONDHALF	SPU_DECODED_SECONDHALF


#define	SPU_COMMON_MVOLL		(0x01 <<  0)
#define	SPU_COMMON_MVOLR		(0x01 <<  1)
#define	SPU_COMMON_MVOLMODEL		(0x01 <<  2)
#define	SPU_COMMON_MVOLMODER		(0x01 <<  3)
#define	SPU_COMMON_RVOLL		(0x01 <<  4)
#define	SPU_COMMON_RVOLR		(0x01 <<  5)
#define	SPU_COMMON_CDVOLL		(0x01 <<  6)
#define	SPU_COMMON_CDVOLR		(0x01 <<  7)
#define	SPU_COMMON_CDREV		(0x01 <<  8)
#define	SPU_COMMON_CDMIX		(0x01 <<  9)
#define	SPU_COMMON_EXTVOLL		(0x01 << 10)
#define	SPU_COMMON_EXTVOLR		(0x01 << 11)
#define	SPU_COMMON_EXTREV		(0x01 << 12)
#define	SPU_COMMON_EXTMIX		(0x01 << 13)



#define	SPU_REV_MODE		(0x01 <<  0)
#define	SPU_REV_DEPTHL		(0x01 <<  1)
#define	SPU_REV_DEPTHR		(0x01 <<  2)
#define	SPU_REV_DELAYTIME	(0x01 <<  3)
#define	SPU_REV_FEEDBACK	(0x01 <<  4)

#define SPU_REV_MODE_CHECK	(-1)
#define SPU_REV_MODE_OFF	0
#define SPU_REV_MODE_ROOM	1
#define SPU_REV_MODE_STUDIO_A	2
#define SPU_REV_MODE_STUDIO_B	3
#define SPU_REV_MODE_STUDIO_C	4
#define SPU_REV_MODE_HALL	5
#define SPU_REV_MODE_SPACE	6
#define SPU_REV_MODE_ECHO	7
#define SPU_REV_MODE_DELAY	8
#define SPU_REV_MODE_PIPE	9
#define SPU_REV_MODE_MAX	10

#define SPU_REV_MODE_CLEAR_WA	0x100



#define SPU_EVENT_KEY      (0x01 << 0)
#define SPU_EVENT_PITCHLFO (0x01 << 1)
#define SPU_EVENT_NOISE    (0x01 << 2)
#define SPU_EVENT_REVERB   (0x01 << 3)

#define SPU_EVENT_ALL 0



typedef struct {
    s16 left;
    s16 right;
} SpuVolume;

typedef struct {
    unsigned long	voice;
    unsigned long	mask;
    SpuVolume		volume;
    SpuVolume		volmode;
    SpuVolume		volumex;
    unsigned short	pitch;
    unsigned short	note;
    unsigned short	sample_note;
    short		envx;
    unsigned long	addr;
    unsigned long	loop_addr;
    long		a_mode;
    long		s_mode;
    long		r_mode;
    unsigned short	ar;
    unsigned short	dr;
    unsigned short	sr;
    unsigned short	rr;
    unsigned short	sl;
    unsigned short	adsr1;
    unsigned short	adsr2;
} SpuVoiceAttr;

typedef struct {
    unsigned long	mask;

    long		mode;
    SpuVolume		depth;
    long                delay;
    long                feedback;
} SpuReverbAttr;

#define SPU_DECODEDDATA_SIZE 0x200
#define SPU_DECODEDATA_SIZE SPU_DECODEDDATA_SIZE
typedef struct {
    short cd_left  [SPU_DECODEDDATA_SIZE];
    short cd_right [SPU_DECODEDDATA_SIZE];
    short voice1   [SPU_DECODEDDATA_SIZE];
    short voice3   [SPU_DECODEDDATA_SIZE];
} SpuDecodedData;
typedef SpuDecodedData SpuDecodeData;

typedef struct {
    SpuVolume	volume;
    long	reverb;
    long	mix;
} SpuExtAttr;

typedef struct {
    unsigned long	mask;

    SpuVolume		mvol;
    SpuVolume		mvolmode;
    SpuVolume		mvolx;
    SpuExtAttr		cd;
    SpuExtAttr		ext;
} SpuCommonAttr;

#ifndef __SPU_IRQCALLBACK_PROC
#define __SPU_IRQCALLBACK_PROC
typedef void (*SpuIRQCallbackProc)(void);
#endif
#ifndef __SPU_TRANSFERCALLBACK_PROC
#define __SPU_TRANSFERCALLBACK_PROC
typedef void (*SpuTransferCallbackProc)(void);
#endif



#define SPU_MALLOC_RECSIZ 8



typedef struct {
    unsigned long mask;
    unsigned long queueing;
} SpuEnv;

#define SPU_ENV_EVENT_QUEUEING			(0x01 << 0)



#if defined(_LANGUAGE_C_PLUS_PLUS) || defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif
extern void SpuInit (void);
extern void SpuInitHot (void);
extern void SpuStart (void);
extern void SpuQuit (void);
extern long SpuSetMute (long on_off);
extern long SpuGetMute (void);
extern void SpuSetEnv (SpuEnv *env);

extern long SpuSetNoiseClock (long n_clock);
extern long SpuGetNoiseClock (void);
extern unsigned long SpuSetNoiseVoice (long on_off, unsigned long voice_bit);
extern unsigned long SpuGetNoiseVoice (void);

extern long SpuSetReverb (long on_off);
extern long SpuGetReverb (void);
extern long SpuSetReverbModeParam (SpuReverbAttr *attr);
extern void SpuGetReverbModeParam (SpuReverbAttr *attr);
extern long SpuSetReverbDepth (SpuReverbAttr *attr);
extern long SpuReserveReverbWorkArea (long on_off);
extern long SpuIsReverbWorkAreaReserved (long on_off);
extern unsigned long SpuSetReverbVoice (long on_off, unsigned long voice_bit);
extern unsigned long SpuGetReverbVoice (void);
extern long SpuClearReverbWorkArea (long mode);

extern unsigned long SpuWrite (unsigned char *addr, unsigned long size);
extern unsigned long SpuWrite0 (unsigned long size);
extern unsigned long SpuRead (unsigned char *addr, unsigned long size);
extern long SpuSetTransferMode (long mode);
#define SpuSetTransMode(mode) SpuSetTransferMode((mode))
extern long SpuGetTransferMode (void);
#define SpuGetTransMode() SpuGetTransferMode()
extern unsigned long SpuSetTransferStartAddr (unsigned long addr);
#define SpuSetTransStartAddr(addr) SpuSetTransferStartAddr((addr))
extern unsigned long SpuGetTransferStartAddr (void);
#define SpuGetTransStartAddr() SpuGetTransferStartAddr()
extern unsigned long SpuWritePartly (unsigned char *addr, unsigned long size);

extern long SpuIsTransferCompleted (long flag);
extern SpuTransferCallbackProc SpuSetTransferCallback (SpuTransferCallbackProc func);
extern long SpuReadDecodedData (SpuDecodedData *d_data, long flag);
#define SpuReadDecodeData(d_data,flag) SpuReadDecodedData((d_data), (flag))

extern long SpuSetIRQ (long on_off);
extern long SpuGetIRQ (void);
extern unsigned long SpuSetIRQAddr (unsigned long);
extern unsigned long SpuGetIRQAddr (void);
extern SpuIRQCallbackProc SpuSetIRQCallback (SpuIRQCallbackProc);

extern void SpuSetVoiceAttr (SpuVoiceAttr *arg);
extern void SpuGetVoiceAttr (SpuVoiceAttr *arg);
extern void SpuSetKey (long on_off, unsigned long voice_bit);
extern void SpuSetKeyOnWithAttr (SpuVoiceAttr *attr);
extern long SpuGetKeyStatus (unsigned long voice_bit);
extern void SpuGetAllKeysStatus (char *status);
extern unsigned long SpuFlush (unsigned long ev);

extern unsigned long SpuSetPitchLFOVoice (long on_off, unsigned long voice_bit);
extern unsigned long SpuGetPitchLFOVoice (void);

extern void SpuSetCommonAttr (SpuCommonAttr *attr);
extern void SpuGetCommonAttr (SpuCommonAttr *attr);

extern long SpuInitMalloc (long num, char *top);
extern long SpuMalloc (long size);
extern long SpuMallocWithStartAddr (unsigned long addr, long size);
extern void SpuFree (unsigned long addr);

extern long SpuRGetAllKeysStatus (long min_, long max_, char *status);
extern long SpuRSetVoiceAttr (long min_, long max_, SpuVoiceAttr *arg);

extern void SpuNSetVoiceAttr (int vNum, SpuVoiceAttr *arg);
extern void SpuNGetVoiceAttr (int vNum, SpuVoiceAttr *arg);

extern void SpuSetVoiceVolume (int vNum, short volL, short volR);
extern void SpuSetVoiceVolumeAttr (int vNum, short volL, short volR,
				   short volModeL, short volModeR);
extern void SpuSetVoicePitch (int vNum, unsigned short pitch);
extern void SpuSetVoiceNote (int vNum, unsigned short note);
extern void SpuSetVoiceSampleNote (int vNum, unsigned short sampleNote);
extern void SpuSetVoiceStartAddr (int vNum, unsigned long startAddr);
extern void SpuSetVoiceLoopStartAddr (int vNum, unsigned long lsa);
extern void SpuSetVoiceAR (int vNum, unsigned short AR);
extern void SpuSetVoiceDR (int vNum, unsigned short DR);
extern void SpuSetVoiceSR (int vNum, unsigned short SR);
extern void SpuSetVoiceRR (int vNum, unsigned short RR);
extern void SpuSetVoiceSL (int vNum, unsigned short SL);
extern void SpuSetVoiceARAttr (int vNum, unsigned short AR, long ARmode);
extern void SpuSetVoiceSRAttr (int vNum, unsigned short SR, long SRmode);
extern void SpuSetVoiceRRAttr (int vNum, unsigned short RR, long RRmode);
extern void SpuSetVoiceADSR (int vNum, unsigned short AR, unsigned short DR,
			     unsigned short SR, unsigned short RR,
			     unsigned short SL);
extern void SpuSetVoiceADSRAttr (int vNum,
				 unsigned short AR, unsigned short DR,
				 unsigned short SR, unsigned short RR,
				 unsigned short SL,
				 long ARmode, long SRmode, long RRmode);

extern void SpuGetVoiceVolume (int vNum, short *volL, short *volR);
extern void SpuGetVoiceVolumeAttr (int vNum, short *volL, short *volR,
				   short *volModeL, short *volModeR);
extern void SpuGetVoiceVolumeX (int vNum, short *volXL, short *volXR);
extern void SpuGetVoicePitch (int vNum, unsigned short *pitch);
extern void SpuGetVoiceNote (int vNum, unsigned short *note);
extern void SpuGetVoiceSampleNote (int vNum, unsigned short *sampleNote);
extern void SpuGetVoiceEnvelope (int vNum, short *envx);
extern void SpuGetVoiceStartAddr (int vNum, unsigned long *startAddr);
extern void SpuGetVoiceLoopStartAddr (int vNum, unsigned long *loopStartAddr);
extern void SpuGetVoiceAR (int vNum, unsigned short *AR);
extern void SpuGetVoiceDR (int vNum, unsigned short *DR);
extern void SpuGetVoiceSR (int vNum, unsigned short *SR);
extern void SpuGetVoiceRR (int vNum, unsigned short *RR);
extern void SpuGetVoiceSL (int vNum, unsigned short *SL);
extern void SpuGetVoiceARAttr (int vNum, unsigned short *AR, long *ARmode);
extern void SpuGetVoiceSRAttr (int vNum, unsigned short *SR, long *SRmode);
extern void SpuGetVoiceRRAttr (int vNum, unsigned short *RR, long *RRmode);
extern void SpuGetVoiceADSR (int vNum,
			     unsigned short *AR, unsigned short *DR,
			     unsigned short *SR, unsigned short *RR,
			     unsigned short *SL);
extern void SpuGetVoiceADSRAttr (int vNum,
				 unsigned short *AR, unsigned short *DR,
				 unsigned short *SR, unsigned short *RR,
				 unsigned short *SL,
				 long *ARmode, long *SRmode, long *RRmode);
extern void SpuGetVoiceEnvelopeAttr (int vNum, long *keyStat, short *envx );

extern void SpuSetCommonMasterVolume (short mvol_left, short mvol_right);
extern void SpuSetCommonMasterVolumeAttr (short mvol_left, short mvol_right,
					  short mvolmode_left,
					  short mvolmode_right);
extern void SpuSetCommonCDMix (long cd_mix);
extern void SpuSetCommonCDVolume (short cd_left, short cd_right);
extern void SpuSetCommonCDReverb (long cd_reverb);

extern void SpuGetCommonMasterVolume (short *mvol_left, short *mvol_right);
extern void SpuGetCommonMasterVolumeX (short *mvolx_left, short *mvolx_right);
extern void SpuGetCommonMasterVolumeAttr (short *mvol_left, short *mvol_right,
					  short *mvolmode_left,
					  short *mvolmode_right);
extern void SpuGetCommonCDMix (long *cd_mix);
extern void SpuGetCommonCDVolume (short *cd_left, short *cd_right);
extern void SpuGetCommonCDReverb (long *cd_reverb);

extern long SpuSetReverbModeType (long mode);
extern void SpuSetReverbModeDepth (short depth_left, short depth_right);
extern void SpuSetReverbModeDelayTime (long delay);
extern void SpuSetReverbModeFeedback (long feedback);
extern void SpuGetReverbModeType (long *mode);
extern void SpuGetReverbModeDepth (short *depth_left, short *depth_right);
extern void SpuGetReverbModeDelayTime (long *delay);
extern void SpuGetReverbModeFeedback (long *feedback);
#if defined(_LANGUAGE_C_PLUS_PLUS) || defined(__cplusplus) || defined(c_plusplus)
}
#endif



#define SPU_ST_NOT_AVAILABLE 0
#define SPU_ST_ACCEPT  1

#define SPU_ST_ERROR (-1)
#define SPU_ST_INVALID_ARGUMENT (-2)
#define SPU_ST_WRONG_STATUS (-3)

#define SPU_ST_STOP     2
#define SPU_ST_IDLE     3
#define SPU_ST_PREPARE  4
#define SPU_ST_START    5
#define SPU_ST_PLAY     6
#define SPU_ST_TRANSFER 7
#define SPU_ST_FINAL    8



#define SPU_ST_VAG_HEADER_SIZE 0x30

typedef struct {
    char status;
    char pad1;
    char pad2;
    char pad3;
    long last_size;
    unsigned long buf_addr;
    unsigned long data_addr;
} SpuStVoiceAttr;

typedef struct {
    long size;
    long low_priority;
    SpuStVoiceAttr voice [24];
} SpuStEnv;

#ifndef __SPU_ST_TRANSFERCALLBACK_PROC
#define __SPU_ST_TRANSFERCALLBACK_PROC
typedef void (*SpuStCallbackProc)(unsigned long, long);
#endif

#if defined(_LANGUAGE_C_PLUS_PLUS) || defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif
extern SpuStEnv *SpuStInit (long);
extern long SpuStQuit (void);
extern long SpuStGetStatus (void);
extern unsigned long SpuStGetVoiceStatus (void);
extern long SpuStTransfer (long flag, unsigned long voice_bit);
extern SpuStCallbackProc SpuStSetPreparationFinishedCallback (SpuStCallbackProc func);
extern SpuStCallbackProc SpuStSetTransferFinishedCallback (SpuStCallbackProc func);
extern SpuStCallbackProc SpuStSetStreamFinishedCallback (SpuStCallbackProc func);
#if defined(_LANGUAGE_C_PLUS_PLUS) || defined(__cplusplus) || defined(c_plusplus)
}
#endif


#endif


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
    SPU_RXX _rxx;
    SPU_RXX rxxnv;
    volatile SPU_RXX rxx;
    u16 _raw[0x100];
    volatile u16 raw[0x100];
};


typedef struct {

    u16 m_dAPF1;
    u16 m_dAPF2;


    s16 m_vIIR;
    s16 m_vCOMB1;
    s16 m_vCOMB2;
    s16 m_vCOMB3;
    s16 m_vCOMB4;
    s16 m_vWALL;
    s16 m_vAPF1;
    s16 m_vAPF2;


    u16 m_mLSAME;
    u16 m_mRSAME;


    u16 m_mLCOMB1;
    u16 m_mRCOMB1;
    u16 m_mLCOMB2;
    u16 m_mRCOMB2;


    u16 m_dLSAME;
    u16 m_dRSAME;


    u16 m_mLDIFF;
    u16 m_mRDIFF;


    u16 m_mLCOMB3;
    u16 m_mRCOMB3;
    u16 m_mLCOMB4;
    u16 m_mRCOMB4;


    u16 m_dLDIFF;
    u16 m_dRDIFF;


    u16 m_mLAPF1;
    u16 m_mRAPF1;
    u16 m_mLAPF2;
    u16 m_mRAPF2;


    s16 m_vLIN;
    s16 m_vRIN;
} ReverbRegisters;

#define SPU_REV_MASK_dAPF1       (1U <<  0)
#define SPU_REV_MASK_dAPF2       (1U <<  1)
#define SPU_REV_MASK_vIIR        (1U <<  2)
#define SPU_REV_MASK_vCOMB1      (1U <<  3)
#define SPU_REV_MASK_vCOMB2      (1U <<  4)
#define SPU_REV_MASK_vCOMB3      (1U <<  5)
#define SPU_REV_MASK_vCOMB4      (1U <<  6)
#define SPU_REV_MASK_vWALL       (1U <<  7)
#define SPU_REV_MASK_vAPF1       (1U <<  8)
#define SPU_REV_MASK_vAPF2       (1U <<  9)
#define SPU_REV_MASK_mLSAME      (1U << 10)
#define SPU_REV_MASK_mRSAME      (1U << 11)
#define SPU_REV_MASK_mLCOMB1     (1U << 12)
#define SPU_REV_MASK_mRCOMB1     (1U << 13)
#define SPU_REV_MASK_mLCOMB2     (1U << 14)
#define SPU_REV_MASK_mRCOMB2     (1U << 15)
#define SPU_REV_MASK_dLSAME      (1U << 16)
#define SPU_REV_MASK_dRSAME      (1U << 17)
#define SPU_REV_MASK_mLDIFF      (1U << 18)
#define SPU_REV_MASK_mRDIFF      (1U << 19)
#define SPU_REV_MASK_mLCOMB3     (1U << 20)
#define SPU_REV_MASK_mRCOMB3     (1U << 21)
#define SPU_REV_MASK_mLCOMB4     (1U << 22)
#define SPU_REV_MASK_mRCOMB4     (1U << 23)
#define SPU_REV_MASK_dLDIFF      (1U << 24)
#define SPU_REV_MASK_dRDIFF      (1U << 25)
#define SPU_REV_MASK_mLAPF1      (1U << 26)
#define SPU_REV_MASK_mRAPF1      (1U << 27)
#define SPU_REV_MASK_mLAPF2      (1U << 28)
#define SPU_REV_MASK_mRAPF2      (1U << 29)
#define SPU_REV_MASK_vLIN        (1U << 30)
#define SPU_REV_MASK_vRIN        (1U << 31)

typedef struct {
    u32 m_Mask;
    ReverbRegisters m_Regs;
} ReverbPreset;

#define SPU_CONTROL_FLAG_CD_AUDIO_ENABLE    (1u <<  0)
#define SPU_CONTROL_FLAG_EXT_AUDIO_ENABLE   (1u <<  1)
#define SPU_CONTROL_FLAG_CD_AUDIO_REVERB    (1u <<  2)
#define SPU_CONTROL_FLAG_EXT_AUDIO_REVERB   (1u <<  3)
#define SPU_CONTROL_SRAM_TRANSFER_MODE      (1u <<  4)
#define SPU_CONTROL_FLAG_IRQ9_ENABLE        (1u <<  6)
#define SPU_CONTROL_FLAG_MASTER_REVERB      (1u <<  7)
#define SPU_CONTROL_NOISE_FREQUENCY_STEP    (1u <<  8)
#define SPU_CONTROL_NOISE_FREQUENCY_SHIFT   (1u << 10)
#define SPU_CONTROL_FLAG_MUTE_SPU           (1u << 14)
#define SPU_CONTROL_FLAG_SPU_ENABLE         (1u << 15)


extern s32 _spu_EVdma;
extern u32 _spu_keystat;
extern s32 _spu_trans_mode;
extern long _spu_rev_flag;
extern s32 _spu_rev_reserve_wa;
extern s32 _spu_rev_offsetaddr;
extern SpuReverbAttr _spu_rev_attr;
extern s32 _spu_RQvoice;
extern vs32 _spu_RQmask;
extern u16 _spu_voice_centerNote[24];
extern s8 _spu_zerobuf[1024];
extern s32 _spu_env;
extern s32 _spu_isCalled;


extern s32 _spu_AllocBlockNum;
extern s32 _spu_AllocLastNum;
extern s8* _spu_memList;


typedef struct tagSpuMalloc {
    u_long addr;
    u_long size;
} SPU_MALLOC;

#define SPU_MALLOC_RESERVED_FLAG   (1 << 28)
#define SPU_MALLOC_DEAD_ENTRY      (1 << 29)
#define SPU_MALLOC_END_MARKER      (1 << 30)
#define SPU_MALLOC_FREE_ENTRY      (1 << 31)
#define SPU_MALLOC_FLAGS           (SPU_MALLOC_RESERVED_FLAG | SPU_MALLOC_DEAD_ENTRY | SPU_MALLOC_END_MARKER | SPU_MALLOC_FREE_ENTRY)
#define SPU_MALLOC_ADDR_MASK       (~SPU_MALLOC_FLAGS)
#define SPU_MALLOC_DEAD_MARKER     (SPU_MALLOC_DEAD_ENTRY | 0x0FFFFFFF)
#define BLK(_i) (&((SPU_MALLOC *)_spu_memList)[_i])
#define AS_BLK(_blk) (((SPU_MALLOC *)(_blk)))
#define _IS_FREE(_blk) (_TAG(_blk) & SPU_MALLOC_FREE_ENTRY)
#define _IS_END(_blk) (_TAG(_blk) & SPU_MALLOC_END_MARKER)
#define _IS_DEAD(_blk) (_TAG(_blk) & SPU_MALLOC_DEAD_ENTRY)
#define _IS_DEAD_MARK(_blk) (_TAG(_blk) == SPU_MALLOC_DEAD_MARKER)
#define BLK_IS_FREE(_i) (_IS_FREE(BLK(_i)))
#define BLK_IS_END(_i) (_IS_END(BLK(_i)))
#define BLK_IS_DEAD(_i) (_IS_DEAD(BLK(_i)))
#define BLK_IS_DEAD_MARK(_i) (_IS_DEAD_MARK(BLK(_i)))
#define _TAG(_blk) (AS_BLK(_blk)->addr)
#define BLK_TAG(_i) (_TAG(_blk))
#define _SIZE(_blk) (AS_BLK(_blk)->size)
#define BLK_SIZE(_i) (_SIZE(BLK(_i)))
#define _ADDR(_blk) (AS_BLK(_blk)->addr & SPU_MALLOC_ADDR_MASK)
#define BLK_ADDR(_i) (_ADDR(BLK(_i)))
#define _ADDR_END(_blk) (_ADDR(_blk) + _SIZE(_blk))
#define BLK_ADDR_END(_i) (_ADDR_END(BLK(_i)))
#define BLK_SWAP(_dst, _src) {\
    u_long _addr, _size;\
    \
    _addr = (_src)->addr;\
    _size = (_src)->size;\
    \
    (_src)->addr = (_dst)->addr;\
    (_src)->size = (_dst)->size;\
    \
    (_dst)->addr = _addr;\
    (_dst)->size = _size;\
}
#define BLK_SWAP2(_dst, _src, _sz) {\
    u_long _addr, _size;\
    \
    _addr = (_src)->addr;\
    _size = (_src)->size;\
    \
    (_src)->addr = ((_dst)->addr + _sz) | 0x80000000;\
    (_src)->size = (_dst)->size - _sz;\
    \
    (_dst)->addr = _addr;\
    (_dst)->size = _size;\
}


extern s32 _spu_rev_startaddr[0xA];
extern s32 _spu_rev_workareasize[0xA];
extern ReverbPreset _spu_rev_param[0xA];


extern s32 _spu_init(s32 arg0);
extern void _spu_FiDMA(void);
extern void _spu_Fr_(s32 arg0, u16 arg1, s32 arg2);
extern s32 _spu_t(s32 arg0, ...);
extern u32 _spu_Fw(void* arg0, u32 arg1);
extern u32 _spu_Fr(u32 arg0, u32 arg1);
extern void _spu_FsetRXX(u32 offset, u32 value, u32 mode);
extern u32 _spu_FsetRXXa(s32 arg0, u32 arg1);
extern s32 _spu_FgetRXXa(s32 arg0, s32 arg1);
extern void _spu_FsetPCR(s32 arg0);
extern void _spu_Fw1ts(void);

extern union SpuUnion* _spu_RXX;
extern u16 _spu_tsa;
extern s32 _spu_transMode;
extern s32 _spu_addrMode;
extern s32 _spu_mem_mode;
extern s32 _spu_mem_mode_plus;
extern s32 _spu_mem_mode_unit;
extern s32 _spu_mem_mode_unitM;
extern s32 _spu_inTransfer;
extern volatile SpuIRQCallbackProc _spu_transferCallback;
extern volatile SpuTransferCallbackProc _spu_IRQCallback;

extern vu16 _spu_RQ[10];




static inline void _memcpy(void* _dst, void* _src, u32 _size) {
    char *pDst = (char*)_dst;
    char *pSrc = (char*)_src;

    while (_size--) {
        *pDst++ = *pSrc++;
    }
}



#define CALC_VOL(_dst, _val) { \
    u32 x = 0x8000; \
    u32 y = _val; \
    \
    if (y >= 0x4000) { \
        _dst = y - x; \
    } else { \
        _dst = _val; \
    }\
}


extern int printf(char *fmt, ...);

#define NUM_SPU_CHANNELS 24

typedef enum { false, true } bool;

#define SPU_RXX_REV_WA_START_ADDR 0xD1

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
