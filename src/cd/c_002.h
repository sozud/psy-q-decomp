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

#if VERSION >= 40 && !defined(CDROM_DEFINE_ALL)
#define ST_EXTERN extern
#else
#define ST_EXTERN
#endif

ST_EXTERN s32 StRingAddr; /* 11 */
ST_EXTERN s32 StRingBase; /* 12 */
ST_EXTERN s32 StRgb24; /* 13 */
ST_EXTERN s32 StEmu_Addr; /* 14 */
ST_EXTERN s32 StEmu_Idx; /* 15 */
ST_EXTERN s32 StRingSize; /* 16 */
ST_EXTERN s32 StRingIdx1; /* 17 */
ST_EXTERN s32 StRingIdx2; /* 18 */
ST_EXTERN s32 StRingIdx3; /* 19 */
ST_EXTERN s32 StSTART_FLAG; /* 20 */
ST_EXTERN s32 StStartFrame; /* 21 */
ST_EXTERN s32 StEndFrame; /* 22 */
ST_EXTERN s32 StFinalSector; /* 23 */
#if VERSION >= 41 && !defined(CDROM_DEFINE_FUNC)
extern s32 StFunc1;
extern s32 StFunc2;
#elif !(VERSION >= 40 && defined(CDROM_DEFINE_ALL))
s32 StFunc1;
s32 StFunc2;
#endif
ST_EXTERN s32 Stframe_no; /* 26 */
ST_EXTERN s16 Stsector_offset; /* 27 */
ST_EXTERN s32 StCdIntrFlag; /* 28 */
ST_EXTERN s32 StCHANNEL; /* 29 */
ST_EXTERN s32 CChannel; /* 30 */
#if VERSION == 40 && defined(CDROM_DEFINE_ALL)
s32 StFunc1;
s32 StFunc2;
#endif
