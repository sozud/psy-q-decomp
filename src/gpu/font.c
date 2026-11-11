#include "common.h"
#include <psxsdk/libgpu.h>
#include <psxsdk/stdarg.h>

#define LOW(x) (*(s32*)&(x))

#if VERSION <= 30

#if VERSION == 26
static char rcsid[] = "$Id: font.c,v 1.51 1995/01/12 11:35:19 suzu Exp $";
#endif

typedef struct {
     TILE tile;
     DR_MODE draw_mode;
     s32 capacity;
     SPRT_8* sprites;
     char* buffer;
     s32 written;
} FntStream;

extern int (*GPU_printf)(const char*, ...);

static u16 tpage;
static u16 clut;

FntStream Font[4] = {{{0, 0, 0, 0, 0, 0}}};
static s32 D_8002B810 = 0;
static s32 D_8002B814 = 0;
extern u16 font[];

void SetDumpFnt(int id) {
    if (id >= 0 && D_8002B810 >= id) {
        D_8002B814 = id;
        GPU_printf = FntPrint;
    }
}

void FntLoad(s32 tx, s32 ty) {
    clut = LoadClut(font, tx, ty + 0x80);
    tpage = LoadTPage(font + 0x100, 0, 0, tx, ty, 0x80, 0x20);
    D_8002B810 = 0;
    memset(Font, 0, sizeof(Font));
}

static s32 D_8002C218 = 0;

s32 FntOpen(s32 x, s32 y, s32 w, s32 h, s32 isbg, s32 n) {
    static char str[0x400];
    static SPRT_8 sprt[0x400];
    s32 i;
    SPRT_8* sprites;

    if (D_8002B810 >= 4) {
        return -1;
    }
    if (D_8002B810 == 0) {
        D_8002C218 = 0;
    }
    if ((n + D_8002C218) > 0x400) {
        n = 0x400 - D_8002C218;
    }
    SetDrawMode(&Font[D_8002B810].draw_mode, 0, 0, tpage, NULL);
    if (isbg) {
        SetTile(&Font[D_8002B810].tile);
        (*&Font[D_8002B810].tile).r0 = 0;
        (*&Font[D_8002B810].tile).g0 = 0;
        (*&Font[D_8002B810].tile).b0 = 0;
    }
    (*&Font[D_8002B810].tile).x0 = x;
    (*&Font[D_8002B810].tile).y0 = y;
    (*&Font[D_8002B810].tile).w = w;
    (*&Font[D_8002B810].tile).h = h;
    Font[D_8002B810].capacity = n;
    Font[D_8002B810].buffer = &str[D_8002C218];
    Font[D_8002B810].sprites = &sprt[D_8002C218];
    Font[D_8002B810].written = 0;
    *Font[D_8002B810].buffer = 0;
    sprites = Font[D_8002B810].sprites;
    for (i = 0; i < n; i++) {
        SetSprt8(sprites);
        SetShadeTex(sprites, true);
        sprites->clut = clut;
        sprites++;
    }
    D_8002C218 += n;
    return D_8002B810++;
}

u_long* FntFlush(s32 id) {
    DR_MODE* dr;
    s32 x, y;
    s32 max_x, max_y;
    s32 len;
    bool line_break;
    s32 ch;
    char* buf;
    FntStream* font;
    SPRT_8* sprt;
    u8 u, v;

    if (id < 0 || id >= D_8002B810) {
        id = D_8002B814;
        if (Font[id].buffer == NULL) {
            return NULL;
        }
    }
    font = &Font[id];
    dr = &font->draw_mode;
    buf = font->buffer;
    sprt = font->sprites;
    len = font->capacity;
    x = font->tile.x0;
    y = font->tile.y0;
    max_x = x + font->tile.w;
    max_y = y + font->tile.h;
    TermPrim(dr);
    while (*buf != 0 && len != 0) {
        line_break = false;
        switch (*buf) {
        case '\n':
            line_break = true;
            break;

        case '\t':
            x += 0x20;
            if (x >= max_x) {
                line_break = true;
            }
            break;

        case ' ':
            x += 8;
            if (x >= max_x) {
                line_break = true;
            }
            break;

        default:
            if (*buf >= 'a' && *buf <= 'z') {
                ch = *buf - 0x40;
            } else {
                ch = *buf - 0x20;
            }
            u = (ch % 16) * 8;
            v = (ch / 16) * 8;
            sprt->u0 = u;
            sprt->v0 = v;
            sprt->x0 = x;
            sprt->y0 = y;
            AddPrim(dr, &sprt->tag);
            x += 8;
            if (x >= max_x) {
                line_break = true;
            }
            break;
        }
        if (line_break) {
            x = font->tile.x0;
            y += 8;
            if (y >= max_y) {
                break;
            }
        }
        sprt++;
        buf++;
        len--;
    }
    if (font->tile.code != 0) {
        AddPrim(dr, &font->tile.tag);
    }
    DrawOTag((u_long*)dr);
    font->written = 0;
    *font->buffer = 0;
    return (u_long*)dr;
}

