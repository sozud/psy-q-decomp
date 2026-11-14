// SPDX-License-Identifier: MIT
#include "common.h"
#include <psxsdk/libgpu.h>

extern int (*GPU_printf)();

#if VERSION == 26
static char rcsid[] = "$Id: prim.c,v 1.51 1995/01/12 11:35:25 suzu Exp $";
#endif

u_short GetTPage(int tp, int abr, int x, int y) {
#if VERSION <= 30
    return GetGraphType() ? (tp & 3) << 9 | (abr & 3) << 7 | (y & 0x300) >> 3 |
                                (x & 0x3FF) >> 6
                          : (tp & 3) << 7 | (abr & 3) << 5 | (y & 0x100) >> 4 |
                                (x & 0x3FF) >> 6;
#elif VERSION >= 40
    return (tp & 3) << 7 | (abr & 3) << 5 | (y & 0x100) >> 4 |
           (x & 0x3FF) >> 6 | (y & 0x200) << 2;
#else
    return GetGraphType() == 1 || GetGraphType() == 2
               ? (tp & 3) << 9 | (abr & 3) << 7 | (y & 0x300) >> 3 |
                     (x & 0x3FF) >> 6
               : (tp & 3) << 7 | (abr & 3) << 5 | (y & 0x100) >> 4 |
                     (x & 0x3FF) >> 6 | (y & 0x200) << 2;
#endif
}

u16 GetClut(s32 x, s32 y) { return getClut(x, y); }

void DumpTPage(u_short tpage) {
#if VERSION <= 30
    if (GetGraphType()) {
        GPU_printf("tpage: (%d,%d,%d,%d)\n", (tpage >> 9) & 3, (tpage >> 7) & 3,
                   (tpage << 6) & 0x7C0, (tpage * 8) & 0x300);
    } else {
        GPU_printf("tpage: (%d,%d,%d,%d)\n", (tpage >> 7) & 3, (tpage >> 5) & 3,
                   (tpage << 6) & 0x7C0, (tpage * 0x10) & 0x100);
    }
#elif VERSION >= 40
    GPU_printf("tpage: (%d,%d,%d,%d)\n", (tpage >> 7) & 3, (tpage >> 5) & 3,
               (tpage << 6) & 0x7C0,
               ((tpage << 4) & 0x100) + ((tpage >> 2) & 0x200));
#else
    if (GetGraphType() == 1 || GetGraphType() == 2) {
        GPU_printf("tpage: (%d,%d,%d,%d)\n", (tpage >> 9) & 3, (tpage >> 7) & 3,
                   (tpage << 6) & 0x7C0, (tpage * 8) & 0x300);
    } else {
        GPU_printf("tpage: (%d,%d,%d,%d)\n", (tpage >> 7) & 3, (tpage >> 5) & 3,
                   (tpage << 6) & 0x7C0,
                   ((tpage * 0x10) & 0x100) + ((tpage >> 2) & 0x200));
    }
#endif
}

void DumpClut(u_short clut) {
    GPU_printf("clut: (%d,%d)\n", (clut & 0x3F) * 0x10, clut >> 6);
}

void* NextPrim(void* p) { return nextPrim(p); }

int IsEndPrim(void* p) { return isendprim(p); }

void AddPrim(void* ot, void* p) { addPrim(ot, p); }

void AddPrims(void* ot, void* p0, void* p1) { addPrims(ot, p0, p1); }

#if VERSION == 26
void CatPrim(void* p0, void* p1) { setaddr(p0, getaddr(p1)); }
#else
void CatPrim(void* p0, void* p1) { catPrim(p0, p1); }
#endif

void TermPrim(void* p) { return termPrim(p); }

void SetSemiTrans(void* p, int abe) { return setSemiTrans(p, abe); }

void SetShadeTex(void* p, int tge) { return setShadeTex(p, tge); }

