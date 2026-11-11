// SPDX-License-Identifier: MIT

#include "common.h"
#include <psxsdk/libgpu.h>

#if VERSION == 26

#define LOW(x) (*(s32*)&(x))

typedef struct {
     char* rcsid;
     int (*addque)(void (*)(), void*, int);
     int (*addque2)(void (*)(), void*, int, int);
     int (*clr)(RECT*, int);
     void (*ctl)(u_long);
     int (*cwb)(u_long*, int);
     void (*cwc)(u_long*);
     int (*drs)(RECT*, u_long*);
     int (*dws)(RECT*, u_long*);
     int (*exeque)(void);
     int (*getctl)(int);
     int (*otc)(u_long*, int);
     int (*param)(int);
     int (*reset)(int);
     u_long (*status)(void);
     int (*sync)(int);
} gpu;

typedef struct {
     void (*func)();
     void* param;
     int arg;
     u_long buf[8];
} QueueItem;

static int _addque(void (*func)(), void* param, int arg);
static int _addque2(void (*func)(), void* param, int len, int arg);
static int _clr(RECT* rect, int color);
static void _ctl(u_long cmd);
static int _cwb(u_long* p, int n);
static void _cwc(u_long* p);
static int _drs(RECT* rect, u_long* p);
static int _dws(RECT* rect, u_long* p);
static int _exeque(void);
static int _getctl(int n);
static int _otc(u_long* ot, int n);
static int _param(int n);
static int _reset(int mode);
static u_long _status(void);
static int _sync(int mode);
static u_long get_mode(int dfe, int dtd, int tpage);
static u_long get_cs(short x, short y);
static u_long get_ce(short x, short y);
static u_long get_ofs(short x, short y);
static u_long get_tw(RECT* tw);
static u_long get_dx(DISPENV* env);
static void set_alarm(void);
static int get_alarm(void);
static void memcpy(void* dst, void* src, int n);

int printf(const char*, ...);
void GPU_cw(u_long);
void ResetCallback(void);
int DMACallback(int, int (*)());
int CheckCallback(void);
void CheckPrim(char*, void*);
void DrawOTagIO(OT_TYPE*);
extern int Vcount;

static char rcsid[] = "$Id: sys.c,v 1.52 1995/01/12 11:35:27 suzu Exp $";

static void set_jtb(gpu* jtb) { GPU_cw((u_long)jtb & 0xFFFFFF); }

static gpu _jtb = {
    rcsid,   _addque, _addque2, _clr,    _ctl,    _cwb,   _cwc,    _drs,
    _dws,    _exeque, _getctl,  _otc,    _param,  _reset, _status, _sync,
};
static gpu* D_jtb = &_jtb;
int (*GPU_printf)(const char* fmt, ...) = printf;
static int _level = 0;
static int _type = 0;
static int _reverse = 0;
static void (*_drawsync_cb)() = NULL;

int ResetGraph(int mode) {
    switch (mode & 3) {
    case 0:
        ResetCallback();
        DMACallback(2, D_jtb->exeque);
        set_jtb(D_jtb);
        _type = D_jtb->reset(0);
        break;
    case 1:
    case 2:
        _type = D_jtb->reset(1);
        break;
    case 3:
        _type = D_jtb->reset(1);
        DMACallback(2, D_jtb->exeque);
        break;
    default:
        return -1;
    }
    D_jtb->ctl(D_jtb->getctl(8) | (_reverse ? 0x08000080 : 0x08000000));
    if (mode & 8) {
        _type = 2;
        D_jtb->ctl(_reverse ? 0x20000501 : 0x20000504);
    }
}

int SetGraphReverse(int mode) {
    int old = _reverse;

    _reverse = mode;
    D_jtb->ctl(D_jtb->getctl(8) | (_reverse ? 0x08000080 : 0x08000000));
    if (_type == 2) {
        D_jtb->ctl(_reverse ? 0x20000501 : 0x20000504);
    }
    return old;
}

int SetGraphDebug(int level) {
    int old = _level;

    _level = level;
    if (level) {
        GPU_printf("SetGraphDebug:level:%d,type:%d reverse:%d\n", level,
                   _type, _reverse);
    }
    return old;
}

int GetGraphType(void) { return _type; }

int GetGraphDebug(void) { return _level; }

u_long DrawSyncCallback(void (*func)()) {
    void (*old)() = _drawsync_cb;

    _drawsync_cb = func;
    return (u_long)old;
}

void SetDispMask(int mask) { D_jtb->ctl(mask ? 0x03000000 : 0x03000001); }

int DrawSync(int mode) { return D_jtb->sync(mode); }

int ClearImage(RECT* rect, u_char r, u_char g, u_char b) {
    return D_jtb->addque2(
        D_jtb->clr, rect, sizeof(RECT), (b << 0x10) | (g << 8) | r);
}

int LoadImage(RECT* rect, u_long* p) {
    return D_jtb->addque2(D_jtb->dws, rect, sizeof(RECT), p);
}

int StoreImage(RECT* rect, u_long* p) {
    return D_jtb->addque2(D_jtb->drs, rect, sizeof(RECT), p);
}

int MoveImage(RECT* rect, int x, int y) {
    u_long param[5];

    param[0] = 0x04FFFFFF;
    param[1] = 0x80000000;
    param[2] = LOW(rect->x);
    param[3] = y << 0x10 | (u_short)x;
    param[4] = LOW(rect->w);
    return D_jtb->addque2(D_jtb->cwc, param, sizeof(param), 0);
}

OT_TYPE* ClearOTag(OT_TYPE* ot, int n) {
    while (--n) {
        setlen(ot, 0);
        setaddr(ot, ot + 1);
        ot++;
    }
    *ot = 0xFFFFFF;
    return ot;
}

OT_TYPE* ClearOTagR(OT_TYPE* ot, int n) {
    D_jtb->otc(ot, n);
    return ot;
}

static DRAWENV c_draw;
static DISPENV c_disp;

void DrawPrim(void* p) {
    if (_level) {
        CheckPrim("DrawPrim", p);
    }
    D_jtb->sync(0);
    D_jtb->cwb((u_long*)&((P_TAG*)p)->r0, getlen(p));
}

void DrawOTag(OT_TYPE* p) {
    if (_level) {
        DrawOTagIO(p);
    } else {
        D_jtb->addque(D_jtb->cwc, p, 0);
    }
}

DRAWENV* PutDrawEnv(DRAWENV* env) {
    SetDrawEnv(&env->dr_env, env);
    env->dr_env.tag |= 0xFFFFFF;
    D_jtb->addque(D_jtb->cwc, &env->dr_env, 0);
    c_draw = *env;
    return env;
}

DRAWENV* GetDrawEnv(DRAWENV* env) {
    *env = c_draw;
    return env;
}

#define CLAMP(value, low, high)                                                \
    value < low ? low : (value > high ? high : value)

DISPENV* PutDispEnv(DISPENV* env) {
    int h_start;
    int v_start;
    int cmd;
    int v_end;
    int h_end;

    cmd = 0x08000000;
    D_jtb->ctl(0x05000000 | (_type ? ((env->disp.y & 0xFFF) << 0xC) |
                                         (get_dx(env) & 0xFFF)
                                   : ((env->disp.y & 0x1FF) << 0xA) |
                                         (env->disp.x & 0x3FF)));
    h_start = env->screen.x * 0xA + 0x260;
    v_start = env->screen.y + 0x10;
    h_end = h_start + (env->screen.w ? env->screen.w * 0xA : 0xA00);
    v_end = v_start + (env->screen.h ? env->screen.h : 0xF0);
    h_start = CLAMP(h_start, 0x1F4, 0xCDA);
    h_end = CLAMP(h_end, h_start + 0x50, 0xCDA);
    v_start = CLAMP(v_start, 0, 0xFF);
    v_end = CLAMP(v_end, v_start, 0xFF);
    D_jtb->ctl(0x06000000 | ((h_end & 0xFFF) << 0xC) | (h_start & 0xFFF));
    D_jtb->ctl(0x07000000 | ((v_end & 0x3FF) << 0xA) | (v_start & 0x3FF));
    if (env->pad0 == 1) {
        cmd |= 0x8;
    }
    if (env->isrgb24) {
        cmd |= 0x10;
    }
    if (env->isinter) {
        cmd |= 0x20;
    }
    if (_reverse) {
        cmd |= 0x80;
    }
    if (env->disp.w > 0x118) {
        if (env->disp.w <= 0x160) {
            cmd |= 1;
        } else if (env->disp.w <= 0x190) {
            cmd |= 0x40;
        } else if (env->disp.w <= 0x230) {
            cmd |= 2;
        } else {
            cmd |= 3;
        }
    }
    if (env->disp.h <= (env->pad0 ? 0x120 : 0x100)) {
    } else {
        cmd |= 0x24;
    }
    D_jtb->ctl(cmd);
    c_disp = *env;
    return env;
}

DISPENV* GetDispEnv(DISPENV* env) {
    *env = c_disp;
    return env;
}

void SetTexWindow(DR_TWIN* p, RECT* tw) {
    setlen(p, 2);
    p->code[0] = get_tw(tw);
    p->code[1] = 0;
}

void SetDrawArea(DR_AREA* p, RECT* r) {
    setlen(p, 2);
    p->code[0] = get_cs(r->x, r->y);
    p->code[1] = get_ce(r->x + r->w - 1, r->y + r->h - 1);
}

void SetDrawOffset(DR_OFFSET* p, u_short* ofs) {
    setlen(p, 2);
    p->code[0] = get_ofs(ofs[0], ofs[1]);
    p->code[1] = 0;
}

void SetPriority(DR_PRIO* p, int pbc, int pbw) {
    setlen(p, 2);
    p->code[0] = 0xE6000000 | (pbc ? 2 : 0) | (pbw ? 1 : 0);
    p->code[1] = 0;
}

void SetDrawMode(DR_MODE* p, int dfe, int dtd, int tpage, RECT* tw) {
    setlen(p, 2);
    p->code[0] = get_mode(dfe, dtd, tpage);
    p->code[1] = get_tw(tw);
}

void SetDrawEnv(DR_ENV* dr_env_in, DRAWENV* env) {
    RECT clip_rect;
    int offset;
    DR_ENV* dr_env;

    dr_env = dr_env_in;

    dr_env->code[0] = get_cs(env->clip.x, env->clip.y);
    dr_env->code[1] =
        get_ce(env->clip.w + env->clip.x - 1, env->clip.y + env->clip.h - 1);
    dr_env->code[2] = get_ofs(env->ofs[0], env->ofs[1]);
    dr_env->code[3] = get_mode(env->dfe, env->dtd, env->tpage);
    dr_env->code[4] = get_tw(&env->tw);
    dr_env->code[5] = 0xE6000000;

    offset = 7;
    if (env->isbg != 0) {
        clip_rect.x = env->clip.x;
        clip_rect.y = env->clip.y;
        clip_rect.w = env->clip.w;
        clip_rect.h = env->clip.h;
        clip_rect.w = CLAMP(clip_rect.w, 0, 0x400 - 1);
        clip_rect.h = CLAMP(clip_rect.h, 0, (_type ? 0x400 : 0x200) - 1);

        if ((clip_rect.x & 0x3F) || (clip_rect.w & 0x3F)) {
            clip_rect.x -= env->ofs[0];
            clip_rect.y -= env->ofs[1];
            *((int*)dr_env + offset++) =
                0x60000000 | env->b0 << 0x10 | env->g0 << 8 | env->r0;
            *((int*)dr_env + offset++) = LOW(clip_rect.x);
            *((int*)dr_env + offset++) = LOW(clip_rect.w);
            clip_rect.x += env->ofs[0];
            clip_rect.y += env->ofs[1];
        } else {
            *((int*)dr_env + offset++) =
                0x02000000 | env->b0 << 0x10 | env->g0 << 8 | env->r0;
            *((int*)dr_env + offset++) = LOW(clip_rect.x);
            *((int*)dr_env + offset++) = LOW(clip_rect.w);
        }
    }

    ((P_TAG*)dr_env)->len = (u_char)offset - 1;
}

static u_long get_mode(int dfe, int dtd, int tpage) {
    if (_type) {
        return 0xE1000000 | (dtd ? 0x800 : 0) | (dfe ? 0x1000 : 0) |
               (tpage & 0x27FF);
    } else {
        return 0xE1000000 | (dtd ? 0x200 : 0) | (dfe ? 0x400 : 0) |
               (tpage & 0x1FF);
    }
}

static u_long get_cs(short x, short y) {
    x = CLAMP(x, 0, 0x400 - 1);
    y = CLAMP(y, 0, (_type ? 0x400 : 0x200) - 1);
    if (_type) {
        return 0xE3000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE3000000 | ((y & 0x3FF) << 10) | (x & 0x3FF);
    }
}

static u_long get_ce(short x, short y) {
    x = CLAMP(x, 0, 0x400 - 1);
    y = CLAMP(y, 0, (_type ? 0x400 : 0x200) - 1);
    if (_type) {
        return 0xE4000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE4000000 | ((y & 0x1FF) << 10) | (x & 0x3FF);
    }
}

static u_long get_ofs(short x, short y) {
    if (_type) {
        return 0xE5000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE5000000 | ((y & 0x7FF) << 11) | (x & 0x7FF);
    }
}

static u_long get_tw(RECT* tw) {
    u_long pad[4];

    if (tw != NULL) {
        pad[0] = (tw->x & 0xFF) >> 3;
        pad[2] = (-tw->w & 0xFF) >> 3;
        pad[1] = (tw->y & 0xFF) >> 3;
        pad[3] = (-tw->h & 0xFF) >> 3;
        return 0xE2000000 | (pad[1] << 0xF) | (pad[0] << 0xA) | (pad[3] << 5) |
               pad[2];
    }
    return 0;
}

static u_long get_dx(DISPENV* env) {
    switch (_type) {
    case 0:
        return env->disp.x;
    case 1:
        return _reverse ? 0x400 - env->disp.x - env->disp.w : env->disp.x;
    case 2:
        return _reverse ? 0x400 - env->disp.x - env->disp.w / 2
                        : env->disp.x / 2;
    }
}

static volatile int _qin = 0;
static volatile int _qout = 0;
static volatile QueueItem que[0x40];

static volatile u_long* GPU_DATA = (u_long*)0x1F801810;
static volatile u_long* GPU_STATUS = (u_long*)0x1F801814;
static volatile u_long* DMA2_MADR = (u_long*)0x1F8010A0;
static volatile u_long* DMA2_BCR = (u_long*)0x1F8010A4;
static volatile u_long* DMA2_CHCR = (u_long*)0x1F8010A8;
static volatile u_long* DMA6_MADR = (u_long*)0x1F8010E0;
static volatile u_long* DMA6_BCR = (u_long*)0x1F8010E4;
static volatile u_long* DMA6_CHCR = (u_long*)0x1F8010E8;
static volatile u_long* DPCR = (u_long*)0x1F8010F0;

static u_long _status(void) { return *GPU_STATUS; }

static int _otc(u_long* ot, int n) {
    *DPCR;
    *DPCR |= 0x08000000;
    *DMA6_CHCR = 0;
    *DMA6_MADR = (u_long)&ot[n - 1];
    *DMA6_BCR = n;
    *DMA6_CHCR = 0x11000002;
    set_alarm();
    while (*DMA6_CHCR & 0x01000000) {
        if (get_alarm()) {
            return -1;
        }
    }
    return n;
}

static int _clr(RECT* rect, int color) {
    static u_long p0[8];
    static u_long p1[8];
    RECT temp;

    temp.x = rect->x;
    temp.y = rect->y;
    temp.w = rect->w;
    temp.h = rect->h;
    temp.w = CLAMP(temp.w, 0, 0x400 - 1);
    temp.h = CLAMP(temp.h, 0, (_type ? 0x400 : 0x200) - 1);
    if ((temp.x & 0x3F) || (temp.w & 0x3F)) {
        p0[0] = ((u_long)p1 & 0xFFFFFF) | 0x07000000;
        p0[1] = 0xE3000000;
        p0[2] = 0xE4FFFFFF;
        p0[3] = 0xE5000000;
        p0[4] = 0xE6000000;
        p0[5] = (color & 0xFFFFFF) | 0x60000000;
        p0[6] = LOW(temp.x);
        p0[7] = LOW(temp.w);
        p1[0] = 0x03FFFFFF;
        p1[1] = _param(3) | 0xE3000000;
        p1[2] = _param(4) | 0xE4000000;
        p1[3] = _param(5) | 0xE5000000;
    } else {
        p0[0] = 0x04FFFFFF;
        p0[1] = 0xE6000000;
        p0[2] = (color & 0xFFFFFF) | 0x02000000;
        p0[3] = LOW(temp.x);
        p0[4] = LOW(temp.w);
    }
    _cwc(p0);
    return 0;
}

static int _dws(RECT* rect, u_long* p) {
    RECT temp;
    int size;
    int n;
    int i;
    int isbg;

    temp.x = rect->x;
    temp.y = rect->y;
    temp.w = rect->w;
    temp.h = rect->h;
    isbg = 0;
    temp.w = CLAMP(temp.w, 0, 0x400 - 1);
    temp.h = CLAMP(temp.h, 0, (_type ? 0x400 : 0x200) - 1);
    if (temp.x >= 0x400) {
        temp.x = (temp.x - 0x400) / 2;
        isbg = 1;
    }
    size = ((temp.w * temp.h) + 1) / 2;
    i = size % 16;
    n = size / 16;
    set_alarm();
    while (!(*GPU_STATUS & 0x04000000)) {
        if (get_alarm()) {
            break;
        }
    }
    *GPU_DATA = 0x01000000;
    *GPU_DATA = isbg ? 0xB0000000 : 0xA0000000;
    *GPU_DATA = LOW(temp.x);
    *GPU_DATA = LOW(temp.w);
    while (i--) {
        *GPU_DATA = *p++;
    }
    if (n != 0) {
        *GPU_STATUS = 0x04000002;
        *DPCR |= 0x800;
        *DMA2_MADR = (u_long)p;
        *DMA2_BCR = (n << 0x10) | 0x10;
        *DMA2_CHCR = 0x01000201;
    }
    return 0;
}

static int _drs(RECT* rect, u_long* p) {
    RECT temp;
    int size;
    int n;
    int i;

    temp.x = rect->x;
    temp.y = rect->y;
    temp.w = rect->w;
    temp.h = rect->h;
    temp.w = CLAMP(temp.w, 0, 0x400 - 1);
    temp.h = CLAMP(temp.h, 0, (_type ? 0x400 : 0x200) - 1);
    size = ((temp.w * temp.h) + 1) / 2;
    i = size % 16;
    n = size / 16;
    if (size == 0) {
        return -1;
    }
    while (!(*GPU_STATUS & 0x04000000)) {
        if (get_alarm()) {
            break;
        }
    }
    *GPU_DATA = 0x01000000;
    *GPU_DATA = 0xC0000000;
    *GPU_DATA = LOW(temp.x);
    *GPU_DATA = LOW(temp.w);
    while (!(*GPU_STATUS & 0x08000000)) {
    }
    while (i--) {
        *p++ = *GPU_DATA;
    }
    if (n != 0) {
        *GPU_STATUS = 0x04000003;
        *DPCR |= 0x800;
        *DMA2_MADR = (u_long)p;
        *DMA2_BCR = (n << 0x10) | 0x10;
        *DMA2_CHCR = 0x01000200;
    }
    return 0;
}

