extern s32 dummy0;
extern s32 dummy1;
extern s32 dummy2;
extern s32 dummy3;
extern s32 dummy4;
extern s32 dummy5;
extern s32 dummy6;
extern s32 dummy7;
extern s32 dummy8;
extern s32 dummy9;
extern s32 dummy10;

int init_ring_status(s32, u32);

#if VERSION == 40 && !defined(CDROM_DEFINE_ALL)
#define ST_EXTERN extern
#else
#define ST_EXTERN
#endif

ST_EXTERN s32 StRingAddr;
ST_EXTERN s32 StRingBase;
ST_EXTERN s32 StRgb24;
ST_EXTERN s32 StEmu_Addr;
ST_EXTERN s32 StEmu_Idx;
ST_EXTERN s32 StRingSize;
ST_EXTERN s32 StRingIdx1;
ST_EXTERN s32 StRingIdx2;
ST_EXTERN s32 StRingIdx3;
ST_EXTERN s32 StSTART_FLAG;
ST_EXTERN s32 StStartFrame;
ST_EXTERN s32 StEndFrame;
ST_EXTERN s32 StFinalSector;
#if !(VERSION == 40 && defined(CDROM_DEFINE_ALL))
s32 StFunc1;
s32 StFunc2;
#endif
ST_EXTERN s32 Stframe_no;
ST_EXTERN s16 Stsector_offset;
ST_EXTERN s32 StCdIntrFlag;
ST_EXTERN s32 StCHANNEL;
ST_EXTERN s32 CChannel;
#if VERSION == 40 && defined(CDROM_DEFINE_ALL)
s32 StFunc1;
s32 StFunc2;
#endif