#if VERSION == 26
#define WriteChar(c) font->buffer[font->written++] = (c)
#else
#define WriteChar(c)                                                           \
    font->buffer[font->written++] = (c);                                       \
    if (font->written > font->capacity) {                                      \
        return -1;                                                             \
    }
#endif

static char* D_8002C21C = "0123456789ABCDEF";

int FntPrint(const char* id, ...) {
    char buf[0x200];
    va_list args;
    FntStream* font;
    u8 padZeros;
    s32 num;
    s32 len;
    s32 width;
    char* f;
    char* bufPtr;
    char sign;
    u32 ch;

    va_start(args, id);
    if (LOW(id) < 0 || LOW(id) >= D_8002B810) {
        f = (char*)id;
        id = D_8002B814;
        if (Font[LOW(id)].buffer == NULL) {
            return -1;
        }
    } else {
        f = va_arg(args, char*);
    }

    font = &Font[LOW(id)];
    if (font->written > font->capacity) {
        return -1;
    }
    for (; ch = *f, ch; ++f) {
        if (ch != '%') {
            WriteChar(ch);
            continue;
        }

        ch = *++f;
        if (ch == '%') {
            WriteChar('%');
            continue;
        }

        width = 0;
        padZeros = ch == '0';
        while (ch >= '0' && ch <= '9') {
            width = (width * 10) + (ch - '0');
            ch = *++f;
        }
        bufPtr = &args;
        if (width <= 0) {
            width = 1;
        }

        switch (ch) {
        case 'd':
            num = va_arg(args, s32);
            sign = 0;
            if (num < 0) {
                num = -num;
                sign = '-';
            }
            len = 0;
            do {
                *--bufPtr = (num % 10U) + '0';
                num /= 10U;
                len++;
            } while (len == 0 || num != 0);
            if (sign != 0) {
                *--bufPtr = sign;
                len++;
            }
            break;

        case 'X':
        case 'x':
            len = 0;
            num = va_arg(args, s32);
            do {
                *--bufPtr = D_8002C21C[num % 16U];
                num /= 16U;
                len++;
            } while (len == 0 || num != 0);
            if (padZeros) {
                while (len < width) {
                    *--bufPtr = '0';
                    len++;
                }
            }
            break;

        case 'c':
            *--bufPtr = va_arg(args, char);
            len = 1;
            break;

        case 's':
            bufPtr = va_arg(args, char*);
            len = strlen(bufPtr);
            break;
        }

        while (len < width) {
            WriteChar(' ');
            width--;
        }
        len--;
        while (len != -1) {
            WriteChar(*bufPtr++);
            len--;
        }
    }
    font->buffer[font->written] = 0;
    return font->written;
}

#elif VERSION == 31

typedef struct {
     TILE tile;
     DR_MODE draw_mode;
     s32 capacity;
     SPRT_8* sprites;
     char* buffer;
     s32 written;
} FntStream;

extern int (*GPU_printf)(const char*, ...);

static char D_80033A18[0x400];
static SPRT_8 D_80033E18[0x400];
static u16 tpage;
STATIC_PAD_BSS(2);
static u16 clut;
STATIC_PAD_BSS(2);