void SetPolyF3(POLY_F3* p) { setPolyF3(p); }
void SetPolyFT3(POLY_FT3* p) { setPolyFT3(p); }
void SetPolyG3(POLY_G3* p) { setPolyG3(p); }
void SetPolyGT3(POLY_GT3* p) { setPolyGT3(p); }
void SetPolyF4(POLY_F4* p) { setPolyF4(p); }
void SetPolyFT4(POLY_FT4* p) { setPolyFT4(p); }
void SetPolyG4(POLY_G4* p) { setPolyG4(p); }
void SetPolyGT4(POLY_GT4* p) { setPolyGT4(p); }
void SetSprt8(SPRT_8* p) { setSprt8(p); }
void SetSprt16(SPRT_16* p) { setSprt16(p); }
void SetSprt(SPRT* p) { setSprt(p); }
void SetTile1(TILE_1* p) { setTile1(p); }
void SetTile8(TILE_8* p) { setTile8(p); }
void SetTile16(TILE_16* p) { setTile16(p); }
void SetTile(TILE* p) { setTile(p); }
#if VERSION <= 30
void SetBlockFill(BLK_FILL* p) { setBlockFill(p); }
#endif
void SetLineF2(LINE_F2* p) { setLineF2(p); }
void SetLineG2(LINE_G2* p) { setLineG2(p); }
#if VERSION == 26
void SetLineF3(LINE_F3* p) { setlen(p, 5), setcode(p, 0x48); }
#else
void SetLineF3(LINE_F3* p) { setLineF3(p); }
#endif
#if VERSION >= 43
void SetLineG3(LINE_G3* p) { setLineG3(p), p->p2 = 0; }
void SetLineF4(LINE_F4* p) { setLineF4(p); }
void SetLineG4(LINE_G4* p) { setLineG4(p), p->p2 = 0, p->p3 = 0; }
#else
void SetLineG3(LINE_G3* p) { setLineG3(p); }
void SetLineF4(LINE_F4* p) { setLineF4(p); }
void SetLineG4(LINE_G4* p) { setLineG4(p); }
#endif
#if VERSION > 30 && VERSION < 37
void SetBlockFill(BLK_FILL* p) { setBlockFill(p); }
#endif

#if VERSION > 30 && VERSION < 36
void SetDrawMove(u_long* p) {
    ((u8*)p)[3] = 5;
    ((u8*)p)[7] = 1;
    p[2] = 0x80000000;
}
#endif

#if VERSION >= 35
void SetDrawTPage(u_long* p, int dfe, int dtd, int tpage) {
#if VERSION >= 40
    ((u8*)p)[3] = 1;
    p[1] = 0xE1000000 | (dtd ? 0x200 : 0) | (dfe ? 0x400 : 0) |
           (tpage & 0x9FF);
#else
    u32 code;

    ((u8*)p)[3] = 1;
    if (GetGraphType() == 1 || GetGraphType() == 2) {
        code = 0xE1000000 | (dtd ? 0x800 : 0) | (dfe ? 0x1000 : 0) |
               (tpage & 0x27FF);
    } else {
        code = 0xE1000000 | (dtd ? 0x200 : 0) | (dfe ? 0x400 : 0) |
               (tpage & 0x9FF);
    }
    p[1] = code;
#endif
}
#endif

#if VERSION >= 36
void SetDrawMove(u_long* p, RECT* rect, int x, int y) {
    int len = 5;

    if (!rect->w || !rect->h) {
        len = 0;
    }
    ((u8*)p)[3] = len;
    p[1] = 0x01000000;
    p[2] = 0x80000000;
    p[3] = *(u_long*)&rect->x;
    p[4] = (y << 16) | (x & 0xFFFF);
    p[5] = *(u_long*)&rect->w;
}

typedef struct {
    u_long tag;
    u_long code[12];
} DR_LOAD;