static u_long ctlbuf[0x21];

static void _ctl(u_long cmd) {
    *GPU_STATUS = cmd;
    ctlbuf[cmd >> 0x18] = cmd & 0xFFFFFF;
}

static int _getctl(int n) { return (n << 0x18) | ctlbuf[n]; }

static int _cwb(u_long* p, int n) {
    while (n--) {
        *GPU_DATA = *p++;
    }
    return 0;
}

static void _cwc(u_long* p) {
    *GPU_STATUS = 0x04000002;
    *DPCR |= 0x800;
    *DMA2_MADR = (u_long)p;
    *DMA2_BCR = 0;
    *DMA2_CHCR = 0x01000401;
}

static int _param(int n) {
    *GPU_STATUS = 0x10000000 | n;
    return *GPU_DATA & 0xFFFFFF;
}

int gpu_param(int n) {
    *GPU_STATUS = 0x10000000 | n;
    return *GPU_DATA & 0xFFFFFF;
}

static void GPU_clearque(void) {
    _qout = 0;
    _qin = _qout;
}

static int _addque2(void (*func)(), void* param, int len, int arg) {
    memcpy((void*)que[_qin].buf, param, len);
    return _addque(func, (void*)que[_qin].buf, arg);
}

static int _addque(void (*func)(), void* param, int arg) {
    int next;

    next = (_qin + 1) & 0x3F;
    set_alarm();
    while (next == _qout) {
        if (get_alarm()) {
            return -1;
        }
    }
    que[_qin].func = func;
    que[_qin].param = param;
    que[_qin].arg = arg;
    _qin = next;
    if (!(*DMA2_CHCR & 0x01000000)) {
        _exeque();
    }
    return (_qin - _qout) & 0x3F;
}

static volatile int _lock = 0;

static int _exeque(void) {
    int ret;

    if (_lock) {
        return -1;
    }
    _lock = 1;
    set_alarm();
loop:
    if (*DMA2_CHCR & 0x01000000) {
        ret = 1;
        goto end;
    }
    if (_qin == _qout) {
        ret = 0;
        goto end;
    }
    while (!(*GPU_STATUS & 0x10000000)) {
        if (get_alarm()) {
            break;
        }
    }
    *GPU_STATUS = 0x04000000;
    que[_qout].func(que[_qout].param, que[_qout].arg);
    _qout = (_qout + 1) & 0x3F;
    goto loop;
end:
    _lock = 0;
    if (ret == 0 && _drawsync_cb) {
        _drawsync_cb();
    }
    return ret;
}

static int _reset(int mode) {
    switch (mode) {
    case 0:
        *DMA2_CHCR = 0x401;
        *GPU_STATUS = 0;
        _qout = 0;
        _qin = _qout;
        break;
    case 1:
        *DMA2_CHCR = 0x401;
        *GPU_STATUS = 0x04000000;
        *GPU_STATUS = 0x02000000;
        *GPU_STATUS = 0x01000000;
        _qout = 0;
        _qin = _qout;
        break;
    }
    *GPU_DATA = (*GPU_STATUS & 0x3FFF) | 0xE1001000;
    *GPU_DATA;
    return (*GPU_STATUS >> 0xC) & 1;
}

static int _sync(int mode) {
    volatile u_long* p;

    if (mode == 0) {
        set_alarm();
        while (_qin != _qout) {
            if (get_alarm()) {
                return -1;
            }
            if (!(*DMA2_CHCR & 0x01000000)) {
                _exeque();
            }
        }
        while (1) {
            if (get_alarm()) {
                return -1;
            }
            p = GPU_STATUS;
            if (!(*p & 0x60000000)) {
                if (*p & 0x10000000) {
                    break;
                }
            } else if (!(*DMA2_CHCR & 0x01000000)) {
                *p = 0x04000000;
                break;
            }
        }
        while (!(*GPU_STATUS & 0x04000000)) {
            if (get_alarm()) {
                return -1;
            }
        }
    } else {
        if (_qin != _qout) {
            return (_qin - _qout) & 0x3F;
        }
        if (!(*GPU_STATUS & 0x10000000) || (*GPU_STATUS & 0x04000000)) {
            return 1;
        }
        return 0;
    }
}

static int target;

static void set_alarm(void) {

    target = Vcount + 240;
}

static int get_alarm(void) {
    if (target < Vcount) {
        printf("GPU timeout:QUE=(%2d,%2d),CODE=(%d,%d,%08X)\n", _qin, _qout,
               (*GPU_STATUS >> 26) & 1, (*DMA2_CHCR >> 24) & 1, *DMA2_MADR);
        _reset(1);
        return -1;
    }
    return 0;
}

static void memcpy(void* dst, void* src, int n) {
    u_char* d = dst;
    u_char* s = src;

    while (n--) {
        *d++ = *s++;
    }
}

static void memset(void* ptr, int value, int num) {
    u_char* p = ptr;

    while (num--) {
        *p++ = value;
    }
}

#elif VERSION == 30

#define LOW(x) (*(s32*)&(x))

typedef struct {
     char* rcsid;
     void (*addque)(void (*)(), void*, int);
     int (*addque2)(void (*)(), void*, int, int);
     int (*clr)(RECT*, int);
     void (*ctl)(u_long);
     int (*cwb)(u_long*, int);
     void (*cwc)(u_long*);
     int (*drs)(RECT*, u_long*);
     int (*dws)(RECT*, u_long*);
     int (*exeque)(void);
     int (*getctl)(int);
     int (*otc)(u_long*, int);
     int (*param)(int);
     int (*reset)(int);
     u_long (*status)(void);
     int (*sync)(int);
} gpu;

typedef struct {
     void (*func)();
     void* param;
     int arg;
     u_long buf[0x10];
} QueueItem;

static void _addque(void (*func)(), void* param, int arg);
static int _addque2(void (*func)(), void* param, int len, int arg);
static int _clr(RECT* rect, int color);
static void _ctl(u_long cmd);
static int _cwb(u_long* p, int n);
static void _cwc(u_long* p);
static int _drs(RECT* rect, u_long* p);
static int _dws(RECT* rect, u_long* p);
static int _exeque(void);
static int _getctl(int n);
static int _otc(u_long* ot, int n);
static int _param(int n);
static int _reset(int mode);
static u_long _status(void);
static int _sync(int mode);
static u_long get_mode(int dfe, int dtd, int tpage);
static u_long get_cs(short x, short y);
static u_long get_ce(short x, short y);
static u_long get_ofs(short x, short y);
static u_long get_tw(RECT* tw);
static u_long get_dx(DISPENV* env);
static void set_alarm(void);
static int get_alarm(void);
static void memcpy(void* dst, void* src, int n);

int printf(const char*, ...);
void GPU_cw(u_long);
void ResetCallback(void);
int DMACallback(int, int (*)());
int CheckCallback(void);
void CheckPrim(char*, void*);
void DrawOTagIO(OT_TYPE*);
extern int Vcount;

static char rcsid[] = "$Id: sys.c,v 1.67 1995/03/13 08:48:19 suzu Exp $";

static void set_jtb(gpu* jtb) { GPU_cw((u_long)jtb & 0xFFFFFF); }

static gpu _jtb = {
    rcsid,   _addque, _addque2, _clr,    _ctl,    _cwb,   _cwc,    _drs,
    _dws,    _exeque, _getctl,  _otc,    _param,  _reset, _status, _sync,
};
static gpu* D_jtb = &_jtb;
int (*GPU_printf)(const char* fmt, ...) = printf;
static int _level = 0;
static int _type = 0;
static int _reverse = 0;
static void (*_drawsync_cb)() = NULL;

int ResetGraph(int mode) {
    switch (mode & 3) {
    case 0:
        ResetCallback();
        DMACallback(2, D_jtb->exeque);
        set_jtb(D_jtb);
        _type = D_jtb->reset(0);
        break;
    case 1:
    case 2:
        _type = D_jtb->reset(1);
        break;
    case 3:
        _type = D_jtb->reset(1);
        DMACallback(2, D_jtb->exeque);
        break;
    default:
        return -1;
    }
    D_jtb->ctl(D_jtb->getctl(8) | (_reverse ? 0x08000080 : 0x08000000));
    if (mode & 8) {
        _type = 2;
        D_jtb->ctl(_reverse ? 0x20000501 : 0x20000504);
    }
}

int SetGraphReverse(int mode) {
    int old = _reverse;

    _reverse = mode;
    D_jtb->ctl(D_jtb->getctl(8) | (_reverse ? 0x08000080 : 0x08000000));
    if (_type == 2) {
        D_jtb->ctl(_reverse ? 0x20000501 : 0x20000504);
    }
    return old;
}

int SetGraphDebug(int level) {
    int old = _level;

    _level = level;
    if (level) {
        GPU_printf("SetGraphDebug:level:%d,type:%d reverse:%d\n", level,
                   _type, _reverse);
    }
    return old;
}

int GetGraphType(void) { return _type; }

int GetGraphDebug(void) { return _level; }

u_long DrawSyncCallback(void (*func)()) {
    void (*old)() = _drawsync_cb;

    _drawsync_cb = func;
    return (u_long)old;
}

void SetDispMask(int mask) { D_jtb->ctl(mask ? 0x03000000 : 0x03000001); }

int DrawSync(int mode) { return D_jtb->sync(mode); }

int ClearImage(RECT* rect, u_char r, u_char g, u_char b) {
    return D_jtb->addque2(
        D_jtb->clr, rect, sizeof(RECT), (b << 0x10) | (g << 8) | r);
}

int LoadImage(RECT* rect, u_long* p) {
    return D_jtb->addque2(D_jtb->dws, rect, sizeof(RECT), p);
}

int StoreImage(RECT* rect, u_long* p) {
    return D_jtb->addque2(D_jtb->drs, rect, sizeof(RECT), p);
}

int MoveImage(RECT* rect, int x, int y) {
    u_long param[5];

    param[0] = 0x04FFFFFF;
    param[1] = 0x80000000;
    param[2] = LOW(rect->x);
    param[3] = y << 0x10 | (u_short)x;
    param[4] = LOW(rect->w);
    return D_jtb->addque2(D_jtb->cwc, param, sizeof(param), 0);
}

OT_TYPE* ClearOTag(OT_TYPE* ot, int n) {
    while (--n) {
        setlen(ot, 0);
        setaddr(ot, ot + 1);
        ot++;
    }
    *ot = 0xFFFFFF;
    return ot;
}

OT_TYPE* ClearOTagR(OT_TYPE* ot, int n) {
    D_jtb->otc(ot, n);
    return ot;
}

static DRAWENV c_draw;
static DISPENV c_disp;

void DrawPrim(void* p) {
    int len = getlen(p);

    if (_level) {
        CheckPrim("DrawPrim", p);
    }
    D_jtb->sync(0);
    D_jtb->cwb((u_long*)&((P_TAG*)p)->r0, len);
}

void DrawOTag(OT_TYPE* p) {
    if (_level) {
        DrawOTagIO(p);
    } else {
        D_jtb->addque(D_jtb->cwc, p, 0);
    }
}

DRAWENV* PutDrawEnv(DRAWENV* env) {
    SetDrawEnv(&env->dr_env, env);
    env->dr_env.tag |= 0xFFFFFF;
    D_jtb->addque(D_jtb->cwc, &env->dr_env, 0);
    c_draw = *env;
    return env;
}

DRAWENV* GetDrawEnv(DRAWENV* env) {
    *env = c_draw;
    return env;
}

#define CLAMP(value, low, high)                                                \
    value < low ? low : (value > high ? high : value)

DISPENV* PutDispEnv(DISPENV* env) {
    int h_start;
    int v_start;
    int cmd;
    int v_end;
    int h_end;

    cmd = 0x08000000;
    D_jtb->ctl(0x05000000 | (_type ? ((env->disp.y & 0xFFF) << 0xC) |
                                         (get_dx(env) & 0xFFF)
                                   : ((env->disp.y & 0x1FF) << 0xA) |
                                         (env->disp.x & 0x3FF)));
    h_start = env->screen.x * 0xA + 0x260;
    v_start = env->screen.y + 0x10;
    h_end = h_start + (env->screen.w ? env->screen.w * 0xA : 0xA00);
    v_end = v_start + (env->screen.h ? env->screen.h : 0xF0);
    h_start = CLAMP(h_start, 0x1F4, 0xCDA);
    h_end = CLAMP(h_end, h_start + 0x50, 0xCDA);
    v_start = CLAMP(v_start, 0, (env->pad0 ? 0x136 : 0xFE));
    v_end = CLAMP(v_end, v_start + 1, (env->pad0 ? 0x137 : 0xFF));
    D_jtb->ctl(0x06000000 | ((h_end & 0xFFF) << 0xC) | (h_start & 0xFFF));
    D_jtb->ctl(0x07000000 | ((v_end & 0x3FF) << 0xA) | (v_start & 0x3FF));
    if (env->pad0 == 1) {
        cmd |= 0x8;
    }
    if (env->isrgb24) {
        cmd |= 0x10;
    }
    if (env->isinter) {
        cmd |= 0x20;
    }
    if (_reverse) {
        cmd |= 0x80;
    }
    if (env->disp.w > 0x118) {
        if (env->disp.w <= 0x160) {
            cmd |= 1;
        } else if (env->disp.w <= 0x190) {
            cmd |= 0x40;
        } else if (env->disp.w <= 0x230) {
            cmd |= 2;
        } else {
            cmd |= 3;
        }
    }
    if (env->disp.h <= (env->pad0 ? 0x120 : 0x100)) {
    } else {
        cmd |= 0x24;
    }
    D_jtb->ctl(cmd);
    c_disp = *env;
    return env;
}

DISPENV* GetDispEnv(DISPENV* env) {
    *env = c_disp;
    return env;
}

void SetTexWindow(DR_TWIN* p, RECT* tw) {
    setlen(p, 2);
    p->code[0] = get_tw(tw);
    p->code[1] = 0;
}

void SetDrawArea(DR_AREA* p, RECT* r) {
    setlen(p, 2);
    p->code[0] = get_cs(r->x, r->y);
    p->code[1] = get_ce(r->x + r->w - 1, r->y + r->h - 1);
}

void SetDrawOffset(DR_OFFSET* p, u_short* ofs) {
    setlen(p, 2);
    p->code[0] = get_ofs(ofs[0], ofs[1]);
    p->code[1] = 0;
}

void SetPriority(DR_PRIO* p, int pbc, int pbw) {
    setlen(p, 2);
    p->code[0] = 0xE6000000 | (pbc ? 2 : 0) | (pbw ? 1 : 0);
    p->code[1] = 0;
}

void SetDrawMode(DR_MODE* p, int dfe, int dtd, int tpage, RECT* tw) {
    setlen(p, 2);
    p->code[0] = get_mode(dfe, dtd, tpage);
    p->code[1] = get_tw(tw);
}

void SetDrawEnv(DR_ENV* dr_env_in, DRAWENV* env) {
    RECT clip_rect;
    int offset;
    DR_ENV* dr_env;

    dr_env = dr_env_in;

    dr_env->code[0] = get_cs(env->clip.x, env->clip.y);
    dr_env->code[1] =
        get_ce(env->clip.w + env->clip.x - 1, env->clip.y + env->clip.h - 1);
    dr_env->code[2] = get_ofs(env->ofs[0], env->ofs[1]);
    dr_env->code[3] = get_mode(env->dfe, env->dtd, env->tpage);
    dr_env->code[4] = get_tw(&env->tw);
    dr_env->code[5] = 0xE6000000;

    offset = 7;
    if (env->isbg != 0) {
        clip_rect.x = env->clip.x;
        clip_rect.y = env->clip.y;
        clip_rect.w = env->clip.w;
        clip_rect.h = env->clip.h;
        clip_rect.w = CLAMP(clip_rect.w, 0, 0x400 - 1);
        clip_rect.h = CLAMP(clip_rect.h, 0, (_type ? 0x400 : 0x200) - 1);

        if ((clip_rect.x & 0x3F) || (clip_rect.w & 0x3F)) {
            clip_rect.x -= env->ofs[0];
            clip_rect.y -= env->ofs[1];
            *((int*)dr_env + offset++) =
                0x60000000 | env->b0 << 0x10 | env->g0 << 8 | env->r0;
            *((int*)dr_env + offset++) = LOW(clip_rect.x);
            *((int*)dr_env + offset++) = LOW(clip_rect.w);
            clip_rect.x += env->ofs[0];
            clip_rect.y += env->ofs[1];
        } else {
            *((int*)dr_env + offset++) =
                0x02000000 | env->b0 << 0x10 | env->g0 << 8 | env->r0;
            *((int*)dr_env + offset++) = LOW(clip_rect.x);
            *((int*)dr_env + offset++) = LOW(clip_rect.w);
        }
    }

    setlen(dr_env, offset - 1);
}

static u_long get_mode(int dfe, int dtd, int tpage) {
    if (_type) {
        return 0xE1000000 | (dtd ? 0x800 : 0) | (dfe ? 0x1000 : 0) |
               (tpage & 0x27FF);
    } else {
        return 0xE1000000 | (dtd ? 0x200 : 0) | (dfe ? 0x400 : 0) |
               (tpage & 0x1FF);
    }
}

static u_long get_cs(short x, short y) {
    x = CLAMP(x, 0, 0x400 - 1);
    y = CLAMP(y, 0, (_type ? 0x400 : 0x200) - 1);
    if (_type) {
        return 0xE3000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE3000000 | ((y & 0x3FF) << 10) | (x & 0x3FF);
    }
}

static u_long get_ce(short x, short y) {
    x = CLAMP(x, 0, 0x400 - 1);
    y = CLAMP(y, 0, (_type ? 0x400 : 0x200) - 1);
    if (_type) {
        return 0xE4000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE4000000 | ((y & 0x1FF) << 10) | (x & 0x3FF);
    }
}

static u_long get_ofs(short x, short y) {
    if (_type) {
        return 0xE5000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE5000000 | ((y & 0x7FF) << 11) | (x & 0x7FF);
    }
}

static u_long get_tw(RECT* tw) {
    u_long pad[4];

    if (tw != NULL) {
        pad[0] = (tw->x & 0xFF) >> 3;
        pad[2] = (-tw->w & 0xFF) >> 3;
        pad[1] = (tw->y & 0xFF) >> 3;
        pad[3] = (-tw->h & 0xFF) >> 3;
        return 0xE2000000 | (pad[1] << 0xF) | (pad[0] << 0xA) | (pad[3] << 5) |
               pad[2];
    }
    return 0;
}

static u_long get_dx(DISPENV* env) {
    switch (_type) {
    case 0:
        return env->disp.x;
    case 1:
        return _reverse ? 0x400 - env->disp.x - env->disp.w : env->disp.x;
    case 2:
        return _reverse ? 0x400 - env->disp.x - env->disp.w / 2
                        : env->disp.x / 2;
    }
}

