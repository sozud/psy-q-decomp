#ifndef _LIBSND_H_
#define _LIBSND_H_

/*****************************************************************
 *
 * $RCSfile: libsnd.h,v $
 *
 * 	Copyright (C) 1994 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/
/*
 * $PSLibId: Runtime Library Version 3.5$
 */

/* #include <sys/types.h> */

/*
 * Macro 
 */
#define SSPLAY_INFINITY  0 
#define SS_NOTICK	 0x1000
#define SS_NOTICK0	 0
#define SS_TICK60	 1
#define SS_TICK240	 2
#define SS_TICK120	 3
#define SS_TICK50	 4
#define SS_TICKVSYNC	 5
#define SS_TICKMODE_MAX  6
#define SSPLAY_PAUSE     0
#define SSPLAY_PLAY      1
#define SS_SOFF          0      
#define SS_SON           1     
#define SS_MIX           0    
#define SS_REV           1   
#define SS_SERIAL_A      0 
#define SS_SERIAL_B      1     
#define SS_CD  SS_SERIAL_A
#define SS_EXT SS_SERIAL_B
#define SS_MUTE_OFF      0     
#define SS_MUTE_ON       1     

#define SS_IMEDIATE 	    0
#define SS_IMMEDIATE        0
#define SS_WAIT_COMPLETED   1

#define SS_REV_TYPE_OFF        0
#define SS_REV_TYPE_ROOM       1
#define SS_REV_TYPE_STUDIO_A   2
#define SS_REV_TYPE_STUDIO_B   3
#define SS_REV_TYPE_STUDIO_C   4
#define SS_REV_TYPE_HALL       5
#define SS_REV_TYPE_SPACE      6
#define SS_REV_TYPE_ECHO       7
#define SS_REV_TYPE_DELAY      8
#define SS_REV_TYPE_PIPE       9

#define SS_SEQ_TABSIZ     172

/*
 * Vag & Vab Structure 
 */ 
typedef struct VabHdr {         /* VAB Bank Headdings */

	long           form;          /* always 'VABp' */
	long           ver;           /* VAB file version number */
	long           id;            /* VAB id */
	unsigned long  fsize;         /* VAB file size */
	unsigned short reserved0;     /* system reserved */
	unsigned short ps;            /* # of the programs in this bank */
	unsigned short ts;            /* # of the tones in this bank */
	unsigned short vs;            /* # of the vags in this bank */
	unsigned char  mvol;          /* master volume for this bank */
	unsigned char  pan;           /* master panning for this bank */
	unsigned char  attr1;         /* bank attributes1 */
	unsigned char  attr2;         /* bank attributes2 */
	unsigned long  reserved1;     /* system reserved */

} VabHdr;			/* 32 byte */


typedef struct ProgAtr {        /* Program Headdings */

	unsigned char tones;          /* # of tones */
	unsigned char mvol;           /* program volume */
	unsigned char prior;          /* program priority */
	unsigned char mode;           /* program mode */
	unsigned char mpan;           /* program pan */
	char          reserved0;      /* system reserved */
	short         attr;           /* program attribute */
	unsigned long reserved1;      /* system reserved */
	unsigned long reserved2;      /* system reserved */

} ProgAtr;			/* 16 byte */


typedef struct VagAtr {         /* VAG Tone Headdings */

	unsigned char  prior;         /* tone priority */
	unsigned char  mode;          /* play mode */
	unsigned char  vol;           /* tone volume*/
	unsigned char  pan;           /* tone panning */
	unsigned char  center;        /* center note */
	unsigned char  shift;         /* center note fine tune */
	unsigned char  min;           /* minimam note limit */
	unsigned char  max;           /* maximam note limit */
	unsigned char  vibW;          /* vibrate depth */
	unsigned char  vibT;          /* vibrate duration */
	unsigned char  porW;          /* portamento depth */
	unsigned char  porT;          /* portamento duration */
	unsigned char  pbmin;         /* under pitch bend max */
	unsigned char  pbmax;         /* upper pitch bend max */
	unsigned char  reserved1;     /* system reserved */
	unsigned char  reserved2;     /* system reserved */
	unsigned short adsr1;         /* adsr1 */
	unsigned short adsr2;         /* adsr2 */
	short          prog;          /* parent program*/
	short          vag;           /* vag reference */
	short          reserved[4];   /* system reserved */

} VagAtr;			/* 32 byte */


/*
 * Volume Structure
 */
typedef struct {
        unsigned short left;    /* L Channel */
        unsigned short right;   /* R Channel */
} SndVolume;


/*
 * CallBack
 */
typedef void (*SsMarkCallbackProc)(short, short, short);


/*  
 * Prototype 
 */
#if defined(_LANGUAGE_C_PLUS_PLUS) || defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif
extern short SsVabOpen (unsigned char*, VabHdr*);
extern void  SsVabClose (short);          
extern short SsVabTransCompleted (short);
extern short SsVabOpenHead (unsigned char*, short);
extern short SsVabTransBody (unsigned char*, short);
extern short SsVabTransBodyPartly (unsigned char*, unsigned long, short);
extern short SsVabOpenHeadSticky (unsigned char*, short, unsigned long);
extern short SsVabTransfer (unsigned char *, unsigned char *, short, short);

