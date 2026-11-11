#ifndef LIBGPU_LOCAL_H
#define LIBGPU_LOCAL_H

#include "../types.h"

typedef struct {
    short x;
    short y;
    short w;
    short h;
} RECT;

typedef struct {
    u_long reserved[16];
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

int LoadImage(RECT*, u_long*);
u_short GetTPage(int, int, int, int);
u_short GetClut(int, int);
int GetGraphType(void);
int GetVideoMode(void);

#endif