static volatile u_long* GPU_DATA = (u_long*)0x1F801810;
static volatile u_long* GPU_STATUS = (u_long*)0x1F801814;
static volatile u_long* DMA2_MADR = (u_long*)0x1F8010A0;
static volatile u_long* DMA2_BCR = (u_long*)0x1F8010A4;
static volatile u_long* DMA2_CHCR = (u_long*)0x1F8010A8;
static volatile u_long* DMA6_MADR = (u_long*)0x1F8010E0;
static volatile u_long* DMA6_BCR = (u_long*)0x1F8010E4;
static volatile u_long* DMA6_CHCR = (u_long*)0x1F8010E8;
static volatile u_long* DPCR = (u_long*)0x1F8010F0;

static u_long _status(void) { return *GPU_STATUS; }

static int _otc(u_long* ot, int n) {
    *DPCR |= 0x08000000;
    *DMA6_CHCR = 0;
    *DMA6_MADR = (u_long)&ot[n - 1];
    *DMA6_BCR = n;
    *DMA6_CHCR = 0x11000002;
    set_alarm();
    while (*DMA6_CHCR & 0x01000000) {
        if (get_alarm()) {
            return -1;
        }
    }
    return n;
}

static int _clr(RECT* rect, int color) {
    static u_long p0[8];
    static u_long p1[8];
    RECT temp;

    temp.x = rect->x;
    temp.y = rect->y;
    temp.w = rect->w;
    temp.h = rect->h;
    temp.w = CLAMP(temp.w, 0, 0x400 - 1);
    temp.h = CLAMP(temp.h, 0, (_type ? 0x400 : 0x200) - 1);
    if ((temp.x & 0x3F) || (temp.w & 0x3F)) {
        p0[0] = ((u_long)p1 & 0xFFFFFF) | 0x07000000;
        p0[1] = 0xE3000000;
        p0[2] = 0xE4FFFFFF;
        p0[3] = 0xE5000000;
        p0[4] = 0xE6000000;
        p0[5] = (color & 0xFFFFFF) | 0x60000000;
        p0[6] = LOW(temp.x);
        p0[7] = LOW(temp.w);
        p1[0] = 0x03FFFFFF;
        p1[1] = _param(3) | 0xE3000000;
        p1[2] = _param(4) | 0xE4000000;
        p1[3] = _param(5) | 0xE5000000;
    } else {
        p0[0] = 0x04FFFFFF;
        p0[1] = 0xE6000000;
        p0[2] = (color & 0xFFFFFF) | 0x02000000;
        p0[3] = LOW(temp.x);
        p0[4] = LOW(temp.w);
    }
    _cwc(p0);
    return 0;
}

static int _dws(RECT* rect, u_long* p) {
    RECT temp;
    int size;
    int n;
    int i;
    int isbg;

    set_alarm();
    temp.x = rect->x;
    temp.y = rect->y;
    temp.w = rect->w;
    temp.h = rect->h;
    isbg = 0;
    temp.w = CLAMP(temp.w, 0, 0x400 - 1);
    temp.h = CLAMP(temp.h, 0, (_type ? 0x400 : 0x200) - 1);
    if (temp.x >= 0x400) {
        isbg = 1;
        temp.x = (temp.x - 0x400) / 2;
    }
    size = ((temp.w * temp.h) + 1) / 2;
    if (size <= 0) {
        return -1;
    }
    i = size % 16;
    n = size / 16;
    while (!(*GPU_STATUS & 0x04000000)) {
        if (get_alarm()) {
            return -1;
        }
    }
    *GPU_STATUS = 0x04000000;
    *GPU_DATA = 0x01000000;
    *GPU_DATA = isbg ? 0xB0000000 : 0xA0000000;
    *GPU_DATA = LOW(temp.x);
    *GPU_DATA = LOW(temp.w);
    while (i--) {
        *GPU_DATA = *p++;
    }
    if (n != 0) {
        *GPU_STATUS = 0x04000002;
        *DMA2_MADR = (u_long)p;
        *DMA2_BCR = (n << 0x10) | 0x10;
        *DMA2_CHCR = 0x01000201;
    }
    return 0;
}

static int _drs(RECT* rect, u_long* p) {
    RECT temp;
    int size;
    int n;
    int i;

    set_alarm();
    temp.x = rect->x;
    temp.y = rect->y;
    temp.w = rect->w;
    temp.h = rect->h;
    temp.w = CLAMP(temp.w, 0, 0x400 - 1);
    temp.h = CLAMP(temp.h, 0, (_type ? 0x400 : 0x200) - 1);
    size = ((temp.w * temp.h) + 1) / 2;
    if (size <= 0) {
        return -1;
    }
    i = size % 16;
    n = size / 16;
    while (!(*GPU_STATUS & 0x04000000)) {
        if (get_alarm()) {
            return -1;
        }
    }
    *GPU_STATUS = 0x04000000;
    *GPU_DATA = 0x01000000;
    *GPU_DATA = 0xC0000000;
    *GPU_DATA = LOW(temp.x);
    *GPU_DATA = LOW(temp.w);
    while (!(*GPU_STATUS & 0x08000000)) {
        if (get_alarm()) {
            return -1;
        }
    }
    while (i--) {
        *p++ = *GPU_DATA;
    }
    if (n != 0) {
        *GPU_STATUS = 0x04000003;
        *DMA2_MADR = (u_long)p;
        *DMA2_BCR = (n << 0x10) | 0x10;
        *DMA2_CHCR = 0x01000200;
    }
    return 0;
}

static u_long ctlbuf[0x21];

static void _ctl(u_long cmd) {
    *GPU_STATUS = cmd;
    ctlbuf[cmd >> 0x18] = cmd & 0xFFFFFF;
}

static int _getctl(int n) { return (n << 0x18) | ctlbuf[n]; }

static int _cwb(u_long* p, int n) {
    *GPU_STATUS = 0x04000000;
    while (n--) {
        *GPU_DATA = *p++;
    }
    return 0;
}

static void _cwc(u_long* p) {
    *GPU_STATUS = 0x04000002;
    *DMA2_MADR = (u_long)p;
    *DMA2_BCR = 0;
    *DMA2_CHCR = 0x01000401;
}

static int _param(int n) {
    *GPU_STATUS = 0x10000000 | n;
    return *GPU_DATA & 0xFFFFFF;
}

static volatile int _qin = 0;
static volatile int _qout = 0;
static volatile int _lock = 0;
static volatile QueueItem que[0x40];

static void _addque(void (*func)(), void* param, int arg) {
    _addque2(func, param, 0, arg);
}

static int _addque2(void (*func)(), void* param, int len, int arg) {
    int next;
    int n;

    set_alarm();
    next = (_qin + 1) & 0x3F;
    while (next == _qout && que[_qout].func != NULL) {
        if (_lock && CheckCallback()) {
            return -1;
        }
        if (get_alarm()) {
            return -1;
        }
        _exeque();
    }
    n = _qin;
    que[n].func = NULL;
    _qin = next;
    if (len) {
        memcpy((void*)que[n].buf, param, len);
        que[n].param = (void*)que[n].buf;
    } else {
        que[n].param = param;
    }
    que[n].arg = arg;
    que[n].func = func;
    _exeque();
    return (_qin - _qout) & 0x3F;
}

static int _exeque(void) {
    set_alarm();
    while (1) {
        if (_qin == _qout) {
            if (_drawsync_cb) {
                _drawsync_cb();
            }
            return 0;
        }
        if (_lock) {
            return 1;
        }
        _lock++;
        if (*DMA2_CHCR & 0x01000000) {
            if (_lock > 0) {
                _lock--;
            }
            return 2;
        }
        while (!(*GPU_STATUS & 0x10000000)) {
            if (get_alarm()) {
                return -1;
            }
        }
        if (que[_qout].func != NULL) {
            que[_qout].func(que[_qout].param, que[_qout].arg);
            que[_qout].func = NULL;
            _qout = (_qout + 1) & 0x3F;
            if (_lock > 0) {
                _lock--;
            }
        } else {
            if (_lock > 0) {
                _lock--;
            }
            return 3;
        }
    }
}

static int _reset(int mode) {
    int i;

    _lock++;
    _qout = 0;
    _qin = _qout;
    for (i = 0; i < 0x40; i++) {
        que[i].func = NULL;
    }
    switch (mode) {
    case 0:
        *DMA2_CHCR = 0x401;
        *DPCR |= 0x800;
        *GPU_STATUS = 0;
        break;
    case 1:
        *DMA2_CHCR = 0x401;
        *DPCR |= 0x800;
        *GPU_STATUS = 0x02000000;
        *GPU_STATUS = 0x01000000;
        break;
    }
    *GPU_DATA = (*GPU_STATUS & 0x3FFF) | 0xE1001000;
    _lock = 0;
    return (*GPU_STATUS >> 0xC) & 1;
}

static int _sync(int mode) {
    if (mode == 0) {
        set_alarm();
        while (_qin != _qout) {
            _exeque();
            if (get_alarm()) {
                return -1;
            }
        }
        while (*DMA2_CHCR & 0x01000000) {
            if (get_alarm()) {
                return -1;
            }
        }
        while (!(*GPU_STATUS & 0x04000000)) {
            if (get_alarm()) {
                return -1;
            }
        }
    } else {
        if (_qin != _qout) {
            return (_qin - _qout) & 0x3F;
        }
        if (!(*GPU_STATUS & 0x10000000) || (*GPU_STATUS & 0x04000000)) {
            return 1;
        }
        return 0;
    }
}

static int target;

static void set_alarm(void) {

    target = Vcount + 240;
}

static int get_alarm(void) {
    if (target < Vcount) {
        printf("GPU timeout:QUE=(%2d,%2d),CODE=(%d,%d,%08X)\n", _qin, _qout,
               (*GPU_STATUS >> 26) & 1, (*DMA2_CHCR >> 24) & 1, *DMA2_MADR);
        _reset(1);
        return -1;
    }
    return 0;
}

static void memcpy(void* dst, void* src, int n) {
    u_char* d = dst;
    u_char* s = src;

    while (n--) {
        *d++ = *s++;
    }
}

static void memset(void* ptr, int value, int num) {
    u_char* p = ptr;

    while (num--) {
        *p++ = value;
    }
}

#elif VERSION == 31

#ifndef LOW
#define LOW(x) (*(s32*)&(x))
#endif

typedef struct {
     const char* rcsid;
     void (*addque)(void (*)(int*, int), int*, int);
     int (*addque2)(void (*)(int*, int), int*, int, int);
     int (*clr)(RECT*, int);
     void (*ctl)(u_long);
     int (*cwb)(int*, int);
     void (*cwc)(int);
     int (*drs)(RECT*, int*);
     int (*dws)(RECT*, int*);
     int (*exeque)(void);
     int (*getctl)(int);
     int (*otc)(int, int);
     int (*param)(int);
     int (*reset)(int);
     u_long (*status)(void);
     int (*sync)(int);
} gpu;

struct QueueItem {
    void (*unk0)(int*, int);
    int* unk4;
    int unk8;
    int unk[0x10];
};

void _addque(void (*)(int*, int), int*, int);
int _addque2(void (*)(int*, int), int*, int, int);
int _clr(RECT*, int);
void _ctl(u_long);
int _cwb(int*, int);
void _cwc(int);
int _drs(RECT*, int*);
int _dws(RECT*, int*);
int _exeque(void);
int _getctl(int);
int _otc(int, int);
int _param(int);
int _reset(int);
u_long _status(void);
int _sync(int);

int printf(char*, ...);
u_long get_ce(short, short);
u_long get_cs(short, short);
u_long get_tw(RECT* tw);
u_long get_ofs(short, short);

static int D_80037E20[0x10];
static DRAWENV D_80037E60;
static DISPENV D_80037EBC;
static int ctlbuf[0x21];
static volatile struct QueueItem D_80037F54[0x40];
static int D_80039254;
static int D_80039258;

static gpu D_8002C220 = {
    .rcsid = "$Id: sys.c,v 1.83 1995/05/25 13:43:27 suzu Exp $",
    .addque = _addque,
    .addque2 = _addque2,
    .clr = _clr,
    .ctl = _ctl,
    .cwb = _cwb,
    .cwc = _cwc,
    .drs = _drs,
    .dws = _dws,
    .exeque = _exeque,
    .getctl = _getctl,
    .otc = _otc,
    .param = _param,
    .reset = _reset,
    .status = _status,
    .sync = _sync,
};
static gpu* D_8002C260 = &D_8002C220;
void (*GPU_printf)(const char* fmt, ...) = printf;
static int D_8002C268 = 0;
static int D_8002C26C = 0;
static int D_8002C270 = 0;
static int D_8002C274 = 1;
static void (*D_8002C278)() = NULL;
static volatile int* GPU_DATA = (int*)0x1F801810;
static volatile u_long* GPU_STATUS = (u_long*)0x1F801814;
static volatile int* DMA2_MADR = (int*)0x1F8010A0;
static volatile int* DMA2_BCR = (int*)0x1F8010A4;
static volatile int* DMA2_CHCR = (int*)0x1F8010A8;
static volatile int* DMA6_MADR = (int*)0x1F8010E0;
static volatile int* DMA6_BCR = (int*)0x1F8010E4;
static volatile int* DMA6_CHCR = (int*)0x1F8010E8;
static volatile int* DPCR = (int*)0x1F8010F0;
static volatile int _qin = 0;
static volatile int _qout = 0;

#ifndef CLAMP
#define CLAMP(value, low, high)                                                \
    value < low ? low : (value > high ? high : value)
#endif

#define CMD_CLEAR_CACHE 0x01000000
#define CMD_FILL_RECTANGLE_IN_VRAM(color) ((color & 0xFFFFFF) | 0x02000000)
#define CMD_MONOCHROME_RECTANGLE(color) ((color & 0xFFFFFF) | 0x60000000)
#define CMD_COPY_VRAM_TO_CPU 0xC0000000
#define CMD_COPY_CPU_TO_VRAM 0xA0000000

#define CMD_RESET_GPU 0x00000000
#define CMD_RESET_CMD_BUF 0x01000000
#define CMD_ACK_GPU_INTR 0x02000000
#define CMD_DISP_ENABLE 0x03000000
#define CMD_DATA_REQUEST 0x04000000
#define CMD_SET_DISP_AREA_START 0x05000000
#define CMD_SET_H_DISP_RANGE 0x06000000
#define CMD_SET_V_DISP_RANGE 0x07000000
#define CMD_SET_DISP_MODE 0x08000000
#define CMD_GPU_READ_REG 0x10000000
#define CMD_SET_VRAM_SIZE 0x20000000

#define DATA_REQ_OFF 0
#define DATA_REQ_FIFO 1
#define DATA_REQ_CPU_TO_GPU 2
#define DATA_REQ_GPU_TO_CPU 3

#define STATUS_READY_TO_RECEIVE_CMD (1 << 26)
#define STATUS_READY_TO_SEND_VRAM_TO_CPU (1 << 27)

int ResetGraph(int level) {
    if (D_8002C268 >= 2) {
        GPU_printf("ResetGraph(%d)...\n", level);
    }
    switch (level & 3) {
    case 0:
        ResetCallback();
        D_8002C26C = D_8002C260->reset(0);
        GPU_cw((int)D_8002C260 & 0xFFFFFF);
        break;
    case 1:
    case 2:
    case 3:
        D_8002C26C = D_8002C260->reset(1);
        break;
    default:
        return -1;
    }

    memset2(&D_80037E60, -1, sizeof(DRAWENV));
    memset2(&D_80037EBC, -1, sizeof(DISPENV));
    if (D_8002C26C != 0) {
        D_8002C260->ctl(CMD_SET_DISP_MODE | (D_8002C270 ? 0x80 : 0) |
                        D_8002C260->getctl(8));
        if (level & 8) {
            D_8002C26C = 2;
            D_8002C260->ctl(CMD_SET_VRAM_SIZE | (D_8002C270 ? 0x501 : 0x504));
        }
    }
}

int SetGraphReverse(int mode) {
    int old = D_8002C270;

    if (D_8002C268 >= 2) {
        GPU_printf("SetGraphReverse(%d)...\n", mode);
    }

    D_8002C270 = mode;
    D_8002C260->ctl(
        CMD_SET_DISP_MODE | (D_8002C270 ? 0x80 : 0) | D_8002C260->getctl(8));
    if (D_8002C26C == 2) {
        D_8002C260->ctl(CMD_SET_VRAM_SIZE | (D_8002C270 ? 0x501 : 0x504));
    }
    return old;
}

int SetGraphDebug(int level) {
    int old = D_8002C268;

    D_8002C268 = level;
    if (level) {
        GPU_printf("SetGraphDebug:level:%d,type:%d reverse:%d\n", level,
                   D_8002C26C, D_8002C270);
    }

    return old;
}

int SetGraphQueue(int mode) {
    int old = D_8002C274;

    if (D_8002C268 >= 2) {
        GPU_printf("SetGraphQueue(%d)...\n", mode);
    }
    if (mode != D_8002C274) {
        D_8002C260->reset(1);
        D_8002C274 = mode;
    }

    return old;
}

int GetGraphType(void) { return D_8002C26C; }

int GetGraphDebug(void) { return D_8002C268; }

u_long DrawSyncCallback(void (*func)()) {
    void (*old)();

    if (D_8002C268 >= 2) {
        GPU_printf("DrawSyncCallback(%08x)...\n", func);
    }
    old = D_8002C278;
    D_8002C278 = func;
    return old;
}

void SetDispMask(int mask) {
    if (D_8002C268 >= 2) {
        GPU_printf("SetDispMask(%d)...\n", mask);
    }
    D_8002C260->ctl(mask ? (CMD_DISP_ENABLE | 0) : (CMD_DISP_ENABLE | 1));
}

int DrawSync(int mode) {
    if (D_8002C268 >= 2) {
        GPU_printf("DrawSync(%d)...\n", mode);
    }
    D_8002C260->sync(mode);
}

void checkRECT(const char* log, RECT* r) {
    switch (D_8002C268) {
    case 1:
        if (r->w > 0x400 || r->w + r->x > 0x400 || r->y > 0x400 ||
            r->y + r->h > 0x400 || r->w <= 0 || r->x < 0 || r->y < 0 ||
            r->h <= 0) {
            GPU_printf("%s:bad RECT", log);
            GPU_printf("(%d,%d)-(%d,%d)\n", r->x, r->y, r->w, r->h);
        }
        break;
    case 2:
        GPU_printf("%s:", log);
        GPU_printf("(%d,%d)-(%d,%d)\n", r->x, r->y, r->w, r->h);
        break;
    }
}

int ClearImage(RECT* rect, u8 r, u8 g, u8 b) {
    checkRECT("ClearImage", rect);
    return D_8002C260->addque2(
        D_8002C260->clr, rect, sizeof(RECT), (b << 0x10) | (g << 8) | r);
}

int LoadImage(RECT* rect, u_long* p) {
    checkRECT("LoadImage", rect);
    return D_8002C260->addque2(D_8002C260->dws, rect, sizeof(RECT), p);
}

