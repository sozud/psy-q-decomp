// SPDX-License-Identifier: MIT
#include "libgpu.h"

#if VERSION == 26
static char rcsid[] = "$Id: ext.c,v 1.51 1995/01/12 11:35:15 suzu Exp $";
#endif

u_short LoadTPage(u_long* pix, int tp, int abr, int x, int y, int w, int h) {
    RECT rect;

    rect.x = x;
    rect.y = y;
    rect.h = h;
    switch (tp) {
    case 0:
        rect.w = w / 4;
        break;
    case 1:
        rect.w = w / 2;
        break;
    case 2:
        rect.w = w;
        break;
    }
    LoadImage(&rect, pix);

    return GetTPage(tp, abr, x, y);
}

u_short LoadClut(u_long* clut, int x, int y) {
    RECT rect;

    rect.x = x;
    rect.y = y;
    rect.w = 0x100;
    rect.h = 1;
    LoadImage(&rect, clut);

    return GetClut(x, y);
}

#if VERSION >= 35
u_short LoadClut2(u_long* clut, int x, int y) {
    RECT rect;

    rect.x = x;
    rect.y = y;
    rect.w = 0x10;
    rect.h = 1;
    LoadImage(&rect, clut);

    return GetClut(x, y);
}
#endif

#if VERSION <= 30
DRAWENV* SetDefDrawEnv(DRAWENV* env, s32 x, s32 y, s32 w, s32 h) {
    env->clip.x = x;
    env->clip.y = y;
    env->clip.w = w;
    env->tw.x = 0;
    env->tw.y = 0;
    env->tw.w = 0;
    env->tw.h = 0;
    env->r0 = 0;
    env->g0 = 0;
    env->b0 = 0;
    env->dtd = 1;
    env->ofs[0] = x;
    env->ofs[1] = y;
    env->clip.h = h;
    env->dfe = h != 480;
    env->tpage = GetTPage(0, 0, 640, 0);
    env->isbg = 0;
    return env;
}
#elif VERSION == 33
DRAWENV* SetDefDrawEnv(DRAWENV* env, s32 x, s32 y, s32 w, s32 h) {
    s32 videoMode = GetVideoMode();

    env->clip.x = x;
    env->clip.y = y;
    env->clip.w = w;
    env->tw.x = 0;
    env->tw.y = 0;
    env->tw.w = 0;
    env->tw.h = 0;
    env->r0 = 0;
    env->g0 = 0;
    env->b0 = 0;
    env->dtd = 1;
    env->dfe = 0;
    env->ofs[0] = x;
    env->ofs[1] = y;
    env->clip.h = h;
    if (GetGraphType() != 1) {
        GetGraphType();
    }
    env->tpage = 10;
    env->isbg = 0;
    return env;
}
#else
DRAWENV* SetDefDrawEnv(DRAWENV* env, s32 x, s32 y, s32 w, s32 h) {
    s32 videoMode = GetVideoMode();

    env->clip.x = x;
    env->clip.y = y;
    env->clip.w = w;
    env->tw.x = 0;
    env->tw.y = 0;
    env->tw.w = 0;
    env->tw.h = 0;
    env->r0 = 0;
    env->g0 = 0;
    env->b0 = 0;
    env->dtd = 1;
    env->clip.h = h;
    env->dfe = videoMode ? h < 289 : h < 257;
    env->ofs[0] = x;
    env->ofs[1] = y;
#if VERSION < 40
    if (GetGraphType() != 1) {
        GetGraphType();
    }
#endif
    env->tpage = 10;
    env->isbg = 0;
    return env;
}
#endif

DISPENV* SetDefDispEnv(DISPENV* env, s32 x, s32 y, s32 w, s32 h) {
    env->disp.x = x;
    env->disp.y = y;
    env->disp.w = w;
    env->disp.h = h;
    env->screen.x = 0;
    env->screen.y = 0;
    env->screen.w = 0;
    env->screen.h = 0;
    env->isrgb24 = 0;
    env->isinter = 0;
    env->pad1 = 0;
    env->pad0 = 0;
    return env;
}