static FntStream Font[4] = {0};
static s32 D_8002B810 = 0;
static s32 D_8002B814 = 0;
static u16 kanjiclut[] = {
#include "font_kanjiclut.h"
};
static u8 D_8002BA18[] = {
#include "font_bitmap.h"
};
static s32 D_8002C218 = 0;
static char* D_8002C21C = "0123456789ABCDEF";

void SetDumpFnt(int id) {
    if (id >= 0 && D_8002B810 >= id) {
        D_8002B814 = id;
        GPU_printf = FntPrint;
    }
}

void FntLoad(s32 tx, s32 ty) {
    clut = LoadClut(kanjiclut, tx, ty + 0x80);
    tpage = LoadTPage(kanjiclut + 0x100, 0, 0, tx, ty, 0x80, 0x20);
    D_8002B810 = 0;
    memset(Font, 0, sizeof(Font));
}

s32 FntOpen(s32 x, s32 y, s32 w, s32 h, s32 isbg, s32 n) {
    s32 i;
    SPRT_8* sprites;

    if (D_8002B810 >= 4) {
        return -1;
    }
    if (D_8002B810 == 0) {
        D_8002C218 = 0;
    }
    if ((n + D_8002C218) > 0x400) {
        n = 0x400 - D_8002C218;
    }
    SetDrawMode(&Font[D_8002B810].draw_mode, 0, 0, tpage, NULL);
    if (isbg) {
        SetTile(&Font[D_8002B810].tile);
        (*&Font[D_8002B810].tile).r0 = 0;
        (*&Font[D_8002B810].tile).g0 = 0;
        (*&Font[D_8002B810].tile).b0 = 0;
    }
    (*&Font[D_8002B810].tile).x0 = x;
    (*&Font[D_8002B810].tile).y0 = y;
    (*&Font[D_8002B810].tile).w = w;
    (*&Font[D_8002B810].tile).h = h;
    Font[D_8002B810].capacity = n;
    Font[D_8002B810].written = 0;
    Font[D_8002B810].buffer = &D_80033A18[D_8002C218];
    Font[D_8002B810].sprites = &D_80033E18[D_8002C218];
    *Font[D_8002B810].buffer = 0;
    sprites = Font[D_8002B810].sprites;
    for (i = 0; i < n; i++) {
        SetSprt8(sprites);
        SetShadeTex(sprites, true);
        sprites->clut = clut;
        sprites++;
    }
    D_8002C218 += n;
    return D_8002B810++;
}

u_long* FntFlush(s32 id) {
    DR_MODE* dr;
    s32 x, y;
    s32 max_x, max_y;
    s32 len;
    bool line_break;
    s32 ch;
    char* buf;
    FntStream* font;
    SPRT_8* sprt;
    u8 u, v;

    if (id < 0 || id >= D_8002B810) {
        id = D_8002B814;
        if (Font[id].buffer == NULL) {
            return NULL;
        }
    }
    font = &Font[id];
    dr = &font->draw_mode;
    buf = font->buffer;
    sprt = font->sprites;
    len = font->capacity;
    x = font->tile.x0;
    y = font->tile.y0;
    max_x = x + font->tile.w;
    max_y = y + font->tile.h;
    TermPrim(dr);
    while (*buf != 0) {
        if (len == 0) {
            break;
        }
        line_break = false;
        switch (*buf) {
        case '\n':
            line_break = true;
            break;

        case '\t':
            x += 0x20;
            if (x >= max_x) {
                line_break = true;
            }
            break;

        case ' ':
            x += 8;
            if (x >= max_x) {
                line_break = true;
            }
            break;

        default:
            if (*buf >= 'a' && *buf <= 'z') {
                ch = *buf - 0x40;
            } else {
                ch = *buf - 0x20;
            }
            u = (ch % 16) * 8;
            v = (ch / 16) * 8;
            sprt->u0 = u;
            sprt->v0 = v;
            sprt->x0 = x;
            sprt->y0 = y;
            AddPrim(dr, &sprt->tag);
            x += 8;
            if (x >= max_x) {
                line_break = true;
            }
            break;
        }
        if (line_break) {
            x = font->tile.x0;
            y += 8;
            if (y >= max_y) {
                break;
            }
        }
        sprt++;
        buf++;
        len--;
    }
    if (font->tile.code != 0) {
        AddPrim(dr, &font->tile.tag);
    }
    DrawOTag((u_long*)dr);
    font->written = 0;
    *font->buffer = 0;
    return (u_long*)dr;
}

