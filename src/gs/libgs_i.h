#ifndef LIBGS_I_H
#define LIBGS_I_H

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned long u_long;

typedef struct {
    short x, y;
    short w, h;
} RECT;

typedef struct {
    unsigned addr : 24;
    unsigned len : 8;
    u_char r0, g0, b0, code;
} P_TAG;

#define setlen(p, _len) (((P_TAG*)(p))->len = (u_char)(_len))
#define setcode(p, _code) (((P_TAG*)(p))->code = (u_char)(_code))
#define setRECT(r, _x, _y, _w, _h)                                             \
    (r)->x = (_x), (r)->y = (_y), (r)->w = (_w), (r)->h = (_h)

typedef struct {
    u_long tag;
    u_long code[15];
} DR_ENV;

typedef struct {
    RECT clip;
    short ofs[2];
    RECT tw;
    u_short tpage;
    u_char dtd;
    u_char dfe;
    u_char isbg;
    u_char r0, g0, b0;
    DR_ENV dr_env;
} DRAWENV;

typedef struct {
    RECT disp;
    RECT screen;
    u_char isinter;
    u_char isrgb24;
    u_char pad0;
    u_char pad1;
} DISPENV;

typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    short w, h;
} TILE;

typedef struct {
    short m[3][3];
    long t[3];
} MATRIX;

typedef struct {
    unsigned p : 24;
    unsigned char num : 8;
} GsOT_TAG;

typedef struct {
    unsigned long length;
    GsOT_TAG* org;
    unsigned long offset;
    unsigned long point;
    GsOT_TAG* tag;
} GsOT;

typedef struct {
    short offx, offy;
} _GsPOSITION;

typedef struct {
    u_long pmode;
    short px, py;
    u_short pw, ph;
    u_long* pixel;
    short cx, cy;
    u_short cw, ch;
    u_long* clut;
} GsIMAGE;

typedef unsigned char PACKET;

#define ZRESOLUTION 0x3fff
#define GsDOFF (1 << 31)

typedef struct {
    u_long attribute;
    short x, y;
    u_short w, h;
    u_short tpage;
    u_char u, v;
    short cx, cy;
    u_char r, g, b;
    short mx, my;
    short scalex, scaley;
    long rotate;
} GsSPRITE;

typedef struct {
    u_char u, v;
    u_short cba;
    u_short flag;
    u_short tpage;
} GsCELL;

typedef struct {
    u_char cellw, cellh;
    u_short ncellw, ncellh;
    GsCELL* base;
    u_short* index;
} GsMAP;

typedef struct {
    u_long attribute;
    short x, y;
    short w, h;
    short scrollx, scrolly;
    u_char r, g, b;
    GsMAP* map;
    short mx, my;
    short scalex, scaley;
    long rotate;
} GsBG;

typedef struct {
    u_long attribute;
    short x0, y0;
    short x1, y1;
    u_char r, g, b;
} GsLINE;

typedef struct {
    u_long attribute;
    short x, y;
    u_short w, h;
    u_char r, g, b;
} GsBOXF;

typedef struct {
    u_long tag;
    u_long code[1];
} DR_TPAGE;

typedef struct {
    u_char r0, g0, b0, code;
    short x0, y0;
    short x1, y1;
} LINE_F2_BODY;

typedef struct {
    u_char r0, g0, b0, code;
    short x0, y0;
    short w, h;
} TILE_BODY;

typedef struct {
    u_char r0, g0, b0, code;
    short x0, y0;
    u_char u0, v0;
    u_short clut;
    short w, h;
} SPRT_BODY;

typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    short x1, y1;
    short x2, y2;
    short x3, y3;
} POLY_F4;

typedef struct {
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    u_char u0, v0;
    u_short clut;
    short x1, y1;
    u_char u1, v1;
    u_short tpage;
    short x2, y2;
    u_char u2, v2;
    u_short pad1;
    short x3, y3;
    u_char u3, v3;
    u_short pad2;
} POLY_FT4;

typedef struct {
    short vx, vy, vz, pad;
} SVECTOR;