int StoreImage(RECT* rect, u_long* p) {
    checkRECT("StoreImage", rect);
    return D_8002C260->addque2(D_8002C260->drs, rect, sizeof(RECT), p);
}

int MoveImage(RECT* rect, int x, int y) {
    u_long param[5];

    checkRECT("MoveImage", rect);
    param[0] = 0x04FFFFFF;
    param[1] = 0x80000000;
    param[2] = LOW(rect->x);
    param[3] = (u16)y << 0x10 | (u16)x;
    param[4] = LOW(rect->w);
    return D_8002C260->addque2(D_8002C260->cwc, param, sizeof(param), 0);
}

OT_TYPE* ClearOTag(OT_TYPE* ot, int n) {
    if (D_8002C268 >= 2) {
        GPU_printf("ClearOTag(%08x,%d)...\n", ot, n);
    }

    while (--n) {
        setlen(ot, 0);
        setaddr(ot, ot + 1);
        ot++;
    }

    *ot = 0xFFFFFF;
    return ot;
}

OT_TYPE* ClearOTagR(OT_TYPE* ot, int n) {
    if (D_8002C268 >= 2) {
        GPU_printf("ClearOTagR(%08x,%d)...\n", ot, n);
    }

    D_8002C260->otc(ot, n);
    return ot;
}

void DrawPrim(void* p) {
    int len = getlen(p);
    D_8002C260->sync(0);
    D_8002C260->cwb((u_long*)&((P_TAG*)p)->r0, len);
}

void DrawOTag(OT_TYPE* p) {
    if (D_8002C268 >= 2) {
        GPU_printf("DrawOTag(%08x)...\n", p);
    }
    D_8002C260->addque(D_8002C260->cwc, p, 0);
}

DRAWENV* PutDrawEnv(DRAWENV* env) {
    if (D_8002C268 >= 2) {
        GPU_printf("PutDrawEnv(%08x)...\n", env);
    }

    SetDrawEnv(&env->dr_env, env);
    env->dr_env.tag |= 0xFFFFFF;
    D_8002C260->addque2(D_8002C260->cwc, &env->dr_env, sizeof(DR_ENV), 0);
    D_80037E60 = *env;

    return env;
}

DRAWENV* GetDrawEnv(DRAWENV* env) {
    *env = D_80037E60;
    return env;
}

DISPENV* PutDispEnv(DISPENV* env) {
    int h_start;
    int v_start;
    int cmd;
    int v_end;
    int h_end;

    cmd = CMD_SET_DISP_MODE;
    if (D_8002C268 >= 2) {
        GPU_printf("PutDispEnv(%08x)...\n", env);
    }
    D_8002C260->ctl(
        CMD_SET_DISP_AREA_START |
        (D_8002C26C ? ((env->disp.y & 0xFFF) << 0xC) | (get_dx(env) & 0xFFF)
                    : ((env->disp.y & 0x1FF) << 0xA) | (env->disp.x & 0x3FF)));
    if (!(LOW(D_80037EBC.screen.x) == LOW(env->screen.x) &&
          LOW(D_80037EBC.screen.w) == LOW(env->screen.w))) {
        env->pad0 = GetVideoMode();
        h_start = env->screen.x * 0xA + 0x260;
        v_start = env->screen.y + 0x10;
        h_end = h_start + (env->screen.w ? env->screen.w * 0xA : 0xA00);
        v_end = v_start + (env->screen.h ? env->screen.h : 0xF0);
        h_start = CLAMP(h_start, 0x1F4, 0xCDA);
        h_end = CLAMP(h_end, h_start + 0x50, 0xCDA);
        v_start = CLAMP(v_start, 0, (env->pad0 ? 0x136 : 0xFE));
        v_end = CLAMP(v_end, v_start + 1, (env->pad0 ? 0x138 : 0x100));
        D_8002C260->ctl(CMD_SET_H_DISP_RANGE | ((h_end & 0xFFF) << 0xC) |
                        (h_start & 0xFFF));
        D_8002C260->ctl(CMD_SET_V_DISP_RANGE | ((v_end & 0x3FF) << 0xA) |
                        (v_start & 0x3FF));
    }
    if (LOW(D_80037EBC.isinter) != LOW(env->isinter) ||
        !(LOW(D_80037EBC.disp.x) == LOW(env->disp.x) &&
          LOW(D_80037EBC.disp.w) == LOW(env->disp.w))) {
        env->pad0 = GetVideoMode();
        if (env->pad0 == 1) {
            cmd |= 0x8;
        }
        if (env->isrgb24) {
            cmd |= 0x10;
        }
        if (env->isinter) {
            cmd |= 0x20;
        }
        if (D_8002C270 != 0) {
            cmd |= 0x80;
        }
        if (env->disp.w > 0x118) {
            if (env->disp.w <= 0x160) {
                cmd |= 1;
            } else if (env->disp.w <= 0x190) {
                cmd |= 0x40;
            } else if (env->disp.w <= 0x230) {
                cmd |= 2;
            } else {
                cmd |= 3;
            }
        }
        if (env->disp.h <= (env->pad0 ? 0x120 : 0x100)) {
        } else {
            cmd |= 0x24;
        }
        D_8002C260->ctl(cmd);
    }
    memcpy((u8*)&D_80037EBC, (u8*)env, sizeof(DISPENV));
    return env;
}

DISPENV* GetDispEnv(DISPENV* env) {
    *env = D_80037EBC;
    return env;
}

u_long GetODE(void) { return D_8002C260->status() >> 0x1F; }

void SetTexWindow(DR_TWIN* p, RECT* tw) {
    setlen(p, 2);
    p->code[0] = get_tw(tw);
    p->code[1] = 0;
}

void SetDrawArea(DR_AREA* p, RECT* r) {
    setlen(p, 2);
    p->code[0] = get_cs(r->x, r->y);
    p->code[1] = get_ce(r->x + r->w - 1, r->y + r->h - 1);
}

void SetDrawOffset(DR_OFFSET* p, u_short* ofs) {
    setlen(p, 2);
    p->code[0] = get_ofs(ofs[0], ofs[1]);
    p->code[1] = 0;
}

void SetPriority(DR_PRIO* p, int pbc, int pbw) {
    setlen(p, 2);
    p->code[0] = 0xE6000000 | (pbc ? 2 : 0) | (pbw ? 1 : 0);
    p->code[1] = 0;
}

void SetDrawMode(DR_MODE* p, int dfe, int dtd, int tpage, RECT* tw) {
    setlen(p, 2);
    p->code[0] = get_mode(dfe, dtd, (u_short)tpage);
    p->code[1] = get_tw(tw);
}

void SetDrawEnv(DR_ENV* dr_env_in, DRAWENV* env) {
    RECT clip_rect;
    int offset;
    DR_ENV* dr_env;

    dr_env = dr_env_in;

    dr_env->code[0] = get_cs(env->clip.x, env->clip.y);
    dr_env->code[1] =
        get_ce(env->clip.w + env->clip.x - 1, env->clip.y + env->clip.h - 1);
    dr_env->code[2] = get_ofs(env->ofs[0], env->ofs[1]);
    dr_env->code[3] = get_mode(env->dfe, env->dtd, env->tpage);
    dr_env->code[4] = get_tw(&env->tw);
    dr_env->code[5] = 0xE6000000;

    offset = 7;
    if (env->isbg != 0) {
        clip_rect.x = env->clip.x;
        clip_rect.y = env->clip.y;
        clip_rect.w = env->clip.w;
        clip_rect.h = env->clip.h;
        clip_rect.w = CLAMP(clip_rect.w, 0, 0x400 - 1);
        clip_rect.h = CLAMP(clip_rect.h, 0, (D_8002C26C ? 0x400 : 0x200) - 1);

        if ((clip_rect.x & 0x3F) || (clip_rect.w & 0x3F)) {
            clip_rect.x -= env->ofs[0];
            clip_rect.y -= env->ofs[1];
            *((int*)dr_env + offset++) =
                0x60000000 | env->b0 << 0x10 | env->g0 << 8 | env->r0;
            *((int*)dr_env + offset++) = LOW(clip_rect.x);
            *((int*)dr_env + offset++) = LOW(clip_rect.w);
            clip_rect.x += env->ofs[0];
            clip_rect.y += env->ofs[1];
        } else {
            *((int*)dr_env + offset++) =
                0x02000000 | env->b0 << 0x10 | env->g0 << 8 | env->r0;
            *((int*)dr_env + offset++) = LOW(clip_rect.x);
            *((int*)dr_env + offset++) = LOW(clip_rect.w);
        }
    }

    setlen(dr_env, offset - 1);
}

int get_mode(int dfe, int dtd, int tpage) {
    if (D_8002C26C) {
        return 0xE1000000 | (dtd ? 0x800 : 0) | (dfe ? 0x1000 : 0) |
               (tpage & 0x27FF);
    } else {
        return 0xE1000000 | (dtd ? 0x200 : 0) | (dfe ? 0x400 : 0) |
               (tpage & 0x1FF);
    }
}

u_long get_cs(short x, short y) {
    x = CLAMP(x, 0, 0x400 - 1);
    y = CLAMP(y, 0, (D_8002C26C ? 0x400 : 0x200) - 1);
    if (D_8002C26C) {
        return 0xE3000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE3000000 | ((y & 0x3FF) << 10) | (x & 0x3FF);
    }
}

u_long get_ce(short x, short y) {
    x = CLAMP(x, 0, 0x400 - 1);
    y = CLAMP(y, 0, (D_8002C26C ? 0x400 : 0x200) - 1);
    if (D_8002C26C) {
        return 0xE4000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE4000000 | ((y & 0x1FF) << 10) | (x & 0x3FF);
    }
}

u_long get_ofs(short x, short y) {
    if (D_8002C26C) {
        return 0xE5000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE5000000 | ((y & 0x7FF) << 11) | (x & 0x7FF);
    }
}

u_long get_tw(RECT* tw) {
    u_long pad[4];

    if (tw != NULL) {
        pad[0] = (tw->x & 0xFF) >> 3;
        pad[2] = (-tw->w & 0xFF) >> 3;
        pad[1] = (tw->y & 0xFF) >> 3;
        pad[3] = (-tw->h & 0xFF) >> 3;
        return 0xE2000000 | (pad[1] << 0xF) | (pad[0] << 0xA) | (pad[3] << 5) |
               pad[2];
    }
    return 0;
}

u_long get_dx(DISPENV* env) {
    switch (D_8002C26C) {
    case 0:
        return env->disp.x;
    case 1:
        return D_8002C270 ? 0x400 - env->disp.x - env->disp.w : env->disp.x;
    case 2:
        return D_8002C270 ? 0x400 - env->disp.x - (env->disp.w / 2)
                          : env->disp.x / 2;
    }
}

u_long _status(void) { return *GPU_STATUS; }

int _otc(int arg0, int arg1) {
    *DPCR |= 0x08000000;
    *DMA6_CHCR = 0;
    *DMA6_MADR = arg0 - 4 + arg1 * 4;
    *DMA6_BCR = arg1;
    *DMA6_CHCR = 0x11000002;
    set_alarm();
    while (*DMA6_CHCR & 0x01000000) {
        if (get_alarm()) {
            return -1;
        }
    }
    return arg1;
}

int _clr(RECT* arg0, int color) {
    RECT temp;
    int* ptr;

    temp.x = arg0->x;
    temp.y = arg0->y;
    temp.w = arg0->w;
    temp.h = arg0->h;

    temp.w = CLAMP(temp.w, 0, 0x400 - 1);
    temp.h = CLAMP(temp.h, 0, (D_8002C26C ? 0x400 : 0x200) - 1);

    if ((temp.x & 0x3F) || (temp.w & 0x3F)) {
        ptr = &D_80037E20[8];
        D_80037E20[0] = 0x07000000 | ((int)ptr & 0xFFFFFF);
        D_80037E20[1] = 0xE3000000;
        D_80037E20[2] = 0xE4FFFFFF;
        D_80037E20[3] = 0xE5000000;
        D_80037E20[4] = 0xE6000000;

        D_80037E20[5] = CMD_MONOCHROME_RECTANGLE(color);
        D_80037E20[6] = LOW(temp.x);
        D_80037E20[7] = LOW(temp.w);
        D_80037E20[8] = 0x03FFFFFF;
        D_80037E20[9] = 0xE3000000 | _param(3);
        D_80037E20[10] =
            0xE4000000 | _param(4);
        D_80037E20[11] = 0xE5000000 | _param(5);
    } else {
        D_80037E20[0] = 0x04FFFFFF;
        D_80037E20[1] = 0xE6000000;

        D_80037E20[2] = CMD_FILL_RECTANGLE_IN_VRAM(color);
        D_80037E20[3] = LOW(temp.x);
        D_80037E20[4] = LOW(temp.w);
    }
    _cwc(&D_80037E20[0]);
    return 0;
}

int _dws(RECT* arg0, int* arg1) {
    RECT temp;
    int temp_a0;
    int size;
    int var_s0;
    int var_s4;

    set_alarm();
    temp.x = arg0->x;
    temp.y = arg0->y;
    temp.w = arg0->w;
    temp.h = arg0->h;
    var_s4 = 0;

    temp.w = CLAMP(temp.w, 0, 0x400 - 1);
    temp.h = CLAMP(temp.h, 0, (D_8002C26C ? 0x400 : 0x200) - 1);

    temp_a0 = ((temp.w * temp.h) + 1) / 2;
    if (temp_a0 <= 0) {
        return -1;
    }
    var_s0 = temp_a0 % 16;
    size = temp_a0 / 16;
    while (!(*GPU_STATUS & STATUS_READY_TO_RECEIVE_CMD)) {
        if (get_alarm()) {
            return -1;
        }
    }

    *GPU_STATUS = CMD_DATA_REQUEST | DATA_REQ_OFF;

    *GPU_DATA = CMD_CLEAR_CACHE;
    *GPU_DATA = var_s4 ? 0xB0000000 : CMD_COPY_CPU_TO_VRAM;
    *GPU_DATA = LOW(temp.x);
    *GPU_DATA = LOW(temp.w);

    while (var_s0--) {
        *GPU_DATA = *arg1++;
    }

    if (size != 0) {
        *GPU_STATUS = CMD_DATA_REQUEST | DATA_REQ_CPU_TO_GPU;
        *DMA2_MADR = arg1;
        *DMA2_BCR = (size << 0x10) | 0x10;
        *DMA2_CHCR = 0x01000201;
    }

    return 0;
}

int _drs(RECT* arg0, int* arg1) {
    RECT temp;
    int temp_a0;
    int size;
    int var_s0;

    set_alarm();
    temp.x = arg0->x;
    temp.y = arg0->y;
    temp.w = arg0->w;
    temp.h = arg0->h;

    temp.w = CLAMP(temp.w, 0, 0x400 - 1);
    temp.h = CLAMP(temp.h, 0, (D_8002C26C ? 0x400 : 0x200) - 1);

    temp_a0 = ((temp.w * temp.h) + 1) / 2;
    if (temp_a0 <= 0) {
        return -1;
    }
    var_s0 = temp_a0 % 16;
    size = temp_a0 / 16;
    while (!(*GPU_STATUS & STATUS_READY_TO_RECEIVE_CMD)) {
        if (get_alarm()) {
            return -1;
        }
    }

    *GPU_STATUS = CMD_DATA_REQUEST | DATA_REQ_OFF;

    *GPU_DATA = CMD_CLEAR_CACHE;
    *GPU_DATA = CMD_COPY_VRAM_TO_CPU;
    *GPU_DATA = LOW(temp.x);
    *GPU_DATA = LOW(temp.w);

    while (!(*GPU_STATUS & STATUS_READY_TO_SEND_VRAM_TO_CPU)) {
        if (get_alarm()) {
            return -1;
        }
    }

    while (var_s0--) {
        *arg1++ = *GPU_DATA;
    }

    if (size != 0) {
        *GPU_STATUS = CMD_DATA_REQUEST | DATA_REQ_GPU_TO_CPU;
        *DMA2_MADR = arg1;
        *DMA2_BCR = (size << 0x10) | 0x10;
        *DMA2_CHCR = 0x01000200;
    }

    return 0;
}

void _ctl(u_long arg0) {
    *GPU_STATUS = arg0;
    ctlbuf[(arg0 >> 0x18)] = arg0 & 0xFFFFFF;
}

int _getctl(int arg0) { return (arg0 << 0x18) | ctlbuf[arg0]; }

int _cwb(int* arg0, int arg1) {
    *GPU_STATUS = CMD_DATA_REQUEST | DATA_REQ_OFF;
    while (arg1--) {
        *GPU_DATA = *arg0++;
    }
    return 0;
}

void _cwc(int arg0) {
    *GPU_STATUS = CMD_DATA_REQUEST | DATA_REQ_CPU_TO_GPU;
    *DMA2_MADR = arg0;
    *DMA2_BCR = 0;
    *DMA2_CHCR = 0x01000401;
}

int _param(int arg0) {
    *GPU_STATUS = CMD_GPU_READ_REG | arg0;
    return *GPU_DATA & 0xFFFFFF;
}

void _addque(void (*arg0)(int*, int), int* arg1, int arg2) {
    _addque2(arg0, arg1, 0, arg2);
}

int _addque2(void (*arg0)(int*, int), int* arg1, int arg2, int arg3) {
    int temp_s0;
    int intrMask;
    int i;
    struct QueueItem* queueItem;

    intrMask = SetIntrMask(0);
    if (D_8002C274 == 0) {
        _sync(0);
        arg0(arg1, arg3);
        SetIntrMask(intrMask);
        return 0;
    }
    if ((_qin == _qout) && !(*DMA2_CHCR & 0x01000000)) {
        if (D_8002C278 != NULL) {
            DMACallback(2, _exeque);
        }
        while (!(*GPU_STATUS & 0x10000000)) {
        }
        arg0(arg1, arg3);
        SetIntrMask(intrMask);
        return 0;
    }
    temp_s0 = (_qin + 1) & 0x3F;
    if (temp_s0 == _qout) {
        set_alarm();
        while (temp_s0 == _qout) {
            if (get_alarm()) {
                SetIntrMask(intrMask);
                return -1;
            }
            _exeque();
        }
    }
    queueItem = &D_80037F54[_qin];
    _qin = temp_s0;
    queueItem->unk0 = NULL;
    if (arg2 != 0) {
        for (i = 0; i < (arg2 / 4); i++) {
            queueItem->unk[i] = arg1[i];
        }
        queueItem->unk4 = queueItem->unk;
    } else {
        queueItem->unk4 = arg1;
    }
    queueItem->unk8 = arg3;
    queueItem->unk0 = arg0;
    _exeque();
    SetIntrMask(intrMask);
    return (_qin - _qout) & 0x3F;
}