#define WriteChar(c)                                                           \
    font->buffer[font->written++] = (c);                                       \
    if (font->written > font->capacity) {                                      \
        return -1;                                                             \
    }

int FntPrint(const char* id, ...) {
    char buf[0x200];
    va_list args;
    FntStream* font;
    u8 padZeros;
    s32 num;
    s32 len;
    s32 width;
    char* f;
    char* bufPtr;
    char sign;
    u32 ch;

    va_start(args, id);
    if (LOW(id) < 0 || LOW(id) >= D_8002B810) {
        f = (char*)id;
        id = D_8002B814;
        if (Font[LOW(id)].buffer == NULL) {
            return -1;
        }
    } else {
        f = va_arg(args, char*);
    }

    font = &Font[LOW(id)];
    if (font->written > font->capacity) {
        return -1;
    }
    for (; ch = *f, ch; ++f) {
        if (ch != '%') {
            WriteChar(ch);
            continue;
        }

        ch = *++f;
        if (ch == '%') {
            WriteChar('%');
            continue;
        }

        width = 0;
        padZeros = ch == '0';
        while (ch >= '0' && ch <= '9') {
            width = (width * 10) + (ch - '0');
            ch = *++f;
        }
        bufPtr = &args;
        if (width <= 0) {
            width = 1;
        }

        switch (ch) {
        case 'd':
            num = va_arg(args, s32);
            sign = 0;
            if (num < 0) {
                num = -num;
                sign = '-';
            }
            len = 0;
            do {
                do {
                    *--bufPtr = (num % 10U) + '0';
                    num /= 10U;
                    len++;
                } while (len == 0);
            } while (num != 0);
            if (sign != 0) {
                *--bufPtr = sign;
                len++;
            }
            break;

        case 'X':
        case 'x':
            len = 0;
            num = va_arg(args, s32);
            do {
                do {
                    *--bufPtr = D_8002C21C[num % 16U];
                    num /= 16U;
                    len++;
                } while (len == 0);
            } while (num != 0);
            if (padZeros) {
                while (len < width) {
                    *--bufPtr = '0';
                    len++;
                }
            }
            break;

        case 'c':
            *--bufPtr = va_arg(args, char);
            len = 1;
            break;

        case 's':
            bufPtr = va_arg(args, char*);
            len = strlen(bufPtr);
            break;
        }

        while (len < width) {
            WriteChar(' ');
            width--;
        }
        len--;
        while (len != -1) {
            WriteChar(*bufPtr++);
            len--;
        }
    }
    font->buffer[font->written] = 0;
    return font->written;
}

#else

typedef struct {
     TILE tile;
     DR_MODE draw_mode;
     s32 capacity;
     SPRT_8* sprites;
     char* buffer;
     s32 written;
     s32 autosize;
} FntStream;

extern int (*GPU_printf)(const char*, ...);

static u16 tpage;
static u16 clut;

FntStream Font[8] = {{{0, 0, 0, 0, 0, 0}}};
static s32 D_8002B810 = 0;
static s32 D_8002B814 = 0;

void SetDumpFnt(int id) {
    if (id >= 0 && D_8002B810 >= id) {
        D_8002B814 = id;
        GPU_printf = FntPrint;
    }
}

static u16 kanjiclut[] = {
#include "font_kanjiclut.h"
};
static u8 D_8002BA18[] = {
#include "font_bitmap.h"
};