extern void  SsInit (void);
extern void  SsInitHot (void);
extern void  SsQuit (void);
extern void  SsSetTableSize (char*, short, short);
extern void  SsSetTickMode (long);
extern int   SsSetTickCallback (void (*cb)());
extern void  SsStart (void);
extern void  SsStart2 (void);
extern void  SsEnd (void);

extern void  SsSeqCalledTbyT (void);           
extern short SsSeqOpen (unsigned long*, short); 
extern void  SsSeqPlay (short, char, short);    
extern void  SsSeqPause (short);                    
extern void  SsSeqReplay (short);                  
extern void  SsSeqStop (short);                   
extern void  SsSeqSetVol (short, short, short);  
extern void  SsSeqSetNext (short, short);             
extern void  SsSeqSetCrescendo (short, short, long); 
extern void  SsSeqSetDecrescendo (short, short, long); 
extern void  SsSeqSetRitardando (short, long, long);   
extern void  SsSeqSetAccelerando (short, long, long);  
extern void  SsSeqClose (short);                       

extern short SsSepOpen (unsigned long*, short, short);
extern void  SsSepPlay (short, short, char, short);
extern void  SsSepPause (short, short);
extern void  SsSepReplay (short, short);
extern void  SsSepStop (short, short);
extern void  SsSepSetVol (short, short, short, short);
extern void  SsSepSetCrescendo (short, short, short, long);
extern void  SsSepSetDecrescendo (short, short, short, long);
extern void  SsSepSetRitardando (short, short, long, long);   
extern void  SsSepSetAccelerando (short, short, long, long); 
extern void  SsSepClose (short);

extern long  SsVoKeyOn (long, long, unsigned short, unsigned short);
extern long  SsVoKeyOff (long, long);                
extern void  SsSetMVol (short, short);              
extern void  SsGetMVol (SndVolume*);             
extern void  SsSetRVol (short, short);             
extern void  SsGetRVol (SndVolume*);              
extern void  SsSetMute (char);                        
extern void  SsSetSerialAttr (char, char, char);     
extern char  SsGetSerialAttr (char, char);           
extern void  SsSetSerialVol (char, short, short);    
extern void  SsGetSerialVol (char, SndVolume*); 
extern void  SsSetNck (short);                         
extern short SsGetNck (void);                          
extern void  SsSetNoiseOn (short, short);    
extern void  SsSetNoiseOff (void);                     
extern void  SsSetMono (void);                         
extern void  SsSetStereo (void);                       

extern void  SsSetTempo (short, short, short);
extern void  SsSetLoop (short, short, short);
extern short SsIsEos (short, short);
extern void  SsPlayBack (short, short, short);
extern void  SsSetMarkCallback (short, short, SsMarkCallbackProc);
extern char  SsSetReservedVoice (char);

extern short SsUtKeyOn (short, short, short, short, short, short, short);
extern short SsUtKeyOff (short, short, short, short, short);

extern short SsUtKeyOnV (short voice, short vabId, short prog, short tone, short note, short fine, short voll, short volr);
extern short SsUtKeyOffV (short voice);

extern short SsUtPitchBend (short, short, short, short, short);
extern short SsUtChangePitch (short, short, short, short, short,
                              short, short);
extern short SsUtChangeADSR (short, short, short, short, 
                             unsigned short, unsigned short);
extern short SsUtSetVabHdr (short, VabHdr*);
extern short SsUtGetVabHdr (short, VabHdr*);
extern short SsUtSetProgAtr (short, short, ProgAtr*);
extern short SsUtGetProgAtr (short, short, ProgAtr*);
extern short SsUtSetVagAtr (short, short, short, VagAtr*);
extern short SsUtGetVagAtr (short, short, short, VagAtr*);
extern short SsUtSetDetVVol (short, short, short);
extern short SsUtGetDetVVol (short, short*, short*);
extern short SsUtSetVVol (short, short, short);
extern short SsUtGetVVol (short, short*, short*);
extern short SsUtAutoVol (short, short, short, short);
extern short SsUtAutoPan (short, short, short, short);
extern void  SsUtReverbOn (void);
extern void  SsUtReverbOff (void);
extern short SsUtSetReverbType (short);
extern short SsUtGetReverbType (void);
extern void  SsUtSetReverbDepth (short, short);
extern void  SsUtSetReverbFeedback (short);
extern void  SsUtSetReverbDelay (short);
extern void  SsUtAllKeyOff (short);

extern void  SsSetAutoKeyOffMode( short mode );
extern void  SsUtFlush( void );
extern void  SsUtVibrateOn( short vc, short vibW, short vibT );
extern void  SsUtVibrateOff( short vc );

extern short SsVabFakeHead (unsigned char*, short, unsigned long);
extern short SsVabFakeBody (short);
extern unsigned long SsUtGetVBaddrInSB (short);
extern long SsUtGetVagAddr( short vabId, short vagId );
extern unsigned long SsUtGetVagAddrFromTone(short vabId, short progId, short toneId);

extern void  SsSetNext (short, short, short, short);             
extern void SsSeqGetVol (short, short, short*, short*);

#if defined(_LANGUAGE_C_PLUS_PLUS) || defined(__cplusplus) || defined(c_plusplus)
}
#endif



/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/*
 */
#endif /* _LIBSND_H_ */
/* DON'T ADD STUFF AFTER THIS */