int _exeque(void) {
    int intrMask;
    int ret;

    intrMask = SetIntrMask(0);

loop_1:

    if (_qin == _qout) {
        if (D_8002C278 == NULL) {
            DMACallback(2, NULL);
        }
        ret = 0;
        goto temp0;
    }

    if (*DMA2_CHCR & 0x01000000) {
        goto temp1;
    }

    if (D_80037F54[_qout].unk0 != NULL) {
        while (!(*GPU_STATUS & 0x10000000)) {
        }
        D_80037F54[_qout].unk0(D_80037F54[_qout].unk4, D_80037F54[_qout].unk8);
        D_80037F54[_qout].unk0 = NULL;
        _qout = (_qout + 1) & 0x3F;
        goto loop_1;
    }
    _reset(1);
    printf("GPU_exeque: null func.\n");
    ret = -1;
    goto temp0;

temp0:

    if ((D_8002C278 != NULL) && CheckCallback() && (ret == 0) &&
        !(*DMA2_CHCR & 0x01000000)) {
        set_alarm();
        while (!(*GPU_STATUS & 0x04000000)) {
            if (get_alarm()) {
                break;
            }
        }
        SetIntrMask(intrMask);
        D_8002C278();
        return ret;
    } else {
        goto temp2;
    }

temp1:

    DMACallback(2, _exeque);
    ret = 1;
    goto temp0;

temp2:

    SetIntrMask(intrMask);
    return ret;
}

inline int _reset(int arg0) {
    int intrMask;
    int i;

    intrMask = SetIntrMask(0);
    DMACallback(2, NULL);
    _qout = 0;
    _qin = _qout;

    for (i = 0; i < 64; i++) {
        D_80037F54[i].unk0 = 0;
    }

    switch (arg0) {
    case 0:
        *DMA2_CHCR = 0x401;
        *DPCR |= 0x800;
        *GPU_STATUS = CMD_RESET_GPU;
        break;
    case 1:
        *DMA2_CHCR = 0x401;
        *DPCR |= 0x800;
        *GPU_STATUS = CMD_ACK_GPU_INTR;
        *GPU_STATUS = CMD_RESET_CMD_BUF;
        break;
    default:
        break;
    }
    *GPU_DATA = (*GPU_STATUS & 0x3FFF) | 0xE1001000;
    SetIntrMask(intrMask);
    return ((u_long)*GPU_STATUS >> 0xC) & 1;
}

int _sync(int arg0) {
    if (arg0 == 0) {
        if (D_8002C278 == NULL) {
            DMACallback(2, NULL);
        }
        set_alarm();
        while (_qin != _qout) {
            if (!(*DMA2_CHCR & 0x01000000)) {
                _exeque();
            }
            if (get_alarm()) {
                return -1;
            }
        }
        while (*DMA2_CHCR & 0x01000000) {
            if (get_alarm()) {
                return -1;
            }
        }
        while (!(*GPU_STATUS & 0x04000000)) {
            if (get_alarm()) {
                return -1;
            }
        }
        return 0;
    }
    if ((_qin != _qout) && !(*DMA2_CHCR & 0x01000000)) {
        _exeque();
    }

    if (*DMA2_CHCR & 0x01000000) {
        return 1;
    } else if (!(*GPU_STATUS & 0x04000000)) {
        return 1;
    } else {
        return 0;
    }
}

void set_alarm(void) {

    D_80039254 = VSync(-1) + 240;
    D_80039258 = 0;
}

int get_alarm(void) {
    if (D_80039254 < VSync(-1) || D_80039258++ > 0x780000) {
        *GPU_STATUS;
        printf("GPU timeout:que=%d,stat=%08x,chcr=%08x,madr=%08x\n",
               (_qin - _qout) & 0x3F, *GPU_STATUS, *DMA2_CHCR, *DMA2_MADR);
        _reset(1);
        return -1;
    }
    return 0;
}

static void memset2(s8* ptr, int value, int num) {
    while (num--) {
        *ptr++ = value;
    }
}

#elif VERSION == 40 || VERSION == 37 || VERSION == 36 || VERSION == 35

#define LOW(x) (*(s32*)&(x))

typedef struct {
     const char* rcsid;
     void (*addque)(void (*)(), u_long*, int);
     int (*addque2)(void (*)(), void*, int, int);
     int (*clr)(RECT*, u_long);
     void (*ctl)(u_long);
     int (*cwb)(u_long*, int);
     void (*cwc)(u_long*);
     int (*drs)(RECT*, u_long*);
     int (*dws)(RECT*, u_long*);
     int (*exeque)(void);
     int (*getctl)(int);
     int (*otc)(u_long*, int);
     int (*param)(int);
     int (*reset)(int);
     u_long (*status)(void);
     int (*sync)(int);
} gpu;

typedef struct {
     u_char version;
     u_char queue_mode;
     u_char level;
     u_char reverse;
     short w;
     short h;
     volatile int cb_pending;
     void (*drawsync_cb)();
     DRAWENV draw;
     DISPENV disp;
} GpuCtl;

typedef struct {
     void (*func)();
     void* param;
     int arg;
     u_long buf[0x15];
} QueueItem;

typedef struct {
     void (*func)();
     void* param;
     int arg;
     int unk;
} QueueLog;

static void _addque(void (*func)(), u_long* param, int arg);
static int _addque2(void (*func)(), void* param, int len, int arg);
static int _clr(RECT* rect, u_long color);
static void _ctl(u_long cmd);
static int _cwb(u_long* p, int n);
static void _cwc(u_long* p);
static int _drs(RECT* rect, u_long* p);
static int _dws(RECT* rect, u_long* p);
static int _exeque(void);
static int _getctl(int n);
static int _otc(u_long* ot, int n);
static int _param(int n);
static int _reset(int mode);
static u_long _status(void);
static int _sync(int mode);
static void checkRECT(const char* log, RECT* r);
#if VERSION != 36 && VERSION != 35
static void SetDrawEnv2(DR_ENV* dr_env, DRAWENV* env);
#endif
static u_long get_mode(int dfe, int dtd, int tpage);
static u_long get_cs(short x, short y);
static u_long get_ce(short x, short y);
static u_long get_ofs(short x, short y);
static u_long get_tw(RECT* tw);
static u_long get_dx(DISPENV* env);
static void set_alarm(void);
static int get_alarm(void);
static int _version(int mode);
static void memset(void* ptr, int value, int num);

int printf(const char*, ...);
void* memcpy(void*, const void*, int);
void GPU_cw(u_long);
void ResetCallback(void);
int GetVideoMode(void);
int DMACallback(int, void (*)());
int SetIntrMask(int);
int VSync(int);

#if VERSION == 35

#elif VERSION == 36
static int __ps_libinfo__[2] = {0x0A077350, 0x1036D61A};
#elif VERSION == 37
static int __ps_libinfo__[2] = {0x0C077350, 0x00379CDA};
#else
static int __ps_libinfo__[2] = {0x15077350, 0x0040899C};
#endif

static gpu jtb = {
#if VERSION == 35
    "$Id: sys.c,v 1.120 1996/05/01 12:09:07 noda Exp $",
#elif VERSION == 36
    "$Id: sys.c,v 1.126 1996/09/13 06:29:54 noda Exp $",
#else
    "$Id: sys.c,v 1.129 1996/12/25 03:36:20 noda Exp $",
#endif
    _addque,
    _addque2,
    _clr,
    _ctl,
    _cwb,
    _cwc,
    _drs,
    _dws,
    _exeque,
    _getctl,
    _otc,
    _param,
    _reset,
    _status,
    _sync,
};
static gpu* jtbp = &jtb;
int (*GPU_printf)(const char* fmt, ...) = printf;
static GpuCtl GEnv = {0, 0, 0, 0, 0, 0, 0, 0, {{0, 0}}};
static int def_width[] = {1024, 1024, 1024, 1024, 1024};
static int def_height[] = {512, 1024, 1024, 512, 1024};

#if VERSION == 35
int ResetGraph(int mode) {
    int op = mode & 7;

    if (op == 0 || op == 3) {
        printf("ResetGraph:jtb=%08x,env=%08x\n", &jtb, &GEnv);
        memset(&GEnv, 0, sizeof(GEnv));
        ResetCallback();
        GPU_cw((u_long)jtbp & 0xFFFFFF);
        GEnv.version = _reset(op != 0);
        GEnv.queue_mode = 1;
        GEnv.w = def_width[GEnv.version];
        GEnv.h = def_height[GEnv.version];
        memset(&GEnv.draw, -1, sizeof(DRAWENV));
        memset(&GEnv.disp, -1, sizeof(DISPENV));
        return GEnv.version;
    }
    if (GEnv.level >= 2) {
        GPU_printf("ResetGraph(%d)...\n", mode);
    }
    return jtbp->reset(1);
}
#elif VERSION == 36
int ResetGraph(int mode) {
    switch (mode & 7) {
    case 3:
    case 0:
        printf("ResetGraph:jtb=%08x,env=%08x\n", &jtb, &GEnv);
        memset(&GEnv, 0, sizeof(GEnv));
        ResetCallback();
        GPU_cw((u_long)jtbp & 0xFFFFFF);
        GEnv.version = _reset(mode);
        GEnv.queue_mode = 1;
        GEnv.w = def_width[GEnv.version];
        GEnv.h = def_height[GEnv.version];
        memset(&GEnv.draw, -1, sizeof(DRAWENV));
        memset(&GEnv.disp, -1, sizeof(DISPENV));
        return GEnv.version;
    }
    if (GEnv.level >= 2) {
        GPU_printf("ResetGraph(%d)...\n", mode);
    }
    return jtbp->reset(1);
}
#else
int ResetGraph(int mode) {
    switch (mode & 7) {
    case 3:
    case 0:
        printf("ResetGraph:jtb=%08x,env=%08x\n", &jtb, &GEnv);
    case 5:
        memset(&GEnv, 0, sizeof(GEnv));
        ResetCallback();
        GPU_cw((u_long)jtbp & 0xFFFFFF);
        GEnv.version = _reset(mode);
        GEnv.queue_mode = 1;
        GEnv.w = def_width[GEnv.version];
        GEnv.h = def_height[GEnv.version];
        memset(&GEnv.draw, -1, sizeof(DRAWENV));
        memset(&GEnv.disp, -1, sizeof(DISPENV));
        return GEnv.version;
    }
    if (GEnv.level >= 2) {
        GPU_printf("ResetGraph(%d)...\n", mode);
    }
    return jtbp->reset(1);
}
#endif

int SetGraphReverse(int mode) {
    u_char old = GEnv.reverse;

    if (GEnv.level >= 2) {
        GPU_printf("SetGraphReverse(%d)...\n", mode);
    }
    GEnv.reverse = mode;
    jtbp->ctl(jtbp->getctl(8) |
                    (GEnv.reverse ? 0x08000080 : 0x08000000));
    if (GEnv.version == 2) {
        jtbp->ctl(GEnv.reverse ? 0x20000501 : 0x20000504);
    }
    return old;
}

int SetGraphDebug(int level) {
    u_char old = GEnv.level;

    if (GEnv.level = level) {
        GPU_printf("SetGraphDebug:level:%d,type:%d reverse:%d\n",
                   GEnv.level, GEnv.version, GEnv.reverse);
    }
    return old;
}

int SetGraphQueue(int mode) {
    u_char old = GEnv.queue_mode;

    if (GEnv.level >= 2) {
        GPU_printf("SetGrapQue(%d)...\n", mode);
    }
    if (mode != GEnv.queue_mode) {
        jtbp->reset(1);
        GEnv.queue_mode = mode;
        DMACallback(2, NULL);
    }
    return old;
}

int GetGraphType(void) { return GEnv.version; }

int GetGraphDebug(void) { return GEnv.level; }

u_long DrawSyncCallback(void (*func)()) {
    void (*old)();

    if (GEnv.level >= 2) {
        GPU_printf("DrawSyncCallback(%08x)...\n", func);
    }
    old = GEnv.drawsync_cb;
    GEnv.drawsync_cb = func;
    return (u_long)old;
}

void SetDispMask(int mask) {
    if (GEnv.level >= 2) {
        GPU_printf("SetDispMask(%d)...\n", mask);
    }
    if (mask == 0) {
        memset(&GEnv.disp, -1, sizeof(DISPENV));
    }
    jtbp->ctl(mask ? 0x03000000 : 0x03000001);
}

int DrawSync(int mode) {
    if (GEnv.level >= 2) {
        GPU_printf("DrawSync(%d)...\n", mode);
    }
    return jtbp->sync(mode);
}

static void checkRECT(const char* log, RECT* r) {
    switch (GEnv.level) {
    case 1:
        if (r->w > GEnv.w || r->w + r->x > GEnv.w ||
            r->y > GEnv.h || r->y + r->h > GEnv.h || r->w <= 0 ||
            r->x < 0 || r->y < 0 || r->h <= 0) {
            GPU_printf("%s:bad RECT", log);
            GPU_printf("(%d,%d)-(%d,%d)\n", r->x, r->y, r->w, r->h);
        }
        break;
    case 2:
        GPU_printf("%s:", log);
        GPU_printf("(%d,%d)-(%d,%d)\n", r->x, r->y, r->w, r->h);
        break;
    }
}

int ClearImage(RECT* rect, u_char r, u_char g, u_char b) {
    checkRECT("ClearImage", rect);
    return jtbp->addque2(
        jtbp->clr, rect, sizeof(RECT), (b << 0x10) | (g << 8) | r);
}

#if VERSION != 35
int ClearImage2(RECT* rect, u_char r, u_char g, u_char b) {
    checkRECT("ClearImage", rect);
    return jtbp->addque2(jtbp->clr, rect, sizeof(RECT),
                               0x80000000 | (b << 0x10) | (g << 8) | r);
}
#endif

int LoadImage(RECT* rect, u_long* p) {
    checkRECT("LoadImage", rect);
    return jtbp->addque2(jtbp->dws, rect, sizeof(RECT), p);
}

int StoreImage(RECT* rect, u_long* p) {
    checkRECT("StoreImage", rect);
    return jtbp->addque2(jtbp->drs, rect, sizeof(RECT), p);
}

int MoveImage(RECT* rect, int x, int y) {
    static u_long com[5] = {0x04FFFFFF, 0x80000000};

    checkRECT("MoveImage", rect);
    if (rect->w == 0 || rect->h == 0) {
        return -1;
    }
    com[2] = LOW(rect->x);
    com[3] = y << 0x10 | (u_short)x;
    com[4] = LOW(rect->w);
    return jtbp->addque2(jtbp->cwc, com, sizeof(com), 0);
}

static u_long nop[5] = {0x04FFFFFF, 0, 0, 0, 0};

OT_TYPE* ClearOTag(OT_TYPE* ot, int n) {
    if (GEnv.level >= 2) {
        GPU_printf("ClearOTag(%08x,%d)...\n", ot, n);
    }
    while (--n) {
        setlen(ot, 0);
        setaddr(ot, ot + 1);
        ot++;
    }
    *ot = (u_long)nop & 0xFFFFFF;
    return ot;
}

OT_TYPE* ClearOTagR(OT_TYPE* ot, int n) {
    if (GEnv.level >= 2) {
        GPU_printf("ClearOTagR(%08x,%d)...\n", ot, n);
    }
    jtbp->otc(ot, n);
    *ot = (u_long)nop & 0xFFFFFF;
    return ot;
}

void DrawPrim(void* p) {
    int len = getlen(p);
    jtbp->sync(0);
    jtbp->cwb((u_long*)&((P_TAG*)p)->r0, len);
}

void DrawOTag(OT_TYPE* p) {
    if (GEnv.level >= 2) {
        GPU_printf("DrawOTag(%08x)...\n", p);
    }
    jtbp->addque2(jtbp->cwc, p, 0, 0);
}

#if VERSION == 36 || VERSION == 35
DRAWENV* PutDrawEnv(DRAWENV* env) {
    if (GEnv.level >= 2) {
        GPU_printf("PutDrawEnv(%08x)...\n", env);
    }
    SetDrawEnv(&env->dr_env, env);
    env->dr_env.tag |= 0xFFFFFF;
    jtbp->addque2(jtbp->cwc, &env->dr_env, sizeof(DR_ENV), 0);
    memcpy(&GEnv.draw, env, sizeof(DRAWENV));
    return env;
}
#else
DRAWENV* PutDrawEnv(DRAWENV* env) {
    if (GEnv.level >= 2) {
        GPU_printf("PutDrawEnv(%08x)...\n", env);
    }
    SetDrawEnv2(&env->dr_env, env);
    termPrim(&env->dr_env);
    jtbp->addque2(jtbp->cwc, &env->dr_env, sizeof(DR_ENV), 0);
    __builtin_memcpy(&GEnv.draw, env, sizeof(DRAWENV));
    return env;
}
#endif

void DrawOTagEnv(OT_TYPE* p, DRAWENV* env) {
    if (GEnv.level >= 2) {
        GPU_printf("DrawOTagEnv(%08x,&08x)...\n", p, env);
    }
#if VERSION == 36 || VERSION == 35
    SetDrawEnv(&env->dr_env, env);
#else
    SetDrawEnv2(&env->dr_env, env);
#endif
    setaddr(&env->dr_env, p);
    jtbp->addque2(jtbp->cwc, &env->dr_env, sizeof(DR_ENV), 0);
#if VERSION == 36 || VERSION == 35
    memcpy(&GEnv.draw, env, sizeof(DRAWENV));
#else
    __builtin_memcpy(&GEnv.draw, env, sizeof(DRAWENV));
#endif
}

DRAWENV* GetDrawEnv(DRAWENV* env) {
    memcpy(env, &GEnv.draw, sizeof(DRAWENV));
    return env;
}

#define CLAMP(value, low, high)                                                \
    value < low ? low : (value > high ? high : value)

#define VOL(x) (*(volatile __typeof__(x)*)&(x))
#define IS_NEW_GPU(version) ((unsigned int)((version) - 1) < 2)

static inline int get_version(void) { return VOL(GEnv.version); }

static inline int is_new_gpu(void) { return IS_NEW_GPU(get_version()); }

#define CLAMP_MAX(value, max)                                                  \
    ((value) < 1 ? 1 : ({                                                       \
        int _max = (max);                                                      \
        (value) > _max ? _max : (value);                                       \
    }))

#define CLAMP_MAX0(value, max)                                                 \
    ((value) < 0 ? 0 : ({                                                       \
        int _max = (max);                                                      \
        (value) > _max ? _max : (value);                                       \
    }))

#define RECT_EQ(r1, r2)                                                        \
    (((volatile RECT*)(r1))->x == (r2).x &&                                    \
     ((volatile RECT*)(r1))->y == (r2).y &&                                    \
     ((volatile RECT*)(r1))->w == (r2).w && ((volatile RECT*)(r1))->h == (r2).h)