typedef struct {
    long vx, vy, vz, pad;
} VECTOR;

#define ONE 4096
#define setVector(v, _x, _y, _z) (v)->vx = _x, (v)->vy = _y, (v)->vz = _z

extern MATRIX* RotMatrix(SVECTOR* r, MATRIX* m);
extern MATRIX* ScaleMatrix(MATRIX* m, VECTOR* v);
extern MATRIX* TransMatrix(MATRIX* m, VECTOR* v);
extern void SetRotMatrix(MATRIX* m);
extern void SetTransMatrix(MATRIX* m);
extern long ReadGeomScreen(void);
extern long RotTransPers4(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3,
                          long* sxy0, long* sxy1, long* sxy2, long* sxy3,
                          long* p, long* flag);
extern int printf(const char* fmt, ...);
extern PACKET* _make_packet(u_long* p, GsOT* ot, unsigned short pri, int len);

extern PACKET* GsOUT_PACKET_P;

#define MODE_PAL 1

extern void ResetGraph(int mode);
extern DRAWENV* PutDrawEnv(DRAWENV* env);
extern DISPENV* PutDispEnv(DISPENV* env);
extern void AddPrim(void* ot, void* p);
extern long GetVideoMode(void);
extern void SetGeomOffset(long ofx, long ofy);
extern void gte_init(void);
extern void InitGeom(void);
extern void SetFarColor(long r, long g, long b);
extern void SetBlockFill(void* p);
extern void SetDispMask(int mask);
extern void DrawOTag(u_long* p);
extern u_long* ClearOTagR(u_long* ot, int n);
extern int ClearImage(RECT* rect, u_char r, u_char g, u_char b);

void GsSetDrawBuffClip(void);
void GsSetDrawBuffOffset(void);

#if (VERSION < 40 && !defined(LIBGS_SCLIB) && !defined(LIBGS_EXTERN_ONLY) && \
     !defined(LIBGS_MATRIX3)) || \
    defined(LIBGS_DEFINE_GLOBALS)
#define GS_GLOBAL
#else
#define GS_GLOBAL extern
#endif
#if VERSION == 30

typedef struct {
    short flag;
    short scale;
    struct _GsCOORDINATE* coord;
    MATRIX matrix;
} _GsDSTACK;

GS_GLOBAL RECT CLIP2;
GS_GLOBAL u_short PSDBASEX[2];
GS_GLOBAL u_short PSDBASEY[2];
GS_GLOBAL u_short PSDOFSX[2];
GS_GLOBAL u_short PSDOFSY[2];
GS_GLOBAL u_short PSDIDX;
GS_GLOBAL u_short PSDGPU;
GS_GLOBAL _GsPOSITION POSITION;
GS_GLOBAL _GsDSTACK GsDSTACK[32];
GS_GLOBAL DRAWENV GsDRAWENV;
GS_GLOBAL DISPENV GsDISPENV;
extern u_long PSDCNT;
extern short GsORGOFSX;
extern short GsORGOFSY;
#else
GS_GLOBAL RECT CLIP2;
GS_GLOBAL short PSDBASEX[2];
GS_GLOBAL short PSDBASEY[2];
GS_GLOBAL short PSDIDX;
GS_GLOBAL u_long PSDCNT;
GS_GLOBAL _GsPOSITION POSITION;
GS_GLOBAL DRAWENV GsDRAWENV;
GS_GLOBAL DISPENV GsDISPENV;
GS_GLOBAL short PSDOFSX[2];
GS_GLOBAL short PSDOFSY[2];
GS_GLOBAL short GsORGOFSX;
GS_GLOBAL short GsORGOFSY;
GS_GLOBAL short PSDGPU;
#if VERSION <= 33
typedef struct {
    short flag;
    short scale;
    struct _GsCOORDINATE* coord;
    MATRIX matrix;
} _GsDSTACK;
GS_GLOBAL _GsDSTACK GsDSTACK[32];
#endif
#endif