void SetDrawLoad(DR_LOAD* p, RECT* rect) {
    int pixel_count = (rect->w * rect->h + 1) / 2;
    int len = pixel_count + 4;

    if (pixel_count < 0 || pixel_count > 12) {
        len = 0;
    }
    setlen(p, len);
    p->code[0] = 0xA0000000;
    p->code[1] = *(u_long*)&rect->x;
    p->code[2] = *(u_long*)&rect->w;
    p->code[len - 1] = 0x01000000;
}
#elif VERSION >= 35
void SetDrawLoad(u_long* p, RECT* rect) {
    int length;

    length = (rect->w * rect->h + 1) / 2 + 4;
    if (length < 5 || length >= 16) {
        length = 0;
    }
    p[1] = 0x01000000;
    ((u8*)p)[3] = length;
    p[2] = 0xA0000000;
    p[3] = *(u_long*)&rect->x;
    p[4] = *(u_long*)&rect->w;
}
#endif

int MargePrim(void* p1, void* p2) {
    int length = getlen(p1) + getlen(p2) + 1;
#if VERSION >= 36
    if (length > 16) {
#else
    if (length >= 0x21) {
#endif
        return -1;
    }
    setlen(p1, length);
#if VERSION >= 35 || VERSION == 26
    *(u_long*)p2 = 0;
#endif
    return 0;
}

void DumpDrawEnv(DRAWENV* env) {
    GPU_printf("clip (%3d,%3d)-(%d,%d)\n", env->clip.x, env->clip.y,
               env->clip.w, env->clip.h);
    GPU_printf("ofs  (%3d,%3d)\n", env->ofs[0], env->ofs[1]);
    GPU_printf(
        "tw   (%d,%d)-(%d,%d)\n", env->tw.x, env->tw.y, env->tw.w, env->tw.h);
    GPU_printf("dtd   %d\n", env->dtd);
    GPU_printf("dfe   %d\n", env->dfe);
#if VERSION <= 30
    if (GetGraphType()) {
        GPU_printf("tpage: (%d,%d,%d,%d)\n", (env->tpage >> 9) & 3,
                   (env->tpage >> 7) & 3, (env->tpage << 6) & 0x7C0,
                   (env->tpage * 8) & 0x300);
    } else {
        GPU_printf("tpage: (%d,%d,%d,%d)\n", (env->tpage >> 7) & 3,
                   (env->tpage >> 5) & 3, (env->tpage << 6) & 0x7C0,
                   (env->tpage * 0x10) & 0x100);
    }
#elif VERSION == 41
    {
        int tpage = env->tpage;

        GPU_printf("tpage: (%d,%d,%d,%d)\n", (tpage >> 7) & 3,
                   (tpage >> 5) & 3, (tpage << 6) & 0x7C0,
                   ((tpage << 4) & 0x100) + ((tpage >> 2) & 0x200));
    }
#elif VERSION >= 40
    GPU_printf("tpage: (%d,%d,%d,%d)\n", (env->tpage >> 7) & 3,
               (env->tpage >> 5) & 3, (env->tpage << 6) & 0x7C0,
               ((env->tpage << 4) & 0x100) + ((env->tpage >> 2) & 0x200));
#else
    if (GetGraphType() == 1 || GetGraphType() == 2) {
        GPU_printf("tpage: (%d,%d,%d,%d)\n", (env->tpage >> 9) & 3,
                   (env->tpage >> 7) & 3, (env->tpage << 6) & 0x7C0,
                   (env->tpage * 8) & 0x300);
    } else {
        GPU_printf("tpage: (%d,%d,%d,%d)\n", (env->tpage >> 7) & 3,
                   (env->tpage >> 5) & 3, (env->tpage << 6) & 0x7C0,
                   ((env->tpage * 0x10) & 0x100) +
                       ((env->tpage >> 2) & 0x200));
    }
#endif
}

void DumpDispEnv(DISPENV* env) {
    GPU_printf("disp   (%3d,%3d)-(%d,%d)\n", env->disp.x, env->disp.y,
               env->disp.w, (s32)env->disp.h);
    GPU_printf("screen (%3d,%3d)-(%d,%d)\n", env->screen.x, env->screen.y,
               env->screen.w, (s32)env->screen.h);
    GPU_printf("isinter %d\n", env->isinter);
    GPU_printf("isrgb24 %d\n", env->isrgb24);
}