DISPENV* PutDispEnv(DISPENV* env) {
    int h_start, h_end;
    int v_start, v_end;
    int mode;

    mode = 0x08000000;
#if VERSION == 36 || VERSION == 35
    if (VOL(GEnv.level) >= 2) {
#else
    if (GEnv.level >= 2) {
#endif
        GPU_printf("PutDispEnv(%08x)...\n", env);
    }
    jtbp->ctl(
        GEnv.version == 1 || GEnv.version == 2
            ? ((env->disp.y & 0xFFF) << 12) | (get_dx(env) & 0xFFF) | 0x05000000
            : ((env->disp.y & 0x3FF) << 10) | (env->disp.x & 0x3FF) |
                  0x05000000);
    if (!RECT_EQ(&GEnv.disp.screen, env->screen)) {
        env->pad0 = GetVideoMode();
        h_start = env->screen.x * 10 + 0x260;
        v_start = env->screen.y + (env->pad0 ? 0x13 : 0x10);
        h_end = h_start + (env->screen.w ? env->screen.w * 10 : 2560);
        v_end = v_start + (env->screen.h ? env->screen.h : 240);
        h_start = CLAMP(h_start, 500, 3290);
        h_end = CLAMP(h_end, h_start + 0x50, 3290);
        v_start = CLAMP(v_start, 0x10, (env->pad0 ? 310 : 256));
        v_end = CLAMP(v_end, v_start + 2, (env->pad0 ? 312 : 258));
        jtbp->ctl(((h_end & 0xFFF) << 12) | 0x06000000 |
                        (h_start & 0xFFF));
        jtbp->ctl(((v_end & 0x3FF) << 10) | 0x07000000 |
                        (v_start & 0x3FF));
    }
    if (LOW(GEnv.disp.isinter) != LOW(env->isinter) ||
        !RECT_EQ(&GEnv.disp.disp, env->disp)) {
        env->pad0 = GetVideoMode();
        if (env->pad0 == 1) {
            mode |= 0x8;
        }
        if (env->isrgb24) {
            mode |= 0x10;
        }
        if (env->isinter) {
            mode |= 0x20;
        }
#if VERSION == 36 || VERSION == 35
        if (VOL(GEnv.reverse)) {
#else
        if (GEnv.reverse) {
#endif
            mode |= 0x80;
        }
        if (env->disp.w <= 280) {
        } else if (env->disp.w <= 352) {
            mode |= 1;
        } else if (env->disp.w <= 400) {
            mode |= 0x40;
        } else if (env->disp.w <= 560) {
            mode |= 2;
        } else {
            mode |= 3;
        }
        if (env->disp.h <= (!env->pad0 ? 256 : 288)) {
        } else {
            mode |= 0x24;
        }
        jtbp->ctl(mode);
    }
    memcpy(&GEnv.disp, env, sizeof(DISPENV));
    return env;
}

DISPENV* GetDispEnv(DISPENV* env) {
    memcpy(env, &GEnv.disp, sizeof(DISPENV));
    return env;
}

u_long GetODE(void) { return jtbp->status() >> 0x1F; }

void SetTexWindow(DR_TWIN* p, RECT* tw) {
    setlen(p, 2);
    p->code[0] = get_tw(tw);
    p->code[1] = 0;
}

void SetDrawArea(DR_AREA* p, RECT* r) {
    setlen(p, 2);
    p->code[0] = get_cs(r->x, r->y);
    p->code[1] = get_ce(r->x + r->w - 1, r->y + r->h - 1);
}

void SetDrawOffset(DR_OFFSET* p, u_short* ofs) {
    setlen(p, 2);
    p->code[0] = get_ofs(ofs[0], ofs[1]);
    p->code[1] = 0;
}

void SetPriority(DR_PRIO* p, int pbc, int pbw) {
    u_long data;

    setlen(p, 2);
    data = 0xE6000000;
    if (pbc) {
        data = 0xE6000002;
    }
    p->code[0] = data | (pbw != 0);
    p->code[1] = 0;
}

void SetDrawMode(DR_MODE* p, int dfe, int dtd, int tpage, RECT* tw) {
    setlen(p, 2);
    p->code[0] = get_mode(dfe, dtd, (u_short)tpage);
    p->code[1] = get_tw(tw);
}

#if VERSION == 36 || VERSION == 35
void SetDrawEnv(DR_ENV* dr_env_in, DRAWENV* env) {
    RECT clip_rect;
    int offset;
    DR_ENV* dr_env;

    dr_env = dr_env_in;

    dr_env->code[0] = get_cs(env->clip.x, env->clip.y);
    dr_env->code[1] =
        get_ce(env->clip.w + env->clip.x - 1, env->clip.y + env->clip.h - 1);
    dr_env->code[2] = get_ofs(env->ofs[0], env->ofs[1]);
    dr_env->code[3] = get_mode(env->dfe, env->dtd, env->tpage);
    dr_env->code[4] = get_tw(&env->tw);
    dr_env->code[5] = 0xE6000000;

    offset = 7;
    if (env->isbg != 0) {
        clip_rect.x = env->clip.x;
        clip_rect.y = env->clip.y;
        clip_rect.w = env->clip.w;
        clip_rect.h = env->clip.h;
        clip_rect.w = CLAMP_MAX0(clip_rect.w, VOL(GEnv.w) - 1);
        clip_rect.h = CLAMP_MAX0(clip_rect.h, VOL(GEnv.h) - 1);

        if ((clip_rect.x & 0x3F) || (clip_rect.w & 0x3F)) {
            clip_rect.x -= env->ofs[0];
            clip_rect.y -= env->ofs[1];
            *((int*)dr_env + offset++) =
                0x60000000 | env->b0 << 0x10 | env->g0 << 8 | env->r0;
            *((int*)dr_env + offset++) = LOW(clip_rect.x);
            *((int*)dr_env + offset++) = LOW(clip_rect.w);
            clip_rect.x += env->ofs[0];
            clip_rect.y += env->ofs[1];
        } else {
            *((int*)dr_env + offset++) =
                0x02000000 | env->b0 << 0x10 | env->g0 << 8 | env->r0;
            *((int*)dr_env + offset++) = LOW(clip_rect.x);
            *((int*)dr_env + offset++) = LOW(clip_rect.w);
        }
    }

    setlen(dr_env, offset - 1);
}
#else
void SetDrawEnv(DR_ENV* dr_env, DRAWENV* env) {
    DR_ENV* dr;
    int len;
    RECT rect;
    u_short w, h;

    len = 1;
    dr = dr_env;
    ((u_long*)&dr->code[-1])[len++] = get_cs(env->clip.x, env->clip.y);
    ((u_long*)&dr->code[-1])[len++] =
        get_ce(env->clip.w + env->clip.x - 1, env->clip.y + env->clip.h - 1);
    ((u_long*)&dr->code[-1])[len++] = get_ofs(env->ofs[0], env->ofs[1]);
    ((u_long*)&dr->code[-1])[len++] = get_mode(env->dfe, env->dtd, env->tpage);
    ((u_long*)&dr->code[-1])[len++] = get_tw(&env->tw);
    ((u_long*)&dr->code[-1])[len++] = 0xE6000000;
    if (env->isbg) {
        rect.x = env->clip.x;
        rect.y = env->clip.y;
        rect.w = env->clip.w;
        rect.h = env->clip.h;
        if (rect.w >= 0) {
            if (GEnv.w - 1 < rect.w) {
                w = GEnv.w - 1;
            } else {
                w = rect.w;
            }
        } else {
            w = 0;
        }
        rect.w = w;
        if (rect.h >= 0) {
            if (GEnv.h - 1 < rect.h) {
                h = GEnv.h - 1;
            } else {
                h = rect.h;
            }
        } else {
            h = 0;
        }
        rect.h = h;
        rect.x -= env->ofs[0];
        rect.y -= env->ofs[1];
        ((u_long*)&dr->code[-1])[len++] =
            0x60000000 | (env->b0 << 16) | (env->g0 << 8) | env->r0;
        ((u_long*)&dr->code[-1])[len++] = LOW(rect.x);
        ((u_long*)&dr->code[-1])[len++] = LOW(rect.w);
        rect.x += env->ofs[0];
        rect.y += env->ofs[1];
    }
    setlen(dr, len - 1);
}
#endif

#if VERSION != 36 && VERSION != 35
static void SetDrawEnv2(DR_ENV* dr_env, DRAWENV* env) {
    DR_ENV* dr;
    int len;
    RECT rect;
    u_short w, h;

    len = 1;
    dr = dr_env;
    ((u_long*)&dr->code[-1])[len++] = get_cs(env->clip.x, env->clip.y);
    ((u_long*)&dr->code[-1])[len++] =
        get_ce(env->clip.w + env->clip.x - 1, env->clip.y + env->clip.h - 1);
    ((u_long*)&dr->code[-1])[len++] = get_ofs(env->ofs[0], env->ofs[1]);
    ((u_long*)&dr->code[-1])[len++] = get_mode(env->dfe, env->dtd, env->tpage);
    ((u_long*)&dr->code[-1])[len++] = get_tw(&env->tw);
    ((u_long*)&dr->code[-1])[len++] = 0xE6000000;
    if (env->isbg) {
        rect.x = env->clip.x;
        rect.y = env->clip.y;
        rect.w = env->clip.w;
        rect.h = env->clip.h;
        if (rect.w >= 0) {
            if (GEnv.w - 1 < rect.w) {
                w = GEnv.w - 1;
            } else {
                w = rect.w;
            }
        } else {
            w = 0;
        }
        rect.w = w;
        if (rect.h >= 0) {
            if (GEnv.h - 1 < rect.h) {
                h = GEnv.h - 1;
            } else {
                h = rect.h;
            }
        } else {
            h = 0;
        }
        rect.h = h;
        if (rect.x & 0x3F || rect.w & 0x3F) {
            rect.x -= env->ofs[0];
            rect.y -= env->ofs[1];
            ((u_long*)&dr->code[-1])[len++] =
                0x60000000 | (env->b0 << 16) | (env->g0 << 8) | env->r0;
            ((u_long*)&dr->code[-1])[len++] = LOW(rect.x);
            ((u_long*)&dr->code[-1])[len++] = LOW(rect.w);
            rect.x += env->ofs[0];
            rect.y += env->ofs[1];
        } else {
            ((u_long*)&dr->code[-1])[len++] =
                0x02000000 | (env->b0 << 16) | (env->g0 << 8) | env->r0;
            ((u_long*)&dr->code[-1])[len++] = LOW(rect.x);
            ((u_long*)&dr->code[-1])[len++] = LOW(rect.w);
        }
    }
    setlen(dr, len - 1);
}
#endif

#if VERSION == 36 || VERSION == 35
static u_long get_mode(int dfe, int dtd, int tpage) {
    if (is_new_gpu()) {
        return 0xE1000000 | (dtd ? 0x800 : 0) | (dfe ? 0x1000 : 0) |
               (tpage & 0x27FF);
    } else {
        return 0xE1000000 | (dtd ? 0x200 : 0) | (dfe ? 0x400 : 0) |
               (tpage & 0x9FF);
    }
}
#else
static u_long get_mode(int dfe, int dtd, int tpage) {
    if (GEnv.version == 1 || GEnv.version == 2) {
        return (dtd ? 0xE1000800 : 0xE1000000) | (dfe ? 0x1000 : 0) |
               (tpage & 0x27FF);
    } else {
        return (dtd ? 0xE1000200 : 0xE1000000) | (dfe ? 0x400 : 0) |
               (tpage & 0x9FF);
    }
}
#endif

#if VERSION == 36 || VERSION == 35
static u_long get_cs(short x, short y) {
    x = CLAMP_MAX0(x, VOL(GEnv.w) - 1);
    y = CLAMP_MAX0(y, VOL(GEnv.h) - 1);
    if (is_new_gpu()) {
        return 0xE3000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE3000000 | ((y & 0x3FF) << 10) | (x & 0x3FF);
    }
}
#else
static u_long get_cs(short x, short y) {
    x = CLAMP(x, 0, GEnv.w - 1);
    y = CLAMP(y, 0, GEnv.h - 1);
    if (GEnv.version == 1 || GEnv.version == 2) {
        return 0xE3000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE3000000 | ((y & 0x3FF) << 10) | (x & 0x3FF);
    }
}
#endif

#if VERSION == 36 || VERSION == 35
static u_long get_ce(short x, short y) {
    x = CLAMP_MAX0(x, VOL(GEnv.w) - 1);
    y = CLAMP_MAX0(y, VOL(GEnv.h) - 1);
    if (is_new_gpu()) {
        return 0xE4000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE4000000 | ((y & 0x3FF) << 10) | (x & 0x3FF);
    }
}
#else
static u_long get_ce(short x, short y) {
    x = CLAMP(x, 0, GEnv.w - 1);
    y = CLAMP(y, 0, GEnv.h - 1);
    if (GEnv.version == 1 || GEnv.version == 2) {
        return 0xE4000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE4000000 | ((y & 0x3FF) << 10) | (x & 0x3FF);
    }
}
#endif

#if VERSION == 36 || VERSION == 35
static u_long get_ofs(short x, short y) {
    if (is_new_gpu()) {
        return 0xE5000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE5000000 | ((y & 0x7FF) << 11) | (x & 0x7FF);
    }
}
#else
static u_long get_ofs(short x, short y) {
    if (GEnv.version == 1 || GEnv.version == 2) {
        return 0xE5000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE5000000 | ((y & 0x7FF) << 11) | (x & 0x7FF);
    }
}
#endif

static u_long get_tw(RECT* tw) {
    u_long pad[4];

    if (tw != NULL) {
        pad[0] = (tw->x & 0xFF) >> 3;
        pad[2] = (-tw->w & 0xFF) >> 3;
        pad[1] = (tw->y & 0xFF) >> 3;
        pad[3] = (-tw->h & 0xFF) >> 3;
        return 0xE2000000 | (pad[1] << 0xF) | (pad[0] << 0xA) | (pad[3] << 5) |
               pad[2];
    }
    return 0;
}

#if VERSION == 36 || VERSION == 35
static u_long get_dx(DISPENV* env) {
    switch (get_version()) {
    case 1:
        return GEnv.reverse ? 0x400 - env->disp.x - env->disp.w
                                : env->disp.x;
    case 2:
        return GEnv.reverse ? 0x400 - env->disp.x - env->disp.w / 2
                                : env->disp.x / 2;
    }
    return env->disp.x;
}
#else
static u_long get_dx(DISPENV* env) {
    switch (GEnv.version) {
    case 1:
        return GEnv.reverse ? 0x400 - env->disp.x - env->disp.w
                                : env->disp.x;
    case 2:
        return GEnv.reverse ? 0x400 - env->disp.x - env->disp.w / 2
                                : env->disp.x / 2;
    default:
        return env->disp.x;
    }
}
#endif

static volatile u_long* gp0 = (u_long*)0x1F801810;
static volatile u_long* gp1 = (u_long*)0x1F801814;
static volatile u_long* d2_madr = (u_long*)0x1F8010A0;
static volatile u_long* d2_bcr = (u_long*)0x1F8010A4;
static volatile u_long* d2_chcr = (u_long*)0x1F8010A8;
static volatile u_long* d6_madr = (u_long*)0x1F8010E0;
static volatile u_long* d6_bcr = (u_long*)0x1F8010E4;
static volatile u_long* d6_chcr = (u_long*)0x1F8010E8;
static volatile u_long* d_pcr = (u_long*)0x1F8010F0;

static u_long _status(void) { return *gp1; }

static int _otc(u_long* ot, int n) {
    *d_pcr |= 0x08000000;
    *d6_chcr = 0;
    *d6_madr = (u_long)&ot[n - 1];
    *d6_bcr = n;
    *d6_chcr = 0x11000002;
    set_alarm();
    while (*d6_chcr & 0x01000000) {
        if (get_alarm()) {
            return -1;
        }
    }
    return n;
}

static int _clr(RECT* rect, u_long color) {
#if VERSION == 35
    static u_long p0[8];
#else
    static u_long p0[9];
#endif
    static u_long p1[8];

#if VERSION == 36 || VERSION == 35
    rect->w = CLAMP_MAX0(rect->w, VOL(GEnv.w) - 1);
    rect->h = CLAMP_MAX0(rect->h, VOL(GEnv.h) - 1);
#else
    rect->w = CLAMP(rect->w, 0, GEnv.w - 1);
    rect->h = CLAMP(rect->h, 0, GEnv.h - 1);
#endif
#if VERSION == 35

    if ((rect->x & 0x3F) || (rect->w & 0x3F)) {
        p0[0] = ((u_long)p1 & 0xFFFFFF) | 0x07000000;
        p0[1] = 0xE3000000;
        p0[2] = 0xE4FFFFFF;
        p0[3] = 0xE5000000;
        p0[4] = 0xE6000000;
        p0[5] = (color & 0xFFFFFF) | 0x60000000;
        p0[6] = LOW(rect->x);
        p0[7] = LOW(rect->w);
        p1[0] = 0x03FFFFFF;
        p1[1] = _param(3) | 0xE3000000;
        p1[2] = _param(4) | 0xE4000000;
        p1[3] = _param(5) | 0xE5000000;
    } else {
        p0[0] = 0x04FFFFFF;
        p0[1] = 0xE6000000;
        p0[2] = (color & 0xFFFFFF) | 0x02000000;
        p0[3] = LOW(rect->x);
        p0[4] = LOW(rect->w);
    }
#else
    if ((rect->x & 0x3F) || (rect->w & 0x3F)) {
        p0[0] = ((u_long)p1 & 0xFFFFFF) | 0x08000000;
        p0[1] = 0xE3000000;
        p0[2] = 0xE4FFFFFF;
        p0[3] = 0xE5000000;
        p0[4] = 0xE6000000;
        p0[5] = 0xE1000000 | (*gp1 & 0x7FF) | (color >> 0x1F) << 10;
        p0[6] = (color & 0xFFFFFF) | 0x60000000;
        p0[7] = LOW(rect->x);
        p0[8] = LOW(rect->w);
        p1[0] = 0x03FFFFFF;
        p1[1] = _param(3) | 0xE3000000;
        p1[2] = _param(4) | 0xE4000000;
        p1[3] = _param(5) | 0xE5000000;
    } else {
        p0[0] = 0x05FFFFFF;
        p0[1] = 0xE6000000;
        p0[2] = 0xE1000000 | (*gp1 & 0x7FF) | (color >> 0x1F) << 10;
        p0[3] = (color & 0xFFFFFF) | 0x02000000;
        p0[4] = LOW(rect->x);
        p0[5] = LOW(rect->w);
    }
#endif
    _cwc(p0);
    return 0;
}

static int _dws(RECT* rect, u_long* p) {
    int size;
    int n;
    int i;
    int isbg;

    set_alarm();
    isbg = 0;
#if VERSION == 36 || VERSION == 35
    rect->w = rect->w < 0 ? 0 : (VOL(GEnv.w) < rect->w ? VOL(GEnv.w) : rect->w);
    rect->h = rect->h < 0 ? 0 : (VOL(GEnv.h) < rect->h ? VOL(GEnv.h) : rect->h);
#else
    rect->w = CLAMP(rect->w, 0, GEnv.w);
    rect->h = CLAMP(rect->h, 0, GEnv.h);
#endif
    size = ((rect->w * rect->h) + 1) / 2;
    if (size <= 0) {
        return -1;
    }
    i = size % 16;
    n = size / 16;
    while (!(*gp1 & 0x04000000)) {
        if (get_alarm()) {
            return -1;
        }
    }
    *gp1 = 0x04000000;
    *gp0 = 0x01000000;
    *gp0 = isbg ? 0xB0000000 : 0xA0000000;
    *gp0 = LOW(rect->x);
    *gp0 = LOW(rect->w);
    while (i--) {
        *gp0 = *p++;
    }
    if (n != 0) {
        *gp1 = 0x04000002;
        *d2_madr = (u_long)p;
        *d2_bcr = (n << 0x10) | 0x10;
        *d2_chcr = 0x01000201;
    }
    return 0;
}

static int _drs(RECT* rect, u_long* p) {
    int size;
    int n;
    int i;

    set_alarm();
#if VERSION == 36 || VERSION == 35
    rect->w = rect->w < 0 ? 0 : (VOL(GEnv.w) < rect->w ? VOL(GEnv.w) : rect->w);
    rect->h = rect->h < 0 ? 0 : (VOL(GEnv.h) < rect->h ? VOL(GEnv.h) : rect->h);
#else
    rect->w = CLAMP(rect->w, 0, GEnv.w);
    rect->h = CLAMP(rect->h, 0, GEnv.h);
#endif
    size = ((rect->w * rect->h) + 1) / 2;
    if (size <= 0) {
        return -1;
    }
    i = size % 16;
    n = size / 16;
    while (!(*gp1 & 0x04000000)) {
        if (get_alarm()) {
            return -1;
        }
    }
    *gp1 = 0x04000000;
    *gp0 = 0x01000000;
    *gp0 = 0xC0000000;
    *gp0 = LOW(rect->x);
    *gp0 = LOW(rect->w);
    while (!(*gp1 & 0x08000000)) {
        if (get_alarm()) {
            return -1;
        }
    }
    while (i--) {
        *p++ = *gp0;
    }
    if (n != 0) {
        *gp1 = 0x04000003;
        *d2_madr = (u_long)p;
        *d2_bcr = (n << 0x10) | 0x10;
        *d2_chcr = 0x01000200;
    }
    return 0;
}

static u_char ctlbuf[0x100];

static void _ctl(u_long cmd) {
    *gp1 = cmd;
    ctlbuf[cmd >> 0x18] = cmd;
}

static int _getctl(int n) { return ctlbuf[n]; }

static int _cwb(u_long* p, int n) {
    *gp1 = 0x04000000;
    while (n--) {
        *gp0 = *p++;
    }
    return 0;
}

static void _cwc(u_long* p) {
    *gp1 = 0x04000002;
    *d2_madr = (u_long)p;
    *d2_bcr = 0;
    *d2_chcr = 0x01000401;
}

static int _param(int n) {
    *gp1 = 0x10000000 | n;
    return *gp0 & 0xFFFFFF;
}

volatile QueueLog _qlog = {0, 0, 0, 0};
volatile int _qin = 0;
volatile int _qout = 0;
volatile QueueItem _que[0x40];

static void _addque(void (*func)(), u_long* param, int arg) {
    _addque2(func, param, 0, arg);
}

static int _addque2(void (*func)(), void* param, int len, int arg) {
    int i;
    static int i_mask_save = 0;

    set_alarm();
    while (((_qin + 1) & 0x3F) == _qout) {
        if (get_alarm()) {
            return -1;
        }
        _exeque();
    }
    i_mask_save = SetIntrMask(0);
    GEnv.cb_pending = 1;
    if (GEnv.queue_mode == 0 ||
        (_qin == _qout && !(*d2_chcr & 0x01000000) &&
         GEnv.drawsync_cb == NULL)) {
        while (!(*gp1 & 0x04000000)) {
        }
        func(param, arg);
        _qlog.func = func;
        _qlog.param = param;
        _qlog.arg = arg;
        SetIntrMask(i_mask_save);
        return 0;
    } else {
        DMACallback(2, (void (*)())_exeque);
        if (len) {
            for (i = 0; i < len / 4; i++) {
                _que[_qin].buf[i] = ((u_long*)param)[i];
            }
            _que[_qin].param = _que[_qin].buf;
        } else {
            _que[_qin].param = param;
        }
        _que[_qin].arg = arg;
        _que[_qin].func = func;
        _qin = (_qin + 1) & 0x3F;
        SetIntrMask(i_mask_save);
        _exeque();
        return (_qin - _qout) & 0x3F;
    }
}

static int _exeque(void) {
    static int i_mask_save = 0;

    if (*d2_chcr & 0x01000000) {
        return 1;
    }
    i_mask_save = SetIntrMask(0);
    while (_qin != _qout && !(*d2_chcr & 0x01000000)) {
        if (((_qout + 1) & 0x3F) == _qin && GEnv.drawsync_cb == NULL) {
            DMACallback(2, NULL);
        }
        while (!(*gp1 & 0x04000000)) {
        }
        _que[_qout].func(_que[_qout].param, _que[_qout].arg);
        _qlog.func = _que[_qout].func;
        _qlog.param = _que[_qout].param;
        _qlog.arg = _que[_qout].arg;
        _qout = (_qout + 1) & 0x3F;
    }
    SetIntrMask(i_mask_save);
    if (_qin == _qout && !(*d2_chcr & 0x01000000) && GEnv.cb_pending &&
        GEnv.drawsync_cb != NULL) {
        GEnv.cb_pending = 0;
        GEnv.drawsync_cb();
    }
    return (_qin - _qout) & 0x3F;
}

static inline int _reset(int mode) {
    static int i_mask_save = 0;

    i_mask_save = SetIntrMask(0);
    _qout = 0;
    _qin = _qout;
    switch (mode & 7) {
    case 0:
#if VERSION != 36 && VERSION != 35
    case 5:
#endif
        *d2_chcr = 0x401;
        *d_pcr |= 0x800;
        *gp1 = 0;
        memset(ctlbuf, 0, sizeof(ctlbuf));
        memset(_que, 0, sizeof(_que));
        break;
    case 1:
#if VERSION != 35
    case 3:
#endif
        *d2_chcr = 0x401;
        *d_pcr |= 0x800;
        *gp1 = 0x02000000;
        *gp1 = 0x01000000;
        break;
    }
    SetIntrMask(i_mask_save);
    if ((mode & 7) == 0) {
        return _version(mode);
    }
    return 0;
}

static int _sync(int mode) {
    int n;

    if (mode == 0) {
        set_alarm();
        while (_qin != _qout) {
            _exeque();
            if (get_alarm()) {
                return -1;
            }
        }
        while ((*d2_chcr & 0x01000000) || !(*gp1 & 0x04000000)) {
            if (get_alarm()) {
                return -1;
            }
        }
        return 0;
    }
    n = (_qin - _qout) & 0x3F;
    if (n) {
        _exeque();
    }
    if ((*d2_chcr & 0x01000000) || !(*gp1 & 0x04000000)) {
        return n ? n : 1;
    }
    return n;
}

static int target = 0;
static int nenter = 0;

static void set_alarm(void) {

    target = VSync(-1) + 240;
    nenter = 0;
}

static int get_alarm(void) {
    if (target < VSync(-1) || nenter++ > 0xF0000) {
        *gp1;
        printf("GPU timeout:que=%d,stat=%08x,chcr=%08x,madr=%08x,",
               (_qin - _qout) & 0x3F, *gp1, *d2_chcr, *d2_madr);
        printf("func=(%08x)(%08x,%08x)\n", _qlog.func, _qlog.param,
               _qlog.arg);
        _reset(1);
        return -1;
    }
    return 0;
}

static int _version(int mode) {
    *gp1 = 0x10000007;
    if ((*gp0 & 0xFFFFFF) != 2) {
        *gp0 = (*gp1 & 0x3FFF) | 0xE1001000;
        *gp0;
        if (!(*gp1 & 0x1000)) {
            return 0;
        }
        if (!(mode & 8)) {
            return 1;
        }
        *gp1 = 0x20000504;
        return 2;
    }
    if (!(mode & 8)) {
        return 3;
    }
    *gp1 = 0x09000001;
    return 4;
}

static void memset(void* ptr, int value, int num) {
    u_char* p = ptr;

    while (num--) {
        *p++ = value;
    }
}

#else

#define LOW(x) (*(s32*)&(x))

typedef struct {
     const char* rcsid;
     void (*addque)(void (*)(), u_long*, int);
     int (*addque2)(void (*)(), void*, int, int);
     int (*clr)(RECT*, int);
     void (*ctl)(u_long);
     int (*cwb)(u_long*, int);
     void (*cwc)(u_long*);
     int (*drs)(RECT*, u_long*);
     int (*dws)(RECT*, u_long*);
     int (*exeque)(void);
     int (*getctl)(int);
     int (*otc)(u_long*, int);
     int (*param)(int);
     int (*reset)(int);
     u_long (*status)(void);
     int (*sync)(int);
} gpu;

typedef struct {
     u_char version;
     u_char queue_mode;
     u_char level;
     u_char reverse;
     short w;
     short h;
     volatile int cb_pending;
     void (*drawsync_cb)();
     DRAWENV draw;
     DISPENV disp;
} GpuCtl;

typedef struct {
     void (*func)();
     void* param;
     int arg;
     u_long buf[0x15];
} QueueItem;

typedef struct {
     void (*func)();
     void* param;
     int arg;
     int unk;
} QueueLog;

static void _addque(void (*func)(), u_long* param, int arg);
static int _addque2(void (*func)(), void* param, int len, int arg);
static int _clr(RECT* rect, int color);
static void _ctl(u_long cmd);
static int _cwb(u_long* p, int n);
static void _cwc(u_long* p);
static int _drs(RECT* rect, u_long* p);
static int _dws(RECT* rect, u_long* p);
static int _exeque(void);
static int _getctl(int n);
static int _otc(u_long* ot, int n);
static int _param(int n);
static int _reset(int mode);
static u_long _status(void);
static int _sync(int mode);
static void checkRECT(const char* log, RECT* r);
static u_long get_mode(int dfe, int dtd, int tpage);
static u_long get_cs(short x, short y);
static u_long get_ce(short x, short y);
static u_long get_ofs(short x, short y);
static u_long get_tw(RECT* tw);
static u_long get_dx(DISPENV* env);
static void set_alarm(void);
static int get_alarm(void);
static int _version(int mode);
static void memset(void* ptr, int value, int num);

int printf(const char*, ...);
void* memcpy(void*, const void*, int);
void GPU_cw(u_long);
void ResetCallback(void);
int GetVideoMode(void);
int DMACallback(int, void (*)());
int SetIntrMask(int);
int VSync(int);

static gpu D_8002C220 = {
    "$Id: sys.c,v 1.107 1995/10/18 01:00:02 suzu Exp $",
    _addque,
    _addque2,
    _clr,
    _ctl,
    _cwb,
    _cwc,
    _drs,
    _dws,
    _exeque,
    _getctl,
    _otc,
    _param,
    _reset,
    _status,
    _sync,
};
static gpu* D_8002C260 = &D_8002C220;
int (*GPU_printf)(const char* fmt, ...) = printf;
static GpuCtl _ctlinfo = {0, 0, 0, 0, 0, 0, 0, 0, {{0, 0}}};
static int _width[] = {1024, 1024, 1024, 1024, 1024};
static int _height[] = {512, 1024, 1024, 512, 1024};

int ResetGraph(int mode) {
    if ((mode & 7) == 0) {
        printf("ResetGraph:jtb=%08x,env=%08x\n", &D_8002C220, &_ctlinfo);
        memset(&_ctlinfo, 0, sizeof(_ctlinfo));
        ResetCallback();
        GPU_cw((u_long)D_8002C260 & 0xFFFFFF);
        _ctlinfo.version = _reset(mode);
        _ctlinfo.queue_mode = 1;
        _ctlinfo.w = _width[_ctlinfo.version];
        _ctlinfo.h = _height[_ctlinfo.version];
        memset(&_ctlinfo.draw, -1, sizeof(DRAWENV));
        memset(&_ctlinfo.disp, -1, sizeof(DISPENV));
        return _ctlinfo.version;
    }
    if (_ctlinfo.level >= 2) {
        GPU_printf("ResetGraph(%d)...\n", mode);
    }
    return D_8002C260->reset(1);
}

int SetGraphReverse(int mode) {
    u_char old = _ctlinfo.reverse;

    if (_ctlinfo.level >= 2) {
        GPU_printf("SetGraphReverse(%d)...\n", mode);
    }
    _ctlinfo.reverse = mode;
    D_8002C260->ctl(D_8002C260->getctl(8) |
                    (_ctlinfo.reverse ? 0x08000080 : 0x08000000));
    if (_ctlinfo.version == 2) {
        D_8002C260->ctl(_ctlinfo.reverse ? 0x20000501 : 0x20000504);
    }
    return old;
}

int SetGraphDebug(int level) {
    u_char old = _ctlinfo.level;

    if (_ctlinfo.level = level) {
        GPU_printf("SetGraphDebug:level:%d,type:%d reverse:%d\n",
                   _ctlinfo.level, _ctlinfo.version, _ctlinfo.reverse);
    }
    return old;
}

int SetGraphQueue(int mode) {
    u_char old = _ctlinfo.queue_mode;

    if (_ctlinfo.level >= 2) {
        GPU_printf("SetGrapQue(%d)...\n", mode);
    }
    if (mode != _ctlinfo.queue_mode) {
        D_8002C260->reset(1);
        _ctlinfo.queue_mode = mode;
        DMACallback(2, NULL);
    }
    return old;
}

int GetGraphType(void) { return _ctlinfo.version; }

int GetGraphDebug(void) { return _ctlinfo.level; }

u_long DrawSyncCallback(void (*func)()) {
    void (*old)();

    if (_ctlinfo.level >= 2) {
        GPU_printf("DrawSyncCallback(%08x)...\n", func);
    }
    old = _ctlinfo.drawsync_cb;
    _ctlinfo.drawsync_cb = func;
    return (u_long)old;
}

void SetDispMask(int mask) {
    if (_ctlinfo.level >= 2) {
        GPU_printf("SetDispMask(%d)...\n", mask);
    }
    if (mask == 0) {
        memset(&_ctlinfo.disp, -1, sizeof(DISPENV));
    }
    D_8002C260->ctl(mask ? 0x03000000 : 0x03000001);
}

int DrawSync(int mode) {
    if (_ctlinfo.level >= 2) {
        GPU_printf("DrawSync(%d)...\n", mode);
    }
    return D_8002C260->sync(mode);
}

static void checkRECT(const char* log, RECT* r) {
    switch (_ctlinfo.level) {
    case 1:
        if (r->w > _ctlinfo.w || r->w + r->x > _ctlinfo.w ||
            r->y > _ctlinfo.h || r->y + r->h > _ctlinfo.h || r->w <= 0 ||
            r->x < 0 || r->y < 0 || r->h <= 0) {
            GPU_printf("%s:bad RECT", log);
            GPU_printf("(%d,%d)-(%d,%d)\n", r->x, r->y, r->w, r->h);
        }
        break;
    case 2:
        GPU_printf("%s:", log);
        GPU_printf("(%d,%d)-(%d,%d)\n", r->x, r->y, r->w, r->h);
        break;
    }
}

int ClearImage(RECT* rect, u_char r, u_char g, u_char b) {
    checkRECT("ClearImage", rect);
    return D_8002C260->addque2(
        D_8002C260->clr, rect, sizeof(RECT), (b << 0x10) | (g << 8) | r);
}

int LoadImage(RECT* rect, u_long* p) {
    checkRECT("LoadImage", rect);
    return D_8002C260->addque2(D_8002C260->dws, rect, sizeof(RECT), p);
}

int StoreImage(RECT* rect, u_long* p) {
    checkRECT("StoreImage", rect);
    return D_8002C260->addque2(D_8002C260->drs, rect, sizeof(RECT), p);
}

int MoveImage(RECT* rect, int x, int y) {
    static u_long param[5] = {0x04FFFFFF, 0x80000000};

    checkRECT("MoveImage", rect);
    if (rect->w == 0 || rect->h == 0) {
        return -1;
    }
    param[2] = LOW(rect->x);
    param[3] = y << 0x10 | (u_short)x;
    param[4] = LOW(rect->w);
    return D_8002C260->addque2(D_8002C260->cwc, param, sizeof(param), 0);
}

static u_long D_terminator[5] = {0x04FFFFFF, 0, 0, 0, 0};

OT_TYPE* ClearOTag(OT_TYPE* ot, int n) {
    if (_ctlinfo.level >= 2) {
        GPU_printf("ClearOTag(%08x,%d)...\n", ot, n);
    }
    while (--n) {
        setlen(ot, 0);
        setaddr(ot, ot + 1);
        ot++;
    }
    *ot = (u_long)D_terminator & 0xFFFFFF;
    return ot;
}

OT_TYPE* ClearOTagR(OT_TYPE* ot, int n) {
    if (_ctlinfo.level >= 2) {
        GPU_printf("ClearOTagR(%08x,%d)...\n", ot, n);
    }
    D_8002C260->otc(ot, n);
    *ot = (u_long)D_terminator & 0xFFFFFF;
    return ot;
}

void DrawPrim(void* p) {
    int len = getlen(p);
    D_8002C260->sync(0);
    D_8002C260->cwb((u_long*)&((P_TAG*)p)->r0, len);
}

void DrawOTag(OT_TYPE* p) {
    if (_ctlinfo.level >= 2) {
        GPU_printf("DrawOTag(%08x)...\n", p);
    }
    D_8002C260->addque2(D_8002C260->cwc, p, 0, 0);
}

DRAWENV* PutDrawEnv(DRAWENV* env) {
    if (_ctlinfo.level >= 2) {
        GPU_printf("PutDrawEnv(%08x)...\n", env);
    }
    SetDrawEnv(&env->dr_env, env);
    env->dr_env.tag |= 0xFFFFFF;
    D_8002C260->addque2(D_8002C260->cwc, &env->dr_env, sizeof(DR_ENV), 0);
    memcpy(&_ctlinfo.draw, env, sizeof(DRAWENV));
    return env;
}

DRAWENV* GetDrawEnv(DRAWENV* env) {
    memcpy(env, &_ctlinfo.draw, sizeof(DRAWENV));
    return env;
}

#define CLAMP(value, low, high)                                                \
    value < low ? low : (value > high ? high : value)

#define VOL(x) (*(volatile __typeof__(x)*)&(x))
#define IS_NEW_GPU(version) ((unsigned int)((version) - 1) < 2)

static inline int get_version(void) { return VOL(_ctlinfo.version); }

static inline int is_new_gpu(void) { return IS_NEW_GPU(get_version()); }

#define CLAMP_MAX(value, max)                                                  \
    ((value) < 1 ? 1 : ({                                                       \
        int _max = (max);                                                      \
        (value) > _max ? _max : (value);                                       \
    }))

DISPENV* PutDispEnv(DISPENV* env) {
    int h_start;
    int v_start;
    int cmd;
    int v_end;
    int h_end;

    cmd = 0x08000000;
    if (VOL(_ctlinfo.level) >= 2) {
        GPU_printf("PutDispEnv(%08x)...\n", env);
    }
    D_8002C260->ctl(
        0x05000000 | (IS_NEW_GPU(_ctlinfo.version)
                          ? ((env->disp.y & 0xFFF) << 0xC) |
                                (get_dx(env) & 0xFFF)
                          : ((env->disp.y & 0x3FF) << 0xA) |
                                (env->disp.x & 0x3FF)));
    if (!(LOW(_ctlinfo.disp.screen.x) == LOW(env->screen.x) &&
          LOW(_ctlinfo.disp.screen.w) == LOW(env->screen.w))) {
        env->pad0 = GetVideoMode();
        h_start = env->screen.x * 0xA + 0x260;
        v_start = env->screen.y + (env->pad0 ? 0x13 : 0x10);
        h_end = h_start + (env->screen.w ? env->screen.w * 0xA : 0xA00);
        v_end = v_start + (env->screen.h ? env->screen.h : 0xF0);
        h_start = CLAMP(h_start, 0x1F4, 0xCDA);
        h_end = CLAMP(h_end, h_start + 0x50, 0xCDA);
        v_start = CLAMP(v_start, 0, (env->pad0 ? 0x136 : 0xFE));
        v_end = CLAMP(v_end, v_start + 1, (env->pad0 ? 0x138 : 0x100));
        D_8002C260->ctl(0x06000000 | ((h_end & 0xFFF) << 0xC) |
                        (h_start & 0xFFF));
        D_8002C260->ctl(0x07000000 | ((v_end & 0x3FF) << 0xA) |
                        (v_start & 0x3FF));
    }
    if (LOW(_ctlinfo.disp.isinter) != LOW(env->isinter) ||
        !(LOW(_ctlinfo.disp.disp.x) == LOW(env->disp.x) &&
          LOW(_ctlinfo.disp.disp.w) == LOW(env->disp.w))) {
        env->pad0 = GetVideoMode();
        if (env->pad0 == 1) {
            cmd |= 0x8;
        }
        if (env->isrgb24) {
            cmd |= 0x10;
        }
        if (env->isinter) {
            cmd |= 0x20;
        }
        if (VOL(_ctlinfo.reverse)) {
            cmd |= 0x80;
        }
        if (env->disp.w > 0x118) {
            if (env->disp.w <= 0x160) {
                cmd |= 1;
            } else if (env->disp.w <= 0x190) {
                cmd |= 0x40;
            } else if (env->disp.w <= 0x230) {
                cmd |= 2;
            } else {
                cmd |= 3;
            }
        }
        if (env->disp.h <= (env->pad0 ? 0x120 : 0x100)) {
        } else {
            cmd |= 0x24;
        }
        D_8002C260->ctl(cmd);
    }
    memcpy(&_ctlinfo.disp, env, sizeof(DISPENV));
    return env;
}

DISPENV* GetDispEnv(DISPENV* env) {
    memcpy(env, &_ctlinfo.disp, sizeof(DISPENV));
    return env;
}

u_long GetODE(void) { return D_8002C260->status() >> 0x1F; }

void SetTexWindow(DR_TWIN* p, RECT* tw) {
    setlen(p, 2);
    p->code[0] = get_tw(tw);
    p->code[1] = 0;
}

void SetDrawArea(DR_AREA* p, RECT* r) {
    setlen(p, 2);
    p->code[0] = get_cs(r->x, r->y);
    p->code[1] = get_ce(r->x + r->w - 1, r->y + r->h - 1);
}

void SetDrawOffset(DR_OFFSET* p, u_short* ofs) {
    setlen(p, 2);
    p->code[0] = get_ofs(ofs[0], ofs[1]);
    p->code[1] = 0;
}

void SetPriority(DR_PRIO* p, int pbc, int pbw) {
    setlen(p, 2);
    p->code[0] = 0xE6000000 | (pbc ? 2 : 0) | (pbw ? 1 : 0);
    p->code[1] = 0;
}

void SetDrawMode(DR_MODE* p, int dfe, int dtd, int tpage, RECT* tw) {
    setlen(p, 2);
    p->code[0] = get_mode(dfe, dtd, (u_short)tpage);
    p->code[1] = get_tw(tw);
}

void SetDrawEnv(DR_ENV* dr_env_in, DRAWENV* env) {
    RECT clip_rect;
    int offset;
    DR_ENV* dr_env;

    dr_env = dr_env_in;

    dr_env->code[0] = get_cs(env->clip.x, env->clip.y);
    dr_env->code[1] =
        get_ce(env->clip.w + env->clip.x - 1, env->clip.y + env->clip.h - 1);
    dr_env->code[2] = get_ofs(env->ofs[0], env->ofs[1]);
    dr_env->code[3] = get_mode(env->dfe, env->dtd, env->tpage);
    dr_env->code[4] = get_tw(&env->tw);
    dr_env->code[5] = 0xE6000000;

    offset = 7;
    if (env->isbg != 0) {
        clip_rect.x = env->clip.x;
        clip_rect.y = env->clip.y;
        clip_rect.w = env->clip.w;
        clip_rect.h = env->clip.h;
        clip_rect.w = CLAMP_MAX(clip_rect.w, VOL(_ctlinfo.w) - 1);
        clip_rect.h = CLAMP_MAX(clip_rect.h, VOL(_ctlinfo.h) - 1);

        if ((clip_rect.x & 0x3F) || (clip_rect.w & 0x3F)) {
            clip_rect.x -= env->ofs[0];
            clip_rect.y -= env->ofs[1];
            *((int*)dr_env + offset++) =
                0x60000000 | env->b0 << 0x10 | env->g0 << 8 | env->r0;
            *((int*)dr_env + offset++) = LOW(clip_rect.x);
            *((int*)dr_env + offset++) = LOW(clip_rect.w);
            clip_rect.x += env->ofs[0];
            clip_rect.y += env->ofs[1];
        } else {
            *((int*)dr_env + offset++) =
                0x02000000 | env->b0 << 0x10 | env->g0 << 8 | env->r0;
            *((int*)dr_env + offset++) = LOW(clip_rect.x);
            *((int*)dr_env + offset++) = LOW(clip_rect.w);
        }
    }

    setlen(dr_env, offset - 1);
}

static u_long get_mode(int dfe, int dtd, int tpage) {
    if (is_new_gpu()) {
        return 0xE1000000 | (dtd ? 0x800 : 0) | (dfe ? 0x1000 : 0) |
               (tpage & 0x27FF);
    } else {
        return 0xE1000000 | (dtd ? 0x200 : 0) | (dfe ? 0x400 : 0) |
               (tpage & 0x9FF);
    }
}

static u_long get_cs(short x, short y) {
    x = CLAMP_MAX(x, VOL(_ctlinfo.w) - 1);
    y = CLAMP_MAX(y, VOL(_ctlinfo.h) - 1);
    if (is_new_gpu()) {
        return 0xE3000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE3000000 | ((y & 0x3FF) << 10) | (x & 0x3FF);
    }
}

static u_long get_ce(short x, short y) {
    x = CLAMP_MAX(x, VOL(_ctlinfo.w) - 1);
    y = CLAMP_MAX(y, VOL(_ctlinfo.h) - 1);
    if (is_new_gpu()) {
        return 0xE4000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE4000000 | ((y & 0x3FF) << 10) | (x & 0x3FF);
    }
}

static u_long get_ofs(short x, short y) {
    if (is_new_gpu()) {
        return 0xE5000000 | ((y & 0xFFF) << 12) | (x & 0xFFF);
    } else {
        return 0xE5000000 | ((y & 0x7FF) << 11) | (x & 0x7FF);
    }
}

static u_long get_tw(RECT* tw) {
    u_long pad[4];

    if (tw != NULL) {
        pad[0] = (tw->x & 0xFF) >> 3;
        pad[2] = (-tw->w & 0xFF) >> 3;
        pad[1] = (tw->y & 0xFF) >> 3;
        pad[3] = (-tw->h & 0xFF) >> 3;
        return 0xE2000000 | (pad[1] << 0xF) | (pad[0] << 0xA) | (pad[3] << 5) |
               pad[2];
    }
    return 0;
}

static u_long get_dx(DISPENV* env) {
    switch (get_version()) {
    case 1:
        return _ctlinfo.reverse ? 0x400 - env->disp.x - env->disp.w
                                : env->disp.x;
    case 2:
        return _ctlinfo.reverse ? 0x400 - env->disp.x - env->disp.w / 2
                                : env->disp.x / 2;
    }
    return env->disp.x;
}

static volatile u_long* GPU_DATA = (u_long*)0x1F801810;
static volatile u_long* GPU_STATUS = (u_long*)0x1F801814;
static volatile u_long* DMA2_MADR = (u_long*)0x1F8010A0;
static volatile u_long* DMA2_BCR = (u_long*)0x1F8010A4;
static volatile u_long* DMA2_CHCR = (u_long*)0x1F8010A8;
static volatile u_long* DMA6_MADR = (u_long*)0x1F8010E0;
static volatile u_long* DMA6_BCR = (u_long*)0x1F8010E4;
static volatile u_long* DMA6_CHCR = (u_long*)0x1F8010E8;
static volatile u_long* DPCR = (u_long*)0x1F8010F0;

static u_long _status(void) { return *GPU_STATUS; }

static int _otc(u_long* ot, int n) {
    *DPCR |= 0x08000000;
    *DMA6_CHCR = 0;
    *DMA6_MADR = (u_long)&ot[n - 1];
    *DMA6_BCR = n;
    *DMA6_CHCR = 0x11000002;
    set_alarm();
    while (*DMA6_CHCR & 0x01000000) {
        if (get_alarm()) {
            return -1;
        }
    }
    return n;
}

static int _clr(RECT* rect, int color) {
    static u_long p0[8];
    static u_long p1[8];

    rect->w = CLAMP_MAX(rect->w, VOL(_ctlinfo.w) - 1);
    rect->h = CLAMP_MAX(rect->h, VOL(_ctlinfo.h) - 1);
    if ((rect->x & 0x3F) || (rect->w & 0x3F)) {
        p0[0] = ((u_long)p1 & 0xFFFFFF) | 0x07000000;
        p0[1] = 0xE3000000;
        p0[2] = 0xE4FFFFFF;
        p0[3] = 0xE5000000;
        p0[4] = 0xE6000000;
        p0[5] = (color & 0xFFFFFF) | 0x60000000;
        p0[6] = LOW(rect->x);
        p0[7] = LOW(rect->w);
        p1[0] = 0x03FFFFFF;
        p1[1] = _param(3) | 0xE3000000;
        p1[2] = _param(4) | 0xE4000000;
        p1[3] = _param(5) | 0xE5000000;
    } else {
        p0[0] = 0x04FFFFFF;
        p0[1] = 0xE6000000;
        p0[2] = (color & 0xFFFFFF) | 0x02000000;
        p0[3] = LOW(rect->x);
        p0[4] = LOW(rect->w);
    }
    _cwc(p0);
    return 0;
}

static int _dws(RECT* rect, u_long* p) {
    int size;
    int n;
    int i;
    int isbg;

    set_alarm();
    isbg = 0;
    rect->w = CLAMP_MAX(rect->w, VOL(_ctlinfo.w) - 1);
    rect->h = CLAMP_MAX(rect->h, VOL(_ctlinfo.h) - 1);
    size = ((rect->w * rect->h) + 1) / 2;
    if (size <= 0) {
        return -1;
    }
    i = size % 16;
    n = size / 16;
    while (!(*GPU_STATUS & 0x04000000)) {
        if (get_alarm()) {
            return -1;
        }
    }
    *GPU_STATUS = 0x04000000;
    *GPU_DATA = 0x01000000;
    *GPU_DATA = isbg ? 0xB0000000 : 0xA0000000;
    *GPU_DATA = LOW(rect->x);
    *GPU_DATA = LOW(rect->w);
    while (i--) {
        *GPU_DATA = *p++;
    }
    if (n != 0) {
        *GPU_STATUS = 0x04000002;
        *DMA2_MADR = (u_long)p;
        *DMA2_BCR = (n << 0x10) | 0x10;
        *DMA2_CHCR = 0x01000201;
    }
    return 0;
}

static int _drs(RECT* rect, u_long* p) {
    int size;
    int n;
    int i;

    set_alarm();
    rect->w = CLAMP_MAX(rect->w, VOL(_ctlinfo.w) - 1);
    rect->h = CLAMP_MAX(rect->h, VOL(_ctlinfo.h) - 1);
    size = ((rect->w * rect->h) + 1) / 2;
    if (size <= 0) {
        return -1;
    }
    i = size % 16;
    n = size / 16;
    while (!(*GPU_STATUS & 0x04000000)) {
        if (get_alarm()) {
            return -1;
        }
    }
    *GPU_STATUS = 0x04000000;
    *GPU_DATA = 0x01000000;
    *GPU_DATA = 0xC0000000;
    *GPU_DATA = LOW(rect->x);
    *GPU_DATA = LOW(rect->w);
    while (!(*GPU_STATUS & 0x08000000)) {
        if (get_alarm()) {
            return -1;
        }
    }
    while (i--) {
        *p++ = *GPU_DATA;
    }
    if (n != 0) {
        *GPU_STATUS = 0x04000003;
        *DMA2_MADR = (u_long)p;
        *DMA2_BCR = (n << 0x10) | 0x10;
        *DMA2_CHCR = 0x01000200;
    }
    return 0;
}

static u_char ctlbuf[0x100];

static void _ctl(u_long cmd) {
    *GPU_STATUS = cmd;
    ctlbuf[cmd >> 0x18] = cmd;
}

static int _getctl(int n) { return ctlbuf[n]; }

static int _cwb(u_long* p, int n) {
    *GPU_STATUS = 0x04000000;
    while (n--) {
        *GPU_DATA = *p++;
    }
    return 0;
}

static void _cwc(u_long* p) {
    *GPU_STATUS = 0x04000002;
    *DMA2_MADR = (u_long)p;
    *DMA2_BCR = 0;
    *DMA2_CHCR = 0x01000401;
}

static int _param(int n) {
    *GPU_STATUS = 0x10000000 | n;
    return *GPU_DATA & 0xFFFFFF;
}

volatile QueueLog _qlog = {0, 0, 0, 0};
volatile int _qin = 0;
volatile int _qout = 0;
volatile QueueItem _que[0x40];

static void _addque(void (*func)(), u_long* param, int arg) {
    _addque2(func, param, 0, arg);
}

static int _addque2_mask = 0;

static int _addque2(void (*func)(), void* param, int len, int arg) {
    int i;

    set_alarm();
    while (((_qin + 1) & 0x3F) == _qout) {
        if (get_alarm()) {
            return -1;
        }
        _exeque();
    }
    _addque2_mask = SetIntrMask(0);
    _ctlinfo.cb_pending = 1;
    if (_ctlinfo.queue_mode == 0 ||
        (_qin == _qout && !(*DMA2_CHCR & 0x01000000) &&
         _ctlinfo.drawsync_cb == NULL)) {
        while (!(*GPU_STATUS & 0x10000000)) {
        }
        func(param, arg);
        _qlog.func = func;
        _qlog.param = param;
        _qlog.arg = arg;
    } else {
        DMACallback(2, (void (*)())_exeque);
        if (len) {
            for (i = 0; i < len / 4; i++) {
                _que[_qin].buf[i] = ((u_long*)param)[i];
            }
            _que[_qin].param = _que[_qin].buf;
        } else {
            _que[_qin].param = param;
        }
        _que[_qin].arg = arg;
        _que[_qin].func = func;
        _qin = (_qin + 1) & 0x3F;
        SetIntrMask(_addque2_mask);
        _exeque();
        return (_qin - _qout) & 0x3F;
    }
    SetIntrMask(_addque2_mask);
    return 0;
}

static int _exeque_mask = 0;

static int _exeque(void) {
    if (*DMA2_CHCR & 0x01000000) {
        return 1;
    }
    _exeque_mask = SetIntrMask(0);
    while (_qin != _qout && !(*DMA2_CHCR & 0x01000000)) {
        if (((_qout + 1) & 0x3F) == _qin && _ctlinfo.drawsync_cb == NULL) {
            DMACallback(2, NULL);
        }
        while (!(*GPU_STATUS & 0x10000000)) {
        }
        _que[_qout].func(_que[_qout].param, _que[_qout].arg);
        _qlog.func = _que[_qout].func;
        _qlog.param = _que[_qout].param;
        _qlog.arg = _que[_qout].arg;
        _qout = (_qout + 1) & 0x3F;
    }
    SetIntrMask(_exeque_mask);
    if (_qin == _qout && !(*DMA2_CHCR & 0x01000000) && _ctlinfo.cb_pending &&
        _ctlinfo.drawsync_cb != NULL) {
        _ctlinfo.cb_pending = 0;
        _ctlinfo.drawsync_cb();
    }
    return (_qin - _qout) & 0x3F;
}

static int _reset_mask = 0;

static inline int _reset(int mode) {
    _reset_mask = SetIntrMask(0);
    _qout = 0;
    _qin = _qout;
    switch (mode & 7) {
    case 0:
        *DMA2_CHCR = 0x401;
        *DPCR |= 0x800;
        *GPU_STATUS = 0;
        memset(ctlbuf, 0, sizeof(ctlbuf));
        memset(_que, 0, sizeof(_que));
        break;
    case 1:
        *DMA2_CHCR = 0x401;
        *DPCR |= 0x800;
        *GPU_STATUS = 0x02000000;
        *GPU_STATUS = 0x01000000;
        break;
    }
    SetIntrMask(_reset_mask);
    if ((mode & 7) == 0) {
        return _version(mode);
    }
    return 0;
}

static int _sync(int mode) {
    int n;

    if (mode == 0) {
        set_alarm();
        while (_qin != _qout) {
            _exeque();
            if (get_alarm()) {
                return -1;
            }
        }
        while ((*DMA2_CHCR & 0x01000000) || !(*GPU_STATUS & 0x04000000)) {
            if (get_alarm()) {
                return -1;
            }
        }
        return 0;
    }
    n = (_qin - _qout) & 0x3F;
    if (n) {
        _exeque();
    }
    if ((*DMA2_CHCR & 0x01000000) || !(*GPU_STATUS & 0x04000000)) {
        return n ? n : 1;
    }
    return n;
}

static int D_alarm_time = 0;
static int D_alarm_count = 0;

static void set_alarm(void) {

    D_alarm_time = VSync(-1) + 240;
    D_alarm_count = 0;
}

static int get_alarm(void) {
    if (D_alarm_time < VSync(-1) || D_alarm_count++ > 0xF0000) {
        *GPU_STATUS;
        printf("GPU timeout:que=%d,stat=%08x,chcr=%08x,madr=%08x,",
               (_qin - _qout) & 0x3F, *GPU_STATUS, *DMA2_CHCR, *DMA2_MADR);
        printf("func=(%08x)(%08x,%08x)\n", _qlog.func, _qlog.param,
               _qlog.arg);
        _reset(1);
        return -1;
    }
    return 0;
}

static int _version(int mode) {
    *GPU_STATUS = 0x10000007;
    if ((*GPU_DATA & 0xFFFFFF) != 2) {
        *GPU_DATA = (*GPU_STATUS & 0x3FFF) | 0xE1001000;
        *GPU_DATA;
        if (!(*GPU_STATUS & 0x1000)) {
            return 0;
        }
        if (!(mode & 8)) {
            return 1;
        }
        *GPU_STATUS = 0x20000504;
        return 2;
    }
    if (!(mode & 8)) {
        return 3;
    }
    *GPU_STATUS = 0x09000001;
    return 4;
}

static void memset(void* ptr, int value, int num) {
    u_char* p = ptr;

    while (num--) {
        *p++ = value;
    }
}
#endif