#if VERSION < 40 && defined(LIBGS_SCLIB)
#define SC104_GLOBAL
#define SC107_GLOBAL
#define SC123_GLOBAL
#else
#ifdef LIBGS_DEFINE_SC104
#define SC104_GLOBAL
#else
#define SC104_GLOBAL extern
#endif
#ifdef LIBGS_DEFINE_SC107
#define SC107_GLOBAL
#else
#define SC107_GLOBAL extern
#endif
#ifdef LIBGS_DEFINE_SC123
#define SC123_GLOBAL
#else
#define SC123_GLOBAL extern
#endif
#endif
#if VERSION == 30
SC123_GLOBAL MATRIX GsWSMATRIX;
SC104_GLOBAL int GsLIGHT_MODE;
SC104_GLOBAL u_long HWD0;
SC104_GLOBAL u_long VWD0;
SC104_GLOBAL long GsCLIP3far;
SC104_GLOBAL long GsCLIP3near;
SC107_GLOBAL MATRIX GsLIGHTWSMATRIX;
SC107_GLOBAL MATRIX _LC;
SC123_GLOBAL MATRIX GsIDMATRIX;
SC123_GLOBAL MATRIX GsIDMATRIX2;
#else
SC104_GLOBAL long HWD0;
SC104_GLOBAL long VWD0;
SC107_GLOBAL MATRIX GsLIGHTWSMATRIX;
SC123_GLOBAL MATRIX GsIDMATRIX;
#if VERSION >= 40
SC123_GLOBAL MATRIX GsWSMATRIX;
#endif
SC123_GLOBAL MATRIX GsIDMATRIX2;
#if VERSION >= 40
SC123_GLOBAL MATRIX GsWSMATRIX_ORG;
#endif
SC104_GLOBAL int GsLIGHT_MODE;
SC104_GLOBAL long GsCLIP3far;
SC104_GLOBAL long GsCLIP3near;
SC107_GLOBAL MATRIX _LC;
#endif

typedef struct {
    VECTOR scale;
    SVECTOR rotate;
    VECTOR trans;
} GsCOORD2PARAM;

typedef struct _GsCOORDINATE2 {
    u_long flg;
    MATRIX coord;
    MATRIX workm;
    GsCOORD2PARAM* param;
    struct _GsCOORDINATE2* super;
    struct _GsCOORDINATE2* sub;
} GsCOORDINATE2;

typedef struct {
    MATRIX view;
    GsCOORDINATE2* super;
} GsVIEW2;

typedef struct {
    long vpx, vpy, vpz;
    long vrx, vry, vrz;
    long rz;
    GsCOORDINATE2* super;
} GsRVIEW2;

extern GsCOORDINATE2* DSTACK[];
extern void GsMulCoord2(MATRIX* m1, MATRIX* m2);
extern void GsMulCoord3(MATRIX* m1, MATRIX* m2);
extern void GsGetLw(GsCOORDINATE2* coord, MATRIX* m);
extern MATRIX* TransposeMatrix(MATRIX* m0, MATRIX* m1);
extern VECTOR* ApplyMatrixLV(MATRIX* m, VECTOR* v0, VECTOR* v1);

#if VERSION < 40 && defined(LIBGS_MATRIX3)
#define M3_GLOBAL
#else
#define M3_GLOBAL extern
#endif
#if VERSION >= 35
M3_GLOBAL MATRIX GsLSMATRIX;
#endif
#if VERSION < 40
M3_GLOBAL MATRIX GsWSMATRIX;
M3_GLOBAL MATRIX GsWSMATRIX_ORG;
#endif

typedef struct _GsCOORDUNIT {
    u_long flg;
    MATRIX matrix;
    MATRIX workm;
    SVECTOR rot;
    struct _GsCOORDUNIT* super;
} GsCOORDUNIT;

typedef struct {
    MATRIX view;
    GsCOORDUNIT* super;
} GsVIEWUNIT;

typedef struct {
    long vpx, vpy, vpz;
    long vrx, vry, vrz;
    long rz;
    GsCOORDUNIT* super;
} GsRVIEWUNIT;

extern GsCOORDUNIT* USTACK[];
extern void GsGetLwUnit(GsCOORDUNIT* coord, MATRIX* m);

#endif