void FntLoad(s32 tx, s32 ty) {
#if VERSION >= 35
    clut = LoadClut2(kanjiclut, tx, ty + 0x80);
#else
    clut = LoadClut(kanjiclut, tx, ty + 0x80);
#endif
    tpage = LoadTPage(kanjiclut + 0x100, 0, 0, tx, ty, 0x80, 0x20);
    D_8002B810 = 0;
    memset(Font, 0, sizeof(Font));
}

static s32 D_8002C218 = 0;

s32 FntOpen(s32 x, s32 y, s32 w, s32 h, s32 isbg, s32 n) {
    static char str[0x400];
    static SPRT_8 sprt[0x400];
    s32 i;
    SPRT_8* sprites;
    RECT rect;

    if (D_8002B810 >= 8) {
        return -1;
    }
    if (D_8002B810 == 0) {
        D_8002C218 = 0;
    }
    Font[D_8002B810].autosize = w == 0;
#if VERSION >= 37
    if ((D_8002C218 + n) > 0x400) {
#else
    if ((n + D_8002C218) > 0x400) {
#endif
        n = 0x400 - D_8002C218;
    }
    rect.x = 0;
    rect.y = 0;
    rect.w = 0x100;
    rect.h = 0x100;
    SetDrawMode(&Font[D_8002B810].draw_mode, 0, 0, tpage, &rect);
    if (isbg) {
        SetTile(&Font[D_8002B810].tile);
        (*&Font[D_8002B810].tile).r0 = 0;
        (*&Font[D_8002B810].tile).g0 = 0;
        (*&Font[D_8002B810].tile).b0 = 0;
        SetSemiTrans(&Font[D_8002B810].tile, isbg == 2);
    }
    (*&Font[D_8002B810].tile).x0 = x;
    (*&Font[D_8002B810].tile).y0 = y;
    (*&Font[D_8002B810].tile).w = w;
    (*&Font[D_8002B810].tile).h = h;
    Font[D_8002B810].capacity = n;
    Font[D_8002B810].written = 0;
    Font[D_8002B810].buffer = &str[D_8002C218];
    Font[D_8002B810].sprites = &sprt[D_8002C218];
    *Font[D_8002B810].buffer = 0;
    sprites = Font[D_8002B810].sprites;
    for (i = 0; i < n; i++) {
        SetSprt8(sprites);
        sprites->clut = clut;
        sprites++;
    }
    D_8002C218 += n;
    return D_8002B810++;
}

u_long* FntFlush(s32 id) {
    DR_MODE* dr;
    s32 x, y;
    s32 width;
    s32 max_x, max_y;
    s32 len;
    s32 line_break;
    s32 ch;
    char* buf;
    FntStream* font;
    SPRT_8* sprt;
    s32 autosize;
    s32 r, g, b;
    u8 u, v;

    r = 0x80;
    g = 0x80;
    width = 0;
    b = 0x80;
    if (id < 0 || id >= D_8002B810) {
        id = D_8002B814;
        if (Font[id].buffer == NULL) {
            return NULL;
        }
    }
    font = &Font[id];
    dr = &font->draw_mode;
    buf = font->buffer;
    len = font->capacity;
    x = font->tile.x0;
    y = font->tile.y0;
    max_x = x + font->tile.w;
    max_y = y + font->tile.h;
    sprt = font->sprites;
    autosize = font->autosize;
    TermPrim(dr);
    while (*buf != 0) {
        if (len == 0) {
            break;
        }
        line_break = 0;
        switch (*buf) {
        case '~':
            buf++;
            if (*buf == 'c') {
                buf++;
                r = (*buf++ - '0') << 4;
                g = (*buf++ - '0') << 4;
                b = (*buf - '0') << 4;
            }
            break;

        case '\n':
            line_break = 1;
            break;

        case '\t':
            x += 0x20;
            if (x >= max_x && !autosize) {
                line_break = 1;
            }
            break;

        case ' ':
            x += 8;
            if (x >= max_x && !autosize) {
                line_break = 1;
            }
            break;

        default:
            if (*buf >= 'a' && *buf <= 'z') {
                ch = *buf - 0x40;
            } else {
                ch = *buf - 0x20;
            }
            u = (ch % 16) * 8;
            v = (ch / 16) * 8;
            sprt->u0 = u;
            sprt->v0 = v;
            sprt->x0 = x;
            sprt->y0 = y;
            sprt->r0 = r;
            sprt->g0 = g;
            sprt->b0 = b;
            AddPrim(dr, sprt++);
            x += 8;
            if (x >= max_x && !autosize) {
                line_break = 1;
            }
            break;
        }
        if (line_break) {
            if (width < x) {
                width = x;
            }
            y += 8;
            x = font->tile.x0;
            if (y >= max_y) {
                break;
            }
        }
        buf++;
        len--;
    }
    if (font->tile.code != 0) {
        AddPrim(dr, &font->tile);
        if (autosize) {
            font->tile.w = width - font->tile.x0;
            font->tile.h = y - (s16)(font->tile.y0 - 8);
        }
    }
    DrawOTag((u_long*)dr);
    font->written = 0;
    *font->buffer = 0;
    return (u_long*)dr;
}

#define WriteChar(c)                                                           \
    font->buffer[font->written++] = (c);                                       \
    if (font->written > font->capacity) {                                      \
        return -1;                                                             \
    }

static char* D_8002C21C = "0123456789ABCDEF";

int FntPrint(const char* id, ...) {
    char buf[0x200];
    va_list args;
    FntStream* font;
    u8 padZeros;
    s32 num;
    s32 len;
    s32 width;
    char* f;
    char* bufPtr;
    char sign;
    u32 ch;

    va_start(args, id);
    if (LOW(id) < 0 || LOW(id) >= D_8002B810) {
        f = (char*)id;
        id = D_8002B814;
        if (Font[LOW(id)].buffer == NULL) {
            return -1;
        }
    } else {
        f = va_arg(args, char*);
    }

    font = &Font[LOW(id)];
    if (font->written > font->capacity) {
        return -1;
    }
    for (; ch = *f, ch; ++f) {
        if (ch != '%') {
            WriteChar(ch);
            continue;
        }

        ch = *++f;
        if (ch == '%') {
            WriteChar('%');
            continue;
        }

        width = 0;
        padZeros = ch == '0';
        while (ch >= '0' && ch <= '9') {
            width = (width * 10) + (ch - '0');
            ch = *++f;
        }
#if VERSION >= 37
        if (width <= 0) {
            width = 1;
        }
        bufPtr = &args;
#else
        bufPtr = &args;
        if (width <= 0) {
            width = 1;
        }
#endif

        switch (ch) {
        case 'd':
            num = va_arg(args, s32);
            sign = 0;
            if (num < 0) {
                num = -num;
                sign = '-';
            }
            len = 0;
#if VERSION >= 37
            do {
                *--bufPtr = (num % 10U) + '0';
                num /= 10U;
                len++;
            } while (len == 0 || num != 0);
#else
            do {
                do {
                    *--bufPtr = (num % 10U) + '0';
                    num /= 10U;
                    len++;
                } while (len == 0);
            } while (num != 0);
#endif
            if (sign != 0) {
                *--bufPtr = sign;
                len++;
            }
            break;

        case 'X':
        case 'x':
            len = 0;
            num = va_arg(args, s32);
            do {
                do {
                    *--bufPtr = D_8002C21C[num % 16U];
                    num /= 16U;
                    len++;
                } while (len == 0);
            } while (num != 0);
            if (padZeros) {
                while (len < width) {
                    *--bufPtr = '0';
                    len++;
                }
            }
            break;

        case 'c':
#if VERSION >= 37
            *--bufPtr = va_arg(args, int);
#else
            *--bufPtr = va_arg(args, char);
#endif
            len = 1;
            break;

        case 's':
            bufPtr = va_arg(args, char*);
            len = strlen(bufPtr);
            break;
        }

        while (len < width) {
            WriteChar(' ');
            width--;
        }
        len--;
        while (len != -1) {
            WriteChar(*bufPtr++);
            len--;
        }
    }
    font->buffer[font->written] = 0;
    return font->written;
}

#endif
